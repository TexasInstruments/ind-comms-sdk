/*!
 *  \file pn_app_iod_data.h
 *
 *  \brief
 *  Functions and callbacks for handling Profinet data.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PN_APP_IOD_DATA_H
#define PN_APP_IOD_DATA_H

#include "pn_api_iod_types.h"

/*! \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 * @{
 */
#define AR_INFO_LIST_LEN      PN_API_IOD_NUM_OF_IO_AR + PN_API_IOD_NUM_OF_DEV_ACCESS_AR /*!< Length of AR info list. */
/** @}*/

extern uint16_t demoOutDataLen[PN_API_IOD_MAX_NUM_OF_SLOTS][PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];
extern uint16_t demoInDataLen[PN_API_IOD_MAX_NUM_OF_SLOTS][PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];

#if(defined __cplusplus)
extern "C" {
#endif

/*!
 * \brief
 * Get the index of a subslot based on its subslot number.
 *
 * \details
 * The app stored input/output data in forms of two dimensional matrices, where
 * each element in the matrix represents a subslot. This function gets the
 * index of a subslot in the matrix based on its PROFINET subslot number. If
 * subslot index is given as input parameter, it is returned as it is.
 *
 * \param[in]      slot                Slot number.
 * \param[in]      subslot             Subslot number.
 *
 * \return         subslot index as uint32_t.
 * \retval         subslot index
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 */
uint32_t PN_APP_IOD_getSubslotIndex(uint32_t slot, uint32_t subslot);

/*!
 * \brief
 * Changes the IOPS value inside demoInDataIops global array.
 *
 * \details
 * Based on slot and subslot indexes, the IOPS value of the corresponding
 * array element is changed.
 *
 * \param[in]      slot                Slot number.
 * \param[in]      subslot             Subslot index.
 * \param[in]      state               IO provider state.
 *
 * \see PN_API_IOD_setIops
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 */
void PN_APP_IOD_setIops(uint32_t slot, uint32_t subslot, PN_API_IOD_IOXS state);

/*!
 * \brief
 * Initialize IO data and status arrays.
 *
 * \details
 * Input data is initialized to a global counter, ouput data is initialized to zero,
 * while IOxS data are all set to #PN_API_IOD_STATUS_GOOD.
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 */
void PN_APP_IOD_initAppData(void);

/*!
 * \brief
 * Initiates continuous exchange of IO data between the user application and Profinet
 * stack for all ARs (RT, IRT).
 *
 * \details
 * Upon this function call, an infinite thread starts and runs in parallel to Profinet
 * stack. The purpose of this thread is to continuously call data input and output
 * trigger functions (PN_API_IOD_initiateDataRead() and PN_API_IOD_initiateDataWrite())
 * whenever the stack is done doing a single IO data exchange. Hence, the IO data
 * exchange between the user application and Profinet stack can keep going.
 *
 * \remarks
 * Without this thread, the IO data exchange between the IO device and IO controller
 * would still take place but on the stack level (isolated from the user application).
 * This means, the cyclic exchange will continue but the values of input/output data
 * won't be up to date.
 *
 * \warning
 * Initiating the thread handling the IO data exchange on the user application level
 * does not include activating this process on the stack level. This mean, the thread
 * will be blocked in "waiting state" until PN_API_IOD_activateCyclicDataExchange()
 * is called.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 */
uint32_t PN_APP_IOD_initCyclicDataExchange(PN_API_IOD_Handle_t* const pnHandle);

/*!
 * \brief
 * Callback function to inform that the cyclic data communication was started.
 *
 * \details
 * This function simply informs the user of this update.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          Session key.
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_arInDataCbf
 */
void PN_APP_IOD_cbArInData(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint16_t arNum,
    const uint16_t sessionKey);

/*!
 * \brief
 * Callback function to write input data from the physical input of a submodule to
 * the IO controller.
 *
 * \details
 * This function writes data from the global input data array to the buffer provided
 * by the stack. IOCS of the corresponding submodule is also updated while IOPS is
 * sent back as a return value.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the submodule.
 * \param[in]      bufLen              Length of data.
 * \param[in]      buffer              Pointer to data buffer to read from.
 * \param[in]      remoteIocs          Last IOCS from the IO controller, can be:
 *                                     #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         Local IOPS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         Data sent is good.
 * \retval         #PN_API_IOD_STATUS_BAD          Data sent is bad.
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_dataWriteCbf
 */
PN_API_IOD_IOXS PN_APP_IOD_cbDataWrite(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const uint32_t bufLen,
    uint8_t *const buffer,
    const PN_API_IOD_IOXS remoteIocs);

/*!
 * \brief
 * Callback function to read output data from the IO controller to the physical
 * output of a submodule. If output data is invalid, replacement substitute value
 * is used.
 *
 * \details
 * This function reads data from the buffer provided by the stack to the global
 * output data array if data is valid, else replacement substitute value is copied
 * and used instead. It also turns ON/OFF some LEDs based on the value received.
 * IOPS of the corresponding submodule is also updated while IOCS is sent back as a
 * return value.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the submodule.
 * \param[in]      bufLen              Length of data.
 * \param[in]      buffer              Pointer to data buffer to read from.
 * \param[in]      remoteIops          IOPS from the IO controller, can be:
 *                                     #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         Local IOCS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         Data received is good.
 * \retval         #PN_API_IOD_STATUS_BAD          Data received is bad.
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_dataReadCbf
 */
PN_API_IOD_IOXS PN_APP_IOD_cbDataRead(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const uint32_t bufLen,
    uint8_t *const buffer,
    const PN_API_IOD_IOXS remoteIops);

/*!
 * \brief
 * Callback function to update only IOxS for write.
 *
 * \details
 * This function sets the IOCS status of the IO controller of a specific
 * submodule to a specific value.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the submodule.
 * \param[in]      remoteIocs          IOCS of the IO controller, can be:
 *                                     #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         Local IOPS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         IOPS of the IO device is good.
 * \retval         #PN_API_IOD_STATUS_BAD          IOPS of the IO device is bad.
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_dataWriteIOxSOnlyCbf
 */
PN_API_IOD_IOXS PN_APP_IOD_cbDataWriteIOxSOnly(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const PN_API_IOD_IOXS remoteIocs);

/*!
 * \brief
 * Callback function to update only IOxS for read.
 *
 * \details
 * This function sets the IOPS status of the IO controller of a specific
 * submodule to a specific value.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical address (slot/subslot) of the submodule.
 * \param[in]      remoteIops          IOPS from the IO controller, can be:
 *                                     #PN_API_IOD_STATUS_BAD or #PN_API_IOD_STATUS_GOOD.
 *
 * \return         Local IOCS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         IOCS from the IO device is good.
 * \retval         #PN_API_IOD_STATUS_BAD          IOCS from the IO device is bad.
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_dataReadIOxSOnlyCbf
 */
PN_API_IOD_IOXS PN_APP_IOD_cbDataReadIOxSOnly(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const PN_API_IOD_IOXS remoteIops);

/*!
 * \brief
 * Callback function to read substitute values for output submodule.
 *
 * \details
 * If the corresponding IOCS status is PN_API_IOD_STATUS_GOOD, this function
 * copies output data from the global output data array to the buffer provided
 * by Profinet stack. Otherwise, a substitute value is filled for input and
 * output data.
 *
 * \warning
 * This function is only called when PN_API_IOD_INCLUDE_REC8028_8029 is set to 1.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      addr                Geographical or logical address.
 * \param[in]      bufLen              Length of the submodule output substitute data.
 * \param[in]      buffer              Pointer to submodule output substitute data.
 * \param[in,out]  substMode           Substitution mode:<br>
 *                                      0: substitute value is set to zero.<br>
 *                                      1: last output value received from controller.<br>
 *                                      2: replacement value decided by the application.
 * \param[in,out]  substActive         Substitute active flag:<br>
 *                                      0: normal operation (IOXS are GOOD).<br>
 *                                      1: otherwise (substitute value active).
 *
 * \return         IOCS as #PN_API_IOD_IOXS.
 * \retval         #PN_API_IOD_STATUS_GOOD         Data received is good.
 * \retval         #PN_API_IOD_STATUS_BAD          Data received is bad.
 *
 * \ingroup PN_APP_IOD_DATA_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_outSubmodSubstValReadCbf
 */
PN_API_IOD_IOXS PN_APP_IOD_cbOutSubmodSubstValRead(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const uint32_t bufLen,
    uint8_t *const buffer,
    uint16_t *const substMode,
    uint16_t *const substActive);

#if (defined __cplusplus)
}
#endif

#endif /* PN_APP_IOD_DATA_H */
