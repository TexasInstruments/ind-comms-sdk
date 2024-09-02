/*!
 * \file pn_api_iod_communication.h
 *
 * \brief
 * Functions and callback-prototypes needed to handle ARs (Application Relations).
 *
 * \author
 * KUNBUS GmbH
 *
 * \copyright
 * Copyright (c) 2023, KUNBUS GmbH<br /><br />
 * SPDX-License-Identifier: LicenseRef-Kunbus
 *
 * Copyright (c) 2024 KUNBUS GmbH
 * All rights reserved.
 *
 *
 */

#ifndef PN_API_IOD_COMMUNICATION_H
#define PN_API_IOD_COMMUNICATION_H

#include "pn_api_iod_types.h"

/*! \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 * @{
 */
#define PN_API_IOD_ARFSU_ENABLED     0x01   /*!< FSU enabled. */
#define PN_API_IOD_ARFSU_DISABLED    0x00   /*!< FSU disabled. */
#define PN_API_IOD_ARFSU_CHANGED     0x01   /*!< ARFSU_UUID changed in NV memory. */
#define PN_API_IOD_ARFSU_NOT_CHANGED 0x00   /*!< ARFSU_UUID not changed in NV memory. */
/** @}*/

#if(defined __cplusplus)
extern "C" {
#endif

/*!
 * \brief
 * A running application relationship to a PROFINET controller is terminated by this function.
 *
 * \details
 * The IO controller's and the IO device's connection is therefore cut off.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_communication.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status, arNum = 1;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Prepare and call PN_API_IOD_startup()
 * ...
 *
 * status = PN_API_IOD_arAbort(pnHandle, arNum);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 */
uint32_t PN_API_IOD_arAbort(PN_API_IOD_Handle_t *const pnHandle, uint32_t arNum);

/*!
 * \brief
 * Retrieves an application relation's type.
 *
 * \details
 * Application relation type can be "single", "supervisor", "single RTC3" or "single
 * system redundancy".
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 *
 * \return         AR type as #PN_API_IOD_ArType_t.
 * \retval         #PN_API_IOD_ArTypeSingle                Single AR.
 * \retval         #PN_API_IOD_ArTypeSupervisor            Supervisor AR.
 * \retval         #PN_API_IOD_ArTypeSingleRtc3            Single AR that uses RT_Class_3.
 * \retval         #PN_API_IOD_ArTypeSingleSysRedundancy   System redundancy AR.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_communication.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t arNum = 1;
 * PN_API_IOD_ArType_t arType;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Prepare and call PN_API_IOD_startup()
 * ...
 *
 * arType = PN_API_IOD_arGetType(pnHandle, arNum);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 */
PN_API_IOD_ArType_t PN_API_IOD_arGetType(PN_API_IOD_Handle_t* const pnHandle, uint16_t arNum);

/*!
 * \brief
 * Callback function prototype to notify the user application about an AR FSU update.
 *
 * \details
 * Upon receiving a new ARFSU write record, the Profinet stack calls this function. If the
 * ARFSU_UUID (Universal Unique IDentifier) kept in the NV memory has changed, or if FSU has been
 * enabled, it notifies the user program.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arFsuEnabled        #PN_API_IOD_ARFSU_ENABLED or
 *                                     #PN_API_IOD_ARFSU_DISABLED.
 * \param[in]      arFsuChanged        #PN_API_IOD_ARFSU_CHANGED or
 *                                     #PN_API_IOD_ARFSU_NOT_CHANGED.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_communication.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * void PN_APP_IOD_cbReportArFSURecord(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    const uint8_t arFsuEnabled,
 *    const uint8_t arFsuChanged)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .reportArFsuRecord = PN_APP_IOD_cbReportArFSURecord,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 */
typedef void (*PN_API_IOD_reportArFsuRecordCbf)(
    PN_API_IOD_Handle_t* const pnHandle,
    const uint8_t arFsuEnabled,
    const uint8_t arFsuChanged);

/*!
 * \brief
 * Callback function prototype to notify the user application that the stack is ready
 * to receive the first valid input data.
 *
 * \details
 * Prior to sending a "application ready" message to the participating submodule, the Profinet stack
 * calls this function either after establishing a connection or after plugging and reconfiguring an
 * operational submodule. Its purpose is to alert the user application to the necessity to write
 * valid input data within this callback.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      inputUpdateState    Input update state (see \ref PN_API_IOD_InputUpdateState_t).
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_communication.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * void PN_APP_IOD_cbReadyForInputUpdate(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    const uint16_t arNum,
 *    const PN_API_IOD_InputUpdateState_t inputUpdateState)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .readyForInputUpdate = PN_APP_IOD_cbReadyForInputUpdate,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 */
typedef void (*PN_API_IOD_readyForInputUpdateCbf)(PN_API_IOD_Handle_t *const          pnHandle,
                                                  const uint16_t                      arNum,
                                                  const PN_API_IOD_InputUpdateState_t inputUpdateState);

/*!
 * \brief
 * Callback function prototype to notify the user application that a new connection
 * to an IO controller has been established.
 *
 * \details
 * The user application must save the AR information that it received from the stack
 * within the implementation of this callback.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arType              AR type.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          Session key.
 * \param[in]      sendClock           Send clock parameter.
 * \param[in]      redRatioIoCrIn      Reduction ratio of input IOCR.
 * \param[in]      redRatioIoCrOut     Reduction ratio of output IOCR.
 * \param[in]      hostIpAddr          IP address of the host (IO controller).
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_communication.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * void PN_APP_IOD_cbArConnectInd(PN_API_IOD_Handle_t *const pnHandle,
 * PN_API_IOD_ArType_t arType, uint32_t arNum, uint16_t sessionKey,
 * uint16_t sendClock, uint16_t redRatioIoCrIn, uint16_t redRatioIoCrOut,
 * uint32_t hostIpAddr)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .arConnect = PN_APP_IOD_cbArConnectInd,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 */
typedef void (*PN_API_IOD_arConnectCbf)(PN_API_IOD_Handle_t *const pnHandle,
                                            PN_API_IOD_ArType_t        arType,
                                            uint32_t                   arNum,
                                            uint16_t                   sessionKey,
                                            uint16_t                   sendClock,
                                            uint16_t                   redRatioIoCrIn,
                                            uint16_t                   redRatioIoCrOut,
                                            uint32_t                   hostIpAddr);

/*!
 * \brief
 * Callback function prototype to notify the user application that an AR connection
 * with the IO controller has been terminated.
 *
 * \details
 * A connection termination event takes place when an established connection is
 * either alarmed or deliberately terminated by the IO controller or the IO device.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          Session key.
 * \param[in]      reasonCode          Reason code (see \ref PN_API_IOD_ArReason_t)
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_communication.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * void PN_APP_IOD_cbArDisconn(PN_API_IOD_Handle_t *const pnHandle,
 * uint32_t arNum, PN_API_IOD_ArReason_t reasonCode)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .arDisconn = PN_APP_IOD_cbArDisconn,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 */
typedef void (*PN_API_IOD_arDisconnCbf)(PN_API_IOD_Handle_t *const  pnHandle,
                                        const uint16_t              arNum,
                                        const uint16_t              sessionKey,
                                        const PN_API_IOD_ArReason_t reasonCode);

/*!
 * \brief
 * Callback function prototype to inform the user application about the submodule configuration assumed
 * by the IO controller and to ask the IO device to claim the ownership of these submodules.
 *
 * \details
 * Prior to engaging in data exchange within AR, it is imperative to have the same module/submodule
 * configuration on bother IO controller and IO device's sides. Failing to do so will result in the
 * inability to exchange valid data. The user application is responsible for confirming that the
 * configuration received from the IO controller aligns with the expected one. Subsequently, the
 * application must communicate with the stack, asserting AR ownership for each submodule if the
 * configurations match, or rejecting the submodule to prevent the exchange of valid IO data if a
 * discrepancy is identified. The application can also use this function to dynamically plug/pull
 * submodules to align the actual configuration with the expected one.
 *
 * \warning
 * A subslot can be allocated to a maximum of one AR, indicating that only a single AR can take ownership
 * of a subslot among multiple ARs. For the remaining ARs, this particular submodule is designated as
 * "superordinated locked."
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in,out]  ownSubmod           Configured submodule list to be verified. User
 *                                     application needs to set the following values:<br>
 *                                     ownSubmod->submod[i].ownSessionKey = 0, if AR
 *                                         ownership is NOT assumed.<br>
 *                                     ownSubmod->submod[i].isWrongSubmod = true, if
 *                                         submodule ownership is NOT assumed. Otherwise, true.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_communication.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * void PN_APP_IOD_cbArOwnershipInd(PN_API_IOD_Handle_t *const pnHandle,
 * uint32_t arNum, PN_API_IOD_Exp_t *pOwnSub)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .arOwnership = PN_APP_IOD_cbArOwnershipInd,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 */
typedef void (*PN_API_IOD_arOwnershipCbf)(PN_API_IOD_Handle_t *const pnHandle,
                                          uint32_t                   arNum,
                                          PN_API_IOD_Exp_t          *ownSubmod);

/*!
 * \brief
 * Callback function prototype to notify the user application that the parameter
 * assignment for all modules has been completed.
 *
 * \details
 * The user application is required to confirm this operation by using the
 * #PN_API_IOD_SubmodStatusRun return value, triggering the automatic transmission of
 * the "application ready" message to the IO controller. In this scenario, there is
 * no requirement for PN_API_IOD_asyncApplReady(). Conversely, if the user application
 * is not ready, it can acknowledge this state with a return value of #PN_API_IOD_SubmodStatusAppReadyFollows.
 * Later, when ready, the application can call PN_API_IOD_asyncApplReady().
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
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_communication.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * PN_API_IOD_SubmodStatus_t PN_APP_IOD_cbParamEnd(PN_API_IOD_Handle_t *const pnHandle,
 *    const uint16_t arNum, const uint16_t sessionKey, const uint32_t api,
 *    const uint16_t slot, const uint16_t subslot, const bool moreFollows)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .paramEnd = PN_APP_IOD_cbParamEnd,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_COMMUNICATION_DOXY_GROUP
 */
typedef PN_API_IOD_SubmodStatus_t (*PN_API_IOD_paramEndCbf)(PN_API_IOD_Handle_t *const pnHandle,
                                                            const uint16_t             arNum,
                                                            const uint16_t             sessionKey,
                                                            const uint32_t             api,
                                                            const uint16_t             slot,
                                                            const uint16_t             subslot,
                                                            const bool                 moreFollows);

#if (defined __cplusplus)
}
#endif

#endif // PN_API_IOD_COMMUNICATION_H
