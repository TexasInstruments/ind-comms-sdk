/*!
 *  \file pn_api_iod_bsp.h
 *
 *  \brief
 *  Functions and callback-prototypes needed for the board support package like memory and LED control.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#ifndef PN_API_IOD_BSP_H
#define PN_API_IOD_BSP_H

#include "pn_api_iod_types.h"

#if (defined __cplusplus)
extern "C" {
#endif

/*!
 * \brief
 * Callback function prototype to request NV data storage.
 *
 * \details
 * This function is called by Profinet stack to transfer all received PDev records
 * to the user application to store them in the NV memory. The user application needs
 * not to change or interpret the data. It just needs to store it, and then during the
 * next startup, restore it using PN_API_IOD_restoreRemaData() and transfer it to the stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      type                type of content to be stored
 * \param[in]      instance            instance of type
 * \param[in]      memSize             Size of the PDev buffer that needs to be
 *                                     written to NV memory.
 * \param[in]      srcMem              Pointer to the buffer holding PDev data.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK                  Success.
 * \retval         #PN_API_ERR_PARAM           Invalid parameter.
 * \retval         #PN_API_ERR_FLASH_STORE     NV memory write operation failed.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_bsp.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbStoreRemaMem(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    PN_API_IOD_RemaDataType_t type, sconst uint32_t instance,
 *    uint32_t memSize, uint8_t *srcMem)
 * {
 *     PN_API_IOD_dataStoreComplete(pnHandle, memSize);
 *     ...
 * }
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .storeRemaMem = PN_APP_IOD_cbStoreRemaMem,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_BSP_DOXY_GROUP
 */
typedef uint32_t (*PN_API_IOD_storeRemaMemCbf)(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_RemaDataType_t  type,
    const uint32_t             instance,
    const uint32_t             memSize,
    uint8_t *const             srcMem);

/*!
 * \brief
 * Callback function prototype to request stored NV data.
 *
 * \details
 * This function is called by Profinet stack to request PDev record data block from
 * to the user application. This function is called after startup and insertion of
 * PDev modules.
 *
 * \warning
 * The actual address of the rema data should be given back through "mem" parameter.
 * Rema data must be static and remain valid after the call of this function is completed.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      type                type of content to be stored
 * \param[in]      instance            instance of type
 * \param[out]     destMem             Pointer to the buffer holding PDev data.
 * \param[in]      memSize             Size of the PDev buffer that needs to be
 *                                     restored from NV memory.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_bsp.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbRestoreRemaMem(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    PN_API_IOD_RemaDataType_t type, const uint32_t instance,
 *    uint8_t *const *destMem, uint32_t *const memSize)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .restoreRemaMem = PN_APP_IOD_cbRestoreRemaMem,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_BSP_DOXY_GROUP
 */
typedef uint32_t (*PN_API_IOD_restoreRemaMemCbf)(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_RemaDataType_t  type,
    const uint32_t             instance,
    uint8_t **const            destMem,
    uint32_t *const            memSize);

/*!
 * \brief
 * Callback function prototype to request to clear data from NV memory.
 *
 * \details
 * This function is called by Profinet stack to request from the user
 * application to free Profinet-related data from the NV memory.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      destMem             Pointer to a buffer where the data to be cleared.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_bsp.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbFreeRemaMem(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    uint8_t *const destMem)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .freeRemaMem = PN_APP_IOD_cbFreeRemaMem,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_BSP_DOXY_GROUP
 */
typedef uint32_t (
    *PN_API_IOD_freeRemaMemCbf)(PN_API_IOD_Handle_t *const pnHandle, uint8_t *const destMem);

/*!
 * \brief
 * Non volatile store completed
 *
 * \details
 * This function needs to be called if a write process has finished.
 * It can be called directly in the store callback or later if the writing takes time.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      lenWritten          number of bytes written to NV memory
 *
 *
 * \ingroup PN_API_IOD_BSP_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_storeRemaMemCbf
 */
void PN_API_IOD_dataStoreComplete(PN_API_IOD_Handle_t *pnHandle, uint32_t lenWritten);

/*!
 * \brief
 * Callback function prototype to request changing of LED states
 *
 * \details
 * This function is called by Profinet stack if a LED need to change its state.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      led                 Port number (currently not used).
 * \param[in]      state               Blinking frequency in Hz.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_bsp.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbSetLed(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    PN_API_IOD_Led_t led, bool state)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .setLed = PN_APP_IOD_cbSetLed,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_BSP_DOXY_GROUP
 */
typedef uint32_t (
    *PN_API_IOD_setLedCbf)(PN_API_IOD_Handle_t *pnHandle, PN_API_IOD_Led_t led, bool state);

/*!
 * \brief
 * Callback function prototype to request the start of LED blinking.
 *
 * \details
 * This function is called by Profinet stack to request from the user
 * application to start blinking the LED with a specific frequency. This
 * usually happens in response to LED blinking DCP requests.
 *
 * \warning
 * For now, LED blinking lasts for 3 seconds only. Afterwards, the stack
 * automatically asks the user application to stop LED blinking using
 * PN_API_IOD_stopLedBlinkCbf().
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      portNum             Port number (currently not used).
 * \param[in]      frequency           Blinking frequency in Hz.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_bsp.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbStartLedBlink(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    const uint32_t portNum, const uint32_t frequency)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .startLedBlink = PN_APP_IOD_cbStartLedBlink,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_BSP_DOXY_GROUP
 */
typedef uint32_t (*PN_API_IOD_startLedBlinkCbf)(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t             portNum,
    const uint32_t             frequency);

/*!
 * \brief
 * Callback function prototype to request the stop of LED blinking.
 *
 * \details
 * This function is called by Profinet stack to request from the user
 * application to stop blinking the LED. In order to achieve this, the
 * user application may call PN_API_IOD_ledBlink() with frequency set
 * to zero.
 *
 * \warning
 * For now, PN_API_IOD_stopLedBlinkCbf() is automatically called by the
 * stack 3 seconds after PN_API_IOD_startLedBlinkCbf() is called.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      portNum             Port number (currently not used).
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_bsp.h"
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbStopLedBlink(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    const uint32_t portNum)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .stopLedBlink = PN_APP_IOD_cbStopLedBlink,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_BSP_DOXY_GROUP
 */
typedef uint32_t (
    *PN_API_IOD_stopLedBlinkCbf)(PN_API_IOD_Handle_t *const pnHandle, const uint32_t portNum);

/*!
 * \brief
 * Callback function prototype to update app cycle Timer
 *
 * \details
 * This function is called by Profinet stack to update the cycletime
 * of the app cycle timer. The timer calls PND_EDDP_IrqSendClock in
 * this configured interval.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      timeNs              new timer value in nanoseconds
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \ingroup PN_API_IOD_BSP_DOXY_GROUP
 */
typedef uint32_t (
    *PN_API_IOD_updateAppCycleTimerCbf)(PN_API_IOD_Handle_t *const pnHandle, const uint32_t timeNs);

#if (defined __cplusplus)
}
#endif

#endif // PN_API_IOD_BSP_H
