/*!
 *  \file pn_api_iod_startup.h
 *
 *  \brief
 *  Functions and callback-prototypes needed for starting up the IO device.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#ifndef PN_API_IOD_STARTUP_H
#define PN_API_IOD_STARTUP_H

#include "pn_api_iod_callbacks.h"
#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*!
 * \brief
 * Initialize the stack and allocate necessary resources.
 *
 * \details
 * Initialize the stack including its memory resources, interprocess communication mechanism,
 * task handling and interfaces to OS, BSP, and sockets.
 *
 * \warning
 * Must be called first.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_startup.h"
 *
 * uint32_t status;
 * status = PN_API_IOD_initStack();
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_STARTUP_DOXY_GROUP
 */
uint32_t PN_API_IOD_initStack(void);

/*!
 * \brief
 * Startup Profinet stack.
 *
 * \details
 * Use the configuration given to the function in addition to some device information obtained from the
 * NV memory (like name and IP address) to startup the device, build its SNMP parameters and plug its
 * initial submodules on their corresponding subslots.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      iodInst             IO Device Instance.
 * \param[in]      devAnnotation       Device annotation.
 * \param[in]      ioSubmodList        IO Submodule list.
 * \param[in]      ioSubmodListSize    Number of elements in IO Submodule list.
 * \param[in]      im0List             IM0 list.
 * \param[in]      im0ListSize         Number of elements in IM0 list.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      NV memory read operation failed or general error.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_IodInstance_t iodInst = { 0 };
 * PN_API_IOD_DevAnnotation_t devAnnotation = { 0 };
 * PN_API_IOD_SubmodListEntry_t* ioSubmodList = NULL;
 * PN_API_IOD_Im0ListEntry_t* im0List = NULL;
 * uint32_t ioSubmodListSize = 0;
 * uint32_t im0ListSize = 0;
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Build iodInst
 * ...
 *
 * //Build devAnnotation
 * ...
 *
 * //Fill up info and sizes of ioSubmodList and im0List
 * ...
 *
 * status = PN_API_IOD_startup(pnHandle, &iodInst, &devAnnotation, ioSubmodList,
 *     ioSubmodListSize, im0List, im0ListSize);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_STARTUP_DOXY_GROUP
 */
uint32_t PN_API_IOD_startup(
    PN_API_IOD_Handle_t *const    pnHandle,
    PN_API_IOD_IodInstance_t     *iodInst,
    PN_API_IOD_DevAnnotation_t   *devAnnotation,
    PN_API_IOD_SubmodListEntry_t *ioSubmodList,
    uint32_t                      ioSubmodListSize,
    PN_API_IOD_Im0ListEntry_t    *im0List,
    uint32_t                      im0ListSize);

/*!
 * \brief
 * Register callback functions.
 *
 * \details
 * Register callback functions implemented by the user application.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      callbacksInst       List of implemented callback function names.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * PN_API_IOD_Callbacks_t callbacks = {
 *      .setStationName = PN_APP_IOD_cbSetStationName,
 *      .setIpAddr = PN_APP_IOD_cbSetIpAddr,
 *      .arConnect = PN_APP_IOD_cbArConnectInd,
 *      ... }
 *
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_STARTUP_DOXY_GROUP
 */
uint32_t PN_API_IOD_registerCallbacks(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_Callbacks_t    *callbacksInst);

/*!
 * \brief
 * Send an asynchronous application ready message.
 *
 * \details
 * Needed only if the "application ready" for a submodule should be delayed due to incomplete
 * parameter assignment for that submodule. Therefore, instead of passing this information to the
 * stack using the return value of PN_API_IOD_paramEndCbf() so the stack can automatically generate
 * "application ready", the user application returns #PN_API_IOD_SubmodStatusAppReadyFollows for the
 * PN_API_IOD_paramEndCbf() and later sends the "application ready" using this function.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      slot                Slot number.
 * \param[in]      submodId            Submodule identifier, or 0 for all submodules.
 * \param[in]      submodState         Submodule state.
 * \param[in]      moreFollows         \c true, if "application ready" for more submodules<br>
 *                                        in this AR are expected.<br>
 *                                     \c false, if no further "application ready" needed.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_API_IOD_STARTUP_DOXY_GROUP
 */
uint32_t PN_API_IOD_asyncApplReady(
    PN_API_IOD_Handle_t *const pnHandle,
    uint16_t                   arNum,
    uint32_t                   api,
    uint16_t                   slot,
    uint16_t                   submodId,
    PN_API_IOD_SubmodStatus_t  submodState,
    bool                       moreFollows);

#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_STARTUP_H
