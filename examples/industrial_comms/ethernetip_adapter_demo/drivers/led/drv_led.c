/*!
 *  \file drv_led.c
 *
 *  \brief
 *  LED's application driver.
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

#include <stdbool.h>
#include <stdint.h>

#include "ti_board_open_close.h"

#include "board/led.h"

#include "osal.h"

#include "drivers/common/drv_common.h"
#include "drivers/led/drv_led.h"

typedef struct DRV_LED_Industrial
{
    LED_Handle handle;
    uint32_t   instance;
    uint32_t   value;
}DRV_LED_Industrial_t;

static DRV_LED_Industrial_t DRV_LED_industrial_s;

/*!
*
*  \brief
*  Provides specific LED handle defined by instance.
*
*  \param[in]  instance       SysConfig ID of LED
*
*  \return     requested LED handler
*
*  \retval     NULL           Failed.
*  \retval     Other          Success.
*
*/
LED_Handle DRV_LED_getHandle(uint32_t instanceId)
{
    LED_Handle handle = NULL;

#if (defined CONFIG_LED_NUM_INSTANCES) && (CONFIG_LED_NUM_INSTANCES > 0)
    if (CONFIG_LED_NUM_INSTANCES > instanceId)
    {
        handle = gLedHandle[instanceId];
    }
#else
    OSALUNREF_PARM(instanceId);
#endif

    return handle;
}

/*!
*
*  \brief
*  Initialize application LED's driver.
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
uint32_t DRV_LED_init(DRV_LED_SInit_t* pParams)
{
    uint32_t   result = (uint32_t) OSAL_GENERAL_ERROR;
    DRV_COMMON_Mutex_EError_t mutexErr;

#if !(defined FBTLPROVIDER)
    LED_Attrs* pAttrs = NULL;
    int32_t    status = SystemP_FAILURE;

    // Initialize industrial LED's object
    DRV_LED_industrial_s.instance = pParams->instance;
    DRV_LED_industrial_s.handle   = DRV_LED_getHandle(DRV_LED_industrial_s.instance);
    DRV_LED_industrial_s.value    = 0;

    if(NULL == DRV_LED_industrial_s.handle)
    {
        OSAL_printf("No led handle available.\n");

        result = (uint32_t) OSAL_LED_DRV_HANDLE_INVALID;
        goto laError;
    }
    else
    {
        pAttrs = (LED_Attrs*) LED_getAttrs(DRV_LED_industrial_s.instance);
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
                mutexErr = DRV_COMMON_Mutex_Lock(DRV_COMMON_MUTEX_I2C, 10);
                if(DRV_COMMON_MUTEX_eERR_NOERROR == mutexErr)
                {
                    status = LED_setMask(DRV_LED_industrial_s.handle, DRV_LED_industrial_s.value);

                    DRV_COMMON_Mutex_Unlock(DRV_COMMON_MUTEX_I2C);

                    if(SystemP_SUCCESS != status)
                    {
                        OSAL_printf("Can not set LED Mask.\n");

                        result = (uint32_t) OSAL_LED_DRV_SETMASK;
                        goto laError;
                    }
                }
            }
        }
    }
#endif

    result = OSAL_NO_ERROR;
    goto laError;

laError:
    return result;
}

/*!
*
*  \brief
*  De-initialize application LED's driver.
*
*  \return     error code as uint32_t
*
*  \retval     #OSAL_NO_ERROR               Success.
*  \retval     #OSAL_GENERAL_ERROR          Negative default value.
*  \retval     #OSAL_LED_DRV_HANDLE_INVALID LED handle is NULL.
*  \retval     #OSAL_LED_DRV_SETMASK        LED_setMask call failed.
*
*/
uint32_t DRV_LED_deInit(void)
{
    uint32_t   result = (uint32_t) OSAL_GENERAL_ERROR;
    DRV_COMMON_Mutex_EError_t mutexErr;

#if !(defined FBTLPROVIDER)
    int32_t    status = SystemP_FAILURE;

    // Get LED handle from sys config
    if(NULL == DRV_LED_industrial_s.handle)
    {
        OSAL_printf("No led handle available.\n");

        result = (uint32_t) OSAL_LED_DRV_HANDLE_INVALID;
        goto laError;
    }
    mutexErr = DRV_COMMON_Mutex_Lock(DRV_COMMON_MUTEX_I2C, 10);
    if(DRV_COMMON_MUTEX_eERR_NOERROR == mutexErr)
    {
        status = LED_setMask(DRV_LED_industrial_s.handle, 0x0U);

        DRV_COMMON_Mutex_Unlock(DRV_COMMON_MUTEX_I2C);

        if (SystemP_FAILURE == status)
        {
            OSAL_printf("Can not set LED Mask.\n");

            result = (uint32_t) OSAL_LED_DRV_SETMASK;
            goto laError;
        }


    }
#endif

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
void DRV_LED_industrialSet (uint32_t value)
{
#if !(defined FBTLPROVIDER)
    LED_Attrs* pAttrs = NULL;
    int32_t    status;
    uint32_t   ledGroupMask;
    DRV_COMMON_Mutex_EError_t mutexErr;

    if( DRV_LED_industrial_s.value == value)
    {
        return;
    }

    if (NULL == DRV_LED_industrial_s.handle)
    {
        return;
    }

    pAttrs = (LED_Attrs*) LED_getAttrs(DRV_LED_industrial_s.instance);

    if (NULL == pAttrs)
    {
        return;
    }

    DRV_LED_industrial_s.value = value;
    ledGroupMask = (1 << pAttrs->numLedPerGroup) - 1;
    mutexErr = DRV_COMMON_Mutex_Lock(DRV_COMMON_MUTEX_I2C, 1);
    if(DRV_COMMON_MUTEX_eERR_NOERROR == mutexErr)
    {
        status = LED_setMask(DRV_LED_industrial_s.handle, DRV_LED_industrial_s.value & ledGroupMask);

        DRV_COMMON_Mutex_Unlock(DRV_COMMON_MUTEX_I2C);

        if(SystemP_SUCCESS != status)
        {
            OSAL_printf("Can not set LED Mask.\n");
        }
    }

#endif

    return;
}
