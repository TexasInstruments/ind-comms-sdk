# Release Notes 09.01.00 {#RELEASE_NOTES_09_01_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\attention For release notes of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/RELEASE_NOTES_09_01_00_PAGE.html, @VAR_SOC_NAME MCU+ SDK Release Notes 09.01.00}.

\note The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless noted otherwise, the SW modules would work on all supported EVMs \n

## New in this Release

<table>
<tr>
    <th> Feature
</tr>
<tr>
    <td> Refactoring of EtherCAT API
</tr>
<tr>
    <td> Improvements in Discrete I/O Device example 
</tr>
<tr>
    <td> Ethernet IP Conformance Tool v20 Support
</tr>
<tr>
    <td> ICSSM Ethernet/IP FW supports additional MAC address for traffic routing
</tr>
<tr>
    <td> IOLINK Improvements - Added NVRAM support for AM243 LP and EVM in example app. Support for flexible Maximum number of Ports.
</tr>
<tr>
    <td> Moved ICSSG ENET LLD examples back to MCU SDK
</tr>
<tr>
    <td> Bug Fixes
</tr>
</table>

## Device and Validation Information

SOC    | Supported CPUs  | Boards                                                                                                      | Host PC
-------|-----------------|-------------------------------------------------------------------------------------------------------------|-----------------------------------
AM64x  | R5F             | AM64x PG2 EVM (referred to as am64x-evm in code)                                                             | Windows 10 64b or Ubuntu 18.04 64b

## Tools, Compiler and Other Open Source SW Module Information

Tools / SW module       | Version
------------------------|-----------------------
Code Composer Studio    | @VAR_CCS_VERSION
SysConfig               | @VAR_SYSCFG_VERSION, build @VAR_SYSCFG_BUILD
TI ARM CLANG            | @VAR_TI_ARM_CLANG_VERSION
MCU+ SDK                |  9.1.0

\attention TI ARM CLANG @VAR_TI_ARM_CLANG_VERSION is not part of CCS by default, Follow steps at \htmllink{@VAR_MCU_SDK_DOCS_PATH/SDK_DOWNLOAD_PAGE.html#INSTALL_TIARMCLANG, TI CLANG Compiler Toolchain} to install the compiler.

## Key Features

<!-- ### Features not supported in release -->
Profinet Device Stack and example. For more information, see Profinet Stack Transition

## Fixed Issues

<table>
<tr>
    <th> ID
    <th> Head Line
    <th> Module
    <th> Applicable Releases
    <th> Resolution/Comments
</tr>
<tr>
    <td> PINDSW-5668
    <td> HSR/PRP is not functional in rgmii mode
    <td> HSR-PRP
    <td> 08.04.00
    <td> 
</tr>
<tr>
    <td> PINDSW-5669
    <td> Read access for 0x0984 Register not enabled
    <td> EtherCAT Device
    <td> 08.06.00
    <td> 
</tr>
<tr>
    <td> PINDSW-5675
    <td> HSR/PRP - PTP Device is unable to keep offset under 1000 ns
    <td> HSR-PRP
    <td> 08.04.00
    <td> 
</tr>
<tr>
    <td> PINDSW-6644
    <td> EtherCAT - Disable Link Polling Mode Support in MDIO Workaround firmware
    <td> EtherCAT Device
    <td> 08.06.00
    <td> 
</tr>
<tr>
    <td> PINDSW-6904
    <td> Change of IP configuration does not work
    <td> EtherNet/IP Adapter
    <td> 09.00.00
    <td> 
</tr>
<tr>
    <td> PINDSW-7099
    <td> Reg.0x805.bit1 remains 0 and does not change
    <td> EtherCAT Device
    <td> 08.05.00
    <td> 
</tr>
</table>

## Known Issues

<table>
<tr>
    <th> ID
    <th> Head Line
    <th> Module
    <th> Applicable Releases
    <th> Workaround
</tr>
<tr>
    <td> PINDSW-5122
    <td> DLR does not work in RGMII mode
    <td> EtherNet/IP Adapter
    <td> 08.04.00
    <td> 
</tr>
<tr>
    <td> PINDSW-5508
    <td> Observed "tcp_err_handler: Connection aborted (-13) while running CTT
    <td> EtherNet/IP Adapter
    <td> 08.06.00
    <td> 
</tr>
<tr>
    <td> PINDSW-5666
    <td> EtherNet/IP : PTP Device is unable to keep offset under 1000 ns
    <td> EtherNet/IP Adapter
    <td> 08.04.00
    <td> Value of OFFSET_THRESHOLD_FOR_RESET is set to 10000 ns by default in SDK
</tr>
<tr>
    <td> PINDSW-6587
    <td> EtherCAT syscfg - Rx MLINK enable does not enables MLINK if it is disabled from application
    <td> EtherCAT Device
    <td> 08.06.00
    <td> 
</tr>
</table>

<!-- ## Errata
<table>
<tr>
    <th> ID
    <th> Head Line
    <th> Module
    <th> SDK Status
</tr>
<tr>
    <td> -
    <td> -
    <td> -
    <td> -
</tr>
</table> -->

## Upgrade and Compatibility Information {#UPGRADE_AND_COMPATIBILITY_INFORMATION_9_1_0}

<!-- \attention When migrating from MCU+ SDK, see \ref MIGRATION_GUIDES for more details -->

This section lists changes which could affect user applications developed using older SDK versions.
Read this carefully to see if you need to do any changes in your existing application when migrating to this SDK version relative to
previous SDK version. Also refer to older SDK version release notes to see changes in
earlier SDKs.

<!-- ### Compiler Options

<table>
<tr>
    <th> Module
    <th> Affected API
    <th> Change
    <th> Additional Remarks
</tr>
<tr>
    <td> 
    <td> 
    <td> 
    <td> 
</tr>
</table> -->

### Examples

<table>
<tr>
    <th> Module
    <th> Affected API
    <th> Change
    <th> Additional Remarks
</tr>
<tr>
    <td> EtherCAT Device
    <td> <a href="../am64x/ethercat_slave/_a_p_i_changes.html" target="_blank"> Refer Ethercat Example documentation </a>
    <td> <a href="../am64x/ethercat_slave/_a_p_i_changes.html" target="_blank"> Refer Ethercat Example documentation </a>
    <td> 
</tr>
</table>

### Drivers

<table>
<tr>
    <th> Module
    <th> Affected API
    <th> Change
    <th> Additional Remarks
</tr>
<tr>
    <td> 
    <td> 
    <td> 
    <td> 
</tr>
</table>
