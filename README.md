## 改进的ramfs - __myfs__
----
### 使用方法：

你可以直接__用root权限__运行 `test.sh` （测试脚本），会自动完成make、安装、挂载、测试、结果展示和最终清理

否则手动步骤如下：

* __使用root权限__ make
* __使用root权限__ 运行 `install.sh`
* __使用root权限__ 运行 `loadramdisk.sh` 或 `mount -t myfs -o <选项> /dev/null <挂载点>`
> 选项是逗号分割的`键=值`列表，比如`mode=0777,size=10000000`
> 支持的选项：
> `mode`（根目录权限位，值是8进制）
> `size`（文件系统大小，值是16进制，单位字节）
> `filemsz`（文件最大大小，值是16进制，单位字节）
> `blksz`（块大小，值是16进制，单位字节，通常不能修改）

* 建议将挂载点所有者设为当前用户，然后便可以在挂载点内测试了
* `df -h -t myfs` 可以看到文件系统的大小、已用空间
* `viewlog.sh` 可以看到myfs的日志

----
### 改进之处：

1. 增加了很多选项，可以自定义更多参数
2. ramfs会无限制地占用内存，myfs增加了设置大小限制的选项
3. 原本的ramfs不支持statfs（df查看不到文件系统的大小和已用空间等信息），myfs增加了该项支持
4. ramfs删除inode或取消挂载时，由于页面和inode是脏的，会有内存泄露，myfs通过强行驱逐脏页减少了内存泄露