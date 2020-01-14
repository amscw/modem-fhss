################################################################################
#
# Configuration network applications
#
################################################################################

NW_CONFIGS_VERSION = 1.0
NW_CONFIGS_SITE = /home/alex-m/projects/modem-fhss/PS/configs/nw-configs
NW_CONFIGS_SITE_METHOD = local
NW_CONFIGS_INSTALL_STAGING = YES
NW_CONFIGS_INSTALL_TARGET = YES

PURE_FTPD_CFG = pure-ftpd.conf
SSHD_CFG = sshd_config
ETH_CFG = system.conf

PURE_FTPD_CFG_DST_DIR = /etc
SSHD_CFG_DST_DIR = /etc/ssh
ETH_CFG_DST_DIR = /etc/network

define NW_CONFIGS_INSTALL_TARGET_CMDS
	cp -rf $(NW_CONFIGS_SITE)/$(PURE_FTPD_CFG) $(TARGET_DIR)/$(PURE_FTPD_CFG_DST_DIR)/
	cp -rf $(NW_CONFIGS_SITE)/$(SSHD_CFG) $(TARGET_DIR)/$(SSHD_CFG_DST_DIR)/
	cp -rf $(NW_CONFIGS_SITE)/$(ETH_CFG) $(TARGET_DIR)/$(ETH_CFG_DST_DIR)/
endef

$(eval $(generic-package))
