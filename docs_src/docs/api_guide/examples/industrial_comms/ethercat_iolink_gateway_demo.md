# EtherCAT-IOLink Gateway Demo{#EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_IOLINK_GATEWAY_DEMO}

[TOC]

# Introduction

This example is an EtherCAT-IOLink Gateway application based on evaluation stack provided in the SDK. Links to important sections in the detailed documentation are provided below:

\cond SOC_AM64X

- <a href="../am64x/ethercat_iolink_gateway/index.html" target="_blank"> Home Page </a>
- <a href="../am64x/ethercat_iolink_gateway/ethercat_datasheet.html" target="_blank"> Data Sheet </a>
- <a href="../am64x/ethercat_iolink_gateway/releasenotes.html" target="_blank"> Release Notes </a>
- <a href="../am64x/ethercat_iolink_gateway/usergroup0.html" target="_blank"> Example Quick Start </a>
- <a href="../am64x/ethercat_iolink_gateway/modules.html" target="_blank"> API Documentation </a>

\endcond

\cond SOC_AM243X

## @VAR_BOARD_NAME

- <a href="../am243x/ethercat_iolink_gateway/evm/index.html" target="_blank"> Home Page </a>
- <a href="../am243x/ethercat_iolink_gateway/evm/ethercat_datasheet.html" target="_blank"> Data Sheet </a>
- <a href="../am243x/ethercat_iolink_gateway/evm/releasenotes.html" target="_blank"> Release Notes </a>
- <a href="../am243x/ethercat_iolink_gateway/evm/usergroup0.html" target="_blank"> Example Quick Start </a>
- <a href="../am243x/ethercat_iolink_gateway/evm/modules.html" target="_blank"> API Documentation </a>

## @VAR_LP_BOARD_NAME

- <a href="../am243x/ethercat_iolink_gateway/lp/index.html" target="_blank"> Home Page </a>
- <a href="../am243x/ethercat_iolink_gateway/lp/ethercat_datasheet.html" target="_blank"> Data Sheet </a>
- <a href="../am243x/ethercat_iolink_gateway/lp/releasenotes.html" target="_blank"> Release Notes </a>
- <a href="../am243x/ethercat_iolink_gateway/lp/usergroup0.html" target="_blank"> Example Quick Start </a>
- <a href="../am243x/ethercat_iolink_gateway/lp/modules.html" target="_blank"> API Documentation </a>

\endcond

\note
Fast link detection using RX_LINK pins (MLINK mode) is required to support complete EtherCAT functionality, and certain conformance tests will fail if fast link detection is not used. Link Polling is not suggested, use it only for debugging if required. 

\attention If you need to reload and run the example again, EVM power-cycle is MUST.

# Supported Combinations

\cond SOC_AM64X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ICSSG          | ICSSG0 and ICSSG1
 Toolchain      | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER
 Example folder | examples/industrial_comms/ethercat_iolink_gateway_demo

\endcond

\cond SOC_AM243X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ICSSG          | ICSSG0 and ICSSG1
 Toolchain      | ti-arm-clang
 Boards         | @VAR_BOARD_NAME_LOWER, @VAR_LP_BOARD_NAME_LOWER (E1/E2 Revision)
 Example folder | examples/industrial_comms/ethercat_iolink_gateway_demo

\endcond
