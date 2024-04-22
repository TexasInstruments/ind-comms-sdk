/*!
 *  \file appUart.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application, UART application.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2024, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2024 None.
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

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "osal_error.h"

#include "drivers/uart/v0/uart.h"

#include "ti_drivers_config.h"

#include "appUart.h"

extern UART_Handle gUartHandle[CONFIG_UART_NUM_INSTANCES];

typedef struct EI_APP_Uart
{
    UART_Handle      handle;
    uint32_t         instance;
    char             aOutStream[0x200];
    UART_Transaction transaction;
}EI_APP_Uart_t;

static EI_APP_Uart_t EI_APP_uart_s = {0};

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
UART_Handle EI_APP_UART_getHandle(uint32_t instanceId)
{
    UART_Handle handle = NULL;

    if (CONFIG_UART_NUM_INSTANCES > instanceId)
    {
        handle = gUartHandle[instanceId];
    }

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
uint32_t EI_APP_UART_init(const EI_APP_UART_SInit_t* pParams)
{
    uint32_t result = OSAL_GENERAL_ERROR;

    EI_APP_uart_s.instance = pParams->uartInst;
    EI_APP_uart_s.handle   = EI_APP_UART_getHandle(EI_APP_uart_s.instance);

    if(NULL == EI_APP_uart_s.handle)
    {
        OSAL_printf("No UART handle available.\n");

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
uint32_t EI_APP_UART_deInit(void)
{
    uint32_t result = OSAL_GENERAL_ERROR;

    if (NULL != EI_APP_uart_s.handle)
    {
        UART_flushTxFifo(EI_APP_uart_s.handle);
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
void EI_APP_UART_printf(void* pContext, const char* pFormat, va_list argptr)
{
    /* @cppcheck_justify{unusedVariable} false-positive: variable is used */
    //cppcheck-suppress unusedVariable
    int32_t transferOK;
    /* @cppcheck_justify{unusedVariable} false-positive: variable is used */
    //cppcheck-suppress unusedVariable

    OSALUNREF_PARM(pContext);

    UART_flushTxFifo(EI_APP_uart_s.handle);
    UART_Transaction_init(&EI_APP_uart_s.transaction);

    OSAL_MEMORY_memset(EI_APP_uart_s.aOutStream, 0, sizeof(EI_APP_uart_s.aOutStream));
    (void)vsnprintf(EI_APP_uart_s.aOutStream, sizeof(EI_APP_uart_s.aOutStream), pFormat, argptr);

    EI_APP_uart_s.transaction.count = strlen(EI_APP_uart_s.aOutStream);
    EI_APP_uart_s.transaction.buf = (void *) EI_APP_uart_s.aOutStream;
    EI_APP_uart_s.transaction.args = NULL;

    transferOK = UART_write(EI_APP_uart_s.handle, &EI_APP_uart_s.transaction);

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
void EI_APP_UART_LOG_printf(void* pContext, const char* pFormat, va_list argptr)
{
    OSALUNREF_PARM(pContext);

    OSAL_MEMORY_memset(EI_APP_uart_s.aOutStream, 0, sizeof(EI_APP_uart_s.aOutStream));
    (void)vsnprintf(EI_APP_uart_s.aOutStream, sizeof(EI_APP_uart_s.aOutStream), pFormat, argptr);

    DebugP_log(EI_APP_uart_s.aOutStream);
}
