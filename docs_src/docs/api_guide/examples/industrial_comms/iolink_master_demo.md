# IO-Link Controller Demo{#EXAMPLES_INDUSTRIAL_COMMS_IOLINK_MASTER_DEMO}

[TOC]

# Introduction

This example is an IO-Link controller application based on evaluation stack provided in the SDK. Links to important sections in the detailed documentation are provided below:

\cond SOC_AM64X

- <a href="../am64x/iolink_master/index.html" target="_blank"> Home Page </a>
- <a href="../am64x/iolink_master/iolink_datasheet.html" target="_blank"> Data Sheet </a>
- <a href="../am64x/iolink_master/releasenotes.html" target="_blank"> Release Notes </a>
- <a href="../am64x/iolink_master/usergroup0.html" target="_blank"> Example Quick Start </a>
- <a href="../am64x/iolink_master/modules.html" target="_blank"> API Documentation </a>

\endcond

\cond SOC_AM243X

- \htmllink{../am243x/iolink_master/index.html" target="_blank"> Home Page </a>
- \htmllink{../am243x/iolink_master/iolink_datasheet.html" target="_blank"> Data Sheet </a>
- \htmllink{../am243x/iolink_master/releasenotes.html" target="_blank"> Release Notes </a>
- \htmllink{../am243x/iolink_master/usergroup0.html" target="_blank"> Example Quick Start </a>
- \htmllink{../am243x/iolink_master/modules.html" target="_blank"> API Documentation </a>

\endcond

IO-Link Controller GUI Tool can be downloaded from \htmllink{http://software-dl.ti.com/mcu-plus-sdk/esd/common/IOL_Master_GUI.zip, here}.

\attention If you need to reload and run the example again, EVM power-cycle is MUST.

# Supported Combinations

\cond SOC_AM64X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ICSSG          | ICSSG0
 Toolchain      | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER
 Example folder | examples/industrial_comms/iolink_master_demo

\endcond

\cond SOC_AM243X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ICSSG          | ICSSG0
 Toolchain      | ti-arm-clang
 Boards         | @VAR_BOARD_NAME_LOWER, @VAR_LP_BOARD_NAME_LOWER (E1/E2 Revision)
 Example folder | examples/industrial_comms/iolink_master_demo

\endcond
