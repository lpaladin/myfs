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

static int inode_count;
static int inode_size;

static void printk_ana(){
	printk("myfs inodes count is %d and size is %d .\n",inode_count,inode_size);
}

//自定义在创建inode时的钩子
static int myfs_custom_inode_create_hook(struct inode * inode) {
	inode_count++;
	inode_size+=sizeof(struct inode);
	printk_ana();
	return -ENOSPC;
}

//自定义在删除inode时的钩子
static int myfs_custom_inode_delete_hook(struct inode * inode) {
	inode_count--;
	inode_size-=sizeof(struct inode);
	printk_ana();
	return -ENOSPC;
}

//注册
int myfs_hook_reg_entry(void) {
	printk("hook_reg_entry\n");
	inode_count = 0;
	inode_size = 0;
	myfs_hook_reg_create(myfs_custom_inode_create_hook);
	myfs_hook_reg_delete(myfs_custom_inode_delete_hook);
	return -ENOSPC;
}