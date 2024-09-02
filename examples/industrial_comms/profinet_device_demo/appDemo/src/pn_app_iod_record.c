/*!
 * \file pn_app_iod_record.c
 *
 * \brief
 * Functions and callback for handling record data read and write.
 *
 * \author
 * KUNBUS GmbH
 *
 * \copyright
 * Copyright (c) 2023, KUNBUS GmbH<br /><br />
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024 KUNBUS GmbH.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * <ol>
 * <li>Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer./<li>
 * <li>Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.</li>
 * <li>Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.</li>
 * </ol>
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#include "pn_app_iod_record.h"

#include "osal.h"
#include "osal_error.h"

#include "pn_api_iod_device.h"
#include "pn_api_iod_error.h"
#include "pn_api_iod_handle.h"
#include "pn_api_iod_record.h"
#include "pn_api_iod_startup.h"

#include "pn_app_iod_data.h"
#include "pn_app_iod_isoch.h"


/*!
 * \brief A global variable designed to manage user record parameters as specified in the GSDML file.
 */
PN_APP_IOD_userRec_t userRecList[PN_API_IOD_MAX_NUM_OF_SUBSLOTS + 1U];

/*!
 * \brief A global variable to hold the parameters of the actual asynchronous record
 * read request currently in progress.
 */
PN_APP_IOD_asyncReqParams_t asyncRecReadParams;

/*!
 * \brief A global variable to hold the parameters of the actual asynchronous record
 * write request currently in progress.
 */
PN_APP_IOD_asyncReqParams_t asyncRecWriteParams;

/*!
 * \brief A static variable used to activate/deactivate asynchronous read mode.
 */
static bool asyncRecReadMode;

/*!
 * \brief A static variable used to activate/deactivate asynchronous write mode.
 */
static bool asyncRecWriteMode;

/*!
 * \brief A global variable used as a handle for asynchronous read request.
 */
void *asyncRecReadHandle;

/*!
 * \brief A global variable used as a handle for asynchronous write request.
 */
void* asyncRecWriteHandle;

/* \todo Solve workaround for the unit tests - conflicting types for ‘OsNtohl’ */
#ifndef UNIT_TEST
extern uint16_t OsHtons(uint16_t);
extern uint32_t OsNtohl(uint32_t);
extern uint32_t OsNtohs(uint32_t);
#endif

void PN_APP_IOD_initRecordInfo(bool asyncRead, bool asyncWrite)
{
    /* Init userRecList */
    OSAL_MEMORY_memset(userRecList, 0, sizeof(userRecList));

    /* Set asynchronous mode */
    asyncRecReadMode = asyncRead;
    asyncRecWriteMode = asyncWrite;

    /* Set asynchronous handles */
    asyncRecReadHandle = NULL;
    asyncRecWriteHandle = NULL;

    /* Init asynchronous record read/write buffers and parameters */
    OSAL_MEMORY_memset(&asyncRecReadParams, 0, sizeof(asyncRecReadParams));
    OSAL_MEMORY_memset(&asyncRecWriteParams, 0, sizeof(asyncRecWriteParams));
}

void PN_APP_IOD_clearRecordListElement(uint16_t index)
{
    if ((PN_API_IOD_MAX_NUM_OF_SUBSLOTS + 1U) > index)
    {
        OSAL_MEMORY_memset(&userRecList[index], 0, sizeof(PN_APP_IOD_userRec_t));
    }
}

/*!
 * \brief
 * Verify timing values of the received isochronous data record.
 *
 * \details
 * This function checks if the timing values of the received isochronous data
 * record are within the permitted ranges defined by the application and GSDML.
 *
 * \param[in]      cycleFactor         Cycle factor.
 * \param[in]      dataCycleTime       Data exchange cycle time.
 * \param[in]      ioInputTime         Time to get/update an input submodule.
 * \param[in]      ioOutputTime        Time to get/update an output submodule.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_verifyTimingValues(
    uint16_t cycleFactor,
    uint16_t dataCycleTime,
    uint32_t ioInputTime,
    uint32_t ioOutputTime)
{
    uint32_t status = PN_API_NOT_OK;
    uint32_t cycleTime = (uint32_t)(dataCycleTime * 31250U);

    if ((0U < cycleFactor) &&
        (0U < dataCycleTime) &&
        (0U < ioInputTime) &&
        (0U < ioOutputTime) &&
        (ISO_T_DC_MIN <= cycleTime) &&
        (ISO_T_DC_MAX >= cycleTime) &&
        (ISO_T_IO_IN_MIN <= ioInputTime) &&
        (ISO_T_DC_MAX >= ioInputTime) &&
        (ISO_T_IO_OUT_MIN <= ioOutputTime) &&
        (ISO_T_DC_MAX >= ioOutputTime) &&
        (ioInputTime <= cycleTime) &&
        (ioOutputTime <= cycleTime))
    {
        status = PN_API_OK;
    }

    return status;
}

/*!
 * \brief
 * Handle record read requested for isochronous data.
 *
 * \details
 * This function is called upon receiving a record read request with index
 * REC_INDEX_ISO_DATA. It simply reads the corresponding record array element
 * and places it into the buffer provided by the stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      bufLen              Length of data to read.
 * \param[in]      buffer              Buffer in which data will be copied.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_isoRecordReadHandler(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t *bufLen,
    uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_RealSubmod_t realSubmod;
    PN_APP_IOD_userRec_t *userRec = NULL;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != addr) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState))
    {
        OSAL_MEMORY_memset(&realSubmod, 0, sizeof(realSubmod));

        uint32_t slotIndex = addr->address.geo.slot;
        uint32_t subslotIndex = addr->address.geo.subslot;

        status = PN_API_IOD_getRealSubmod(pnHandle, &realSubmod, api, slotIndex, subslotIndex);

        if ((PN_API_OK != status) || ((PN_API_IOD_MAX_NUM_OF_SUBSLOTS + 1U) < realSubmod.index))
        {
            OSAL_MEMORY_memset(buffer, 0, sizeof(PN_APP_IOD_isoDataBlock_t));
            *bufLen = 0;
            errState->errCode1 = ERR_CODE1_RESOURCE_UNAVAILBALE;
        }
        else if ((0 >= slotIndex) ||
            (PN_API_IOD_MAX_NUM_OF_SLOTS <= slotIndex) ||
            (1 >= subslotIndex) ||
            (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT < subslotIndex))
        {
            errState->errCode1 = ERR_CODE1_INVALID_INDEX;
        }
        else if (sizeof(PN_APP_IOD_isoDataBlock_t) > *bufLen)
        {
            errState->errCode1 = ERR_CODE1_READ_CONSTRAINT_CONFLICT;
        }
        else
        {
            userRec = &userRecList[realSubmod.index];

            if ((0 < userRec->dataSize) && (REC_INDEX_ISO_DATA == userRec->index))
            {
                *bufLen = sizeof(PN_APP_IOD_isoDataBlock_t);
                PN_APP_IOD_isoDataBlock_t *isoRecToSend = (PN_APP_IOD_isoDataBlock_t*)buffer;
                PN_APP_IOD_isoDataBlock_t *isoRecLocal = (PN_APP_IOD_isoDataBlock_t*)&userRec->dataByteArray[0];

                isoRecToSend->blockHeader.type = (uint16_t)OsHtons(isoRecLocal->blockHeader.type);
                isoRecToSend->blockHeader.len = (uint16_t)OsHtons(isoRecLocal->blockHeader.len);
                isoRecToSend->blockHeader.version = (uint16_t)OsHtons(isoRecLocal->blockHeader.version);
                isoRecToSend->slot = (uint16_t)OsHtons(isoRecLocal->slot);
                isoRecToSend->subslot = (uint16_t)OsHtons(isoRecLocal->subslot);
                isoRecToSend->cycleFactor = (uint16_t)OsHtons(isoRecLocal->cycleFactor);
                isoRecToSend->dataCycleTime = (uint16_t)OsHtons(isoRecLocal->dataCycleTime);
                isoRecToSend->ioInputTime = (uint32_t)OsNtohl(isoRecLocal->ioInputTime);
                isoRecToSend->ioOutputTime = (uint32_t)OsNtohl(isoRecLocal->ioOutputTime);
                isoRecToSend->ioInputValidTime = (uint32_t)OsNtohl(isoRecLocal->ioInputValidTime);
                isoRecToSend->ioOutputValidTime = (uint32_t)OsNtohl(isoRecLocal->ioOutputValidTime);
            }
            else
            {
                OSAL_MEMORY_memset(buffer, 0, sizeof(PN_APP_IOD_isoDataBlock_t));
                *bufLen = 0;

                PN_API_IOD_ioBlockHeader_t *isoBlockHeader = (PN_API_IOD_ioBlockHeader_t*)buffer;

                isoBlockHeader->type = (uint16_t)OsHtons(BLOCK_HEADER_TYPE_ISO);
                isoBlockHeader->version = (uint16_t)OsHtons(BLOCK_HEADER_VERSION_ISO);
                isoBlockHeader->len = 0U;
            }

            errState->errCode1 = ERR_CODE1_NO_ERROR;
            status = PN_API_OK;
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
}

/*!
 * \brief
 * Setup ISOCH Events
 *
 * \details
* This function setup hardware events for TI/TO. It is required to initialize the event
* with the lower delay time first. Before changing the event, all events need to be cleared.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      ti                  Input Event offset in ns
 * \param[in]      to                  Output Event offset in ns
 * \param[in]      tcyc                Cycle Time in ns
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
void PN_APP_IOD_setupTiTo(PN_API_IOD_Handle_t *const pnHandle, uint32_t ti, uint32_t to, uint32_t tcyc)
{
    static PN_API_IOD_IsoObjHndl_t pnIsomHandlTi = NULL;
    static PN_API_IOD_IsoObjHndl_t pnIsomHandlTo = NULL;

    // delete old handles
    if (pnIsomHandlTi != NULL)
    {
        PN_API_IOD_freeIsoObj(pnHandle, pnIsomHandlTi);
        pnIsomHandlTi = NULL;
    }
    if (pnIsomHandlTo != NULL)
    {
        PN_API_IOD_freeIsoObj(pnHandle, pnIsomHandlTo);
        pnIsomHandlTo = NULL;
    }

    // create new handles
    PN_API_IOD_activateIsoGpioObj(pnHandle, to, 40000, &pnIsomHandlTo);
    PN_API_IOD_activateIsoGpioObj(pnHandle, tcyc - ti, 40000, &pnIsomHandlTi);
}

/*!
 * \brief
 * Handle record write requested for isochronous data.
 *
 * \details
 * This function is called upon receiving a record write request with index
 * REC_INDEX_ISO_DATA. It simply reads the data from the buffer provided by
 * the stack and places it into the corresponding record array element.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      bufLen              Length of data to write.
 * \param[in]      buffer              Buffer containing data to be written.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_isoRecordWriteHandler(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    const PN_API_IOD_DevAddr_t *const addr,
    const uint32_t *bufLen,
    const uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_RealSubmod_t realSubmod;
    PN_APP_IOD_userRec_t *userRec = NULL;
    PN_APP_IOD_isoDataBlock_t *isoRecReceived;
    PN_APP_IOD_isoDataBlock_t isoRecLocal;
    uint16_t isoDataSize = 0;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != addr) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState))
    {
        isoDataSize = sizeof(PN_APP_IOD_isoDataBlock_t) - sizeof(uint16_t) - sizeof(uint16_t);
        isoRecReceived = (PN_APP_IOD_isoDataBlock_t*)buffer;

        isoRecLocal.blockHeader.type = (uint16_t)OsNtohs(isoRecReceived->blockHeader.type);
        isoRecLocal.blockHeader.len = (uint16_t)OsNtohs(isoRecReceived->blockHeader.len);
        isoRecLocal.blockHeader.version = (uint16_t)OsNtohs(isoRecReceived->blockHeader.version);

        OSAL_MEMORY_memset(&realSubmod, 0, sizeof(realSubmod));

        uint32_t slotIndex = addr->address.geo.slot;
        uint32_t subslotIndex = addr->address.geo.subslot;

        status = PN_API_IOD_getRealSubmod(pnHandle, &realSubmod, api, slotIndex, subslotIndex);
        if ((PN_API_OK != status) || ((PN_API_IOD_MAX_NUM_OF_SUBSLOTS + 1U) < realSubmod.index))
        {
            errState->errCode1 = ERR_CODE1_RESOURCE_UNAVAILBALE;
        }
        else if ((0 >= slotIndex) ||
            (PN_API_IOD_MAX_NUM_OF_SLOTS <= slotIndex) ||
            (1 >= subslotIndex) ||
            (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT < subslotIndex))
        {
            errState->errCode1 = ERR_CODE1_INVALID_SLOT_SUBSLOT;
        }
        else if (sizeof(PN_APP_IOD_isoDataBlock_t) > *bufLen)
        {
            errState->errCode1 = ERR_CODE1_WRITE_LENGTH;
        }
        else if ((BLOCK_HEADER_TYPE_ISO != isoRecLocal.blockHeader.type) ||
            (BLOCK_HEADER_VERSION_ISO != isoRecLocal.blockHeader.version) ||
            (isoDataSize > isoRecLocal.blockHeader.len))
        {
            errState->errCode1 = ERR_CODE1_INVALID_TYPE;
        }

        if (ERR_CODE1_NO_ERROR == errState->errCode1)
        {
            isoRecLocal.slot = (uint16_t)OsNtohs(isoRecReceived->slot);
            isoRecLocal.subslot = (uint16_t)OsNtohs(isoRecReceived->subslot);
            isoRecLocal.dataCycleTime = (uint16_t)OsNtohs(isoRecReceived->dataCycleTime);
            isoRecLocal.ioInputTime = (uint32_t)OsNtohl(isoRecReceived->ioInputTime);
            isoRecLocal.ioOutputTime = (uint32_t)OsNtohl(isoRecReceived->ioOutputTime);
            isoRecLocal.ioInputValidTime = (uint32_t)OsNtohl(isoRecReceived->ioInputValidTime);
            isoRecLocal.ioOutputValidTime = (uint32_t)OsNtohl(isoRecReceived->ioOutputValidTime);
            isoRecLocal.cycleFactor = (uint16_t)OsNtohs(isoRecReceived->cycleFactor);

            if (PN_API_NOT_OK == PN_APP_IOD_verifyTimingValues(
                isoRecLocal.cycleFactor,
                isoRecLocal.dataCycleTime,
                isoRecLocal.ioInputTime,
                isoRecLocal.ioOutputTime))
            {
                errState->errCode1 = ERR_CODE1_INVALID_PARAMETER;
            }
        }

        if (ERR_CODE1_NO_ERROR == errState->errCode1)
        {
            userRec = &userRecList[realSubmod.index];

            OSAL_MEMORY_memset(userRec, 0, sizeof(PN_APP_IOD_userRec_t));
            userRec->periphRealCfgInd = realSubmod.index;
            userRec->index = REC_INDEX_ISO_DATA;
            userRec->dataSize = sizeof(PN_APP_IOD_isoDataBlock_t);

            OSAL_MEMORY_memset(&userRec->dataByteArray[0], 0, sizeof(userRec->dataByteArray));
            OSAL_MEMORY_memcpy(&userRec->dataByteArray[0], &isoRecLocal, userRec->dataSize);

            // setup TI/TO for slot 1
            if (slotIndex == 1)
            {
                PN_APP_IOD_setupTiTo(
                    pnHandle,
                    isoRecLocal.ioInputTime,
                    isoRecLocal.ioOutputTime,
                    isoRecLocal.dataCycleTime * 31250U);
            }

            status = PN_API_OK;
        }
        else
        {
            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
}

/* pragma to disable -Wunused-function warning */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"

/*!
 * \brief
 * Handle record read requested for IM0 data.
 *
 * \details
 * This function is called upon receiving a record read request with index REC_INDEX_IM0.
 *
 * \warning
 * When PN_API_IOD_INCLUDE_IM0_4 is set to 1, Profinet stack takes care of all IMx record
 * read requests. This means, PN_APP_IOD_cbRecordRead function is never called by the stack
 * for requests with indexes between REC_INDEX_IM0 and REC_INDEX_IM5. As a reasult, this
 * function is never called as well. It's only called when PN_API_IOD_INCLUDE_IM0_4 is set to 0.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      bufLen              Length of data to read.
 * \param[in]      buffer              Buffer in which data will be copied.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_im0RecordReadHandler(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t *bufLen,
    uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;

    OSALUNREF_PARM(api);

#if (0 == PN_API_IOD_USED_WITH_XHIF_HOST)
    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != addr) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState))
    {
        PN_APP_IOD_im0DataBlock_t *im0DataBlock = (PN_APP_IOD_im0DataBlock_t*) buffer;

        if (*bufLen < sizeof(PN_APP_IOD_im0DataBlock_t))
        {
            errState->errCode1 = ERR_CODE1_READ_CONSTRAINT_CONFLICT;

            status = PN_API_NOT_OK;
        }
        else
        {
            uint32_t slotIndex = addr->address.geo.slot;
            uint32_t subslotIndex = addr->address.geo.subslot;

            *bufLen = sizeof(PN_APP_IOD_im0DataBlock_t);

            /* fill IM0 BlockHeader */
            im0DataBlock->blockHeader.type = OsHtons(BLOCK_HEADER_TYPE_IM0);
            im0DataBlock->blockHeader.len = OsHtons(sizeof(PN_APP_IOD_im0DataBlock_t) -
                sizeof(PN_API_IOD_ioBlockHeader_t) + 2);
            im0DataBlock->blockHeader.version = OsHtons(BLOCK_HEADER_VERSION);

            /* Fill IM0 vendor ID */
            im0DataBlock->im0Data.vendorId = OsHtons(PN_API_IOD_VENDOR_ID);

            /* Fill IM0 order ID and serial number */
            OSAL_MEMORY_memcpy(
                im0DataBlock->im0Data.orderId,
                PN_API_IOD_DEV_ANNOTATION_ORDER_ID,
                sizeof(im0DataBlock->im0Data.orderId));
            OSAL_MEMORY_memcpy(im0DataBlock->im0Data.serialNum,
                PN_API_IOD_IM0_SERIAL_NUM,
                sizeof(im0DataBlock->im0Data.serialNum));

            /* Fill IM0 hardware and software revision */
            im0DataBlock->im0Data.hwRevision = OsHtons(PN_API_IOD_HW_REV);
            im0DataBlock->im0Data.swRevision.prefix = (uint8_t)PN_API_IOD_VER_PREFIX;
            im0DataBlock->im0Data.swRevision.funcEnhancement = (uint8_t)PN_API_IOD_VER_HH;
            im0DataBlock->im0Data.swRevision.bugFix = (uint8_t)PN_API_IOD_VER_H;
            im0DataBlock->im0Data.swRevision.internalChange = (uint8_t)PN_API_IOD_VER_L;

            /* Fill the rest of IM0 data*/
            im0DataBlock->im0Data.revisionCnt = OsHtons(PN_API_IOD_VER_LL);
            im0DataBlock->im0Data.profileId = OsHtons(IM0_PROFILE_ID);
            im0DataBlock->im0Data.profileSpecType = OsHtons(IM0_PROFILE_SPECIFIC_TYPE_IO_MOD);
            im0DataBlock->im0Data.versionMajor = IM0_VERSION_MAJOR;
            im0DataBlock->im0Data.versionMinor = IM0_VERSION_MINOR;

            /* Is further IM1_4 supported */
            if ((0 == slotIndex) && (1 == subslotIndex))
            {
                im0DataBlock->im0Data.imXSupported = OsHtons(IM0_DAP_DATA);

                /* Is DAP? override IM0 profileSpecType */
                im0DataBlock->im0Data.profileSpecType = OsHtons(IM0_PROFILE_SPECIFIC_TYPE_IF_MOD);
            }
            else
            {
                im0DataBlock->im0Data.imXSupported = OsHtons(IM0_NO_DAP_DATA);
            }

            errState->errCode1 = ERR_CODE1_NO_ERROR;
            status = PN_API_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
#else
#error "The current firmware version does not support XHIF memory interface!"
#endif
}

/*!
 * \brief
 * Handle record write requested for IM0 data.
 *
 * \details
 * This function is called upon receiving a record write request with index REC_INDEX_IM0.
 * Since IM0 data is read only, the function should respond with the coresponding error code.
 *
 * \warning
 * When PN_API_IOD_INCLUDE_IM0_4 is set to 1, Profinet stack takes care of all IMx record
 * write requests. This means, PN_APP_IOD_cbRecordWrite function is never called by the stack
 * for requests with indexes between REC_INDEX_IM0 and REC_INDEX_IM5. As a reasult, this
 * function is never called as well. It's only called when PN_API_IOD_INCLUDE_IM0_4 is set to 0.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      bufLen              Length of data to write.
 * \param[in]      buffer              Buffer containing data to be written.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         PN_API_NOT_OK       Something went wrong.
 * \retval         PN_API_ERR_PARAM    Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_im0RecordWriteHandler(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    const PN_API_IOD_DevAddr_t *const addr,
    const uint32_t *bufLen,
    const uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;

    OSALUNREF_PARM(api);
    OSALUNREF_PARM(addr);
    OSALUNREF_PARM(bufLen);
    OSALUNREF_PARM(buffer);

#if (0 == PN_API_IOD_USED_WITH_XHIF_HOST)
    if (PN_API_IOD_isHandleValid(pnHandle) && (NULL != errState))
    {
        /* IM0 is only readable */
        errState->errCode1 = ERR_CODE1_ACCESS_DENIED;

        status = PN_API_NOT_OK;
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
#else
#error "The current firmware version does not support XHIF memory interface!"
#endif
}

#pragma clang diagnostic pop

/*!
 * \brief
 * Handle record read requested for user data with index REC_INDEX_USER_1000.
 *
 * \details
 * This function is called upon receiving a record read request with index
 * REC_INDEX_USER_1000. It simply reads the corresponding record array element
 * and places it into the buffer provided by the stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      bufLen              Length of data to read.
 * \param[in]      buffer              Buffer in which data will be copied.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_userRecordReadHandler1000(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t *bufLen,
    uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_RealSubmod_t realSubmod;
    const PN_APP_IOD_userRec_t *userRec = NULL;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != addr) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState))
    {
        OSAL_MEMORY_memset(&realSubmod, 0, sizeof(realSubmod));

        uint32_t slotIndex = addr->address.geo.slot;
        uint32_t subslotIndex = addr->address.geo.subslot;

        status = PN_API_IOD_getRealSubmod(pnHandle, &realSubmod, api, slotIndex, subslotIndex);

        if ((PN_API_OK != status) || ((PN_API_IOD_MAX_NUM_OF_SUBSLOTS + 1U) < realSubmod.index))
        {
            OSAL_MEMORY_memset(buffer, 0, USER_MOD_REC_LEN);
            *bufLen = 0;

            errState->errCode1 = ERR_CODE1_RESOURCE_UNAVAILBALE;
        }

         /* This submod item is fixed on IO subslot 1 (see GSDML file for more info) */
        else if ((0 >= slotIndex) || (PN_API_IOD_MAX_NUM_OF_SLOTS <= slotIndex) || (1 != subslotIndex))
        {
            errState->errCode1 = ERR_CODE1_INVALID_SLOT_SUBSLOT;
        }
        else if (USER_MOD_REC_LEN > *bufLen)
        {
            errState->errCode1 = ERR_CODE1_READ_CONSTRAINT_CONFLICT;
        }
        else
        {
            userRec = &userRecList[realSubmod.index];

            if ((0 < userRec->dataSize) && (REC_INDEX_USER_1000 == userRec->index))
            {
                OSAL_MEMORY_memcpy(buffer, &userRec->dataByteArray[0], userRec->dataSize);
                *bufLen = userRec->dataSize;

                status = PN_API_OK;
            }
            else
            {
                OSAL_MEMORY_memset(buffer, 0, USER_MOD_REC_LEN);
                *bufLen = USER_MOD_REC_LEN;

                errState->errCode1 = ERR_CODE1_RESOURCE_UNAVAILBALE;
            }
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
}

/*!
 * \brief
 * Handle record write requested for user data with index REC_INDEX_USER_1000.
 *
 * \details
 * This function is called upon receiving a record write request with index
 * REC_INDEX_USER_1000. It simply reads the data from the buffer provided by
 * the stack and places it into the corresponding record array element.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      bufLen              Length of data to write.
 * \param[in]      buffer              Buffer containing data to be written.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_userRecordWriteHandler1000(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t recordIndex,
    const uint32_t *bufLen,
    const uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_RealSubmod_t realSubmod;
    PN_APP_IOD_userRec_t *userRec = NULL;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != addr) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState))
    {
        OSAL_MEMORY_memset(&realSubmod, 0, sizeof(realSubmod));

        uint32_t slotIndex = addr->address.geo.slot;
        uint32_t subslotIndex = addr->address.geo.subslot;

        status = PN_API_IOD_getRealSubmod(pnHandle, &realSubmod, api, slotIndex, subslotIndex);
        if ((PN_API_OK != status) || ((PN_API_IOD_MAX_NUM_OF_SUBSLOTS + 1U) < realSubmod.index))
        {
            errState->errCode1 = ERR_CODE1_RESOURCE_UNAVAILBALE;
        }

        /* This submod item is fixed on IO subslot 1 (see GSDML file for more info) */
        else if ((0 >= slotIndex) || (PN_API_IOD_MAX_NUM_OF_SLOTS <= slotIndex) || (1 != subslotIndex))
        {
            errState->errCode1 = ERR_CODE1_INVALID_SLOT_SUBSLOT;
        }
        else if (USER_MOD_REC_LEN > *bufLen)
        {
            errState->errCode1 = ERR_CODE1_WRITE_LENGTH;
        }
        else
        {
            userRec = &userRecList[realSubmod.index];

            OSAL_MEMORY_memset(userRec, 0, sizeof(PN_APP_IOD_userRec_t));
            userRec->periphRealCfgInd = realSubmod.index;
            userRec->index = recordIndex;
            userRec->dataSize = *bufLen;
            OSAL_MEMORY_memcpy(&userRec->dataByteArray[0], buffer, *bufLen);

            errState->errCode1 = ERR_CODE1_NO_ERROR;
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
}

/*!
 * \brief
 * Handle user-specific record data read requests.
 *
 * \details
 * This fucntion is called upon receiving a record read request with index between
 * REC_INDEX_USER_START and REC_INDEX_USER_END. In this example, the currently expected
 * user record data is REC_INDEX_USER_1000, which requests the IO device to read a
 * 16 byte buffer from one of the IO submodules plugged on subslot 1.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          AR session key.
 * \param[in]      sequenceNum         Sequence number.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      recordIndex         Index of the record.
 * \param[in]      bufLen              Length of data to read.
 * \param[in]      buffer              Buffer in which data will be copied.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_userRecordReadHandler(
    PN_API_IOD_Handle_t* const pnHandle,
    uint32_t api,
    uint16_t arNum,
    uint16_t sessionKey,
    uint32_t sequenceNum,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t recordIndex,
    uint32_t *bufLen,
    uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;

    OSALUNREF_PARM(sessionKey);
    OSALUNREF_PARM(sequenceNum);
    OSALUNREF_PARM(arNum);

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != addr) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState))
    {
        status = PN_API_OK;

        switch (recordIndex)
        {
        case REC_INDEX_USER_1000:
            status = PN_APP_IOD_userRecordReadHandler1000(pnHandle, api, addr, bufLen, buffer, errState);
            break;
        default:
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
}

/*!
 * \brief
 * Handle user-specific record data write requests.
 *
 * \details
 * This fucntion is called upon receiving a record write request with index between
 * REC_INDEX_USER_START and REC_INDEX_USER_END. In this example, the currently expected
 * user record data is REC_INDEX_USER_1000, which requests the IO device to write a
 * 16 byte buffer into one of the IO submodules plugged on subslot 1.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          AR session key.
 * \param[in]      sequenceNum         Sequence number.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      recordIndex         Index of the record.
 * \param[in]      bufLen              Length of data to write.
 * \param[in]      buffer              Buffer containing data to be written.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_userRecordWriteHandler(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint16_t arNum,
    uint16_t sessionKey,
    uint32_t sequenceNum,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t recordIndex,
    const uint32_t *bufLen,
    const uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;

    OSALUNREF_PARM(arNum);
    OSALUNREF_PARM(sessionKey);
    OSALUNREF_PARM(sequenceNum);

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != addr) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState))
    {
        status = PN_API_OK;

        switch (recordIndex)
        {
        case REC_INDEX_USER_1000:
            status = PN_APP_IOD_userRecordWriteHandler1000(
                pnHandle,
                api,
                addr,
                recordIndex,
                bufLen,
                buffer,
                errState);
            break;
        default:
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
}

/*!
 * \brief
 * Handle record data read requests.
 *
 * \details
 * Based on the record index, this function decides how to handle all record
 * read requests arriving from the IO controller through Profinet stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          AR session key.
 * \param[in]      sequenceNum         Sequence number.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      recordIndex         Index of the record.
 * \param[in]      bufLen              Length of data to read.
 * \param[in]      buffer              Buffer in which data will be copied.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_recordRead(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint16_t arNum,
    uint16_t sessionKey,
    uint32_t sequenceNum,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t recordIndex,
    uint32_t *bufLen,
    uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState) &&
        (NULL != addr))
    {
        status = PN_API_OK;

        switch (recordIndex)
        {
#if PN_API_IOD_INCLUDE_AMR
        case REC_INDEX_ASSET_MANAGEMENT:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
#endif
        case REC_INDEX_PROFI_ENERGY:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_INVALID_INDEX;
            break;
#if(0 == PN_API_IOD_INCLUDE_IM0_4) /* By default, IMx data is handled by Profinet stack, IM0 handler
                                    * is only a user example, in case it is handled by the app */
        case REC_INDEX_IM0:
            status = PN_APP_IOD_im0RecordReadHandler(pnHandle, api, addr, bufLen, buffer, errState);
            break;
        case REC_INDEX_IM1:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        case REC_INDEX_IM2:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        case REC_INDEX_IM3:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        case REC_INDEX_IM4:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
#endif
#if(0 == PN_API_IOD_INCLUDE_IM5)
        case REC_INDEX_IM5:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
#endif
        case REC_INDEX_ISO_DATA:
            status = PN_APP_IOD_isoRecordReadHandler(pnHandle, api, addr, bufLen, buffer, errState);
            break;
/* If REC8028_8029 data is not handled inside Profinet stack, the user application needs to handle these data records */
#if (0 == PN_API_IOD_INCLUDE_REC8028_8029)
        case REC_INDEX_READ_IN_DATA:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        case REC_INDEX_READ_OUT_DATA:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
#endif
        default:
            if ((REC_INDEX_USER_START <= recordIndex) && (REC_INDEX_USER_END >= recordIndex))
            {
                status = PN_APP_IOD_userRecordReadHandler(
                    pnHandle,
                    api,
                    arNum,
                    sessionKey,
                    sequenceNum,
                    addr,
                    recordIndex,
                    bufLen,
                    buffer,
                    errState);
            }
            else
            {
                errState->errCode1 = ERR_CODE1_INVALID_INDEX;
            }
            break;
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            *bufLen = 0;
            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }
    return status;
}

/*!
 * \brief
 * Handle record data write requests.
 *
 * \details
 * Based on the record index, this function decides how to handle all record
 * write requests arriving from the IO controller through Profinet stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          AR session key.
 * \param[in]      sequenceNum         Sequence number.
 * \param[in]      addr                Logical or geographical address.
 * \param[in]      recordIndex         Index of the record.
 * \param[in]      bufLen              Length of data to write.
 * \param[in]      buffer              Buffer containing data to be written.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid input parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_recordWrite(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint16_t arNum,
    uint16_t sessionKey,
    uint32_t sequenceNum,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t recordIndex,
    const uint32_t *bufLen,
    const uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != addr) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != errState))
    {
        status = PN_API_OK;

        switch (recordIndex)
        {
        case REC_INDEX_PROFI_ENERGY:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_INVALID_INDEX;
            break;
#if(0 == PN_API_IOD_INCLUDE_IM0_4) /* By default, IMx data is handled by Profinet stack, IM0 handler
                                    * is only a user example, in case it is handled by the app */
        case REC_INDEX_IM0:
            status = PN_APP_IOD_im0RecordWriteHandler(pnHandle, api, addr, bufLen, buffer, errState);
            break;
        case REC_INDEX_IM1:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        case REC_INDEX_IM2:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        case REC_INDEX_IM3:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
        case REC_INDEX_IM4:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
#endif
#if(0 == PN_API_IOD_INCLUDE_IM5)
        case REC_INDEX_IM5:
            /* TBD upon user request */
            errState->errCode1 = ERR_CODE1_FEATURE_NOT_SUPPORTED;
            break;
#endif
        case REC_INDEX_ISO_DATA:
            status = PN_APP_IOD_isoRecordWriteHandler(pnHandle, api, addr, bufLen, buffer, errState);
            break;
        default:
            if ((REC_INDEX_USER_START <= recordIndex) && (REC_INDEX_USER_END >= recordIndex))
            {
                status = PN_APP_IOD_userRecordWriteHandler(
                    pnHandle,
                    api,
                    arNum,
                    sessionKey,
                    sequenceNum,
                    addr,
                    recordIndex,
                    bufLen,
                    buffer,
                    errState);
            }
            else
            {
                errState->errCode1 = ERR_CODE1_INVALID_INDEX;
            }
            break;
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode1 = ERR_CODE1_APP_BUSY;
    }

    return status;
}

void PN_APP_IOD_recordReadAsyncResponse(void)
{
    uint32_t status = PN_API_NOT_OK;

    if (NULL != asyncRecReadHandle)
    {
        /* Reset error state */
        asyncRecReadParams.errState.errCode = ERR_CODE_NO_ERROR;
        asyncRecReadParams.errState.errDecode = ERR_DECODE_NO_ERROR;
        asyncRecReadParams.errState.errCode1 = ERR_CODE1_NO_ERROR;
        asyncRecReadParams.errState.errCode2 = ERR_CODE2_NO_ERROR;
        asyncRecReadParams.errState.addValue1 = 0;
        asyncRecReadParams.errState.addValue2 = 0;

        status = PN_APP_IOD_recordRead(
            asyncRecReadParams.pnHandle,
            asyncRecReadParams.api,
            asyncRecReadParams.arNum,
            asyncRecReadParams.sessionKey,
            asyncRecReadParams.sequenceNum,
            &(asyncRecReadParams.addr),
            asyncRecReadParams.recordIndex,
            &(asyncRecReadParams.bufLen),
            asyncRecReadParams.buffer,
            &(asyncRecReadParams.errState));

        if (ERR_CODE1_NO_ERROR < asyncRecReadParams.errState.errCode1)
        {
            asyncRecReadParams.errState.errCode = ERR_CODE_IOD_READ_RSP;
            asyncRecReadParams.errState.errDecode = ERR_DECODE_PNIO_RW;
        }

        if (PN_API_ERR_PARAM != status)
        {
            status = PN_API_IOD_recordReadResponse(
                asyncRecReadParams.pnHandle,
                asyncRecReadHandle,
                asyncRecReadParams.buffer,
                asyncRecReadParams.bufLen,
                &(asyncRecReadParams.errState));

            OSAL_MEMORY_memset(&asyncRecReadParams, 0, sizeof(asyncRecReadParams));
            asyncRecReadHandle = NULL;
        }
    }

    if (PN_API_OK != status)
    {
        OSAL_printf("Failed to send asynchronous record read response!\r\n");
    }
}

void PN_APP_IOD_recordWriteAsyncResponse(void)
{
    uint32_t status = PN_API_NOT_OK;

    if (NULL != asyncRecWriteHandle)
    {
        /* Reset error state */
        asyncRecWriteParams.errState.errCode = ERR_CODE_NO_ERROR;
        asyncRecWriteParams.errState.errDecode = ERR_DECODE_NO_ERROR;
        asyncRecWriteParams.errState.errCode1 = ERR_CODE1_NO_ERROR;
        asyncRecWriteParams.errState.errCode2 = ERR_CODE2_NO_ERROR;
        asyncRecWriteParams.errState.addValue1 = 0;
        asyncRecWriteParams.errState.addValue2 = 0;

        status = PN_APP_IOD_recordWrite(
            asyncRecWriteParams.pnHandle,
            asyncRecWriteParams.api,
            asyncRecWriteParams.arNum,
            asyncRecWriteParams.sessionKey,
            asyncRecWriteParams.sequenceNum,
            &(asyncRecWriteParams.addr),
            asyncRecWriteParams.recordIndex,
            &(asyncRecWriteParams.bufLen),
            asyncRecWriteParams.buffer,
            &(asyncRecWriteParams.errState));

        if (ERR_CODE1_NO_ERROR < asyncRecWriteParams.errState.errCode1)
        {
            asyncRecReadParams.errState.errCode = ERR_CODE_IOD_WRITE_RSP;
            asyncRecReadParams.errState.errDecode = ERR_DECODE_PNIO_RW;
        }

        if (PN_API_ERR_PARAM != status)
        {
            status = PN_API_IOD_recordWriteResponse(
                asyncRecWriteParams.pnHandle,
                asyncRecWriteHandle,
                asyncRecWriteParams.bufLen,
                &(asyncRecWriteParams.errState));

            OSAL_MEMORY_memset(&asyncRecWriteParams, 0, sizeof(asyncRecWriteParams));
            asyncRecWriteHandle = NULL;
        }
    }

    if (PN_API_OK != status)
    {
        OSAL_printf("Failed to send asynchronous record write response!\r\n");
    }
}

uint32_t PN_APP_IOD_cbRecordRead(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint16_t arNum,
    uint16_t sessionKey,
    uint32_t sequenceNum,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t recordIndex,
    uint32_t *bufLen,
    uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != bufLen) &&
        (NULL != buffer) &&
        (NULL != addr) &&
        (NULL != errState))
    {
        /* Reset error state */
        errState->errCode = ERR_CODE_NO_ERROR;
        errState->errDecode = ERR_DECODE_NO_ERROR;
        errState->errCode1 = ERR_CODE1_NO_ERROR;
        errState->errCode2 = ERR_CODE2_NO_ERROR;
        errState->addValue1 = 0;
        errState->addValue2 = 0;

        /* Check asynchrounous read mode */
        if (false == asyncRecReadMode) /* Synchronous */
        {
            status = PN_APP_IOD_recordRead(
                pnHandle,
                api,
                arNum,
                sessionKey,
                sequenceNum,
                addr,
                recordIndex,
                bufLen,
                buffer,
                errState);
        }
        /* Asynchronous with pending asynchronous read requests */
        else if (NULL != asyncRecReadHandle)
        {
            OSAL_printf("Application currently does not handle more than one asynchronous request at a time!\r\n");
            errState->errCode1 = ERR_CODE1_APP_BUSY;
        }
        /* Asynchronous with no pending asynchronous read requests */
        else
        {
            status = PN_API_IOD_recordSetResponseAsync(pnHandle, &asyncRecReadHandle);

            if (PN_API_OK == status)
            {
                asyncRecReadParams.pnHandle = pnHandle;
                asyncRecReadParams.api = api;
                asyncRecReadParams.arNum = arNum;
                asyncRecReadParams.sessionKey = sessionKey;
                asyncRecReadParams.sequenceNum = sequenceNum;
                asyncRecReadParams.addr.address.geo.slot = addr->address.geo.slot;
                asyncRecReadParams.addr.address.geo.subslot = addr->address.geo.subslot;
                asyncRecReadParams.recordIndex = recordIndex;
                asyncRecReadParams.bufLen = *bufLen;
                asyncRecReadParams.buffer = (uint8_t*)OSAL_MEMORY_calloc(1, *bufLen);
            }
            else
            {
                errState->errCode1 = ERR_CODE1_APP_BUSY;
            }
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            errState->errCode = ERR_CODE_IOD_READ_RSP;
            errState->errDecode = ERR_DECODE_PNIO_RW;

            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode = ERR_CODE_IOD_READ_RSP;
        errState->errDecode = ERR_DECODE_PNIO_RW;
        errState->errCode1 = ERR_CODE1_APP_BUSY;
        errState->errCode2 = ERR_CODE2_NO_ERROR; /* Doesn't matter */
        errState->addValue1 = 0; /* Doesn't matter */
        errState->addValue2 = 0; /* Doesn't matter */
    }

    return status;
}

uint32_t PN_APP_IOD_cbRecordWrite(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint16_t arNum,
    uint16_t sessionKey,
    uint32_t sequenceNum,
    const PN_API_IOD_DevAddr_t *const addr,
    uint32_t recordIndex,
    const uint32_t *bufLen,
    const uint8_t *buffer,
    PN_API_IOD_ErrState_t *errState)
{
    uint32_t status = PN_API_ERR_PARAM;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != bufLen) &&
        (0 < *bufLen) &&
        (NULL != buffer) &&
        (NULL != addr) &&
        (NULL != errState))
    {
        /* Reset error state */
        errState->errCode = ERR_CODE_NO_ERROR;
        errState->errDecode = ERR_DECODE_NO_ERROR;
        errState->errCode1 = ERR_CODE1_NO_ERROR;
        errState->errCode2 = ERR_CODE2_NO_ERROR;
        errState->addValue1 = 0;
        errState->addValue2 = 0;

        /* Check asynchrounous write mode */
        if (false == asyncRecWriteMode) /* Synchronous */
        {
            status = PN_APP_IOD_recordWrite(
                pnHandle,
                api,
                arNum,
                sessionKey,
                sequenceNum,
                addr,
                recordIndex,
                bufLen,
                buffer,
                errState);
        }
        /* Asynchronous with pending asynchronous write requests */
        else if (NULL != asyncRecWriteHandle)
        {
            OSAL_printf("Application currently does not handle more than one asynchronous request at a time!\r\n");
            errState->errCode1 = ERR_CODE1_APP_BUSY;
        }
        /* Asynchronous with no pending asynchronous write requests */
        else
        {
            status = PN_API_IOD_recordSetResponseAsync(pnHandle, &asyncRecWriteHandle);

            if (PN_API_OK == status)
            {
                asyncRecWriteParams.pnHandle = pnHandle;
                asyncRecWriteParams.api = api;
                asyncRecWriteParams.arNum = arNum;
                asyncRecWriteParams.sessionKey = sessionKey;
                asyncRecWriteParams.sequenceNum = sequenceNum;
                asyncRecWriteParams.addr.address.geo.slot = addr->address.geo.slot;
                asyncRecWriteParams.addr.address.geo.subslot = addr->address.geo.subslot;
                asyncRecWriteParams.recordIndex = recordIndex;
                asyncRecWriteParams.bufLen = *bufLen;
                asyncRecWriteParams.buffer = (uint8_t*)OSAL_MEMORY_calloc(1, *bufLen);
                OSAL_MEMORY_memcpy(asyncRecWriteParams.buffer, buffer, *bufLen);
            }
            else
            {
                errState->errCode1 = ERR_CODE1_APP_BUSY;
            }
        }

        if (ERR_CODE1_NO_ERROR < errState->errCode1)
        {
            errState->errCode = ERR_CODE_IOD_WRITE_RSP;
            errState->errDecode = ERR_DECODE_PNIO_RW;

            status = PN_API_NOT_OK;
        }
    }

    if ((PN_API_ERR_PARAM == status) && (NULL != errState))
    {
        errState->errCode = ERR_CODE_IOD_WRITE_RSP;
        errState->errDecode = ERR_DECODE_PNIO_RW;
        if((NULL != bufLen) && (0 >= *bufLen))
        {
            errState->errCode1 = ERR_CODE1_WRITE_LENGTH;
        }
        else
        {
            errState->errCode1 = ERR_CODE1_APP_BUSY;
        }
        errState->errCode2 = ERR_CODE2_NO_ERROR; /* Doesn't matter */
        errState->addValue1 = 0; /* Doesn't matter */
        errState->addValue2 = 0; /* Doesn't matter */
    }

    return status;
}
