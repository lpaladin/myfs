#include "myfs.h"

int test_create_inode_hook(struct inode * inode) {
	printk("create_hook_activated.");
	return -ENOSPC;
}

int test_delete_inode_hook(struct inode * inode) {
	printk("delete_hook_activated.");
	return -ENOSPC;
}

myfs_hook_reg_create(test_create_inode_hook);
myfs_hook_reg_delete(test_delete_inode_hook);