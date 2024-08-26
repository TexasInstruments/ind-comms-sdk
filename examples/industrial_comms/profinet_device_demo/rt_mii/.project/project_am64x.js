let path = require('path');

let device = "am64x";

const files = {
    common: [
        "pn_app_iod_alarm.c",
        "pn_app_iod_bsp.c",
        "pn_app_iod_communication.c",
        "pn_app_iod_data.c",
        "pn_app_iod_device.c",
        "pn_app_iod_mod_cfg.c",
        "pn_app_iod_settings.c",
        "pn_app_iod_record.c",
        "pn_app_iod_utils.c",
        "CUST_PHY_base.c",
        "CUST_PHY_dp83869.c",
        "appHwBoardInfo.c",
        "appPRUConfig.c",
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
        "../../../..",
        "../../../../appDemo/inc",
        "../../../../appDemo/src",
        "../../../../appHw",
        "../../../../appHw/eeprom",
        "../../../../../custom_phy/src",
        "../../../../../nvm/app/src",
        "../../../../../nvm/drv/src",
    ],
};

const libdirs_freertos = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_timesync/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/networking/icss_emac/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/profinet_device/stack/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/profinet_device/stack/lib/lwip",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/profinet_device/stack/lib/patch",
    ],
};

const includes_freertos_r5f = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am64x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-stack/src/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/networking/lwip/lwip-port/freertos/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/custom_phy/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/nvm/app/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/nvm/drv/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/profinet_device/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/profinet_device/stack/inc/ext",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/profinet_device_demo/appDemo/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/profinet_device_demo",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/profinet_device_demo/appHw/eeprom",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/profinet_device_demo/appHw",
    ],
};

const libs_freertos_r5f = {
    common: [
        "freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
                "drivers.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
                "board.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
                "icss_emac.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
                "kunbusapi_mii.am64x.r5f.ti-arm-clang.release.lib",
                "profinetiodevice_mii.am64x.r5f.ti-arm-clang.release.lib",
                "lwipallapps.am64x.r5f.ti-arm-clang.release.lib",
                "lwipcore.am64x.r5f.ti-arm-clang.release.lib",
                "profinet_device_irt_mii_icss_fwhal.am64x.r5f.ti-arm-clang.release.lib",
    ],
};

const defines_r5f = {
    common: [
        "OSAL_FREERTOS",
        "INCLUDE_MDIO_MANUAL_MODE_WORKAROUND",
        "CUST_PHY_DP83869=1"
    ],
};

const cflags_r5f = {
    common: [
        "-mno-unaligned-access",
        "-mllvm -align-all-functions=2",
    ],
    debug: [
        
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

const readmeDoxygenPageTag = "EXAMPLES_INDUSTRIAL_COMMS_PROFINET_DEVICE_DEMO";

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am64x-evm", os: "freertos"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "profinet_device_demo";
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
