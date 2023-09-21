let path = require('path');

let device = "am243x";

const files = {
    common: [
        "ESL_eeprom.c",
        "ESL_OS_os.c",
        "main.c",
        "nvram_driver.c",
        "SMIdirect_UART.c",
    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const filedirs_evm = {
    common: [
        "..",         /* core_os_combo base */
        "../../..",   /* Example base */
        "../../../../../common",
        "../../../../ethercat_slave_demo/common/os",
        "../../../../ethercat_slave_demo/common/os/freertos",
    ],
};

const filedirs_lp = {
    common: [
        "..",         /* core_os_combo base */
        "../../..",   /* Example base */
        "../../../../ethercat_slave_demo/common/os",
        "../../../../ethercat_slave_demo/common/os/freertos",
    ],
};


const libdirs_freertos = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_iolink_gateway/stack/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/lib",
    ],
};

const includes_freertos_r5f_evm = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am243x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_iolink_gateway_demo",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_iolink_gateway_demo/am243x-evm/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am243evm",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am243evm/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/custom_phy/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc/littlefs",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc/drivers",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_iolink_gateway/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/SMI",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
    ],
};

const includes_freertos_r5f_lp = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am243x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_iolink_gateway_demo",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_iolink_gateway_demo/am243x-lp/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am243lp",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am243lp/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/custom_phy/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc/littlefs",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc/drivers",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_iolink_gateway/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/SMI",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
    ],
};


const libs_freertos_r5f_evm = {
    common: [
        "freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethercat_slave_bkhfSsc.am243x_evm.r5f.ti-arm-clang.release.lib",
        "ethercat_iolink_gateway.am243x_evm.r5f.ti-arm-clang.release.lib",
        "ethercat_slave.am243x_evm.r5f.ti-arm-clang.release.lib",
        "iolink_master.am243x_evm.r5f.ti-arm-clang.release.lib",
        "littlefs.am243x_evm.r5f.ti-arm-clang.release.lib",
    ],
};

const libs_freertos_r5f_lp = {
    common: [
        "freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethercat_slave_bkhfSsc.am243x_lp.r5f.ti-arm-clang.release.lib",
        "ethercat_iolink_gateway.am243x_lp.r5f.ti-arm-clang.release.lib",
        "ethercat_slave.am243x_lp.r5f.ti-arm-clang.release.lib",
        "iolink_master.am243x_lp.r5f.ti-arm-clang.release.lib",
        "littlefs.am243x_lp.r5f.ti-arm-clang.release.lib",
    ],
};

const defines_r5f_evm = {
    common: [
        "SOC_AM243X=1",
        "SOC_AM243X_ALV=1",
        "OSAL_FREERTOS=1",
        "core0",
        "BUILD_MCU",
        "OS_TYPE=FreeRtos",
        "VENDOR=TI",
        "TI_EC_VENDOR=1",
        "VARIANT=Rel",
        "CPU=R5F",
        "INDEX=0",
        "KUNBUS_EC_STACK_EVAL=0",
        "am243x",
        "am243x_evm",
        "SSC_CHECKTIMER=1",
        "USE_ECAT_TIMER=1",
        "EC_IOL_GATEWAY=1",
        "BUILD_MCU1_0",
        "IOLM_OS_AVAILABLE",
        "IOLM_SMI_ENABLED=1",
        "IO_LINK_EXT_SMISERIAL=1",
        "OSAL_PRINTF_UART_DISABLE=1",
        "IOL8M_SITARA_VERSION=0x00010000",
        "ECAT_REVISION=0x00010000",
        "OSPIFLASH_APP_STARTMAGIC=0xFFAA11EE",
    ],
};

const defines_r5f_lp = {
    common: [
        "SOC_AM243X=1",
        "SOC_AM243X_ALX=1",
        "OSAL_FREERTOS=1",
        "core0",
        "BUILD_MCU",
        "OS_TYPE=FreeRtos",
        "VENDOR=TI",
        "TI_EC_VENDOR=1",
        "VARIANT=Rel",
        "CPU=R5F",
        "INDEX=0",
        "KUNBUS_EC_STACK_EVAL=0",
        "am243x",
        "am243x_lp",
        "SSC_CHECKTIMER=1",
        "USE_ECAT_TIMER=1",
        "EC_IOL_GATEWAY=1",
        "BUILD_MCU1_0",
        "IOLM_OS_AVAILABLE",
        "IOLM_SMI_ENABLED=1",
        "IO_LINK_EXT_SMISERIAL=1",
        "OSAL_PRINTF_UART_DISABLE=1",
        "IOL8M_SITARA_VERSION=0x00010000",
        "ECAT_REVISION=0x00010000",
        "OSPIFLASH_APP_STARTMAGIC=0xFFAA11EE",
    ],
};

const cflags_r5f = {
    common: [
        "-Wno-unused-but-set-variable",
    ],
    debug: [
        "-Og",
    ],
};

const lflags_r5f = {
    common: [
        "--use_memcpy=fast",
        "--use_memset=fast",
    ],
};

const lnkfiles = {
    common: [
        "linker.cmd",
    ]
};

const syscfgfile = "../example.syscfg";

const readmeDoxygenPageTag = "EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_IOLINK_GATEWAY_DEMO";

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-evm", os: "freertos"},
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-lp", os: "freertos"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "ethercat_iolink_gateway_demo";
    property.isInternal = false;
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    build_property.files = files;
    build_property.lnkfiles = lnkfiles;
    build_property.syscfgfile = syscfgfile;
    build_property.readmeDoxygenPageTag = readmeDoxygenPageTag;
    build_property.projecspecFileAction = "link";

    if(buildOption.cpu.match(/r5f*/)) {
        if(buildOption.os.match(/freertos*/) )
        {
            build_property.libdirs = libdirs_freertos;
            build_property.cflags = cflags_r5f;
            build_property.lflags = lflags_r5f;

            if(buildOption.board.match(/am243x-evm*/) )
            {
                build_property.defines = defines_r5f_evm;
                build_property.libs = libs_freertos_r5f_evm;
                build_property.filedirs = filedirs_evm;
                build_property.includes = includes_freertos_r5f_evm;
            }
            else if(buildOption.board.match(/am243x-lp*/) )
            {
                build_property.defines = defines_r5f_lp;
                build_property.libs = libs_freertos_r5f_lp;
                build_property.filedirs = filedirs_lp;
                build_property.includes = includes_freertos_r5f_lp;
            }
        }
    }

    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
