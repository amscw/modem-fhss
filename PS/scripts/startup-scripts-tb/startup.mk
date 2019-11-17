################################################################################
#
# Startup
#
################################################################################

STARTUP_SCRIPTS_VERSION = 1.0
STARTUP_SCRIPTS_SITE = /home/alex-m/projects/modem-fhss/PS/scripts/startup-scripts
STARTUP_SCRIPTS_SITE_METHOD = local
STARTUP_SCRIPTS_INSTALL_STAGING = YES
STARTUP_SCRIPTS_INSTALL_TARGET = YES

STARTUP97_SCRIPT = S97startup
STARTUP98_SCRIPT = S98startup
STARTUP99_SCRIPT = S99startup
STARTUP_SCRIPT_DST_DIR = etc/init.d

define STARTUP_SCRIPTS_INSTALL_TARGET_CMDS

	cp -rf $(STARTUP_SCRIPTS_SITE)/$(STARTUP97_SCRIPT) $(TARGET_DIR)/$(STARTUP_SCRIPT_DST_DIR)/$(STARTUP97_SCRIPT)
	cp -rf $(STARTUP_SCRIPTS_SITE)/$(STARTUP98_SCRIPT) $(TARGET_DIR)/$(STARTUP_SCRIPT_DST_DIR)/$(STARTUP98_SCRIPT)
	cp -rf $(STARTUP_SCRIPTS_SITE)/$(STARTUP99_SCRIPT) $(TARGET_DIR)/$(STARTUP_SCRIPT_DST_DIR)/$(STARTUP99_SCRIPT)

	chmod +x $(TARGET_DIR)/$(STARTUP_SCRIPT_DST_DIR)/$(STARTUP97_SCRIPT)
	chmod +x $(TARGET_DIR)/$(STARTUP_SCRIPT_DST_DIR)/$(STARTUP98_SCRIPT)
	chmod +x $(TARGET_DIR)/$(STARTUP_SCRIPT_DST_DIR)/$(STARTUP99_SCRIPT)

endef

$(eval $(generic-package))
