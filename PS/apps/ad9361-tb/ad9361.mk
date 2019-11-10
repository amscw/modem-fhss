################################################################################
#
# AD9361
#
################################################################################

AD9361_VERSION = 1.0
AD9361_SITE = /home/alex/projects/modem-fhss/apps/ad9361
AD9361_SITE_METHOD = local
AD9361_INSTALL_STAGING = YES
AD9361_INSTALL_TARGET = YES

AD9361_CONFIG = config.yaml
AD9361_FIR_CONFIG = ad9361_fir.conf
AD9361_CONFIG_SRC_DIR = settings
AD9361_CONFIG_DST_DIR = /etc/ad9361

define AD9361_BUILD_CMDS
	$(MAKE) -f Makefile.linux CC=$(TARGET_CC) -C $(@D) all
endef

define AD9361_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/ad9361 $(TARGET_DIR)/bin
	mkdir -p $(TARGET_DIR)/$(AD9361_CONFIG_DST_DIR)
	cp -rf $(AD9361_SITE)/$(AD9361_CONFIG_SRC_DIR)/$(AD9361_CONFIG) $(TARGET_DIR)/$(AD9361_CONFIG_DST_DIR)/$(AD9361_CONFIG)
	cp -rf $(AD9361_SITE)/$(AD9361_CONFIG_SRC_DIR)/$(AD9361_FIR_CONFIG) $(TARGET_DIR)/$(AD9361_CONFIG_DST_DIR)/$(AD9361_FIR_CONFIG)
endef

$(eval $(generic-package))
