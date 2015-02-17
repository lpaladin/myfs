#!/bin/bash
insmod ./myfs.ko
lsmod | grep myfs
