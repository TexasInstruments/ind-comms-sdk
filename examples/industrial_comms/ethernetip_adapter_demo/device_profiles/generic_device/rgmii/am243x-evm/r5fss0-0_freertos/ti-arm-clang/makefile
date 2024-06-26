#
# Auto generated makefile
#

export INDUSTRIAL_COMMUNICATIONS_SDK_PATH?=$(abspath ../../../../../../../../..)
include $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/imports.mak
include $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/devconfig/devconfig.mak

CG_TOOL_ROOT=$(CGT_TI_ARM_CLANG_PATH)

CC=$(CG_TOOL_ROOT)/bin/tiarmclang
LNK=$(CG_TOOL_ROOT)/bin/tiarmclang
STRIP=$(CG_TOOL_ROOT)/bin/tiarmstrip
OBJCOPY=$(CG_TOOL_ROOT)/bin/tiarmobjcopy
ifeq ($(OS), Windows_NT)
	PYTHON=python
else
	PYTHON=python3
endif

PROFILE?=release
ConfigName:=$(PROFILE)

OUTNAME:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).out

BOOTIMAGE_PATH=$(abspath .)
BOOTIMAGE_NAME:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).appimage
BOOTIMAGE_NAME_XIP:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).appimage_xip
BOOTIMAGE_NAME_SIGNED:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).appimage.signed
BOOTIMAGE_RPRC_NAME:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).rprc
BOOTIMAGE_RPRC_NAME_XIP:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).rprc_xip
BOOTIMAGE_RPRC_NAME_TMP:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).rprc_tmp
BOOTIMAGE_NAME_HS:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).appimage.hs
BOOTIMAGE_NAME_HS_FS:=ethernetip_adapter_generic_device_rgmii_demo.$(PROFILE).appimage.hs_fs
TARGETS := $(BOOTIMAGE_NAME)
ifeq ($(DEVICE_TYPE), HS)
   TARGETS += $(BOOTIMAGE_NAME_HS)
endif

FILES_common := \
	app.c \
	appCfg.c \
	appNV.c \
	appRst.c \
	appTask.c \
	appWebServer.c \
	CUST_drivers.c \
	CUST_eeprom.c \
	CUST_ethPhy.c \
	CUST_flash.c \
	CUST_led.c \
	CUST_pruIcss.c \
	CUST_uart.c \
	CUST_PHY_base.c \
	CUST_PHY_dp83869.c \
	CMN_CPU_main.c \
	CMN_app.c \
	CMN_board.c \
	CMN_mem.c \
	CMN_os.c \
	app_device_profile.c \
	app_generic_device.c \
	app_generic_device_cfg.c \
	ti_drivers_config.c \
	ti_drivers_open_close.c \
	ti_board_config.c \
	ti_board_open_close.c \
	ti_dpl_config.c \
	ti_pinmux_config.c \
	ti_power_clock_config.c \

FILES_PATH_common = \
	../../../../../.. \
	../../../../../../board/am243x-evm/freertos/drivers \
	../../../../../../board/am243x-evm/freertos/drivers/eeprom \
	../../../../../../board/am243x-evm/freertos/drivers/ethphy \
	../../../../../../board/am243x-evm/freertos/drivers/flash \
	../../../../../../board/am243x-evm/freertos/drivers/led \
	../../../../../../board/am243x-evm/freertos/drivers/pru_icss \
	../../../../../../board/am243x-evm/freertos/drivers/uart \
	../../../../../../device_profiles \
	../../../../../../device_profiles/generic_device \
	../../../../../../../custom_phy/src \
	../../../../../../os/freertos \
	generated \

INCLUDES_common := \
	-I${CG_TOOL_ROOT}/include/c \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am243x/r5f \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/board/am243x-evm/freertos \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/custom_phy/inc \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/device_profiles \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/device_profiles/generic_device \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/os/freertos \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/inc \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/lwip/lwip-config \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-stack/src/include \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/include \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/freertos/include \
	-Igenerated \

DEFINES_common := \
	-DSOC_AM243X \
	-DSOC_AM243X=1 \
	-DOSAL_FREERTOS=1 \
	-DEIP_TIME_SYNC=1 \
	-DCPU_LOAD_MONITOR=0 \

CFLAGS_common := \
	-mcpu=cortex-r5 \
	-mfloat-abi=hard \
	-mfpu=vfpv3-d16 \
	-mthumb \
	-Wall \
	-Werror \
	-g \
	-Wno-gnu-variable-sized-type-not-at-end \
	-Wno-unused-function \
	-Wno-unused-but-set-variable \
	-mllvm \
	-align-all-functions=2 \

CFLAGS_debug := \
	-D_DEBUG_=1 \
	-Og \

CFLAGS_cpp_common := \
	-Wno-c99-designator \
	-Wno-extern-c-compat \
	-Wno-c++11-narrowing \
	-Wno-reorder-init-list \
	-Wno-deprecated-register \
	-Wno-writable-strings \
	-Wno-enum-compare \
	-Wno-reserved-user-defined-literal \
	-Wno-unused-const-variable \
	-x c++ \

CFLAGS_release := \
	-Os \

LNK_FILES_common = \
	linker.cmd \

LIBS_PATH_common = \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_timesync/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/icss_fwhal/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/lwip/lib \
	-Wl,-i${CG_TOOL_ROOT}/lib \

LIBS_common = \
	-lfreertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-ldrivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lboard.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-licss_emac.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-licss_timesync.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lethernetip_adapter_rgmii_icss_fwhal.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lethernetip_adapter_lwip_contrib.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lethernetip_adapter_lwip_freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lethernetipadapter_rgmii.am243x-evm.r5f.ti-arm-clang.release.lib \
	-llibc.a \
	-llibsysbm.a \

LFLAGS_common = \
	-Wl,--diag_suppress=10063 \
	-Wl,--ram_model \
	-Wl,--reread_libs \
	-Wl,--use_memcpy=fast \
	-Wl,--use_memset=fast \


LIBS_NAME = \
	freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	drivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	board.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	icss_emac.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	icss_timesync.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	ethernetip_adapter_rgmii_icss_fwhal.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	ethernetip_adapter_lwip_contrib.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	ethernetip_adapter_lwip_freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	ethernetipadapter_rgmii.am243x-evm.r5f.ti-arm-clang.release.lib \
	libc.a \
	libsysbm.a \

LIBS_PATH_NAME = \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_timesync/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/icss_fwhal/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/lwip/lib \
	${CG_TOOL_ROOT}/lib \

FILES := $(FILES_common) $(FILES_$(PROFILE))
ASMFILES := $(ASMFILES_common) $(ASMFILES_$(PROFILE))
FILES_PATH := $(FILES_PATH_common) $(FILES_PATH_$(PROFILE))
CFLAGS := $(CFLAGS_common) $(CFLAGS_$(PROFILE))
DEFINES := $(DEFINES_common) $(DEFINES_$(PROFILE))
INCLUDES := $(INCLUDES_common) $(INCLUDE_$(PROFILE))
LIBS := $(LIBS_common) $(LIBS_$(PROFILE))
LIBS_PATH := $(LIBS_PATH_common) $(LIBS_PATH_$(PROFILE))
LFLAGS := $(LFLAGS_common) $(LFLAGS_$(PROFILE))
LNKOPTFLAGS := $(LNKOPTFLAGS_common) $(LNKOPTFLAGS_$(PROFILE))
LNK_FILES := $(LNK_FILES_common) $(LNK_FILES_$(PROFILE))

OBJDIR := obj/$(PROFILE)/
OBJS := $(FILES:%.c=%.obj)
OBJS += $(ASMFILES:%.S=%.obj)
DEPS := $(FILES:%.c=%.d)

vpath %.obj $(OBJDIR)
vpath %.c $(FILES_PATH)
vpath %.S $(FILES_PATH)
vpath %.lib $(LIBS_PATH_NAME)
vpath %.a $(LIBS_PATH_NAME)

$(OBJDIR)/%.obj %.obj: %.c
	@echo  Compiling: am243x:r5fss0-0:freertos:ti-arm-clang $(OUTNAME): $<
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) -MMD -o $(OBJDIR)/$@ $<

$(OBJDIR)/%.obj %.obj: %.S
	@echo  Compiling: am243x:r5fss0-0:freertos:ti-arm-clang $(LIBNAME): $<
	$(CC) -c $(CFLAGS) -o $(OBJDIR)/$@ $<

all: $(TARGETS)

SYSCFG_GEN_FILES=generated/ti_drivers_config.c generated/ti_drivers_config.h
SYSCFG_GEN_FILES+=generated/ti_drivers_open_close.c generated/ti_drivers_open_close.h
SYSCFG_GEN_FILES+=generated/ti_dpl_config.c generated/ti_dpl_config.h
SYSCFG_GEN_FILES+=generated/ti_pinmux_config.c generated/ti_power_clock_config.c
SYSCFG_GEN_FILES+=generated/ti_board_config.c generated/ti_board_config.h
SYSCFG_GEN_FILES+=generated/ti_board_open_close.c generated/ti_board_open_close.h

$(OUTNAME): syscfg $(SYSCFG_GEN_FILES) $(OBJS) $(LNK_FILES) $(LIBS_NAME)
	@echo  .
	@echo  Linking: am243x:r5fss0-0:freertos:ti-arm-clang $@ ...
	$(LNK) $(LNKOPTFLAGS) $(LFLAGS) $(LIBS_PATH) -Wl,-m=$(basename $@).map -o $@ $(addprefix $(OBJDIR), $(OBJS)) $(LIBS) $(LNK_FILES)
	@echo  Linking: am243x:r5fss0-0:freertos:ti-arm-clang $@ Done !!!
	@echo  .

clean:
	@echo  Cleaning: am243x:r5fss0-0:freertos:ti-arm-clang $(OUTNAME) ...
	$(RMDIR) $(OBJDIR)
	$(RM) $(OUTNAME)
	$(RM) $(BOOTIMAGE_NAME)
	$(RM) $(BOOTIMAGE_NAME_XIP)
	$(RM) $(BOOTIMAGE_NAME_SIGNED)
	$(RM) $(BOOTIMAGE_NAME_HS)
	$(RM) $(BOOTIMAGE_NAME_HS_FS)
	$(RM) $(BOOTIMAGE_RPRC_NAME)
	$(RM) $(BOOTIMAGE_RPRC_NAME_XIP)
	$(RMDIR) generated/

scrub:
	@echo  Scrubing: am243x:r5fss0-0:freertos:ti-arm-clang ethernetip_adapter_generic_device_rgmii_demo ...
	$(RMDIR) obj
ifeq ($(OS),Windows_NT)
	$(RM) \*.out
	$(RM) \*.map
	$(RM) \*.appimage*
	$(RM) \*.rprc*
	$(RM) \*.tiimage*
	$(RM) \*.bin
else
	$(RM) *.out
	$(RM) *.map
	$(RM) *.appimage*
	$(RM) *.rprc*
	$(RM) *.tiimage*
	$(RM) *.bin
endif
	$(RMDIR) generated

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	$(MKDIR) $@


.NOTPARALLEL:

.INTERMEDIATE: syscfg
$(SYSCFG_GEN_FILES): syscfg

syscfg: ../example.syscfg
	@echo Generating SysConfig files ...
	$(SYSCFG_NODE) $(SYSCFG_CLI_PATH)/dist/cli.js --product $(SYSCFG_SDKPRODUCT) --context r5fss0-0 --part ALV --package ALV --output generated/ ../example.syscfg

syscfg-gui:
	$(SYSCFG_NWJS) $(SYSCFG_PATH) --product $(SYSCFG_SDKPRODUCT) --device AM243x_ALV_beta --context r5fss0-0 --part ALV --package ALV --output generated/  ../example.syscfg

#
# Generation of boot image which can be loaded by Secondary Boot Loader (SBL)
#
ifeq ($(OS),Windows_NT)
EXE_EXT=.exe
endif
ifeq ($(OS),Windows_NT)
  BOOTIMAGE_CERT_GEN_CMD=powershell -executionpolicy unrestricted -command $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/tools/boot/signing/x509CertificateGen.ps1
else
  BOOTIMAGE_CERT_GEN_CMD=$(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/tools/boot/signing/x509CertificateGen.sh
endif
BOOTIMAGE_TEMP_OUT_FILE=temp_stdout_$(PROFILE).txt

BOOTIMAGE_CERT_KEY=$(APP_SIGNING_KEY)

BOOTIMAGE_CORE_ID_r5fss0-0 = 4
BOOTIMAGE_CORE_ID_r5fss0-1 = 5
BOOTIMAGE_CORE_ID_r5fss1-0 = 6
BOOTIMAGE_CORE_ID_r5fss1-1 = 7
BOOTIMAGE_CORE_ID_m4fss0-0 = 14
SBL_RUN_ADDRESS=0x70000000
SBL_DEV_ID=55

MULTI_CORE_IMAGE_GEN = $(SYSCFG_NODE) $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/tools/boot/multicoreImageGen/multicoreImageGen.js
OUTRPRC_CMD = $(SYSCFG_NODE) $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/tools/boot/out2rprc/elf2rprc.js
APP_IMAGE_SIGN_CMD = $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/tools/boot/signing/appimage_x509_cert_gen.py

ifeq ($(OS),Windows_NT)
  XIPGEN_CMD=$(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/tools/boot/xipGen/xipGen.exe
else
  XIPGEN_CMD=$(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/tools/boot/xipGen/xipGen.out
endif

MULTI_CORE_IMAGE_PARAMS = \
	$(BOOTIMAGE_RPRC_NAME)@$(BOOTIMAGE_CORE_ID_r5fss0-0) \

MULTI_CORE_IMAGE_PARAMS_XIP = \
	$(BOOTIMAGE_RPRC_NAME_XIP)@$(BOOTIMAGE_CORE_ID_r5fss0-0) \

$(BOOTIMAGE_NAME): $(OUTNAME)
	@echo  Boot image: am243x:r5fss0-0:freertos:ti-arm-clang $(BOOTIMAGE_PATH)/$@ ...
ifneq ($(OS),Windows_NT)
	$(CHMOD) a+x $(XIPGEN_CMD)
endif
	$(OUTRPRC_CMD) $(OUTNAME) >> $(BOOTIMAGE_TEMP_OUT_FILE)
	$(COPY) $(BOOTIMAGE_RPRC_NAME) $(BOOTIMAGE_RPRC_NAME_TMP)
	$(RM) $(BOOTIMAGE_RPRC_NAME)
	$(XIPGEN_CMD) -i $(BOOTIMAGE_RPRC_NAME_TMP) -o $(BOOTIMAGE_RPRC_NAME) -x $(BOOTIMAGE_RPRC_NAME_XIP) --flash-start-addr 0x60000000 -v > $(BOOTIMAGE_TEMP_OUT_FILE)
	$(MULTI_CORE_IMAGE_GEN) --devID $(SBL_DEV_ID) --out $(BOOTIMAGE_NAME) $(MULTI_CORE_IMAGE_PARAMS) >> $(BOOTIMAGE_TEMP_OUT_FILE)
	$(MULTI_CORE_IMAGE_GEN) --devID $(SBL_DEV_ID) --out $(BOOTIMAGE_NAME_XIP) $(MULTI_CORE_IMAGE_PARAMS_XIP) >> $(BOOTIMAGE_TEMP_OUT_FILE)
# Sign the appimage for HS-FS using appimage signing script
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(BOOTIMAGE_NAME) --authtype 1 --key $(APP_SIGNING_KEY) --output $(BOOTIMAGE_NAME_HS_FS)
	$(RM) $(BOOTIMAGE_RPRC_NAME_TMP)
	$(RM) $(BOOTIMAGE_TEMP_OUT_FILE)
	@echo  Boot image: am243x:r5fss0-0:freertos:ti-arm-clang $(BOOTIMAGE_PATH)/$@ Done !!!
	@echo  .
	@echo  Boot image: am243x:r5fss0-0:freertos:ti-arm-clang $(BOOTIMAGE_PATH)/$(BOOTIMAGE_NAME_HS_FS) Done !!!
	@echo  .

$(BOOTIMAGE_NAME_HS): $(BOOTIMAGE_NAME)
ifeq ($(DEVICE_TYPE), HS)
# Sign the appimage using appimage signing script
ifeq ($(ENC_ENABLED),no)
	@echo Boot image signing: Encryption is disabled.
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(BOOTIMAGE_NAME) --authtype 1 --key $(APP_SIGNING_KEY) --output $(BOOTIMAGE_NAME_HS)
else
	@echo Boot image signing: Encryption is enabled.
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(BOOTIMAGE_NAME) --authtype 1 --key $(APP_SIGNING_KEY) --enc y --enckey $(APP_ENCRYPTION_KEY) --output $(BOOTIMAGE_NAME_HS)
	$(RM) $(BOOTIMAGE_NAME)-enc
endif
	@echo  Boot image: am243x:r5fss0-0:freertos:ti-arm-clang $(BOOTIMAGE_PATH)/$(BOOTIMAGE_NAME_HS) Done !!!
	@echo  .
endif
-include $(addprefix $(OBJDIR)/, $(DEPS))
