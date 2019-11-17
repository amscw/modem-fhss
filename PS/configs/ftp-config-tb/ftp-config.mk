################################################################################
#
# Configuration pure-ftpd
#
################################################################################

FTP_CONFIG_VERSION = 1.0
FTP_CONFIG_SITE = /home/alex-m/projects/modem-fhss/PS/configs/ftp-config
FTP_CONFIG_SITE_METHOD = local
FTP_CONFIG_INSTALL_STAGING = NO
FTP_CONFIG_INSTALL_TARGET = YES

FTP_CONFIG_FILE = pure-ftpd.conf
FTP_CONFIG_DST_DIR = etc

define FTP_CONFIG_INSTALL_TARGET_CMDS
	cp -rf $(FTP_CONFIG_SITE)/$(FTP_CONFIG_FILE) $(TARGET_DIR)/$(FTP_CONFIG_DST_DIR)/$(FTP_CONFIG_FILE)
endef

$(eval $(generic-package))
