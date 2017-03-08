ifneq ($(VALID_CUR_CORE), arcemfull)
ifneq ($(VALID_CUR_CORE), arcemsecureshield)
$(error $(VALID_BOARD)-$(VALID_BD_VER)-$(VALID_CUR_CORE) is not supported in secureshield!)
else
ifneq ($(LIB_SECURESHIELD_VERSION), 2)
$(error $(VALID_BOARD)-$(VALID_BD_VER)-$(VALID_CUR_CORE) is not supported in secureshield v1!)
endif
endif
else
ifneq ($(LIB_SECURESHIELD_VERSION), 1)
$(error $(VALID_BOARD)-$(VALID_BD_VER)-$(VALID_CUR_CORE) is not supported in secureshield v2!)
endif
endif

LIB_SECURESHIELD_BOARD_NSIM_DIR = $(LIB_SECURESHIELD_DIR)/board/nsim

LIB_SECURESHIELD_ASMSRCDIR += $(LIB_SECURESHIELD_BOARD_NSIM_DIR)
LIB_SECURESHIELD_CSRCDIR += $(LIB_SECURESHIELD_BOARD_NSIM_DIR)
LIB_SECURESHIELD_INCDIR += $(LIB_SECURESHIELD_BOARD_NSIM_DIR)


SECURESHIELD_BOARD_CONFIG_FILE = $(LIB_SECURESHIELD_DIR)/board/nsim/secureshield_board_config.h