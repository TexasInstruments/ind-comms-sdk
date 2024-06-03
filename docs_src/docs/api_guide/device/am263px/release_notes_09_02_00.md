# Release Notes 09.02.00 {#RELEASE_NOTES_09_02_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\attention For release notes of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/RELEASE_NOTES_09_02_00_PAGE.html, @VAR_SOC_NAME MCU+ SDK Release Notes 09.02.00}.

\note The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless noted otherwise, the SW modules would work on all supported EVMs \n

## New in this Release

<table>
<tr>
    <th> Feature
</tr>
<tr>
    <td> EtherCAT SubDevice Beckhoff SSC Demo
</tr>
</table>

<table>
<tr>
    <th> Bug Fixes
</tr>
<tr>
    <td> PINDSW-5668
</tr>
<tr>
    <td> PINDSW-7521
</tr>
</table>

\note ICSS-EMAC driver has been migrated to MCU+ SDK for AM263Px platforms. \n

## Device and Validation Information

SOC    | Supported CPUs  | Boards                                                                                                      | Host PC
-------|-----------------|-------------------------------------------------------------------------------------------------------------|-----------------------------------
AM263Px| R5F             | AM263Px ControlCard E2 Rev                      (referred to as am263Px-cc in code). \n                     | Windows 10 64b or Ubuntu 18.04 64b
AM263Px| R5F             | AM263Px LaunchPad                               (referred to as am263Px-lp in code). \n                     | Windows 10 64b or Ubuntu 18.04 64b

## Tools, Compiler and Other Open Source SW Module Information

Tools / SW module       | Supported CPUs | Version
------------------------|----------------|-----------------------
Code Composer Studio    | R5F            | 12.7.0
SysConfig               | R5F            | 1.20.0, build 3587
TI ARM CLANG            | R5F            | 3.2.2.LTS
FreeRTOS Kernel         | R5F            | 10.4.3
LwIP                    | R5F            | STABLE-2_2_0_RELEASE
Mbed-TLS                | R5F            | mbedtls-2.13.1

## Key Features

<!-- ### Features not supported in release -->

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
    <td> -
    <td> -
</tr>
<tr>
    <td> PINDSW-7521
    <td> Drift Compensation issue for filter values other than 0
    <td> EtherCAT Device
    <td> -
    <td> -
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
    <td> -
    <td> -
    <td> -
    <td> -
    <td> -
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

