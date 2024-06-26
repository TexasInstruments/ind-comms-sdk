let path = require('path');

let device = "am243x";

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
        "ecSlvSimple.c",
        "EtherCAT_Slave_Simple.c",
    
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
        "../../../../../common/board/am243evm",
        "../../../../../common/board/am243evm/freertos",
        "../../../../../common/os",
        "../../../../../common/os/freertos",
        "../../../../../../custom_phy/src",
        "../../../../../../custom_phy/inc",
    
    ],
};

const filedirs_lp = {
    common: [
        
        "..",         /* core_os_combo base */
        "../../..",   /* Example base */
        "../../../../../common",
        "../../../../../common/board/am243lp",
        "../../../../../common/board/am243lp/freertos",
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

const includes_freertos_r5f_evm = {
    common: [
       
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am243x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/device_profiles/ctt",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am243evm",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am243evm/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/custom_phy/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
    
    ],
};

const includes_freertos_r5f_lp = {
    common: [
       
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am243x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/device_profiles/ctt",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/os/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am243lp",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/ethercat_slave_demo/common/board/am243lp/freertos",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/examples/industrial_comms/custom_phy/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
    
    ],
};

const libs_freertos_r5f_evm = {
    common: [
       
        "freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethercat_slave.am243x_evm.r5f.ti-arm-clang.release.lib",
        "ethercat_slave_bkhf_ssc.am243x_evm.r5f.ti-arm-clang.release.lib",
    
    ],
};

const libs_freertos_r5f_lp = {
    common: [
       
        "freertos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "ethercat_slave.am243x_lp.r5f.ti-arm-clang.release.lib",
        "ethercat_slave_bkhf_ssc.am243x_lp.r5f.ti-arm-clang.release.lib",
    
    ],
};

const defines_r5f_evm = {
    common: [
       
        "SOC_AM243X=1",
        "OSAL_FREERTOS=1",
        "core0",
        "am243x",
        "am243x_evm",
        "SSC_CHECKTIMER=1",
        "USE_ECAT_TIMER=1",
    
    ],
};

const defines_r5f_lp = {
    common: [
       
        "SOC_AM243X=1",
        "OSAL_FREERTOS=1",
        "core0",
        "am243x",
        "am243x_lp",
        "SSC_CHECKTIMER=1",
        "USE_ECAT_TIMER=1",
    
    ],
};

const cflags_r5f = {
    common: [
       
        "-Wno-unused-but-set-variable",
     
    ],
    debug: [
       
        "-O0",
     
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
    
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-evm", os: "freertos"},
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-lp", os: "freertos"},
      
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "ethercat_slave_ctt_demo";
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
