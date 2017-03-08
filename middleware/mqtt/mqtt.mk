# dir declaration
MID_MQTT_DIR = $(MIDDLEWARES_ROOT)/mqtt

MID_MQTT_ASMSRCDIR	= $(MID_MQTT_DIR)/MQTTClient/src \
			$(MID_MQTT_DIR)/MQTTClient/src/lwip \
			$(MID_MQTT_DIR)/MQTTPacket/src
MID_MQTT_CSRCDIR	= $(MID_MQTT_DIR)/MQTTClient/src \
			$(MID_MQTT_DIR)/MQTTClient/src/lwip \
			$(MID_MQTT_DIR)/MQTTPacket/src
MID_MQTT_INCDIR		= $(MID_MQTT_DIR)/MQTTClient/src \
			$(MID_MQTT_DIR)/MQTTClient/src/lwip \
			$(MID_MQTT_DIR)/MQTTPacket/src

# find all the source files in the target directories
MID_MQTT_CSRCS = $(call get_csrcs, $(MID_MQTT_CSRCDIR))
MID_MQTT_ASMSRCS = $(call get_asmsrcs, $(MID_MQTT_ASMSRCDIR))

# get object files
MID_MQTT_COBJS = $(call get_relobjs, $(MID_MQTT_CSRCS))
MID_MQTT_ASMOBJS = $(call get_relobjs, $(MID_MQTT_ASMSRCS))
MID_MQTT_OBJS = $(MID_MQTT_COBJS) $(MID_MQTT_ASMOBJS)

# get dependency files
MID_MQTT_DEPS = $(call get_deps, $(MID_MQTT_OBJS))

# extra macros to be defined
MID_MQTT_DEFINES = -DMID_MQTT

# genearte library
MID_LIB_MQTT = $(OUT_DIR)/libmidmqtt.a

# library generation rule
$(MID_LIB_MQTT): $(MID_MQTT_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_MQTT_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_MQTT_INCDIR)
MID_CSRCDIR += $(MID_MQTT_CSRCDIR)
MID_ASMSRCDIR += $(MID_MQTT_ASMSRCDIR)

MID_CSRCS += $(MID_MQTT_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_MQTT_ASMSRCS)
MID_ALLSRCS += $(MID_MQTT_CSRCS) $(MID_MQTT_ASMSRCS)

MID_COBJS += $(MID_MQTT_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_MQTT_ASMOBJS)
MID_ALLOBJS += $(MID_MQTT_OBJS)

MID_DEFINES += $(MID_MQTT_DEFINES)
MID_DEPS += $(MID_MQTT_DEPS)
MID_LIBS += $(MID_LIB_MQTT)