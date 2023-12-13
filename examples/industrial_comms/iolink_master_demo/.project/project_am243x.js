let path = require('path');

let device = "am243x";

const files_evm = {
    common: [
        "IOLM_Port_LEDTask.c",
        "IOLM_Port_Sitara_soc.c",
        "IOLM_Port_SMI.c",
        "IOLM_Port_spi.c",
        "IOLM_Port_smiExample.c",
        "IOLM_Port_Utils.c",
        "nvram_driver.c",
        "main.c",
        "IOLM_workTask.c",
    ],
};

const files_lp = {
    common: [
        "IOLM_Port_led.c",
        "IOLM_Port_LEDTask.c",
        "IOLM_Port_Sitara_soc.c",
        "IOLM_Port_SMI.c",
        "IOLM_Port_spi.c",
        "IOLM_Port_smiExample.c",
        "IOLM_Port_Utils.c",
        "nvram_driver.c",
        "main.c",
        "IOLM_workTask.c",
    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const filedirs = {
    common: [
        "..",       /* core_os_combo base */
        "../..", /* Example base */
        "../../IOLinkPort",
        "../../../KBDrv",
    ],
};

const libdirs_freertos = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/stack/lib",
    ],
};

const includes_freertos_r5f_evm = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am243x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/am243x-evm",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/am243x-evm/IOLinkPort",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/KBDrv",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/SMI",
    ],
};

const includes_freertos_r5f_lp = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am243x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/am243x-lp",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/am243x-lp/IOLinkPort",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/KBDrv",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/SMI",
    ],
};

const libs_freertos_r5f_evm = {
    common: [
        "freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "iolink.am243x_evm.r5f.ti-arm-clang.release.lib",
        "littlefs.am243x_evm.r5f.ti-arm-clang.release.lib",
    ],
};

const libs_freertos_r5f_lp = {
    common: [
        "freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "iolink.am243x_lp.r5f.ti-arm-clang.release.lib",
        "littlefs.am243x_lp.r5f.ti-arm-clang.release.lib",
    ],
};

const defines_r5f_evm = {
    common: [
        "SOC_AM243X=1",
        "OSAL_FREERTOS",
    ],
};

const defines_r5f_lp = {
    common: [
        "SOC_AM243X=1",
        "OSAL_FREERTOS",
    ],
};

const cflags_r5f = {
    common: [
        "-Wno-unused-but-set-variable",
        "-Wno-cpp",
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

const readmeDoxygenPageTag = "EXAMPLES_INDUSTRIAL_COMMS_IOLINK_MASTER_DEMO";

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-evm", os: "freertos"},
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-lp", os: "freertos"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "iolink_master_demo";
    property.isInternal = false;
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    build_property.filedirs = filedirs;
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
                build_property.files = files_evm;
                build_property.includes = includes_freertos_r5f_evm;
                build_property.libs = libs_freertos_r5f_evm;
            }
            else if(buildOption.board.match(/am243x-lp*/) )
            {
                build_property.defines = defines_r5f_lp;
                build_property.files = files_lp;
                build_property.includes = includes_freertos_r5f_lp;
                build_property.libs = libs_freertos_r5f_lp;
            }
        }
    }

    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
