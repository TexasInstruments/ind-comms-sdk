/*!
 *  \file IOLM_Port_SMI.c
 *
 *  \brief
 *  SOC/OS specific IO Link SMI functions.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 KUNBUS GmbH.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  <ol>
 *  <li>Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer./<li>
 *  <li>Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.</li>
 *  <li>Neither the name of the copyright holder nor the names of its contributors
 *  may be used to endorse or promote products derived from this software without
 *  specific prior written permission.</li>
 *  </ol>
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 *  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 */




#define IOLM_SMI_TASK_STACK_SIZE       (0x2000U / sizeof(configSTACK_DEPTH_TYPE))

#include <FreeRTOS.h>
#include <drivers/uart.h>
#include <ti_drivers_open_close.h>
#include <osal.h>
#include <IOLM_SMI.h>
#include <IOL_Serial.h>
#include "IOLM_Port_SMI.h"

OSAL_SCHED_MutexHandle_t*pSemSMI_g;
OSAL_SCHED_EventHandle_t *pSemUartWake_g;

volatile bool boUartTxPending_g = false;
volatile bool boUartRxComplete_g = false;
uint8_t au8UARTRXBuffer_g[UART_SIZE_RX_BUFFER];
volatile uint32_t u32UARTRxPosWrite;
uint32_t u32UARTRxPosRead;

IOL_Serial_SHandle suUartSerial_g;
uint8_t au8UartSerialDatRx_g[IOL_SERIAL_RX_DEFAULT_LENGTH];
uint8_t au8UartSerialData_g[UART_NUM_TX_BUFFER * UART_SIZE_TX_BUFFER];
uint32_t au32UartSerialLength_g[UART_NUM_TX_BUFFER];
uint32_t u32ReceivedBytes_g;
uint32_t u32ReceiveExpected_g;
UART_Handle UartHandle_g = NULL;
bool headerActive_g = false;
UART_Transaction rxTransaction_g;
UART_Transaction txTransaction_g;

int32_t s32LastRxCancelTime_g;

static void* IOLM_pSmiTaskHandle_s;
static StackType_t IOLM_pSmiTaskStack_s[IOLM_SMI_TASK_STACK_SIZE]   \
                   __attribute__((aligned(32), section(".threadstack"))) = {0};


/**
\brief UART Rx Callback

Rx Callback for UART

\param[in]  handle          Handle for UART instance
\param[in]  transaction     Pointer to UART transaction

*/
void vUartRxCallback(UART_Handle handle, UART_Transaction *transaction)
{
    u32ReceivedBytes_g = transaction->count;
    boUartRxComplete_g = true;
    OSAL_EVT_set(pSemUartWake_g);
}

/**
\brief UART Tx Callback

Tx Callback for UART

\param[in]  handle          Handle for UART instance
\param[in]  transaction     Pointer to UART transaction

*/
void vUartTxCallback(UART_Handle handle, UART_Transaction* transaction)
{
    (void)handle;
    IOL_Serial_vReleaseTxBuffer(&suUartSerial_g);
    boUartTxPending_g = false;
    OSAL_EVT_set(pSemUartWake_g);
}

/**
\brief UART run task

Process UART data

*/
void OSAL_FUNC_NORETURN vUARTrun()
{
    while (1)
    {
        if (boUartTxPending_g == false)
        {
            uint32_t u32Length;
            uint8_t* pu8Buffer;
            pu8Buffer = IOL_Serial_pu8GetTxBuffer(&suUartSerial_g, &u32Length);
            if (pu8Buffer)
            {
                /* new data to send available */
                UART_Transaction_init(&txTransaction_g);
                boUartTxPending_g = true;
                txTransaction_g.count = u32Length;
                txTransaction_g.buf = (void*)pu8Buffer;
                txTransaction_g.args = NULL;
                UART_write(UartHandle_g, &txTransaction_g);
            }
        }
        if (boUartRxComplete_g != false)
        {
            INT8U u8HeaderLength = IOL_Serial_u8GetHeaderLength(&suUartSerial_g);

            /* interpret result */
            IOL_Serial_vReceive(&suUartSerial_g, au8UARTRXBuffer_g, u32ReceivedBytes_g);

            /* reset variables */
            u32ReceiveExpected_g = 0;
            /* prepare next message */
            if (headerActive_g && (u32ReceivedBytes_g >= u8HeaderLength))
            {
                /* received header -> some data will follow */
                headerActive_g = false;
                u32ReceiveExpected_g = IOL_Serial_u16GetArgBlockLength(&suUartSerial_g);
                s32LastRxCancelTime_g = OSAL_getMsTick();
            }

            if (u32ReceiveExpected_g == 0)
            {
                /* no data follows -> wait for header */
                u32ReceiveExpected_g = u8HeaderLength;
                headerActive_g = true;
            }
            /* start UART */
            u32ReceivedBytes_g = 0;
            boUartRxComplete_g = false;

            UART_Transaction_init(&rxTransaction_g);
            rxTransaction_g.count = u32ReceiveExpected_g;
            rxTransaction_g.buf = (void*)au8UARTRXBuffer_g;
            rxTransaction_g.args = NULL;
            UART_read(UartHandle_g, &rxTransaction_g);
        }

        OSAL_MTX_get(pSemSMI_g, OSAL_WAIT_INFINITE, NULL);
        IOL_Serial_vRxProcessing(&suUartSerial_g);
        OSAL_MTX_release(pSemSMI_g);
        if (headerActive_g == false && OSAL_getMsTick() - s32LastRxCancelTime_g >
            (IOL_SERIAL_TIMEOUT_MS / IOLM_SYSTICK_INTERVAL_MS))
        {
            UART_readCancel(UartHandle_g, &rxTransaction_g);

            /* trigger dummy receive */
            boUartRxComplete_g = true;
            u32ReceivedBytes_g = 0;
        }
        OSAL_EVT_wait(pSemUartWake_g, 1, NULL);
        OSAL_SCHED_yield();
    }
}

/**
\brief SMI Port Init

Set up neccessary configurations for UART and the SMI-Uart-Task

*/
void IOLM_SMI_portInit(void)
{
    suUartSerial_g.u32BufferCount   = UART_NUM_TX_BUFFER;
    suUartSerial_g.u32BufferLen     = UART_SIZE_TX_BUFFER;
    suUartSerial_g.pu8Data          = au8UartSerialData_g;
    suUartSerial_g.pu32Length       = au32UartSerialLength_g;
    suUartSerial_g.pu8RxBuffer      = au8UartSerialDatRx_g;
    suUartSerial_g.u32RxBufferLen   = sizeof(au8UartSerialDatRx_g);
    suUartSerial_g.cbRxGeneric      = IOLM_SMI_vGenericReq;
    suUartSerial_g.cbRxStd          = IOLM_SMI_vStdReq;
#if IOLM_SERIAL_STD
    suUartSerial_g.eCbType = IOLM_SMI_eOrigin_Std;
#else
    suUartSerial_g.eCbType = IOLM_SMI_eOrigin_Generic;
#endif

    IOL_Serial_vInit(&suUartSerial_g);

    u32UARTRxPosRead    = 0;
    u32UARTRxPosWrite   = 0;

    pSemSMI_g       = OSAL_MTXCTRLBLK_alloc();
    pSemUartWake_g  = OSAL_EVTCTRLBLK_alloc();
    OSAL_MTX_init(pSemSMI_g);
    OSAL_EVT_init(pSemUartWake_g);

    UartHandle_g = gUartHandle[CONFIG_UART_SMI];

    if (!UartHandle_g) {
        OSAL_printf("Error while starting serial interface\r\n");
        OSAL_error(__FILE__, __LINE__, OSAL_STARTUP_SERIAL, true, 0);
    }

    // dummy receive to trigger correct receive
    boUartRxComplete_g = true;
    u32ReceivedBytes_g = 0;
    IOLM_pSmiTaskHandle_s = OSAL_SCHED_startTask((OSAL_SCHED_CBTask_t)vUARTrun,
                                                 NULL,
                                                 OSAL_TASK_Prio_IOL_SMI,
                                                 (uint8_t*)IOLM_pSmiTaskStack_s,
                                                 sizeof(IOLM_pSmiTaskStack_s),
                                                 OSAL_OS_START_TASK_FLG_NONE,
                                                 "SMI Task");

    if (NULL == IOLM_pSmiTaskHandle_s)
    {
        OSAL_error(__FILE__, __LINE__, OSAL_eERR_INVALIDSTATE, true, 1,
                   "Creating UART SMI Task failed.\r\n");
    }
}

/**
\brief SMI generic confirmation

The function is called on each confirmation and indication from the
SMI module.

\param[in]  pHeader_p     Header
\param[in]  pArgBlock_p   Argblock

*/
void IOLM_SMI_cbGenericCnf(IOLM_SMI_SHeader* pHeader_p, INT8U* pArgBlock_p)
{
    OSAL_MTX_get(pSemSMI_g, OSAL_WAIT_INFINITE, NULL);
    IOL_Serial_vSendSmi(&suUartSerial_g, pHeader_p, pArgBlock_p);
    OSAL_MTX_release(pSemSMI_g);
    OSAL_EVT_set(pSemUartWake_g);
}

/**
\brief SMI std indication

The function is called on each confirmation and indication from the
SMI module.

\param[in]  pHeader_p     Header
\param[in]  pArgBlock_p   Argblock

*/
void IOLM_SMI_cbStdInd(IOLM_SMI_SStdHeader* pHeader_p, INT8U* pArgBlock_p)
{
    OSAL_MTX_get(pSemSMI_g, OSAL_WAIT_INFINITE, NULL);
    IOL_Serial_vSendSmiStd(&suUartSerial_g, pHeader_p, pArgBlock_p);
    OSAL_MTX_release(pSemSMI_g);
    OSAL_EVT_set(pSemUartWake_g);
}


