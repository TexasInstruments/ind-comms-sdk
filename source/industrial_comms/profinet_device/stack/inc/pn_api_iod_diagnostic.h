/*!
 * \file pn_api_iod_diagnostic.h
 *
 * \brief
 * Channel and manufacturer diagnostic data setting and removal APIs
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

#ifndef PN_API_IOD_DIAGNOSTIC_H
#define PN_API_IOD_DIAGNOSTIC_H

#include "pn_api_iod_callbacks.h"
#include "pn_api_iod_types.h"

/*!
 * \brief
 * Add channel diagnostic alarm to PN stack.
 *
 * \details
 * Installs a diagnostic record into a subslot. When the device is the owner of this subslot in
 * an AR, a 'Diagnostic alarm - incoming' is automatically dispatched to the IO controller. The
 * PN_API_IOD_removeDiagChannel() function can be utilized to eliminate the diagnostic record
 * once the issue is resolved. The 'diagTag' value is a user-defined variable <> 0, serving to
 * differentiate between various alarms for a single subslot that are concurrently valid.
 * Therefore, it must always remain unique within one subslot. If only one alarm can be
 * accessible at a given time, a constant value (e.g. 1) can be employed. Otherwise, the
 * application is responsible for managing 'diagTag' values and ensuring their uniqueness within
 * this subslot.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      channelNum          Channel number.
 * \param[in]      errorNum            Error number,
 *                                     see PNIO specification coding of "ChannelErrorType".
 * \param[in]      chanDir             Channel direction
 *                                     (see \ref PN_API_IOD_DiagChannelPropDirection_t).
 * \param[in]      chanTyp             Channel type
 *                                     (see \ref PN_API_IOD_DiagChannelPropType_t).
 * \param[in]      maintenanceReq      Maintenance required.
 * \param[in]      maintenanceDem      Maintenance demanded.
 * \param[in]      diagTag             User defined diag tag != 0.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_diagnostic.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Prepare and call PN_API_IOD_startup()
 * ...
 *
 * uint32_t api = 0;
 * uint16_t channelNum = 1;
 * uint16_t errorNum;
 * uint16_t diagTag = 1;
 * PN_API_IOD_DevAddr_t addr;
 * PN_API_IOD_DiagChannelPropDirection_t chanDir = PN_API_IOD_DiagChannelPropDirectionIn;
 * PN_API_IOD_DiagChannelPropType_t chanTyp = PN_API_IOD_DiagChannelPropTypeByte;
 * bool maintenanceReq = 1;
 * bool maintenanceDem = 0;
 * addr.type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 1;
 * addr.address.geo.subslot = 2;
 *
 * status = PN_API_IOD_addDiagChannel(pnHandle, api, &addr, channelNum
 *    errorNum, chanDir, chanTyp, maintenanceReq, maintenanceDem, diagTag);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DIAGNOSTIC_DOXY_GROUP
 */
uint32_t PN_API_IOD_addDiagChannel(
        PN_API_IOD_Handle_t *const pnHandle,
        const uint32_t api,
        PN_API_IOD_DevAddr_t *const addr,
        const uint16_t channelNum,
        const uint16_t errorNum,
        const PN_API_IOD_DiagChannelPropDirection_t chanDir,
        const PN_API_IOD_DiagChannelPropType_t chanTyp,
        const bool maintenanceReq,
        const bool maintenanceDem,
        const uint16_t diagTag);

/*!
 * \brief
 * Remove channel diagnostic alarm to PN stack.
 *
 * \details
 * Removal of a diagnostic record previously added using PN_API_IOD_addDiagChannel(). The same
 * values must be provided for referencing as in the corresponding PN_API_IOD_addDiagChannel()
 * call. When the device owns this subslot in an AR, an 'Outgoing Diagnostic Alarm' is
 * automatically transmitted to the IO controller. The 'alarmState' parameter determines whether
 * additional diagnostic entries are accessible for the same channel. If 'channelNum' is equal
 * to 0x8000, the diagnostic entry remains valid for the entire submodule, and the 'alarmState'
 * value is also applicable to the complete submodule.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      channelNum          Channel number.
 * \param[in]      errorNum            Error number,
 *                                     see PNIO specification coding of "ChannelErrorType".
 * \param[in]      chanDir             Channel direction
 *                                     (see \ref PN_API_IOD_DiagChannelPropDirection_t).
 * \param[in]      chanTyp             Channel type
 *                                     (see \ref PN_API_IOD_DiagChannelPropType_t).
 * \param[in]      diagTag             User defined diag tag != 0.
 * \param[in]      alarmState          #PN_API_IOD_DiagChannelPropSpecErrDisappear,
 *                                     #PN_API_IOD_DiagChannelPropSpecErrDisappearMore.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_diagnostic.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Prepare and call PN_API_IOD_startup()
 * //add a a diagnostic channel with PN_API_IOD_addDiagChannel()
 * ...
 *
 * uint32_t api = 0;
 * uint16_t channelNum = 1;
 * uint16_t errorNum;
 * uint16_t diagTag = 1;
 * PN_API_IOD_DevAddr_t addr;
 * PN_API_IOD_DiagChannelPropDirection_t chanDir = PN_API_IOD_DiagChannelPropDirectionIn;
 * PN_API_IOD_DiagChannelPropType_t chanTyp = PN_API_IOD_DiagChannelPropTypeByte;
 * uint16_t alarmState = PN_API_IOD_DiagChannelPropSpecErrDisappear;
 * addr.type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 1;
 * addr.address.geo.subslot = 2;
 *
 * status = PN_API_IOD_removeDiagChannel(pnHandle, api, &addr, channelNum
 *    errorNum, chanDir, chanTyp, diagTag, alarmState);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DIAGNOSTIC_DOXY_GROUP
 */
uint32_t PN_API_IOD_removeDiagChannel(
        PN_API_IOD_Handle_t *const pnHandle,
        const uint32_t api,
        PN_API_IOD_DevAddr_t *const addr,
        const uint16_t channelNum,
        const uint16_t errorNum,
        const PN_API_IOD_DiagChannelPropDirection_t chanDir,
        const PN_API_IOD_DiagChannelPropType_t chanTyp,
        const uint16_t diagTag,
        const uint16_t alarmState);

/*!
 * \brief
 * Add extended channel diagnostic alarm to PN stack.
 *
 * \details
 * Installs an extended diagnostic record within a subslot. When the device owns this subslot in
 * an AR, an extended 'Diagnostic alarm - incoming' is automatically dispatched to the IO
 * controller. The PN_API_IOD_removeExtDiagChannel() function can be employed for eliminating the
 * diagnostic record once the issue is resolved. The 'diagTag' value is a user-defined
 * variable <> 0, serving to differentiate between various alarms for a single subslot that are
 * concurrently valid. Therefore, it must always remain unique within one subslot. If only one
 * alarm can be accessible at a given time, a constant value (e.g. 1) can be utilized.
 * Otherwise, the application is responsible for managing 'diagTag' values and ensuring their
 * uniqueness within this subslot.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      channelNum          Channel number.
 * \param[in]      errorNum            Error number,
 *                                     see PNIO specification coding of "ChannelErrorType".
 * \param[in]      chanDir             Channel direction
 *                                     (see \ref PN_API_IOD_DiagChannelPropDirection_t).
 * \param[in]      chanTyp             Channel type
 *                                     (see \ref PN_API_IOD_DiagChannelPropType_t).
 * \param[in]      extChannelErrType   Extended channel error type (see PNIO specification).
 * \param[in]      extChannelAddValue  Extended channel add. value (see PNIO specification).
 * \param[in]      maintenanceReq      Maintenance required.
 * \param[in]      maintenanceDem      Maintenance demanded.
 * \param[in]      diagTag             User defined diag tag != 0.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_diagnostic.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Prepare and call PN_API_IOD_startup()
 * ...
 *
 * uint32_t api = 0;
 * uint16_t channelNum = 1;
 * uint16_t errorNum;
 * uint16_t diagTag = 1;
 * PN_API_IOD_DevAddr_t addr;
 * PN_API_IOD_DiagChannelPropDirection_t chanDir = PN_API_IOD_DiagChannelPropDirectionIn;
 * PN_API_IOD_DiagChannelPropType_t chanTyp = PN_API_IOD_DiagChannelPropTypeByte;
 * uint16_t extChannelErrType = ..; // look at IEC61158-5-10/
 * uint32_t extChannelAddValue = ....; // look at IEC61158-5-10/
 * bool maintenanceReq = 1, bool maintenanceDem = 0;
 * addr.type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 1;
 * addr.address.geo.subslot = 2;
 *
 * status = PN_API_IOD_addExtDiagChannel(pnHandle, api, &addr, channelNum
 *    errorNum, chanDir, chanTyp, extChannelErrType, extChannelAddValue, maintenanceReq,
 * maintenanceDem, diagTag);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DIAGNOSTIC_DOXY_GROUP
 */
uint32_t PN_API_IOD_addExtDiagChannel(
        PN_API_IOD_Handle_t *const pnHandle,
        const uint32_t api,
        PN_API_IOD_DevAddr_t *const addr,
        const uint16_t channelNum,
        const uint16_t errorNum,
        const PN_API_IOD_DiagChannelPropDirection_t chanDir,
        const PN_API_IOD_DiagChannelPropType_t chanTyp,
        const uint16_t extChannelErrType,
        const uint32_t extChannelAddValue,
        const bool maintenanceReq,
        const bool maintenanceDem,
        const uint16_t diagTag);

/*!
 * \brief
 * Remove extended channel diagnostic alarm to PN stack.
 *
 * \details
 * Removal of a diagnostic record that was previously added using
 * PN_API_IOD_addExtDiagChannel(). If the device owns this subslot within an AR, an extended
 * 'Diagnostic alarm - outgoing' is automatically transmitted to the IO controller. The
 * 'alarmState' parameter determines whether additional diagnostic entries are accessible for the
 * same channel. If 'channelNum' equals 0x8000, the diagnostic entry remains applicable to the
 * entire submodule, and the 'alarmState' value also applies to the complete submodule.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      channelNum          Channel number.
 * \param[in]      errorNum            Error number,
 *                                     see PNIO specification coding of "ChannelErrorType".
 * \param[in]      chanDir             Channel direction
 *                                     (see \ref PN_API_IOD_DiagChannelPropDirection_t).
 * \param[in]      chanTyp             Channel type
 *                                     (see \ref PN_API_IOD_DiagChannelPropType_t).
 * \param[in]      extChannelErrType   Extended channel error type (see PNIO specification).
 * \param[in]      extChannelAddValue  Extended channel add. value  (see PNIO specification).
 * \param[in]      diagTag             User defined diag tag != 0.
 * \param[in]      alarmState          #PN_API_IOD_DiagChannelPropSpecErrDisappear or
 *                                     #PN_API_IOD_DiagChannelPropSpecErrDisappearMore.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_diagnostic.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Prepare and call PN_API_IOD_startup()
 * //add an extended diagnostic channel with PN_API_IOD_addExtDiagChannel()
 * ...
 *
 * uint32_t api = 0;
 * uint16_t channelNum = 1;
 * uint16_t errorNum;
 * uint16_t diagTag = 1;
 * PN_API_IOD_DevAddr_t addr;
 * PN_API_IOD_DiagChannelPropDirection_t chanDir = PN_API_IOD_DiagChannelPropDirectionIn;
 * PN_API_IOD_DiagChannelPropType_t chanTyp = PN_API_IOD_DiagChannelPropTypeByte;
 * uint16_t extChannelErrType = ..; // look at IEC61158-5-10/
 * uint32_t extChannelAddValue = ....; // look at IEC61158-5-10/
 * addr.type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 1;
 * addr.address.geo.subslot = 2;
 * uint16_t alarmState = PN_API_IOD_DiagChannelPropSpecErrDisappear;
 *
 * status = PN_API_IOD_removeExtDiagChannel(pnHandle, api, &addr, channelNum
 *    errorNum, chanDir, chanTyp, extChannelErrType, extChannelAddValue, diagTag, alarmState);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DIAGNOSTIC_DOXY_GROUP
 */
uint32_t PN_API_IOD_removeExtDiagChannel(
        PN_API_IOD_Handle_t *const pnHandle,
        const uint32_t api,
        PN_API_IOD_DevAddr_t *const addr,
        const uint16_t channelNum,
        const uint16_t errorNum,
        const PN_API_IOD_DiagChannelPropDirection_t chanDir,
        const PN_API_IOD_DiagChannelPropType_t chanTyp,
        const uint16_t extChannelErrType,
        const uint32_t extChannelAddValue,
        const uint16_t diagTag,
        const uint16_t alarmState);

/*!
 * \brief
 * Add generic channel diagnostic alarm to PN stack.
 *
 * \details
 * Installs a manufacturer-specific diagnostic record within a subslot. The diagnostic data can
 * be retrieved using a distinct record call (as described in IEC61158-5-10), such as through a
 * diagnostic tool. Multiple diagnostic records associated with a user-specified tag can be
 * transferred to asubslot. The diagnostic record can be deleted using the
 * PN_API_IOD_removeDiagGeneric() function via this reference. If the device assumes ownership
 * of this subslot within an AR, a 'manufacturer specific incoming alarm' is automatically dispatched
 * to the IO controller.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      chanNum             Channel number.
 * \param[in]      chanDir             Channel direction
 *                                     (see \ref PN_API_IOD_DiagChannelPropDirection_t).
 * \param[in]      chanTyp             Channel type
 *                                     (see \ref PN_API_IOD_DiagChannelPropType_t).
 * \param[in]      diagTag             User defined diag tag != 0.
 * \param[in]      userStructIdent     Manufacturer specific, 0...0x7fff, see IEC 61158.
 * \param[in]      pInfoData           User defined generic diagnostic data.
 * \param[in]      infoDataLen         Length of generic diagnostic data.
 * \param[in]      maintenanceReq      Maintenance required.
 * \param[in]      maintenanceDem      Maintenance demanded.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_diagnostic.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Prepare and call PN_API_IOD_startup()
 * ...
 *
 * uint32_t api = 0;
 * uint16_t diagTag = 1;
 * PN_API_IOD_DevAddr_t addr;
 * PN_API_IOD_DiagChannelPropDirection_t chanDir = PN_API_IOD_DiagChannelPropDirectionIn;
 * PN_API_IOD_DiagChannelPropType_t chanTyp = PN_API_IOD_DiagChannelPropTypeByte;
 * uint16_t userStructIdent = ..; // look at IEC61158-5-10/
 * uint16_t infoDataLen = ..; // look at IEC61158-5-10/
 * uint16_t pInfoData = ..; // look at IEC61158-5-10/
 * uint16_t chanNum = 1;
 * bool maintenanceReq = 1;
 * bool maintenanceDem = 0;
 * addr.type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 1;
 * addr.address.geo.subslot = 2;
 *
 * status = PN_API_IOD_addDiagGeneric(pnHandle, api, &addr, chanNum,
 *    chanDir, chanTyp, diagTag, userStructIdent, pInfoData,
 *    infoDataLen, maintenanceReq, maintenanceDem);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DIAGNOSTIC_DOXY_GROUP
 */
uint32_t PN_API_IOD_addDiagGeneric(
        PN_API_IOD_Handle_t *const pnHandle,
        const uint32_t api,
        PN_API_IOD_DevAddr_t *const addr,
        const uint16_t chanNum,
        const PN_API_IOD_DiagChannelPropDirection_t chanDir,
        const PN_API_IOD_DiagChannelPropType_t chanTyp,
        const uint16_t diagTag,
        const uint16_t userStructIdent,
        uint8_t *const pInfoData,
        const uint32_t infoDataLen,
        const bool maintenanceReq,
        const bool maintenanceDem);

/*!
 * \brief
 * Remove generic channel diagnostic alarm to PN stack.
 *
 * \details
 * Removal of a diagnostic record previously added using PN_API_IOD_addDiagGeneric().
 * If the device assumes ownership of this subslot within an AR, a 'manuf. specific
 * outgoing alarm' is automatically transmitted to the IO controller.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      chanNum             Channel number.
 * \param[in]      chanDir             Channel direction
 *                                     (see \ref PN_API_IOD_DiagChannelPropDirection_t).
 * \param[in]      chanTyp             Channel type
 *                                     (see \ref PN_API_IOD_DiagChannelPropType_t).
 * \param[in]      diagTag             User defined diag tag != 0.
 * \param[in]      userStructIdent     Manufacturer specific, 0...0x7fff, see IEC 61158.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_diagnostic.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Prepare and call PN_API_IOD_startup()
 * //add an generic diagnostic channel with PN_API_IOD_addDiagGeneric()
 * ...
 *
 * uint32_t api = 0;
 * uint16_t diagTag = 1;
 * PN_API_IOD_DevAddr_t addr;
 * PN_API_IOD_DiagChannelPropDirection_t chanDir = PN_API_IOD_DiagChannelPropDirectionIn;
 * PN_API_IOD_DiagChannelPropType_t chanTyp = PN_API_IOD_DiagChannelPropTypeByte;
 * uint16_t userStructIdent = ..; // look at IEC61158-5-10/
 * uint16_t chanNum = 1;
 * addr.type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 1;
 * addr.address.geo.subslot = 2;
 *
 * status = PN_API_IOD_removeDiagGeneric(pnHandle, api, &addr, chanNum,
 *    chanDir, chanTyp, diagTag, userStructIdent);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DIAGNOSTIC_DOXY_GROUP
 */
uint32_t PN_API_IOD_removeDiagGeneric(
        PN_API_IOD_Handle_t *const pnHandle,
        const uint32_t api,
        PN_API_IOD_DevAddr_t *const addr,
        const uint16_t chanNum,
        const PN_API_IOD_DiagChannelPropDirection_t chanDir,
        const PN_API_IOD_DiagChannelPropType_t chanTyp,
        const uint16_t diagTag,
        const uint16_t userStructIdent);

#endif /* PN_API_IOD_DIAGNOSTIC_H */
