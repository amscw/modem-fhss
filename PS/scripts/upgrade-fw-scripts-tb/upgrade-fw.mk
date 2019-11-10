################################################################################
#
# Firmware upgrade
#
################################################################################

UPGRADE_FW_SCRIPTS_VERSION = 1.0
UPGRADE_FW_SCRIPTS_SITE = /home/alex/projects/modem-fhss/scripts/upgrade-fw-scripts
UPGRADE_FW_SCRIPTS_SITE_METHOD = local
UPGRADE_FW_SCRIPTS_INSTALL_STAGING = YES
UPGRADE_FW_SCRIPTS_INSTALL_TARGET = YES

UPGRADE_FW_SCRIPT = upgrade-fw
UPGRADE_FW_SCRIPT_DST_DIR = root

define UPGRADE_FW_SCRIPTS_INSTALL_TARGET_CMDS
	cp -rf $(UPGRADE_FW_SCRIPTS_SITE)/$(UPGRADE_FW_SCRIPT) $(TARGET_DIR)/$(UPGRADE_FW_SCRIPT_DST_DIR)/$(UPGRADE_FW_SCRIPT)
	chmod +x $(TARGET_DIR)/$(UPGRADE_FW_SCRIPT_DST_DIR)/$(UPGRADE_FW_SCRIPT)
endef

$(eval $(generic-package))
