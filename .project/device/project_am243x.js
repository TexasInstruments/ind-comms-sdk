const common = require("../common.js");

const component_file_list = [
    "source/industrial_comms/ethercat_slave/icss_fwhal/.project/project.js",
    "source/industrial_comms/ethercat_slave/stack/patch/.project/project.js",
    "source/industrial_comms/ethernetip_adapter/icss_fwhal/.project/project_mii.js",
    "source/industrial_comms/ethernetip_adapter/icss_fwhal/.project/project_rgmii.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip/.project/project_contrib.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip/.project/project_stack.js",
    "source/industrial_comms/hsr_prp/icss_fwhal/.project/project_hsr_mii.js",
    "source/industrial_comms/hsr_prp/icss_fwhal/.project/project_hsr_rgmii.js",
    "source/industrial_comms/hsr_prp/icss_fwhal/.project/project_prp_mii.js",
    "source/industrial_comms/hsr_prp/icss_fwhal/.project/project_prp_rgmii.js",
    "source/industrial_comms/hsr_prp/stack/lwip/.project/project_contrib.js",
    "source/industrial_comms/hsr_prp/stack/lwip/.project/project_stack.js",
    "source/industrial_comms/hsr_prp/stack/lwip/.project/project_emac_lwip_if.js",
    "source/industrial_comms/profinet_device/icss_fwhal/.project/project_irt_mii.js",
    "source/industrial_comms/profinet_device/icss_fwhal/.project/project_irt_rgmii.js",
    "source/industrial_comms/profinet_device/icss_fwhal/.project/project_rt_mrp_mii.js",
    "source/industrial_comms/profinet_device/icss_fwhal/.project/project_rt_mrp_rgmii.js",
    "source/networking/icss_emac/.project/project.js",
    "source/networking/icss_emac/lwipif/.project/project.js",
    "source/networking/icss_timesync/.project/project.js",
];

const device_defines = {
    common: [
        "SOC_AM243X",
    ],
};

const example_file_list = [
    "examples/industrial_comms/ethercat_iolink_gateway_demo/.project/project.js",
    "examples/industrial_comms/ethercat_slave_beckhoff_ssc_demo/.project/project.js",
    "examples/industrial_comms/ethercat_slave_demo/cia402/.project/project.js",
    "examples/industrial_comms/ethercat_slave_demo/ctt/.project/project.js",
    "examples/industrial_comms/ethercat_slave_demo/simple/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/mii/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/rgmii/.project/project.js",
    "examples/industrial_comms/hsr_prp_demo/hsr_mii/.project/project.js",
    "examples/industrial_comms/hsr_prp_demo/hsr_rgmii/.project/project.js",
    "examples/industrial_comms/hsr_prp_demo/prp_mii/.project/project.js",
    "examples/industrial_comms/hsr_prp_demo/prp_rgmii/.project/project.js",
    "examples/industrial_comms/iolink_master_demo/.project/project.js",
    "examples/networking/enet_loopback/enet_icssg_loopback/.project/project.js",
    "examples/networking/enet_icssg_tas/.project/project.js",
    "examples/networking/enet_layer2_icssg/.project/project.js",
    "examples/networking/enet_vlan_icssg/.project/project.js",
    "examples/networking/lwip/enet_icssg_tcpserver/.project/project.js",
    "examples/networking/lwip/enet_lwip_icssg/.project/project.js",
];

function getProjectSpecCpu(cpu) {
    let projectSpecCpu =
    {
        "r5fss0-0": "MAIN_PULSAR_Cortex_R5_0_0",
        "r5fss0-1": "MAIN_PULSAR_Cortex_R5_0_1",
        "r5fss1-0": "MAIN_PULSAR_Cortex_R5_1_0",
        "r5fss1-1": "MAIN_PULSAR_Cortex_R5_1_1",
        "m4fss0-0": "Cortex_M4F_0",
        "icssg0-pru0": "ICSS_G0_PRU_0",
        "icssg0-pru1": "ICSS_G0_PRU_1",
        "icssg0-rtupru0": "ICSS_G0_RTU_PRU_0",
        "icssg0-rtupru1": "ICSS_G0_RTU_PRU_1",
        "icssg0-txpru0": "ICSS_G0_TX_PRU_0",
        "icssg0-txpru1": "ICSS_G0_TX_PRU_1",
        "icssg1-pru0": "ICSS_G1_PRU_0",
        "icssg1-pru1": "ICSS_G1_PRU_1",
        "icssg1-rtupru0": "ICSS_G1_RTU_PRU_0",
        "icssg1-rtupru1": "ICSS_G1_RTU_PRU_1",
        "icssg1-txpru0": "ICSS_G1_TX_PRU_0",
        "icssg1-txpru1": "ICSS_G1_TX_PRU_1",
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
        case "am243x-lp":
            return "AM243x_ALX_beta";
        default:
        case "am243x-evm":
            return "AM243x_ALV_beta";
    }
}

function getProjectSpecDevice(board) {
    switch (board) {
        case "am243x":
            return "AM243x";
        case "am243x-lp":
            return "AM2434_ALX";
        default:
        case "am243x-evm":
            return "AM2434_ALV";
    }
}

function getSysCfgCpu(cpu) {
    return cpu;
}

function getSysCfgPkg(board) {
    switch (board) {
        case "am243x-lp":
            return "ALX";
        default:
        case "am243x-evm":
            return "ALV";
    }
}

function getSysCfgPart(board) {
    switch (board) {
        case "am243x-lp":
            return "ALX";
        default:
        case "am243x-evm":
            return "ALV";
    }
}

function getDevToolTirex(board) {
    switch (board) {
        case "am243x-lp":
            return "AM243x_LAUNCHPAD";
        default:
        case "am243x-evm":
            return "AM243x_GP_EVM";
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
            return "am243-main-r5f0_0-fw";
        case "r5fss0-1":
            return "am243-main-r5f0_1-fw";
        case "r5fss1-0":
            return "am243-main-r5f1_0-fw";
        case "r5fss1-1":
            return "am243-main-r5f1_1-fw";
        case "m4fss0-0":
            return "am243-mcu-m4f0_0-fw";
    }
    return undefined;
}

function getProductNameProjectSpec() {
    return "INDUSTRIAL_COMMUNICATIONS_SDK_AM243X";
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
