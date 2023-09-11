# Release Notes 09.00.00 {#RELEASE_NOTES_09_00_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\attention For release notes of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/RELEASE_NOTES_09_00_00_PAGE.html, @VAR_SOC_NAME MCU+ SDK Release Notes 09.00.00}.

\note The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless noted otherwise, the SW modules would work on all supported EVMs \n

## New in this Release

<table>
<tr>
    <th> Feature
</tr>
<tr>
    <td> IOLink v1.1.3 Support
</tr>
<tr>
    <td> Ethernet/IP Discrete IO example added
</tr>
<tr>
    <td> ICSSM Profinet FW supports four MAC address
</tr>
<tr>
    <td> ICSSG Layer 2 example (\ref EXAMPLES_ENET_LAYER2_ICSSG) is divided in to two new examples : ICSSG Layer 2 dual MAC and ICSSG Layer 2 Switch examples.
</tr>
<tr>
    <td> Enet PHY loopback for ICSSG on NoRTOS (baremetal) is added
</tr>
</table>

## Device and Validation Information

SOC    | Supported CPUs  | Boards                                                                                                      | Host PC
-------|-----------------|-------------------------------------------------------------------------------------------------------------|-----------------------------------
AM243x | R5F             | AM243x GP EVM (referred to as am243x-evm in code), \n AM243x LAUNCHPAD (referred to as am243x-lp in code)   | Windows 10 64b or Ubuntu 18.04 64b

## Tools, Compiler and Other Open Source SW Module Information

Tools / SW module       | Version
------------------------|-----------------------
Code Composer Studio    | @VAR_CCS_VERSION
SysConfig               | @VAR_SYSCFG_VERSION, build @VAR_SYSCFG_BUILD
TI ARM CLANG            | @VAR_TI_ARM_CLANG_VERSION
MCU+ SDK                |  9.0.0

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
    <td> PINDSW-5663
    <td> EtherCAT: General Stack Memory footprint data not updated in Documentation
    <td> EtherCAT Device
    <td> 08.06.00
    <td> 
</tr>
<tr>
    <td> PINDSW-5667
    <td> EtherCAT Slave Demo doesn't work with AM243x Launch Pad
    <td> EtherCAT Device
    <td> 08.06.00
    <td> 
</tr>
<tr>
    <td> PINDSW-5686
    <td> EtherNet/IP: LLDP forwarding issue
    <td> EtherNet/IP Adapter
    <td> 08.04.00
    <td>
</tr>
<tr>
    <td> PINDSW-6493
    <td> Add an option to change the PHY MDIO addresses in Sysconfig
    <td> ICSSG
    <td> 08.04.00
    <td>
</tr>
<tr>
    <td> PINDSW-6494
    <td> ENET: Performance Degrade Layer2 ICSSG Example
    <td> ICSSG
    <td> 08.05.00
    <td>
</tr>
<tr>
    <td> PINDSW-6635
    <td> EtherNet/IP: Conformance Test LLDP Management Object and LLDP data table
    <td> EtherNet/IP Adapter
    <td> 08.06.00
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
    <td> PINDSW-5666
    <td> EtherNet/IP : PTP Device is unable to keep offset under 1000 ns
    <td> EtherNet/IP Adapter
    <td> 08.04.00
    <td> Value of OFFSET_THRESHOLD_FOR_RESET is set to 10000 ns by default in SDK
</tr>
<tr>
    <td> PINDSW-5668
    <td> HSR/PRP is not functional in rgmii mode
    <td> HSR-PRP
    <td> 08.04.00
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
    <td> PINDSW-5781
    <td> MDIO Workaround - Polling mode does not work with EtherCAT
    <td> EtherCAT Device
    <td> 08.06.00
    <td> 
</tr>
<tr>
    <td> PINDSW-6498
    <td> ICSSG0: could not send Ethernet packets out
    <td> ICSSG
    <td> 08.04.00 onwards
    <td>
</tr>
<tr>
    <td> PINDSW-6500
    <td> DMA resource acquisition failure error occurs, when running the enet_icssg_tcpserver example on the R5FSS0_1 core
    <td> ICSSG
    <td> 08.06.00 onwards
    <td>
</tr>
<tr>
    <td> PINDSW-6587
    <td> EtherCAT syscfg - Rx MLINK enable does not enables MLINK if it is disabled from application
    <td> EtherCAT Device
    <td> 08.06.00
    <td> 
</tr>
<tr>
    <td> PINDSW-6624
    <td> AM243x: ENET: ICSSG TAS Example Failure
    <td> ICSSG
    <td> 08.06.00 onwards
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

## Upgrade and Compatibility Information {#UPGRADE_AND_COMPATIBILITY_INFORMATION_9_0_0}

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
    <td> 
    <td> 
    <td> 
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
