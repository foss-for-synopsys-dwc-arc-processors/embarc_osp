## Current Supported Core Configurations
##
# ====================HSDK========================
#    archs4x_c0      - arc hs38x4 core 0, Boot Core selected 0b'00
#    archs4x_c1      - arc hs38x4 core 1, Boot Core selected 0b'01
#    archs4x_c2      - arc hs38x4 core 2, Boot Core selected 0b'10
#    archs4x_c3      - arc hs38x4 core 3, Boot Core selected 0b'11
##

##
# CURRENT CORE CONFIGURATION
# Select the core of HSDK
##
CUR_CORE ?= archs4x_c0
ONCHIP_IP_LIST ?= designware/iic designware/spi designware/uart designware/gpio \
		designware/sdio
