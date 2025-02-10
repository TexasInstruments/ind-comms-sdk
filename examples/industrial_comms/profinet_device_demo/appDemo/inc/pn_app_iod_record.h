/*!
 *  \file pn_app_iod_record.h
 *
 *  \brief
 *  Functions and callback for handling record data read and write.
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

#ifndef PN_APP_IOD_RECORD_H
#define PN_APP_IOD_RECORD_H

#include <string.h>
#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*!< Other record indexes can be obtained from Profinet specifications <br>*/
#define REC_INDEX_USER_START            0x0000 /*!< Starting index for user defined records */
#define REC_INDEX_USER_END              0x7FFF /*!< Ending index for user defined records */
#define REC_INDEX_USER_1000             0x1000 /*!< See index = 4096 in GSDML file */
#ifdef PN_API_IOD_INCLUDE_IM0_4
#define REC_INDEX_IM0                   0xAFF0 /*!< Only read access */
#define REC_INDEX_IM1                   0xAFF1 /*!< Read/write access */
#define REC_INDEX_IM2                   0xAFF2 /*!< Read/write access */
#define REC_INDEX_IM3                   0xAFF3 /*!< Read/write access */
#define REC_INDEX_IM4                   0xAFF4 /*!< Read/write access */
#endif
#ifdef PN_API_IOD_INCLUDE_IM5
#define REC_INDEX_IM5                   0xAFF5 /*!< Only read access */
#endif
#define REC_INDEX_ISO_DATA              0x8030 /*!< Read/write access */
#define REC_INDEX_PROFI_ENERGY          0x80A0 /*!< Read/write access */
#define REC_INDEX_ASSET_MANAGEMENT      0xAFF0 /*!< Only read access */
#define REC_INDEX_READ_IN_DATA          0x8028 /*!< Only read access */
#define REC_INDEX_READ_OUT_DATA         0x8029 /*!< Only read access */

/*!< Other error codes can be obtained from Profinet specifications <br>*/
#define ERR_CODE_NO_ERROR                       0x00 /*!< No error */
#define ERR_CODE_IOD_READ_RSP                   0xDE /*!< Read error */
#define ERR_CODE_IOD_WRITE_RSP                  0xDF /*!< Write error */

#define ERR_DECODE_NO_ERROR                     0x00 /*!< No error */
#define ERR_DECODE_PNIO_RW                      0x80 /*!< Write error */

#define ERR_CODE1_NO_ERROR                      0x00 /*!< No error */
#define ERR_CODE1_APP_BUSY                      0xA7 /*!< App busy */
#define ERR_CODE1_FEATURE_NOT_SUPPORTED         0xA9 /*!< Feature not supported */
#define ERR_CODE1_INVALID_INDEX                 0xB0 /*!< Invalid index */
#define ERR_CODE1_WRITE_LENGTH                  0xB1 /*!< Write length */
#define ERR_CODE1_INVALID_SLOT_SUBSLOT          0xB2 /*!< Invalid slot/subslot */
#define ERR_CODE1_STATE_CONFLICT                0xB5 /*!< State conflict */
#define ERR_CODE1_ACCESS_DENIED                 0xB6 /*!< Access denied */
#define ERR_CODE1_INVALID_PARAMETER             0xB8 /*!< Invalid parameter */
#define ERR_CODE1_INVALID_TYPE                  0xB9 /*!< Invalid type */
#define ERR_CODE1_READ_CONSTRAINT_CONFLICT      0xC0 /*!< Read constraint conflict */
#define ERR_CODE1_RESOURCE_UNAVAILBALE          0xC3 /*!< Resource unavailable */

#define ERR_CODE2_NO_ERROR                      0x00 /*!< No error */

/*!< Other block header types can be obtained from Profinet specifications <br>*/
#ifdef PN_API_IOD_INCLUDE_IM0_4
#define BLOCK_HEADER_TYPE_IM0                   0x0020 /*!< Block header type for IM0 */
#define BLOCK_HEADER_TYPE_IM1                   0x0021 /*!< Block header type for IM1 */
#define BLOCK_HEADER_TYPE_IM2                   0x0022 /*!< Block header type for IM2 */
#define BLOCK_HEADER_TYPE_IM3                   0x0023 /*!< Block header type for IM3 */
#define BLOCK_HEADER_TYPE_IM4                   0x0024 /*!< Block header type for IM4 */
#endif
#ifdef PN_API_IOD_INCLUDE_IM5
#define BLOCK_HEADER_TYPE_IM5                   0x0025 /*!< Block header type for IM5 */
#endif
#define BLOCK_HEADER_TYPE_ISO                   0x0204U /*!< Block header type for ISO */

/*!< Other block header versions can be obtained from Profinet specifications <br>*/
#define BLOCK_HEADER_VERSION                    0x0100 /*!< Block header version */
#define BLOCK_HEADER_VERSION_ISO                BLOCK_HEADER_VERSION

/*!< IM0 default defines, can be updated by end user <br>*/
#define IM0_PROFILE_ID                          0 /*!< Profile id, defined by profiles */
#define IM0_PROFILE_SPECIFIC_TYPE_IO_MOD        3 /*!< Module type, IO */
#define IM0_PROFILE_SPECIFIC_TYPE_IF_MOD        5 /*!< Module type, IF */
#define IM0_VERSION_MAJOR                       1 /*!< IM0 version major */
#define IM0_VERSION_MINOR                       1 /*!< IM0 version minor */
#define IM0_DAP_DATA                            0x3E /*!< Additional support for IM1_5 */
#define IM0_NO_DAP_DATA                         0x1E /*!< IM not for DAP, IM1_4 supported */

/*!< Max length of Profinet user specific parameter record */
#define USER_MAX_REC_LEN                        40

/*!< Actual length of Profinet specific parameter record (see GSDML file for more info) */
#define USER_MOD_REC_LEN                        16

/*!
 * \brief User record parameter.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t index;                              /*!< Record index */
    uint32_t periphRealCfgInd;                   /*!< Index of submodule Peripheral config */
    uint32_t dataSize;                           /*!< Record read/write data size */
    uint8_t dataByteArray[USER_MAX_REC_LEN];     /*!< Byte array to exchange user parameter data */
} PN_APP_IOD_userRec_t;

/*!
 * \brief Asynchronous record parameter.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    PN_API_IOD_Handle_t *pnHandle;      /*!< Profinet API Handle. */
    uint32_t api;                       /*!< Application Process Identifier. */
    uint16_t arNum;                     /*!< AR number */
    uint16_t sessionKey;                /*!< Session key. */
    uint32_t sequenceNum;               /*!< Sequence number. */
    PN_API_IOD_DevAddr_t addr;          /*!< Logical or geographical address. */
    uint32_t recordIndex;               /*!< Index of the record. */
    uint32_t bufLen;                    /*!< Length of data to read/write */
    uint8_t *buffer;                    /*!< Buffer to hold the read/write data */
    PN_API_IOD_ErrState_t errState;     /*!< Profinet error state */
} PN_APP_IOD_asyncReqParams_t;

#pragma pack(push)
#pragma pack(1)

/*!
 * \brief IM0 data block.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    PN_API_IOD_ioBlockHeader_t blockHeader;     /*!< BlockHeader ,see Profinet specifications */
    PN_API_IOD_Im0Data_t       im0Data;         /*!< IM0 data */
} PN_APP_IOD_im0DataBlock_t;

#pragma pack(pop)

/*!
 * \brief
 * Initialize record info.
 *
 * \details
 * Record data is stored in a global array (userRecList) that is initialized to zero
 * along with all asynchronous record read/write buffers, parameters, handles and modes.
 * If asynchronous mode for read and write functions is initialized to \c true, the
 * user application informs the stack within the context of PN_APP_IOD_cbRecordRead()
 * or PN_APP_IOD_cbRecordWrite() functions that the read/write response will follow later
 * and there is no need for the stack to wait for it synchronously.
 *
 * \param[in]      asyncRead           Asynchronous read.
 * \param[in]      asyncWrite          Asynchronous write.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
void PN_APP_IOD_initRecordInfo(bool asyncRead, bool asyncWrite);

/*!
 * \brief
 * Clear an element in the global record data array userRecList.
 *
 * \details
 * Based on the it's index, an element from userRecList is cleared to zero using
 * this function.
 *
 * \param[in]      index               Index of the array element.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
void PN_APP_IOD_clearRecordListElement(uint16_t index);

    /*!
 * \brief
 * Send asynchronous read response.
 *
 * \details
 * If asynchronous mode for read function is set to \c true, the user application
 * is expected at one point to send a response to the request received earlier by
 * PN_APP_IOD_cbRecordRead() function. The response is sent using this function.
 * The function checks if there's any pending read request and sends its response.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
void PN_APP_IOD_recordReadAsyncResponse(void);

/*!
 * \brief
 * Send asynchronous write response.
 *
 * \details
 * If asynchronous mode for write function is set to \c true, the user application
 * is expected at one point to send a response to the request received earlier by
 * PN_APP_IOD_cbRecordWrite() function. The response is sent using this function.
 * The function checks if there's any pending write request and sends its response.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 */
void PN_APP_IOD_recordWriteAsyncResponse(void);

/*!
 * \brief
 * Callback function to read user record data requested by the IO controller.
 *
 * \details
 * This function first checks if the asynchronous record read mode is activated.
 * If not, it reads the record data from the corresponding array element and
 * stores it in the buffer returned to Profinet stack. Otherwise, it sends
 * a notification to the stack informing it that the read response will follow later.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          AR session key.
 * \param[in]      sequenceNum         Sequence number.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      recordIndex         Index of the record.
 * \param[in]      bufLen              Length of data to read.
 * \param[in]      buffer              Buffer to store the read data.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_recordReadCbf
 */
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
    PN_API_IOD_ErrState_t *errState);

/*!
 * \brief
 * Callback function to write user record data requested by the IO controller.
 *
 * \details
 * This function first checks if the asynchronous record write mode is activated.
 * If not, it writes the record data from the buffer provided by Profinet stack to
 * the corresponding array element. Otherwise, it sends a notification to the stack
 * informing it that the write response will follow later.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      api                 Application Process Identifier.
 * \param[in]      arNum               AR number.
 * \param[in]      sessionKey          AR session key.
 * \param[in]      sequenceNum         Sequence number.
 * \param[in]      addr                Geographical address (slot/subslot) of the module/submodule.
 * \param[in]      recordIndex         Index of the record.
 * \param[in]      bufLen              Length of data to write.
 * \param[in]      buffer              Buffer containing data to be written.
 * \param[in,out]  errState            Profinet status.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_APP_IOD_RECORD_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_recordWriteCbf
 */
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
    PN_API_IOD_ErrState_t *errState);

#if (defined __cplusplus)
}
#endif

#endif /* PN_APP_IOD_RECORD_H */
