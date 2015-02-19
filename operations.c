#include "myfs.h"
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/backing-dev.h>
#include <linux/ramfs.h>
#include <linux/mm.h>
#include <linux/statfs.h>

// ---直接照搬过来的部分---（起始）

/*
 * For address_spaces which do not use buffers nor write back.
 */
int myset_page_dirty_no_writeback(struct page *page)
{
	if (!PageDirty(page))
		return !TestSetPageDirty(page);
	return 0;
}

// 页（块）写入开始并检查剩余空间
int myfs_write_begin(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned flags,
			struct page **pagep, void **fsdata)
{
	struct super_block *sb = mapping->host->i_sb;
	struct page *page;
	pgoff_t index;

	if (MYFS_INFO(sb)->used_blocks >= MYFS_INFO(sb)->fs_max_size / MYFS_INFO(sb)->block_size)
	{
		printk("myfs: write_begin[%pD] - insufficent space\n", file);
		return -ENOMEM;
	}

	index = pos >> PAGE_CACHE_SHIFT;

	page = grab_cache_page_write_begin(mapping, index, flags);
	if (!page)
		return -ENOMEM;

	*pagep = page;

	if (!PageUptodate(page) && (len != PAGE_CACHE_SIZE)) {
		unsigned from = pos & (PAGE_CACHE_SIZE - 1);

		zero_user_segments(page, 0, from, from + len, PAGE_CACHE_SIZE);
	}
	return 0;
}

// 页（块）写入结束并设置Dirty
int myfs_write_end(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	struct inode *inode = page->mapping->host;
	loff_t last_pos = pos + copied;

	/* zero the stale part of the page if we did a short copy */
	if (copied < len) {
		unsigned from = pos & (PAGE_CACHE_SIZE - 1);

		zero_user(page, from + copied, len - copied);
	}

	if (!PageUptodate(page))
		SetPageUptodate(page);
	/*
	 * No need to use i_size_read() here, the i_size
	 * cannot change under us because we hold the i_mutex.
	 */
	if (last_pos > inode->i_size)
		i_size_write(inode, last_pos);

	if (set_page_dirty(page))
	{
		// 如果是第一次设为Dirty，则修改文件系统的已用块数
		struct super_block *sb = inode->i_sb;
		MYFS_INFO(sb)->used_blocks++;
		printk("myfs: write_end[%pD] - set to dirty\n", file);
	}
	unlock_page(page);
	page_cache_release(page);

	return copied;
}

static const struct address_space_operations myfs_aops = {
	.readpage	= simple_readpage,
	.write_begin	= myfs_write_begin,
	.write_end	= myfs_write_end,
	.set_page_dirty	= myset_page_dirty_no_writeback,
};

/*
 * File creation. Allocate an inode, and we're done..
 */
/* SMP-safe */
static int
ramfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
	struct inode * inode = myfs_get_inode(dir->i_sb, dir, mode, dev);
	int error = -ENOSPC;

	if (inode) {
		d_instantiate(dentry, inode);
		dget(dentry);	/* Extra count - pin the dentry in core */
		error = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;
	}
	return error;
}

static int ramfs_mkdir(struct inode * dir, struct dentry * dentry, umode_t mode)
{
	int retval = ramfs_mknod(dir, dentry, mode | S_IFDIR, 0);
	if (!retval)
		inc_nlink(dir);
	return retval;
}

static int ramfs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl)
{
	return ramfs_mknod(dir, dentry, mode | S_IFREG, 0);
}

static int ramfs_symlink(struct inode * dir, struct dentry *dentry, const char * symname)
{
	struct inode *inode;
	int error = -ENOSPC;

	inode = myfs_get_inode(dir->i_sb, dir, S_IFLNK|S_IRWXUGO, 0);
	if (inode) {
		int l = strlen(symname)+1;
		error = page_symlink(inode, symname, l);
		if (!error) {
			d_instantiate(dentry, inode);
			dget(dentry);
			dir->i_mtime = dir->i_ctime = CURRENT_TIME;
		} else
			iput(inode);
	}
	return error;
}

static int myfs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	struct super_block *sb = dentry->d_sb;
	buf->f_type = sb->s_magic;
	buf->f_bsize = sb->s_blocksize;
	buf->f_namelen = NAME_MAX;
	buf->f_blocks = MYFS_INFO(sb)->fs_max_size / sb->s_blocksize;
	buf->f_bavail = buf->f_bfree = buf->f_blocks - MYFS_INFO(sb)->used_blocks;
	if (buf->f_bavail < 0)
		buf->f_bavail = buf->f_bfree = 0;
	printk("myfs: statfs - maxblks = %llu, freeblks = %llu\n", buf->f_blocks, buf->f_bfree);
	return 0;
}

int myfs_unlink(struct inode *dir, struct dentry *dentry)
{
	struct inode *inode = dentry->d_inode;

	inode->i_ctime = dir->i_ctime = dir->i_mtime = CURRENT_TIME;
	drop_nlink(inode);
	dput(dentry);
	return 0;
}

const struct super_operations myfs_super_ops = {
	.statfs		= myfs_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

static const struct file_operations myfs_file_operations = {
	.read		= do_sync_read,
	.aio_read	= generic_file_aio_read,
	.write		= do_sync_write,
	.aio_write	= generic_file_aio_write,
	.mmap		= generic_file_mmap,
	.fsync		= noop_fsync,
	.splice_read	= generic_file_splice_read,
	.splice_write	= generic_file_splice_write,
	.llseek		= generic_file_llseek,
};

static const struct inode_operations myfs_file_inode_operations = {
	.setattr	= simple_setattr,
	.getattr	= simple_getattr,
};

static const struct inode_operations myfs_dir_inode_operations = {
	.create		= ramfs_create,
	.lookup		= simple_lookup,
	.link		= simple_link,
	.unlink		= myfs_unlink,
	.symlink	= ramfs_symlink,
	.mkdir		= ramfs_mkdir,
	.rmdir		= simple_rmdir,
	.mknod		= ramfs_mknod,
	.rename		= simple_rename,
};

struct inode *myfs_get_inode(struct super_block *sb,
				const struct inode *dir, umode_t mode, dev_t dev)
{
	struct inode * inode = new_inode(sb);

	if (inode) {
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);
		inode->i_mapping->a_ops = &myfs_aops;
		mapping_set_gfp_mask(inode->i_mapping, GFP_HIGHUSER);
		mapping_set_unevictable(inode->i_mapping);
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		switch (mode & S_IFMT) {
		default:
			init_special_inode(inode, mode, dev);
			break;
		case S_IFREG:
			inode->i_op = &myfs_file_inode_operations;
			inode->i_fop = &myfs_file_operations;
			break;
		case S_IFDIR:
			inode->i_op = &myfs_dir_inode_operations;
			inode->i_fop = &simple_dir_operations;

			/* directory inodes start off with i_nlink == 2 (for "." entry) */
			inc_nlink(inode);
			break;
		case S_IFLNK:
			inode->i_op = &page_symlink_inode_operations;
			break;
		}
	}
	return inode;
}

// ---直接照搬过来的部分---（终止）
