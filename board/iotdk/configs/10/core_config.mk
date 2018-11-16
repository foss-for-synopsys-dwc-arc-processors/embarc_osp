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
ONCHIP_IP_LIST		?= subsystem/gpio \
			subsystem/uart \
			subsystem/spi \
			subsystem/iic \
			designware/i2s \
			designware/sdio \
			designware/pwm_timer
# dfss/dmac \
