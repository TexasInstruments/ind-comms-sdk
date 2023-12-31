# Introduction {#mainpage}

[TOC]

Welcome to **@VAR_SDK_NAME for @VAR_SOC_NAME**. This SDK contains examples, libraries and tools to develop **RTOS** based Industrial Communication applications on PRU-ICSS

The Industrial Communications SDK enables real-time industrial communications for TI processors. Industrial communication is typically handled by the Programmable Real-Time Unit Industrial Communication Subsystem (PRU-ICSS). The PRU-ICSS is a co-processor subsystem containing Programmable Real-Time (PRU) cores and Ethernet media access controllers (EMACs), which implement the low level industrial Ethernet and fieldbus protocols through firmware. The upper layers of the protocols stacks are implemented in software running on ARM cores.

PRU cores are primarily used for industrial communication, and can also be used for other applications such as motor control and custom interfaces. The PRU-ICSS frees up the main ARM cores in the device for other functions, such as control and data processing.

\attention This SDK also includes \htmllink{@VAR_MCU_SDK_DOCS_PATH/index.html, @VAR_SOC_NAME MCU+ SDK}.

## Getting Started

To get started, see \htmllink{@VAR_MCU_SDK_DOCS_PATH/GETTING_STARTED.html, GETTING STARTED} page.

## Block Diagram

Given below is a block diagram of the SW modules in this SDK

\image html block_diagram_am263x.png "Software Block Diagram"

The main software components in the block diagram are described below


<table>
<tr>
    <th>Software Components
    <th>Documentation Page
    <th>Description
</tr>
<tr><td colspan="3" bgcolor=#F0F0F0>**Device Drivers and HAL (Hardware Abstraction Layer)**</td></tr>
<tr>
    <td>Industrial Protocol Firmware (FW) HAL
    <td> \ref INDUSTRIAL_COMMS_FWHAL
    <td>Hardware abstraction layer APIs to higher level industrial protocol stacks. Example, EtherCAT FW HAL
</tr>
<tr><td colspan="3" bgcolor=#F0F0F0>**Protocol Stacks and Middleware**</td></tr>
<tr>
    <td>Industrial Protocol Stacks
    <td> \ref INDUSTRIAL_COMMS
    <td>Industrial protocol stacks for protocols like EtherCAT
</tr>
<tr><td colspan="3" bgcolor=#F0F0F0>**Examples and Demos**</td></tr>
<tr>
    <td>Examples and Demos
    <td>\ref EXAMPLES
    <td>Examples and demos showing usage of different SW libraries and APIs
</tr>
</table>

For details on software components of MCU+ SDK, please refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/index.html, @VAR_SOC_NAME MCU+ SDK}.

## Directory Structure

Given below is a overview of the directory structure to help you navigate the SDK and related tools.

<table>
<tr>
    <th>Folder/Files
    <th>Description
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/</td></tr>
<tr>
    <td>README_FIRST_@VAR_SOC_NAME.html
    <td>Open this file in a web browser to reach this user guide</td>
</tr>
<tr>
    <td>makefile
    <td>Top level makefile to build the whole SDK using "make"</td>
</tr>
<tr>
    <td>imports.mak
    <td>Top level makefile to list paths to dependent tools</td>
</tr>
<tr>
    <td>docs/
    <td>Offline browseable HTML documentation</td>
</tr>
<tr>
    <td>examples/
    <td>Example applications for @VAR_SOC_NAME, across multiple boards </td>
</tr>
<tr>
    <td>mcu_plus_sdk/
    <td>MCU + SDK</td>
</tr>
<tr>
    <td>source/
    <td>Device drivers, middleware libraries and APIs</td>
</tr>
</table>

Given below are the paths where the different tools needed outside the SDK, like CCS, SysConfig are installed by default in Windows.
In Linux, the tools are installed by default in ${HOME}/ti.

<table>
<tr>
    <th>Folder/Files
    <th>Description
</tr>
<tr>
    <td>C:/ti/ccs@VAR_CCS_FOLDER_VERSION_AM263X
    <td>Code Composer Studio</td>
</tr>
<tr>
    <td>C:/ti/sysconfig_@VAR_SYSCFG_VERSION_AM263X
    <td>SysConfig. **NOTE**, SysConfig is also installed as part of CCS at ${CCS_INSTALL_PATH}/ccs/utils/sysconfig_x.x.x</td>
</tr>
<tr>
    <td>C:/ti/ti-cgt-armllvm_@VAR_TI_ARM_CLANG_VERSION
    <td>TI ARM CLANG compiler tool chain</td>
</tr>
</table>

## Licenses

The licensing information of this SDK, as well as any third-party components included which are made available under a number of other open-source licenses are enumerated as part of the manifest.
A complete manifest along with export control information is detailed here [\htmllink{../../docs/@VAR_SOC_MANIFEST,LINK}] and the SDK Software License Agreement (SLA) is here [\htmllink{../../license.txt,LINK}]

## Help and Support

For additional help and support, see https://e2e.ti.com/support/microcontrollers/arm-based-microcontrollers-group/arm-based-microcontrollers/f/arm-based-microcontrollers-forum

## Documentation Credits

This user guide is generated using doxygen, v1.8.20. See https://www.doxygen.nl/index.html