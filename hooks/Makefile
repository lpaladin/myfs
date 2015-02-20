obj-m := test_hooks.o

all:
	@echo -e "\033[1;33;44mPlease notice that this module need root access to compile.\033[0m"
	@echo -e "\033[1;31;47mWhy? Because it used a hack reading the address of a function defined in kernel internally and hardcoring it into code.\033[0m"
	@echo invalidate_nodes is defined at 0x$(INVALIDATE_NODES_ADDR)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
