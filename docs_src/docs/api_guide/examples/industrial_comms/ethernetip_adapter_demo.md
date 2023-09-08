# EtherNet/IP Adapter Demos{#EXAMPLES_INDUSTRIAL_COMMS_ETHERNETIP_ADAPTER_DEMOS}

[TOC]

# Introduction

These examples are EtherNet/IP Adapter applications based on evaluation stacks provided in the SDK. Links to important sections in the detailed documentation are provided below:

\cond SOC_AM64X

- \htmllink{../am64x/ethernetip_adapter/index.html, Home Page}
- \htmllink{../am64x/ethernetip_adapter/eip_datasheet.html, Data Sheet}
- \htmllink{../am64x/ethernetip_adapter/eip_releasenotes.html, Release Notes}
- \htmllink{../am64x/ethernetip_adapter/eip_quickstart.html, Example Quick Start}
- \htmllink{../am64x/ethernetip_adapter/modules.html, API Documentation}

\endcond

\cond SOC_AM243X

- \htmllink{../am243x/ethernetip_adapter/index.html, Home Page}
- \htmllink{../am243x/ethernetip_adapter/eip_datasheet.html, Data Sheet}
- \htmllink{../am243x/ethernetip_adapter/eip_releasenotes.html, Release Notes}
- \htmllink{../am243x/ethernetip_adapter/eip_quickstart.html, Example Quick Start}
- \htmllink{../am243x/ethernetip_adapter/modules.html, API Documentation}

\endcond

\attention If you need to reload and run the example again, EVM power-cycle is MUST.

# Supported Combinations

\cond SOC_AM64X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ICSSG          | ICSSG1
 Toolchain      | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER
 Example folder | examples/industrial_comms/ethernetip_adapter_demo

\endcond

\cond SOC_AM243X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ICSSG          | ICSSG1
 Toolchain      | ti-arm-clang
 Boards         | @VAR_BOARD_NAME_LOWER, @VAR_LP_BOARD_NAME_LOWER (E1/E2 Revision)
 Example folder | examples/industrial_comms/ethernetip_adapter_demo

\endcond

\note This example will not work on PG2.0 HS-FS devices.

