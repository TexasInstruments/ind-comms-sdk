/*!
 *  \file ecSlvApiDef_Diagnosis.h
 *
 *  \brief
 *  Diagnosis history defines.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2023 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#if !(defined __ECSLVAPIDEF_DIAGNOSIS_H__)
#define __ECSLVAPIDEF_DIAGNOSIS_H__		1

#include <stdint.h>

#if (defined __cplusplus)
extern "C" {
#endif

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

/**
* \addtogroup DIAG flags
* @{
*/

/*---------------------------------------------
-    DIAG flags
-----------------------------------------------*/

#define EC_API_SLV_DIAG_MSG_TYPE_INFO           0x0 /**< \brief Diagnosis message type info*/
#define EC_API_SLV_DIAG_MSG_TYPE_WARNING        0x1 /**< \brief Diagnosis message type warning*/
#define EC_API_SLV_DIAG_MSG_TYPE_ERROR          0x2 /**< \brief Diagnosis message type error*/

#define EC_API_SLV_DIAG_MSG_PARAM_TYPE_DATA    0x0 /**< \brief Diagnosis parameter type data*/
#define EC_API_SLV_DIAG_MSG_PARAM_TYPE_B_ARRY  0x1 /**< \brief Diagnosis parameter type Byte Array*/
#define EC_API_SLV_DIAG_MSG_PARAM_TYPE_ASCII   0x2 /**< \brief Diagnosis parameter type ASCII*/
#define EC_API_SLV_DIAG_MSG_PARAM_TYPE_UNICODE 0x3 /**< \brief Diagnosis parameter type UNICODE*/

/*Defined Diagnosis Codes*/
#define EC_API_SLV_DIAG_CODE_EMCY(Code)        (((uint32_t)(Code))<< 16 | (uint16_t)(0xE800))  /**< \brief Macro to create emergency code*/
#define EC_API_SLV_DIAG_APPL_INFO              0xE002                                          /**< \brief Diag Code for application information*/
#define EC_API_SLV_DIAG_MDP_DEVICE             0xE803                                          /**< \brief Device related messages in a modular device*/

/*Text IDs*/
#define EC_API_SLV_DIAG_TEXT_ID_STATE_CHANGE_INFO           0x01 /**< \brief State change Request From: %d To:%d*/
#define EC_API_SLV_DIAG_TEXT_ID_DEC_NUMBER                  0x02 /**< \brief %d*/
#define EC_API_SLV_DIAG_TEXT_ID_CALC_BUS_CYCLE_FAILED       0x0F /**< \brief Calculate bus cycle time failed (Local timer too slow)*/
#define EC_API_SLV_DIAG_TEXT_ID_STATE_SM_ADDR               0x11 /**< \brief Sync Manager %d invalid address (%d)*/
#define EC_API_SLV_DIAG_TEXT_ID_STATE_SM_SIZE               0x12 /**< \brief Sync Manager %d invalid size (%d)*/
#define EC_API_SLV_DIAG_TEXT_ID_STATE_SM_SETTING            0x13 /**< \brief Sync Manager %d invalid settings (%d)*/
#define EC_API_SLV_DIAG_TEXT_ID_INVALID_REG_981             0x20 /**< \brief DC activation register is invalid*/
#define EC_API_SLV_DIAG_TEXT_ID_INVALID_SYNC_TYPE           0x21 /**< \brief Configured SyncType (0x1C32.1 or 0x1C33.1) not supported. Check DC registers and supported SyncTypes (0x1C32.4 and 0x1C33.4)*/

/** @}*/

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPIDEF_DIAGNOSIS_H__ */
