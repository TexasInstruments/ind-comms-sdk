/*!
 * \file pn_api_iod_settings.h
 *
 * \brief
 * Functions and callback-prototypes for setting up the IO device's info like name and IP address.
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

#ifndef PN_API_IOD_SETTINGS_H
#define PN_API_IOD_SETTINGS_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*!
 * \brief
 * Momentarily modifies the IP suite within the Profinet stack.
 *
 * \details
 * Changes IP suite values and sends the new values to Profinet stack to be used. However,
 * the modified values are not retained within the NV memory, so they are lost after device restarts.
 *
 * \warning
 * Must be called only when no AR is running.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      newIpAddr           New IP address.
 * \param[in]      subnetMask          New value for subnet mask.
 * \param[in]      defaultGw           New value for default gateway.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong or an AR is running.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_settings.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * uint8_t ip1 = 192, ip2 = 168, ip3 = 1, ip4 = 10;
 * uint8_t mask1 = 255, mask2 = 255, mask3 = 0, mask4 = 0;
 * uint8_t gw1 = 192, gw2 = 168, gw3 = 1, gw4 = 1;
 *
 * uint32_t ip = ((uint32_t)ip1 << 24) | ((uint32_t)ip2 << 16) | ((uint32_t)ip3 << 8) | ip4;
 * uint32_t mask = ((uint32_t)mask1 << 24) | ((uint32_t)mask2 << 16) | ((uint32_t)mask3 << 8) |
 * mask4; uint32_t gw = ((uint32_t)gw1 << 24) | ((uint32_t)gw2 << 16) | ((uint32_t)gw3 << 8) | gw4;
 *
 * status = PN_API_IOD_changeIpSuite(pnHandle, ip, mask, gw);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_SETTINGS_DOXY_GROUP
 */
uint32_t PN_API_IOD_changeIpSuite(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t                   newIpAddr,
    uint32_t                   subnetMask,
    uint32_t                   defaultGw);

/*!
 * \brief
 * Momentarily modifies the device name within the Profinet stack.
 *
 * \details
 * Changes device name and sends the new value to Profinet stack to be used. However,
 * the modified name is not retained within the NV memory, so it is lost after device restarts.
 *
 * \warning
 * Must be called only when no AR is running.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      stationName         Pointer to the string containing the new station name.
 * \param[in]      stationNameLen      Length of the string in bytes.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong or an AR is running.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_settings.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * char name[8] = "NewName";
 *
 * status = PN_API_IOD_changeDeviceName(pnHandle, name, 8);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_SETTINGS_DOXY_GROUP
 */
uint32_t PN_API_IOD_changeDeviceName(
    PN_API_IOD_Handle_t *const pnHandle,
    int8_t *stationName,
    uint16_t stationNameLen);


/**
 * @brief Device Type in SNMP. Used in I&M0 and SNMP.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      typeName            Name as string
 */
uint32_t PN_API_IOD_setDeviceType(
    const PN_API_IOD_Handle_t *const pnHandle,
    const char *const typeName);

/**
 * @brief Device Order ID. Used in I&M0 and SNMP.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      orderId             OrderID as string
 */
uint32_t PN_API_IOD_setDeviceOrderId(
    const PN_API_IOD_Handle_t *const pnHandle,
    const char *const orderId);

/**
 * @brief Serial number of device. Used in I&M0 and SNMP.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      serialNumber        Serial number as string
 */
uint32_t PN_API_IOD_setDeviceSerialNumber(
    const PN_API_IOD_Handle_t *const pnHandle,
    const char *const serialNumber);

/**
 * @brief HW revision of Device. Used in I&M0 and SNMP.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      hwRevision          Hardware revision as uint32_t
 */
uint32_t PN_API_IOD_setDeviceHWRevision(
    const PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t hwRevision);

/**
 * @brief   Device serial number. Used in I&M0 and SNMP.
 *
 * \param[in]       pnHandle    Profinet API Handle.
 * \param[in]       prefix      Prefix of the versions info; f.e. R for release
 * \param[in]       hh          Project versions info; Used in I&M0 and SNMP.
 * \param[in]       h           Increment versions info; Used in I&M0 and SNMP
 * \param[in]       l           Integration version info; Used in I&M0 and SNMP.
 * \param[in]       ll          Generation version info; Not used in I&M0 and SNMP.
 */
uint32_t PN_API_IOD_setDeviceVersionNumber(
    const PN_API_IOD_Handle_t *const pnHandle,
    const char *const prefix,
    const uint32_t hh,
    const uint32_t h,
    const uint32_t l,
    const uint32_t ll);

/*!
 * \brief
 * Callback function prototype to reset IO device to factory settings.
 *
 * \details
 * When the stack receives a "reset to factory settings" DCP request from the IO
 * controller, it calls this callback function which needs to be implemented by
 * the user application to reset the IO device settings to the factory defaults.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      rtfOption           Reset option (data to be reset).
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_settings.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbResetToFactory(PN_API_IOD_Handle_t *const pnHandle,
 * const PN_API_IOD_RtfOption_t rtfOption)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .resetToFactory = PN_APP_IOD_cbResetToFactory,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_SETTINGS_DOXY_GROUP
 */
typedef uint32_t (*PN_API_IOD_resetToFactoryCbf)(PN_API_IOD_Handle_t *const   pnHandle,
                                                 const PN_API_IOD_RtfOption_t rtfOption);

/*!
 * \brief
 * Callback function prototype to inform the user about new IP address.
 *
 * \details
 * When the stack changes the IP address of the IO device, it calls this callback
 * function which needs to be implemented by the user application to notify the
 * user about this update. The implementation can be as simple as a printf.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      ipAddr              IP address.
 * \param[in]      subnetMask          Subnet mask.
 * \param[in]      defaultGw           Default gateway.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_settings.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * void PN_APP_IOD_cbReportNewIpAddr(PN_API_IOD_Handle_t *const pnHandle,
 * const uint32_t ipAddr, const uint32_t subnetMask, const uint32_t defaultGw)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .reportNewIpAddr = PN_APP_IOD_cbReportipAddr,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_SETTINGS_DOXY_GROUP
 */
typedef void (*PN_API_IOD_reportNewIpAddrCbf)(PN_API_IOD_Handle_t *const pnHandle,
                                              const uint32_t             ipAddr,
                                              const uint32_t             subnetMask,
                                              const uint32_t             defaultGw);

extern uint8_t  PN_API_IOD_DEV_TYPE[];
extern uint32_t PN_API_IOD_DEV_TYPE_SIZE;
extern uint8_t  PN_API_IOD_DEV_ANNOTATION_ORDER_ID[];
extern uint32_t PN_API_IOD_DEV_ANNOTATION_ORDER_ID_SIZE;
extern uint8_t  PN_API_IOD_IM0_SERIAL_NUM[];
extern uint32_t PN_API_IOD_IM0_SERIAL_NUM_SIZE;
extern uint32_t PN_API_IOD_HW_REV;
extern uint8_t  PN_API_IOD_VER_PREFIX;
extern uint32_t PN_API_IOD_VER_HH;
extern uint32_t PN_API_IOD_VER_H;
extern uint32_t PN_API_IOD_VER_L;
extern uint32_t PN_API_IOD_VER_LL;

#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_SETTINGS_H
