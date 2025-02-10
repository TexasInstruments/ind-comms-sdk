/*!
 *  \file main.c
 *
 *  \brief
 *  Demo application
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drivers/pinmux.h"
#include "hwal.h"
#include "osal.h"
#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#include "pn_api_iod_callbacks.h"
#include "pn_api_iod_error.h"
#include "pn_api_iod_handle.h"
#include "pn_api_iod_startup.h"

#include "pn_app_iod_alarm.h"
#include "pn_app_iod_bsp.h"
#include "pn_app_iod_communication.h"
#include "pn_app_iod_data.h"
#include "pn_app_iod_device.h"
#include "pn_app_iod_mod_cfg.h"
#include "pn_app_iod_record.h"
#include "pn_app_iod_settings.h"
#include "pn_app_iod_utils.h"

#define PN_APP_MAIN_TASK_PRIO                   22 /* TASK_PRIO_MAIN */
#define PN_APP_UART_TASK_PRIO                   2
#define PN_APP_MAIN_TASK_STACK_SIZE             (2048+256)
#define PN_APP_UART_TASK_STACK_SIZE             1024
#define ASYNC_REC_RSP_WAITING_COUNTER           5000 /* Number of counts before sending a response */

/*!
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t asyncRecReadTimer;     /*!< For asynchronous record read responses */
    uint32_t asyncRecWriteTimer;    /*!< For asynchronous record write responses  */
} PN_APP_IOD_asyncRecRspTimer_t;

static char aOutStream_s[1024] = { 0 };
static uint32_t uartWritePos = 0;
static uint32_t uartReadPos = 0;
void *uartSignal;
void *uartMutex;
static void* PN_APP_mainHandle;
static void* PN_APP_uartTaskHandle;
uint8_t PN_APP_mainTaskStack[PN_APP_MAIN_TASK_STACK_SIZE]__attribute__((aligned(32), section(".threadstack")));

/* Handles of asynchronous record read/write request */
extern void *asyncRecReadHandle;
extern void *asyncRecWriteHandle;


/*!
 * \brief
 * Callback function to handle system errors.
 *
 * \details
 * This function is called in response to a system error. It informs the user
 * of the error, and in case the error is fatal, it terminates the program.
 *
 * \param[in]       errorCode           Error code.
 * \param[in]       fatal               Fatal error indicator.
 * \param[in]       paramCnt            Number of parameters received for the error.
 * \param[in]       argptr              List of error parameters (arguments).
 *
 * \ingroup PN_APP_IOD_MAIN_DOXY_GROUP
 *
 */
void PN_APP_IOD_errorHandlerCallback(uint32_t errorCode, bool fatal, uint8_t paramCnt, va_list argptr)
{
    int32_t indexArg;
    uint32_t arg;

    OSAL_printf("\r\nError: 0x%8.8x, Fatal: %s", errorCode, fatal ? "yes" : "no");
    for (indexArg = 0; indexArg < paramCnt; indexArg++)
    {
        arg = va_arg(argptr, uint32_t);
        OSAL_printf(", P%d: 0x%8.8x", indexArg, arg);
    }

    if (fatal == true)
    {
        /* It's possible to introduce an infinite while loop here */
        exit(1);
    }
}

/*!
 * \brief
 * Initialize application data.
 *
 * \details
 * This function initializes all global variables and array that are required
 * for further operations. This includes, AR information, IO data arrays and
 * record lists and modes.
 *
 * \ingroup PN_APP_IOD_MAIN_DOXY_GROUP
 *
 */
static void PN_APP_IOD_init(void)
{
    bool asyncRead = false;
    bool asyncWrite = false;

    /* Initialize AR information */
    PN_APP_IOD_initArInfo();

    /* Initialize application IO datasets */
    PN_APP_IOD_initAppData();

    /* Initialize user record list and information */
    PN_APP_IOD_initRecordInfo(asyncRead, asyncWrite);
}

/*!
 * \brief
 * Initialize device instance.
 *
 * \details
 * This function initializes the device instance that is later passed to Profinet stack.
 *
 * \param[in]       iodInst            Device instance.
 *
 * \ingroup PN_APP_IOD_MAIN_DOXY_GROUP
 *
 */
static void PN_APP_IOD_buildDevInstance(PN_API_IOD_IodInstance_t *iodInst)
{
    /* Set IO-device instance info according to the user configuration found in pn_app_iod_cfg.h */
    iodInst->vendorId = PN_API_IOD_VENDOR_ID;
    iodInst->deviceId = PN_API_IOD_DEVICE_ID;
    iodInst->pDevType = (int8_t *)PN_API_IOD_DEV_TYPE;
    iodInst->maxNumOfSubslots = PN_API_IOD_MAX_NUM_OF_SUBSLOTS;
    iodInst->maxNumOfBytesPerSubslot = PN_API_IOD_NUM_OF_BYTES_PER_SUBSLOT;
}

/*!
 * \brief
 * Initialize device annotation.
 *
 * \details
 * This function initializes the device annotation that is later passed to Profinet stack.
 *
 * \param[in]       devAnnotation      Device annotation.
 *
 * \ingroup PN_APP_IOD_MAIN_DOXY_GROUP
 *
 */
static void PN_APP_IOD_buildDevAnnotation(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAnnotation_t *devAnnotation)
{
#if SOC_AM243X
    PN_API_IOD_setDeviceType(pnHandle, "AM243EVM");
#elif SOC_AM64X
    PN_API_IOD_setDeviceType(pnHandle, "AM64EVM");
#else
#error Unsupported target type
#endif
    PN_API_IOD_setDeviceOrderId(pnHandle, "TI-Sitara EVM");
    PN_API_IOD_setDeviceSerialNumber(pnHandle, "1234567890");
    PN_API_IOD_setDeviceHWRevision(pnHandle, 0x101C);
    PN_API_IOD_setDeviceVersionNumber(pnHandle, "V", 4, 1, 0, 0);

    /* Set IO-device annotation info according to the user configuration found in pn_app_iod_cfg.h */
    OSAL_MEMORY_memset(devAnnotation, ' ', sizeof(PN_API_IOD_DevAnnotation_t));

    OSAL_MEMORY_memcpy(
        &devAnnotation->deviceType,
        PN_API_IOD_DEV_TYPE,
        PN_API_IOD_DEV_TYPE_SIZE);

    OSAL_MEMORY_memcpy(
        &devAnnotation->orderId,
        PN_API_IOD_DEV_ANNOTATION_ORDER_ID,
        PN_API_IOD_DEV_ANNOTATION_ORDER_ID_SIZE);

    OSAL_MEMORY_memcpy(
        &devAnnotation->serialNum,
        PN_API_IOD_IM0_SERIAL_NUM,
        PN_API_IOD_IM0_SERIAL_NUM_SIZE);

    devAnnotation->hwRevision  = PN_API_IOD_HW_REV;
    devAnnotation->swRevisionPrefix = PN_API_IOD_VER_PREFIX;
    devAnnotation->swRevision1 = PN_API_IOD_VER_HH;
    devAnnotation->swRevision2 = PN_API_IOD_VER_H;
    devAnnotation->swRevision3 = PN_API_IOD_VER_L;
}

/*!
 * \brief
 * Build the list of callback functions.
 *
 * \details
 * This function assigns the functions implemented in the application to the
 * corresponding elements in the callback function list and register them.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_APP_IOD_MAIN_DOXY_GROUP
 *
 */
static uint32_t PN_APP_IOD_buildCallbacksList(PN_API_IOD_Handle_t *const pnHandle)
{
    uint32_t status = PN_API_NOT_OK;
    if (NULL != pnHandle)
    {
        /* register app callback functions */
        PN_API_IOD_Callbacks_t callbacks = {
            .arConnect = PN_APP_IOD_cbArConnectInd,
            .arDisconn = PN_APP_IOD_cbArDisconn,
            .reportNewIpAddr = PN_APP_IOD_cbReportipAddr,
            .resetToFactory = PN_APP_IOD_cbResetToFactory,
            .storeRemaMem = PN_APP_IOD_cbStoreRemaMem,
            .restoreRemaMem = PN_APP_IOD_cbRestoreRemaMem,
            .freeRemaMem = PN_APP_IOD_cbFreeRemaMem,
            .setLed = PN_APP_IOD_cbSetLed,
            .stopLedBlink = PN_APP_IOD_cbStopLedBlink,
            .startLedBlink = PN_APP_IOD_cbStartLedBlink,
            .arOwnership = PN_APP_IOD_cbArOwnershipInd,
            .paramEnd = PN_APP_IOD_cbParamEnd,
            .readData = PN_APP_IOD_cbDataRead,
            .writeData = PN_APP_IOD_cbDataWrite,
            .recordRead = PN_APP_IOD_cbRecordRead,
            .arInData = PN_APP_IOD_cbArInData,
            .reportArFsuRecord = PN_APP_IOD_cbReportArFSURecord,
            .recordWrite = PN_APP_IOD_cbRecordWrite,
            .newModPull = PN_APP_IOD_cbNewModPull,
            .readIOxSDataOnly = PN_APP_IOD_cbDataReadIOxSOnly,
            .writeIOxSDataOnly = PN_APP_IOD_cbDataWriteIOxSOnly,
            .readyForInputUpdate = PN_APP_IOD_cbReadyForInputUpdate,
            .newModPlug = PN_APP_IOD_cbNewModPlug,
            .asyncReqDone = PN_APP_IOD_cbAsyncReqDone,
            .devAlarm = PN_APP_IOD_cbDevAlarm,
            .outSubmodSubstValRead = PN_APP_IOD_cbOutSubmodSubstValRead,
            .errorLog = PN_APP_IOD_cbErrorLog,
            .updateAppCycle = PN_APP_IOD_updateAppCycle
        };

        /* Register callback functions */
        status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
    }
    return status;
}

/*!
 * \brief
 * Startup Profinet process.
 *
 * \details
 * This function calls for initialing all profinet-related data, creates a Profinet
 * device and starts up Profinet stack. It then runs infinitely trying to handle
 * asynchronous record read/write requests, if there's any.
 *
 * \ingroup PN_APP_IOD_MAIN_DOXY_GROUP
 *
 */
static void PN_APP_IOD_startup(void)
{
    PN_API_IOD_IodInstance_t iodInst = { 0 };
    PN_API_IOD_DevAnnotation_t devAnnotation = { 0 };
    PN_API_IOD_SubmodListEntry_t *pIoSubmodList = NULL;
    PN_API_IOD_Im0ListEntry_t *pIm0List = NULL;
    uint32_t ioSubmodListSize = 0;
    uint32_t im0ListSize = 0;
    bool checkRemaData = true;
    uint32_t status = PN_API_NOT_OK;
    PN_API_IOD_Handle_t* pHandle = NULL;
    PN_APP_IOD_asyncRecRspTimer_t asyncRecRspTimer;

    /* Initialize asynchronous record response timers */
    asyncRecRspTimer.asyncRecReadTimer = 0;
    asyncRecRspTimer.asyncRecWriteTimer = 0;

    /* Initialize remanent storage */
    PN_APP_IOD_remaInit();

    /* Initialize LED handling */
    PN_APP_IOD_ledInit();


    /* Initialize output hardware signal */
    PN_APP_IOD_outHwSignalInit();

    /* Initialize PNIO user application */
    PN_APP_IOD_init();

    /* Initialize PNIO stack */
    PN_API_IOD_initStack();

    /* Build IO-device instance */
    PN_APP_IOD_buildDevInstance(&iodInst);

    /* Create a device handle */
    pHandle = PN_API_IOD_new();

    /* Build IO-device Annotation */
    PN_APP_IOD_buildDevAnnotation(pHandle, &devAnnotation);
    if(NULL != pHandle)
    {
        /* Load submodule configuration data */
        status = PN_APP_IOD_loadSubmodConfig(pHandle, &pIoSubmodList, &ioSubmodListSize,
                                          &pIm0List, &im0ListSize, checkRemaData);
        if (status == PN_API_OK)
        {
            /* Create callback functions instance and register it */
            status = PN_APP_IOD_buildCallbacksList(pHandle);
        }

        if (status == PN_API_OK)
        {
            status = PN_API_IOD_startup(pHandle, &iodInst, &devAnnotation, pIoSubmodList,
                                    ioSubmodListSize, pIm0List, im0ListSize);
        }

        if (status == PN_API_OK)
        {
            status = PN_APP_IOD_initCyclicDataExchange(pHandle);
        }
    }

    while(1)
    {
        OSAL_SCHED_sleep(1);

        /* An asynchronous read request has been registered? */
        if (NULL != asyncRecReadHandle)
        {
            /* Wait for a few seconds, then respond */
            asyncRecRspTimer.asyncRecReadTimer++;
            if (ASYNC_REC_RSP_WAITING_COUNTER == asyncRecRspTimer.asyncRecReadTimer)
            {
                PN_APP_IOD_recordReadAsyncResponse();
                asyncRecRspTimer.asyncRecReadTimer = 0;
            }
        }

        /* An asynchronous write request has been registered? */
        if (NULL != asyncRecWriteHandle)
        {
            /* Wait for a few seconds, then respond */
            asyncRecRspTimer.asyncRecWriteTimer++;
            if (ASYNC_REC_RSP_WAITING_COUNTER == asyncRecRspTimer.asyncRecWriteTimer)
            {
                PN_APP_IOD_recordWriteAsyncResponse();
                asyncRecRspTimer.asyncRecWriteTimer = 0;
            }
        }
    }
}

/*!
 * \brief
 * Callback function to print.
 *
 * \details
 * This function is called when the system needs to print out something. It
 * initializes a UART transaction and writes the received data to it.
 *
 * \param[in]       pContext            Context (not used).
 * \param[in]       pFormat             Format of the printed data.
 * \param[in]       arg                 List of parameters (arguments) to be printed.
 *
 * \ingroup PN_APP_IOD_MAIN_DOXY_GROUP
 *
 */
void PN_APP_IOD_printf(void *pContext, const char *__restrict pFormat, va_list arg)
{
    int lengthWritten;
    char tmpString[256];
    char *tmpStringPos = tmpString;

    OSALUNREF_PARM(pContext);

    lengthWritten = vsnprintf(tmpString, sizeof(tmpString), pFormat, arg);

    OSAL_lockNamedMutex(uartMutex, OSAL_WAIT_INFINITE);
    while (lengthWritten > 0)
    {
        uint32_t lengthAvailable;
        uint32_t lengthWrite = lengthWritten;

        if (uartReadPos > uartWritePos)
        {
            lengthAvailable = uartReadPos - uartWritePos - 1;
        }
        else
        {
            lengthAvailable = sizeof(aOutStream_s) - uartWritePos;
            if (uartReadPos == 0)
                lengthAvailable -= 1;
        }

        if (lengthAvailable == 0)
        {
            break;
        }

        if (lengthWrite > lengthAvailable)
        {
            lengthWrite = lengthAvailable;
        }

        memcpy(&aOutStream_s[uartWritePos], tmpStringPos, lengthWrite);

        uartWritePos += lengthWrite;
        tmpStringPos += lengthWrite;
        lengthWritten -= lengthWrite;
        if (uartWritePos >= sizeof(aOutStream_s))
        {
            uartWritePos -= sizeof(aOutStream_s);
        }
    }
    OSAL_unLockNamedMutex(uartMutex);
    OSAL_postSignal(uartSignal);
}

/*!
 * \brief
 * Start the system.
 *
 * \details
 * This function opens SoC and board drivers, then calls Profinet startup function.
 *
 * \param[in]       pvTaskArg           Arguments passed during task initialization.
 *
 * \ingroup PN_APP_IOD_MAIN_DOXY_GROUP
 *
 */
static void PN_APP_IOD_mainTask(void *pvTaskArg)
{
    uint32_t systemRetVal = SystemP_FAILURE;

    (void)pvTaskArg;

    /* Open SoC and board drivers. */
    Drivers_open();
    systemRetVal = Board_driversOpen();
    DebugP_assert(systemRetVal == SystemP_SUCCESS);


    PN_APP_IOD_startup();

    OSAL_SCHED_exitTask(NULL);
}


/*!
 * \brief
 *  uart print task
 *
 * \details
 * This function, is used to do printf on a low prio thread
 *
 */
static void PN_APP_IOD_uartTask(void *pvTaskArg)
{
    (void)pvTaskArg;

    while(1)
    {
        uint32_t bytesToWrite;

        static UART_Transaction transaction; // in Interrupt mode this needs to be static

        while (uartReadPos == uartWritePos)
        {
            OSAL_waitSignal(uartSignal, 2);
        }

        UART_flushTxFifo(gUartHandle[CONFIG_UART_CONSOLE]);
        UART_Transaction_init(&transaction);

        bytesToWrite = uartWritePos; // read uartWrite only once to prevent changes due to higher prio
        if (bytesToWrite > uartReadPos)
        {
            transaction.count = bytesToWrite - uartReadPos;
        }
        else
        {
            transaction.count = sizeof(aOutStream_s) - uartReadPos;
        }

        transaction.buf = (void *)&aOutStream_s[uartReadPos];
        transaction.args = NULL;

        OSAL_lockNamedMutex(uartMutex, OSAL_WAIT_INFINITE);
        uartReadPos += transaction.count;
        if (uartReadPos == sizeof(aOutStream_s))
        {
            uartReadPos = 0;
        }
        OSAL_unLockNamedMutex(uartMutex);

        (void)UART_write(gUartHandle[CONFIG_UART_CONSOLE], &transaction);

    }

    OSAL_SCHED_exitTask(NULL);
}

/*!
 * \brief
 *  Main entry point.
 *
 * \details
 * This function, initializes and configures SoC hardware and specific OSAL and HWAL modules,
 * which are essential for PROFINET communication. Afterwards, it creates and starts the main
 * application task.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 */
int main(void)
{
    uint32_t status = PN_API_OK;
    uint8_t syncEvent = SYNC_OUT0;

    /* Initialize SoC specific modules. */
    System_init();

    /* Additional configuration to route the sync0 or sync1 signal to SYNC0_OUT PIN */
    PN_APP_IOD_tsrConfig(syncEvent);

    Board_init();
    /*
     * Both HWAL and OSAL use OSAL error handler for error reporting.
     * Therefore error handler should be registered prior the initialization.
     *
     * OSAL_init() must be called PRIOR to HWAL_init()
     */
    OSAL_registerErrorHandler(PN_APP_IOD_errorHandlerCallback);

    status = OSAL_init();
    if (PN_API_OK != status)
    {
        status = PN_API_NOT_OK;
    }

    status = HWAL_init();
    if (PN_API_OK != status)
    {
        status = PN_API_NOT_OK;
    }

    OSAL_registerPrintOut(NULL, PN_APP_IOD_printf);

    PN_APP_mainHandle = OSAL_SCHED_startTask(
        PN_APP_IOD_mainTask,
        NULL,
        PN_APP_MAIN_TASK_PRIO,
        PN_APP_mainTaskStack,
        sizeof(PN_APP_mainTaskStack),
        OSAL_OS_START_TASK_FLG_NONE,
        "app_main");

    if (NULL == PN_APP_mainHandle)
    {
        status = PN_API_NOT_OK;
    }

    uartSignal = OSAL_createSignal("uartSignal");
    uartMutex = OSAL_createNamedMutex("uartMutex");
    PN_APP_uartTaskHandle = OSAL_SCHED_startTask(
        PN_APP_IOD_uartTask,
        NULL,
        PN_APP_UART_TASK_PRIO,
        NULL,
        PN_APP_UART_TASK_STACK_SIZE,
        OSAL_OS_START_TASK_FLG_NONE,
        "uart_task");

    OSAL_startOs();

    return status;
}
