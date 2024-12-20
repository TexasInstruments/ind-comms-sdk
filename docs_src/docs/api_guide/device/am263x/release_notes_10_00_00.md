# Release Notes 10.00.00 {#RELEASE_NOTES_10_00_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\attention For release notes of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/RELEASE_NOTES_10_00_00_PAGE.html, @VAR_SOC_NAME MCU+ SDK Release Notes 10.00.00}.

\note The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless noted otherwise, the SW modules would work on all supported EVMs \n

## New in this Release

<table>
<tr>
    <th> Feature
</tr>
<tr>
    <td> Bug Fixes
</tr>
</table>

## Device and Validation Information

SOC    | Supported CPUs  | Boards                                                               | Host PC
-------|-----------------|----------------------------------------------------------------------|-----------------------------------
AM263x | R5F             | AM263x GP CC (referred to as am263x-cc in code)                      | Windows 10 64b or Ubuntu 18.04 64b
AM263x | R5F             | AM263x LAUNCHPAD (referred to as am263x-lp in code)                  | Windows 10 64b or Ubuntu 18.04 64b

## Tools, Compiler and Other Open Source SW Module Information

Tools / SW module       | Supported CPUs | Version
------------------------|----------------|-----------------------
Code Composer Studio    | R5F            | 12.8.0
SysConfig               | R5F            | 1.21.0 build, build 3721
TI ARM CLANG            | R5F            | 4.0.0.LTS
FreeRTOS Kernel         | R5F            | 10.4.3
LwIP                    | R5F            | STABLE-2_2_0_RELEASE
Mbed-TLS                | R5F            | mbedtls-3.0.0

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
    <td> PINDSW-47
    <td> Single datagram accessing multiple FMMU mapped areas using LRD/LWR commands from a single SubDevice.
    <td> EtherCAT Device
    <td> 09.02.00
    <td>
</tr>
<tr>
    <td> PINDSW-141
    <td> LRW access to non-interleaved input and output process data of multiple SubDevices does not work.
    <td> EtherCAT Device
    <td> 09.02.00
    <td>
</tr>
<tr>
    <td> PINDSW-8115
    <td> Watchdog error while using LRD and LWR with same logical address.
    <td> EtherCAT Device
    <td> 09.02.00
    <td>
</tr>
<tr>
    <td> PINDSW-8246
    <td> Triple buffer issue - Not getting the latest data from buffer during free-run mode.
    <td> EtherCAT Device
    <td> 09.02.00
    <td>
</tr>
<tr>
    <td> PINDSW-8060
    <td> Reduce Auto-forward (reverse path) delay.
    <td> EtherCAT Device
    <td> 09.02.00
    <td> Optimization in latency.
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


## Upgrade and Compatibility Information {#UPGRADE_AND_COMPATIBILITY_INFORMATION_10_0_0}

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
    <td> -
    <td> -
    <td> -
    <td> -
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
    <td> -
    <td> -
    <td> -
    <td> -
</tr>
</table>

