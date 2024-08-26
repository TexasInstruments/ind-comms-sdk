/*!
 * \file pn_app_iod_alarm.c
 *
 * \brief
 * Functions and callbacks for the handling of IO device's alarms.
 *
 * \author
 * KUNBUS GmbH
 *
 * \copyright
 * Copyright (c) 2023, KUNBUS GmbH<br /><br />
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024 KUNBUS GmbH.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * <ol>
 * <li>Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer./<li>
 * <li>Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.</li>
 * <li>Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.</li>
 * </ol>
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "pn_app_iod_alarm.h"

#include <osal.h>

#include "pn_api_iod_callbacks.h"
#include "pn_api_iod_error.h"

void PN_APP_IOD_cbAsyncReqDone(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t                   arNum,
    PN_API_IOD_AlarmType_t     alarmType,
    uint32_t                   api,
    PN_API_IOD_DevAddr_t      *addr,
    uint32_t                   status,
    uint16_t                   diagnosisTag)
{
    OSALUNREF_PARM(pnHandle);

    if(NULL != addr)
    {
        if ((0 < arNum) && (PN_API_OK == status))
        {
            OSAL_printf("Asynchronous request sent: ");
        }
        else
        {
            OSAL_printf("Asynchronous request failed: ");
        }

        OSAL_printf(
            "AR = %d, alarm type = %d, API = %d, slot = %d, subslot = %d, user tag: %x\r\n",
            arNum,
            alarmType,
            api,
            addr->address.geo.slot,
            addr->address.geo.subslot,
            diagnosisTag);
    }
}

void PN_APP_IOD_cbDevAlarm(PN_API_IOD_Handle_t *const pnHandle, const PN_API_IOD_DevAlarmData_t *devAlarmData)
{
    OSALUNREF_PARM(pnHandle);

    char* alarmData = NULL;

    OSAL_printf(
        "Alarm received from IO controller: AR = %d, session key = %d, API = %d, slot = %d, subslot = %d, \
        module = %d, submodule = %d, priority = %d, type = %d, sequence = %d",
        devAlarmData->arNum,
        devAlarmData->sessionKey,
        devAlarmData->api,
        devAlarmData->slot,
        devAlarmData->subslot,
        devAlarmData->modId,
        devAlarmData->submodId,
        devAlarmData->alarmPriority,
        devAlarmData->alarmType,
        devAlarmData->alarmSequence);

    if (0 == devAlarmData->userAlarmDataLen)
    {
        OSAL_printf(".\r\n");
    }
    else
    {
        alarmData = (char *)devAlarmData->userAlarmData;
        OSAL_printf(", alarm data = %s\r\n", alarmData);
    }
}
