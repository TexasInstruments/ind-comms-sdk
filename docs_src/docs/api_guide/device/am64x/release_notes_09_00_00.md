# Release Notes 09.00.00 {#RELEASE_NOTES_09_00_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\attention For release notes of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/RELEASE_NOTES_09_00_00_PAGE.html, @VAR_SOC_NAME MCU+ SDK Release Notes 09.00.00}.

\note The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless noted otherwise, the SW modules would work in both FreeRTOS and NORTOS environment. \n
      Unless noted otherwise, the SW modules would work on any of the R5F's present on the SOC. \n
      Unless noted otherwise, the SW modules would work on all supported EVMs \n

## New in this Release

Feature                                                                                         | Module
------------------------------------------------------------------------------------------------|-----------------------------------

## Device and Validation Information

SOC    | Supported CPUs  | Boards                                                                                                      | Host PC
-------|-----------------|-------------------------------------------------------------------------------------------------------------|-----------------------------------
AM64x  | R5F             | AM64x GP EVM (referred to as am64x-evm in code)                                                             | Windows 10 64b or Ubuntu 18.04 64b

## Tools, Compiler and Other Open Source SW Module Information

Tools / SW module       | Supported CPUs | Version
------------------------|----------------|-----------------------
Code Composer Studio    | R5F, M4F, A53  | @VAR_CCS_VERSION
SysConfig               | R5F, M4F, A53  | @VAR_SYSCFG_VERSION, build @VAR_SYSCFG_BUILD
TI ARM CLANG            | R5F, M4F       | @VAR_TI_ARM_CLANG_VERSION
FreeRTOS Kernel         | R5F, M4F, A53  | @VAR_FREERTOS_KERNEL_VERSION
FreeRTOS SMP Kernel     | A53            | @VAR_FREERTOS_SMP_KERNEL_VERSION

\attention TI ARM CLANG @VAR_TI_ARM_CLANG_VERSION is not part of CCS by default, Follow steps at \htmllink{@VAR_MCU_SDK_DOCS_PATH/SDK_DOWNLOAD_PAGE.html#INSTALL_TIARMCLANG, TI CLANG Compiler Toolchain} to install the compiler.

## Key Features

<!-- ### Experimental Features

\attention Features listed below are early versions and should be considered as "experimental".
\attention Users can evaluate the feature, however the feature is not fully tested at TI side.
\attention TI would not support these feature on public e2e.
\attention Experimental features will be enabled with limited examples and SW modules.


Feature                                                             | Module
--------------------------------------------------------------------|--------------------------
                                                                    |  -->

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
    <td> -
    <td> -
    <td> -
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
<tr>
    <td> -
    <td> -
    <td> -
    <td> -
</tr>
</table> -->

## Limitations
<table>
<tr>
    <th> ID
    <th> Head Line
    <th> Module
    <th> Reported in Release
    <th> Applicable Devices
    <th> Workaround
</tr>
<tr>
    <td> MCUSDK-208
    <td> gmake with -j can sometimes lock up Windows command prompt
    <td> Build
    <td> 7.3.0
    <td> AM64x, AM243x
    <td> Use bash for windows as part of git for windows or don't use -j option
</tr>
</table>

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

