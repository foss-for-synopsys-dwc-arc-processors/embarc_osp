##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CORE ?= em7d_esp_dfss

ONCHIP_IP_LIST ?= . designware/spi designware/uart designware/gpio designware/sdio designware/trng designware/pwm_timer subsystem/spi subsystem/gpio subsystem/uart subsystem/iic subsystem/i2s