module = pktrace
obj-m += pktrace.o
ccflags-y += 
$(module)-objs += pkttracer.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNELDIR)  M=$(PWD) modules
	if [ ! -z $(TARGET_DIR_MODULE) ]; then mkdir -p $(TARGET_DIR_MODULE) && cp -vf $(module).ko $(TARGET_DIR_MODULE); fi

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
