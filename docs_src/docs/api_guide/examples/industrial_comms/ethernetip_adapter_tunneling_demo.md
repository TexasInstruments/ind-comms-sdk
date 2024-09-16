# EtherNet/IP Adapter Intercore Tunneling {#EXAMPLES_INDUSTRIAL_COMMS_ETHERNETIP_ADAPTER_TUNNELING_DEMO_HOME}

# Introduction {#ETHERNETIP_TUNNELING_INTRODUCTION}

This example shows about how to use shared memory based Intercore driver for data traffic sharing coupled with ethernet driver (ICSS_EMAC) and LwIP bridge. It uses ICSS_EMAC ethernet driver underneath with LwIP TCP/IP networking stack

The example demonstrates the driver feature to exchange ethernet (IEEE 802.1) packets between r5fss0-0 and a53 core (linux), via shared memory and ethernet bridge (LwIP software bridge). CPU core r5fss0-0 fully owns & configures ICSSG1 peripheral resource to handle packet reception/transmission directly from/to ICSSG1. It steers the traffic to linux based on MAC address, using LwIP bridge. Seperate MAC addresses are used to differentiate different CPU core traffic. Seperate TCP/IP (LiWP) stack instances are setup in each core. Furthermore, at application level TCP echo servers are initialized on socket port 8888, in each core to handle the TCP traffic.

Linux get the packet from ICSSG1 via LwIP bridge running on core r5fss0-0, using shared memory driver.

Few details on the operating sequence of this example is mentioned below:

- CPU r5fss0-0 initializes the ethernet driver (LwIP netif 0) and shared memory (LwIP netif 1) driver. Setups LwIP bridge with bridgeIF, linked to netif0 and netif1.
- Linux runs on the Cortex A-core and initializes the shared memory driver and links to LwIP netif. Linux logs are directed to UART0 terminal, EtherNet/IP specific logs are directed to UART1 terminal and the LwIP specific logs are directed to the Console.
- CPU core r5fss0-0 and Linux are referred respectively as main core and remote core in the example.  
- Both r5fss0-0 and linux use RPMsg to intercommunicate the control messages, in addition to shared memory interface (which is used to communicate packet data).
- TCP Server tasks running on each core awaits for connection from external client on port 8888. When connection is established, it waits for a message from the connected client.
  In response to external clients message. TCP Server task sends back "Greetings from Texas Instruments!" message back to client and closes the connection.
- Supports iPerf TCP and UDP performance tests

# Peripheral Resource Sharing Layout

 Resource          | Owner     | Remarks
 ------------------|-----------|-----------
 CPSW peripheral   | A53       | Second Port (RGMII2) of CPSW is disabled, to allow ICSSG1 to access RGMII2 
 ICSSG1            | R5F0_0    | Both the ports/RGMIIs are owned by R5F0
 I2C0              | R5F0_0    | For EEPROM communication
 I2C1, I2C2, I2C3  | A53       | Control of the muxed port between CPSW and ICSSG
 OSPI0             | R5F0_0    | For EtherNet/IP stack
 USART0            | A53       | For Linux logs
 USART1            | R5F0_0    | For EtherNet/IP specific logs, LwIP logs are directed to the CCS console
 Other peripherals | A53       | Any other peripherals that are not mentioned above

# Intercore driver setup

-# Intercore driver is a shared memory based data routing layer used to send data traffic across cores.
-# This needs two unique channels open between given pair of cores.
-# This needs shared memory space, which is placed in a commonly accessible region.
-# The region and the channel depth can be adjusted through application code.
-# This uses custom pBufs which helps in decreasing the memory footprint on other dependent libraries. 
-# This example can be modified to work in tandem with r5fss0_1 core as well. Refer to \htmllink{@VAR_MCU_SDK_DOCS_PATH/EXAMPLES_ENET_INTERCORE_ICSSG.html, ENET intercore traffic tunneling example} (or TI E2E forums) for more details.

# Steps to Run the Example {#ETHERNETIP_TUNNELING_EXAMPLE_QUICK_START}

## Build the example

- When using CCS projects to build, import the CCS project for the required combination
  and build it using the CCS project menu (see \htmllink{@VAR_MCU_SDK_DOCS_PATH/CCS_PROJECTS_PAGE.html, Using SDK with CCS Projects}).
- When using makefiles to build, note the required combination and build using
  make command (see \htmllink{@VAR_MCU_SDK_DOCS_PATH/MAKEFILE_BUILD_PAGE.html, Using SDK with Makefiles})

## HW Setup

\note Make sure you have setup the EVM with cable connections as shown here, \htmllink{@VAR_MCU_SDK_DOCS_PATH/EVM_SETUP_PAGE.html, EVM Setup}.
      In addition do below steps.

\cond SOC_AM64X

### AM64X-EVM

- Switch the EVM to SD BOOT MODE to boot via SD card on the EVM.
- Connect a ethernet cable to the EVM from host PC as shown below

  \imageStyle{am64x_evm_lwip_example_01.png,width:30%}
  \image html am64x_evm_lwip_example_01.png Ethernet cable for ICSS based ethernet

\endcond

## Run the example

\attention Initialization of the board is done through SPL. Linux (A53) will boot up first and then it loads the binaries for remote core (R5F).

To run this demo, Linux needs to run on the Cortex A-core. The Linux SDK used in the below steps is <a href="https://www.ti.com/tool/download/PROCESSOR-SDK-LINUX-AM64X/09.02.00.08" target="_blank"> TI Processor SDK v09.02.00.08 </a>
- Assuming the boot media is SD card, refer to **Processor SDK Linux** user guide on how to create SD cards to boot up linux.
- Since the bootup is through Linux, the ICSSG peripheral ownership would be with Linux by default. In order to move the ownership of the ICSSG peripheral, the resource allocation must be updated.
- Replace the **rm-cfg.yaml** in ``board/ti/am64x/`` folder of the Processor SDK U-Boot directory with the file located in ``<ind_comms_sdk>/networking_tunneling_patches`` directory.
\note To make custom changes to the rm-cfg file, follow the steps mentioned in <a href="https://e2e.ti.com/support/processors-group/processors/f/processors-forum/1308689/faq-how-to-use-k3-resource-partitioning-tool-with-processor-sdk-v9-0-or-v9-1/4970549#4970549" target="_blank"> How to use Resource partitioning tool </a> with Processor SDK v9.1. Update the Resource Allocation table by following the steps in <a href="https://e2e.ti.com/support/processors-group/processors/f/processors-forum/1206420/processor-sdk-am64x-enet-open-failure-from-r5-when-a53-is-running/4596608#4596608" target="_blank"> ICSSG resource allocation </a> and replace the rm-cfg.yaml file of the U-boot directory with the one generated by the tool.
- Re-build the U-boot binaries and replace the required files in the SD card (see <a href="https://dev.ti.com/tirex/explore/node?node=A__ATXEBw5Oq0glgUg1.45oag__AM64-ACADEMY__WI1KRXP__LATEST" target="_blank"> Build U-boot</a>)
- Apply the Linux kernel patch located in ``<ind_comms_sdk>/networking_tunneling_patches`` directory, build the kernel and replace the required files in the SD card (see <a href="https://dev.ti.com/tirex/explore/node?node=A__AceFRv7J1nztrzmj7EDcRg__AM64-ACADEMY__WI1KRXP__LATEST" target="_blank"> Build Kernel</a>).
- Copy the generated application .out file into the Linux filesystem under ``/lib/firmware`` in the SD card "root" partition.
- Create a softlink to the RTOS firmware with the command below. Refer <a href="https://dev.ti.com/tirex/explore/node?a=7qm9DIS__LATEST&node=A__AdAyuKWUWVV5j4wBc7C6XA__AM64-ACADEMY__WI1KRXP__LATEST" target="_blank"> AM64x Academy Booting Remote Cores</a> for more details on booting remote cores during Linux boot.
\code
$ ln -sf /lib/firmware/<generated_application.out> am64-main-r5f0_0-fw
\endcode

Once all the required files are copied to the SD card,

- Insert the prepared SD card in the EVM's SD card slot and <b>POWER-ON</b> the EVM.
- You will see the Linux and EtherNet/IP application logs on UART0 and UART1 terminal respectively.

### Virtual interface configuration
Once Linux is booted up on the AM64x, intercore virtual ethernet interface should be up. By default, no MAC address is assigned to the port.

\note Since MAC address is not set to the virtual port interface, it is mandatory to set a MAC address.

#### Steps to add MAC Address
\code
#Bring down the virtual port interface
$ ifconfig eth1 down

#Set MAC address for the virtual port interface, ex 01:02:03:04:05:06
$ ifconfig eth1 hw ether 01:02:03:04:05:06

#Bring the interface up
$ ifconfig eth1 up

\endcode

#### Steps to assign IP address 
\note Since the example currently supports only Static IP configuration, it is mandatory to set an IP address.

\code
#For static IP mode, assign IP address to the virtual port interface, ex 192.168.1.20
$ ifconfig eth1 192.168.1.20
\endcode

#### Steps to work with Multicast Addresses
\code
#Add the multicast address 01:80:c4:00:00:0e
$ ip maddr add dev eth1 01:80:c4:00:00:0e

#Delete the multicast address 01:80:c4:00:00:0e
$ ip maddr del dev eth1 01:80:c4:00:00:0e

#Display current Multicast Address
$ ip maddr show dev eth1

\endcode

### Sample Output

#### Linux logs
\code
Arago 2023.10 am64xx-evm -

am64xx-evm login: root

root@am64xx-evm:~# 
root@am64xx-evm:~# ifconfig eth1 down
root@am64xx-evm:~# ifconfig eth1 hw ether 01:02:03:04:05:06
root@am64xx-evm:~# ifconfig eth1 up
root@am64xx-evm:~# [   64.653993] inter_core_virt_eth virtio1.ti.icve.-1.13 eth1: tx_buffer->head = 0xa0500000
[   64.662139] inter_core_virt_eth virtio1.ti.icve.-1.13 eth1: tx_buffer->buf = 0xa0500008
[   64.670152] inter_core_virt_eth virtio1.ti.icve.-1.13 eth1: tx_buffer->tail = 0xa0518308
[   64.678270] inter_core_virt_eth virtio1.ti.icve.-1.13 eth1: port->icve_tx_max_buffers = 64
[   64.686550] inter_core_virt_eth virtio1.ti.icve.-1.13 eth1: rx_buffer->head = 0xa0400000
[   64.694680] inter_core_virt_eth virtio1.ti.icve.-1.13 eth1: rx_buffer->buf = 0xa0400008
[   64.702705] inter_core_virt_eth virtio1.ti.icve.-1.13 eth1: rx_buffer->tail = 0xa0418308
[   64.710816] inter_core_virt_eth virtio1.ti.icve.-1.13 eth1: port->icve_tx_max_buffers = 64
[   64.821932] IPv6: ADDRCONF(NETDEV_CHANGE): eth1: link becomes ready

root@am64xx-evm:~# ifconfig eth1 192.168.1.20
root@am64xx-evm:~# 

\endcode

#### EtherNet/IP logs

Once Linux is loaded onto the A53 cores of AM64x and the RemoteProc driver boots the remote core with our application binaries during Linux kernel boot time. The following logs appear on UART1.

\code
Pruicss  max =3 selected PRU:3
Did Map 0x30080000 len 0x2000 to 0x30080000 (dram0)
Did Map 0x30082000 len 0x2000 to 0x30082000 (dram1)
Did Map 0x300b4000 len 0x4000 to 0x300b4000 (iram0)
Did Map 0x300b8000 len 0x4000 to 0x300b8000 (iram1)
Did Map 0x30090000 len 0x10000 to 0x30090000 (shdram)
Did Map 0x300a2000 len 0x400 to 0x300a2000 (control0)
Did Map 0x300a4000 len 0x400 to 0x300a4000 (control1)
Did Map 0x300a0000 len 0x2000 to 0x300a0000 (intc)
Did Map 0x300a6000 len 0x2000 to 0x300a6000 (cfg)
Did Map 0x300a8000 len 0x2000 to 0x300a8000 (uart0)
Did Map 0x300ae000 len 0x2000 to 0x300ae000 (iep)
Did Map 0x300b0000 len 0x2000 to 0x300b0000 (ecap0)
Did Map 0x300b2000 len 0x400 to 0x300b2000 (mii_rt)
Did Map 0x3009f000 len 0x1c00 to 0x3009f000 (mdio)
Did Map 0x3008a000 len 0x2000 to 0x3008a000 (txPru0Iram)
Did Map 0x3008c000 len 0x2000 to 0x3008c000 (txPru1Iram)
Did Map 0x300a5000 len 0x100 to 0x300a5000 (txPru0CtlReg)
Did Map 0x300a5000 len 0x100 to 0x300a5000 (txPru1CtlReg)
DP83869 detected
DP83869 detected
PRU_PHY_detect:185 Phy 3 alive
PRU_PHY_detect:185 Phy 15 alive
Phy 15 : Disable GBit ANEG
Phy 3 : Disable GBit ANEG
Phy 15 : RGMII enable low latency
Phy 3 : RGMII enable low latency
Phy 15 : RGMII set TX Half/Full Threshold: 1
Phy 3 : RGMII set TX Half/Full Threshold: 1
Phy 15 : RGMII set RX Half/Full Threshold: 1
Phy 3 : RGMII set RX Half/Full Threshold: 1
+EI_API_ADP_pruicssStart
-EI_API_ADP_pruicssStart
Local interface IP is 192.168.1.10
EI_API_ADP_getMacAddr:  aa:bb:cc:dd:ee:ff
\endcode

#### LwIP logs

Once the linux boot completes and the remote core is running. The LwIP logs, CPU load prints and results of the iPerf test performed with the main core appear on the Console. To view the Console logs follow the steps below:
- Connect to MAIN_Cortex_R5_0_0 from the "Debug" window of AM64X target configuration.
- After connecting CCS to the remote core, **DO NOT reset the remote core**.
- Instead, select Load > Load Symbols from the "Run" menu and select the required .out for "ethernetip adapter tunneling demo" program.

\note Since the remote core starts running once its binaries are loaded, you will miss the initial Console logs by the time you load symbols through CCS. 

CCS provides the option to halt the remote core at a particular location in code and start the execution manually by adding an infinite loop (see <a href="https://dev.ti.com/tirex/explore/node?a=7qm9DIS__LATEST&node=A__AZVLTFgnCWyMV5cDwLf4gg__AM64-ACADEMY__WI1KRXP__LATEST" target="_blank"> Debug the remote core through CCS</a>) 

To get the complete logs, an infinite loop can be used to halt the code and the execution can be resumed after loading the symbols on the core from CCS.

## Communicate with the EVM using ethernet

\note Ping to the bridgeIF is not supported in the out-of-box example.

- You can try to reach the EVM using ping as shown below, using a command shell on the host PC

        > ping 192.168.1.20

- TCP and UDP Performance analysis can be performed. R5F application supports iperf2 and Linux supports iperf3.

## iPerf Performance

\attention The running CPU load prints degrade the performance and cause packet loss due to the CPU heavy string formatting (printf, snprintf) done in the reporting functions.

### UDP Throughput
 Direction                   | Main core               | Remote Core              
 ----------------------------|-------------------------|--------------------------
 From Network; To core       | 95 Mbps @ 100% CPU load | 95 Mbps @ 40% CPU load 
 From Core; To Network       | N/A                     | 22 Mbps @ 20% CPU load

### TCP Throughput
 Direction                   | Main core               | Remote Core              
 ----------------------------|-------------------------|--------------------------
 From Network; To core       | 91 Mbps @ 100% CPU load | 94 Mbps @ 45% CPU load 
 From Core; To Network       | 41 Mbps @ 70%  CPU load | 5 Mbps  @ 10% CPU load 

## Troubleshooting issues

- If you see a valid MAC address and `link_callback==DOWN` is seen then
   - check the ethernet cable if it is properly connected
- For more information on how to debug an application on a remote core, running alongside a Linux application, refer <a href="https://dev.ti.com/tirex/explore/node?a=7qm9DIS__LATEST&node=A__AU9Punu4yTQu9hRP62aoug__AM64-ACADEMY__WI1KRXP__LATEST" target="_blank"> AM64x Multicore academy</a>

# See Also
\ref ETHERNETIP_ADAPTER_FWHAL