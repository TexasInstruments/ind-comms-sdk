let path = require('path');

let device = "am64x";

const files = {
    common: [

        "iolm_ledtask.c",
        "iolm_sitara_soc.c",
        "iolm_board_cfg.c",
        "iolm_ext_board_cfg.c",
        "iolm_smi_uart.c",
        "iolm_example.c",
        "iolm_utils.c",
        "nvm.c",
        "nvm_drv_eeprom.c",
        "nvm_drv_flash.c",
        "main.c",

    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const filedirs = {
    common: [

        "../../..",       /* core_os_combo base */
        "../../../..", /* Example base */
        "../../../../example",
        "../../../../example/am64x-evm",
        "../../../../common",
        "../../../../common/board/am64x-evm",
        "../../../../common/board/tmds64dc01evm",
        "../../../../../nvm/app/src",
        "../../../../../nvm/drv/src",

    ],
};

const libdirs_freertos = {
    common: [

        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/stack/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/lib",

    ],
};

const includes_freertos_r5f = {
    common: [

        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am64x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/example",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/common",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/common/board/am64x-evm",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/iolink_master_demo/example/am64x-evm",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/nvm/app/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/nvm/drv/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/stack/inc/ext",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/iolink/stack/inc/SMI",

    ],
};

const libs_freertos_r5f = {
    common: [

        "freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "iolink.am64x.r5f.ti-arm-clang.release.lib",

    ],
};

const defines_r5f = {
    common: [

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
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am64x-evm", os: "freertos" },
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

    build_property.files = files;
    build_property.filedirs = filedirs;
    build_property.lnkfiles = lnkfiles;
    build_property.syscfgfile = syscfgfile;
    build_property.readmeDoxygenPageTag = readmeDoxygenPageTag;
    build_property.projecspecFileAction = "link";

    if (buildOption.cpu.match(/r5f*/)) {
        if (buildOption.os.match(/freertos*/)) {
            build_property.includes = includes_freertos_r5f;
            build_property.libdirs = libdirs_freertos;
            build_property.libs = libs_freertos_r5f;
            build_property.defines = defines_r5f;
            build_property.cflags = cflags_r5f;
            build_property.lflags = lflags_r5f;
        }
    }

    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
