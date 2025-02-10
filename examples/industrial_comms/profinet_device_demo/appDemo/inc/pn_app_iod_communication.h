/*!
 *  \file pn_app_iod_communication.h
 *
 *  \brief
 *  Functions and callbacks for the handling of ARs (Application Relations).
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

#ifndef PN_APP_IOD_COMMUNICATION_H
#define PN_APP_IOD_COMMUNICATION_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*!
 * \brief
 * Initialize AR information.
 *
 * \details
 * AR info list and the number of running ARs are all set to zero.
 *
 * \ingroup PN_APP_IOD_COMMUNICATION_DOXY_GROUP
 */
void PN_APP_IOD_initArInfo(void);

/*!
 * \brief
 * Callback function to notify that the parameter assignment for all modules has been completed.
 *
 * \details
 * This function confirms the parametrization end operation by setting the status of the
 * corresponding submodule to #PN_API_IOD_SubmodStatusRun and returning it to the stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          Session key.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      slot                Slot number, can not be 0.
 * \param[in]      subslot             Subslot number, which can be:<br>
 *                                     0: paramEnd for all submodules.<br>
 *                                     not 0: paramEnd for the specific module.
 * \param[in]      moreFollows         More calls of this function for additional
 *                                     submodules is to be expected.
 *
 * \return         Submodule status as #PN_API_IOD_SubmodStatus_t.
 * \retval         #PN_API_IOD_SubmodStatusRun                 Success, generate "application ready".
 * \retval         #PN_API_IOD_SubmodStatusAppReadyFollows     Success, but not ready yet.
 * \retval         #PN_API_IOD_SubmodStatusUnknown             Wrong submodule.
 * \retval         #PN_API_IOD_SubmodStatusStop                Something went wrong.
 *
 * \ingroup PN_APP_IOD_COMMUNICATION_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_paramEndCbf
 */
PN_API_IOD_SubmodStatus_t PN_APP_IOD_cbParamEnd(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint16_t             arNum,
    const uint16_t             sessionKey,
    const uint32_t             api,
    const uint16_t             slot,
    const uint16_t             subslot,
    const bool                 moreFollows);

/*!
 * \brief
 * Callback function to notify about an AR FSU update.
 *
 * \details
 * This function simply notifies the user if an AR FSU change has taken place.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arFsuEnabled        #PN_API_IOD_ARFSU_ENABLED or
 *                                     #PN_API_IOD_ARFSU_DISABLED.
 * \param[in]      arFsuChanged        #PN_API_IOD_ARFSU_CHANGED or
 *                                     #PN_API_IOD_ARFSU_NOT_CHANGED.
 *
 * \ingroup PN_APP_IOD_COMMUNICATION_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_reportArFsuRecordCbf
 */
void PN_APP_IOD_cbReportArFSURecord(
    PN_API_IOD_Handle_t* const pnHandle,
    const uint8_t arFsuEnabled,
    const uint8_t arFsuChanged);

/*!
 * \brief
 * Callback function prototype to notify that a new connection to an IO controller has been established.
 *
 * \details
 * Based on the AR type, this function adds the new AR to the application's database
 * and informs the user about the received update.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arType              AR type.
 * \param[in]      arNum               AR number.
 * \param[in]      arSessionKey        Session key.
 * \param[in]      sendClock           Send clock parameter.
 * \param[in]      redRatioIoCrIn      Reduction ratio of input IOCR.
 * \param[in]      redRatioIoCrOut     Reduction ratio of output IOCR.
 * \param[in]      hostIp              IP address of the host (IO controller).
 *
 * \ingroup PN_APP_IOD_COMMUNICATION_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_arConnectCbf
 */
void PN_APP_IOD_cbArConnectInd(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_ArType_t        arType,
    uint32_t                   arNum,
    uint16_t                   arSessionKey,
    uint16_t                   sendClock,
    uint16_t                   redRatioIoCrIn,
    uint16_t                   redRatioIoCrOut,
    uint32_t                   hostIp);

/*!
 * \brief
 * Callback function to notify that an AR connection with the IO controller has been terminated.
 *
 * \details
 * Based on the AR type, this function removes the deactivated AR from the application's database
 * and if it was the last running AR, it deactivates the exchange of cyclic IO data.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      arSessionKey        Session key.
 * \param[in]      reasonCode          Reason code (see \ref PN_API_IOD_ArReason_t)
 *
 * \ingroup PN_APP_IOD_COMMUNICATION_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_arDisconnCbf
 */
void PN_APP_IOD_cbArDisconn(
    PN_API_IOD_Handle_t* const pnHandle,
    const uint16_t arNum,
    const uint16_t arSessionKey,
    const PN_API_IOD_ArReason_t reasonCode);

/*!
 * \brief
 * Callback function to notify that the stack is ready to receive the first valid input data.
 *
 * \details
 * Upon receiving this function from the stack, This function activates the exchange of cyclic IO data.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      inputUpdateState    Input update state (see \ref PN_API_IOD_InputUpdateState_t).
 *
 * \ingroup PN_APP_IOD_COMMUNICATION_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_readyForInputUpdateCbf
 */
void PN_APP_IOD_cbReadyForInputUpdate(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint16_t arNum,
    const PN_API_IOD_InputUpdateState_t inputUpdateState);

/*!
 * \brief
 * Callback function to dynamically reconfigure the submodules and claiming their ownership.
 *
 * \details
 * This function receives the expected submodule configuration from the IO controller and tries to
 * align with it. To do this, it checks the compatibility of each sumodule, updates its location if
 * possible, stores the updated information in the remanent memory and informs the IO controller
 * about the final ownership status.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in,out]  expSubmodsCfg       Configured submodule list to be verified. User
 *                                     application needs to set the following values:<br>
 *                                     ownSubmod->submod[i].ownSessionKey = 0, if AR
 *                                         ownership is NOT assumed.<br>
 *                                     ownSubmod->submod[i].isWrongSubmod = true, if
 *                                         submodule ownership is NOT assumed. Otherwise, true.
 *
 * \ingroup PN_APP_IOD_COMMUNICATION_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_arOwnershipCbf
 */
void PN_APP_IOD_cbArOwnershipInd(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t arNum,
    PN_API_IOD_Exp_t *expSubmodsCfg);

#if (defined __cplusplus)
}
#endif

#endif /* PN_APP_IOD_COMMUNICATION_H */
