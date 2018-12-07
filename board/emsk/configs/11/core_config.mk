## Current Supported Core Configurations
##
# ====================EMSK 1.0 & EMSK 1.1========================
#    arcem4      -configuration 0 (switch 1 up,   switch 2 up):
#    arcem4cr16  -configuration 1 (switch 1 down, switch 2 up):
#    arcem6      -configuration 2 (switch 1 up,   switch 2 down):
#    arcem6gp    -configuration 3 (switch 1 down, switch 2 down):
##

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CUR_CORE ?= arcem6
ONCHIP_IP_LIST ?= designware/iic designware/spi designware/uart designware/gpio