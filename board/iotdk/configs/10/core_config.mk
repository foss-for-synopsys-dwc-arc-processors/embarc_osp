## Current Supported Core Configurations
##
# ====================Huangshan II IC========================
#    arcem9d	 -non-configurable
##

##
# CURRENT CORE CONFIGURATION
# Select core of IoTDK
##
CUR_CORE ?= arcem9d
ONCHIP_IP_LIST		?= dfss/gpio \
			dfss/iic \
			dfss/uart \
			dfss/spi \
			dfss/dmac \
			designware/i2s \
			designware/sdio
			