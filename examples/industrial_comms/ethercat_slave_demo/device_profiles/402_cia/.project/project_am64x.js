let path = require('path');

let device = "am64x";

const files = {
    common: [
        
        "ESL_version.c",
        "ESL_OS_os.c",
        "ESL_eeprom.c",
        "ESL_fileHandling.c",
        "ESL_foeDemo.c",
        "ESL_soeDemo.c",
        "ESL_gpioHelper.c",
        "ESL_BOARD_OS_config.c",
        "CUST_PHY_base.c",
        "CUST_PHY_dp83869.c",
        "ecSlvCiA402.c",
        "ESL_cia402Demo.c",
        "ESL_cia402Obd.c",
        "EtherCAT_Slave_CiA402.c",
    
    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const filedirs = {
    common: [
        
        "..",         /* core_os_combo base */
        "../../..",   /* Example base */
        "../../../../../common",
        "../../../../../common/board/am64gpevm",
        "../../../../../common/board/am64gpevm/freertos",
        "../../../../../common/os",
        "../../../../../common/os/freertos",
        "../../../../../../custom_phy/src",
        "../../../../../../custom_phy/inc",
        
    ],
};

const libdirs_freertos = {
    common: [
       
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/drivers/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/board/lib",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/lib",
    
    ],
};

const includes_freertos_r5f = {
    common: [
       
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am64x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/device_profiles/402_cia",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am64gpevm",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am64gpevm/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/custom_phy/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
    
    ],
};

const libs_freertos_r5f = {
    common: [
       
        "freertos.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am64x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethercat_slave.am64x.r5f.ti-arm-clang.release.lib",
        "ethercat_slave_bkhfSsc.am64x.r5f.ti-arm-clang.release.lib",
    
    ],
};

const defines_r5f = {
    common: [
       
        "SOC_AM64X=1",
        "OSAL_FREERTOS=1",
        "core0",
        "am64x",
        "am64x_evm",
        "SSC_CHECKTIMER=1",
        "USE_ECAT_TIMER=1",
    
    ],
};

const cflags_r5f = {
    common: [
       
        "-Wno-unused-but-set-variable",
     
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

const readmeDoxygenPageTag = "EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_SLAVE_DEMOS";

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am64x-evm", os: "freertos"},  
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "ethercat_slave_cia402_demo";
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
