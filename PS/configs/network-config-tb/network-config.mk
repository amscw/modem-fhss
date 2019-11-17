################################################################################
#
# Configuration network
#
################################################################################

NETWORK_CONFIG_VERSION = 1.0
NETWORK_CONFIG_SITE = /home/alex-m/projects/modem-fhss/PS/configs/network-config
NETWORK_CONFIG_SITE_METHOD = local
NETWORK_CONFIG_INSTALL_STAGING = YES
NETWORK_CONFIG_INSTALL_TARGET = YES

NETWORK_CONFIG_FILE = system.conf
NETWORK_CONFIG_DST_DIR = etc/network

define NETWORK_CONFIG_INSTALL_TARGET_CMDS
	cp -rf $(NETWORK_CONFIG_SITE)/$(NETWORK_CONFIG_FILE) $(TARGET_DIR)/$(NETWORK_CONFIG_DST_DIR)/$(NETWORK_CONFIG_FILE)
endef

$(eval $(generic-package))
