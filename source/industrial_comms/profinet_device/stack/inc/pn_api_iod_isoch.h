/*!
 * \file pn_api_iod_isoch.h
 *
 * \brief
 * Functions for isochronous real-time (IRT)
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

#ifndef PN_API_IOD_ISOCH_H
#define PN_API_IOD_ISOCH_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

typedef void (*PN_API_IOD_cbIscoh_t)(void);

/*!
 * \brief
 * Implements an ISR handler to be executed after a specified delay to NewCycle. The service scans
 * for an available object, sets it up and returns an object handle.
 * If multiple events are requested it is important to setup the event with the lower delaytime first.
 *
 * \param[in]      pnHandle             Profinet API Handle.
 * \param[in]      pIstCbf              Pointer to callback function.
 * \param[in]      nsDelayTime          Delay value added to new cycle in nsec.
 * \param[out]     pObjHandle           Return handle to given address, used by PN_API_IOD_freeIsoObj().
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_isoch.h"
 *
 * void afterIsrCallback()
 * {
 *    // do after ISR callback work
 * }
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 * PN_API_IOD_IsoObjHndl objHandle;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_activateIsoIsrObj(pnHandle, afterIsrCallback, 0, &objHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ISOCH_DOXY_GROUP
 */
uint32_t PN_API_IOD_activateIsoIsrObj(
    PN_API_IOD_Handle_t     *const pnHandle,
    PN_API_IOD_cbIscoh_t pIstCbf,
    const uint32_t           nsDelayTime,
    PN_API_IOD_IsoObjHndl_t *pObjHandle);

/*!
 * \brief
 * Configures a comparator to generate a pulse on a chosen GPIO after a specific delay to
 * NewCycle. The service scans for an available object, sets it up and returns an object handle.
 * If multiple events are requested it is important to setup the event with the lower delaytime first.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      nsDelayTime         Delay value added to new cycle in nsec.
 * \param[in]      nsDurationTime      Pulse duration in nsec.
 * \param[out]     pObjHandle          Return handle to given address, used by PN_API_IOD_freeIsoObj().
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_isoch.h"
 *
 * void afterIsrCallback()
 * {
 *    // do after ISR callback work
 * }
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 * PN_API_IOD_IsoObjHndl objHandle;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_activateIsoGpioObj(pnHandle, 0, 4000, &objHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ISOCH_DOXY_GROUP
 */
uint32_t PN_API_IOD_activateIsoGpioObj(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t             nsDelayTime,
    const uint32_t             nsDurationTime,
    PN_API_IOD_IsoObjHndl_t   *pObjHandle);

/*!
 * \brief
 * Dealocates a pre-existing object.
 * There is a internal limitation that it is required to free all instances at once.
 * If only single one is freed, the other will not work anymore.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[out]     objHandle           Handle to given address.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \warning
 * The application is responsible for reconfiguring a GPIO previously set up using
 * PN_API_IOD_activateIsoGpioObj().
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_isoch.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 * PN_API_IOD_IsoObjHndl objHandle;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * // Example to free objHandle
 * status = PN_API_IOD_activateIsoIsrObj(pnHandle, afterIsrCallback, 0, &objHandle);
 * status = PN_API_IOD_freeIsoObj(pnHandle, objHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ISOCH_DOXY_GROUP
 */
uint32_t PN_API_IOD_freeIsoObj(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_IsoObjHndl_t const objHandle);


#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_ISOCH_H
