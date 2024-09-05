/*!
 *  \file CUST_drivers.c
 *
 *  \brief
 *  Provides initialization of custom drivers.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 None.
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

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#include <drivers/CUST_drivers.h>

/*!
* <!-- Description: -->
*
* \brief
* Custom drivers initialization.
*
*  \return     uint32_t                      Error code.
*
*  \retval     CUST_DRIVERS_eERR_NOERROR     Success.
*  \retval     CUST_DRIVERS_eERR_PHY         PHY driver initialization failed.
*  \retval     CUST_DRIVERS_eERR_UART        UART driver initialization failed.
*  \retval     CUST_DRIVERS_eERR_LED         LED driver initialization failed.
*  \retval     CUST_DRIVERS_eERR_FLASH       FLASH driver initialization failed.
*
*/
uint32_t CUST_DRIVERS_init(CUST_DRIVERS_SInit_t* pParams_p)
{
    CUST_ETHPHY_SParams_t        ethPhyParams;
    CUST_PRUICSS_InitParams_t    pruIcssParams;

    uint32_t                     error = (uint32_t) CUST_DRIVERS_eERR_NOERROR;

    Drivers_open();

    ethPhyParams.pruIcssSysConfigId  = pParams_p->pruIcss.instance;
    ethPhyParams.ethPhySysConfigId_0 = pParams_p->pruIcss.ethPhy.instance_0;
    ethPhyParams.ethPhySysConfigId_1 = pParams_p->pruIcss.ethPhy.instance_1;

    if (CUST_ETHPHY_eERR_NOERROR != CUST_ETHPHY_init(&ethPhyParams))
    {
        error = (uint32_t) CUST_DRIVERS_eERR_ETHPHY;
        goto initErr;
    }

    if (SystemP_SUCCESS != Board_driversOpen())
    {
        error = (uint32_t) CUST_DRIVERS_eERR_GENERALERROR;
        goto initErr;
    }

    pruIcssParams.pruIcssId = pParams_p->pruIcss.instance;
    pruIcssParams.ethPhy0Id = pParams_p->pruIcss.ethPhy.instance_0;
    pruIcssParams.ethPhy1Id = pParams_p->pruIcss.ethPhy.instance_1;

    if (CUST_PRUICSS_eERR_NOERROR != CUST_PRUICSS_init(&pruIcssParams))
    {
        error = (uint32_t) CUST_DRIVERS_eERR_PRUICSS;
        goto initErr;
    }

#ifndef ENABLE_INTERCORE_TUNNELING
    if (CUST_LED_eERR_NOERROR != CUST_LED_init())
    {
        error = (uint32_t) CUST_DRIVERS_eERR_LED;
        goto initErr;
    }
#endif

    if (CUST_FLASH_eERR_NOERROR != CUST_FLASH_init())
    {
        error = (uint32_t) CUST_DRIVERS_eERR_FLASH;
        goto initErr;
    }

    if (CUST_EEPROM_eERR_NOERROR != CUST_EEPROM_init())
    {
        error = (uint32_t) CUST_DRIVERS_eERR_EEPROM;
        goto initErr;
    }

initErr:
    return error;
}

/*!
* <!-- Description: -->
*
* \brief
* Custom drivers deinitialization.
*
*  \return     uint32_t                        Error code.
*
*  \retval     CUST_DRIVERS_eERR_NOERROR       Success.
*  \retval     CUST_DRIVERS_eERR_PHY           PHY driver deinitialization failed.
*  \retval     CUST_DRIVERS_eERR_UART          UART driver deinitialization failed.
*  \retval     CUST_DRIVERS_eERR_LED           LED driver deinitialization failed.
*  \retval     CUST_DRIVERS_eERR_FLASH         FLASH driver deinitialization failed.
*
*/
uint32_t CUST_DRIVERS_deinit(void)
{
    uint32_t error = (uint32_t) CUST_DRIVERS_eERR_NOERROR;

    if (CUST_LED_eERR_NOERROR != CUST_LED_deInit())
    {
        error = (uint32_t) CUST_DRIVERS_eERR_LED;
        goto deinitErr;
    }

    if (CUST_FLASH_eERR_NOERROR != CUST_FLASH_deInit())
    {
        error = (uint32_t) CUST_DRIVERS_eERR_FLASH;
        goto deinitErr;
    }

    if (CUST_ETHPHY_eERR_NOERROR != CUST_ETHPHY_deInit())
    {
        error = (uint32_t) CUST_DRIVERS_eERR_ETHPHY;
        goto deinitErr;
    }

    Board_driversClose();

    Drivers_close();

deinitErr:
    return error;
}
