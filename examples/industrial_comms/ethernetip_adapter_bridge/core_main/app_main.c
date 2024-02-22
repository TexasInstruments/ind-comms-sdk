/*
 *  Copyright (c) Texas Instruments Incorporated 2024
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file  app_main.c
 *
 * \brief This file contains the implementation of the Enet Remote Connect example for main core.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "FreeRTOS.h"
#include "task.h"
#include <kernel/dpl/TaskP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/SemaphoreP.h>
#include "ti_board_config.h"
#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include <examples/lwiperf/lwiperf_example.h>

/* EMAC includes */
#include "hsr_prp_menu.h"
#include <hsr_prp_soc.h>
#include "hsr_prp_prvmib.h"
#include "hsr_prp_lwipcfg.h"
#include <networking/icss_emac/icss_emac.h>
#include <networking/icss_emac/lwipif/inc/lwip2icss_emac.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_firmwareOffsets.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_red_nodeTable.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_red_snmp.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_red_statistics.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_red_config.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_red.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_red_multicastTable.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_red_vlanTable.h>
#include <industrial_comms/hsr_prp/icss_fwhal/hsrPrp_handle.h>

#include <drivers/sciclient.h>
#include <drivers/udma/udma_priv.h>
#include <lwip2icss_emac.h>

#include "app_tcpserver.h"
#include "emac_lwipif.h"
#include "enet_netific.h"
#include "app_announce.h"
#include "app_netif.h"
#include "udp_iperf.h"

#include <industrial_comms/profinet_device/icss_fwhal/firmware/icss_emac_mmap.h>
#include <industrial_comms/profinet_device/icss_fwhal/tiswitch_pruss_intc_mapping.h>
//PN_SPECIAL_UNICAST_TEST_INCLUDES
#include <industrial_comms/profinet_device/icss_fwhal/iPNDrv.h>
#include <industrial_comms/profinet_device/icss_fwhal/PN_HandleDef.h>
#include <industrial_comms/profinet_device/icss_fwhal/PN_Handle.h>
#include <industrial_comms/profinet_device/icss_fwhal/iPNLegacy.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

//BALUFF - DEBUG
//Function to get the MAC address
#define PRU_PN_MAC_ADDR_LEN             6
static uint8_t ai8uIntfMacAddress_s[PRU_PN_MAC_ADDR_LEN];
/**Interface macid index*/
#define INTERFACE_MAC_TEMP 0
/**Port1 macid index*/
#define PORT1_MAC_TEMP     1
/**Port2 macid index*/
#define PORT2_MAC_TEMP     2

/* Max number of ports supported per context */ //- check
#define ENETMP_PORT_MAX                          (ENET_SYSCFG_NUM_EXT_MAC_PORTS)

/* Max number of hardware RX channels. Note that this is different than Enet LLD's
 * RX channel concept which maps to UDMA hardware RX flows */
#define ENETMP_HW_RXCH_MAX                       (2U)
#define ENETMP_HW_RXFLOW_MAX                     (8U)

/* Local flag to disable a peripheral from test list */
#define ENETMP_DISABLED                          (0U)

/* Max number of ports that can be tested with this app */
#define ENETMP_ICSSG_INSTANCE_MAX                (2U)
#define ENETMP_PER_MAX                           (ENETMP_ICSSG_INSTANCE_MAX)

/* Task stack size */
#define ENETMP_TASK_STACK_SZ                     (10U * 1024U)

/* 100-ms periodic tick */
#define ENETMP_PERIODIC_TICK_MS                  (100U)

/*Counting Semaphore count*/
#define COUNTING_SEM_COUNT                       (10U)

/*Maximum Tx channels allowed*/
#define ENETMP_MAX_TX_CHANNEL_NUM                (8U)

/* UDP Iperf task should be highest priority task to ensure processed buffers
 * are freed without delay so that we get maximum throughput for
 * UDP Iperf.
 */
#define UDP_IPERF_THREAD_PRIO  (14U)

//NEW 
static uint8_t IntfMacAddress_s[PRU_PN_MAC_ADDR_LEN];

#define PN_RX_INT_OFFSET            (0)
#define PN_PPM_INT_OFFSET           (1)
#define PN_CPM_INT_OFFSET           (2)
#define PN_DHT_INT_OFFSET           (3)
#define PN_PTCP_INT_OFFSET          (4)
#define PN_LINK_INT_OFFSET          (6)
#define PN_ISOM_INT_OFFSET          (7)

#define PRU_RX_INT_NUM        20
#define PRU_LINK_INT_NUM      26
#define TIEIP_MDIO_CLKDIV   79 //For 2.5MHz MDIO clock: 200/(TIESC_MDIO_CLKDIV+1)
#define PRU_PN_PTCP_TASK_PRIO                       11
#define PRU_PN_PTCP_SYNC_MONITOR_TASK_PRIO          11
#define PRU_PN_LINK_TASK_PRIO                       12
#define PRU_PN_RX_TASK_PRIO                         12

#define PN_PTCP_TIMERID         0 // TimerP_ANY
#define PN_PTCP_TIMER_FREQ      25000000

/* EEPROM data offset in I2C EEPROM Flash */
#define I2C_EEPROM_DATA_OFFSET          (0x8000)
#define I2C_EEPROM_MAC_DATA_OFFSET      (0x42)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

// static EnetApp_AppEnetInfo gEnetAppParams[ENET_SYSCFG_MAX_ENET_INSTANCES];

//PROFINET_UNICAST_TEST PN_Handle
PN_Handle appPnHandle;

/** \brief PRU-ICSS LLD handle */
PRUICSS_Handle prusshandle;
/** \brief ICSSEmac LLD handle */
ICSS_EMAC_Handle emachandle;
/** \brief Time Sync handle */
TimeSync_ParamsHandle_t timeSyncHandle;
/** \brief hsrprp handle */
hsrPrpHandle *hsrPrphandle;
/** \brief LwIP Interface layer handle */
Lwip2Emac_Handle Lwipif_handle;
/* EMAC mac-addr */
uint8_t lclMac[6];

//FIX ME - check if dead code

// struct netif *g_pNetif[2];
// /* dhcp struct for the ethernet netif */
// static struct dhcp g_netifDhcp[ENET_SYSCFG_NETIF_COUNT];
// err_t bridgeif_status = ERR_OK;
// struct netif *bridge_netif;

/** Temporary placeholder to copy packets */
uint8_t tempFrame[ICSS_EMAC_MAXMTU] __attribute__((aligned(32)));

/**PTP MAC ID for comparison*/
uint8_t timeSyncMAC[6] = {0x1, 0x1b, 0x19, 0x0, 0x0, 0x0};
uint8_t linkLocalMAC[6] = {0x1, 0x80, 0xc2, 0x0, 0x0, 0xE};

int collision_pkt_dropped;
int num_of_collision_occured;
uint8_t hsr_prp_testPkt[HSR_PRP_TEST_FRAME_SIZE] = {
    0x6a, 0x3e, 0x26, 0x7a, 0xb2, 0x7d,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
    0x00,0x45, 0x00, 0x00, 0x2E, 0x00,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x3A,
    0xD1
};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

extern void Lwip2Emac_getHandle(Lwip2Emac_Handle *AppLwipHandle);

void appMain(void *args);

static void EnetMp_remoteCoreTask(void *args);

static void App_printCpuLoad();

void PN_socinitIRTHandle (PN_Handle appPnHandle);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
// NEW - PRU_PN_socgetMACAddress
void PRU_PN_socgetMACAddress (
    uint8_t index,
    uint8_t *lclMac)

{
    uint32_t  tmp;

    tmp = (ai8uIntfMacAddress_s[3] << 16) | (ai8uIntfMacAddress_s[4] << 8) |  (ai8uIntfMacAddress_s[5] << 0);

    switch(index)
    {
        case INTERFACE_MAC_TEMP:
            break;
        case PORT1_MAC_TEMP:
            tmp += 1;
            break;
        case PORT2_MAC_TEMP:
            tmp += 2;
            break;
        default:
            break;
    }

    memcpy (lclMac, ai8uIntfMacAddress_s, 3);
    lclMac[3] = tmp >> 16;
    lclMac[4] = tmp >> 8;
    lclMac[5] = tmp >> 0;

}

void PN_getMACAddr(uint8_t index, uint8_t *lclMac)
{
    uint32_t *MAC_0_3 = NULL;
    uint16_t *MAC_4_5 = NULL;
    uint8_t i;
   
    /* Below code shall never be in production environment - to handle boards with uninitialized EEPROM MAC ID */
    MAC_0_3 = (uint32_t *)(IntfMacAddress_s);
    MAC_4_5 = (uint16_t *)(&IntfMacAddress_s[4]);

    if((*MAC_0_3 == 0xFFFFFFFFU) && (*MAC_4_5 == 0xFFFFU))
    {
        DebugP_log("\r\n Warning : Board EEPROM is not initialized correctly with MAC address - please fix this ");
        DebugP_log("\r\n Generating random MAC address for now \n");
        *MAC_0_3 = rand();
        *MAC_4_5 = rand() & 0xFFFF;
        IntfMacAddress_s[0] &= 0xFE; /* Force unicast MAC address */
    }
    switch(index)
    {
        case INTERFACE_MAC:
            break;
        case PORT1_MAC:
            IntfMacAddress_s[5] += 1;
            break;
        case PORT2_MAC:
            IntfMacAddress_s[5] += 2;
            break;
        default:
            break;
    }

    for(i=0;i<6;i++)
    {
        lclMac[i]=IntfMacAddress_s[i];
    }
}

// void PN_initDefaultValues(PN_Handle appPnHandle,ICSS_EMAC_Handle emachandle,PRUICSS_Handle prusshandle)
// {
//     /* Memory allocations */
//     memset(&appPnHandle->pnIntConfig,0,sizeof(PN_IntConfig));
//     appPnHandle->emacHandle = emachandle;
//     appPnHandle->pruicssHandle  = prusshandle;
//     appPnHandle->initRtcDrvFlag = TRUE;
//     appPnHandle->initRtcMemFlag = 0;
//     appPnHandle->mrpState = MRPREADY;
//     appPnHandle->legState = NOINIT;
//     appPnHandle->pnIntConfig.ppmIntConfig.isrFnPtr = &PN_ppmIsrHandler;
//     appPnHandle->pnIntConfig.ppmIntConfig.args = appPnHandle;
//     appPnHandle->pnIntConfig.cpmIntConfig.isrFnPtr = &PN_cpmIsrHandler;
//     appPnHandle->pnIntConfig.cpmIntConfig.args = appPnHandle;
//     appPnHandle->pnIntConfig.dhtIntConfig.isrFnPtr = &PN_dhtIsrHandler;
//     appPnHandle->pnIntConfig.dhtIntConfig.args = appPnHandle;
//     appPnHandle->getMACAddress = &PRU_PN_socgetMACAddress;
// }

void PN_initDefaultValues(PN_Handle appPnHandle,ICSS_EMAC_Handle emachandle,PRUICSS_Handle prusshandle)
{
    /* Assign EMAC and PRU handles */
    appPnHandle->emacHandle = emachandle;
    appPnHandle->pruicssHandle  = prusshandle;

    appPnHandle->pLegPkt = NULL;

    /*configure the pulse width for sync0: 5000+1 cycles i.e. 25us*/
    appPnHandle->pnPtcpConfig.ptcpSync0PinPulseWidth = 5000;
    /*program cmp1 reg with period, used for sync0 signal generation: 10 us*/
    appPnHandle->pnPtcpConfig.ptcpSync0PinStart = 10000;
    appPnHandle->pnPtcpConfig.ptcpSyncFilterfactor = 8;

    appPnHandle->pnIsoMConfig.isoMIntCreateFlag = 0;
    appPnHandle->pnIsoMConfig.isoMNumEvents = 0;
    appPnHandle->pnIsoMConfig.isoMIntConfig.isrFnPtr = NULL;
    appPnHandle->pnIsoMConfig.isoMIntConfig.args = NULL;


    appPnHandle->initRtcDrvFlag = TRUE;
    appPnHandle->initRtcMemFlag = 0;
    appPnHandle->mrpState = MRPREADY;
    appPnHandle->legState = NOINIT;
    appPnHandle->pnPtcpConfig.cycleCtrInitPending = 0;
    appPnHandle->pnPtcpConfig.calculatedCycleCtr = 0;
    appPnHandle->pnPtcpConfig.masterChange = 0;
    appPnHandle->pnPtcpConfig.phaseCtrChange = 0;
    appPnHandle->pnPtcpConfig.maxSeqId = 0;
    appPnHandle->pnPtcpConfig.minSeqId = 0;

    appPnHandle->pnIntConfig.ppmIntConfig.isrFnPtr = &PN_ppmIsrHandler;
    appPnHandle->pnIntConfig.ppmIntConfig.args = appPnHandle;
    appPnHandle->pnIntConfig.cpmIntConfig.isrFnPtr = &PN_cpmIsrHandler;
    appPnHandle->pnIntConfig.cpmIntConfig.args = appPnHandle;
    appPnHandle->pnIntConfig.dhtIntConfig.isrFnPtr = &PN_dhtIsrHandler;
    appPnHandle->pnIntConfig.dhtIntConfig.args = appPnHandle;
#ifdef PTCP_SUPPORT
    appPnHandle->pnIntConfig.ptcpIntConfig.isrFnPtr = &PN_PTCP_isrHandler;
    appPnHandle->pnIntConfig.ptcpIntConfig.args = appPnHandle;
#endif
    appPnHandle->getMACAddress = &PN_getMACAddr;
    PN_socinitIRTHandle(appPnHandle);
}

void appMain(void *args)
{
    uint32_t instId;
    uint32_t                status = SystemP_FAILURE;
    PRUICSS_IntcInitData    pruss_intc_initdata = PRUSS_INTC_INITDATA;
    ICSS_EMAC_Params        icssEmacParams;

    Drivers_open();
    Board_driversOpen();

    DebugP_log("=================================\r\n");
    DebugP_log("  ICSS LWIP TCP ECHO SERVER 0-0 \r\n");
    DebugP_log("=================================\r\n");


    prusshandle = PRUICSS_open(CONFIG_PRU_ICSS1);
    DebugP_assert(prusshandle != NULL);

#ifdef MDIO_MANUAL_MODE_ENABLED
    mdioManualModeSetup();
#endif

    /*Getting Lwip2EmacHandle for interface layer*/
    Lwip2Emac_getHandle(&Lwipif_handle);
    hsrprp_mii_init();
    DebugP_log("Mode: MII\n");

    ICSS_EMAC_Params_init(&icssEmacParams);
    icssEmacParams.pruicssIntcInitData = &pruss_intc_initdata;
    icssEmacParams.fwStaticMMap = &(icss_emacFwStaticCfg[1]);
    icssEmacParams.fwDynamicMMap = &icss_emacFwDynamicCfg;
    icssEmacParams.pruicssHandle = prusshandle;
    icssEmacParams.ethphyHandle[0] = gEthPhyHandle[CONFIG_ETHPHY0];
    icssEmacParams.ethphyHandle[1] = gEthPhyHandle[CONFIG_ETHPHY1];
    /*Packet processing callback*/
    icssEmacParams.callBackObject.rxRTCallBack.callBack = (ICSS_EMAC_CallBack)Lwip2Emac_serviceRx;
    icssEmacParams.callBackObject.rxRTCallBack.userArg = (void*)(Lwipif_handle);
    icssEmacParams.callBackObject.rxNRTCallBack.callBack = (ICSS_EMAC_CallBack)Lwip2Emac_serviceRx;
    icssEmacParams.callBackObject.rxNRTCallBack.userArg = (void*)(Lwipif_handle);
    icssEmacParams.callBackObject.customRxCallBack.callBack = NULL;
    icssEmacParams.callBackObject.customRxCallBack.userArg = NULL;

    /*! Fetch the SoC provided MAC address and hand it to EMAC driver to allow this traffic to reach host */
    hsrprp_socgetMACAddress(lclMac);
    lclMac[0] = 0xF4;
    lclMac[1] = 0x84;
    lclMac[2] = 0x4C;
    lclMac[3] = 0xF9;
    lclMac[4] = 0x4D;
    lclMac[5] = 0x29;
    memcpy(&(icssEmacParams.macId[0]), &(lclMac[0]), 6);
    memmove(icssEmacParams.macId, ai8uIntfMacAddress_s, PRU_PN_MAC_ADDR_LEN);

    emachandle = ICSS_EMAC_open(CONFIG_ICSS_EMAC0, &icssEmacParams);
    DebugP_assert(emachandle != NULL);

    DebugP_log("Main Core init\r\n");
   ipc_rpmsg_echo_main(NULL);

    /* Enable Promiscuous mode */
    //EnetMp_togglePromisc(gEnetAppParams[0].enetType, gEnetAppParams[0].instId);

    // Icssg_MacAddr mac; //0xF4, 0x84, 0x4C, 0xF9, 0x4D, 0x29
    // mac.macAddr[0] = 0xF4;
    // mac.macAddr[1] = 0x84;
    // mac.macAddr[2] = 0x4C;
    // mac.macAddr[3] = 0xF9;
    // mac.macAddr[4] = 0x4D;
    // mac.macAddr[5] = 0x29;
    // status = App_addMacFdbEntry(gEnetAppParams[0].enetType, gEnetAppParams[0].instId, mac);
    // DebugP_assert(status == ENET_SOK);
    // EnetAppUtils_print("Bridge mac added to FDB \r\n");

    // mac.macAddr[0] = 00;
    // mac.macAddr[1] = 01;
    // mac.macAddr[2] = 02;
    // mac.macAddr[3] = 04;
    // mac.macAddr[4] = 05;
    // mac.macAddr[5] = 06;
    // status = App_addMacFdbEntry(gEnetAppParams[0].enetType, gEnetAppParams[0].instId, mac);
    // DebugP_assert(status == ENET_SOK);
    // EnetAppUtils_print("Remote mac added to FDB \r\n");

    EthApp_lwipMain(NULL, NULL); // - App_setupNetworkStack(): balluff

// Added from Balluff
    appPnHandle = (PN_Handle)malloc(sizeof(PN_Config));
    if(NULL == appPnHandle)
    {
        DebugP_log("Task Creation failed\r\n");
    }
    /* Assigning default values */
    /* Assign emac Handle */
    PN_initDefaultValues(appPnHandle,emachandle,prusshandle);
    int32_t PN_drvStatus = PN_initDrv(appPnHandle);
    if(PN_drvStatus == 0)
    {
        DebugP_log("\r\nProfinet Driver initialization successful!!\r\n");
    }
    else
    {
        DebugP_log("\r\nProfinet Driver initialization unsuccessful!!\r\n");
    }
    //E0-73-E7-C3-81-B9 - ICSS
    //1C-64-B4-51-C7-58 - CPSW (Remote core)

    uint8_t specialMac [6] = {0x00, 0x01, 0x02, 0x04, 0x05, 0x06};
    ICSS_EMAC_IoctlCmd ioctlParamsPNTest;
    ioctlParamsPNTest.ioctlVal = (void *)specialMac;
    ioctlParamsPNTest.command = ICSS_EMAC_IOCTL_SPECIAL_UNICAST_MAC_CTRL_ENABLE_CMD;
    int32_t ICSS_EMAC_ioctl_status = ICSS_EMAC_ioctl(emachandle,ICSS_EMAC_IOCTL_SPECIAL_UNICAST_MAC_CTRL, (uint32_t)NULL, (void *)&ioctlParamsPNTest);
    if(ICSS_EMAC_ioctl_status == 0)
    {
        DebugP_log("\r\nSpecial MAC address configuration successful!!\r\n");
        DebugP_log("MAC address configured : ");
        DebugP_log("%x:%x:%x:%x:%x:%x\r\n\r\n", (char)specialMac[0], (char)specialMac[1],
            (char)specialMac[2], (char)specialMac[3], (char)specialMac[4], (char)specialMac[5]);
    }
    else
    {
        DebugP_log("\r\nSpecial MAC address configuration unsuccessful!!\r\n");
    }

    App_waitForBridgeUp(); // - App_isNetworkUp(): balluff
    DebugP_log("Network is UP ...\r\n");

    ClockP_sleep(2);
    AppTcp_startServer();

    sys_lock_tcpip_core();
    lwiperf_example_init();
    sys_thread_new("UDP Iperf", start_application, NULL, DEFAULT_THREAD_STACKSIZE,
                               UDP_IPERF_THREAD_PRIO);
    sys_unlock_tcpip_core();

    while (1)
    {
        ClockP_usleep(1000);
        App_printCpuLoad();
    }

    // App_shutdownNetworkStack();
}

static void EnetMp_remoteCoreTask(void *args)
{

    Drivers_open();
    Board_driversOpen();
    DebugP_log("Remote Core init");
    ipc_rpmsg_echo_main(NULL);

}

static void App_printCpuLoad()
{
    static uint32_t startTime_ms = 0;
    const  uint32_t currTime_ms  = ClockP_getTimeUsec()/1000;
    const  uint32_t printInterval_ms = 5000;

    if (startTime_ms == 0)
    {
        startTime_ms = currTime_ms;
    }
    else if ( (currTime_ms - startTime_ms) > printInterval_ms )
    {
        const uint32_t cpuLoad = TaskP_loadGetTotalCpuLoad();

        DebugP_log(" %6d.%3ds : CPU load = %3d.%02d %%\r\n",
                    currTime_ms/1000, currTime_ms%1000,
                    cpuLoad/100, cpuLoad%100 );

        startTime_ms = currTime_ms;
        TaskP_loadResetAll();
    }
    return;
}

// FIX ME - Check if dead code

void PN_socinitIRTHandle (PN_Handle appPnHandle)
{
    PRUICSS_HwAttrs const *pruicssHwAttrs = PRUICSS_getAttrs(CONFIG_PRU_ICSS1);
    appPnHandle->pnIntConfig.ppmIntConfig.pruIntNum = 21;
    appPnHandle->pnIntConfig.ppmIntConfig.socEvId = 21;
    appPnHandle->pnIntConfig.ppmIntConfig.intPrio = 11;
    appPnHandle->pnIntConfig.cpmIntConfig.pruIntNum = 22;
    appPnHandle->pnIntConfig.cpmIntConfig.socEvId = 22;
    appPnHandle->pnIntConfig.cpmIntConfig.intPrio = 12;
    appPnHandle->pnIntConfig.dhtIntConfig.pruIntNum = 23;
    appPnHandle->pnIntConfig.dhtIntConfig.socEvId = 23;
    appPnHandle->pnIntConfig.dhtIntConfig.intPrio = 10;

#ifdef PTCP_SUPPORT
    appPnHandle->pnIntConfig.ptcpIntConfig.pruIntNum = 24;
    appPnHandle->pnIntConfig.ptcpIntConfig.socEvId = 24;
    appPnHandle->pnIntConfig.ptcpIntConfig.intPrio = 13;
    appPnHandle->pnPtcpConfig.ptcpEnableSlowCompensation = 1;
    appPnHandle->pnPtcpConfig.ptcpTimer.ptcpTimerID = PN_PTCP_TIMERID;
#endif

    appPnHandle->pnIsoMConfig.isoMIntConfig.pruIntNum = 27;
    appPnHandle->pnIsoMConfig.isoMIntConfig.socEvId = 27;
    appPnHandle->pnIsoMConfig.isoMIntConfig.intPrio = 14;

    if(pruicssHwAttrs->instance == 0)
    {
        /* Interrupt configuration */
        appPnHandle->pnIntConfig.ppmIntConfig.coreIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + PN_PPM_INT_OFFSET;
        appPnHandle->pnIntConfig.cpmIntConfig.coreIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + PN_CPM_INT_OFFSET;
        appPnHandle->pnIntConfig.dhtIntConfig.coreIntNum =  CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + PN_DHT_INT_OFFSET;        
#ifdef PTCP_SUPPORT
        appPnHandle->pnIntConfig.ptcpIntConfig.coreIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + PN_PTCP_INT_OFFSET;
#endif 
        appPnHandle->pnIsoMConfig.isoMIntConfig.coreIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + PN_ISOM_INT_OFFSET;
    }
    else
    {
        appPnHandle->pnIntConfig.ppmIntConfig.coreIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + PN_PPM_INT_OFFSET;
        appPnHandle->pnIntConfig.cpmIntConfig.coreIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + PN_CPM_INT_OFFSET;
        appPnHandle->pnIntConfig.dhtIntConfig.coreIntNum =  CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + PN_DHT_INT_OFFSET;
#ifdef PTCP_SUPPORT
        appPnHandle->pnIntConfig.ptcpIntConfig.coreIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + PN_PTCP_INT_OFFSET;
#endif
        appPnHandle->pnIsoMConfig.isoMIntConfig.coreIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + PN_ISOM_INT_OFFSET;
    }
}

void PN_socgetMACAddress(uint8_t *lclMac)
{
    EEPROM_read(gEepromHandle[CONFIG_EEPROM0],  I2C_EEPROM_MAC_DATA_OFFSET, lclMac, 6U);
}
