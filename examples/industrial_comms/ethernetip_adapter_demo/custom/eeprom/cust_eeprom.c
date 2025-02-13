/*!
 *  \file cust_eeprom.c
 *
 *  \brief
 *  Provides initialization of custom EEPROM's.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2022 Texas Instruments Incorporated
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

#include "ti_board_config.h"

#include "eeprom/cust_eeprom.h"


#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
extern EEPROM_Handle gEepromHandle[CONFIG_EEPROM_NUM_INSTANCES];
#endif

/*!
* <!-- Description: -->
*
* \brief
* Custom EEPROM initialization.
*
* \return     uint32_t                       Error code.
*
* \retval     CUST_EEPROM_eERR_NOERROR       Success.
* \retval     CUST_EEPROM_eERR_GENERALERROR  Failed.
*
*/
uint32_t CUST_EEPROM_init (void)
{
    // Custom EEPROM is currently not supported by SysConfig

    return (uint32_t) CUST_EEPROM_eERR_NOERROR;
}

/*!
* <!-- Description: -->
*
* \brief
* Custom EEPROM deinitialization.
*
* \return     uint32_t                    Error code.
*
* \retval     CUST_EEPROM_eERR_NOERROR       Success.
* \retval     CUST_EEPROM_eERR_GENERALERROR  Failed.
*
*/
uint32_t CUST_EEPROM_deInit(void)
{
    // Custom EEPROM is currently not supported by SysConfig

    return (uint32_t) CUST_EEPROM_eERR_NOERROR;
}

/*!
*
*  \brief
*  Provides specific EEPROM handle defined by instance.
*
*  \param[in]  instance       SysConfig ID of EEPROM
*
*  \return     requested EEPROM handler
*
*  \retval     NULL           Failed.
*  \retval     Other          Success.
*
*/
EEPROM_Handle CUST_EEPROM_getHandle(uint32_t instanceId)
{
    EEPROM_Handle handle = NULL;

#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
    if (CONFIG_EEPROM_NUM_INSTANCES > instanceId)
    {
        handle = gEepromHandle[instanceId];
    }
#else
    OSALUNREF_PARM(instanceId);
#endif

    return handle;
}
