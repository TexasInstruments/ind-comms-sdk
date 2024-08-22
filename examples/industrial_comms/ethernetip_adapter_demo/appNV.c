/*!
 *  \file appNV.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application, access to non volatile data storage.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br><br>
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

#include <stdbool.h>
#include <string.h>

#include <portmacro.h>
#include <FreeRTOSConfig.h>
#include <ti_board_open_close.h>

#include "EI_API.h"
#include "EI_API_def.h"

#include "drivers/CUST_drivers.h"

#include "appCipCodes.h"
#include "board.h"
#include "appTask.h"
#include "appNV.h"
#include "appCfg.h"
#include "appRst.h"

#include "appNvFlash.h"
#include "appNvEeprom.h"

/*!
 *
 *  \brief
 *  Initialize handle to non-volatile memory.
 *
 *  \param[in]  pAdapter       Pointer to EtherNet/IP adapter object.
 *  \param[in]  pParams        Pointer to non-volatile memory parameters.
 *
 *  \return     bool           Initialization result.
 *  \retval     false          Failed.
 *  \retval     true           Success.
 *
 */
uint32_t EI_APP_NV_init(EI_API_ADP_T* pAdapter, EI_APP_NV_SInit_t *pParams)
{
    OSALUNREF_PARM(pAdapter);

    uint32_t err = OSAL_GENERAL_ERROR;

    err = EI_APP_NV_EEPROM_init(pParams->taskPrio);

    if (OSAL_NO_ERROR != err)
    {
        goto laError;
    }

    err = EI_APP_NV_FLASH_init(pParams->taskPrio);

laError:

    return err;
}

/*!
*
*  \brief
*  Provides FLASH object de-initialization.
*
*  \return     uint32_t          Error code
*
*  \retval     OSAL_NO_ERROR     Success.
*
*/
uint32_t EI_APP_NV_deinit(void)
{
    uint32_t err = OSAL_GENERAL_ERROR;

    err = EI_APP_NV_EEPROM_deinit();

    if (OSAL_NO_ERROR != err)
    {
        goto laError;
    }

    err = EI_APP_NV_FLASH_deinit();

laError:

    return err;
}

/*!
*
* \brief
* Provides handler to driver of non-volatile memory.
*
*  \param[in]  type            Defines memory type (FLASH, EEPROM, ...).
*  \param[in]  instance        Instance number of non-volatile memory.
*
*  \return     void*           Handler to non-volatile memory.
*  \retval     NULL            Failed.
*  \retval     Other than NULL Success.
*
*/
void* EI_APP_NV_getHandle (uint32_t type, uint32_t instance)
{
    void* handle = NULL;

    switch(type)
    {
        case EI_APP_NV_eTYPE_FLASH:
            handle = EI_APP_NV_FLASH_getHandle(instance);
            break;
        case EI_APP_NV_eTYPE_EEPROM:
            handle = EI_APP_NV_EEPROM_getHandle(instance);
            break;
        default:
            handle = NULL;
            break;
    }

    return handle;
}

/*!
*
* \brief
* Reads non-volatile configuration data.
*
*  \param[in]  type            Non-volatile data memory type.
*  \param[in]  instance        Instance ID defined by SysConfig.
*  \param[in]  offset_p        Non-volatile data memory offset where data need to be written.
*  \param[in]  pBuf_p          Pointer to read buffer.
*  \param[in]  length_p        Length of data to be read.
*
*  \return     uint32_t                                 Error code.
*
*  \retval     OSAL_NO_ERROR                            Success.
*  \retval     OSAL_GENERAL_ERROR                       Negative default value.
*  \retval     OSAL_FLASH_DRV_HANDLE_INVALID            FLASH handle set to NULL.
*  \retval     OSAL_FLASH_DRV_READ                      Flash_read call failed.
*  \retval     OSAL_EE_DRV_HANDLE_INVALID               EEPROM handle set to NULL.
*  \retval     OSAL_EE_DRV_READ                         EEPROM_read call failed.
*
*
*/
uint32_t EI_APP_NV_read (uint32_t type, uint32_t instance, uint32_t offset, const uint8_t* pBuf, uint32_t length)
{
    void*    handle = NULL;
    uint32_t err    = OSAL_GENERAL_ERROR;

    switch(type)
    {
        case EI_APP_NV_eTYPE_FLASH:
        {
            handle = EI_APP_NV_getHandle(type, instance);

            err = EI_APP_NV_FLASH_read ((Flash_Handle) handle, offset, (uint8_t*) pBuf, length);

            break;
        }
        case EI_APP_NV_eTYPE_EEPROM:
        {
            handle = EI_APP_NV_getHandle(type, instance);

            err = EI_APP_NV_EEPROM_read ((EEPROM_Handle) handle, offset, (uint8_t*) pBuf, length);

            break;
        }
        default:
            break;
    }

    return err;
}

/*!
* <!-- Description: -->
*
* \brief
* Writes non-volatile configuration data.
*
*  <!-- Parameters and return values: -->
*
*  \param[in]  type            Non-volatile data memory type.
*  \param[in]  instance        Instance ID defined by SysConfig.
*  \param[in]  offset          Non-volatile data memory offset where data need to be written.
*  \param[in]  pBuf            Pointer to write buffer.
*  \param[in]  length          Length of data to be read.
*  \param[in]  blocking        Waits until operation is finished.
*
*  \return     uint32_t                             Error code.
*
*  \retval     OSAL_NO_ERROR                        Success.
*  \retval     OSAL_GENERAL_ERROR                   Failed.
*  \retval     OSAL_FLASH_DRV_HANDLE_INVALID        Flash driver handler set to NULL.
*  \retval     OSAL_FLASH_DRV_DATA_INVALID          Flash write data buffer allocation failed or input buffer set to NULL.
*  \retval     OSAL_FLASH_DRV_LENGTH_INVALID        Flash data have 0 length
*  \retval     OSAL_EE_DRV_HANDLE_INVALID           EEPROM driver handler set to NULL.
*  \retval     OSAL_EE_DRV_DATA_INVALID             EEPROM write data buffer allocation failed or input buffer set to NULL.
*  \retval     OSAL_EE_DRV_LENGTH_INVALID           EEPROM data have 0 length
*  \retval     EI_APP_NV_NO_MEMORY_DEFINED          No non-volatile memory defined as storage for configuration
*
*/
uint32_t EI_APP_NV_write (uint32_t       type,
                          uint32_t       instance,
                          uint32_t       offset,
                          const uint8_t* pBuf,
                          uint32_t       length,
                          bool           blocking)
{
    void*    handle = NULL;
    uint32_t err    = OSAL_GENERAL_ERROR;

    switch(type)
    {
        case EI_APP_NV_eTYPE_FLASH:
        {
            handle = EI_APP_NV_getHandle(EI_APP_NV_eTYPE_FLASH, instance);

            err = EI_APP_NV_FLASH_write((Flash_Handle) handle, offset, pBuf, length, blocking);
            break;
        }
        case EI_APP_NV_eTYPE_EEPROM:
        {
            handle = EI_APP_NV_getHandle(EI_APP_NV_eTYPE_EEPROM, instance);

            err = EI_APP_NV_EEPROM_write((EEPROM_Handle) handle, offset, pBuf, length, blocking);
            break;
        }
        default:
            err = EI_APP_NV_NO_MEMORY_DEFINED;
            break;
    }

    return err;
}

/*!
*
* \brief
* Provides non-volatile configuration data write operation status.
*
*  \return     bool        Status of non-volatile configuration data write operation.
*
*  \retval     false       Not pending.
*  \retval     true        Pending.
*
*/
bool EI_APP_NV_isWritePending (void)
{
    bool ret = false;

    if (true ==  EI_APP_NV_EEPROM_isWritePending())
    {
        ret = true;
    }

    if (true == EI_APP_NV_FLASH_isWritePending())
    {
        ret = true;
    }

    return ret;
}
