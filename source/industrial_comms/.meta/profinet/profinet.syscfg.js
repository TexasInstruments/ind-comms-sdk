
let common = system.getScript("/common");
let soc = system.getScript(`/industrial_comms/profinet/profinet_${common.getSocName()}`);
let device = common.getDeviceName();
let is_mdio_workaround_device = ((device === "am64x-evm") || (device === "am243x-evm") || (device === "am243x-lp")) ? true : false;

function pinmuxRequirements(inst) {
    return soc.getPinmuxRequirements(inst);
}

function getInterfaceNameList(inst) {

    return soc.getInterfaceNameList(inst);
}

function getPeripheralPinNames(inst)
{
    return soc.getPeripheralPinNames(inst);
}

function getConfigurables()
{
    /* get 'CPU enable' configurables */
    let config = soc.getConfigDataArr();
    
    if(is_mdio_workaround_device) {
        config.push({
            name: "manualMode",
            description: "Enable MDIO Manual Mode",
            displayName: "Enable MDIO Manual Mode",
            default: true,
            onChange: (inst, ui) => {
                if(inst.manualMode) {
                    ui.mdioManualModeBaseAddr.hidden    = false;
                    ui.mdioManualModeLinkPolling.hidden = false;
                }
                else {
                    ui.mdioManualModeBaseAddr.hidden    = true;
                    ui.mdioManualModeLinkPolling.hidden = true;
                }
            },
        });
        config.push({
            name: "mdioManualModeBaseAddr",
            displayName: "MDIO Manual Mode Base Address",
            default: 0x0001FF00,
            readOnly: true,
            displayFormat: "hex",
        });
        config.push({
            name: "mdioManualModeLinkPolling",
            displayName: "MDIO Manual Mode Link Status Update",
            default: "Polling",
            options: [
                {
                    name: "MLINK",
                    displayName: "MLINK Based",
                    description: "In this MLINK pins for getting link status updates from the PHY",
                },
                {
                    name: "Polling",
                    displayName: "PHY Polling Based",
                    description: "In this MDIO workaround FW Polls the PHY register for link status",
                },
            ],
        });
    }

    return config;
}

let profinet_module_name = "/industrial_comms/profinet/profinet";

let profinet_module = {

    displayName: "Profinet",
    templates: {
        "/drivers/pinmux/pinmux_config.c.xdt": {
            moduleName: profinet_module_name,
        },
        "/drivers/system/system_config.h.xdt": {
            driver_config: "/industrial_comms/profinet/templates/profinet.h.xdt",
        },
    },
    defaultInstanceName: "CONFIG_PROFINET",
    config: getConfigurables(),
    moduleStatic: {
        modules: function(inst) {
            return [{
                name: "system_common",
                moduleName: "/system_common",
            }]
        },
    },
    pinmuxRequirements,
    getInterfaceNameList,
    getPeripheralPinNames,
    sharedModuleInstances: sharedModuleInstances,
    moduleInstances: moduleInstances,
};

function sharedModuleInstances(instance) {
    let modInstances = new Array();
    let icssRequiredArgs = soc.getRequiredArgsIcssInstance(instance);

    modInstances.push({
        name: "icss",
        displayName: "PRU Configuration",
        moduleName: '/drivers/pruicss/pruicss',
        requiredArgs: icssRequiredArgs,
    });

    return (modInstances);
}

function moduleInstances(instance) {
    let modInstances = new Array();

    modInstances.push(
    {
        name: "icss_emac",
        displayName: "ICSS-EMAC Configuration (SWITCH)",
        moduleName: '/networking/icss_emac/icss_emac',
        useArray: true,
        maxInstanceCount: 1,
        minInstanceCount: 1,
        defaultInstanceCount: 1,
        requiredArgs: {
            instance: instance.instance,
            mode: "SWITCH",
            phyAddr0: instance.phyAddr0,
            phyAddr1: instance.phyAddr1,
            phyToMacInterfaceMode: instance.phyToMacInterfaceMode,
        },
    }
    );

    modInstances.push(
        {
            name: "ethphy1",
            displayName: "ETHPHY Configuration (Port 1)",
            moduleName: '/board/ethphy/ethphy',
            useArray: true,
            maxInstanceCount: 1,
            minInstanceCount: 1,
            defaultInstanceCount: 1,
            requiredArgs: (() => {
                if(is_mdio_workaround_device) {
                    return  {
                                mdioInstance: instance.instance,
                                mdioPort: instance.phyAddr0,
                                manualMode: instance.manualMode,
                                mdioManualModeBaseAddr: instance.mdioManualModeBaseAddr,
                                mdioManualModeLinkPolling: instance.mdioManualModeLinkPolling,
                            };
                } else {
                    return  {
                                mdioInstance: instance.instance,
                                mdioPort: instance.phyAddr0,
                            };
                }
            })(),
        },
        {
            name: "ethphy2",
            displayName: "ETHPHY Configuration (Port 2)",
            moduleName: '/board/ethphy/ethphy',
            useArray: true,
            maxInstanceCount: 1,
            minInstanceCount: 1,
            defaultInstanceCount: 1,
            requiredArgs: (() => {
                if(is_mdio_workaround_device) {
                    return  {
                                mdioInstance: instance.instance,
                                mdioPort: instance.phyAddr1,
                                manualMode: instance.manualMode,
                                mdioManualModeBaseAddr: instance.mdioManualModeBaseAddr,
                                mdioManualModeLinkPolling: instance.mdioManualModeLinkPolling,
                            };
                } else {
                    return  {
                                mdioInstance: instance.instance,
                                mdioPort: instance.phyAddr1,
                            };
                }
            })(),
        },
    );

    return (modInstances);
}

exports = profinet_module;
