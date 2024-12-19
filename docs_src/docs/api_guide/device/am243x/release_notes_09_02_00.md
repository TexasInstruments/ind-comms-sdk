# Release Notes 09.02.00 {#RELEASE_NOTES_09_02_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\attention For release notes of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/RELEASE_NOTES_09_02_00_PAGE.html, @VAR_SOC_NAME MCU+ SDK Release Notes 09.02.00}.

\note The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless noted otherwise, the SW modules would work on all supported EVMs \n

\note Examples uses ICSS EMAC driver from Industrial Communication SDK package in 9.2 Release. This driver support will be moved to MCU SDK from 10.0 onwards\n


## New in this Release

<table>
<tr>
    <th> Feature
</tr>
<tr>
    <td> PROFINET RT/IRT support with Siemens ERTEC stack on TMDS243EVM
</tr>
<tr>
    <td> ICSSG0 instance support for EtherCAT Stack
</tr>
<tr>
    <td> Custom PHY integration reference for DP82826 and DP83869
</tr>
<tr>
    <td> Ethernet over EtherCAT EoE
</tr>
<tr>
    <td> File access over EtherCAT FoE
</tr>
<tr>
    <td> Integrate NVM library for persistent data storage - EtherCAT examples now use EEPROM and not Flash memory to store EtherCAT EEPROM data
</tr>
<tr>
    <td> Ethernet/IP - The object dictionary size should be possible to change without rebuild of stack
</tr>
<tr>
    <td> Ethernet/IP - Added functionality to enable/disable forwarding of the LLDP frames in the firmware.
</tr>
<tr>
    <td> IOLINK Controller - Added NVRAM support for AM243 LP and EVM in example app
</tr>
<tr>
    <td> Bug Fixes
</tr>
</table>
 
## Device and Validation Information

SOC    | Supported CPUs  | Boards                                                                                                      | Host PC
-------|-----------------|-------------------------------------------------------------------------------------------------------------|-----------------------------------
AM243x | R5F             | AM243x GP EVM (referred to as am243x-evm in code), \n AM243x LAUNCHPAD (referred to as am243x-lp in code)   | Windows 10 64b or Ubuntu 18.04 64b

## Tools, Compiler and Other Open Source SW Module Information

Tools / SW module       | Supported CPUs | Version
------------------------|----------------|-----------------------
Code Composer Studio    | R5F, M4F       | 12.7.0
SysConfig               | R5F, M4F       | 1.20.0, build 3587
TI ARM CLANG            | R5F, M4F       | 3.2.2.LTS
FreeRTOS Kernel         | R5F, M4F       | 10.4.3
Tiny USB                | R5F            | 0.14.0
LwIP                    | R5F            | STABLE-2_2_0_RELEASE
Mbed-TLS                | R5F            | mbedtls-2.13.1


## Features not supported in release
<table>
<tr>
    <th> Feature Not Supported in Profinet Device
</tr>
<tr>
    <td> LP-AM243 Support
</tr>
<tr>
    <td> RGMII Mode 
</tr>
</table>


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
    <td> PR0_IEP0_EDIO_DATA_IN_OUT30 is not functioning as AL event
    <td> EtherCAT Device
    <td> 09.01.00
    <td> 
</tr>
<tr>
    <td> PINDSW-7521
    <td> Drift Compensation issue for filter values other than 0
    <td> EtherCAT Device
    <td> 09.01.00
    <td> 
</tr>
<tr>
    <td> PINDSW-5122
    <td> DLR does not work in RGMII mode
    <td> EtherNet/IP Adapter
    <td> 08.04.00
    <td> 
</tr>
<tr>
    <td> PINDSW-7712
    <td> DLR : Active Supervisor IP Address gets updated regardless of Supervisor change
    <td> EtherNet/IP Adapter
    <td> 09.01.00
    <td> 
</tr>
<tr>
    <td> PINDSW-7593
    <td> Correct Host Name Attribute is not returned in TCP/IP Object in Bootp mode
    <td> EtherNet/IP Adapter
    <td> 09.01.00
    <td> 
</tr>
<tr>
    <td> PINDSW-7554
    <td> Incorrect Watchdog timer configuration for Beacon time out
    <td> EtherNet/IP Adapter
    <td> 09.01.00
    <td> 
</tr>
<tr>
    <td> PINDSW-7042
    <td> ACD - Active phase not getting restated on Second Link up event
    <td> EtherNet/IP Adapter
    <td> 09.01.00
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
<tr>
    <td> PINDSW-7788
    <td> FSU tests in ART failing occasionally
    <td> Profinet Device
    <td> N/A
    <td> Rerun the test
</tr>
<tr>
    <td> PINDSW-7790
    <td> Different Access Ways Port 2 Port testcase fails occasionally
    <td> Profinet Device
    <td> N/A
    <td> 
</tr>
<tr>
    <td> PINDSW-7791
    <td> SysRed Part 2 - Scenario 3 and 4 failing
    <td> Profinet Device
    <td> N/A
    <td> N/A - Siemens Stack Issue
</tr>
<tr>
    <td> PINDSW-7792
    <td> SysRed Port 2 Port Test Failure
    <td> Profinet Device
    <td> N/A
    <td> ART Known Issues - Bug: 8670
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

## Upgrade and Compatibility Information {#UPGRADE_AND_COMPATIBILITY_INFORMATION_9_2_0}

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
    <td> <a href="../am243x/ethercat_slave/_a_p_i_changes.html" target="_blank"> Refer Ethercat Example documentation </a>
    <td> <a href="../am243x/ethercat_slave/_a_p_i_changes.html" target="_blank"> Refer Ethercat Example documentation </a>
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
