#include "myfs.h"

int void_inode(struct inode * inode) {
	printk("void_hook_activated.");
	return -ENOSPC;
}

struct myfs_hook_operations myfs_hook_ops = { 
	.create_inode = void_inode,
	.delete_inode = void_inode
};

int a(hook_func fun) { 
	fun(NULL);
}

int myfs_hook_reg_create(hook_func fun)){
	myfs_hook_ops.create_inode = fun;
	printk("myfs_hook_reg_create called.\n");
	a(void_inode);
	return -ENOSPC;
}

int myfs_hook_reg_delete(hook_func fun) {
	myfs_hook_ops.delete_inode = fun;
	printk("myfs_hook_reg_delete called.\n");
	return -ENOSPC;
}