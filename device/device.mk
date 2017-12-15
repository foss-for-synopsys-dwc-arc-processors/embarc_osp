# devices root declaration
DEVICES_ROOT = $(EMBARC_ROOT)/device

DEV_INCDIR += $(DEVICES_ROOT)/inc

# onchip ip rules
ifdef ONCHIP_IP_LIST
	DEV_INCDIR += $(DEVICES_ROOT)/ip/ip_hal/inc
	override ONCHIP_IP_LIST  := $(strip $(ONCHIP_IP_LIST))
	DEV_CSRCDIR += $(foreach ONCHIP_IP, $(ONCHIP_IP_LIST), $(addprefix $(DEVICES_ROOT)/ip/, $(ONCHIP_IP)))
	DEV_INCDIR += $(foreach ONCHIP_IP, $(ONCHIP_IP_LIST), $(addprefix $(DEVICES_ROOT)/ip/, $(ONCHIP_IP)))
endif

# external device rules
ifdef EXT_DEV_LIST
	override EXT_DEV_LIST := $(strip $(EXT_DEV_LIST))
	EXT_DEV_MKS = $(foreach EXT_DEV, $(EXT_DEV_LIST), $(join $(DEVICES_ROOT)/peripheral/$(EXT_DEV)/, $(notdir $(EXT_DEV)).mk))
	include $(EXT_DEV_MKS)
endif


