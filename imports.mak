
DEVICE ?= am64x

ifeq ($(OS),Windows_NT)
    TOOLS_PATH?=C:/ti
    CCS_PATH?=$(TOOLS_PATH)/ccs1210/ccs
    CCS_ECLIPSE=$(CCS_PATH)/eclipse/eclipsec
    CYGWIN_PATH?=$(CCS_PATH)/utils/cygwin
    MKDIR=$(CYGWIN_PATH)/mkdir -p
    RMDIR=$(CYGWIN_PATH)/rm -rf
    RM=$(CYGWIN_PATH)/rm -f
    COPY=$(CYGWIN_PATH)/cp
    TOUCH=$(CYGWIN_PATH)/touch
    PATHSEP=\\
    CHMOD=$(CYGWIN_PATH)/echo
    SHELL=cmd.exe
    CGT_GCC_AARCH64_PATH=$(TOOLS_PATH)/gcc-arm-9.2-2019.12-mingw-w64-i686-aarch64-none-elf
    CGT_GCC_ARM_PATH=$(TOOLS_PATH)/gcc-arm-none-eabi-7-2017-q4-major-win32
    CGT_GCC_ARMV7_PATH=$(CGT_GCC_ARM_PATH)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        export TOOLS_PATH?=$(HOME)/ti
        export CCS_PATH?=$(TOOLS_PATH)/ccs1210/ccs
        export CCS_ECLIPSE=$(CCS_PATH)/eclipse/eclipse
        export MKDIR=mkdir -p
        export RMDIR=rm -rf
        export RM=rm -f
        export COPY=cp
        export TOUCH=touch
        export PATHSEP=/
        export CHMOD=chmod
        CGT_GCC_AARCH64_PATH=$(TOOLS_PATH)/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf
        CGT_GCC_ARM_PATH=$(TOOLS_PATH)/gcc-arm-none-eabi-7-2017-q4-major
        CGT_GCC_ARMV7_PATH=$(CGT_GCC_ARM_PATH)
    endif
endif

# Variable pointing to location of MCU+ SDK
MCU_PLUS_SDK_PATH?=$(TOOLS_PATH)/mcu_plus_sdk

CGT_TI_PRU_PATH=$(TOOLS_PATH)/ti-cgt-pru_2.3.3
CGT_TI_SMART_PLACEMENT_TOOL_PATH=$(TOOLS_PATH)/ti-cgt-armllvm_3.0.0.STS
CGT_TI_ARM_CLANG_PATH=$(CCS_PATH)/tools/compiler/ti-cgt-armllvm_2.1.2.LTS
ifeq ($(wildcard $(CGT_TI_ARM_CLANG_PATH)),)
    CGT_TI_ARM_CLANG_PATH=$(TOOLS_PATH)/ti-cgt-armllvm_2.1.2.LTS
endif
CGT_TI_C6000_PATH=$(CCS_PATH)/tools/compiler/ti-cgt-c6000_8.3.12
DSPLIB_PATH=$(MCU_PLUS_SDK_PATH)/source/dsplib_c66x_3_4_0_0
CCS_NODE = $(CCS_PATH)/tools/node/node
SYSCFG_PATH ?= $(TOOLS_PATH)/sysconfig_1.14.0
SYSCFG_CLI_PATH ?= $(SYSCFG_PATH)
SYSCFG_NODE = $(SYSCFG_PATH)/nodejs/node
SYSCFG_NWJS = $(SYSCFG_PATH)/nw/nw
SYSCFG_SDKPRODUCT=$(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/.metadata/product.json
SYSCFG_MCUPLUSSDKPRODUCT=$(MCU_PLUS_SDK_PATH)/.metadata/product.json

# Variable pointing to location of prebuilt industrial communication libraries
IND_COMMS_LIBS_PATH ?= $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/source/commercial/industrial_comms_libs
