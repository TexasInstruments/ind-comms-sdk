#
# Auto generated makefile
#

export INDUSTRIAL_COMMUNICATIONS_SDK_PATH?=$(abspath ../../../../../../..)
include $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/imports.mak
include $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/devconfig/devconfig.mak

CG_TOOL_ROOT=$(CGT_TI_ARM_CLANG_PATH)

CC=$(CG_TOOL_ROOT)/bin/tiarmclang
LNK=$(CG_TOOL_ROOT)/bin/tiarmclang
STRIP=$(CG_TOOL_ROOT)/bin/tiarmstrip
OBJCOPY=$(CG_TOOL_ROOT)/bin/tiarmobjcopy
COV=$(CG_TOOL_ROOT)/bin/tiarmcov
PROFDATA=$(CG_TOOL_ROOT)/bin/tiarmprofdata
COVERAGE_PATH=$(abspath .)
ifeq ($(OS), Windows_NT)
	PYTHON=python
else
	PYTHON=python3
endif

PROFILE?=release
ConfigName:=$(PROFILE)

OUTNAME:=prp_rgmii_demo.$(PROFILE).out

BOOTIMAGE_PATH=$(abspath .)
BOOTIMAGE_NAME:=prp_rgmii_demo.$(PROFILE).appimage
BOOTIMAGE_NAME_XIP:=prp_rgmii_demo.$(PROFILE).appimage_xip
BOOTIMAGE_NAME_SIGNED:=prp_rgmii_demo.$(PROFILE).appimage.signed
BOOTIMAGE_RPRC_NAME:=prp_rgmii_demo.$(PROFILE).rprc
BOOTIMAGE_RPRC_NAME_XIP:=prp_rgmii_demo.$(PROFILE).rprc_xip
BOOTIMAGE_RPRC_NAME_TMP:=prp_rgmii_demo.$(PROFILE).rprc_tmp
BOOTIMAGE_NAME_HS:=prp_rgmii_demo.$(PROFILE).appimage.hs
BOOTIMAGE_NAME_HS_FS:=prp_rgmii_demo.$(PROFILE).appimage.hs_fs
TARGETS := $(BOOTIMAGE_NAME)

FILES_common := \
	hsr_prp_menu.c \
	hsr_prp_main.c \
	hsr_prp_soc.c \
	main.c \
	hsr_prp_prvmib.c \
	udp_iperf.c \
	ti_drivers_config.c \
	ti_drivers_open_close.c \
	ti_board_config.c \
	ti_board_open_close.c \
	ti_dpl_config.c \
	ti_pinmux_config.c \
	ti_power_clock_config.c \

FILES_PATH_common = \
	.. \
	../.. \
	../../.. \
	../../../.. \
	../../../../common/am243lp \
	generated \

INCLUDES_common := \
	-I${CG_TOOL_ROOT}/include/c \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am243x/r5f \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/hsr_prp_demo \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-stack/src/include \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lwipif/inc \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-stack/contrib \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/hsr_prp/stack/lwip/lwip-config/am243x \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/include \
	-I${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/freertos/include \
	-Igenerated \

DEFINES_common := \
	-DSOC_AM243X \
	-DBUILD_PRP \
	-DPTP_TESTING \
	-DHSR_PRP_RGMII \

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
	-mno-unaligned-access \

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

CFLAGS_debug := \
	-D_DEBUG_=1 \

CFLAGS_release := \
	-Os \

LNK_FILES_common = \
	generated/linker.cmd \

LIBS_PATH_common = \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_timesync/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/hsr_prp/icss_fwhal/lib \
	-Wl,-i${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/hsr_prp/stack/lwip/lib \
	-Wl,-i${CG_TOOL_ROOT}/lib \

LIBS_common = \
	-lfreertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-ldrivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lboard.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-licss_emac.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lhsr_prp_icss_emac_lwip_if.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-licss_timesync.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lprp_rgmii_icss_fwhal.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lhsr_prp_lwip_contrib.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lhsr_prp_lwip_freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
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
	hsr_prp_icss_emac_lwip_if.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	icss_timesync.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	prp_rgmii_icss_fwhal.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	hsr_prp_lwip_contrib.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	hsr_prp_lwip_freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib \
	libc.a \
	libsysbm.a \

LIBS_PATH_NAME = \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_timesync/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/hsr_prp/icss_fwhal/lib \
	${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/hsr_prp/stack/lwip/lib \
	${CG_TOOL_ROOT}/lib \

FILES := $(FILES_common) $(FILES_$(PROFILE))
ASMFILES := $(ASMFILES_common) $(ASMFILES_$(PROFILE))
FILES_PATH := $(FILES_PATH_common) $(FILES_PATH_$(PROFILE))
CFLAGS := $(CFLAGS_common) $(CFLAGS_$(PROFILE))
ifeq ($(INSTRUMENTATION_MODE), yes)
CFLAGS += -fprofile-instr-generate -fcoverage-mapping
endif
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

SYSTEM_FLAG ?= false

ifeq ($(SYSTEM_FLAG), false)
	SYSTEM_COMMAND := syscfg $(SYSCFG_GEN_FILES) $(OBJS) $(LNK_FILES) $(LIBS_NAME)
else
	SYSTEM_COMMAND := $(OBJS) $(LNK_FILES) $(LIBS_NAME)
endif

$(OUTNAME):  $(SYSTEM_COMMAND)
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
	@echo  Scrubing: am243x:r5fss0-0:freertos:ti-arm-clang prp_rgmii_demo ...
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
	$(SYSCFG_NODE) $(SYSCFG_CLI_PATH)/dist/cli.js --product $(SYSCFG_SDKPRODUCT) --context r5fss0-0 --part ALX --package ALX --output generated/ ../example.syscfg

syscfg-gui:
	$(SYSCFG_NWJS) $(SYSCFG_PATH) --product $(SYSCFG_SDKPRODUCT) --device AM243x_ALX_beta --context r5fss0-0 --part ALX --package ALX --output generated/  ../example.syscfg

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
	$(RM) $(BOOTIMAGE_RPRC_NAME_TMP)
	$(RM) $(BOOTIMAGE_TEMP_OUT_FILE)

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
	$(RM) $(BOOTIMAGE_NAME)
	@echo  Boot image: am243x:r5fss0-0:freertos:ti-arm-clang $(BOOTIMAGE_PATH)/$(BOOTIMAGE_NAME_HS) Done !!!
	@echo  .
else
# Sign the appimage for HS-FS using appimage signing script
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(BOOTIMAGE_NAME) --authtype 1 --key $(APP_SIGNING_KEY) --output $(BOOTIMAGE_NAME_HS_FS)
	$(RM) $(BOOTIMAGE_NAME)
	@echo  Boot image: am243x:r5fss0-0:freertos:ti-arm-clang $(BOOTIMAGE_PATH)/$(BOOTIMAGE_NAME_HS_FS) Done !!!
	@echo  .
endif
.PHONY: coverage
coverage:
	@echo Creating Coverage Report for prp_rgmii_demo.$(PROFILE) ...
	$(MKDIR) coverage
	$(PROFDATA) merge -sparse -obj-file=$(OUTNAME) $(OUTNAME).cnt -o prp_rgmii_demo.$(PROFILE).profdata
	$(COV) show --format=html --show-expansions --show-instantiations --show-branches=count --object=$(OUTNAME) -instr-profile=prp_rgmii_demo.$(PROFILE).profdata --output-dir=$(COVERAGE_PATH)/coverage --ignore-filename-regex=build_jenkins
	$(COV) export --format=text --object=$(OUTNAME) --instr-profile=prp_rgmii_demo.$(PROFILE).profdata > coverage/prp_rgmii_demo.$(PROFILE).profdata.json
	node $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/mcu_plus_sdk/tools/smart_placement/clang_coverage_analyse.js --input=coverage/prp_rgmii_demo.$(PROFILE).profdata.json --output-json=coverage/prp_rgmii_demo.$(PROFILE).analysis.json --output=../prp_rgmii_demo.annotations.$(PROFILE).S --top-function-count=500
	@echo Coverage Report Generated at $(COVERAGE_PATH)/coverage folder !!!

-include $(addprefix $(OBJDIR)/, $(DEPS))
