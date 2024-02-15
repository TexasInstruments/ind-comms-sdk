
let common = system.getScript("/common");

const topModules_main = [
    "/networking/icss_emac/icss_emac",
];

const topModules_mcu = [

];

const topModules_a53 = [

];

const driverVer = {
    "enet_cpsw": {
        version: "v0",
    },
}

exports = {
    getTopModules: function() {

        let topModules = topModules_main;

        if(common.getSelfSysCfgCoreName().includes("m4f")) {
            topModules = topModules_mcu;
        }
        if (common.getSelfSysCfgCoreName().match(/a53*/))
        {
            topModules = topModules_a53;
        }

        return topModules;
    },
    getDriverVer: function(driverName) {
        return driverVer[driverName].version;
    },
};
