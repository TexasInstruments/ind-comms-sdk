# EtherCAT SubDevice Demos{#EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_SLAVE_DEMOS}

[TOC]

# Introduction

These examples are EtherCAT SubDevice applications based on evaluation stacks provided in the SDK.

\cond SOC_AM64X

- <a href="../am64x/ethercat_slave/example1.html" target="_blank"> EtherCAT SubDevice Simple Demo </a>
- <a href="../am64x/ethercat_slave/example2.html" target="_blank"> EtherCAT SubDevice CiA402 Demo </a>
- <a href="../am64x/ethercat_slave/index.html" target="_blank"> EtherCAT SubDevice CTT Demo </a>

\endcond

\cond SOC_AM243X

- <a href="../am243x/ethercat_slave/example1.html" target="_blank"> EtherCAT SubDevice Simple Demo </a>
- <a href="../am243x/ethercat_slave/example2.html" target="_blank"> EtherCAT SubDevice CiA402 Demo </a>
- <a href="../am243x/ethercat_slave/index.html" target="_blank"> EtherCAT SubDevice CTT Demo </a>

\endcond

\note SDK examples use evaluation version of stack. They will run for 1 hour only. If you want an unlimited version, you need to rebuild the Beckhoff SSC Library used by the examples. Please check "${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/stack/patch/SlaveFiles/src/readme.md" for more details on how to rebuild the library.

Links to important sections in the detailed documentation are provided below:

\cond SOC_AM64X

- <a href="../am64x/ethercat_slave/index.html" target="_blank"> Home Page </a>
- <a href="../am64x/ethercat_slave/ethercat_datasheet.html" target="_blank"> Data Sheet </a>
- <a href="../am64x/ethercat_slave/ethercat_certificate.pdf" target="_blank"> Certificate </a>
- <a href="../am64x/ethercat_slave/releasenotes.html" target="_blank"> Release Notes </a>
- <a href="../am64x/ethercat_slave/page_quickstart.html" target="_blank"> Example Quick Start </a>
- <a href="../am64x/ethercat_slave/modules.html" target="_blank"> API Documentation </a>

\endcond

\cond SOC_AM243X

- <a href="../am243x/ethercat_slave/index.html" target="_blank"> Home Page </a>
- <a href="../am243x/ethercat_slave/ethercat_datasheet.html" target="_blank"> Data Sheet </a>
- <a href="../am243x/ethercat_slave/ethercat_certificate.pdf" target="_blank"> Certificate </a>
- <a href="../am243x/ethercat_slave/releasenotes.html" target="_blank"> Release Notes </a>
- <a href="../am243x/ethercat_slave/page_quickstart.html" target="_blank"> Example Quick Start </a>
- <a href="../am243x/ethercat_slave/modules.html" target="_blank"> API Documentation </a>

\endcond

\note
Fast link detection using RX_LINK pins (MLINK mode) is required to support complete EtherCAT functionality, and certain conformance tests will fail if fast link detection is not used. Link Polling is not suggested, use it only for debugging if required. 

\cond SOC_AM243X

\note

 In the E1/E2 revision of @VAR_LP_BOARD_NAME_LOWER board, there are following issues which impact enhanced link detection and RX_ER monitor by ICSSG. Therefore EtherCAT cable redundancy can not be supported in E1/E2 revision.
 - "PRG1_PRU0_GPO8" is not connected to "PRG1_CPSW_ETH1_LED_LINK" from Ethernet PHY
 - "PRG1_PRU1_GPO8" is not connected to "PRG1_CPSW_ETH2_LED_LINK" from Ethernet PHY
 - "PRG1_PRU0_GPO5" is not connected to "PRG1_CPSW_ETH1_LED_1000/RX_ER" from Ethernet PHY
 - "PRG1_PRU1_GPO5" is not connected to "PRG1_CPSW_ETH2_LED_1000/RX_ER" from Ethernet PHY

 In E3 revision, following connections are available. SDK example does not enable cable redundancy yet, but will work on E3 revision only. For running it on E1/E2 revision, few changes are required. For more details, see \ref EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_SLAVE_DEMOS_COMBOS.

\endcond

\attention If you need to reload and run the example again, EVM power-cycle is MUST.

# Supported Combinations {#EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_SLAVE_DEMOS_COMBOS}

\cond SOC_AM64X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ICSSG          | ICSSG1
 Toolchain      | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER
 Example folder | examples/industrial_comms/ethercat_slave_demo

\endcond

\cond SOC_AM243X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 freertos
 ICSSG          | ICSSG1
 Toolchain      | ti-arm-clang
 Boards         | @VAR_BOARD_NAME_LOWER, @VAR_LP_BOARD_NAME_LOWER (E3 Revision)
 Example folder | examples/industrial_comms/ethercat_slave_demo


As mentioned above, SDK example does not enable cable redundancy yet, but will work on E3 revision of @VAR_LP_BOARD_NAME_LOWER only. For running it on E1/E2 revision, following changes are needed.

- Disable the pinmux configuration for pins not available in E1/E2 revision
    - In "EtherCAT" module in SysConfig, uncheck following pins from PRU_ICSSG1_MII_G_RT.
        - MII0_RXER(PR1_MII0_RXER)
        - MII0_RXLINK(PR1_MII0_RXLINK)
        - MII1_RXER(PR1_MII1_RXER)
        - MII1_RXLINK(PR1_MII1_RXLINK)

- Handle the pin assgnment change for PHY reset
    - In "GPIO" module in SysConfig, change the pin assignment of "CONFIG_GPIO0_PHYRESET0" from "W20" to "W11".

\endcond

