## Current Supported Core Configurations
##
# ====================EMSK 1.0 & EMSK 1.1========================
#    archs38_c0      - arc hs38x4 core 1, Boot Core selected 0b'00
#    archs38_c1      - arc hs38x4 core 1, Boot Core selected 0b'01
#    archs38_c2      - arc hs38x4 core 1, Boot Core selected 0b'10
#    archs38_c3      - arc hs38x4 core 1, Boot Core selected 0b'11
##

##
# CURRENT CORE CONFIGURATION
# Select the core of HSDK
##
CUR_CORE ?= archs38_c0
ONCHIP_IP_LIST ?= designware/iic designware/spi designware/uart designware/gpio \
		designware/sdio