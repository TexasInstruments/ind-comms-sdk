/*!
 * \file pn_api_iod_alarm.h
 *
 * \brief
 * Functions and callback-prototypes needed for handling the Profinet alarms.
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

#ifndef PN_API_IOD_ALARM_H
#define PN_API_IOD_ALARM_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*! \ingroup PN_API_IOD_ALARM_DOXY_GROUP
 * @{
 */
#define PN_API_IOD_CM_ALARM_PRIORITY_LOW              0 /*!< Alarm priority low. */
#define PN_API_IOD_CM_ALARM_PRIORITY_HIGH             1 /*!< Alarm priority high. */

#define PN_API_IOD_CM_ALARM_TYPE_DIAGNOSIS            0x0001 /*!< Diagnosis alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_PROCESS              0x0002 /*!< Process alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_PULL                 0x0003 /*!< Pull alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_PLUG                 0x0004 /*!< Plug alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_STATUS               0x0005 /*!< Status alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_UPDATE               0x0006 /*!< Update alarm.*/
#define PN_API_IOD_CM_ALARM_TYPE_MEDIA_REDUNDANCY     0x0007 /*!< Media redunduncy alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_CTRL_BY_SUPERVISOR   0x0008 /*!< Controlled by supervisor alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_RELEASED             0x0009 /*!< Released by supervisor (old). */
#define PN_API_IOD_CM_ALARM_TYPE_PLUG_WRONG           0x000A /*!< Plug wrong alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_RETURN_OF_SUBMODULE  0x000B /*!< Return of submodule alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS 0x000C /*!< Diagnosis disappears alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_MCR_MISMATCH         0x000D /*!< Multicast communication mismatch. */
#define PN_API_IOD_CM_ALARM_TYPE_PORT_DATA_CHANGED    0x000E /*!< Port data changed alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_SYNC_DATA_CHANGED    0x000F /*!< Sync data changed alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_ISOCHRONE_MODE_PROB  0x0010 /*!< Isochronous mode problem alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_NETWORK_COMP_PROB    0x0011 /*!< Network component problem alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_TIME_DATA_CHANGED    0x0012 /*!< Time data changed alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_DFP_PROB             0x0013 /*!< DFP problem alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_MRPD_PROB            0x0014 /*!< MRPD problem alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_RESERVED_0X0015      0x0015 /*!< Reserved. */
#define PN_API_IOD_CM_ALARM_TYPE_MULTIPLE_IF          0x0016 /*!< Multiple interface mismatch alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_RESERVED_0X0017      0x0017 /*!< Reserved. */
#define PN_API_IOD_CM_ALARM_TYPE_RESERVED_0X0018      0x0018 /*!< Reserved. */
#define PN_API_IOD_CM_ALARM_TYPE_RESERVED_0X0019      0x0019 /*!< Reserved. */
#define PN_API_IOD_CM_ALARM_TYPE_RESERVED_0X001A      0x001A /*!< Reserved. */
#define PN_API_IOD_CM_ALARM_TYPE_RESERVED_0X001B      0x001B /*!< Reserved. */
#define PN_API_IOD_CM_ALARM_TYPE_RESERVED_0X001C      0x001C /*!< Reserved. */
#define PN_API_IOD_CM_ALARM_TYPE_RESERVED_0X001D      0x001D /*!< Reserved. */
#define PN_API_IOD_CM_ALARM_TYPE_UPLOAD_AND_STORAGE   0x001E /*!< Upload and storage alarm. */
#define PN_API_IOD_CM_ALARM_TYPE_PULL_MODULE          0x001F /*!< Pull module alarm. */
/** @}*/

/*!
 * \brief
 * This function notifies the IO controller of a process alarm unique to this submodule.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      data                Alarm data.
 * \param[in]      dataSize            Size of alarm data in bytes.
 * \param[in]      identUserStruct     User structure identifier (PROFINET IO Specification IEC 61158).
 * \param[in]      userHandle          User defined handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_alarm.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status, api = 0, userHandle = 0;
 * PN_API_IOD_DevAddr_t addr;
 * addr.Type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 0;
 * addr.address.geo.subslot = 0;
 * uint8_t buf[64];
 * uint8_t bufSize = 64:
 * uint16_t ident = 0;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_sendProcessAlarm(pnHandle, api, &addr, &buf, bufSize, ident, userHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ALARM_DOXY_GROUP
 */
uint32_t PN_API_IOD_sendProcessAlarm(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t api,
    PN_API_IOD_DevAddr_t *const addr,
    uint8_t *const data,
    const uint32_t dataSize,
    const uint16_t identUserStruct,
    const uint32_t userHandle);

/*!
 * \brief
 * This function notifies the IO controller of a status alarm unique to this submodule.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      data                Alarm data.
 * \param[in]      dataSize            Size of alarm data in bytes.
 * \param[in]      identUserStruct     User structure identifier (PROFINET IO Specification IEC 61158).
 * \param[in]      userHandle          User defined handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_alarm.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status, api = 0, userHandle = 0;
 * PN_API_IOD_DevAddr_t addr;
 * addr.Type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 0;
 * addr.address.geo.subslot = 0;
 * uint8_t buf[64];
 * uint8_t bufSize = 64:
 * uint16_t ident = 0;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_sendStatusAlarm(pnHandle, api, &addr, &buf, bufSize, ident, userHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ALARM_DOXY_GROUP
 */
uint32_t PN_API_IOD_sendStatusAlarm(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t api,
    PN_API_IOD_DevAddr_t *const addr,
    uint8_t *const data,
    const uint32_t dataSize,
    const uint16_t identUserStruct,
    const uint32_t userHandle);

/*!
 * \brief
 * This function sends an event to a central parameter server. As a result of this event, the server
 * runs a record read service to retrieve a set of parameters from the device or a record write service
 * to transmit a set of parameters to it. Part of the alarm data header indicates which record index is
 * being utilized and whether the data is intended to be written or read.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      data                Alarm data.
 * \param[in]      dataSize            Size of alarm data in bytes.
 * \param[in]      userHandle          User defined handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_alarm.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status, api = 0, userHandle = 0;
 * PN_API_IOD_DevAddr_t addr;
 * addr.Type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 0;
 * addr.address.geo.subslot = 0;
 * uint8_t buf[64];
 * uint8_t bufSize = 64:
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_sendUploadRetrievalAlarm(pnHandle, api, &addr, &buf, bufSize, userHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ALARM_DOXY_GROUP
 */
uint32_t PN_API_IOD_sendUploadRetrievalAlarm(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t api,
    PN_API_IOD_DevAddr_t *const addr,
    uint8_t *const data,
    const uint32_t dataSize,
    const uint32_t userHandle);

/*!
 * \brief
 * The function sends a “Return of Submodule” alarm to the IO controller. The alarm is triggered
 * when the status of IOPS/IOCS changes from “BAD” to “GOOD”. The IO controller does not
 * reparameterize the submodule.
 *
 * \details
 * No alarm may be triggered if the state of IOPS/IOCS changes from “GOOD” to “BAD”.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      userHandle          User defined handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_alarm.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status, api = 0, userHandle = 0;
 * PN_API_IOD_DevAddr_t addr:
 * addr.Type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 0;
 * addr.address.geo.subslot = 0;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_sendRetOfSubAlarm(pnHandle, api, &addr, userHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ALARM_DOXY_GROUP
 */
uint32_t PN_API_IOD_sendRetOfSubAlarm(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t api,
    PN_API_IOD_DevAddr_t *const addr,
    const uint32_t userHandle);

/*!
 * \brief
 * Callback function prototype to acknowledge an asynchronous request
 * issued by the user application to the Profinet stack. Currently,
 * only alarms are implemented as asynchronous requests.
 *
 * \details
 * If the submodule to which the alarm has been assigned, is not included
 * in a running AR, then arNum is set to 0 by the stack to refer to a dummy
 * acknowledgement.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       arNum               AR number.
 * \param[in]       alarmType           Alarm type (see \ref PN_API_IOD_AlarmType_t).
 * \param[in]       api                 Application Process Identifier.
 * \param[in]       addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]       status              #PN_API_OK or #PN_API_NOT_OK.
 * \param[in]       diagnosisTag        Diagnosis tag which distinguishes different alarms
 *                                      on one subslot (can not be 0).
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_alarm.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * PN_API_IOD_IOXS PN_APP_IOD_cbAsyncReqDone(
 *     PN_API_IOD_Handle_t *const pnHandle,
 *     uint32_t arNum, PN_API_IOD_AlarmType_t alarmType,
 *     uint32_t api, PN_API_IOD_DevAddr_t *addr,
 *     uint32_t status, uint16_t diagnosisTag)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .asyncReqDone = PN_APP_IOD_cbAsyncReqDone,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ALARM_DOXY_GROUP
 */
typedef void (*PN_API_IOD_asyncReqDoneCbf)(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t arNum,
    const PN_API_IOD_AlarmType_t alarmType,
    const uint32_t api,
    PN_API_IOD_DevAddr_t* const addr,
    const uint32_t status,
    const uint16_t diagnosisTag);

/*!
 * \brief
 * Callback function prototype to notify an alarm that has been received from the IO controller.
 *
 * \details
 * This function sends the received alarm information to the user application.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       devAlarmData        Pointer to alarm data.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_alarm.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * void PN_APP_IOD_cbDevAlarm(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    PN_API_IOD_DevAlarmData_t *devAlarmData)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .devAlarm = PN_APP_IOD_cbDevAlarm,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_ALARM_DOXY_GROUP
 */
typedef void (*PN_API_IOD_devAlarmCbf)(
    PN_API_IOD_Handle_t *const pnHandle,
    const PN_API_IOD_DevAlarmData_t *devAlarmData);


#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_ALARM_H
