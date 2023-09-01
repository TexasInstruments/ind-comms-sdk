let path = require('path');

const files = {
    common: [
        "Icss_emac_sample.c",
        "Icss_timesync_sample.c",
        "Pruicss_sample_g_v0.c",
    ],
};

const files_am263x = {
    common: [
    ],
};

const filedirs = {
    common: [
        "drivers",
        "networking",
    ],
};

const filedirs_am263x = {
    common: [
    ],
};

const buildOptionCombos = [
    { device: device,  cpu: "r5f", cgt: "ti-arm-clang"},
];

const buildOptionCombos_am62x = [
    { device: device,  cpu: "m4f", cgt: "ti-arm-clang"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "library";
    property.name = "doxy_samples";
    property.isInternal = true;
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    if(buildOption.device=="am263x")
    {
        build_property.files = files_am263x;
        build_property.filedirs = filedirs_am263x;
    }
    if(buildOption.device=="am64x" || buildOption.device=="am243x")
    {
        build_property.files = files;
        build_property.filedirs = filedirs;
    }

    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
