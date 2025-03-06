/*!
 *  \file pn_api_iod_data.h
 *
 *  \brief
 *  Functions and callback-prototypes needed for the handling of Profinet cyclic data.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#ifndef PN_API_IOD_DATA_H
#define PN_API_IOD_DATA_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*! \ingroup PN_API_IOD_DATA_DOXY_GROUP
 * @{
 */
#define PN_API_IOD_STATUS_BAD  0x00     /*!< IO provider or consumer status is "bad". */
#define PN_API_IOD_STATUS_GOOD 0x80     /*!< IO provider or consumer status is "good". */
/** @}*/

/*!
 * \brief
 * Initiates data read in Profinet stack and Performs one-time exchange of IO output
 * data between the user application and Profinet stack for all ARs (RT, IRT).
 *
 * \details
 * The stack allocates the data buffer and subsequently invokes PN_API_IOD_dataReadCbf() for
 * all submodules containing output data associated with an active IO-AR linked to an IO controller.
 * The user application is responsible for retrieving the output data received from the IO controller
 * through the stack and updating the submodule's output accordingly. The updated IO data is then
 * conveyed to the controller during the next transfer cycle, along with the exchange of IOCS and IOPS.
 *
 * \warning
 * This function operates synchronously, implying that it does not return until all PN_API_IOD_dataReadCbf()
 * calls have been executed.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_data.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Implement PN_API_IOD_dataReadCbf in the user application
 * ...
 *
 * //Register callback functions
 * ...
 *
 * status = PN_API_IOD_initiateDataRead(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
uint32_t PN_API_IOD_initiateDataRead(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Initiates data write in Profinet stack and Performs one-time exchange of IO input
 * data between the user application and Profinet stack for all ARs (RT, IRT).
 *
 * \details
 * The stack allocates the data buffer and subsequently invokes PN_API_IOD_dataWriteCbf()
 * for all submodules containing input data associated with an active IO-AR linked to an
 * IO controller. The user application is then required to read the data from the input
 * of the submodule and write it to the stack. This data is subsequently transmitted to
 * the controller in the next transfer cycle, accompanied by the exchange of IOCS and IOPS.
 *
 * \warning
 * This function operates synchronously, implying that it does not return until all PN_API_IOD_dataWriteCbf()
 * calls have been executed.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_data.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Implement PN_API_IOD_dataWriteCbf in the user application
 * ...
 *
 * //Register callback functions
 * ...
 *
 * status = PN_API_IOD_initiateDataWrite(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
uint32_t PN_API_IOD_initiateDataWrite(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Gets the Application Protocol Data Unit (APDU) status of the output data.
 *
 * \details
 * If the user application reads output data from the IO controller using standard
 * PN_API_IOD_initiateDataRead(), the corresponding 4-Byte long APDU status is
 * stored within Profinet stack. Using the function, the user application can
 * read this value which contains: Cycle counter normalized to the base value of
 * 32.25 us (bytes 0 & 1) (see note below), APDU status (see \ref PN_API_IOD_ApduStatusByteMask_t)
 * and transfer status (byte 3).
 *
 * \note
 * When the cycle counter of two consecutive PN_API_IOD_initiateDataRead() calls differs
 * by 64, the time interval between these IO data is 2 ms, calculated as (64 * 31.25 us = 2 ms).
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      apduStatus          APDU status.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_data.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status, arNum = 1, apduStatus = 0;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Implement PN_API_IOD_dataReadCbf in the user application
 * ...
 *
 * //Register callback functions
 * ...
 *
 * status = PN_API_IOD_initiateDataRead(pnHandle);
 *
 * status = PN_API_IOD_getLastApduStatus(pnHandle, arNum, &apduStatus);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
uint32_t PN_API_IOD_getLastApduStatus(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t arNum,
    uint32_t *const apduStatus);

/*!
 * \brief
 * Changes the IOPS value of submodules, that have no IO data. For example: PDev.
 *
 * \param[in]      pnHandle        Profinet API Handle
 * \param[in]      api             Application Process Identifier.
 * \param[in]      slotNum         Slot number, can not be 0.
 * \param[in]      subNum          Subslot number, which can be:<br>
 *                                 0: setIops for all submodules.<br>
 *                                 not 0: setIops for the specific module.
 * \param[in]      iops            #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_data.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status, api = 0, slotNum = 0, subNum = 0;
 * PN_API_IOD_IOXS iops = PN_API_IOD_STATUS_GOOD;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_setIops(pnHandle, api, slotnum, subnum, iops);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
uint32_t PN_API_IOD_setIops(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t api,
    const uint32_t slotNum,
    const uint32_t subNum,
    const PN_API_IOD_IOXS iops);

/*!
 * \brief
 * Activates the exchange of IO data between the user application and Profinet
 * stack for all ARs (RT, IRT).
 *
 * \details
 * This function allows the Profinet stack to cyclically send event information to the
 * user application upon completing the transmission or reception of an RTC frame. The
 * user application then should again trigger the process of reading/writing of input/output
 * data (see PN_API_IOD_initiateDataRead() and PN_API_IOD_initiateDataWrite()) in order to
 * transmit/receive another up-to-date RTC frame.
 *
 * \remarks
 * -# Without this function, the IO data exchange between the IO device and IO controller
 * would still take place but on the stack level (isolated from the user application).
 * This means, the cyclic exchange will continue but the values of input/output data
 * won't be up to date.<br>
 * -# Before calling this function, the user application needs to have a mechanism to
 * handle the event coming from Profinet stack. In demo app, this mechanism is
 * implemented as a separate thread (see PN_APP_IOD_initCyclicDataExchange()) which blocks
 * while waiting for the event, and once it arrives, an IO data read/write process is initiated.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_data.h"
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
 * //Implement PN_API_IOD_dataWriteCbf in the user application
 * //Implement PN_API_IOD_dataReadCbf in the user application
 * ...
 *
 * //Register callback functions
 * ...
 *
 * status = PN_APP_IOD_initCyclicDataExchange(pnHandle); //as an example
 *
 * status = PN_API_IOD_activateCyclicDataExchange(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
uint32_t PN_API_IOD_activateCyclicDataExchange(PN_API_IOD_Handle_t* const pnHandle);

/*!
 * \brief
 * Dectivates the exchange of IO data between the user application and Profinet
 * stack for all ARs (RT, IRT).
 *
 * \details
 * This function prevents the Profinet stack from sending event information to the
 * user application upon completing the transmission or reception of an RTC frame.
 * i.e. it stops the process initiated using PN_API_IOD_activateCyclicDataExchange().
 *
 * \remarks
 * If this function is called, the IO data exchange between the IO device and IO
 * controller would still take place but on the stack level (isolated from the user
 * application). This means, the cyclic exchange will continue but the values of
 * input/output data won't be up to date.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_data.h"
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
 * //Implement PN_API_IOD_dataWriteCbf in the user application
 * //Implement PN_API_IOD_dataReadCbf in the user application
 * ...
 *
 * //Register callback functions
 * ...
 *
 * status = PN_APP_IOD_initCyclicDataExchange(pnHandle); //as an example
 *
 * status = PN_API_IOD_activateCyclicDataExchange(pnHandle);
 *
 * ...
 *
 * status = PN_API_IOD_deactivateCyclicDataExchange(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
uint32_t PN_API_IOD_deactivateCyclicDataExchange(PN_API_IOD_Handle_t* const pnHandle);

/*!
 * \brief
 * Callback function prototype to inform the user application that the cyclic data
 * communication was started.
 *
 * \details
 * When the stack receives the first real-time IO data frame from the IO controller,
 * it calls this callback function which needs to be implemented by the user
 * application to handle this update or at least display it.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          Session key.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_data.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * uint32_t PN_APP_IOD_cbArInData(PN_API_IOD_Handle_t *const pnHandle,
 * const uint16_t arNum, const uint16_t sessionKey)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .arInData = PN_APP_IOD_cbArInData,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
typedef void (*PN_API_IOD_arInDataCbf)(PN_API_IOD_Handle_t *const pnHandle,
                                       const uint16_t             arNum,
                                       const uint16_t             sessionKey);

/*!
 * \brief
 * Callback function prototype to write output data received from the IO controller
 * to the physical output of a submodule.
 *
 * \details
 * This function is called by Profinet stack after the user application calls
 * PN_API_IOD_initiateDataRead(). IOCS must be returned to the stack as a
 * return value. This value is communicated to the IO controller in the next cycle.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      bufLen              Length of data.
 * \param[in]      buffer              Pointer to data buffer to read from.
 * \param[in]      remoteIops          IOPS from the IO controller, can be:
 *                                     #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         Local IOCS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         Data received is good.
 * \retval         #PN_API_IOD_STATUS_BAD          Data received is bad.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_data.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * PN_API_IOD_IOXS PN_APP_IOD_cbDataRead(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    PN_API_IOD_DevAddr_t *const addr,
 *    const uint32_t bufLen, uint8_t *const buffer,
 *    const PN_API_IOD_IOXS remoteIops)
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
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
typedef PN_API_IOD_IOXS (*PN_API_IOD_dataReadCbf)(PN_API_IOD_Handle_t *const  pnHandle,
                                                  PN_API_IOD_DevAddr_t *const addr,
                                                  const uint32_t              bufLen,
                                                  uint8_t *const              buffer,
                                                  const PN_API_IOD_IOXS       remoteIops);

/*!
 * \brief
 * Callback function prototype to write input data from the physical input
 * of a submodule to the IO controller.
 *
 * \details
 * This function is called by Profinet stack after the user application calls
 * PN_API_IOD_initiateDataWrite(). The user application needs to fill up the buffer.
 * IOPS must be returned to the stack as a return value. This value is communicated
 * to the IO controller.
 *
 * \warning
 * The length specified with bufLen must not be exceeded under any circumstances.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      bufLen              Length of data.
 * \param[in]      buffer              Pointer to data buffer to read from.
 * \param[in]      remoteIocs          Last IOCS from the IO controller, can be:
 *                                     #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         Local IOPS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         Data sent is good.
 * \retval         #PN_API_IOD_STATUS_BAD          Data sent is bad.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_data.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * PN_API_IOD_IOXS PN_APP_IOD_cbDataWrite(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    PN_API_IOD_DevAddr_t *const addr,
 *    const uint32_t bufLen, uint8_t *const buffer,
 *    const PN_API_IOD_IOXS remoteIocs)
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
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
typedef PN_API_IOD_IOXS (*PN_API_IOD_dataWriteCbf)(PN_API_IOD_Handle_t *const  pnHandle,
                                                   PN_API_IOD_DevAddr_t *const addr,
                                                   const uint32_t              bufLen,
                                                   uint8_t *const              buffer,
                                                   const PN_API_IOD_IOXS       remoteIocs);

/*!
 * \brief
 * Callback function prototype to update only IOxS for read.
 *
 * \details
 * This function is called by Profinet stack to notify the user application to set
 * the IOPS status of the IO controller of a specific submodule to a specific value.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      remoteIops          IOPS from the IO controller, can be:
 *                                     #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         Local IOCS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         IOCS from the IO device is good.
 * \retval         #PN_API_IOD_STATUS_BAD          IOCS from the IO device is bad.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_data.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * PN_API_IOD_IOXS PN_APP_IOD_cbDataReadIOxSOnly(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    PN_API_IOD_DevAddr_t *const addr,
 *    const PN_API_IOD_IOXS iops)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .readIOxSDataOnly = PN_APP_IOD_cbDataReadIOxSOnly,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
typedef PN_API_IOD_IOXS (*PN_API_IOD_dataReadIOxSOnlyCbf)(PN_API_IOD_Handle_t *const  pnHandle,
                                                          PN_API_IOD_DevAddr_t *const addr,
                                                          const PN_API_IOD_IOXS       remoteIops);

/*!
 * \brief
 * Callback function prototype to update only IOxS for write.
 *
 * \details
 * This function is called by Profinet stack to notify the user application to set
 * the IOCS status of the IO controller of a specific submodule to a specific value.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      remoteIocs          IOCS of the IO controller, can be:
 *                                     #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         Local IOPS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         IOPS of the IO device is good.
 * \retval         #PN_API_IOD_STATUS_BAD          IOPS of the IO device is bad.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_callbacks.h"
 * #include "pn_api_iod_data.h"
 * #include "pn_api_iod_startup.h"
 *
 * //Implement the callback function in the user application
 * PN_API_IOD_IOXS PN_APP_IOD_cbDataWriteIOxSOnly(
 *    PN_API_IOD_Handle_t *const pnHandle,
 *    PN_API_IOD_DevAddr_t *const addr,
 *    const PN_API_IOD_IOXS remoteIocs)
 * {...}
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * //Store callback functions implemented by the user application
 * PN_API_IOD_Callbacks_t callbacks = {
 *     .writeIOxSDataOnly = PN_APP_IOD_cbDataWriteIOxSOnly,
 *     ...}
 *
 * //Register callback function
 * status = PN_API_IOD_registerCallbacks(pnHandle, &callbacks);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DATA_DOXY_GROUP
 */
typedef PN_API_IOD_IOXS (*PN_API_IOD_dataWriteIOxSOnlyCbf)(PN_API_IOD_Handle_t *const  pnHandle,
                                                           PN_API_IOD_DevAddr_t *const addr,
                                                           const PN_API_IOD_IOXS       remoteIocs);

#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_DATA_H
