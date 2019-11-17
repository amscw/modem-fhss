################################################################################
#
# Configuration network
#
################################################################################

NETWORK_SCRIPTS_VERSION = 1.0
NETWORK_SCRIPTS_SITE = /home/alex-m/projects/modem-fhss/PS/scripts/network-scripts
NETWORK_SCRIPTS_SITE_METHOD = local
NETWORK_SCRIPTS_INSTALL_STAGING = YES
NETWORK_SCRIPTS_INSTALL_TARGET = YES

NETWORK_SCRIPT = S39network
NETWORK_SCRIPT_DST_DIR = /etc/init.d

define NETWORK_SCRIPTS_INSTALL_TARGET_CMDS
	cp -rf $(NETWORK_SCRIPTS_SITE)/$(NETWORK_SCRIPT) $(TARGET_DIR)/$(NETWORK_SCRIPT_DST_DIR)/$(NETWORK_SCRIPT)
	chmod +x $(TARGET_DIR)/$(NETWORK_SCRIPT_DST_DIR)/$(NETWORK_SCRIPT)
endef

$(eval $(generic-package))
