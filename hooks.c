#include "myfs.h"

int void_inode(struct inode * inode) {
	printk("void_hook_activated.");
	return -ENOSPC;
}

struct myfs_hook_operations myfs_hook_ops = { 
	.create_inode = void_inode,
	.delete_inode = void_inode
};

int myfs_hook_reg_create(int (*fun)(struct inode * inode)){
	myfs_hook_ops.create_inode = fun;
	printk("myfs_hook_reg_create called.\n");
	myfs_hook_ops.create_inode(NULL);
	fun(NULL);
	return -ENOSPC;
}

int myfs_hook_reg_delete(int (*fun)(struct inode * inode)){
	myfs_hook_ops.delete_inode = fun;
	printk("myfs_hook_reg_delete called.\n");
	return -ENOSPC;
}