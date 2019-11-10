#
# Makefile for modem-fhss module of tree kernel modules
#

obj-m := modem-fhss.o
PWD := $(shell pwd)

all: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
