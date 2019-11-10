################################################################################
#
# modem-capture
#
################################################################################

MODEM_CAPTURE_MOD_VERSION = 1.0
MODEM_CAPTURE_MOD_SITE = /home/alex/projects/modem-fhss/modules/modem-capture-mod
MODEM_CAPTURE_MOD_SITE_METHOD = local
MODEM_CAPTURE_MOD_DEPENDENCIES = linux

define MODEM_CAPTURE_MOD_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) CC=$(TARGET_CC) -C $(@D) KERNELDIR=$(LINUX_DIR) modules
endef

define MODEM_CAPTURE_MOD_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) CC=$(TARGET_CC) -C $(@D) KERNELDIR=$(LINUX_DIR) modules_install
endef

$(eval $(kernel-module))
$(eval $(generic-package))
