## Current Supported Core Configurations
##
# ====================AXS 103===================================
#  archs36
#  archs38
#  archs34
##

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CUR_CORE ?= archs36
ONCHIP_IP_LIST ?= designware/uart designware/gpio