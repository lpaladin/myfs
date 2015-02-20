#!/bin/bash
insmod ./test_hook.ko
lsmod | grep test_hook
