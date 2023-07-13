let path = require('path');

let device = "am64x";

const files = {
    common: [
            "loopback_cfg.c",
            "loopback_test.c",
            "loopback_main.c",
            "main.c",
    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const filedirs = {
    common: [
        "..",       /* core_os_combo base */
        "../../..", /* Example base */
    ],
};

const libdirs_nortos = {
    common: [
        "generated",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/kernel/nortos/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/drivers/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/board/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/lib",

    ],
};

const includes_nortos_r5f = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/core",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/utils",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/utils/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/utils/V3",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/core",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/core/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/core/include/phy",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/core/include/core",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/hw_include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/soc/k3/am64x_am243x",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/hw_include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk//source/networking/enet/hw_include/mdio/V4",
    ],
};

const libs_nortos_r5f = {
    common: [
        "nortos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "enet-icssg.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
    ],
};

const linker_includePath_nortos = {
    common: [
        "${PROJECT_BUILD_DIR}/syscfg",

    ],
};

const defines_r5f = {
    common: [
         "ENET_ENABLE_PER_ICSSG=1",
    ],
};

const cflags_r5f = {
    common: [
    ],
    release: [
        "-Oz",
        "-flto",
    ],
};

const lflags_r5f = {
    common: [
        "--zero_init=on",
        "--use_memset=fast",
        "--use_memcpy=fast"
    ],
};

const loptflags_r5f = {
    release: [
        "-mcpu=cortex-r5",
        "-mfloat-abi=hard",
        "-mfpu=vfpv3-d16",
        "-mthumb",
        "-Oz",
        "-flto"
    ],
};

const lnkfiles = {
    common: [
        "linker.cmd",
    ]
};

const syscfgfile = "../example.syscfg";

const readmeDoxygenPageTag = "EXAMPLES_ENET_ICSSG_LOOPBACK";

const templates_nortos_r5f =
[
    {
        input: ".project/templates/am243x/nortos/main_nortos.c.xdt",
        output: "../main.c",
        options: {
            entryFunction: "EnetLpbk_mainTask",
            taskPri : "2",
            stackSize : "8192",
        },
    }
];

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am64x-evm", os: "nortos"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "enet_icssg_loopback";
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
    build_property.projecspecFileAction = "link";
    build_property.readmeDoxygenPageTag = readmeDoxygenPageTag;
    if(buildOption.cpu.match(/r5f*/)) {
        if(buildOption.os.match(/nortos*/) )
        {
            const _ = require('lodash');
            let libdirs_nortos_cpy = _.cloneDeep(libdirs_nortos);
            /* Logic to remove generated/ from libdirs_nortos, it generates warning for ccs build */
            if (buildOption.isProjectSpecBuild === true)
            {
                var delIndex = libdirs_nortos_cpy.common.indexOf('generated');
                if (delIndex !== -1) {
                    libdirs_nortos_cpy.common.splice(delIndex, 1);
                }
            }
            build_property.includes = includes_nortos_r5f;
            build_property.libdirs = libdirs_nortos_cpy;
            build_property.libs = libs_nortos_r5f;
            build_property.templates = templates_nortos_r5f;
            build_property.defines = defines_r5f;
            build_property.cflags = cflags_r5f;
            build_property.lflags = lflags_r5f;
            build_property.projectspecLnkPath = linker_includePath_nortos;
            build_property.loptflags = loptflags_r5f;
        }
    }
    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
