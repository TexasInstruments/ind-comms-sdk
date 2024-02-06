const common = require("../common.js");

const component_file_list = [
    "source/industrial_comms/ethercat_slave/icss_fwhal/.project/project.js",
    "source/networking/icss_emac/.project/project.js",
    "docs_src/docs/api_guide/doxy_samples/.project/project.js",
];

const device_defines = {
    common: [
        "SOC_AM263X",
    ],
};

const example_file_list = [
    "examples/industrial_comms/ethercat_slave_beckhoff_ssc_demo/.project/project.js",
];

function getProjectSpecCpu(cpu) {
    let projectSpecCpu =
    {
        "r5fss0-0": "Cortex_R5_0",
        "r5fss0-1": "Cortex_R5_1",
        "r5fss1-0": "Cortex_R5_2",
        "r5fss1-1": "Cortex_R5_3",
        "icssm-pru0": "ICSSM_PRU_0",
        "icssm-pru1": "ICSSM_PRU_1",
    }

    return projectSpecCpu[cpu];
}

function getComponentList() {
    return component_file_list;
}

function getExampleList() {
    return example_file_list;
}

function getSysCfgDevice(board) {
    switch (board) {
        case "am263x-lp":
            return "AM263x_beta";
        default:
        case "am263x-cc":
            return "AM263x_beta";
    }
}

function getProjectSpecDevice(board) {
    switch (board) {
        case "am263x":
            return "AM263x";
        case "am263x-lp":
            return "AM263x";
        default:
        case "am263x-cc":
            return "AM263x";
    }
}

function getSysCfgCpu(cpu) {
    return cpu;
}

function getSysCfgPkg(board) {
    switch (board) {
        case "am263x-lp":
            return "ZCZ";
        default:
        case "am263x-cc":
            return "ZCZ";
    }
}

function getSysCfgPart(board) {
    switch (board) {
        case "am263x-lp":
            return "AM263x";
        default:
        case "am263x-cc":
            return "AM263x";
    }
}

function getDevToolTirex(board) {
    switch (board) {
        case "am263x-lp":
            return "LP-AM263";
        default:
        case "am263x-cc":
            return "TMDSCNCD263";
    }
}

function getProperty() {
    let property = {};

    property.defines = device_defines;

    return property;
}

function getLinuxFwName(cpu) {

    switch(cpu) {
        case "r5fss0-0":
            return "am263-main-r5f0_0-fw";
        case "r5fss0-1":
            return "am263-main-r5f0_1-fw";
        case "r5fss1-0":
            return "am263-main-r5f1_0-fw";
        case "r5fss1-1":
            return "am263-main-r5f1_1-fw";
    }
    return undefined;
}

function getProductNameProjectSpec() {
    return "INDUSTRIAL_COMMUNICATIONS_SDK_AM263X";
}

function getFlashAddr() {
    return 0x60000000;
}

module.exports = {
    getComponentList,
    getExampleList,
    getSysCfgDevice,
    getSysCfgCpu,
    getSysCfgPkg,
    getSysCfgPart,
    getProjectSpecDevice,
    getProjectSpecCpu,
    getDevToolTirex,
    getProperty,
    getLinuxFwName,
    getProductNameProjectSpec,
    getFlashAddr,
};
