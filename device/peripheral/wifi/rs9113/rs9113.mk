DEV_CSRCDIR +=  $(EMBARC_ROOT)/device/peripheral/wifi/rs9113/common   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/driver/intf/uart   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/driver/intf/spi   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/driver   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/hal   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/os/free_rtos   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/os   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/wlan   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113

DEV_INCDIR +=   $(EMBARC_ROOT)/device/peripheral/wifi/rs9113/include   \
				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113


DEV_DEFINES += -DWIFI_RS9113 -DRSI_SPI_INTERFACE -DRSI_SAMPLE_HAL -DRSI_WAC_MFI_ENABLE -DRSI_ENABLE_DEBUG_PRINT -DRSI_WITH_OS -DRSI_WLAN_ENABLE

# below is dir with rs9113/nwk
# DEV_CSRCDIR +=  $(EMBARC_ROOT)/device/peripheral/wifi/rs9113/common   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/driver/intf/uart   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/driver/intf/spi   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/driver   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/hal   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/applications/mqtt_client/src/MQTTPacket/src   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/applications/mqtt_client/src   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/applications/http_server   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/applications   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/socket   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/os/free_rtos   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/os   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/wlan   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113

# DEV_INCDIR +=  $(EMBARC_ROOT)/device/peripheral/wifi/rs9113/include   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/applications/mqtt_client/src/MQTTPacket/src   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/applications/mqtt_client/src   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/applications/http_server   \
# 				$(EMBARC_ROOT)/device/peripheral/wifi/rs9113/nwk/applications   \