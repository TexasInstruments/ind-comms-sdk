let path = require('path');

let device = "am64x";

const server_files = {
    common: [
            "udp_iperf.c",
            "app_announce.c",
            "ti_ic_open_close.c",
            "enet_netific.c",
            "app_netif.c",
            "emac_lwipif.c",
            "app_tcpserver.c",
            "app_main.c",
            "main.c",
            "enet_custom_board_config.c",
            "hsr_prp_soc.c",
    ],
};

const client_files = {
    common: [
            "enet_netific.c",
            "udp_iperf.c",
            "app_netif.c",
            "app_announce.c",
            "ti_ic_open_close.c",
            "app_main.c",
            "main.c",
            "enet_custom_board_config.c",
    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const main_filedirs = {
    common: [
        "..",       /* core_os_combo base */
        "../../../common", /* Example base */
        "../../../core_main", /* Example base */
        "../../../common/am64evm"
    ],
};

const remote_filedirs = {
    common: [
        "..",       /* core_os_combo base */
        "../../../common", /* Example base */
        "../../../core_remote", /* Example base */
        "../../../../../../mcu_plus_sdk/source/networking/enet/core/lwip_ic/intercore/src", /* IC base */
        "../../../../../../mcu_plus_sdk/source/networking/enet/core/lwip_ic/lwipific/src", /* IC base */
    ],
};

const libdirs_freertos = {
    common: [
	    "generated",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/enet/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lwipif/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_timesync/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/hsr_prp/icss_fwhal/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/profinet_device/icss_fwhal/lib",        
        // "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/icss_fwhal/lib",    //EIP change 
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lib",

    ],
};

const includes_freertos_r5f = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am64x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-stack/src/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/freertos/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-stack/contrib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-config/am64x",
        // "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/icss_fwhal",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/profinet_device/icss_fwhal",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/hsr_prp/icss_fwhal",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lwipif/inc",        
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/hsr_prp_demo",

        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/enet/core/lwip_ic",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/enet/core/lwip_ic/lwipific/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_bridge/common",
    ],
};

const libs_freertos_r5f = {
    common: [
        "freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "lwip-freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "lwipif-ic-icssg-freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "lwip-contrib-freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",

        // emac related includes
        "icss_emac.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "icss_emac_lwip_if.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "icss_timesync.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "prp_mii_icss_fwhal.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "profinet_device_rt_mrp_mii_icss_fwhal.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "profinet_device_irt_mii_icss_fwhal.am64x.r5f.ti-arm-clang.${ConfigName}.lib"
        // "ethernetip_adapter_mii_icss_fwhal.am64x.r5f.ti-arm-clang.debug.lib"
    ],
};

const linker_includePath_freertos = {
    common: [
        "${PROJECT_BUILD_DIR}/syscfg",

    ],
};

const cflags_r5f = {
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

const readmeDoxygenPageTag = "EXAMPLES_ENET_LAYER2_ICSSG";

const templates_freertos_server =
[
    {
        input: ".project/templates/am64x/freertos/main_freertos.c.xdt",
        output: "../main.c",
        options: {
            entryFunction: "appMain",
            taskPri : "1",
            stackSize : "8192",
        },
    },
];

const templates_freertos_client =
[
    {
        input: ".project/templates/am64x/freertos/main_freertos.c.xdt",
        output: "../main.c",
        options: {
            entryFunction: "appMain",
            taskPri : "1",
            stackSize : "8192",
        },
    },
];

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am64x-evm", os: "freertos", isPartOfSystemProject: true},
    { device: device, cpu: "r5fss0-1", cgt: "ti-arm-clang", board: "am64x-evm", os: "freertos", isPartOfSystemProject: true},
];

const systemProject = [
    {
        name: "ethernetip_adapter_bridge",
        tag: "ethernetip_adapter_bridge",
        skipProjectSpec: false,
        readmeDoxygenPageTag: readmeDoxygenPageTag,
        board: "am64x-evm",
        projects: [
            { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am64x-evm", os: "freertos"},
            { device: device, cpu: "r5fss0-1", cgt: "ti-arm-clang", board: "am64x-evm", os: "freertos"},
            ]
    },
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "ethernetip_adapter_bridge";
    property.isInternal = false;
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    if(buildOption.cpu.match(/r5fss0-0/)) {
        build_property.files = server_files;
    }
    else {
        build_property.files = client_files;
    }

    build_property.syscfgfile = syscfgfile;
    build_property.projecspecFileAction = "link";
    build_property.readmeDoxygenPageTag = readmeDoxygenPageTag;

    if(buildOption.cpu.match(/r5f*/)) {
        if(buildOption.os.match(/freertos*/) )
        {
            const _ = require('lodash');
            let libdirs_freertos_cpy = _.cloneDeep(libdirs_freertos);
            /* Logic to remove generated/ from libdirs_freertos, it generates warning for ccs build */
            if (buildOption.isProjectSpecBuild === true)
            {
                var delIndex = libdirs_freertos_cpy.common.indexOf('generated');
                if (delIndex !== -1) {
                    libdirs_freertos_cpy.common.splice(delIndex, 1);
                }
            }
            build_property.includes = includes_freertos_r5f;
            build_property.libdirs = libdirs_freertos_cpy;
            build_property.libs = libs_freertos_r5f;
            build_property.lnkfiles = lnkfiles;
            if(buildOption.cpu.match(/r5fss0-0/)) {
                build_property.templates = templates_freertos_server;
                build_property.filedirs = main_filedirs;
            }
            else {
                build_property.templates = templates_freertos_client;
                build_property.filedirs = remote_filedirs;
            }
            build_property.cflags = cflags_r5f;
            build_property.lflags = lflags_r5f;
            build_property.projectspecLnkPath = linker_includePath_freertos;
            build_property.loptflags = loptflags_r5f;
        }
    }

    return build_property;
}

function getSystemProjects(device)
{
    return systemProject;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
    getSystemProjects,
};



