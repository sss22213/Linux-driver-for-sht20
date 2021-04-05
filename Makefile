PWD := $(shell pwd)
KVERSION := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

MODULE_NAME = sht20_i2c_drv
obj-m := $(MODULE_NAME).o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
