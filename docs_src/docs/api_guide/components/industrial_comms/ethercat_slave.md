# EtherCAT SubDevice FWHAL {#ETHERCAT_SUBDEVICE_FWHAL}

[TOC]

## Introduction

This software is designed for the TI SoCs with PRU-ICSS IP to enable customers add EtherCAT SubDevice protocol support to their system. It implements EtherCAT SubDevice Controller(ESC) Layer 2 functionality with two MII ports (one IN and one OUT port per PRU-ICSS) in accordance with ETG.1000.4 Data Link Layer protocol specification. This provides EtherCAT ASIC like functionality integrated into TI SoCs.

\image html EtherCAT_SubDevice_Software_Architecture.png "Software Architecture"

EtherCAT firmware for PRU-ICSS is a black box product maintained by TI. EtherCAT SubDevice FWHAL(Firmware and Hardware Abstraction Layer) allows loading and running the EtherCAT firmware and acts as an interface with ESC firmware. FWHAL implements the key interface between EtherCAT SubDevice Controller Emulation firmware and EtherCAT stack.

## SysConfig Features

@VAR_SYSCFG_USAGE_NOTE

SysConfig can be used to configure things mentioned below:

\cond SOC_AM64X || SOC_AM243X
- Selecting the PRU-ICSS instance
- Configuring PINMUX needed for MII mode
- Configuring ETHPHY for the two PHY ports of PRU-ICSS
- Configuring MDIO Manual Mode (for enabling work-around for issue "i2329 - MDIO: MDIO interface corruption (CPSW and PRU-ICSS)" described in <a href="https://www.ti.com/lit/er/sprz457e/sprz457e.pdf">AM64x/AM243x Processor Silicon Revision 1.0, 2.0 (Rev. E)</a>)

\endcond

\cond SOC_AM263X
- Selecting the PRU-ICSS instance
- Configuring PINMUX needed for MII mode
- Configuring ETHPHY for the two PHY ports of PRU-ICSS
\endcond

## PRU-ICSS EtherCAT SubDevice Firmware

### Features Supported

- All EtherCAT Commands (NOP, APRD, APWR, APRW, FPRD, FPWR, FPRW, BRD, BWR, BRW, LRD, LWR, LRW, ARMW and FRMW)
- 8 FMMU support
- 8 SM support
\cond SOC_AM64X || SOC_AM243X
- 59KB of Process Data RAM
\endcond
\cond SOC_AM263X || SOC_AM263PX || SOC_AM261X
- 28KB of Process Data RAM
\endcond
- Distributed clocks
    - 64-bit DC
    - SYNC0 out generation single shot and cyclic mode support
    - SYNC1 out generation - SYNC1 cycle time multiple of SYNC0 cycle time
    - Latch0 and Latch1 inputs
    - System Time PDI control
- DL Loop Control
    - Using MII_RX_LINK (fast - depending on PHY link loss detection latency) - mandatory for cable redundancy support
    - Using PRU-ICSS MDIO state machine - not recommended for cable redundancy support
- Interrupts - AL/ECAT events
    - SYNC0, SYNC1 and PDI interrupt events on external SOC pins
- Watchdog - PDI and SM
- Error Counters
    - RX Invalid Frame Counter Port 0/1
    - RX ERR Counter Port 0/1
    - Forwarded Error Counter Port 0/1
    - ECAT Processing Unit Error Counter
- LED - Run, Error and Port0/1 activity
- EEPROM Emulation for ESI EEPROM support
- Management Interface for PHY over EtherCAT
- PHY address configuration and host side API for PHY programming
- Cable redundancy support
\cond SOC_AM64X || SOC_AM243X
- Firmware based on 200 MHz or 333 MHz clock frequency for PRU-ICSS Core Clock and IEP Clock
\endcond
\cond  SOC_AM263X || SOC_AM263PX || SOC_AM261X
- Firmware based on 200 MHz clock frequency for PRU-ICSS Core Clock and IEP Clock
\endcond

\cond  SOC_AM243X || SOC_AM64X 
#### Key Performance Parameters

Sync Jitter measurement done using TwinCAT 3.1 along with C6015-0020 (Beckhoff PLC) for PRU core running at 200MHz.

<table>
    <tr>
        <th style="width: 40%">Feature</th>
        <th style="width: 30%">Detail</th>
        <th style="width: 30%">Value</th>
    </tr>
    <tr>
        <td>Distributed Clock</td>
        <td>Sync Jitter</td>
        <td>16ns (at 50μs cycle time running for 72 hours)</td>
    </tr>
    <tr>
        <td rowspan=2>Latency</td>
        <td>Process Path</td>
        <td>Average = 430ns, Max = 440ns</td>
    </tr>
    <tr>
        <td>Auto Forward Path (Reverse Path)</td>
        <td>Average = 330ns, Max = 340ns</td>
    </tr>
</table>
\endcond

#### Release Notes
\cond SOC_AM64X || SOC_AM243X
##### Industrial Communications SDK Version 09.02 (Not available in 09.02.00.15)
\endcond

\cond SOC_AM263X || SOC_AM263PX || SOC_AM261X
##### Industrial Communications SDK Version 10.00.00
\endcond

- Firmware Version : x.5.43
- Fix for PINDSW-47   : Single datagram accessing multiple FMMU mapped areas using LRD/LWR commands from a single SubDevice.
    - Do note that if this feature is enabled, then the Process Path latency will be dynamically increased to take care of the timing constraints. Please refer to Register 0xED0 of Vendor Specific Register in \ref ETHERCAT_SUBDEVICE_CONTROLLER_REGISTER_LIST
- Fix for PINDSW-141  : LRW access to non-interleaved input and output process data of multiple SubDevices does not work. 
    - This will make EtherCAT SubDevice compatibile with default mode of few open source EtherCAT MainDevice like [SOEM](https://github.com/OpenEtherCATsociety/SOEM) and [IgH](https://gitlab.com/etherlab.org/ethercat).
    - Do note that if this feature is enabled, then the Process Path latency will be dynamically increased to take care of the timing constraints. Please refer to Register 0xED0 of Vendor Specific Register in \ref ETHERCAT_SUBDEVICE_CONTROLLER_REGISTER_LIST
- Bug-fix for PINDSW-8246 : Triple buffer issue - Not getting the latest data from buffer during free-run mode.
- Bug-fix for PINDSW-8115 : Watchdog error while using LRD and LWR with same logical address.

\cond SOC_AM64X || SOC_AM243X

##### MCU+ SDK Version 08.05.00

- Firmware Version : x.5.12
- Bug-fix for PINDSW-5384 : ESC DL Status register is not initialized correctly
- Bug-fix for PINDSW-5385 : Next SYNC1 Pulse register is not updated correctly
- Bug-fix for PINDSW-5401 : SYNC1 Pulse is generated one SYNC0 cycle time late

##### MCU+ SDK Version 08.04.00

- Firmware Version : x.5.8
- Added the support for enabling MDIO Manual Mode. New vendor specific registers are added for this purpose. (See \ref ETHERCAT_SUBDEVICE_CONTROLLER_REGISTER_LIST for more details)
- Bug-fix for PINDSW-5369 : Write to System Time Delay (0x0928) register works only once

\endcond

\cond SOC_AM263X

##### MCU+ SDK Version 08.05.00

- Firmware Version : x.5.12
- Bug-fix for PINDSW-5384 : ESC DL Status register is not initialized correctly
- Bug-fix for PINDSW-5385 : Next SYNC1 Pulse register is not updated correctly
- Bug-fix for PINDSW-5401 : SYNC1 Pulse is generated one SYNC0 cycle time late
- Bug-fix for PINDSW-5369 : Write to System Time Delay (0x0928) register works only once

##### MCU+ SDK Version 08.04.00

- Firmware Version : x.5.6
- No updates from MCU+ SDK Version 08.03.00

\endcond

\cond SOC_AM64X || SOC_AM243X || SOC_AM263X

##### MCU+ SDK Version 08.03.00

- Firmware Version : x.5.6
- Added selection of 200 MHz or 333 MHz clock frequency for PRU-ICSS Core Clock and IEP Clock used by EtherCAT SubDevice Firmware using PRU Clock Frequency Selection Register (0x0E34)
- Added PDI ISR Time Register (0x0E2C)
- Changed default value of Bit 3 and Bit 7 in Sync/Latch PDI Configuration Register (0x0151) to 0
- Bug-fixes in implementation of RX Error Counter

##### MCU+ SDK Version 08.02.00

- Firmware Version : x.5.1
- EtherCAT SubDevice Firmware based on 333 MHz (instead of 200 MHz) clock frequency for PRU-ICSS Core Clock and IEP Clock for better process path latency
- Add PHY RX Error Counter Register (0x0E28) for improving RX Error Counter accuracy (See \ref ETHERCAT_SUBDEVICE_CONTROLLER_EXCEPTIONS_REGISTER_EXCEPTIONS for more details)
- Bug-fixes for PINDSW-3120, PINDSW-5194, PINDSW-5229 and PINDSW-5267

\endcond

### Features Not Supported

- EtherCAT SubDevice Controller
    - ECAT side register protection when using LRD command
    - APRW/FPRW/BRW for SM mapped area
- EtherCAT G
- Reset Isolation

### Known Issues

<table>
<tr>
    <th> Record ID
    <th> Details
    <th> Workaround
</tr>
<tr>
    <td> PINDSW-72
    <td> PDI/PD watchdog counter incremented by 1 whenever PDI/PD watchdog is disabled
    <td> None
</tr>
<tr>
    <td> PINDSW-74
    <td> LRD access on unused registers increment WKC - no register protection while using LRD
    <td> None
</tr>
<tr>
    <td> PINDSW-2204
    <td> Frames with no SFD not counted as errors if received on reverse path
    <td> None
</tr>
<tr>
    <td> PINDSW-2360
    <td> System time of next Sync0 pulse register (0x990:0x993) is not instantaneous, resulting in read of incorrect value if read immediately after sync pulse
    <td> None
</tr>
<tr>
    <td> PINDSW-5135
    <td> Read permissions and byte level write permissions for RW type commands are not checked
    <td> None
</tr>
<tr>
    <td> PINDSW-5145
    <td> RX_ER counter does not count errors outside frame in MII RX_CLK units precisely
    <td> Added PHY RX Error Counter Register (0x0E28) for improving RX Error Counter accuracy. Configuring this register will track RX_ERs within a frame precisely using PHY registers. Refer \ref ETHERCAT_SUBDEVICE_CONTROLLER_EXCEPTIONS_REGISTER_EXCEPTIONS for more details.
</tr>
<tr>
    <td> PINDSW-5414
    <td> Link Lost Counter is incorrectly incremented once for ports with polarity of RXLINK input as “Active Low” during initialization
    <td> Use Active High Polarity for LED_LINK/SPEED connected to MII0/MII1 Receive Link (RXLINK) pin of PRU-ICSS
</tr>
</table>

For more details, please see the \htmllink{../EtherCAT_Slave_Errata.pdf, EtherCAT SubDevice Errata} document.

## Important Files and Directory Structure

\cond SOC_AM64X || SOC_AM243X

<table>
<tr>
    <th>Folder/Files
    <th>Description
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/examples/industrial_comms</td></tr>
<tr>
    <td>ethercat_slave_demo/simple</td>
    <td>EtherCAT SubDevice Simple Example (based on pre-integrated stack) </td>
</tr>
<tr>
    <td>ethercat_slave_demo/cia402</td>
    <td>EtherCAT SubDevice CiA402 Example (based on pre-integrated stack) </td>
</tr>
<tr>
    <td>ethercat_slave_demo/ctt</td>
    <td>EtherCAT SubDevice CTT Example (based on pre-integrated stack) </td>
</tr>
<tr>
    <td>ethercat_slave_beckhoff_ssc_demo</td>
    <td>EtherCAT SubDevice Example based on Beckhoff SSC</td>
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave</td></tr>
<tr>
    <td>icss_fwhal/firmware/g_v1.3</td>
    <td>Firmware for the PRU cores in PRU-ICSS. **Firmware Version : 6.5.43** </td>
</tr>
<tr>
    <td>icss_fwhal/lib/</td>
    <td>FWHAL library for EtherCAT SubDevice</td>
</tr>
<tr>
    <td>icss_fwhal/tiescbsp.h</td>
    <td>FWHAL interface file</td>
</tr>
<tr>
    <td>stack/*.lib</td>
    <td>Evaluation libraries for EtherCAT SubDevice Stack</td>
</tr>
<tr>
    <td>stack/esi</td>
    <td>ESI XML files for EtherCAT SubDevice Simple Example and EtherCAT SubDevice CiA402 Example</td>
</tr>
<tr>
    <td>stack/inc</td>
    <td>Stack header files for evaluation stack</td>
</tr>
<tr>
    <td>beckhoff_stack/esi</td>
    <td>ESI XML file for Beckhoff SubDevice Stack Code(SSC) based example</td>
</tr>
<tr>
    <td>beckhoff_stack/patch</td>
    <td>Patch file for Beckhoff SubDevice Stack Code(SSC) sources</td>
</tr>
<tr>
    <td>beckhoff_stack/stack_hal</td>
    <td>Stack adaptation APIs for Beckhoff SubDevice Stack Code(SSC)</td>
</tr>
<tr>
    <td>beckhoff_stack/stack_sources</td>
    <td>Folder where Beckhoff SubDevice Stack Code(SSC) sources should be copied. Stack sources are not packaged in the SDK</td>
</tr>
</table>
\endcond

\cond SOC_AM263X || SOC_AM263PX || SOC_AM261X

<table>
<tr>
    <th>Folder/Files
    <th>Description
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/examples/industrial_comms</td></tr>
<tr>
    <td>ethercat_slave_beckhoff_ssc_demo</td>
    <td>EtherCAT SubDevice Example based on Beckhoff SSC</td>
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave</td></tr>
<tr>
    <td>icss_fwhal/firmware/m_v2.3</td>
    <td>Firmware for the PRU cores in PRU-ICSS. **Firmware Version : 5.5.43** </td>
</tr>
<tr>
    <td>icss_fwhal/lib/</td>
    <td>FWHAL library for EtherCAT SubDevice</td>
</tr>
<tr>
    <td>icss_fwhal/tiescbsp.h</td>
    <td>FWHAL interface file</td>
</tr>
<tr>
    <td>beckhoff_stack/esi</td>
    <td>ESI XML file for Beckhoff SubDevice Stack Code(SSC) based example</td>
</tr>
<tr>
    <td>beckhoff_stack/patch</td>
    <td>Patch file for Beckhoff SubDevice Stack Code(SSC) sources</td>
</tr>
<tr>
    <td>beckhoff_stack/stack_hal</td>
    <td>Stack adaptation APIs for Beckhoff SubDevice Stack Code(SSC)</td>
</tr>
<tr>
    <td>beckhoff_stack/stack_sources</td>
    <td>Folder where Beckhoff SubDevice Stack Code(SSC) sources should be copied. Stack sources are not packaged in the SDK</td>
</tr>
</table>
\endcond

## API Documentation

Please see \ref INDUSTRIAL_COMMS_ETHERCAT_SLAVE_FWHAL_MODULE for API documentation.

It is recommended to use these FWHAL APIs in the stack adaptation files. For example, see `${SDK_INSTALL_PATH}/source/industrial_comms/ethercat_slave/beckhoff_stack/stack_hal`, which contains the stack adaptation APIs for Beckhoff SubDevice Stack Code(SSC).

## Procedure to kick-off the EtherCAT SubDevice Controller(ESC)

- Initialize memories (register protection, register reset values, EEPROM cache) and PRU-ICSS INTC module. This is done by FWHAL(Firmware and Hardware Abstraction Layer).

- Load firmware into PRUs of PRU-ICSS. This is done by FWHAL.

- Enable the PRU cores. This is done by FWHAL.

- Initialize the EtherCAT SubDevice stack.

- Wait for AL Event Request and SYNC (in DC synchronous mode) interrupts from PRU and run EtherCAT stack main loop for handling mailbox and ESC state machine.

- Handle the events as needed. Note that this is handled by the stack.

## Hardware Requirements

- Sitara Processor with PRU-ICSS IP and EtherCAT support
- Current implementation uses only Spinlock 0. Spinlocks 1 to 7 are free for use. 
- HW signals required to implement EtherCAT slave functionality is shown below, this info needs to be used in conjunction with [SYSCONFIG](https://www.ti.com/tool/SYSCONFIG):
<table>
<tr>
    <th> Signal Name
    <th> Requirement
    <th> Description
</tr>
\cond SOC_AM64X || SOC_AM243X
<tr>
    <td colspan="3">**PRU-ICSS MDIO**</td>
</tr>
<tr>
    <td> PRGx_MDIO0_MDC
    <td> Mandatory
    <td> MDIO clock
</tr>
<tr>
    <td> PRGx_MDIO0_MDIO
    <td> Mandatory
    <td> MDIO Data
</tr>
<tr>
    <td colspan="3">**PRU-ICSS Distributed Clocks (Network Clock synchronization)**</td>
</tr>
<tr>
    <td> PRGx_IEP0_EDC_SYNC_OUT0
    <td> Recommmended (for DC capable SubDevices)
    <td> SYNC0 out - Time synchronized OUT0
</tr>
<tr>
    <td> PRGx_IEP0_EDC_SYNC_OUT1
    <td> Optional (depends on customer application))
    <td> SYNC1 out - Time synchronized OUT1 (depends on SYNC0)
</tr>
<tr>
    <td> PRGx_IEP0_EDC_LATCH_IN0
    <td> Optional
    <td> LATCH0 in (Time stamp latch input0)
</tr>
<tr>
    <td> PRGx_IEP0_EDC_LATCH_IN1
    <td> Optional
    <td> LATCH1 in (Time stamp latch input1)
</tr>
<tr>
    <td colspan="3">**PRU-ICSS MII PDI Interrupt**</td>
</tr>
<tr>
    <td> PRGx_IEP0_EDIO_DATA_IN_OUT28
    <td rowspan=4> Optional
    <td rowspan=4> PDI ISR output to external SOC pin (via one of the 4 PRU-ICSS digio outputs).<br>PDI ISR pin can be selected via vendor specific register at offset 0xE0A. 
</tr>
<tr>
    <td> PRGx_IEP0_EDIO_DATA_IN_OUT29
</tr>
<tr>
    <td> PRGx_IEP0_EDIO_DATA_IN_OUT30
</tr>
<tr>
    <td> PRGx_IEP0_EDIO_DATA_IN_OUT31
</tr>
\endcond
\cond SOC_AM263X || SOC_AM263PX || SOC_AM261X
<tr>
    <td colspan="3">**PRU-ICSS MDIO**</td>
</tr>
<tr>
    <td> PR0_MDIO0_MDC
    <td> Mandatory
    <td> MDIO clock
</tr>
<tr>
    <td> PR0_MDIO0_MDIO
    <td> Mandatory
    <td> MDIO Data
</tr>
<tr>
    <td colspan="3">**PRU-ICSS Distributed Clocks (Network Clock synchronization)**</td>
</tr>
<tr>
    <td> PR0_IEP0_EDC_SYNC_OUT0
    <td> Recommmended (for DC capable SubDevices)
    <td> SYNC0 out - Time synchronized OUT0
</tr>
<tr>
    <td> PR0_IEP0_EDC_SYNC_OUT1
    <td> Optional (depends on customer application))
    <td> SYNC1 out - Time synchronized OUT1 (depends on SYNC0)
</tr>
<tr>
    <td colspan="3">**PRU-ICSS MII PDI Interrupt**</td>
</tr>
<tr>
    <td> PR0_IEP0_EDIO_DATA_IN_OUT30
    <td rowspan=2> Optional
    <td rowspan=2> PDI ISR output to external SOC pin (via one of the 2 PRU-ICSS digio outputs).<br>PDI ISR pin can be selected via vendor specific register at offset 0xE0A. 
</tr>
<tr>
    <td> PR0_IEP0_EDIO_DATA_IN_OUT31
</tr>
\endcond
\cond SOC_AM64X || SOC_AM243X
<tr>
    <td colspan="3">**PRU-ICSS MII Port0 (IN Port) & PRU-ICSS MII Port1 (OUT Port)**</td>
</tr>
<tr>
    <td> PRx_MII0_RXD0
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data0 
</tr>
<tr>
    <td> PRx_MII1_RXD0
</tr>
<tr>
    <td> PRx_MII0_RXD1
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data1 
</tr>
<tr>
    <td> PRx_MII1_RXD1
</tr>
<tr>
    <td> PRx_MII0_RXD2
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data2 
</tr>
<tr>
    <td> PRx_MII1_RXD2
</tr>
<tr>
    <td> PRx_MII0_RXD3
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data3 
</tr>
<tr>
    <td> PRx_MII1_RXD3
</tr>
<tr>
    <td> PRx_MII0_RXDV
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 RX Data Valid 
</tr>
<tr>
    <td> PRx_MII1_RXDV
</tr>
<tr>
    <td> PRx_MII0_RXER
    <td rowspan=2> Optional if PHY supports Enhanced Link detection
    <td rowspan=2> MII0 and MII1 RXERR 
</tr>
<tr>
    <td> PRx_MII1_RXER
</tr>
<tr>
    <td> PRx_MII0_TXD0
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data0 
</tr>
<tr>
    <td> PRx_MII1_TXD0
</tr>
<tr>
    <td> PRx_MII0_TXD1
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data1 
</tr>
<tr>
    <td> PRx_MII1_TXD1
</tr>
<tr>
    <td> PRx_MII0_TXD2
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data2 
</tr>
<tr>
    <td> PRx_MII1_TXD2
</tr>
<tr>
    <td> PRx_MII0_TXD3
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data3
</tr>
<tr>
    <td> PRx_MII1_TXD3
</tr>
<tr>
    <td> PRx_MII0_TXEN
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 TX enable 
</tr>
<tr>
    <td> PRx_MII1_TXEN
</tr>
<tr>
    <td> PRx_MII_MR0_CLK
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive clock
</tr>
<tr>
    <td> PRx_MII_MR1_CLK
</tr>
<tr>
    <td> PRx_MII_MT0_CLK
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit clock
</tr>
<tr>
    <td> PRx_MII_MT1_CLK
</tr>
<tr>
    <td> PRx_MII0_RXLINK
    <td rowspan=2> Mandatory for cable redundancy support
    <td rowspan=2> Enhanced link detection. Redundancy support: connect LED_LINK/LED_SPEED from PHY
here 
</tr>
<tr>
    <td> PRx_MII1_RXLINK
</tr>
\endcond
\cond SOC_AM263X || SOC_AM263PX
<tr>
    <td colspan="3">**PRU-ICSS MII Port0 (IN Port) & PRU-ICSS MII Port1 (OUT Port)**</td>
</tr>
<tr>
    <td> MII0_RXD0
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data0 
</tr>
<tr>
    <td> MII1_RXD0
</tr>
<tr>
    <td> MII0_RXD1
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data1 
</tr>
<tr>
    <td> MII1_RXD1
</tr>
<tr>
    <td> MII0_RXD2
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data2 
</tr>
<tr>
    <td> MII1_RXD2
</tr>
<tr>
    <td> MII0_RXD3
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data3 
</tr>
<tr>
    <td> MII1_RXD3
</tr>
<tr>
    <td> MII0_RXDV
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 RX Data Valid 
</tr>
<tr>
    <td> MII1_RXDV
</tr>
<tr>
    <td> MII0_RXER
    <td rowspan=2> Optional if PHY supports Enhanced Link detection
    <td rowspan=2> MII0 and MII1 RXERR 
</tr>
<tr>
    <td> MII1_RXER
</tr>
<tr>
    <td> MII0_TXD0
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data0 
</tr>
<tr>
    <td> MII1_TXD0
</tr>
<tr>
    <td> MII0_TXD1
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data1 
</tr>
<tr>
    <td> MII1_TXD1
</tr>
<tr>
    <td> MII0_TXD2
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data2 
</tr>
<tr>
    <td> MII1_TXD2
</tr>
<tr>
    <td> MII0_TXD3
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data3
</tr>
<tr>
    <td> MII1_TXD3
</tr>
<tr>
    <td> MII0_TXEN
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 TX enable 
</tr>
<tr>
    <td> MII1_TXEN
</tr>
<tr>
    <td> MII0_RXCLK
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive clock
</tr>
<tr>
    <td> MII1_RXCLK
</tr>
<tr>
    <td> MII0_TXCLK
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit clock
</tr>
<tr>
    <td> MII1_TXCLK
</tr>
<tr>
    <td> MII0_RXLINK
    <td rowspan=2> Mandatory for cable redundancy support
    <td rowspan=2> Enhanced link detection. Redundancy support: connect LED_LINK/LED_SPEED from PHY
here 
</tr>
<tr>
    <td> MII1_RXLINK
</tr>
\endcond
\cond SOC_AM261X
<tr>
    <td colspan="3">**PRU-ICSS MII Port0 (IN Port) & PRU-ICSS MII Port1 (OUT Port)**</td>
</tr>
<tr>
    <td> MII1_RXD0
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data0 
</tr>
<tr>
    <td> MII2_RXD0
</tr>
<tr>
    <td> MII1_RXD1
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data1 
</tr>
<tr>
    <td> MII2_RXD1
</tr>
<tr>
    <td> MII1_RXD2
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data2 
</tr>
<tr>
    <td> MII2_RXD2
</tr>
<tr>
    <td> MII1_RXD3
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive Data3 
</tr>
<tr>
    <td> MII2_RXD3
</tr>
<tr>
    <td> MII1_RXDV
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 RX Data Valid 
</tr>
<tr>
    <td> MII2_RXDV
</tr>
<tr>
    <td> MII1_RXER
    <td rowspan=2> Optional if PHY supports Enhanced Link detection
    <td rowspan=2> MII0 and MII1 RXERR 
</tr>
<tr>
    <td> MII2_RXER
</tr>
<tr>
    <td> MII1_TXD0
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data0 
</tr>
<tr>
    <td> MII2_TXD0
</tr>
<tr>
    <td> MII1_TXD1
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data1 
</tr>
<tr>
    <td> MII2_TXD1
</tr>
<tr>
    <td> MII1_TXD2
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data2 
</tr>
<tr>
    <td> MII2_TXD2
</tr>
<tr>
    <td> MII1_TXD3
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit Data3
</tr>
<tr>
    <td> MII2_TXD3
</tr>
<tr>
    <td> MII1_TXEN
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 TX enable 
</tr>
<tr>
    <td> MII2_TXEN
</tr>
<tr>
    <td> MII1_RXCLK
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Receive clock
</tr>
<tr>
    <td> MII2_RXCLK
</tr>
<tr>
    <td> MII1_TXCLK
    <td rowspan=2> Mandatory
    <td rowspan=2> MII0 and MII1 Transmit clock
</tr>
<tr>
    <td> MII2_TXCLK
</tr>
<tr>
    <td> MII1_RXLINK
    <td rowspan=2> Mandatory for cable redundancy support
    <td rowspan=2> Enhanced link detection. Redundancy support: connect LED_LINK/LED_SPEED from PHY
here 
</tr>
<tr>
    <td> MII2_RXLINK
</tr>
\endcond
</table>

## Interrupts
EtherCAT SubDevice Controller firmware generates the following interrupts.

8 Host Interrupts (Host Interrupts 2 through 9) are exported from the PRU_ICSSG internal INTC for signaling the device level interrupt controllers. PRU_EVTOUT0 to PRU_EVTOUT7 correspond to these eight interrupts in the following table. Please check \htmllink{@VAR_MCU_SDK_DOCS_PATH/DRIVERS_PRUICSS_PAGE.html#PRUICSS_INTC, PRUICSS Interrupt Controller} section for more details.

<table>
<tr>
    <th>Name
    <th>Host Interrupt
    <th>Description
</tr>
<tr>
    <td> DC SYNC0 OUT
    <td> PRU_EVTOUT1
    <td> Used in DC mode for syncing the application
</tr>
<tr>
    <td> DC SYNC1 OUT
    <td> PRU_EVTOUT2
    <td> Used in DC mode for syncing the application
</tr>
<tr>
    <td> PDI Interrupt
    <td> PRU_EVTOUT3
    <td> AL event/PDI interrupt to host stack
</tr>
<tr>
    <td> ESC Command Acknowledgement
    <td> PRU_EVTOUT4
    <td> ESC firmware command completion acknowledgement to Host
</tr>
</table>

## Enhanced Link

- This feature ensures that the link signal is checked every approximately 10 µs. 
- Enhanced MII link detection will additionally disconnect a link if at least 32 RX errors (RX_ER) occur in a fixed interval of ~10 μs. Refer to Section 5.6.2 of [Hardware Data Sheet Section I](https://download.beckhoff.com/download/Document/io/ethercat-development-products/ethercat_esc_datasheet_sec1_technology_2i3.pdf) for more details.
- For this feature to work, `enhancedlink_enable` of \ref bsp_params should be set to \ref TIESC_MDIO_RX_LINK_ENABLE while initializing the EtherCAT SubDevice FWHAL. This configuration is done in `tiesc_socParamsInit()` in tiescsoc.c file. In the current implementation, this feature is enabled by default.
- Enhanced Link is mandatory for [Testing Cable Redundancy](\ref ETHERCAT_SUBDEVICE_DEMO_TWINCAT_CABLE_REDUNDANCY).

### Enhanced Link Detection Procedure

- Firstly, make sure the PHY supports Enhanced Link capability.
- Make sure RXLINK pin is connected properly.
- For this feature to work, LED pin is to be configured correctly within the application. Refer to **ETHPHY_DP83826E_LED0** configuration in `tiesc_ethphyInit()` in tiescsoc.c file.
\cond SOC_AM64X || SOC_AM243X
- Check PRU MDIO registers for the PHY alive and link status. Refer to Section 6.4.14.10.3 (MDIO_ALIVE_REG Register) and Section 6.4.14.10.4 (MDIO_LINK_REG Register) of [AM64x/AM243x Technical Reference Manual](https://www.ti.com/lit/ug/spruim2h/spruim2h.pdf?ts=1731472716061&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FAM6442). 
\endcond
\cond SOC_AM263X || SOC_AM263PX || SOC_AM261X
- Check PRU MDIO registers for the PHY alive and link status. Refer to Section 4.2.2.360.1 (MDIO_ALIVE_REG Register) and Section 4.2.2.361.1 (MDIO_LINK_REG Register) of [AM26x Register Addendum](https://www.ti.com/lit/ug/spruj42e/spruj42e.pdf?ts=1731476077292&ref_url=https%253A%252F%252Fwww.google.com%252F). 
\endcond
- MDIO_ALIVE_REG: Each of the 32 bits of this register is set if the most recent access to the PHY with address corresponding to the register bit number was acknowledged by the PHY, the bit is reset if the PHY fails to acknowledge the access. Both the user and polling accesses to a PHY will cause the corresponding alive bit to be updated. The alive bits are only meant to be used to give an indication of the presence or not of a PHY with the corresponding address. Check to see if the expected bits are set.
- MDIO_LINK_REG: This register is updated after a read of the Generic Status Register of a PHY. The bit is set/reset if the PHY with the corresponding address has link and the PHY acknowledges the read transaction. Check to see if the expected bits are set. 
    - Do note that this register is dependent on the link polarity. If the bits in this register are getting reset while connecting to an active network, it implies the polarity is active low. If the bits in this register are getting set while connecting to an active network, it implies the polarity is active high. This needs to be configured for **TIESC_LINK0_POL** and **TIESC_LINK1_POL** in tiescsoc.c file. Vendor specific ESC Register \ref ESC_ADDR_TI_PHY_LINK_POLARITY can also be used to identify the polarity.
- Make sure that the set bit address is configured as **MDIO Phy Address** in Syscfg -> ETHPHY for the corresponding PHY instances. The configured PHY address can be seen in Vendor Specific ESC Register \ref ESC_ADDR_TI_PORT0_PHYADDR and \ref ESC_ADDR_TI_PORT1_PHYADDR.
\imageStyle{EtherCAT_SubDevice_PHY_Configuration.PNG,width:50%}
\image html EtherCAT_SubDevice_PHY_Configuration.PNG "Syscfg PHY Configuration"

- **Check the PHY status**: The MDIO module includes a user access register (MDIO_USER_ACCESS_REG_j) to directly access a specified Phy. This is described in MDIO Register offset 0x80 of the TRM for Reading and Writing Data from/to a PHY Register
- **Check frame couter and error counter vendor specific registers**: ESC Register **0x0300 (RX Error Counter)** is to be monitored for RX errors and Vendor specific ESC Register \ref ESC_ADDR_TI_PORT0_ACTIVITY and \ref ESC_ADDR_TI_PORT1_ACTIVITY should be monitored for number of valid frames at Port0 and Port1 respectively. 
- Once successful, build and load the application and try scanning with TwinCAT or any other EtherCAT MainDevices. 
- Refer to [Testing Cable Redundancy](\ref ETHERCAT_SUBDEVICE_DEMO_TWINCAT_CABLE_REDUNDANCY) for testing this feature using TwinCAT.

## EtherCAT SubDevice Controller(ESC) Register List

\subpage ETHERCAT_SUBDEVICE_CONTROLLER_REGISTER_LIST contains descriptions of the registers in TI's EtherCAT SubDevice Controller implementation.

## EtherCAT SubDevice Controller(ESC) Exceptions

\subpage ETHERCAT_SUBDEVICE_CONTROLLER_EXCEPTIONS lists the exceptions TI's EtherCAT SubDevice Controller implementation when compared with ET1100 ASIC. Please note that TI ESC is a 2 port EtherCAT SubDevice and it does not support E-bus interface and all the corresponding register fields are not implemented.

## Additional References {#ETHERCAT_SUBDEVICE_ADDITIONAL_REFERENCES}

Please refer to below documents to understand more about EtherCAT SubDevice on TI platforms and EtherCAT SubDevice protocol specifications.

<table>
<tr>
    <th>Document
    <th>Description
</tr>
<tr>
    <td>[EtherCAT on Sitara Processors](https://www.ti.com/lit/pdf/spry187)
    <td>Application note by TI on the EtherCAT SubDevice implementation on TI's Sitara Processors.
</tr>
<tr>
    <td>[PRU-ICSS EtherCAT SubDevice Troubleshooting Guide](https://www.ti.com/lit/an/spracj7/spracj7.pdf)
    <td>This troubleshooting guide is intended to provide guidance on how to set up and debug the EtherCAT SubDevice implemented on TI's Sitara processors.
</tr>
<tr>
    <td>[EtherCAT ESC Datasheet Section 1 - Technology](https://download.beckhoff.com/download/Document/io/ethercat-development-products/ethercat_esc_datasheet_sec1_technology_2i3.pdf)
    <td>Section 1 of Beckhoff's EtherCAT SubDevice Controller (ESC) documentation which describes basic EtherCAT technology.
</tr>
<tr>
    <td>[EtherCAT ESC Datasheet Section 2 - Register Description](https://download.beckhoff.com/download/Document/io/ethercat-development-products/ethercat_esc_datasheet_sec2_registers_3i0.pdf)
    <td>Section 2 of Beckhoff's EtherCAT SubDevice Controller (ESC) documentation which contains ESC register descriptions.
</tr>
<tr>
    <td>[Application Note ET9300 (EtherCAT SubDevice Stack Code)](https://download.beckhoff.com/download/document/io/ethercat-development-products/an_et9300_v1i8.pdf)
    <td>This contains details on how to start EtherCAT SubDevice development with SubDevice Stack Code.
</tr>
<tr>
    <td>[EtherCAT SubDevice Implementation Guide from EtherCAT Technology Group](https://www.ethercat.org/download/documents/ETG2200_V3i1i0_G_R_SlaveImplementationGuide.pdf)
    <td>This contains information on how to develop an EtherCAT SubDevice implementation.
</tr>
<tr>
    <td>[EtherCAT SubDevice Design Quick Guide from Beckhoff](https://download.beckhoff.com/download/document/io/ethercat-development-products/ethercat_slave_design_quick_guide.pdf)
    <td>This contains information on modifying PDO when using Beckhoff SSC Tool for code generation.
</tr>
</table>

## See also

\cond SOC_AM64X || SOC_AM243X
- [EtherCAT SubDevice Demos](\ref EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_SLAVE_DEMOS)
\endcond
- [EtherCAT SubDevice Beckhoff SSC Example](\ref EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_SLAVE_BECKHOFF_SSC_DEMO)
