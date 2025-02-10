/*!
 *  \file pn_api_iod_device.h
 *
 *  \brief
 *  Functions and callback-prototypes to control the configuration and operational state of the IO device.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#ifndef PN_API_IOD_DEVICE_H
#define PN_API_IOD_DEVICE_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*! \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 * @{
 */
#define DEVICE_STATE_OPERATE 1  /*!< Device state "operate". */
#define DEVICE_STATE_CLEAR   2  /*!< Device state "clear". */
/** @}*/

/*!
 * \brief
 * Creates an IO device instance during startup procedure.
 *
 * \details
 * With every call of this function, an IO device instance is created.
 *
 * \warning
 * Do not call this function! It is meant to be called by the user application
 * when the multi-device support is implemented in the future. For now, it's
 * functionality is covered by PN_API_IOD_startup().
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      vendorId            Device Vendor ID.
 * \param[in]      deviceId            Device ID.
 * \param[in]      devAnnotation       Device Annotation.
 * \param[in]      iodSnmpParam        SNMP parameters.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
 *
 * //Pre-requisites for this function call are not yet available, therefore
 * //it can not be called...
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_openDevice(
    PN_API_IOD_Handle_t *const   pnHandle,
    uint16_t                     vendorId,
    uint16_t                     deviceId,
    PN_API_IOD_DevAnnotation_t  *devAnnotation,
    PN_API_IOD_SnmpLldpParams_t *iodSnmpParam);

/*!
 * \brief
 * Closes an IO device instance.
 *
 * \details
 * The IO device needs to be first created using either PN_API_IOD_openDevice()
 * or PN_API_IOD_startup().
 *
 * \warning
 * API not yet implemented, since the current stack version does not support multiple device
 * instances.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * status = PN_API_IOD_closeDevice(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_closeDevice(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Starts communication with an IO device instance.
 *
 * \details
 * Enables the IO device to accept connection requests from the IO controller.
 * The IO device needs to be first created using either PN_API_IOD_openDevice()
 * or PN_API_IOD_startup().
 *
 * \warning
 * This function does not have to be called by the user application. For now, it's
 * functionality is covered by PN_API_IOD_startup().
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * status = PN_API_IOD_startDevice(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_startDevice(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Stops communication with an IO device instance.
 *
 * \details
 * Halts all active Profinet connections.
 * The IO device must first be initialized using either PN_API_IOD_openDevice()
 * or PN_API_IOD_startup().
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * status = PN_API_IOD_stopDevice(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_stopDevice(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Configures the PDev parameters of the IO device.
 *
 * \details
 * PDev parameters are simply the list of submodules including their locations,
 * I/O data lengths and their IM0 support.
 *
 * \warning
 * Do not call this function! It is meant to be called by the user application
 * when the multi-device support is implemented in the future. For now, it's
 * functionality is covered by PN_API_IOD_startup().
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      ioSubmodList        IO Submodule list.
 * \param[in]      ioSubmodListSize    Number of elements in IO Submodule list.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
 *
 * //Pre-requisites for this function call are not yet available, therefore
 * //it can not be called...
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_setupPDev(
    PN_API_IOD_Handle_t *const    pnHandle,
    PN_API_IOD_SubmodListEntry_t *ioSubmodList,
    uint32_t                      ioSubmodListSize);

/*!
 * \brief
 * Sets the IO device state to either "operate" or "clear".
 *
 * \details
 * During startup, this function should be invoked once to transition the IO device to the "operate" state.
 * When called with the "clear" option, all IO submodules retain their "superordinated locked" state until
 * another call with the "operate" option is made. Each invocation of this function terminates any ongoing AR.
 * The initial invocation with the "operate" option doesn't need to be performed separately, as
 * its call is currently incorporated within PN_API_IOD_startup().
 *
 * \warning
 * Resetting to "clear" can signal a critical device error, causing valid IO data no longer to be
 * processed. Therefore, only use this function in exceptional cases.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      devState            #DEVICE_STATE_OPERATE or #DEVICE_STATE_CLEAR.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * status = PN_API_IOD_setDeviceState(pnHandle, DEVICE_STATE_CLEAR);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_setDeviceState(PN_API_IOD_Handle_t *const pnHandle, uint32_t devState);

/*!
 * \brief
 * Inserts a submodule into a subslot.
 *
 * \details
 * During operation, this function can be invoked to add a new submodule or re-install a previously
 * failed or removed submodule.
 *
 * \warning
 * PN_API_IOD_startup() must be called first during startup to insert the DAP and PDev
 * data at least. Only then may additional modules be plugged in.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      modId               Module identifier.
 * \param[in]      submodId            Submodule identifier.
 * \param[in]      inDataLen           Length of input data.
 * \param[in]      outDataLen          Length of output data.
 * \param[in]      im0Support          Type of IM0 support (see \ref PN_API_IOD_Im0Support_t).
 * \param[in]      im0Data             If the module supports IM0, there is a pointer to the IM0 data here.
 * \param[in]      iopsInitVal         Initial value for IOPS-input, used for submodules without IO data
 *                                     (e.g. PDev) which act like input modules with data length of 0.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * uint32_t api = 0, modId = 0x03010300, submodId = 0x31030004;
 * uint32_t inDataLen = 4, outDataLen = 0;
 * uint8_t im0Support = PN_API_IOD_Im0Submodule;
 * uint8_t iopsInitVal = 0;
 * PN_API_IOD_Im0Data_t im0Data; //Fill up this array with the
 *                               //own data
 * PN_API_IOD_DevAddr_t addr;
 * addr.type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 1;
 * addr.address.geo.subslot = 2;
 *
 * status = PN_API_IOD_plugSubmod(pnHandle, api, &addr, modId
 *    submodId, inDataLen, outDataLen, im0Support, im0Data, iopsInitVal);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_plugSubmod(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t                   api,
    PN_API_IOD_DevAddr_t      *addr,
    uint32_t                   modId,
    uint32_t                   submodId,
    uint32_t                   inDataLen,
    uint32_t                   outDataLen,
    uint8_t                    im0Support,
    PN_API_IOD_Im0Data_t      *im0Data,
    uint8_t                    iopsInitVal);

/*!
 * \brief
 * Inserts a list of submodules into subslots.
 *
 * \details
 * During operation, this function can be utilized to add a list of new submodules or re-
 * install a previously failed or removed list of submodules.
 *
 * \warning
 * PN_API_IOD_startup() must be called first during startup to insert the DAP and PDev
 * data at least. Only then may additional modules be plugged in.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      ioSubmodList        IO Submodule list.
 * \param[in]      ioSubmodListSize    Number of elements in IO Submodule list.
 * \param[in]      im0List             IM0 list.
 * \param[in]      im0ListSize         Number of elements in IM0 list.
 * \param[out]     statusList          List of return-Statuses with a size
 *                                     equals to ioSubmodListSize.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * PN_API_IOD_SubmodListEntry_t* ioSubmodList = NULL;
 * PN_API_IOD_Im0ListEntry_t* im0List = NULL;
 * uint32_t ioSubmodListSize = 0;
 * uint32_t im0ListSize = 0;
 * uint32_t statusList [ioSubmodListSize];
 *
 * //Fill up info and sizes of ioSubmodList and im0List
 * ...
 *
 * status = PN_API_IOD_plugSubmodList(pnHandle, ioSubmodList,
 *    ioSubmodListSize, im0List, im0ListSize, statusList);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_plugSubmodList(
    PN_API_IOD_Handle_t *const          pnHandle,
    const PN_API_IOD_SubmodListEntry_t *ioSubmodList,
    uint32_t                            ioSubmodListSize,
    const PN_API_IOD_Im0ListEntry_t    *im0List,
    uint32_t                            im0ListSize,
    uint32_t                           *statusList);

/*!
 * \brief
 * Removes an installed submodule.
 *
 * \details
 * During operation, this function can be invoked to adjust the current configuration,
 * particularly if there's a need to unplug a submodule from its subslot.
 * In such instances, the Profinet stack automatically dispatches an alarm to the IO controller.
 *
 * \warning
 * The submodule to be pulled needs to be plugged in first using one of the functions
 * PN_API_IOD_startup(), PN_API_IOD_plugSubmod() or PN_API_IOD_plugSubmodList() .
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * PN_API_IOD_DevAddr_t addr;
 * addr.type = PN_API_IOD_AddrGeo;
 * addr.address.geo.slot = 1;
 * addr.address.geo.subslot = 2;
 *
 * status = PN_API_IOD_pullSubmod(pnHandle, api, &addr);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_pullSubmod(PN_API_IOD_Handle_t *const pnHandle, uint32_t api, PN_API_IOD_DevAddr_t *addr);

/*!
 * \brief
 * Gets real information about a submodule.
 *
 * \details
 * This function uses a submodule's API and address to get its real information from
 * the stack and return a pointer to it to the user application.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[out]     realSubmod          Submodule structure to be returned.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      slot                Slot number.
 * \param[in]      subslot             Subslot number.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * uint32_t api = 0, slot = 1, subslot = 2;
 * PN_API_IOD_RealSubmod_t realSubmod;
 *
 * status = PN_API_IOD_getRealSubmod(pnHandle, &realSubmod, api, slot, subslot);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_getRealSubmod(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_RealSubmod_t  **realSubmod,
    uint32_t                   api,
    uint32_t                   slot,
    uint32_t                   subslot);

/*!
 * \brief
 * Gets information about real submodule list.
 *
 * \details
 * This function gets real submodule list information from the
 * stack and returns a pointer to it to the user application.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[out]     realSubmodList      Submodule list to be returned.
 * \param[in]      realSubmodListSize  Size of realSubmodList.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * uint32_t realSubmodListSize = 0;
 * PN_API_IOD_RealSubmod_t *realSubmodList;
 *
 * status = PN_API_IOD_getRealSubmodList(pnHandle, &realSubmodList, &realSubmodListSize);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_getRealSubmodList(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_RealSubmod_t  **realSubmodList,
    uint32_t *realSubmodListSize);

/*!
 * \brief
 * Sets the operational status of a submodule.
 *
 * \details
 * Sets the submodule to "stop" or "run" ("unknown" and "application
 * ready follows" are not yet supported).
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      realSubmod          Submodule to be updated.
 * \param[in]      submodStatus        Status to be set (from \ref
 *                                     PN_API_IOD_SubmodStatus_t).
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_device.h"
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
 * uint32_t api = 0, slot = 1, subslot = 2;
 * PN_API_IOD_RealSubmod_t realSubmod;
 *
 * status = PN_API_IOD_getRealSubmod(pnHandle, &realSubmod, api, slot, subslot);
 *
 * stauts = PN_API_IOD_setRealSubmodStatus(pnHandle, &realSubmod, PN_API_IOD_SubmodStatusStop);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_API_IOD_setRealSubmodStatus(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_RealSubmod_t   *realSubmod,
    PN_API_IOD_RealSubmodStatus_t  submodStatus);

/*!
 * \brief
 * Callback function prototype to indicate the pull of a module.
 *
 * \details
 * When a module is pulled, Profinet stack sends a pull indication to the user
 * application indicating the geographical address of the pulled module.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_device.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .newModPull = PN_APP_IOD_cbNewModPull,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
typedef void (*PN_API_IOD_newModPullCbf)(PN_API_IOD_Handle_t *const        pnHandle,
                                         const PN_API_IOD_DevAddr_t *const addr);

/*!
 * \brief
 * Callback function prototype to indicate the plug of a module.
 *
 * \details
 * When a module is plugged, Profinet stack sends a plug indication to the
 * user application indicating the geographical address and data length of
 * the plugged module.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      inDataLen           Length of input data.
 * \param[in]      outDataLen          Length of output data.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_device.h"
 * #include "pn_api_iod_startup.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .newModPlug = PN_APP_IOD_cbNewModPlug,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DEVICE_DOXY_GROUP
 */
typedef void (*PN_API_IOD_newModPlugCbf)(PN_API_IOD_Handle_t *const        pnHandle,
                                         const PN_API_IOD_DevAddr_t *const addr,
                                         const uint32_t                    inDataLen,
                                         const uint32_t                    outDataLen);

#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_DEVICE_H
