#!/bin/bash
echo -e "\033[1;33;44m== Test cases for /myfs/ ==\033[0m"
echo -e "\033[1;33;44mPlease run as root.\033[0m"
read -n1 -r -p "Press any key to start..." key
echo
echo -e "\033[1;37;43m-=0=-\033[1;30;42m Remake and load myfs module...\033[0m"
make clean
make
./install.sh
echo -e "\033[1;37;43m-=1=-\033[1;30;42m Create test directory and mount myfs(options:size=4000000(64m),mode=0755)...\033[0m"
mkdir myfstest
mount -t myfs -o size=4000000,mode=0755 /dev/null myfstest/
cd myfstest/

echo -e "\033[1;37;43m-=2=-\033[1;30;42m Make a 10mb file filled with zero with 1024 bytes written each time...\033[0m"
dd if=/dev/zero of=output.dat  bs=1024  count=10240
ls -alh
df -h -t myfs
echo -e "\033[1;33;44mPress any key to proceed to next stage...(Note that fs statistics are listed above)\033[0m"
read -n1 -r -p "" key
echo

echo -e "\033[1;37;43m-=3=-\033[1;30;42m Make a 4mb file filled with random with 4096 bytes written each time...\033[0m"
dd if=/dev/urandom of=random.dat  bs=4096  count=1024
ls -alh
df -h -t myfs
echo -e "\033[1;33;44mPress any key to proceed to next stage...\033[0m"
read -n1 -r -p "" key
echo

echo -e "\033[1;37;43m-=4=-\033[1;30;42m Make directory structure and some files...\033[0m"
mkdir dir1
dd if=/dev/urandom of=dir1/ha.dat  bs=2048  count=512
mkdir dir1/dir2
dd if=/dev/zero of=dir1/dir2/he.dat  bs=4096  count=4096
ls -Ralh
df -h -t myfs
echo -e "\033[1;33;44mPress any key to proceed to next stage...\033[0m"
read -n1 -r -p "" key
echo

echo -e "\033[1;37;43m-=5=-\033[1;30;42m Make a large file...\033[0m"
dd if=/dev/zero of=large.dat  bs=4096  count=8192
ls -alh
df -h -t myfs
echo -e "\033[1;33;44mPress any key to proceed to next stage...\033[0m"
read -n1 -r -p "" key
echo

echo -e "\033[1;37;43m-=6=-\033[1;30;42m Now that only 1mb free space left, let's test fs limit...\033[0m"
echo -e "\033[1;31;47mFile creation will fail due to insufficent space.\033[0m"
dd if=/dev/urandom of=broken.dat  bs=4096  count=8192
ls -alh
df -h -t myfs
echo -e "\033[1;33;44mPress any key to proceed to next stage...\033[0m"
read -n1 -r -p "" key
echo

echo -e "\033[1;37;43m-=7=-\033[1;30;42m Recursively delete dir1...\033[0m"
rm -rf dir1/
ls -alh
df -h -t myfs
echo -e "\033[1;33;44mPress any key to proceed to next stage...\033[0m"
read -n1 -r -p "" key
echo

echo -e "\033[1;37;43mClean up...\033[0m"
rm -rf *
cd ..
umount myfstest
rmdir myfstest
rmmod myfs
echo -e "\033[1;33;44mPress any key to view myfs log or ^C to exit...\033[0m"
read -n1 -r -p "" key
./viewlog.sh
