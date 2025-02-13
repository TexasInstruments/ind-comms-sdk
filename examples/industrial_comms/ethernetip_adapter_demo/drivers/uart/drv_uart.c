/*!
 *  \file drv_uart.c
 *
 *  \brief
 *  UART application driver.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2025 Texas Instruments Incorporated
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
#include <string.h>
#include <stdio.h>

#include "ti_drivers_open_close.h"

#include "osal.h"

#include "drivers/common/drv_common.h"
#include "drivers/uart/drv_uart.h"

typedef struct DRV_Uart
{
    UART_Handle                        handle;
    uint32_t                           instance;
    char                               aOutStream[0x200];
    UART_Transaction                   transaction;
}DRV_Uart_t;

static DRV_Uart_t DRV_uart_s = {0};

/*!
*
*  \brief
*  Provides handle to UART driver.
*
*  \return     UART_Handle  Handle to UART driver.
*
*  \retval     NULL           Failed.
*  \retval     Other          Success.
*
*/
UART_Handle DRV_UART_getHandle(uint32_t instanceId)
{
    UART_Handle handle = NULL;

#if (defined CONFIG_UART_NUM_INSTANCES) && (CONFIG_UART_NUM_INSTANCES > 0)
    if (CONFIG_UART_NUM_INSTANCES > instanceId)
    {
        handle = gUartHandle[instanceId];
    }
#else
    OSALUNREF_PARM(instanceId);
#endif

    return handle;
}

/*!
*
*  \brief
*  Initialize UART application part.
*
*  \param[in]  UART initialization parameters.
*
*  \return     error code as uint32_t
*
*  \retval     #OSAL_NO_ERROR                Success.
*  \retval     #OSAL_GENERAL_ERROR           Negative default value.
*  \retval     #OSAL_UART_DRV_HANDLE_INVALID UART handle is NULL.
*
*/
uint32_t DRV_UART_init(const DRV_UART_SInit_t* pParams)
{
    uint32_t result = OSAL_GENERAL_ERROR;

    DRV_uart_s.instance = pParams->instance;
    DRV_uart_s.handle   = DRV_UART_getHandle(DRV_uart_s.instance);

    if(NULL == DRV_uart_s.handle)
    {
        result = OSAL_UART_DRV_HANDLE_INVALID;
        goto laError;
    }

    result = OSAL_NO_ERROR;
    goto laError;

laError:
  return result;
}

/*!
*
*  \brief
*  De-initialize UART application part.
*
*  \return     error code as uint32_t
*
*  \retval     #OSAL_NO_ERROR                Success.
*  \retval     #OSAL_GENERAL_ERROR           Negative default value.
*  \retval     #OSAL_UART_DRV_HANDLE_INVALID UART handle is NULL.
*
*/
uint32_t DRV_UART_deInit(void)
{
    uint32_t result = OSAL_GENERAL_ERROR;

    if (NULL != DRV_uart_s.handle)
    {
        UART_flushTxFifo(DRV_uart_s.handle);
    }

    result = OSAL_NO_ERROR;

    return result;
}

/*!
*
*  \brief
*  UART printf output function.
*
*  \details
*  Printing of specific string to UART output.
*
*  \param[in]  pContext      Call context
*  \param[in]  pFormat       Format string.
*  \param[in]  argptr        Parameter list.
*
*
*/
void DRV_UART_printf(void* pContext, const char* pFormat, va_list argptr)
{
    /* @cppcheck_justify{unusedVariable} false-positive: variable is used */
    //cppcheck-suppress unusedVariable
    int32_t transferOK;
    /* @cppcheck_justify{unusedVariable} false-positive: variable is used */
    //cppcheck-suppress unusedVariable
    DRV_COMMON_Mutex_EError_t mutexRetVal;

    OSALUNREF_PARM(pContext);



    mutexRetVal = DRV_COMMON_Mutex_Lock(DRV_COMMON_MUTEX_UART, OSAL_WAIT_INFINITE);

    if(DRV_COMMON_MUTEX_eERR_NOERROR == mutexRetVal)
    {
        UART_flushTxFifo(DRV_uart_s.handle);
        UART_Transaction_init(&DRV_uart_s.transaction);

        OSAL_MEMORY_memset(DRV_uart_s.aOutStream, 0, sizeof(DRV_uart_s.aOutStream));
        (void)vsnprintf(DRV_uart_s.aOutStream, sizeof(DRV_uart_s.aOutStream), pFormat, argptr);

        DRV_uart_s.transaction.count = strlen(DRV_uart_s.aOutStream);
        DRV_uart_s.transaction.buf = (void *) DRV_uart_s.aOutStream;
        DRV_uart_s.transaction.args = NULL;

        transferOK = UART_write(DRV_uart_s.handle, &DRV_uart_s.transaction);

        DRV_COMMON_Mutex_Unlock(DRV_COMMON_MUTEX_UART);
    }
    else
    {
        transferOK = SystemP_FAILURE;
    }

    (void)transferOK;
}

/*!
*
*  \brief
*  DebugLog printf output function.
*
*  \details
*  Printing of specific string to CCS console output.
*
*  \param[in]  pContext      Call context
*  \param[in]  pFormat       Format string.
*  \param[in]  argptr        Parameter list.
*
*
*/
void DRV_UART_LOG_printf(void* pContext, const char* pFormat, va_list argptr)
{
    OSALUNREF_PARM(pContext);

    OSAL_MEMORY_memset(DRV_uart_s.aOutStream, 0, sizeof(DRV_uart_s.aOutStream));
    (void)vsnprintf(DRV_uart_s.aOutStream, sizeof(DRV_uart_s.aOutStream), pFormat, argptr);

    DebugP_log(DRV_uart_s.aOutStream);
}
