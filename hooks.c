#include "myfs.h"

int void_inode(struct inode * inode) {
	printk("void_hook_activated.\n");
	return -ENOSPC;
}

struct myfs_hook_operations myfs_hook_ops = { 
	.create_inode = void_inode,
	.delete_inode = void_inode
};

int myfs_hook_reg_create(hook_func fun){
	myfs_hook_ops.create_inode = fun;
	printk("myfs_hook_reg_create called.\n");
	return -ENOSPC;
}
EXPORT_SYMBOL(myfs_hook_reg_create);

int myfs_hook_reg_delete(hook_func fun) {
	myfs_hook_ops.delete_inode = fun;
	printk("myfs_hook_reg_delete called.\n");
	return -ENOSPC;
}
EXPORT_SYMBOL(myfs_hook_reg_delete);



/*
* Modyfy below
*/
static int myfs_hook_reg_entry() {
	printk("hook_reg_entry\n");
}