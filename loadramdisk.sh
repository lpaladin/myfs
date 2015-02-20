#!/bin/bash
mkdir ramdisk
cd ramdisk
mount -t myfs /dev/null .
echo -e "\033[1;33;44mPlease chown ramdisk/ to yourself for best performance.\033[0m"
