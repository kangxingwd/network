CONFIG_MODULE_SIG=n

obj-m := test_timer.o
KERNELDIR := /lib/modules/4.15.0-142-generic/build/
PWD := $(shell pwd)

modules: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

