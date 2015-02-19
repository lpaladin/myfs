#include "myfs.h"

int void_inode(struct inode * inode) {
	return -ENOSPC;
}

static struct myfs_hook_operations myfs_hook_ops = { 
	.create_inode = void_inode,
	.delete_inode = void_inode
};

int myfs_hook_reg_create(int (*fun)(struct inode * inode)){
	myfs_hook_ops.create_inode = fun;
	return -ENOSPC;
}

int myfs_hook_reg_delete(int (*fun)(struct inode * inode)){
	myfs_hook_ops.delete_inode = fun;
	return -ENOSPC;
}