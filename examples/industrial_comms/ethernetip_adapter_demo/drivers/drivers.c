/*!
 *  \file drivers.c
 *
 *  \brief
 *  Application drivers.
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

#if defined (SOC_AM243X) || defined (SOC_AM64X)
#include "drivers/pruicss/g_v0/pruicss.h"
#elif defined (SOC_AM263PX) || defined (SOC_AM261X)
#include "drivers/pruicss/m_v0/pruicss.h"
#else
#error "Undefined SOC"
#endif

#include "osal_error.h"

#include "cust_drivers.h"

#include "drivers/drivers.h"

/*!
* <!-- Description: -->
*
* \brief
* Custom drivers initialization.
*
*  \return     uint32_t                      Error code.
*
*  \retval     DRIVERS_eERR_NOERROR     Success.
*  \retval     DRIVERS_eERR_PHY         PHY driver initialization failed.
*  \retval     DRIVERS_eERR_UART        UART driver initialization failed.
*  \retval     DRIVERS_eERR_LED         LED driver initialization failed.
*  \retval     DRIVERS_eERR_FLASH       FLASH driver initialization failed.
*
*/
uint32_t APP_DRIVERS_init(APP_DRIVERS_SInit_t* pParams_p)
{
    uint32_t error = (uint32_t) DRIVERS_eERR_NOERROR;

    if (OSAL_NO_ERROR != DRV_EEPROM_init(pParams_p->eeprom.taskPrio))
    {
        error = (uint32_t) DRIVERS_eERR_EEPROM;
        goto initErr;
    }

    if (OSAL_NO_ERROR != DRV_FLASH_init(pParams_p->flash.taskPrio))
    {
        error = (uint32_t) DRIVERS_eERR_EEPROM;
        goto initErr;
    }

    if (OSAL_NO_ERROR != DRV_PRUICSS_init(&pParams_p->pruicss))
    {
        error = (uint32_t) DRIVERS_eERR_PRUICSS;
        goto initErr;
    }

    if (OSAL_NO_ERROR != DRV_LED_init(&pParams_p->led))
    {
        error = (uint32_t) DRIVERS_eERR_LED;
        goto initErr;
    }

    if (OSAL_NO_ERROR != DRV_UART_init(&pParams_p->uart))
    {
        error = (uint32_t) DRIVERS_eERR_UART;
        goto initErr;
    }

initErr:
    return error;
}

/*!
* <!-- Description: -->
*
* \brief
* Application drivers deinitialization.
*
*  \return     uint32_t                   Error code.
*
*  \retval     DRIVERS_eERR_NOERROR       Success.
*  \retval     DRIVERS_eERR_LED           LED driver deinitialization failed.
*  \retval     DRIVERS_eERR_UART          UART driver deinitialization failed.
*
*/
uint32_t APP_DRIVERS_deinit(void)
{
    uint32_t error = (uint32_t) DRIVERS_eERR_NOERROR;

    if (OSAL_NO_ERROR != DRV_EEPROM_deInit())
    {
        error = (uint32_t) DRIVERS_eERR_EEPROM;
        goto deinitErr;
    }

    if (OSAL_NO_ERROR != DRV_FLASH_deInit())
    {
        error = (uint32_t) DRIVERS_eERR_FLASH;
        goto deinitErr;
    }

    if (OSAL_NO_ERROR != DRV_LED_deInit())
    {
        error = (uint32_t) DRIVERS_eERR_LED;
        goto deinitErr;
    }

    if (OSAL_NO_ERROR != DRV_UART_deInit())
    {
        error = (uint32_t) DRIVERS_eERR_UART;
        goto deinitErr;
    }

deinitErr:
    return error;
}
