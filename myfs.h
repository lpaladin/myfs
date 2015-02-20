#ifndef MYFS_H_INCLUDED
#define MYFS_H_INCLUDED

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>

// 文件系统标识符
#define MYFS_MAGIC_NUMBER 0x2333

// 文件系统默认大小限制（256MB）
#define MAX_FS_SIZE 0x10000000

// 文件系统默认权限
#define DEFAULT_MODE 0755

// 从sb获得文件系统特有类型的的fs_info
#define MYFS_INFO(sb) ((struct myfs_fs_info*) ((sb)->s_fs_info))

// 文件系统元配置（来自模块参数）
struct myfs_fs_info {
	unsigned long fs_max_size;		// 文件系统总大小限制，默认是MAX_FS_SIZE
	unsigned long file_max_size;	// 文件大小限制，默认是MAX_LFS_FILESIZE
	unsigned long block_size;		// 默认是PAGE_CACHE_SIZE
	umode_t root_mode;				// 文件系统根目录权限位，默认是DEFAULT_MODE
    atomic_long_t used_blocks;		// 文件系统已用页面数
};

extern const struct super_operations myfs_super_ops;

struct inode *myfs_get_inode(struct super_block *sb,
				const struct inode *dir, umode_t mode, dev_t dev);

#endif // MYFS_H_INCLUDED
