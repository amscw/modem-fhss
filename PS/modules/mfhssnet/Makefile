ifneq ($(KERNELRELEASE),)
#kbuild part of makefile
include kbuild

else
#normal makefile
#KDIR ?= /lib/modules/`uname -r`/build # - for running kernel
KDIR ?= $(KERNELDIR) # - for buildroot kernel

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

endif
