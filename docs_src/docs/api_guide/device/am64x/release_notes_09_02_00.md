# Release Notes 09.02.00 {#RELEASE_NOTES_09_02_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\attention For release notes of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/RELEASE_NOTES_09_02_00_PAGE.html, @VAR_SOC_NAME MCU+ SDK Release Notes 09.02.00}.

\note The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless noted otherwise, the SW modules would work on all supported EVMs \n

\note The EtherCAT IOLINK Gateway demo support is broken in the release. Please use the demo from Industrial Communication SDK 9.1 for evaluation\n

\note Examples uses ICSS EMAC driver from Industrial Communication SDK package in 9.2 Release. This driver support will be moved to MCU SDK from 10.0 onwards\n


## New in this Release

<table>
<tr>
    <th> Feature
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
    <td> Ethernet/IP - Added example to demonstrate Inter-core Tunneling.
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
AM64x  | R5F             | AM64x PG2 EVM (referred to as am64x-evm in code)                                                             | Windows 10 64b or Ubuntu 18.04 64b

## Tools, Compiler and Other Open Source SW Module Information

Tools / SW module       | Supported CPUs | Version
------------------------|----------------|-----------------------
Code Composer Studio    | R5F, M4F, A53  | 12.7.0
SysConfig               | R5F, M4F, A53  | 1.20.0, build 3587
TI ARM CLANG            | R5F, M4F       | 3.2.2.LTS
GCC AARCH64             | A53            | 9.2-2019.12
GCC ARM                 | R5F            | 7-2017-q4-major (AM64x only)
FreeRTOS Kernel         | R5F, M4F, A53  | 10.4.3
FreeRTOS SMP Kernel     | A53            | 202110.00-SMP
Tiny USB                | R5F            | 0.14.0
LwIP                    | R5F            | STABLE-2_2_0_RELEASE
Mbed-TLS                | R5F            | mbedtls-2.13.1

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
<tr>
    <td> PINDSW-8035
    <td> EtherNet/IP Tunneling : Wrong IP and MAC address returned by DUT through ENIP Messaging
    <td> EtherNet/IP Tunneling
    <td> 09.02.00
    <td> 
</tr>
<tr>
    <td> PINDSW-8036
    <td> EtherNet/IP Tunneling : DHCP and BOOTP does not work in EIP Tunneling example
    <td> EtherNet/IP Tunneling
    <td> 09.02.00
    <td> 
</tr>
<tr>
    <td> PINDSW-8037
    <td> EtherNet/IP Tunneling : ARP messages are sent out with Source Address of ICSS EMAC MAC address when ACD is enabled
    <td> EtherNet/IP Tunneling
    <td> 09.02.00
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
