let path = require('path');

let device = "am64x";

const files = {
    common: [
        "ssc.c",
        "ssc_Isr.c",
        "ssc_kbStack.c",
        "ssc_backend.c",
        "aoeappl.c",
        "ecatappl.c",
        "coeappl.c",
        "eoeappl.c",
        "foeappl.c",
        "objdef.c",
        "ecatslv.c",
        "emcy.c",
        "diag.c",
        "sdoserv.c",
        "mailbox.c",
        "ecataoe.c",
        "ecatcoe.c",
        "ecateoe.c",
        "ecatfoe.c",
    ],
};

/* Relative to where the makefile will be generated */
const filedirs = {
    common: [
        "src",
        "SlaveFiles/src",
    ],
};

const includes = {
    common: [
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/common/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/patch/inc",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/patch/src",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source/industrial_comms/ethercat_slave/stack/patch/SlaveFiles/src",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/source",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source/kernel/freertos/config/am64x/r5f",
        "${INDUSTRIAL_COMMUNICATIONS_SDK_PATH}/mcu_plus_sdk/source",
    ],
};

const defines = {
    common: [
        "KUNBUS_STACK_APPLICATION=1",
        "BKHFSSC_EXPORTS",
        "SSC_CHECKTIMER=1",
        "USE_ECAT_TIMER=1",
        "PRUICSS_ETHERCAT_SUPPORT",
	"OSAL_FREERTOS=1",
    ],
};

const cflags = {
    common: [
        "-Wno-self-assign",
        "-Wno-parentheses-equality",
        "-Wno-tautological-constant-out-of-range-compare",
        "-Wno-address-of-packed-member",
        "-Wno-tautological-pointer-compare",
        "-mno-unaligned-access",
    ],
};

const buildOptionCombos = [
    { device: device, cpu: "r5f", cgt: "ti-arm-clang"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "library";
    property.name = "ethercat_slave_bkhf_ssc";
    property.isInternal = false;
    property.isSkipTopLevelBuild = true;
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    build_property.files = files;
    build_property.filedirs = filedirs;
    build_property.cflags = cflags;
    build_property.defines = defines;
    build_property.includes = includes;

    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
