## Current Supported Core Configurations
##
# ====================EMDK 1.0===================================
#  em6_scss     (em6d with ...)		-configuration 0 (switch ...)
#  em11d_dfss    (em11d with ...)	-configuration 2 (switch ...)
##

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CORE ?= em7d_esp_dfss

ONCHIP_IP_LIST ?= . designware/spi designware/uart designware/gpio designware/sdio subsystem/spi subsystem/gpio