################################################################################
#
# modem-fhss
#
################################################################################

MFHSSDRV_VERSION = 1.0
MFHSSDRV_SITE = /home/alex-m/projects/modem-fhss/PS/modules/mfhssdrv
MFHSSDRV_SITE_METHOD = local
MFHSSDRV_DEPENDENCIES = linux

define MODEM_FHSS_MOD_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) CC=$(TARGET_CC) -C $(@D) KERNELDIR=$(LINUX_DIR) modules
endef

define MODEM_FHSS_MOD_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) CC=$(TARGET_CC) -C $(@D) KERNELDIR=$(LINUX_DIR) modules_install
endef

$(eval $(kernel-module))
$(eval $(generic-package))
