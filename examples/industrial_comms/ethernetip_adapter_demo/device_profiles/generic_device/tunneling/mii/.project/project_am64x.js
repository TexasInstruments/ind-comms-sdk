let path = require('path');

let device = "am64x";

const files = {
    common: [
        "app.c",
        "app_task.c",
        "drivers.c",
        "drv_eeprom.c",
        "drv_flash.c",
        "drv_pruicss.c",
        "drv_uart.c",
        "drv_led.c",
        "drv_common.c",
        "web_server.c",
        "cust_drivers.c",
        "cust_eeprom.c",
        "cust_ethphy.c",
        "cust_flash.c",
        "cust_led.c",
        "CUST_PHY_base.c",
        "CUST_PHY_dp83869.c",
        "cmn_cpu_main.c",
        "cmn_app.c",
        "cmn_board.c",
        "cmn_mem.c",
        "cmn_os.c",
        "device_profile_intf.c",
        "device_profile_nvm.c",
        "device_profile_reset.c",
        "device_profile_cfg.c",
        "generic_device.c",
        "generic_device_cfg.c",
        "app_control.c",
        "ti_ic_open_close.c",
        "netif_common.c",
        "app_netif.c",
        "app_tcpserver.c",
        "udp_iperf.c",
    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const filedirs = {
    common: [
        "../../../../../../..",
        "../../../../../../../config",
        "../../../../../../../config/app",
        "../../../../../../../config/board/am64x-evm",
        "../../../../../../../config/profile",
        "../../../../../../../config/profile/generic_device",
        "../../../../../../../config/stack",
        "../../../../../../../custom",
        "../../../../../../../custom/eeprom",
        "../../../../../../../custom/ethphy",
        "../../../../../../../custom/flash",
        "../../../../../../../custom/led",
        "../../../../../../../drivers",
        "../../../../../../../drivers/common",
        "../../../../../../../drivers/eeprom",
        "../../../../../../../drivers/flash",
        "../../../../../../../drivers/led",
        "../../../../../../../drivers/uart",
        "../../../../../../../drivers/pruicss",
        "../../../../../../../device_profiles",
        "../../../../../../../device_profiles/common",
        "../../../../../../../device_profiles/generic_device",
        "../../../../../../../device_profiles/generic_device/tunneling",
        "../../../../../../../../custom_phy/src",
        "../../../../../../../os/freertos",
        "../../../../../../../services/web_server",
        "../../../../../../../services/tunneling",
    ],
};

const libdirs_freertos = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_timesync/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/icss_fwhal/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/lwip_ic/lib",
    ],
};

const includes_freertos_r5f = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am64x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/custom_phy/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/config",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/config/board/am64x-evm",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/device_profiles",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/device_profiles/common",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/device_profiles/generic_device",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/os/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/custom",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/drivers",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/services/web_server",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethernetip_adapter_demo/services/tunneling",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethernetip_adapter/stack/lwip_ic/lwip-config",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-stack/src/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/freertos/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-stack/contrib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/enet/core/lwipif/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/enet/core/lwip_ic",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/enet/core/lwip_ic/lwipific/inc",
    ],
};

const libs_freertos_r5f = {
    common: [
        "freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "lwip-contrib-freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "icss_emac.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "icss_timesync.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethernetip_adapter_mii_icss_fwhal.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethernetip_adapter_lwip-ic-contrib.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethernetip_adapter_lwip-ic-freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethernetip_adapter_lwipif-ic-freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethernetipadapter_mii.am64x.r5f.ti-arm-clang.release.lib",
    ],
};

const defines_r5f = {
    common: [
        "OSAL_FREERTOS=1",
        "SOC_AM64X",
        "SOC_AM64X=1",
        "CUST_PHY_DP83869=1",
        "EIP_TIME_SYNC=1",
        "EIP_QUICK_CONNECT=0",
        "CPU_LOAD_MONITOR=0",
        "ENABLE_INTERCORE_TUNNELING=1"
    ],
};

const cflags_r5f = {
    common: [
        "-Wno-unused-but-set-variable",
        "-mllvm",
        "-align-all-functions=2",
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

const readmeDoxygenPageTag = "EXAMPLES_INDUSTRIAL_COMMS_ETHERNETIP_ADAPTER_TUNNELING_DEMO";

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am64x-evm", os: "freertos"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "ethernetip_adapter_generic_device_mii_tunneling_demo";
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

    if(buildOption.cpu.match(/r5f*/)) {
        if(buildOption.os.match(/freertos*/) )
        {
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
