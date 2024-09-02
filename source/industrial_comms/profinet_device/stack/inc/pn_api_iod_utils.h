/*!
 * \file pn_api_iod_utils.h
 *
 * \brief
 * Collection of useful functions
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

#ifndef PN_API_IOD_UTILS_H
#define PN_API_IOD_UTILS_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*! \ingroup PN_API_IOD_UTILS_DOXY_GROUP
 * @{
 */
#define PN_API_IOD_LOG_CHAT             9  /*!< Chat. */
#define PN_API_IOD_LOG_CHAT_HIGH        8  /*!< Chat, level high. */
#define PN_API_IOD_LOG_NOTE             7  /*!< Note, level normal. */
#define PN_API_IOD_LOG_NOTE_HIGH        6  /*!< Note, level high. */
#define PN_API_IOD_LOG_WARNING          5  /*!< Warning. */
#define PN_API_IOD_LOG_WARNING_HIGH     4  /*!< Warning, level high (unexpected situation). */
#define PN_API_IOD_LOG_ERROR            3  /*!< Error, tradeable. */
#define PN_API_IOD_LOG_ERROR_FATAL      2  /*!< Error, not tradeable (cannot be switched off). */
#define PN_API_IOD_LOG_DEACTIVATED      1  /*!< Logging off. */
/** @}*/

/*!
 * \brief
 * Reads the version of the stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      version            Pointer for version.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_utils.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * PN_API_IOD_DkVersion_t* pVersion;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_getVersion(pnHandle, &pVersion);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_UTILS_DOXY_GROUP
 */
uint32_t PN_API_IOD_getVersion(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DkVersion_t *const version);

/*!
 * \brief
 * Provides to the application a greater deteiled error value.
 *
 * \details
 * The value, saved as a result of the last CM request, is converted into a user-error number.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 * \retval         see \ref PN_API_IOD_ERROR_DOXY_GROUP.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_utils.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t error;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * error = PN_API_IOD_getLastError(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_UTILS_DOXY_GROUP
 */
uint32_t PN_API_IOD_getLastError(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Called when a "fatal error" in detected by the application.
 *
 * \details
 * This function is used to inform Profinet stack that a fatal error has taken place.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_utils.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t error;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * error = PN_API_IOD_fatalError(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_UTILS_DOXY_GROUP
 */
uint32_t PN_API_IOD_fatalError(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Callback function prototype to indicate the need to log an error.
 *
 * \details
 * When the stack detects and error, it sends an error log indication to the application.
 * The indication contains information about how critical the error is (error level), which
 * is described by the values between #PN_API_IOD_LOG_CHAT and #PN_API_IOD_LOG_DEACTIVATED.
 * The application then decides to take a corresponding action which can range from ignoring
 * the error to completely shutting down Profinet service.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      errorLevel          Level of the error.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_utils.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .errorLog = PN_APP_IOD_cbErrorLog,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_UTILS_DOXY_GROUP
 */
typedef void(*PN_API_IOD_errorLogCbf)(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t errorLevel);

#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_UTILS_H
