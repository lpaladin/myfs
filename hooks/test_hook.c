#include "../myfs.h"

static int test_create_inode_hook(struct inode * inode) { 
	printk("test_create_inode_hook called.\n");
	return -ENOSPC;
}

static int test_delete_inode_hook(struct inode * inode) { 
	printk("test_create_inode_hook called.\n");
	return -ENOSPC;
}

// 最终入口
static int __init InstallFileSystem(void)
{
	myfs_hook_reg_create(test_create_inode_hook);
	myfs_hook_reg_delete(test_delete_inode_hook);

	return -ENOSPC;
}

// 最终出口
static void __exit UninstallFileSystem(void)
{
	printk("myfs_hook: bye\n");
}

module_init(InstallFileSystem);
module_exit(UninstallFileSystem);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("osgroup");
