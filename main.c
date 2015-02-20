// 以ramfs为模板但与ramfs实现方式有较大不同的内存（无设备）文件系统

#include "myfs.h"

#include <linux/parser.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/backing-dev.h>
#include <linux/slab.h>

// 支持的参数类型
enum {
	Opt_mode,
	Opt_fsmsize,
	Opt_filemsize,
	Opt_blksize,
	Opt_err
};

// 参数的格式
static const match_table_t tokens = {
	{Opt_mode, "mode=%o"},
	{Opt_fsmsize, "size=%x"},
	{Opt_filemsize, "filemsz=%x"},
	{Opt_blksize, "blksz=%x"},
	{Opt_err, NULL}
};

// 解析参数
static int myfs_parse_options(char *data, struct myfs_fs_info *fsinfo)
{
	substring_t args[MAX_OPT_ARGS];
	int option;
	int token;
	char *p;

	fsinfo->root_mode = DEFAULT_MODE;
	fsinfo->file_max_size = MAX_LFS_FILESIZE;
	fsinfo->fs_max_size = MAX_FS_SIZE;
	fsinfo->block_size = PAGE_CACHE_SIZE;

	while ((p = strsep(&data, ",")) != NULL) {
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_mode:
			if (match_octal(&args[0], &option))
				return -EINVAL;
			fsinfo->root_mode = option & S_IALLUGO;
			break;
		case Opt_blksize:
			if (match_hex(&args[0], &option))
				return -EINVAL;
			fsinfo->block_size = option;
			break;
		case Opt_filemsize:
			if (match_hex(&args[0], &option))
				return -EINVAL;
			fsinfo->file_max_size = option;
			break;
		case Opt_fsmsize:
			if (match_hex(&args[0], &option))
				return -EINVAL;
			fsinfo->fs_max_size = option;
			break;
		}
	}

	return 0;
}

// 填充超级块入口
static int myfs_fill_sb(struct super_block *sb, void *data, int silent)
{
	struct inode *root = NULL;
	struct myfs_fs_info *fsi;
	int err;
	int logval;

	sb->s_magic = MYFS_MAGIC_NUMBER; // 文件系统标识符
	sb->s_op = &myfs_super_ops; // 超级块上的操作
	sb->s_fs_info = fsi = kzalloc(sizeof(struct myfs_fs_info), GFP_KERNEL);
	if (!fsi)
	{
		printk("myfs: fsinfo allocation failed\n");
		return -ENOMEM;
	}

	err = myfs_parse_options(data, fsi);
	printk("myfs: options - blksz=%lu, filemsz=%lu, mode=%o, fssize=%lu!\n",
		fsi->block_size, fsi->file_max_size, fsi->root_mode, fsi->fs_max_size);
	if (err)
		return err;

	sb->s_maxbytes		= fsi->file_max_size;
	sb->s_blocksize		= fsi->block_size;
	for (logval = 0; fsi->block_size > 1; logval++, fsi->block_size >>= 1);
	fsi->block_size = sb->s_blocksize;
	sb->s_blocksize_bits	= logval;
	sb->s_time_gran		= 1;

	root = myfs_get_inode(sb, NULL, S_IFDIR | fsi->root_mode, 0);

	sb->s_root = d_make_root(root);
	if (!sb->s_root)
	{
		printk("myfs: root creation failed\n");
		return -ENOMEM;
	}

	return 0;
}

// 装载文件系统入口
static struct dentry *myfs_mount(struct file_system_type *type, int flags, char const *dev_name, void *data)
{
	  struct dentry *const entry = mount_nodev(type, flags, data, myfs_fill_sb);
	  if (IS_ERR(entry))
		  printk("myfs: myfs mount failed\n");
	  else
		  printk("myfs: myfs mounted\n");
	  return entry;
}


// 这是个Hack，用于从内核中找出私有函数的地址
int (*invalidate_inodes) (struct super_block *, bool) = (int (*)(struct super_block *, bool)) INVALIDATE_NODES_ADDR;

// 回收超级块
static void myfs_kill_sb(struct super_block *sb)
{
	kfree(sb->s_fs_info);
	if (invalidate_inodes(sb, 1)) // 强制毁灭所有inode及其dirty页
		printk("myfs: Eliminate all inodes finished partially!\n");
	kill_litter_super(sb);
}

// 文件系统定义
static struct file_system_type myfs_type = {
	.owner = THIS_MODULE,
	.name = "myfs",
	.mount = myfs_mount,
	.kill_sb = myfs_kill_sb
};

MODULE_ALIAS_FS("myfs");

// 最终入口
static int __init InstallFileSystem(void)
{
	int err;
	printk("myfs: Here comes the /myfs/!\n");

	myfs_hook_reg_entry();
	
	err = register_filesystem(&myfs_type);
	if (err)
	{
		printk("myfs: fs registration failed\n");
		return err;
	}
	return 0;
}

// 最终出口
static void __exit UninstallFileSystem(void)
{
	unregister_filesystem(&myfs_type);
	printk("myfs: bye\n");
}

module_init(InstallFileSystem);
module_exit(UninstallFileSystem);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("osgroup");
