/*!
 *  \file pn_api_iod_handle.h
 *
 *  \brief
 *  Functions and callback-prototypes to control the handle of Profinet stack.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#ifndef PN_API_IOD_HANDLE_H
#define PN_API_IOD_HANDLE_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*!
 * \brief
 * Creates Profinet API handle.
 *
 * \details
 * This handle is an identifier for the Profinet API instance created. It
 * can be given as a parameter to all API function calls that follow afterwards.
 *
 * \return         API handle as #PN_API_IOD_Handle_t.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_handle.h"
 *
 * PN_API_IOD_Handle_t* pnHandle;
 * pnHandle = PN_API_IOD_new();
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_HANDLE_DOXY_GROUP
 */
PN_API_IOD_Handle_t *PN_API_IOD_new(void);

/*!
 * \brief
 * Check Profinet API handle validity.
 *
 * \details
 * The given handle has to be non-NULL and is has to be one of
 * the previously allocated ones.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as bool.
 * \retval         true                Handle is valid.
 * \retval         false               Handle is invalid.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_handle.h"
 *
 * PN_API_IOD_Handle_t* pnHandle;
 * bool isValid = false;
 *
 * isValid = PN_API_IOD_isHandleValid(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_HANDLE_DOXY_GROUP
 */
bool PN_API_IOD_isHandleValid(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Deletes Profinet API handle.
 *
 * \details
 * The handle needs to be previously created using PN_API_IOD_new().
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_handle.h"
 *
 * PN_API_IOD_Handle_t* pnHandle;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_delete(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_HANDLE_DOXY_GROUP
 */
uint32_t PN_API_IOD_delete(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Initializes Profinet stack.
 *
 * \details
 * The initialization process of the stack needs to have a handle that
 * is previously created using PN_API_IOD_new().
 *
 * \warning
 * Not implemented yet.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_handle.h"
 *
 * PN_API_IOD_Handle_t* pnHandle;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_init(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_HANDLE_DOXY_GROUP
 */
uint32_t PN_API_IOD_init(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Runs Profinet stack.
 *
 * \details
 * Upon stack initialization, it can be run using this function.
 *
 * \warning
 * Not implemented yet.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_handle.h"
 *
 * PN_API_IOD_Handle_t* pnHandle;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_init(pnHandle);
 *
 * status = PN_API_IOD_run(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_HANDLE_DOXY_GROUP
 */
uint32_t PN_API_IOD_run(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Resets Profinet stack.
 *
 * \details
 * Resets stack's configuration and its parameter values.
 *
 * \warning
 * Not implemented yet.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_handle.h"
 *
 * PN_API_IOD_Handle_t* pnHandle;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_init(pnHandle);
 *
 * status = PN_API_IOD_run(pnHandle);
 *
 * status = PN_API_IOD_reset(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_HANDLE_DOXY_GROUP
 */
uint32_t PN_API_IOD_reset(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Stops Profinet stack.
 *
 * \details
 * Stops a running Profinet stack.
 *
 * \warning
 * Not implemented yet.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_handle.h"
 *
 * PN_API_IOD_Handle_t* pnHandle;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_init(pnHandle);
 *
 * status = PN_API_IOD_run(pnHandle);
 *
 * status = PN_API_IOD_stop(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_HANDLE_DOXY_GROUP
 */
uint32_t PN_API_IOD_stop(PN_API_IOD_Handle_t *const pnHandle);

#if(defined __cplusplus)
}
#endif

#endif /* PN_API_IOD_HANDLE_H */
