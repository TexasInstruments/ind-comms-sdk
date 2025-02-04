const common = require("../common.js");

const component_file_list = [
    "source/industrial_comms/ethercat_slave/icss_fwhal/.project/project.js",
    "source/industrial_comms/ethercat_slave/stack/patch/.project/project.js",
    "source/industrial_comms/ethernetip_adapter/icss_fwhal/.project/project_mii.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip/.project/project_contrib.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip/.project/project_stack.js",
    "source/industrial_comms/profinet_device/icss_fwhal/.project/project_irt_mii.js",
    "source/industrial_comms/profinet_device/icss_fwhal/.project/project_rt_mrp_mii.js",
    "docs_src/docs/api_guide/doxy_samples/.project/project.js",
];

const device_defines = {
    common: [
        "SOC_AM261X",
    ],
};

const example_file_list = [
    "examples/industrial_comms/ethercat_slave_beckhoff_ssc_demo/.project/project.js",
    "examples/industrial_comms/ethercat_slave_demo/device_profiles/402_cia/.project/project.js",
    "examples/industrial_comms/ethercat_slave_demo/device_profiles/ctt/.project/project.js",
    "examples/industrial_comms/ethercat_slave_demo/device_profiles/401_simple/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/device_profiles/generic_device/standard/mii/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/device_profiles/discrete_io_device/standard/mii/.project/project.js",
    "examples/industrial_comms/iolink_master_demo/example/.project/project.js",
];

function getProjectSpecCpu(cpu) {
    let projectSpecCpu =
    {
        "r5fss0-0": "Cortex_R5_0",
        "r5fss0-1": "Cortex_R5_1",
        "icss_m0_pru0": "ICSS_M0_PRU_0",
        "icss_m0_pru1": "ICSS_M0_PRU_1",
        "icss_m1_pru0": "ICSS_M1_PRU_0",
        "icss_m1_pru1": "ICSS_M1_PRU_1",
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
        case "am261x-lp":
            return "AM261x_ZFG";
        case "am261x-lp (dp83826 phy)":
            return "AM261x_ZFG";
        case "am261x-lp (dp83tg720 phy)":
            return "AM261x_ZFG";
        case "am261x-som":
	default:
            return "AM261x_ZCZ";
    }
}

function getProjectSpecDevice(board) {
    switch (board) {
        case "am261x":
            return "AM261x";
		case "am261x-lp":
			return "AM261x";
        case "am261x-lp (dp83826 phy)":
            return "AM261x";
        case "am261x-lp (dp83tg720 phy)":
            return "AM261x";
        case "am261x-som":
        default:
            return "AM261x";
    }
}

function getSysCfgCpu(cpu) {
    return cpu;
}

function getSysCfgPkg(board) {
    switch (board) {
        case "am261x-lp":
            return "ZFG";
        case "am261x-lp (dp83826 phy)":
            return "ZFG";
        case "am261x-lp (dp83tg720 phy)":
            return "ZFG";
        case "am261x-som":
        default:
            return "ZCZ";
    }
}

function getSysCfgPart(board) {
    switch (board) {
        case "am261x-lp":
            return "AM2612";
        case "am261x-lp (dp83826 phy)":
            return "AM2612";
        case "am261x-lp (dp83tg720 phy)":
            return "AM2612";
        case "am261x-som":
        default:
            return "AM2611";
    }
}

function getDevToolTirex(board) {
	switch (board) {
		case "am261x-lp":
			return "LP-AM263P";
		case "am261x-lp (dp83826 phy)":
			return "LP-AM263P";
		case "am261x-lp (dp83tg720 phy)":
			return "LP-AM263P";
		case "am261x-som":
		default:
			return "TMDSCNCD263P";
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
            return "am261-main-r5f0_0-fw";
        case "r5fss0-1":
            return "am261-main-r5f0_1-fw";
        case "r5fss1-0":
            return "am261-main-r5f1_0-fw";
        case "r5fss1-1":
            return "am261-main-r5f1_1-fw";
    }
    return undefined;
}

function getProductNameProjectSpec() {
    return "INDUSTRIAL_COMMUNICATIONS_SDK_AM261X";
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
