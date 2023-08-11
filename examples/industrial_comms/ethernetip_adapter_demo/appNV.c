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
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
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

#include "api/EI_API.h"
#include "api/EI_API_def.h"
#include "drivers/CUST_drivers.h"

#include "appCipCodes.h"
#include "board.h"
#include "appTask.h"
#include "appCfg.h"
#include "appRst.h"
#include "appNV.h"

static uint8_t* EI_APP_NV_pData_s  = NULL;
static uint32_t EI_APP_NV_length_s = 0;

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Initialize handle to permanent data in Flash memory.
 *
 *  \details
 *  Initialize handle to permanent data in Flash memory.
 *
 */
bool EI_APP_NV_init(EI_API_ADP_T *pAdapter)
{
    bool ret = false;

    EI_APP_NV_pData_s  = (uint8_t*) EI_APP_CFG_getRuntimeData();
    EI_APP_NV_length_s = EI_APP_CFG_getLength();

    if ( (NULL != EI_APP_NV_pData_s) &&
         (0    != EI_APP_NV_length_s) )
    {
        ret = true;
    }

    return ret;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Write the permanent data.
 *
 *  \details
 *  Write the permanent data to memory. Operation can be blocking or non-blocking.
 *
 */
bool EI_APP_NV_write(bool blocking)
{
    EI_APP_CFG_Data_t *pCfgData = (EI_APP_CFG_Data_t*) EI_APP_NV_pData_s;
    void*              nvHandle = NULL;
    uint32_t           err      = CUST_DRIVERS_eERR_NOERROR;

    CUST_DRIVERS_PRM_EType_t memType = PERMANENT_DATA_MEMORY_TYPE;

    if (memType == CUST_DRIVERS_PRM_eTYPE_UNDEFINED)
    {
        return false;
    }

    EI_APP_CFG_setHeader(&pCfgData->header);

    nvHandle = CUST_DRIVERS_PRM_getHandle(PERMANENT_DATA_MEMORY_TYPE, PERMANENT_DATA_MEMORY_INSTANCE);

    err = CUST_DRIVERS_PRM_write (nvHandle,
                                  PERMANENT_DATA_MEMORY_TYPE,
                                  PERMANENT_DATA_MEMORY_OFFSET,
                                  EI_APP_NV_pData_s,
                                  EI_APP_NV_length_s,
                                  blocking);

    if (CUST_DRIVERS_eERR_NOERROR != err)
    {
        return false;
    }

    return true;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Reads the permanent data.
 *
 *  \details
 *  Reads the permanent data. If no data are present,
 *  a new structure is created with default data.
 *
 */
bool EI_APP_NV_read(void)
{
    CUST_DRIVERS_PRM_EType_t   memType      = PERMANENT_DATA_MEMORY_TYPE;
    void*                      nvHandle     = NULL;
    uint32_t                   err          = (uint32_t) CUST_DRIVERS_eERR_GENERALERROR;
    bool                       ret          = false;

    if ( (memType == CUST_DRIVERS_PRM_eTYPE_FLASH) ||
         (memType == CUST_DRIVERS_PRM_eTYPE_EEPROM) )
    {
        nvHandle = CUST_DRIVERS_PRM_getHandle(PERMANENT_DATA_MEMORY_TYPE, PERMANENT_DATA_MEMORY_INSTANCE);

        err = CUST_DRIVERS_PRM_read (nvHandle,
                                     PERMANENT_DATA_MEMORY_TYPE,
                                     PERMANENT_DATA_MEMORY_OFFSET,
                                     EI_APP_NV_pData_s,
                                     EI_APP_NV_length_s);
    }
    else
    {
        err = CUST_DRIVERS_eERR_NO_PERMANENT_STORAGE;
    }

    if ( (CUST_DRIVERS_eERR_NOERROR              != err) &&
         (CUST_DRIVERS_eERR_NO_PERMANENT_STORAGE != err) )
    {
        OSAL_printf ("\r\nNon-Volatile data read failed");
        goto laError;
    }

    ret = EI_APP_CFG_isValid();

    if (false == ret)
    {
        OSAL_printf ("The data is corrupted, write default values.\r\n");
        EI_APP_RST_execute(1);
    }

    return ret;

    //-------------------------------------------------------------------------------------------------
    laError:

        exit (-1);
}

