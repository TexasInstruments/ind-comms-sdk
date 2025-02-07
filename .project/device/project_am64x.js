const common = require("../common.js");

const component_file_list = [
    "source/industrial_comms/ethercat_slave/icss_fwhal/.project/project.js",
    "source/industrial_comms/ethercat_slave/stack/patch/.project/project.js",
    "source/industrial_comms/ethernetip_adapter/icss_fwhal/.project/project_mii.js",
    "source/industrial_comms/ethernetip_adapter/icss_fwhal/.project/project_rgmii.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip/.project/project_contrib.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip/.project/project_stack.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip_ic/.project/project_contrib.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip_ic/.project/project_stack.js",
    "source/industrial_comms/ethernetip_adapter/stack/lwip_ic/.project/project_lwip_ic.js",
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
    "docs_src/docs/api_guide/doxy_samples/.project/project.js",
];

const device_defines = {
    common: [
        "SOC_AM64X",
    ],
};

const example_file_list = [
    "examples/industrial_comms/ethercat_iolink_gateway_demo/.project/project.js",
    "examples/industrial_comms/ethercat_slave_beckhoff_ssc_demo/.project/project.js",
    "examples/industrial_comms/ethercat_subdevice_demo/device_profiles/402_cia/.project/project.js",
    "examples/industrial_comms/ethercat_subdevice_demo/device_profiles/ctt/.project/project.js",
    "examples/industrial_comms/ethercat_subdevice_demo/device_profiles/401_simple/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/device_profiles/generic_device/standard/mii/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/device_profiles/generic_device/standard/rgmii/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/device_profiles/discrete_io_device/standard/mii/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/device_profiles/discrete_io_device/standard/rgmii/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/device_profiles/generic_device/tunneling/mii/.project/project.js",
    "examples/industrial_comms/ethernetip_adapter_demo/device_profiles/generic_device/tunneling/rgmii/.project/project.js",
    "examples/industrial_comms/hsr_prp_demo/hsr_mii/.project/project.js",
    "examples/industrial_comms/hsr_prp_demo/hsr_rgmii/.project/project.js",
    "examples/industrial_comms/hsr_prp_demo/prp_mii/.project/project.js",
    "examples/industrial_comms/hsr_prp_demo/prp_rgmii/.project/project.js",
    "examples/industrial_comms/iolink_master_demo/example/.project/project.js",
    "examples/industrial_comms/profinet_device_demo/rt_mii/.project/project.js",
    "source/industrial_comms/mdio_fw/.project/project.js",
];

function getProjectSpecCpu(cpu) {
    let projectSpecCpu =
    {
        "r5fss0-0": "MAIN_PULSAR_Cortex_R5_0_0",
        "r5fss0-1": "MAIN_PULSAR_Cortex_R5_0_1",
        "r5fss1-0": "MAIN_PULSAR_Cortex_R5_1_0",
        "r5fss1-1": "MAIN_PULSAR_Cortex_R5_1_1",
        "m4fss0-0": "Cortex_M4F_0",
        "a53ss0-0": "CortexA53_0",
        "icss_g0_pru0": "ICSS_G0_PRU_0",
        "icss_g0_pru1": "ICSS_G0_PRU_1",
        "icss_g0_rtu_pru0": "ICSS_G0_RTU_PRU_0",
        "icss_g0_rtu_pru1": "ICSS_G0_RTU_PRU_1",
        "icss_g0_tx_pru0": "ICSS_G0_TX_PRU_0",
        "icss_g0_tx_pru1": "ICSS_G0_TX_PRU_1",
        "icss_g1_pru0": "ICSS_G1_PRU_0",
        "icss_g1_pru1": "ICSS_G1_PRU_1",
        "icss_g1_rtu_pru0": "ICSS_G1_RTU_PRU_0",
        "icss_g1_rtu_pru1": "ICSS_G1_RTU_PRU_1",
        "icss_g1_tx_pru0": "ICSS_G1_TX_PRU_0",
        "icss_g1_tx_pru1": "ICSS_G1_TX_PRU_1",
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
    return "AM64x";
}

function getProjectSpecDevice(board) {
    return "AM64x";
}

function getSysCfgCpu(cpu) {
    return cpu;
}

function getSysCfgPkg(board) {
    return "ALV";
}

function getSysCfgPart(board) {
    return "Default";
}

function getDevToolTirex(board) {
    switch (board) {
        case "am64x-sk":
            return "SK-AM64B";
        default:
        case "am64x-evm":
            return "TMDS64EVM";
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
            return "am64-main-r5f0_0-fw";
        case "r5fss0-1":
            return "am64-main-r5f0_1-fw";
        case "r5fss1-0":
            return "am64-main-r5f1_0-fw";
        case "r5fss1-1":
            return "am64-main-r5f1_1-fw";
        case "m4fss0-0":
            return "am64-mcu-m4f0_0-fw";
    }
    return undefined;
}

function getProductNameProjectSpec() {
    return "INDUSTRIAL_COMMUNICATIONS_SDK_AM64X";
}

function getFlashAddr() {
    return 0x60000000;
}

function getEnableGccBuild() {
    const IsGccBuildEnabled = 0;
    return IsGccBuildEnabled;
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
    getEnableGccBuild,
};
