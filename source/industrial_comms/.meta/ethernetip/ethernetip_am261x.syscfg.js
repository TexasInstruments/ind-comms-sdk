let common = system.getScript("/common");
let pinmux = system.getScript("/drivers/pinmux/pinmux");
let device = common.getDeviceName();

let config_data = [
    {
        name: "instance",
        displayName: "Instance",
        default: "ICSSM0",
        options: [
            {
                name: "ICSSM0",
            },
        ],
    },
    {
        name: "phyToMacInterfaceMode",
        displayName: "MII",
        default: "MII",
        options: [
            {
                name: "MII",
            },
        ],
    },
    {
        name: "phyAddr0",
        description: "Phy Address of the port 0. Value MUST be between 0 .. 31",
        displayName: "Phy Address 0",
        default: 1,
    },
    {
        name: "phyAddr1",
        description: "Phy Address of Port 1. Value MUST be between 0 .. 31",
        displayName: "Phy Address 1",
        default: 3,
    },
];

function getConfigDataArr()
{
    return config_data;
}

function getRequiredArgsIcssInstance(instance)
{
    let arr = {};

    arr.instance = instance.instance;

    return arr;
}

function getInterfaceName(peripheralName)
{
    if(peripheralName !== "")
    {
        return `PRU-ICSS0-${peripheralName}`;
    }
    else
    {
        return "PRU-ICSS0";
    }
}

function getInterfacePinList(inst, peripheralName)
{
    let interfaceName = getInterfaceName(peripheralName);
    let pinList = [];

    if(peripheralName=="IEP")
    {
        pinList.push("PR0_IEP0_EDC_SYNC_OUT0");
        pinList.push("PR0_IEP0_EDC_SYNC_OUT1");
    }
    else if(peripheralName=="MDIO")
    {
        pinList.push("PR0_MDIO0_MDIO");
        pinList.push("PR0_MDIO0_MDC");
    }
    else
    {
        pinList.push("PR0_PRU0_GPIO8");
        pinList.push("PR0_PRU0_GPIO5");
        pinList.push("PR0_PRU0_GPIO6");
        pinList.push("PR0_PRU0_GPIO4");
        pinList.push("PR0_PRU0_GPIO0");
        pinList.push("PR0_PRU0_GPIO1");
        pinList.push("PR0_PRU0_GPIO2");
        pinList.push("PR0_PRU0_GPIO3");
        pinList.push("PR0_PRU0_GPIO16");
        pinList.push("PR0_PRU0_GPIO15");
        pinList.push("PR0_PRU0_GPIO11");
        pinList.push("PR0_PRU0_GPIO12");
        pinList.push("PR0_PRU0_GPIO13");
        pinList.push("PR0_PRU0_GPIO14");
        pinList.push("PR0_PRU1_GPIO8");
        pinList.push("PR0_PRU1_GPIO5");
        pinList.push("PR0_PRU1_GPIO6");
        pinList.push("PR0_PRU1_GPIO4");
        pinList.push("PR0_PRU1_GPIO0");
        pinList.push("PR0_PRU1_GPIO1");
        pinList.push("PR0_PRU1_GPIO2");
        pinList.push("PR0_PRU1_GPIO3");
        pinList.push("PR0_PRU1_GPIO16");
        pinList.push("PR0_PRU1_GPIO15");
        pinList.push("PR0_PRU1_GPIO11");
        pinList.push("PR0_PRU1_GPIO12");
        pinList.push("PR0_PRU1_GPIO13");
        pinList.push("PR0_PRU1_GPIO14");
    }

    return pinList;
}

function getPeripheralRequirements(inst, peripheralName)
{
    let interfaceName = getInterfaceName(peripheralName);
    let pinList = getInterfacePinList(inst, peripheralName);
    let resources = [];
    let device = common.getDeviceName();

    for(let pin of pinList)
    {
        let pinResource = pinmux.getPinRequirements(interfaceName, pin);

        /* make all pins as "rx" and then override to make "rx" as false as needed  */
        pinmux.setConfigurableDefault( pinResource, "rx", true );

        resources.push( pinResource );

    }

    let peripheralRequirements = {
        name: interfaceName,
        displayName: interfaceName,
        interfaceName: interfaceName,
        resources: resources,
    };

    return peripheralRequirements;
}

function getPinmuxRequirements(inst) {

    let icssm = getPeripheralRequirements(inst, "");
    let iep = getPeripheralRequirements(inst, "IEP");
    let mdio = getPeripheralRequirements(inst, "MDIO");

    return [iep, icssm, mdio];

}
function getInterfaceNameList(inst) {

    return [
        getInterfaceName("IEP"),
        getInterfaceName("MDIO"),
        getInterfaceName(""), 
    ];
}

function getPeripheralPinNames(inst)
{
    let pinList = [];

    pinList = pinList.concat( getInterfacePinList(inst, "IEP"),
                    getInterfacePinList(inst, "MDIO"),
                    getInterfacePinList(inst, ""), 
    );
    return pinList;
}

exports = {
    getConfigDataArr,
    getRequiredArgsIcssInstance,
    getPinmuxRequirements,
    getInterfaceNameList,
    getPeripheralPinNames,
};
