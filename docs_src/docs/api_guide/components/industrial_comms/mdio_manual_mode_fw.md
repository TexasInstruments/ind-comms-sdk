# PRU-ICSS Firmware for MDIO Manual Mode {#INDUSTRIAL_COMMS_MDIO_MANUALMODE_FW_USAGE}

## Hardware Issue
If your silicon is affected with errata <a href="https://www.ti.com/lit/er/sprz457e/sprz457e.pdf" target="_blank">i2329— MDIO interface corruption</a>, then TI suggests to use MDIO Manual mode as software workaround mentioned below.

## Working of PRU-ICSS Firmware for MDIO Manual Mode

- PRU Firmware Code Flow
  - Initial configurations:
    1. Configures the MDIO hardware in 'MDIO_MODE_MANUAL' and keeps the MDIO internal state machine disabled.
    2. Configures the MDIO hardware by copying over the MDIO register configuration values from emulated register space to the actual MDIO MMR addresses.
    3. Initializes the PRU CPU registers for use throughout emulation.
  - Continuous Operation:  
    1. Firmware uses the new defined/emulated register space (location offset in ICSSG SRAM) to emulate the functionality of actual MDIO MMR registers.
    2. For MDIO_USER_ACCESS_REG_0/1 emulation: The firmware runs in a loop, and checks if there are any pending PHY access requests by checking the `GO` bit of the user-access registers from emulated register space. If there is any pending PHY access request, it performs the operation and clears the `GO` bit of the user-access register (and sets `ACk` bit as per the response from MDIO interface).
    3. The firmware manually drives the MDIO_DATA and MDIO_CLK pins using MANUAL_IF_REG MMR register of MDIO HW IP to perform the PHY access operations (in clause22 mode only). By default, the MDIO_CLK frequency is fixed around 2.5MHz - this can be changed by modifying and rebuilding PRU firmware.
    4. It emulates the ICSSG_ALIVE_REG register by regularly polling the value of LINK_STS1 bit of PHY BMSR register for PHY addresses configured in MDIO_USER_PHY_SEL_0/1_REG. 
    5. It emulates the ICSSG_LINK_REG register as: 
        - **If set in polling based link detection mode**, it updates the Link Status for PHYs configured in MDIO_USER_PHY_SEL_0/1_REG by regularly polling value of LINK_STS1 bit of PHY BMSR register of the PHYs. Also, if link change interrupt is enabled it generates PHY Link Change interrupt on same interrupt line as the MDIO HW IP.
          - Since this is polling based, no need to connect MLINK pins.
        - **If set in MLINK based link detection mode**, it updates the Link Status for PHYs configured in MDIO_USER_PHY_SEL_0/1_REG by reading from MDIO HW MMR register ICSSG_LINK_REG and copies it to emulated register space. Also, if link change interrupt is enabled it is handled by the MDIO HW IP.
          - MLINK connection from PHY to Device is required in this mode.
        <br> ** In MLINK mode link detection, the application should access link/interrupt status information and clear link change interrupts directly from the MDIO HW MMRs. This is to avoid any delays introduced during the emulation. Explained later in Example Usage section. 

\imageStyle{MDIO_emulation.png,width:30%}
\image html MDIO_emulation.png ""

## SDK9.0 VS SDK8.6 CHANGES

- No interface changes from the application/R5F side
- The firmware has been updated as:
  - Removed initial enabling of MDIO State Machine for a short period to avoid a glitch in MCLK line in start.
  - Modified the MDIO and MCLK signals to more closely mimic MDIO HW IP signals.

Change in MDIO link detection configuration support in release:
| Protocol   | Options in 9.0 Release           | Options in 8.6 Release           |
|------------|----------------------------------|----------------------------------|
| EtherCAT   | MLINK (default) <br> Polling     | MLINK (default) <br> Polling     |
| EthernetIP | MLINK <br> Polling (default)     | MLINK <br> Polling (disabled)    |
| Profinet   | MLINK <br> Polling (default)     | MLINK <br> Polling (disabled)    |
| HSR-PRP    | MLINK <br> Polling (default)     | MLINK <br> Polling (default)     |

## Usage Details
This section describes how to use the PRU MDIO manual-mode firmware to make PHY register accesses. In SDK examples mentioned above, this has already been integrated so those can be used as reference. 

### Sysconfig

Enable the manual mode option from SysConfig from the protocol specific module. On enabling this option, SysConfig updates the MDIO base address in generated files which will be used by the ETHPHY driver.

\imageStyle{MDIO_manual_mode.PNG,width:50%}
\image html MDIO_manual_mode.PNG " "

### Loading and Running PRU firmware

For loading MDIO Manual MDIO firmware to PRU Core, please follow the following steps:
- Make sure any MDIO access is made only after the firmware has been loaded and run.
- Clear the register space which will be used by firmware for MDIO register space emulation.
- Load the PRU firmware to the appropriate core (in SDK examples we load the code to TX_PRU0 cores).
- Set the PRU R10 register for fw configuration as explained below. (default configuration will be MLINK Based link detection and MDIO Clk acc. to 200MHz PRU Clock):
SysConfig generates a macro: `MDIO_MANUAL_MODE_FW_CONFIG_VALUE` containing this configuration which can be directly used to write to R10 CPU register of PRU.

\code

------------------------------------------------------------------------------
|  R10 register's bits will be used to configure the firmware working modes  |
------------------------------------------------------------------------------
| Bit       | Description                                                    |
| R10 bit 0 | Link detection method                                          |
|           |    0 : MLINK based                                             |
|           |    1 : Polling based                                           |
| R10 bit 1 | Link status change interrupt for Polling mode                  |
|           |    0 : disabled                                                |
|           |    1 : enabled                                                 |
| R10 bit 2 | Configure MDIO Clk acc. to PRU CLK = 250MHz (default 200MHz)   |
|           |    0 : disabled                                                |
|           |    1 : enabled                                                 |
| R10 bit 3 | Configure MDIO Clk acc. to PRU CLK = 333MHz (default 200MHz)   |
|           |    0 : disabled                                                |
|           |    1 : enabled                                                 |
| R10.bit 4 | Preamble Suppression                                           |
|           |    0 : disabled (will send Preamble)                           |
|           |    1 : enabled  (will not send Preamble)                       |
------------------------------------------------------------------------------

\endcode

- The emulated register space start address has to be passed to the firmware by setting the R12 CPU register to the desired location for emulating the MDIO register space. The start address should be an offset from start of that ICCSG instance memory space and located in SMEM.
    - For example, if we set R12 to 0x00010000 (ICSSG shared memory offset) for ICSSG1 instance, the MDIO registers will start from 0x30090000 and so the user access registers' offsets will be (0x30090000+0x80) for USER_ACCESS_0_REG and (0x30090000+0x88) for USER_ACCESS_1_REG.
    - The emulated registers offset remain same as MDIO HW MMRs, only the base address is changed to the above defined address. 
- Enable/run the PRU core and then we can create PHY access requests using emulated MDIO User Access Register.

\note Each protocol will have different base register offset configured depending on the availability of free memory region. Emulated MDIO register space start address will be visible from the SysConfig interface as well. See below for more information.

### MDIO Emulated Register Space Base Addresses 
Current emulated base addresses for MDIO emulated space:

| Protocol   | Global Address | Global Address | Local Address |
|------------|----------------|----------------|---------------|
|     -      | ICSSG0         | ICSSG1         | (From ICSSG)  |
| EtherCAT   | 0x30010E40     | 0x30090E40     |  0x00010E40   |
| Profinet   | 0x3001FF00     | 0x3009FF00     |  0x0001FF00   |
| HSR-PRP    | 0x3001FF00     | 0x3009FF00     |  0x0001FF00   |
| EthernetIP | 0x3001FF00     | 0x3009FF00     |  0x0001FF00   |

### Example Usage {#INDUSTRIAL_COMMS_MDIO_MANUALMODE_FW_EXAMPLE_USAGE}
Check the function `tiesc_mdioManualModeSetup()` in `tiescsoc.c` at location `${SDK_INSTALL_PATH}/examples/industrial_comms/ethercat_slave_beckhoff_ssc_demo/am64x-evm/tiescsoc.c` as reference setup guide and for understanding how to load and run the PRU firmware.

<!-- ICSSG PRU cores usage image -->
\imageStyle{Industrial_protocols_cores_usage.png,width:30%}
\image html Industrial_protocols_cores_usage.png " "

By default the MDIO manual mode firmware is loaded on TX_PRU core, but we can load the same firmware on other unused RTU_PRU or TX_PRU cores if needed.

Differences and Usage Suggestions for Polling and MLINK modes: 

| -                                             |  MLINK Mode                                           |  Polling Mode                                        |
|-----------------------------------------------|-------------------------------------------------------|------------------------------------------------------|
| MDIO Base address register in EthPhy Handle   | Points to emulated MDIO register space base address   | Points to emulated MDIO register space base address  |
| MDIO Base address register in PruIcssg Handle | Points to MDIO HW registers base address **          | Points to emulated MDIO register space base address  |
| PHY register access (read/write) requests     | Use emulated MDIO register space                      | Use emulated MDIO register space                     |
| Get Link Status and Link interrupt handling   | Directly Use MDIO HW registers ***                     | Use emulated MDIO registers                          |

**  This register is unchanged as it is being used for link status/interrupt handling, and so requires to be pointing to MDIO HW address as explained in above point. Make sure this is not changed. <br>
***   This is required because link interrupt clearing and link status updates in emulated MDIO registers gets delayed and this can cause issues. 

Default Link detection configurations in SDK Examples are:

| Protocol   | Options                          |
|------------|----------------------------------|
| EtherCAT   | MLINK (default) <br> Polling     |
| EthernetIP | MLINK <br> Polling (default)     |
| Profinet   | MLINK <br> Polling (default)     |
| HSR-PRP    | MLINK <br> Polling (default)     |

### Modifying PRU firmware

The PRU firmware can be modified and built using the example present at `${SDK_INSTALL_PATH}/examples/pru_io/mdio_fw`. This will be helpful if you want to make some changes to the firmware for example operating the MCLK at different frequency. 
    1. Import the example in CCS from the above specified location
    2. Modify and Build it. After rebuilding the project, mdio_fw_bin.h file gets generated in Debug folder (in CCS). From this file you can copy contents of the generated firmware array to `<sdk_path>\source\industrial_comms\<project>\icss_fwhal\firmware\mdio_fw_bin.h` file.
    3. Rebuild the application to use updated firmware.

    For changing MLCK frequency:
    - You can find the assembly macros that are used for communication with the PHYs in `${SDK_INSTALL_PATH}/source/pru_io/firmware/common/mdio_macros.inc`, specifically: m_mdio_read, m_mdio_write macros
    - In these read/write macros, m_mdio_toggle_bit_lh is used to drive MCLK pin and we wait inside this macro for low and high pulse widths of clock, which is decided by value of `MDIO_CLK_PERIOD_CONFIG_REG`
    - To change communication frequency (MCLK frequency), change the value of register `MDIO_CLK_PERIOD_CONFIG_REG` defined in example's `main.asm` file.
    - Other timing requirements are handled by how and when the MCLK and MDATA pins are driven by the firmware, which you can check in these macros.

### Using User Access Registers
Creating PHY access requests from the R5F core remains same as all the configuration settings (for changing the MDIO base address) are modified by SysConfig. There is no change in the MDIO driver APIs, only the base address has to change for the API calls.

## Impact
- One PRU is now reserved for emulating MDIO Manual Mode functionality.
- For MDIO API Usage, the base address is now changed. \ref MDIO_initClock API call is not needed.
- Following Registers are not available when using MDIO manual mode firmware (from the emulated register space):
    -   MDIO_VERSION_REG (MDIO Version Register)
    -   MDIO_CONTROL_REG (MDIO Control Register)
    -   MDIO_LINK_INT_RAW_REG (MDIO Link Interrupt Raw Register)
    -   MDIO_LINK_INT_MASKED_REG (MDIO Link Interrupt Masked Register)
    -   MDIO_LINK_INT_MASK_SET_REG (MDIO Link Interrupt Mask Set Register)
    -   MDIO_LINK_INT_MASK_CLEAR_REG (MDIO Link Interrupt Mask Clear Register)
    -   MDIO_USER_INT_RAW_REG (MDIO User Interrupt Raw Register)
    -   MDIO_USER_INT_MASKED_REG (MDIO User Interrupt Masked Register)
    -   MDIO_USER_INT_MASK_SET_REG (MDIO User Interrupt Mask Set Register)
    -   MDIO_USER_INT_MASK_CLEAR_REG (MDIO User Interrupt Mask Clear Register)
    -   MDIO_MANUAL_IF_REG (MDIO Manual Interface Register)
    -   MDIO_POLL_REG (MDIO Poll Inter Register)
    -   MDIO_POLL_EN_REG (MDIO Poll Enable Register)
    -   MDIO_CLAUS45_REG (Clause 45 Register)
    -   MDIO_USER_ADDR0_REG (MDIO User Address 0 Register)
    -   MDIO_USER_ADDR1_REG (MDIO User Address 1 Register)

- Only following registers are available when using MDIO manual mode firmware (from the emulated register space):
    - MDIO_ALIVE_REG (MDIO Alive Register)
    - MDIO_LINK_REG (MDIO Link Register)
    - MDIO_USER_ACCESS_REG_0 (MDIO User Access 0 Register)
    - MDIO_USER_PHY_SEL_REG_0 (MDIO User PHY Select 0 Register)
    - MDIO_USER_ACCESS_REG_1 (MDIO User Access 1 Register)
    - MDIO_USER_PHY_SEL_REG_1 (MDIO User PHY Select 1 Register)


## Constraints and Limitations
- In MLINK Mode, MLINK pin (same as RX_LINK pins) is necessary for MDIO_LINK_REG updates. This pin indicated the link status to the AM64 using dedicated signal lines. This should be connected to appropriate ICSSG pins.

    \cond SOC_AM243X

    \note

    In the E1/E2 revision of @VAR_LP_BOARD_NAME_LOWER board, the pins PR1_MII0_RXLINK and PR1_MII1_RXLINK are not connected which are required for link detection.
    Only in E3 revision, these connections are available. Therefore MLINK based link detection can not be used on E1/E2 revisions.
    \endcond

- In Link Polling mode - enabled through sysconfig, we do not require RX_LINK pins for link status update as firmware polls and updates the link status of specified PHYs.
- Only Clause 22 frame format is supported. Clause 45 frame format is not supported.
- MDIO state machine is disabled and hence MDIO LINK_INT and USER_INT interrupts are NOT available.

## Important files and directory structure

<table>
<tr>
    <th>Folder/Files
    <th>Description
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/examples/pru_io/</td></tr>
<tr>
    <td>mdio_fw/
    <td>Contains the project files needed for MDIO manual mode firmware </td>
</tr>
<tr><td colspan="2" bgcolor=#F0F0F0> ${SDK_INSTALL_PATH}/source/pru_io/</td></tr>
<tr>
    <td>firmware/common/mdio_macros.inc
    <td>Contains the firmware sources needed for MDIO manual mode firmware </td>
</tr>
</table>
