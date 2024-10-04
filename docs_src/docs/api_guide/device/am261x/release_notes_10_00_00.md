# Release Notes 10.00.00 {#RELEASE_NOTES_10_00_00_PAGE}

[TOC]

\attention Also refer to individual module pages for more details on each feature, unsupported features, important usage guidelines.

\attention For release notes of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/RELEASE_NOTES_10_00_00_PAGE.html, @VAR_SOC_NAME MCU+ SDK Release Notes 10.00.00}.

\note 1. The examples will show usage of SW modules and APIs on a specific CPU instance and OS combination. \n
      Unless noted otherwise, the SW modules would work on all supported EVMs \n


\note 2. Following changes are needed to ensure proper functioning of EtherCAT firmware running on PRU-ICSS. \n
            - In `C:\ti\<ccs_version>\ccs\ccs_base\emulation\gel\AM261x\AM261x.gel`, uncomment `Configure_R5F0_400MHZ` call and comment out `Configure_R5F0_500MHZ`. \n
            - In `C:\ti\<ccs_version>\ccs\ccs_base\emulation\gel\AM261x\AM261x_PLL\AM261x_Periheral_Clocks.gel`, update `Program_ICSSM0_Core_Clocks` to following: \n
                \code
                hotmenu Program_ICSSM0_Core_Clocks() \n
                {
                    Write_MMR(MSS_RCM_U_BASE+MSS_RCM_ICSSM0_CORE_CLK_DIV_VAL, 0x000);
                    Write_MMR(MSS_RCM_U_BASE+MSS_RCM_ICSSM0_CORE_CLK_SRC_SEL, 0x222);
                    while(Read_MMR(MSS_RCM_U_BASE + MSS_RCM_ICSSM0_CORE_CLK_STATUS) != 0x0004);
                    GEL_TextOut("ICSSM0_CORE Clock Enabled \n");
                }
                \endcode

\note 3. EtherCAT SubDevice Beckhoff SSC Demo has been tested for R5F at 400MHz and PRU at 200MHz. \n


## New in this Release

<table>
<tr>
    <th> Feature
</tr>
<tr>
    <td> EtherCAT SubDevice Beckhoff SSC Demo
</tr>
</table>

## Not tested in this release

- R5F at 500MHz and PRU at 225MHz

## Device and Validation Information

SOC    | Supported CPUs  | Boards                                                                                                      | Host PC
-------|-----------------|-------------------------------------------------------------------------------------------------------------|-----------------------------------
AM261x | R5F             | AM261x LaunchPad Revision E1                         (referred to as am261x-lp in code). \n                 | Windows 10 64b or Ubuntu 18.04 64b

## Tools, Compiler and Other Open Source SW Module Information

Tools / SW module       | Supported CPUs | Version
------------------------|----------------|-----------------------
Code Composer Studio    | R5F            | 12.8.0
SysConfig               | R5F            | 1.21.0, build 3721
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

