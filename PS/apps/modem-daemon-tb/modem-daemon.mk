MODEM_DAEMON_VERSION = 1.0
MODEM_DAEMON_SITE = /home/alex-m/projects/modem-fhss/PS/apps/modem-daemon
MODEM_DAEMON_SITE_METHOD = local
MODEM_DAEMON_INSTALL_STAGING = NO
MODEM_DAEMON_INSTALL_TARGET = YES

$(eval $(cmake-package))
