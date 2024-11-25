/*!
 *  \file appTask_tunneling.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application profile common functions.
 *
 *  Copyright (c) Texas Instruments Incorporated 2024
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "EI_API.h"
#include "EI_API_def.h"

#include "appUart.h"
#include "appLed.h"
#include "appNV.h"
#include "appCfg.h"
#include "appMutex.h"

#include <osal.h>
#include <osal_error.h>
#include <hwal.h>
#include <board.h>

#include <CMN_board.h>
#include <CMN_os.h>
#include <CMN_app.h>
#include <CMN_mem.h>
#include <CMN_CPU_api.h>

#include <drivers/CUST_drivers.h>

#include <appWebServer.h>

#include "appCfg.h"
#include "app_tunneling.h"
#include "appTask_tunneling.h"
#include "appRst.h"
#include <../device_profiles/app_device_profile.h>

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#ifdef ENABLE_INTERCORE_TUNNELING
#include "udp_iperf.h"
#include "app_tcpserver.h"
#include "netif_common.h"
#include "app_control.h"
#include "app_netif.h"
#include <examples/lwiperf/lwiperf_example.h>

/* UDP Iperf task should be highest priority task to ensure processed buffers
 * are freed without delay so that we get maximum throughput for UDP Iperf.
 */
#define UDP_IPERF_THREAD_PRIO  (14U)
#endif

extern PRUICSS_Handle prusshandle;

// Static variables and pointers used in this example.

static EI_API_ADP_T*           adapter_s  = NULL;
static EI_API_CIP_NODE_T*      cipNode_s  = NULL;

static int                     EI_APP_TASK_cleanup             (EI_API_ADP_T      *pAdapter, EI_API_CIP_NODE_T *pCipNode);
static void                    EI_APP_TASK_stackInit           (APP_SParams_t     *pParam);
static bool                    EI_APP_TASK_init                (APP_SParams_t     *pParam);
static void                    EI_APP_TASK_adpInit             (EI_API_ADP_T      *pAadapter);
static void                    EI_APP_TASK_run                 (EI_API_CIP_NODE_T *pCipNode);
static uint8_t*                EI_APP_TASK_getMacAddr          (void);

static void                    EI_APP_TASK_stackErrorHandlerCb (uint32_t errorCode,   uint8_t fatal, uint8_t numOfPara, va_list argPtr);
static bool                    EI_APP_TASK_cipCreateCallback   (EI_API_CIP_NODE_T *pCipNode);

static void                    App_printCpuLoad                ();

// Global variables and pointers used in this example.
// has to stay, used in lib_eip_lwip_ip :-(
static uint8_t EI_APP_TASK_macAddress[] = {0xc8, 0x3e, 0xa7, 0x00, 0x00, 0x59};

uint32_t globalError = 0;

uint32_t configMethod;

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  This function deinitializes / cleans the objects created
 *  before an exit.
 *
 *  \details
 *  This function deinitializes CIP node and ADP objects. It also
 *  deinitializes the IDK board GPIOs before an exit.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \return     int               Exit code.
 *
 *  \retval     0                 Clean up success.
 *  \retval     1                 Clean up failed.
 *
 */
static int EI_APP_TASK_cleanup (EI_API_ADP_T* adapter, EI_API_CIP_NODE_T* cipNode)
{
    int exit_code = 0;

    // Try to delete the CIP node.
    if (EI_API_CIP_NODE_delete(cipNode))
    {
        // Fail.
        exit_code = 1;
    }

    // Try to delete the ADP.
    if (EI_API_ADP_delete(adapter))
    {
        // Fail.
        exit_code = 1;
    }

    return exit_code;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Initialize EtherNet/IP&trade; Adapter data.
 *
 *  \details
 *  Enable the device implementer to override default data, such as for example
 *  Vendor ID, Device Type, Product Code, Revision, Product Name, etc.
 *  and read permanent saved data.
 */
static void EI_APP_TASK_adpInit(EI_API_ADP_T* pAdapter)
{
    EI_APP_CFG_apply(pAdapter);
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Creates several callbacks.
 *
 *  \details
 *  Creates and registers callbacks which are needed for program flow.
 *  - Store non-volatile data callback.
 *  - Set_Attribute_Single service callback.
 *  - Reset service callback.
 *
 */
static bool EI_APP_TASK_cipCreateCallback(EI_API_CIP_NODE_T* cipNode)
{
    bool    res = false;

    // Your callback function which stores your data when triggered.
    EI_API_CIP_CBService ptr_my_config_cb = EI_APP_CFG_callback;

    // Register callbacks for Set_Attribute_Single service.
    EI_API_CIP_SService_t srvc = { EI_API_CIP_eSC_SETATTRSINGLE, 0, NULL, ptr_my_config_cb };

    if (EI_API_CIP_eERR_OK != EI_API_CIP_setInstanceServiceFunc(cipNode, 0x00F5, 0x0001, &srvc))
    {
        goto laError;
    }

    if (EI_API_CIP_eERR_OK != EI_API_CIP_setInstanceServiceFunc(cipNode, 0x00F6, 0x0001, &srvc))
    {
        goto laError;
    }

    if (EI_API_CIP_eERR_OK != EI_API_CIP_setInstanceServiceFunc(cipNode, 0x00F6, 0x0002, &srvc))
    {
        goto laError;
    }

    if (EI_API_CIP_eERR_OK != EI_API_CIP_setInstanceServiceFunc(cipNode, 0x0043, 0x0001, &srvc))
    {
        goto laError;
    }

    if (EI_API_CIP_eERR_OK != EI_API_CIP_setInstanceServiceFunc(cipNode, 0x0048, 0x0001, &srvc))
    {
        goto laError;
    }

    if (EI_API_CIP_eERR_OK != EI_API_CIP_setInstanceServiceFunc(cipNode, 0x0109, 0x0001, &srvc))
    {
        goto laError;
    }

    // Add callback for Reset service of class ID 0x01.
    EI_API_CIP_SService_t srvcReset = { EI_API_CIP_eSC_RESET, 0, NULL, EI_APP_RST_service };

    if (EI_API_CIP_eERR_OK != EI_API_CIP_setInstanceServiceFunc(cipNode, 0x01, 0x01, &srvcReset))
    {
        goto laError;
    }

    res = true;

laError:

    return res;
}


/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Basic initialization function.
 *
 *  \details
 *  Creates a new EtherNet/IP&trade; Adapter.<br>
 *  Initializes data structures from non-volatile storage.<br>
 *  Registers stack error handler.<br>
 *  Initializes the Adapter.<br>
 *  Create a CIP&trade; node.<br>
 *
 */
static bool EI_APP_TASK_init(APP_SParams_t* pParam)
{
    bool result = true;

    EI_API_ADP_SParam_t macAddr = {0};

    EI_API_ADP_SInit_t initParams = {0};

    // Initialize adapter for 1 (one) interface.
    adapter_s = EI_API_ADP_new(1);

    EI_APP_CFG_init(adapter_s, &pParam->config);

#if defined(EIP_TIME_SYNC) && (EIP_TIME_SYNC == 1)
    EI_API_ADP_setTimeSyncSupported(adapter_s);
#endif

    // Setup error handler for the EtherNet/IP stack.
    EI_API_ADP_setErrorHandlerFunc(EI_APP_TASK_stackErrorHandlerCb);

    EI_APP_TASK_getMacAddr();

    // Init module for non-volatile data.
    EI_APP_NV_init(adapter_s, &pParam->nv);

    // Read non-volatile data
    EI_APP_CFG_read();

    // Initialize data for the adapter.
    EI_APP_TASK_adpInit(adapter_s);

#if defined(EIP_QUICK_CONNECT) && (EIP_QUICK_CONNECT == 1)
    // Enable QuickConnect
    EI_API_ADP_setQuickConnectSupported(adapter_s);
#endif

    EI_APP_TASK_stackInit(pParam);

    // Create a CIP node.
    cipNode_s = EI_API_CIP_NODE_new(&pParam->cip);

    if (NULL == cipNode_s)
    {
        return false;
    }

    // Create callbacks for changed values.
    EI_APP_TASK_cipCreateCallback(cipNode_s);

    // Create vendor specific classes.
    EI_APP_DEVICE_PROFILE_TASK_init(adapter_s, cipNode_s);

    // Finally apply.
    initParams.taskPrioCyclicIo  = pParam->adapter.taskPrioCyclicIo;
    initParams.taskPrioPacket    = pParam->adapter.taskPrioPacket;
    initParams.taskPrioStatistic = pParam->adapter.taskPrioStatistic;

    initParams.dll.ptp.taskPrioTsDelayRqTx = pParam->adapter.taskPrioPtpDelayRqTx;   /* Task priority for TX delay request. */
    initParams.dll.ptp.taskPrioTxTimeStamp = pParam->adapter.taskPrioPtpTxTimeStamp; /* Task priority for TX time stamp P1 and P2. */
    initParams.dll.ptp.taskPrioNRT         = pParam->adapter.taskPrioPtpNRT;         /* Task priority for NRT. */
    initParams.dll.ptp.taskPrioBackground  = pParam->adapter.taskPrioPtpBackground;  /* Task priority for background thread. */

    initParams.dll.lldp.taskPrioReceive    = pParam->adapter.taskPrioLldpReceive;    /* Task priority for receive thread. */

    EI_API_ADP_init(adapter_s, initParams);

    EI_API_ADP_setAcdDelay(adapter_s, pParam->acd.initialDelay);

    EI_API_ADP_getMacAddr(adapter_s, &macAddr);

    if (macAddr.data != NULL)
    {
        OSAL_printf("EI_API_ADP_getMacAddr:  %02x:%02x:%02x:%02x:%02x:%02x\r\n",
            macAddr.data[0],
            macAddr.data[1],
            macAddr.data[2],
            macAddr.data[3],
            macAddr.data[4],
            macAddr.data[5]);
    }

    return result;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Cyclically called run function.
 *
 *  \details
 *  Cyclically called run function, handles the EtherNet/IP stack and hardware
 *  specific functionality, such as reading switches, reading inputs, setting outputs
 *  and LEDs.
 *
 */
static void EI_APP_TASK_run(EI_API_CIP_NODE_T* cipNode)
{
    EI_API_ADP_run();

    EI_APP_DEVICE_PROFILE_TASK_run(cipNode);
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Cyclically calls run functions of EtherNet/IP Adapter and device profile.
 *
 *  \details
 *  Cyclically calls run function, handles the EtherNet/IP stack and hardware
 *  specific functionality, such as reading inputs, setting outputs
 *  and LEDs. It evaluates reset request and executes it when required.
 *
 *
 */

void EI_APP_TASK_main(void* pvTaskArg_p)
{
    uint32_t err = OSAL_NO_ERROR;
    int16_t  resetServiceFlag = 0;

    APP_SInstance_t* pAppInstance = (APP_SInstance_t*) pvTaskArg_p;
    
    /*! Uncomment this line to debug issues using CCS */
    // volatile int8_t loopHalt = 1;
    // while(loopHalt)
    // {
    //     DebugP_log("%d", loopHalt);
    // }

    CMN_BOARD_init();

    if(EI_APP_MUTEX_eERR_NOERROR != EI_APP_Mutex_init())
    {
        //Fatal error
        return;
    }
    CUST_DRIVERS_init(&pAppInstance->config.customDrivers);

    EI_APP_UART_init(&pAppInstance->config.uart);

#ifndef ENABLE_INTERCORE_TUNNELING
    EI_APP_LED_init(&pAppInstance->config.led);
#endif

    OSAL_registerPrintOut(NULL, EI_APP_UART_printf);

    err = HWAL_init ();
    if (err != OSAL_NO_ERROR)
    {
        goto laError;
    }

    err = EI_APP_DEVICE_PROFILE_init();

    if (EI_APP_DEVICE_PROFILE_ERR_OK != err)
    {
        OSAL_printf("Fatal error: Device profile initialization failed\n");
        return;
    }

    if (false == EI_APP_TASK_init(&pAppInstance->config))
    {
        OSAL_printf("Fatal error: Application initialization failed\n");
        return;
    }

#if (defined CPU_LOAD_MONITOR) && (1==CPU_LOAD_MONITOR)

    if (false == APP_startWebServerTask(&pAppInstance->config.webServer))
    {
        OSAL_printf("Fatal error: Failed to start Webserver task. \n");
        return;
    }

    CMN_CPU_API_startMonitor(&pAppInstance->config.cpuLoad);
#endif

#ifdef ENABLE_INTERCORE_TUNNELING
    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();

    configMethod = pRuntimeData->adapter.configurationMethod;

    DebugP_log("Main Core init\r\n");

    /*! Initialize the IPC Task */
    AppCtrl_createRecvTask();
    
    sys_lock_tcpip_core();
    /*! Handle the IC up notify to bring IC-netif up */
    EthApp_initNetif();
    /* Start the TCP server for the core */
    AppTcp_startServer();
    /* Wait for the TCP initialization to complete
     * Send the IP for remote cores for self test
     */
    ClockP_sleep(2);
    AppCtrl_sendIPNotify();

    lwiperf_example_init();
    sys_thread_new("UDP Iperf", start_application, NULL, DEFAULT_THREAD_STACKSIZE,
                               UDP_IPERF_THREAD_PRIO);
    sys_unlock_tcpip_core();
#endif

    for (;;)
    {
        EI_APP_TASK_run(cipNode_s);

        resetServiceFlag = EI_APP_RST_getServiceFlag();
        if (resetServiceFlag  != -1)
        {
            break;
        }

        if(true == EI_APP_CFG_isChanged())
        {
            EI_APP_CFG_write(false);
        }

#ifdef ENABLE_INTERCORE_TUNNELING
        App_printCpuLoad();
#endif
        OSAL_SCHED_yield();
    }

laError:
    OSAL_printf("resetting device\n");

    EI_APP_UART_deInit();

    EI_APP_TASK_cleanup(adapter_s, cipNode_s);

    EI_API_ADP_pruicssStop();

    SOC_generateSwWarmResetMcuDomain();

    CMN_APP_mainExit();
}


/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  General stack error handler.
 *
 *  \details
 *  General stack error handler. Reporting of stack internal errors.
 *
 */
void EI_APP_TASK_stackErrorHandlerCb (uint32_t errorCode,           //!< [in] Error code
                                      uint8_t  fatal,               //!< [in] Is Error fatal
                                      uint8_t  numOfPara,           //!< [in] Number of parameter
                                      va_list  argPtr)              //!< [in] Error arguments
{
    OSAL_printf("###### Stack Error: 0x%08x, %s ######\n", errorCode, fatal == 0 ? "non fatal" : "fatal");

    if (fatal)
    {
        // TODO: handle the app_cleanup return codes.
        EI_APP_TASK_cleanup(adapter_s, cipNode_s);
        // TODO: Return an appropriate exit code in this case.
        while(1);
    }

}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  General OS error handler.
 *
 *  \details
 *  General OS error handler. Reporting of OS errors.
 *
 */
void EI_APP_TASK_osErrorHandlerCb (uint32_t errorCode,      //!< [in] Error code
                                      bool     fatal,          //!< [in] Is Error fatal
                                      uint8_t  paraCnt,        //!< [in] Parameter counter
                                      va_list  argptr)         //!< [in] Error arguments
{
    globalError = errorCode;

    OSAL_printf ("\nError: 0x%8.8x, Fatal: %s", errorCode, fatal ? "yes" : "no");

#if (defined CMN_MEM_TRACE) && (1==CMN_MEM_TRACE)
        extern uint32_t __HEAP_START;
        extern uint32_t __HEAP_END;

        uint32_t heapStart = (uint32_t) &__HEAP_START;
        uint32_t heapSize  = (uint32_t) &(__HEAP_END) - (uint32_t) &(__HEAP_START);

        CMN_MEM_traceHeapCheck(heapStart, heapSize);

        void* ptr = OSAL_MEMORY_calloc(1078, 1);

        if (ptr != NULL)
        {
            OSAL_MEMORY_free(ptr);
        }
        while(1);
#endif

    if (fatal == true)
    {
        while(1);
    }
}

static void EI_APP_TASK_stackInit(APP_SParams_t* pParam)
{
    uint32_t err;
    EIP_SLoadParameter tParam;

    CUST_PRUICSS_ConfigParams_t* pPruIcssCfgParams = CUST_PRUICSS_getCfgParams();

    OSAL_MEMORY_memset (&tParam, 0, sizeof (EIP_SLoadParameter));
    OSAL_MEMORY_memcpy (tParam.ai8uMacAddr, EI_APP_TASK_macAddress, 6);

    tParam.pruIcssCfgId                 = pPruIcssCfgParams->sysConfigId;
    tParam.pPruIcssCfg                  = (PRUICSS_ConfigPtr) pPruIcssCfgParams->pPruIcssCfg;                  /* Pointer to PRU-ICSS block used by stack. */
    tParam.pPru0Firmware                = pPruIcssCfgParams->pru[0].pFirmware;                                 /* Pointer to PRU-ICSS firmware of PRU 0. */
    tParam.pPru0FirmwareLength          = pPruIcssCfgParams->pru[0].frmLength;                                 /* Length of PRU 0 firmware */
    tParam.pPru1Firmware                = pPruIcssCfgParams->pru[1].pFirmware;                                 /* Pointer to PRU-ICSS firmware of PRU 1. */
    tParam.pPru1FirmwareLength          = pPruIcssCfgParams->pru[1].frmLength;                                 /* Length of PRU 1 firmware */
    tParam.pEthPhyCfg[0]                = (ETHPHY_ConfigPtr)  pPruIcssCfgParams->pru[0].ethPhy.pConfig;        /* Pointer to PRU-ICSS ETHPHY0 configuration used by stack. */
    tParam.pEthPhyCfg[1]                = (ETHPHY_ConfigPtr)  pPruIcssCfgParams->pru[1].ethPhy.pConfig;        /* Pointer to PRU-ICSS ETHPHY1 configuration used by stack. */
    tParam.ethPhyHandle[0]              = pPruIcssCfgParams->pru[0].ethPhy.handle;                             /* PRU-ICSS ETHPHY0 handle used by stack. */
    tParam.ethPhyHandle[1]              = pPruIcssCfgParams->pru[1].ethPhy.handle;                             /* PRU-ICSS ETHPHY1 handle used by stack. */
    tParam.mdioManualMode               = pPruIcssCfgParams->manualMdioMode.isEnabled;                         /* MDIO Manual Mode */
    tParam.mdioManualModeBaseAddress    = pPruIcssCfgParams->manualMdioMode.baseAddress;                       /* MDIO Manual Mode Base Address */
    tParam.pMdioManualModeFirmware      = pPruIcssCfgParams->manualMdioMode.pFirmware;                         /* Pointer to MDIO Manual Mode firmware */
    tParam.mdioManualModeFirmwareLength = pPruIcssCfgParams->manualMdioMode.frmLength;                         /* Size of MDIO Manual Mode firmware in bytes */
    tParam.mdioManualModeFirmwareConfig = pPruIcssCfgParams->manualMdioMode.frmConfig;                         /* Configuration of MDIO Manual Mode firmware */
    tParam.taskPrioPhyMdixTask          = pParam->customDrivers.pruIcss.ethPhy.taskPrioPhyMdixTask;            /* Task priority for PHY MDIX task. */

    err = EI_API_ADP_pruicssInit (&tParam);
    if (err)
    {
        goto laError;
    }

    OSAL_printf("+EI_API_ADP_pruicssStart\r\n");
    EI_API_ADP_pruicssStart();
    OSAL_printf("-EI_API_ADP_pruicssStart\r\n");

    return;

//-------------------------------------------------------------------------------------------------
laError:

    printf ("\nStack Init Error: 0x%8.8x", err);
    return;
}

/*!
*  <!-- Description: -->
*
*  \brief
*  Get MAC address.
*
*  \details
*  By default, retrieve the MAC address used by the EtherNet/IP adapter from the SoC.
*
*/
static uint8_t* EI_APP_TASK_getMacAddr (void)
{
#ifndef _DEBUG_USE_KUNBUS_MAC_ADDRESS
    static uint8_t mac_addr[6];
    const uint32_t mac_address_upper_16_bits = *REG_MAC_ID0;
    const uint32_t mac_address_lower_32_bits = *REG_MAC_ID1;
    mac_addr[0] = (uint8_t) (mac_address_upper_16_bits >> 8);
    mac_addr[1] = (uint8_t) (mac_address_upper_16_bits);
    mac_addr[2] = (uint8_t) (mac_address_lower_32_bits >> 24);
    mac_addr[3] = (uint8_t) (mac_address_lower_32_bits >> 16);
    mac_addr[4] = (uint8_t) (mac_address_lower_32_bits >> 8);
    mac_addr[5] = (uint8_t) (mac_address_lower_32_bits);
    // EI_APP_aMacAddr_g is directly used in lwip
    OSAL_MEMORY_memcpy(EI_APP_TASK_macAddress, mac_addr, 6);
    return mac_addr;
#else
    return EI_APP_TASK_macAddress;
#endif
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Get self core ID.
 *
 *  \details
 *  By default, returns the self core ID, associated with R5F0_0 core.
 *
 */
uint32_t EnetSoc_getCoreId(void)
{
    uint32_t coreId = CSL_CORE_ID_R5FSS0_0;
    return coreId;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Returns self core ID.
 *
 *  \details
 *  By default, returns the value received for the self core ID to the caller.
 *
 */
uint32_t App_getSelfCoreId()
{
    uint32_t coreId = EnetSoc_getCoreId();
    return coreId;
}

/*!
*  <!-- Description: -->
*
*  \brief
*  Helper function enabling Special Unicast MAC address handling and writing the MAC address. 
*
*  \details
*  Helper function to enable Special Unicast MAC address handling and write the Linux interface MAC address. 
*
*/
void configureSpecialMacHelper(uint8_t *macAddress, uint32_t pru1DramBase, uint32_t specialUnicastMACAddrOffset, uint32_t specialUnicastMACAddrFeatureEnableOffset) 
{
    uint32_t            temp_addr = 0U;
    volatile uint8_t    *specialUnicastMACAddressPtr = NULL;
    volatile uint8_t    *specialUnicastMACAddressFeatureEnablePtr = NULL;

    temp_addr = (pru1DramBase + specialUnicastMACAddrOffset);
    specialUnicastMACAddressPtr = (uint8_t*)(temp_addr);

    temp_addr = (pru1DramBase + specialUnicastMACAddrFeatureEnableOffset);
    specialUnicastMACAddressFeatureEnablePtr = (uint8_t*)(temp_addr);

    /* Special Unicast MAC Address feature enable */
    *(specialUnicastMACAddressFeatureEnablePtr) = 1;
    /* Write back the special unicast MAC Address */
    memcpy((void *)specialUnicastMACAddressPtr, macAddress, 6);

    DebugP_log("Adding new MAC to FDB  \r\n");
}


/*!
*  <!-- Description: -->
*
*  \brief
*  Print CPU load
*
*  \details
*  By default, prints CPU load at every 5 seconds on the console. 
*
*/
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

/*!
*  <!-- Description: -->
*
*  \brief
*  Add multicast address to the Bridge
*
*  \details
*  Add the new multicast MAC address entry to LwIP Bridge
*
*/
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

/*!
*  <!-- Description: -->
*
*  \brief
*  Remove multicast address from the Bridge
*
*  \details
*  Remove a multicast MAC address entry from LwIP Bridge
*
*/
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

/*!
*  <!-- Description: -->
*
*  \brief
*  Add special unicast MAC address 
*
*  \details
*  Add the Linux interface MAC as special unicast MAC address
*
*/
int32_t AppCtrl_addMacAddr2fbd(Icss_MacAddr mac) 
{
    int32_t status = ICVE_OK;
    uint8_t *pMac = (uint8_t *)(mac.macAddr);
    uint32_t pru1DramBase = 0x030082000;
    /*  6 bytes in order to store the special unicast MAC address */
    uint32_t specialUnicastMACAddrOffset = 0x1FB0; 
    /* 1 byte in order to check if the special unicast MAC address feature is enabled or disabled */ 
    uint32_t specialUnicastMACAddrFeatureEnableOffset = 0x1FB6;
    
    configureSpecialMacHelper(pMac, pru1DramBase, specialUnicastMACAddrOffset, specialUnicastMACAddrFeatureEnableOffset);

    return status;
}

/*!
*  <!-- Description: -->
*
*  \brief
*  Get Linux availability status
*
*  \details
*  By default, returns 1 since the example uses Linux as the remote core.
*
*/
bool App_IsLinuxPresent()
{
    return 1;
}

