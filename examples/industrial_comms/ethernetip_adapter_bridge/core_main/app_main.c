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
 * \brief This file contains the implementation of the EthernetIP adapter bridge example for main core.
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
#include <networking/icss_emac/icss_emac.h>
#include <networking/icss_emac/source/icss_emac_local.h> 
#include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/icss_emac_mmap.h>
#include <industrial_comms/ethernetip_adapter/icss_fwhal/tiswitch_pruss_intc_mapping.h>
#include <industrial_comms/ethernetip_adapter/icss_fwhal/icss_eip_driver.h>
#include <lwip2icss_emac.h>

#include "hsr_prp_soc.h"
#include "app_tcpserver.h"
#include "netif_common.h"
#include "app_control.h"
#include "app_netif.h"
#include "udp_iperf.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* Interrupt related defines EthernetIP */
#define EIP_PTP_INT_NUM                     (3)
#define EIP_DLR_P0_INT_OFFSET               (1)
#define EIP_DLR_P1_INT_OFFSET               (2)
#define EIP_BEACON_TIMEOUT_INT_OFFSET_P0    (4)
#define EIP_BEACON_TIMEOUT_INT_OFFSET_P1    (7)

/* EEPROM data offset in I2C EEPROM Flash */
#define I2C_EEPROM_DATA_OFFSET          (0x8000)
#define I2C_EEPROM_MAC_DATA_OFFSET      (0x42)

/* UDP Iperf task should be highest priority task to ensure processed buffers
 * are freed without delay so that we get maximum throughput for
 * UDP Iperf.
 */
#define UDP_IPERF_THREAD_PRIO  (14U)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

EIP_Handle icssEipHandle;
EIP_DLRHandle dlrHandle;
TimeSync_ParamsHandle_t timeSyncHandle;

/** \brief PRU-ICSS LLD handle */
PRUICSS_Handle prusshandle;
/** \brief ICSSEmac LLD handle */
ICSS_EMAC_Handle emachandle;
/** \brief Time Sync handle */
TimeSync_ParamsHandle_t timeSyncHandle;
/** \brief LwIP Interface layer handle */
Lwip2Emac_Handle Lwipif_handle;
/* EMAC mac-addr */
uint8_t lclMac[6];

/** Temporary placeholder to copy packets */
uint8_t tempFrame[ICSS_EMAC_MAXMTU] __attribute__((aligned(32)));

/* EthernetIP related global variables */
static uint32_t rx_rt_errors_s  = 0; 
/**PTP MAC ID for comparison*/
uint8_t ptpMAC_EIP[6] = {0x1, 0x0, 0x5e, 0x0, 0x1, 0x81}; 
/**DLR MAC ID for comparison*/
uint8_t dlrMAC_EIP[6] = {0x1, 0x21, 0x6c, 0x0, 0x0, 0x2}; 

/**PTP MAC ID for comparison*/
uint8_t timeSyncMAC[6] = {0x1, 0x1b, 0x19, 0x0, 0x0, 0x0};
uint8_t linkLocalMAC[6] = {0x1, 0x80, 0xc2, 0x0, 0x0, 0xE};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

extern void Lwip2Emac_getHandle(Lwip2Emac_Handle *AppLwipHandle);

void appMain(void *args);

static void App_printCpuLoad();

int32_t AppCtrl_addMacAddr2fbd(Icss_MacAddr assignMac);

int32_t app_getEmacHandle(Lwip2Emac_Handle hLwip2Emac);

uint32_t EnetSoc_getCoreId(void);

/* EthernetIP related functions */ 

// void EIP_DLR_TIMESYNC_assignIP();

void EIP_socgetMACAddress(uint8_t *lclMac);

void EIP_DLR_TIMESYNC_port0ProcessLinkBrk(void *icssEmacVoidPtr, uint8_t linkStatus, void *eipHandleVoidPtr);

void EIP_DLR_TIMESYNC_port1ProcessLinkBrk(void *icssEmacVoidPtr, uint8_t linkStatus, void *eipHandleVoidPtr);

void EIP_configureInterrupts(EIP_DLRHandle dlrHandle, TimeSync_ParamsHandle_t timeSyncHandle);

void EIP_syncLossCallback();

int8_t EIP_initICSSPtpHandle(TimeSync_ParamsHandle_t timeSyncHandle, ICSS_EMAC_Handle emachandle);

void EIP_initICSSDlrHandle(EIP_DLRHandle dlrHandle, ICSS_EMAC_Handle emachandle);

int8_t EIP_initDefaultValues(EIP_Handle icssEipHandle,ICSS_EMAC_Handle emachandle,PRUICSS_Handle prusshandle);

int8_t EIP_RtRxCallback (void *emacHandleVoidPtr, uint8_t portNumber, void * eipHandleVoidPtr);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void appMain(void *args)
{
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
    /*! Uncomment this line to debug issues using CCS */
    // int32_t status = 1;        
    // while(status)
    // {
    //     DebugP_log("%d", status);
    // }

    /*Getting Lwip2EmacHandle for interface layer*/
    Lwip2Emac_getHandle(&Lwipif_handle);
    hsrprp_mii_init();
    DebugP_log("Mode: MII\n");

    ICSS_EMAC_Params_init(&icssEmacParams);
    icssEmacParams.pruicssIntcInitData = &pruss_intc_initdata;
    icssEmacParams.fwStaticMMap = &(icss_emacFwStaticCfgLocal[1]); 
    icssEmacParams.fwDynamicMMap = &icss_emacFwDynamicCfgLocal; 
    icssEmacParams.pruicssHandle = prusshandle;
    icssEmacParams.ethphyHandle[0] = gEthPhyHandle[CONFIG_ETHPHY0];
    icssEmacParams.ethphyHandle[1] = gEthPhyHandle[CONFIG_ETHPHY1];
    /*Packet processing callback*/
    icssEmacParams.callBackObject.rxRTCallBack.callBack = NULL; 
    icssEmacParams.callBackObject.rxRTCallBack.userArg = NULL; 
    icssEmacParams.callBackObject.rxNRTCallBack.callBack = (ICSS_EMAC_CallBack)Lwip2Emac_serviceRx;
    icssEmacParams.callBackObject.rxNRTCallBack.userArg = (void*)(Lwipif_handle);
    icssEmacParams.callBackObject.customRxCallBack.callBack = NULL;
    icssEmacParams.callBackObject.customRxCallBack.userArg = NULL;

    /*! Fetch the SoC provided MAC address and hand it to EMAC driver to allow this traffic to reach host */
    EIP_socgetMACAddress(lclMac);
    memcpy(&(icssEmacParams.macId[0]), &(lclMac[0]), 6);
    emachandle = ICSS_EMAC_open(CONFIG_ICSS_EMAC0, &icssEmacParams);
    DebugP_assert(emachandle != NULL);

    DebugP_log("Main Core init\r\n");
    /*! Initialize the IPC Task */
    AppCtrl_createRecvTask();
    /*! Handle the IC up notify to bring IC-netif up */
    EthApp_lwipMain(NULL, NULL);

    icssEipHandle = (EIP_Handle)malloc(sizeof(struct eip_Config_s));
    if(NULL == icssEipHandle)
    {
        DebugP_log("Creation of EIP Handle failed\r\n");
    }

    /* Assigning default values */
    int8_t status = EIP_initDefaultValues(icssEipHandle, emachandle, prusshandle);
    DebugP_assert(status==SystemP_SUCCESS);

    /* Copy the MAC ID passed to ICSS-EMAC during ICSS-EMAC initialization*/
    memcpy(dlrHandle->macId, &(icssEmacParams.macId[0]), 6);
    memcpy(timeSyncHandle->macId, &(icssEmacParams.macId[0]), 6);
   
    /*Configure the DLR and PTP Interrupts */
    EIP_configureInterrupts(dlrHandle, timeSyncHandle);
    icssEipHandle->dlrHandle = dlrHandle;
    icssEipHandle->timeSyncHandle = timeSyncHandle;

    /* Init EIP Driver */
    EIP_drvInit(icssEipHandle);
    EIP_drvStart(icssEipHandle);
    DebugP_log("Ethernet/IP Firmware Load Done\r\n");

    /* Register Callback APIs for Link Break*/
    ((((ICSS_EMAC_Object *)emachandle->object)->callBackObject).port0LinkCallBack).callBack = (ICSS_EMAC_CallBack)EIP_DLR_TIMESYNC_port0ProcessLinkBrk;
    ((((ICSS_EMAC_Object *)emachandle->object)->callBackObject).port0LinkCallBack).userArg  = (void*)icssEipHandle;
    ((((ICSS_EMAC_Object *)emachandle->object)->callBackObject).port1LinkCallBack).callBack = (ICSS_EMAC_CallBack)EIP_DLR_TIMESYNC_port1ProcessLinkBrk;
    ((((ICSS_EMAC_Object *)emachandle->object)->callBackObject).port1LinkCallBack).userArg  = (void*)icssEipHandle;

    if(((((ICSS_EMAC_Object *)emachandle->object)->callBackObject).port0LinkCallBack).callBack == NULL ||
        ((((ICSS_EMAC_Object *)emachandle->object)->callBackObject).port1LinkCallBack).callBack == NULL)
    {
        DebugP_log("Link Break Callback API registration failed\r\n");
    }
    else 
    {
        DebugP_log("Link Break Callback API register successful\r\n");
    }

    ((((ICSS_EMAC_Object *)emachandle->object)->callBackObject).rxRTCallBack).callBack = (ICSS_EMAC_CallBack)EIP_RtRxCallback;
    ((((ICSS_EMAC_Object *)emachandle->object)->callBackObject).rxRTCallBack).userArg  = (void*)icssEipHandle;

    // For R5 R5 test - enable
    // uint8_t specialMac [6] = {0x00, 0x01, 0x02, 0x04, 0x05, 0x06};
    // ICSS_EMAC_IoctlCmd ioctlParamsPNTest;
    // ioctlParamsPNTest.ioctlVal = (void *)specialMac;
    // ioctlParamsPNTest.command = ICSS_EMAC_IOCTL_SPECIAL_UNICAST_MAC_CTRL_ENABLE_CMD;
    // int32_t ICSS_EMAC_ioctl_status = ICSS_EMAC_ioctl(emachandle,ICSS_EMAC_IOCTL_SPECIAL_UNICAST_MAC_CTRL, (uint32_t)NULL, (void *)&ioctlParamsPNTest);
    // if(ICSS_EMAC_ioctl_status == 0)
    // {
    //     DebugP_log("\r\nSpecial MAC address configuration successful!!\r\n");
    //     DebugP_log("MAC address configured : ");
    //     DebugP_log("%x:%x:%x:%x:%x:%x\r\n\r\n", (char)specialMac[0], (char)specialMac[1],
    //         (char)specialMac[2], (char)specialMac[3], (char)specialMac[4], (char)specialMac[5]);
    // }
    // else
    // {
    //     DebugP_log("\r\nSpecial MAC address configuration unsuccessful!!\r\n");
    // }

    App_waitForBridgeUp();
    DebugP_log("Network is UP ...\r\n");
//    EIP_DLR_TIMESYNC_assignIP(); //Add later 
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
}

// void EIP_DLR_TIMESYNC_assignIP() //EIP change
// {
//    EIP_DLR_addModuleIPAddress(icssEipHandle->dlrHandle, ipAddress);
//    TimeSync_addIP(icssEipHandle->timeSyncHandle, ipAddress);
// }

/*Registering Port 1 Link break callback */
void EIP_DLR_TIMESYNC_port0ProcessLinkBrk(void *icssEmacVoidPtr, uint8_t linkStatus, void *eipHandleVoidPtr) 
{
    EIP_Handle eipHandle = (EIP_Handle)eipHandleVoidPtr;
    EIP_DLR_port0ProcessLinkBrk(linkStatus, (void *)(eipHandle->dlrHandle));
    TimeSync_Port1linkResetCallBack(linkStatus, (void *)(eipHandle->timeSyncHandle));
}

/*Registering Port 2 Link break callback */
void EIP_DLR_TIMESYNC_port1ProcessLinkBrk(void *icssEmacVoidPtr, uint8_t linkStatus, void *eipHandleVoidPtr) 
{
    EIP_Handle eipHandle = (EIP_Handle)eipHandleVoidPtr;
    EIP_DLR_port1ProcessLinkBrk(linkStatus, (void *)(eipHandle->dlrHandle));
    TimeSync_Port2linkResetCallBack(linkStatus, (void *)(eipHandle->timeSyncHandle));
}

void EIP_configureInterrupts(EIP_DLRHandle dlrHandle, TimeSync_ParamsHandle_t timeSyncHandle) 
{
    PRUICSS_HwAttrs const *pruicssHwAttrs = PRUICSS_getAttrs(CONFIG_PRU_ICSS1);

    if(pruicssHwAttrs->instance == 0)
    {
        /*Configure Time Sync interrupts*/
        timeSyncHandle->timeSyncConfig.txIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + EIP_PTP_INT_NUM;

        /*Configure DLR*/
        dlrHandle->dlrObj->port0IntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + EIP_DLR_P0_INT_OFFSET;
        dlrHandle->dlrObj->port1IntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + EIP_DLR_P1_INT_OFFSET;
        dlrHandle->dlrObj->beaconTimeoutIntNum_P0 = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + EIP_BEACON_TIMEOUT_INT_OFFSET_P0;
        dlrHandle->dlrObj->beaconTimeoutIntNum_P1 = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG0_PR1_HOST_INTR_PEND_0 + EIP_BEACON_TIMEOUT_INT_OFFSET_P1;
    }
    else
    {
        /*Configure Time Sync interrupts*/
        timeSyncHandle->timeSyncConfig.txIntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + EIP_PTP_INT_NUM;

        /*Configure DLR*/
        dlrHandle->dlrObj->port0IntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + EIP_DLR_P0_INT_OFFSET;
        dlrHandle->dlrObj->port1IntNum = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + EIP_DLR_P1_INT_OFFSET;
        dlrHandle->dlrObj->beaconTimeoutIntNum_P0 = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + EIP_BEACON_TIMEOUT_INT_OFFSET_P0;
        dlrHandle->dlrObj->beaconTimeoutIntNum_P1 = CSLR_R5FSS0_CORE0_INTR_PRU_ICSSG1_PR1_HOST_INTR_PEND_0 + EIP_BEACON_TIMEOUT_INT_OFFSET_P1;
    }
}

void EIP_syncLossCallback() 
{
    /*This indicates a loss of time sync
      Call your function here which handles sync loss*/

    return;
}

int8_t EIP_initICSSPtpHandle(TimeSync_ParamsHandle_t timeSyncHandle, ICSS_EMAC_Handle emachandle) 
{
    int8_t returnVal = SystemP_FAILURE;
    timeSyncHandle->emacHandle = emachandle;

    /*Configure PTP. These variables must be configured before doing anything else*/
    timeSyncHandle->timeSyncConfig.config = BOTH;
    timeSyncHandle->timeSyncConfig.type = E2E;
    timeSyncHandle->timeSyncConfig.protocol = UDP_IPV4;
    timeSyncHandle->timeSyncConfig.tickPeriod = 500;
    timeSyncHandle->txprotocol = 0;

    timeSyncHandle->timeSyncConfig.delayReqSendTaskPriority = 10;
    timeSyncHandle->timeSyncConfig.txTsTaskPriority = 10;
    timeSyncHandle->timeSyncConfig.nrtTaskPriority = 8;
    timeSyncHandle->timeSyncConfig.backgroundTaskPriority = 7;

    timeSyncHandle->tsSyntInfo = (timeSync_SyntInfo_t *)malloc(sizeof(
                                     timeSync_SyntInfo_t));

    if(timeSyncHandle->tsSyntInfo == NULL)
    {
        DebugP_log("\n\rIssue in allocating memory for timeSyncHandle->tsSyntInfo");
    }

    timeSyncHandle->tsNrrInfo[0] = (timeSync_NrrInfo_t *)malloc(sizeof(
                                       timeSync_NrrInfo_t));

    if(timeSyncHandle->tsNrrInfo[0] == NULL)
    {
        DebugP_log("\n\rIssue in allocating memory for timeSyncHandle->tsNrrInfo[0]");
    }

    timeSyncHandle->tsNrrInfo[1] = (timeSync_NrrInfo_t *)malloc(sizeof(
                                       timeSync_NrrInfo_t));

    if(timeSyncHandle->tsNrrInfo[1] == NULL)
    {
        DebugP_log("\n\rIssue in allocating memory for timeSyncHandle->tsNrrInfo[1]");
    }

    timeSyncHandle->syncParam[0] = (syncParam_t *)malloc(sizeof(syncParam_t));

    if(timeSyncHandle->syncParam[0] == NULL)
    {
        DebugP_log("\n\rIssue in allocating memory for timeSyncHandle->syncParam[0]");
    }

    timeSyncHandle->syncParam[1] = (syncParam_t *)malloc(sizeof(syncParam_t));

    if(timeSyncHandle->syncParam[1] == NULL)
    {
        DebugP_log("\n\rIssue in allocating memory for timeSyncHandle->syncParam[1]");
    }

    timeSyncHandle->tsRunTimeVar = (timeSync_RuntimeVar_t *)malloc(sizeof(
                                       timeSync_RuntimeVar_t));

    if(timeSyncHandle->tsRunTimeVar == NULL)
    {
        DebugP_log("\n\rIssue in allocating memory for timeSyncHandle->tsRunTimeVar");
    }

    timeSyncHandle->delayParams = (delayReqRespParams_t *)malloc(sizeof(
                                      delayReqRespParams_t));

    if(timeSyncHandle->delayParams == NULL)
    {
        DebugP_log("\n\rIssue in allocating memory for timeSyncHandle->delayParams");
    }

    timeSyncHandle->offsetAlgo  = (timeSync_Offset_Stable_Algo_t *)malloc(sizeof(
                                      timeSync_Offset_Stable_Algo_t));

    if(timeSyncHandle->offsetAlgo == NULL)
    {
        DebugP_log("\n\rIssue in allocating memory for timeSyncHandle->offsetAlgo");
    }

    /*Allocate Rx and Tx packet buffers*/
    returnVal = TimeSync_alloc_PktBuffer(timeSyncHandle);

    /*Set only if a custom Tx LLD API is used*/
    timeSyncHandle->timeSyncConfig.custom_tx_api = 0;
    /*Set to 1 if Rx timestamps are coming from shared RAM*/
    timeSyncHandle->timeSyncConfig.timestamp_from_shared_ram = 1;

    /*If there is no forwarding between ports then set this*/
    timeSyncHandle->timeSyncConfig.emac_mode = 0;
    timeSyncHandle->timeSyncConfig.hsrEnabled = 0;

    timeSyncHandle->rxTimestamp_gPTP = (timeStamp *)malloc(sizeof(timeStamp));

    timeSyncHandle->timeSyncConfig.domainNumber[0] = 0;

    timeSyncHandle->timeSyncConfig.logAnnounceRcptTimeoutInterval = DEFAULT_ANNOUNCE_TIMEOUT_LOG_INTERVAL;
    timeSyncHandle->timeSyncConfig.logAnnounceSendInterval = DEFAULT_ANNOUNCE_SEND_LOG_INTERVAL;
    timeSyncHandle->timeSyncConfig.logPDelReqPktInterval = DEFAULT_PDELAY_REQ_LOG_INTERVAL;
    timeSyncHandle->timeSyncConfig.logSyncInterval = DEFAULT_SYNC_SEND_LOG_INTERVAL;

    /*No asymmetry*/
    timeSyncHandle->timeSyncConfig.asymmetryCorrection[0] = 0;
    timeSyncHandle->timeSyncConfig.asymmetryCorrection[1] = 0;
    timeSyncHandle->timeSyncConfig.pdelayBurstNumPkts = 3;      /*3 frames sent in a burst*/
    timeSyncHandle->timeSyncConfig.pdelayBurstInterval = 200;   /*gap between each frame is 100ms*/
    timeSyncHandle->timeSyncConfig.sync0_interval = 1000000;      /*1 milisec value*/
  
    /*Register callback*/
    timeSyncHandle->timeSyncConfig.timeSyncSyncLossCallBackfn = (TimeSync_SyncLossCallBack_t)EIP_syncLossCallback;

    timeSyncHandle->timeSyncConfig.masterParams.priority1 = TIMESYNC_DEFAULT_PRIO_1;
    timeSyncHandle->timeSyncConfig.masterParams.priority2 = TIMESYNC_DEFAULT_PRIO_2;
    timeSyncHandle->timeSyncConfig.masterParams.clockAccuracy = TIMESYNC_DEFAULT_CLOCK_ACCURACY; /*greater than 10s */
    timeSyncHandle->timeSyncConfig.masterParams.clockClass = TIMESYNC_DEFAULT_CLOCK_CLASS;
    timeSyncHandle->timeSyncConfig.masterParams.clockVariance = TIMESYNC_DEFAULT_CLOCK_VARIANCE;
    timeSyncHandle->timeSyncConfig.masterParams.stepRemoved = TIMESYNC_DEFAULT_STEPS_REMOVED;
    timeSyncHandle->timeSyncConfig.masterParams.UTCOffset = TIMESYNC_UTC_OFFSET;
    timeSyncHandle->timeSyncConfig.masterParams.timeSource = TIMESYNC_DEFAULT_TIME_SOURCE; /*Internal oscillator*/

    timeSyncHandle->timeSyncConfig.masterParams.ptp_flags[TS_PTP_TIMESCALE_INDEX] = 1;
    timeSyncHandle->timeSyncConfig.masterParams.ptp_flags[TS_PTP_TWO_STEP_INDEX] = 1;

    timeSyncHandle->timeSyncConfig.rxPhyLatency = 220;
    timeSyncHandle->timeSyncConfig.txPhyLatency = 64;

    returnVal = SystemP_SUCCESS;
    return returnVal;
}

void EIP_initICSSDlrHandle(EIP_DLRHandle dlrHandle, ICSS_EMAC_Handle emachandle) 
{
    dlrHandle->emacHandle = emachandle;
    dlrHandle->dlrObj = (dlrStruct *)malloc(sizeof(dlrStruct));
    dlrHandle->exclusionList = (exceptionList *)malloc(sizeof(exceptionList));
}

int8_t EIP_initDefaultValues(EIP_Handle icssEipHandle,ICSS_EMAC_Handle emachandle,PRUICSS_Handle prusshandle) 
{
    int8_t status = SystemP_SUCCESS;
    /* Memory allocations */
    memset(icssEipHandle,0,sizeof(struct eip_Config_s));
    icssEipHandle->emacHandle = emachandle;
    icssEipHandle->pruicssHandle  = prusshandle;
    dlrHandle = (EIP_DLRHandle)malloc(sizeof(dlr_Config));
    timeSyncHandle = (TimeSync_ParamsHandle_t)malloc(sizeof(TimeSync_ParamsHandle));
    EIP_initICSSDlrHandle(dlrHandle, emachandle);
    status = EIP_initICSSPtpHandle(timeSyncHandle, emachandle);
    return status;
}

int8_t EIP_RtRxCallback (void *emacHandleVoidPtr, uint8_t portNumber, void * eipHandleVoidPtr) 
{
    uint32_t packetLen;
    int32_t  port;
    int8_t   returnVal = SystemP_SUCCESS;
    int32_t  queue;
    int32_t  len;
    ICSS_EMAC_RxArgument rxArgs;

    EIP_Handle            eipHandle = (EIP_Handle)eipHandleVoidPtr;
    ICSS_EMAC_Handle      emacHandle = (ICSS_EMAC_Handle)emacHandleVoidPtr;
    ICSS_EMAC_PortParams  rxPort   = ((ICSS_EMAC_Object*)(emacHandle->object))->switchPort[0];
    uint32_t              rxErrors = rxPort.queue[0].qStat.errCount +
                                     rxPort.queue[1].qStat.errCount +
                                     rxPort.queue[2].qStat.errCount;
    uint8_t               *dstMacId = eipHandle->tempFrame;
    packetLen = ICSS_EMAC_rxPktInfo (emacHandle, &port, &queue);

    if (packetLen == SystemP_FAILURE)
    {
        returnVal = SystemP_FAILURE;
        return returnVal;
    }

    if (rxErrors != rx_rt_errors_s)
    {
        rx_rt_errors_s = rxErrors;

        DebugP_log("RX real-time queue errors: %d", rxErrors);
    }

    switch (queue)
    {
        case ICSS_EMAC_QUEUE1:
        case ICSS_EMAC_QUEUE2:
        case ICSS_EMAC_QUEUE3:
        case ICSS_EMAC_QUEUE4:
        {
            rxArgs.icssEmacHandle = emacHandleVoidPtr;
            rxArgs.destAddress    = (uint32_t)(icssEipHandle->tempFrame);
            rxArgs.queueNumber    = queue;
            rxArgs.more           = 0;
            rxArgs.port           = 0;

            len = ICSS_EMAC_rxPktGet (&rxArgs, NULL);

            /*Compare Destination MAC ID and determine if this is a DLR packet*/
            if((memcmp((void *)dstMacId, (void *)dlrMAC_EIP, 6U) == 0))
            {
                EIP_DLR_processDLRFrame(eipHandle->dlrHandle, eipHandle->tempFrame,
                                        rxArgs.port - 1, len);
            }
            /*Compare Destination MAC ID and determine if this is a PTP packet*/
            else if((memcmp((void *)dstMacId, (void *)ptpMAC_EIP, 6U) == 0) &&
                    (rxArgs.port >= ICSS_EMAC_PORT_1) &&
                    (rxArgs.port <= ICSS_EMAC_PORT_2))
            {
                /*Link local field doesn't matter in case of EIP*/
                TimeSync_processPTPFrame(eipHandle->timeSyncHandle,
                                        eipHandle->tempFrame, rxArgs.port, len, FALSE);
            }

        }   break;
        default:
        {
            returnVal = SystemP_FAILURE;
        }   break;
    }

    return returnVal;
}

uint32_t EnetSoc_getCoreId(void)
{
    uint32_t coreId = CSL_CORE_ID_R5FSS0_0;
    return coreId;
}

uint32_t App_getSelfCoreId()
{
    uint32_t coreId = EnetSoc_getCoreId();
    return coreId;
}

int32_t app_getEmacHandle(Lwip2Emac_Handle hLwip2Emac)
{
    int32_t ret_val = SystemP_FAILURE;
    if(hLwip2Emac != NULL)
    {
        hLwip2Emac->emacHandle = emachandle;
        ret_val = SystemP_SUCCESS;
    }

    return (ret_val);
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

void EIP_socgetMACAddress(uint8_t *lclMac)
{
    // Verify change
    lclMac[0] = 0xF4;
    lclMac[1] = 0x84;
    lclMac[2] = 0x4C;
    lclMac[3] = 0xF9;
    lclMac[4] = 0x4D;
    lclMac[5] = 0x29;
    // EEPROM_read(gEepromHandle[CONFIG_EEPROM0],  I2C_EEPROM_MAC_DATA_OFFSET, lclMac, 6U);
}

int32_t AppCtrl_addMacAddr2fbd(Icss_MacAddr assignMac)
{
    int32_t status = ICVE_OK;

    if(status == ICVE_OK)
    {
        ICSS_EMAC_IoctlCmd ioctlParamsPNTest;
        ioctlParamsPNTest.ioctlVal = (void *)(assignMac.macAddr);
        ioctlParamsPNTest.command = ICSS_EMAC_IOCTL_SPECIAL_UNICAST_MAC_CTRL_ENABLE_CMD;
        int32_t ICSS_EMAC_ioctl_status = ICSS_EMAC_ioctl(emachandle,ICSS_EMAC_IOCTL_SPECIAL_UNICAST_MAC_CTRL, (uint32_t)NULL, (void *)&ioctlParamsPNTest);
        if(ICSS_EMAC_ioctl_status == 0)
        {
            status = ICVE_OK;
        }
        else
        {
            status = ICVE_FAIL;
        }
        DebugP_log("Adding new to FDB  \r\n");
    }

    if(status == ICVE_OK)
    {
        DebugP_log("Remote mac added to FDB \r\n");
    }

    return status;
}

int32_t AppCtrl_addMcastAddr(Icss_MacAddr mac)
{
    int32_t status = ICVE_OK;

    if(status == ICVE_OK)
    {
        status = AddNetif_addBridgeMcastEntry(mac);
        DebugP_log("Adding new MCast entry to LwIP Bridge \r\n");
    }

    if(status == ICVE_OK)
    {
        DebugP_log("MC addr added Successfully \r\n");
    }

    return status;
}

int32_t AppCtrl_delMcastAddr(Icss_MacAddr mac)
{
    int32_t status = ICVE_OK;

    if(status == ICVE_OK)
    {
        status = AddNetif_delBridgeMcastEntry(mac);
        DebugP_log("Deleting new MCast entry from LwIP Bridge \r\n");
    }

    if(status == ICVE_OK)
    {
        DebugP_log("MCast address deleted from FDB \r\n");
    }

    return status;
}