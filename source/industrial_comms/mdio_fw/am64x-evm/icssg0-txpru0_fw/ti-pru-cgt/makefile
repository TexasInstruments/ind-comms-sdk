################################################################################
# Automatically-generated file. Do not edit!
################################################################################
#   Required input arguments :
#   CCS_PROJECT_DEBUG=${CWD}   Use this input argument to include files from external paths 

export INDUSTRIAL_COMMUNICATIONS_SDK_PATH?=$(abspath ../../../../../..)
include $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/imports.mak
SYSCFG_DIR := $(CCS_PROJECT_DEBUG)/syscfg
CG_TOOL_ROOT := $(CGT_TI_PRU_PATH)

# Add inputs and outputs from these tool invocations to the build variables
CMD_SRCS += \
linker.cmd \

ASM_SRCS += \
main.asm

OBJS += \
main.obj

ASM_DEPS += \
main.d

OBJDIR := .

FILES_PATH_common = \
	.. \
	../../.. \

FILES_PATH := $(FILES_PATH_common)

vpath %.asm $(FILES_PATH)
vpath %.obj $(OBJDIR)
vpath %.cmd $(FILES_PATH)


# Add inputs and outputs from these tool invocations to the build variables
EXE_OUTPUTS += \
mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.out \

EXE_OUTPUTS__QUOTED += \
"mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.out" \

BIN_OUTPUTS += \
mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.hex \

BIN_OUTPUTS__QUOTED += \
"mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.hex" \

# All Target
all: $(OBJS) $(CMD_SRCS)
	@$(MAKE) --no-print-directory -Onone "mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.out"

# Each subdirectory must supply rules for building sources it contributes
$(OBJDIR)/$(OBJS): $(ASM_SRCS) 
	@echo 'Building file: "$<"'
	@echo 'Invoking: PRU Compiler'
	"$(CGT_TI_PRU_PATH)/bin/clpru" -DICSSG0 -DTX_PRU0 -DSLICE0 -DPRU0 -DSLICE0 -v4 --define=SOC_AM64X --include_path="$(SYSCFG_DIR)" --include_path="${CG_TOOL_ROOT}/include" --include_path="${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source" --include_path="${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source"  --define=_DEBUG_=1 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

# Tool invocations
mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.out: $(OBJS) $(CMD_SRCS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: PRU Linker'
	"$(CGT_TI_PRU_PATH)/bin/clpru" -DICSSG0 -DTX_PRU0 -DSLICE0 -DPRU0 -DSLICE0 -v4 --define=SOC_AM64X  --define=_DEBUG_=1 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little -z -m"mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.map" --disable_auto_rts   --diag_wrap=off --display_error_number --warn_sections --xml_link_info="mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt_linkInfo.xml" --rom_model -o "mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.out" $^
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# To clean generated files
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj"
	-$(RM) "main.d"
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-$(CGT_TI_PRU_PATH)/bin/hexpru.exe --diag_wrap=off --array --array:name_prefix=PRUFirmware  -o pru_load_bin.h mdio_am64x-evm_icssg0-txpru0_fw_ti-pru-cgt.out
	-@echo ' '

.PHONY: all clean
