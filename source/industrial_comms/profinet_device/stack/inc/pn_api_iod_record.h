/*!
 * \file pn_api_iod_record.h
 *
 * \brief
 * Functions and callback-prototypes needed for the handling of Profinet records.
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

#ifndef PN_API_IOD_RECORD_H
#define PN_API_IOD_RECORD_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*! \ingroup PN_API_IOD_RECORD_DOXY_GROUP
 * @{
 */
#define PN_API_IOD_SUBST_ZERO                   0   /*!< Substitute value is set to zero. */
#define PN_API_IOD_SUBST_LAST_OUT               1   /*!< Substitute value is last output received. */
#define PN_API_IOD_SUBST_REPLACEMENT            2   /*!< Substitute value is set to a replacement value. */

#define PN_API_IOD_SUBST_NORMAL                 0   /*!< Normal operation (IOXS are GOOD). */
#define PN_API_IOD_SUBST_ACTIVE                 1   /*!< Substitute value active (IOXS are not GOOD). */
/** @}*/

/*!
 * \brief
 * Informs Profinet stack that data provisioning will happen asynchronously.
 *
 * \details
 * Once the user application receives a record read/write request from the IO controller, it needs
 * to send a response. Within the implementation of the read/write record callback function, the
 * user application can decide either to send the response directly or to inform the stack (using
 * this function) to expect the response later as an asynchronous action.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[out]     requestHandle       Request handle given by the stack to be used in response.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_record.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Within the implementation of record read/write callback function
 * uint32_t status;
 * void* asyncHandle = NULL;
 * status = PN_API_IOD_recordSetResponseAsync(pnHandle, &asyncHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_RECORD_DOXY_GROUP
 */
uint32_t PN_API_IOD_recordSetResponseAsync(
    PN_API_IOD_Handle_t *const pnHandle,
    void **requestHandle);

/*!
 * \brief
 * Responds to an asynchronous record read request.
 *
 * \details
 * If the record read request from the IO controller is to be handled asynchronously, the
 * user application needs to use this function to send the record read response message to the
 * IO controller. The stack expects this function only when PN_API_IOD_recordSetResponseAsync() is
 * used.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      requestHandle       Request handle obtained from PN_API_IOD_recordSetResponseAsync().
 * \param[in]      data                Data provided by the user application.
 * \param[in]      dataLength          Length of provided data.
 * \param[in]      pnioStat            Profinet status.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_record.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Within the implementation of record read/write callback function
 * void* asyncHandle = NULL;
 * status = PN_API_IOD_recordSetResponseAsync(pnHandle, &asyncHandle);
 *
 * //Later in the user application
 * uint8_t *userData;
 * uint32_t userDataLength;
 * PN_API_IOD_ErrState_t pnioStat;
 * //Fill up userData, userDataLength, pnioStat. Then:
 *
 * status = PN_API_IOD_recordReadResponse(pnHandle, asyncHandle,
 * userData, userDataLength, &pnioStat);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_RECORD_DOXY_GROUP
 */
uint32_t PN_API_IOD_recordReadResponse(
    PN_API_IOD_Handle_t *const pnHandle,
    void                      *requestHandle,
    uint8_t                   *data,
    uint32_t                   dataLength,
    PN_API_IOD_ErrState_t     *pnioStat);

/*!
 * \brief
 * Responds to an asynchronous record write request.
 *
 * \details
 * If the record write request from the IO controller is to be handled asynchronously, the
 * user application needs to use this function to send the record write response message to the
 * IO controller. The stack expects this function only when PN_API_IOD_recordSetResponseAsync() is
 * used.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      requestHandle       Request handle obtained from PN_API_IOD_recordSetResponseAsync().
 * \param[in]      dataLength          Length of provided data.
 * \param[in]      pnioStat            Profinet status.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_record.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Within the implementation of record read/write callback function
 * void* asyncHandle = NULL;
 * status = PN_API_IOD_recordSetResponseAsync(pnHandle, &asyncHandle);
 *
 * //Later in the user application
 * uint32_t userDataLength;
 * PN_API_IOD_ErrState_t pnioStat;
 * //Fill up userDataLength, pnioStat. Then:
 *
 * status = PN_API_IOD_recordWriteResponse(pnHandle, asyncHandle,
 * userDataLength, &pnioStat);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_RECORD_DOXY_GROUP
 */
uint32_t PN_API_IOD_recordWriteResponse(
    PN_API_IOD_Handle_t *const pnHandle,
    void                      *requestHandle,
    uint32_t                   dataLength,
    PN_API_IOD_ErrState_t     *pnioStat);

/*!
 * \brief
 * In case of asynchronous requests, a host controller triggers stack.
 *
 * \param[in]      pnHandle           Profinet API Handle.
 * \param[in]      pnioStat           Result of asynchronous request.
 * \param[in]      dataLength         Length of data.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_record.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 * uint32_t dataLength = 64;
 * PN_API_IOD_ErrState_t pnioStat;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_triggerPndvDsRwDone(pnHandle, &pnioStat, dataLength);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_RECORD_DOXY_GROUP
 */
uint32_t PN_API_IOD_triggerPndvDsRwDone(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_ErrState_t *const pnioStat,
    const uint32_t dataLength);

/*!
 * \brief
 * Callback function prototype to read user record data requested by the IO controller.
 *
 * \details
 * When the stack receives a "record read" request from the IO controller, it calls this callback
 * function which needs to be implemented by the user application to read a record accessed through
 * slot/subslot number and index, then store the read values in buffer.
 *
 * \warning
 * The maximum data length determined by the stack must not be surpassed in any situation.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          Session key.
 * \param[in]      sequenceNum         Sequence number.
 * \param[in]      addr                Geographical address (slot/.
 * \param[in]      recordIndex         Index of the record.
 * \param[in]      bufLen              Length of data to read (only for synchronous).
 * \param[in]      buffer              Buffer to store the read data (only for synchronous).
 * \param[in,out]  errState            Profinet error state (only for synchronous).
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_record.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbDataRead(PN_API_IOD_Handle_t *const pnHandle,
 * uint32_t api, uint16_t arNum, uint16_t sessionKey, uint32_t sequenceNum,
 * PN_API_IOD_DevAddr_t *addr, uint32_t recordIndex, uint32_t *bufLen,
 * uint8_t *buffer, PN_API_IOD_ErrState_t *errState)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .readData = PN_APP_IOD_cbDataRead,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_RECORD_DOXY_GROUP
 */
typedef uint32_t (*PN_API_IOD_recordReadCbf)(
    PN_API_IOD_Handle_t *const        pnHandle,
    uint32_t                          api,
    uint16_t                          arNum,
    uint16_t                          sessionKey,
    uint32_t                          sequenceNum,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t                          recordIndex,
    uint32_t                         *bufLen,
    uint8_t                          *buffer,
    PN_API_IOD_ErrState_t            *errState);

/*!
 * \brief
 * Callback function prototype to write user record data as requested by the IO controller.
 *
 * \details
 * When the stack receives a "record write" request from the IO controller, it calls this callback
 * function which needs to be implemented by the user application to write a record accessed through
 * slot/subslot number and index, based on the the value in buffer.
 *
 * \warning
 * The maximum data length determined by the stack must not be surpassed in any situation.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          Session key.
 * \param[in]      sequenceNum         Sequence number.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      recordIndex         Index of the record.
 * \param[in]      bufLen              Length of data to write (only for synchronous).
 * \param[in]      buffer              Buffer containing data to be written (only for synchronous).
 * \param[in,out]  errState            Profinet error state (only for synchronous).
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_record.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbDataWrite(PN_API_IOD_Handle_t *const pnHandle,
 * uint32_t api, uint16_t arNum, uint16_t sessionKey, uint32_t sequenceNum,
 * PN_API_IOD_DevAddr_t *addr, uint32_t recordIndex, uint32_t *bufLen,
 * uint8_t *buffer, PN_API_IOD_ErrState_t *errState)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .writeData = PN_APP_IOD_cbDataWrite,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_RECORD_DOXY_GROUP
 */
typedef uint32_t (*PN_API_IOD_recordWriteCbf)(
    PN_API_IOD_Handle_t *const        pnHandle,
    uint32_t                          api,
    uint16_t                          arNum,
    uint16_t                          sessionKey,
    uint32_t                          sequenceNum,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t                          recordIndex,
    const uint32_t                   *bufLen,
    const uint8_t                    *buffer,
    PN_API_IOD_ErrState_t            *errState);

/*!
 * \brief
 * Callback function prototype to read substitute values for output submodule.
 *
 * \details
 * When the IO device receives the special read record requests at index 0x8028 (read input
 * data) and index 0x8029 (read output data), it has to respond with input/output data. Profinet
 * stack offers two ways to deal with this:<br>
 * -# completely delegates this task to the user application by setting the value of
 * PN_API_IOD_INCLUDE_REC8028_8029 to 0. In this case, these two special records are treated as
 * normal ones, hence, this callback function is not needed.
 * -# Profinet stack itself handles these requests while giving the user application the opportunity
 * to update (or substitute) the value of the submodule's output data through this callback function.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      bufLen              Length of the submodule output substitute data.
 * \param[in]      buffer              Pointer to submodule output substitute data.
 * \param[in,out]  substMode           Substitution mode:<br>
 *                                     0: substitute value is set to zero.<br>
 *                                     1: last output value received from controller.<br>
 *                                     2: replacement value decided by the application.
 * \param[in,out]  substActive         Substitute active flag:<br>
 *                                     0: normal operation (IOXS are GOOD).<br>
 *                                     1: otherwise (substitute value active).
 *
 * \return         IOCS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         Data received is good.
 * \retval         #PN_API_IOD_STATUS_BAD          Data received is bad.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_alarm.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * void PN_APP_IOD_cbOutSubmodSubstValRead(
 *     PN_API_IOD_Handle_t *const pnHandle,
 *     PN_API_IOD_DevAddr_t *const addr,
 *     uint32_t bufLen,
 *     uint8_t *const buffer,
 *     uint16_t *const substMode,
 *     uint16_t *const substActive);
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .outSubmodSubstValRead = PN_APP_IOD_cbOutSubmodSubstValRead,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_RECORD_DOXY_GROUP
 */
typedef PN_API_IOD_IOXS (*PN_API_IOD_outSubmodSubstValReadCbf)(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const uint32_t bufLen,
    uint8_t *const buffer,
    uint16_t *const substMode,
    uint16_t *const substActive);

#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_RECORD_H
