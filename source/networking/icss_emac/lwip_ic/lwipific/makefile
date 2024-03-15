#
# This file is the makefile for building LWIPIF library.
#
ifeq ($(RULES_MAKE), )
  include $(PDK_INSTALL_PATH)/ti/build/Rules.make
else
  include $(RULES_MAKE)
endif

MODULE_NAME = lwipific_$(BUILD_OS_TYPE)

# lwipific
SRCDIR = src
INCDIR = inc

ifeq ($(BUILD_OS_TYPE),freertos)
  ENET_CFLAGS += -DFREERTOS
else ifeq ($(BUILD_OS_TYPE),safertos)
  ENET_CFLAGS += -DSAFERTOS
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
ifeq ($(BUILD_OS_TYPE),freertos)
  INCLUDE_EXTERNAL_INTERFACES = pdk osal freertos
else ifeq ($(BUILD_OS_TYPE),safertos)
  INCLUDE_EXTERNAL_INTERFACES = pdk osal safertos
endif

INCLUDE_INTERNAL_INTERFACES =  enet
INCLUDE_INTERNAL_INTERFACES += lwipstack_$(BUILD_OS_TYPE)
INCLUDE_INTERNAL_INTERFACES += lwipcontrib_$(BUILD_OS_TYPE)
INCLUDE_INTERNAL_INTERFACES += lwipport_$(BUILD_OS_TYPE)

CFLAGS_LOCAL_COMMON += $(PDK_CFLAGS) $(ENET_CFLAGS)
CFLAGS_LOCAL_COMMON += -DLWIPIFIC_INSTRUMENTATION_ENABLED

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += lwip2enet_ic.c lwip2lwipif_ic.c netif_ic.c bufpool.c

PACKAGE_SRCS_COMMON = .

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Temporary override for "_SYS_SELECT_H" redefined warning
ifeq ($(CORE), mpu1_0)
  CFLAGS_LOCAL_COMMON += -std=c99
endif

# Include common make files
ifeq ($(MAKERULEDIR), )
  #Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/ti/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
