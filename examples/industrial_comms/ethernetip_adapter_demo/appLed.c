/*!
 *  \file appLed.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application, access to LED's.
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

#include <stdbool.h>
#include <stdint.h>

#include "osal_error.h"

#include "ti_board_open_close.h"
#include "board/led.h"

#include "drivers/led/CUST_led.h"
#include "appLed.h"

typedef struct EI_APP_Industrial
{
    LED_Handle handle;
    uint32_t   instance;
    uint32_t   value;
}EI_APP_Industrial_t;

static EI_APP_Industrial_t EI_APP_LED_industrial_s;

/*!
*
*  \brief
*  Initialize application LED's.
*
*  \param[in]  LED initialization parameters.
*
*  \return     error code as uint32_t
*
*  \retval     #OSAL_NO_ERROR               Success.
*  \retval     #OSAL_GENERAL_ERROR          Negative default value.
*  \retval     #OSAL_LED_DRV_HANDLE_INVALID LED handle is NULL.
*  \retval     #OSAL_LED_DRV_GETATTR        LED_getAttrs call returns NULL.
*  \retval     #OSAL_LED_DRV_SETMASK        LED_setMask call failed.
*
*/
uint32_t EI_APP_LED_init(EI_APP_LED_SInit_t* pParams)
{
    uint32_t   result = (uint32_t) OSAL_GENERAL_ERROR;

    LED_Attrs* pAttrs = NULL;
    int32_t    status = SystemP_FAILURE;

    // Initialize industrial LED's object
    EI_APP_LED_industrial_s.instance = pParams->industrialLedsInst;
    EI_APP_LED_industrial_s.handle   = CUST_LED_getHandle(EI_APP_LED_industrial_s.instance);
    EI_APP_LED_industrial_s.value    = 0;

    if(NULL == EI_APP_LED_industrial_s.handle)
    {
        OSAL_printf("No led handle available.\n");

        result = (uint32_t) OSAL_LED_DRV_HANDLE_INVALID;
        goto laError;
    }
    else
    {
        pAttrs = (LED_Attrs*) LED_getAttrs(EI_APP_LED_industrial_s.instance);
        if(NULL == pAttrs)
        {
            OSAL_printf("Can not get LED attributes.\n");

            result = (uint32_t) OSAL_LED_DRV_GETATTR;
            goto laError;
        }
        else
        {
            if(pAttrs->numLedPerGroup > 1U)
            {
                status = LED_setMask(EI_APP_LED_industrial_s.handle, EI_APP_LED_industrial_s.value);

                if(SystemP_SUCCESS != status)
                {
                    OSAL_printf("Can not set LED Mask.\n");

                    result = (uint32_t) OSAL_LED_DRV_SETMASK;
                    goto laError;
                }
            }
        }
    }

    result = OSAL_NO_ERROR;
    goto laError;

laError:
    return result;
}

/*!
*
*  \brief
*  De-initialize application LED's.
*
*  \return     error code as uint32_t
*
*  \retval     #OSAL_NO_ERROR               Success.
*  \retval     #OSAL_GENERAL_ERROR          Negative default value.
*  \retval     #OSAL_LED_DRV_HANDLE_INVALID LED handle is NULL.
*  \retval     #OSAL_LED_DRV_SETMASK        LED_setMask call failed.
*
*/
uint32_t EI_APP_LED_deInit(void)
{
    uint32_t   result = (uint32_t) OSAL_GENERAL_ERROR;

    int32_t    status = SystemP_FAILURE;

    // Get LED handle from sys config
    if(NULL == EI_APP_LED_industrial_s.handle)
    {
        OSAL_printf("No led handle available.\n");

        result = (uint32_t) OSAL_LED_DRV_HANDLE_INVALID;
        goto laError;
    }

    status = LED_setMask(EI_APP_LED_industrial_s.handle, 0x0U);

    if (SystemP_FAILURE == status)
    {
        OSAL_printf("Can not set LED Mask.\n");

        result = (uint32_t) OSAL_LED_DRV_SETMASK;
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
*  Set industrial LEDs controlled by TPIC2810
*
*  \param[in]  pattern             LED pattern.
*
*/
void EI_APP_LED_industrialSet (uint32_t value)
{
    LED_Attrs* pAttrs = NULL;
    int32_t    status;
    uint32_t   ledGroupMask;

    if( EI_APP_LED_industrial_s.value == value)
    {
        return;
    }

    if (NULL == EI_APP_LED_industrial_s.handle)
    {
        return;
    }

    pAttrs = (LED_Attrs*) LED_getAttrs(EI_APP_LED_industrial_s.instance);

    if (NULL == pAttrs)
    {
        return;
    }

    EI_APP_LED_industrial_s.value = value;
    ledGroupMask = (1 << pAttrs->numLedPerGroup) - 1;

    status = LED_setMask(EI_APP_LED_industrial_s.handle, EI_APP_LED_industrial_s.value & ledGroupMask);

    if(SystemP_SUCCESS != status)
    {
        OSAL_printf("Can not set LED Mask.\n");
    }

    return;
}
