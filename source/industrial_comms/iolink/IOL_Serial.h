/*!
 *  \example IOL_Serial.h
 *
 *  \brief
 *  Module for communication via a serial interface
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2020, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2023 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#ifndef __IOL_SERIAL_H__
#define __IOL_SERIAL_H__


#include <SMI/IOLM_SMI.h>

/** \brief Defines the receive default length */
#define IOL_SERIAL_RX_DEFAULT_LENGTH (2048+100)

/** \brief Defines the receive timeout in ms */
#define IOL_SERIAL_TIMEOUT_MS 200


/**
\brief This structure defines the handle for a serial connection
*/
typedef struct IOL_Serial_SHandle
{
    INT32U u32BufferLen;                /**< Length of single Tx buffer */
    INT32U u32BufferCount;              /**< \Number of Tx buffers */
    INT8U* pu8Data;                     /**< Array with u32BufferLen * u32BufferCount Bytes */
    INT32U* pu32Length;                 /**< Array of u32BufferCount with actual length */

    INT32U u32RxBufferLen;              /**< Length of Rx buffer */
    INT8U* pu8RxBuffer;                 /**< Pointer to Rx buffer */
    IOLM_SMI_EOrigin eCbType;           /**< Select used interface (Std or Generic) */
    IOLM_SMI_CBGenericCnf cbRxGeneric;  /**< Pointer to receive callback function for eCbType Generic */
    IOLM_SMI_CBStdInd cbRxStd;          /**< Pointer to receive callback function for eCbType Std*/
    INT8U u8ForceClientId;              /**< Actual ID or 0 if not used */

    volatile TBOOL boWritePending;      /**< Flag for write control (TRUE if write operation is pending) */
    INT32U u32ReadPos;                  /**< Current read position in Tx buffer */
    INT32U u32WritePos;                 /**< Current write position in Tx buffer */
    INT32U u32RxPos;                    /**< Current position in Rx buffer */
    INT32S s32LastRxTime;               /**< Timestamp of last received packet */

}IOL_Serial_SHandle;


INT8U* IOL_Serial_pu8GetTxBuffer(IOL_Serial_SHandle* psuHandle_p, INT32U* pu32Length_p);
void IOL_Serial_vReleaseTxBuffer(IOL_Serial_SHandle* psuHandle_p);
INT8U IOL_Serial_u8GetHeaderLength(IOL_Serial_SHandle* psuHandle_p);
INT16U IOL_Serial_u16GetArgBlockLength(IOL_Serial_SHandle* psuHandle_p);
INT32U IOL_Serial_vGetLenRemain(IOL_Serial_SHandle* psuHandle_p);
void IOL_Serial_vSend(IOL_Serial_SHandle* psuHandle_p, INT8U* pu8Data_p, INT32U u32Length_p);
void IOL_Serial_vSendSmi(IOL_Serial_SHandle* psuHandle_p, IOLM_SMI_SHeader* psuHeader_p, INT8U* pu8ArgBlock_p);
void IOL_Serial_vSendSmiStd(IOL_Serial_SHandle* psuHandle_p, IOLM_SMI_SStdHeader* psuHeader_p, INT8U* pu8ArgBlock_p);
void IOL_Serial_vInit(IOL_Serial_SHandle* psuHandle_p);

void IOL_Serial_vReceive(IOL_Serial_SHandle* psuHandle_p, INT8U* pu8Data_p, INT32U u32Length_p);
void IOL_Serial_vRxProcessing(IOL_Serial_SHandle* psuHandle_p);

#endif
