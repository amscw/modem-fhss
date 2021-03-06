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
RSA_PUB_KEY = ssh_host_rsa_key.pub
RSA_PRIV_KEY = ssh_host_rsa_key
KNOWN_HOSTS = known_hosts

PURE_FTPD_CFG_DST_DIR = /etc
SSHD_CFG_DST_DIR = /etc/ssh
ETH_CFG_DST_DIR = /etc/network
RSA_KEY_DST_DIR = /etc/ssh
KNOWN_HOSTS_DST_DIR = /root/.ssh

define NW_CONFIGS_INSTALL_TARGET_CMDS
	cp -rf $(NW_CONFIGS_SITE)/$(PURE_FTPD_CFG) $(TARGET_DIR)/$(PURE_FTPD_CFG_DST_DIR)/$(PURE_FTPD_CFG)
	cp -rf $(NW_CONFIGS_SITE)/$(SSHD_CFG) $(TARGET_DIR)/$(SSHD_CFG_DST_DIR)/$(SSHD_CFG)
	cp -rf $(NW_CONFIGS_SITE)/$(ETH_CFG) $(TARGET_DIR)/$(ETH_CFG_DST_DIR)/$(ETH_CFG)
	cp -rf $(NW_CONFIGS_SITE)/$(RSA_PUB_KEY) $(TARGET_DIR)/$(RSA_KEY_DST_DIR)/$(RSA_PUB_KEY)
	cp -rf $(NW_CONFIGS_SITE)/$(RSA_PRIV_KEY) $(TARGET_DIR)/$(RSA_KEY_DST_DIR)/$(RSA_PRIV_KEY)
	cp -rf $(NW_CONFIGS_SITE)/$(KNOWN_HOSTS) $(TARGET_DIR)/$(KNOWN_HOSTS_DST_DIR)/$(KNOWN_HOSTS)
	chmod 600 $(TARGET_DIR)/$(RSA_KEY_DST_DIR)/$(RSA_PRIV_KEY)
endef

$(eval $(generic-package))
