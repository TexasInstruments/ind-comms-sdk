/*!
 *  \file pn_app_iod_alarm.h
 *
 *  \brief
 *  Functions and callbacks for the handling of IO device's alarms.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

#ifndef PN_APP_IOD_ALARM_H
#define PN_APP_IOD_ALARM_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*!
 * \brief
 *  Callback function to acknowledge an asynchronous request (alarm) issued
 *  previously from the application to Profinet stack.
 *
 * \details
 * This function simply informs the user about the reception of the acknowledgment.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       arNum               AR number.
 * \param[in]       alarmType           Alarm type (see \ref PN_API_IOD_AlarmType_t).
 * \param[in]       api                 Application Process Identifier.
 * \param[in]       addr                Geographical address (slot/subslot) of the submodule.
 * \param[in]       status              #PN_API_OK or #PN_API_NOT_OK.
 * \param[in]       diagnosisTag        Diagnosis tag which distinguishes different alarms
 *                                      on one subslot (can not be 0).
 *
 * \ingroup PN_APP_IOD_ALARM_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_asyncReqDoneCbf
 */
void PN_APP_IOD_cbAsyncReqDone(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t                   arNum,
    PN_API_IOD_AlarmType_t     alarmType,
    uint32_t                   api,
    PN_API_IOD_DevAddr_t      *addr,
    uint32_t                   status,
    uint16_t                   diagnosisTag);

/*!
 * \brief
 * Callback function to notify an alarm that has been received from the IO controller.
 *
 * \details
 * This function simply informs the user about the received alarm.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       devAlarmData        Pointer to alarm data.
 *
 * \ingroup PN_APP_IOD_ALARM_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_devAlarmCbf
 */
void PN_APP_IOD_cbDevAlarm(
    PN_API_IOD_Handle_t *const       pnHandle,
    const PN_API_IOD_DevAlarmData_t *devAlarmData);

#if(defined __cplusplus)
}
#endif

#endif /* PN_APP_IOD_ALARM_H */
