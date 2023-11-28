/*!
 *  \file ecSlvApi_pdo.h
 *
 *  \brief
 *  EtherCAT User API process data interface.
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

#if !(defined __ECSLVAPI_PDO_H__)
#define __ECSLVAPI_PDO_H__		1

#if (defined ECATSLAVE_SO) && (ECATSLAVE_SO==1) // defined if ECATSLV is compiled as a DLL
 #ifdef ECSLVAPI_EXPORTS // defined if we are building the ECATSLAVE DLL (instead of using it)
  #define ECATSLV_API OSAL_DLL_EXPORT
 #else
  #define ECATSLV_API OSAL_DLL_IMPORT
 #endif // ECSLVAPI_EXPORTS
 #define ECATSLV_LOC OSAL_DLL_LOCAL
#else // ECATSLAVE_SO is not defined: this means ECATSLAVE is a static lib.
 #define ECATSLV_API
 #define ECATSLV_LOC
#endif // ECATSLAVE_SO

#include <osal.h>
#include "defines/ecSlvApiDef_error.h"
#include <ecSlvApi_types.h>


#if (defined __cplusplus)
extern "C" {
#endif

extern ECATSLV_API uint32_t EC_API_SLV_setPDOSize(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t maxPdOutSize,
    uint32_t maxPdInSize);

extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterPreSeqInputPDBuffer(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_CBPreSeqInputPD_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterPreSeqOutputPDBuffer(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_CBPreSeqOutputPD_t  cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterPostSeqInputPDBuffer(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_CBPostSeqInputPD_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterPostSeqOutputPDBuffer(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_CBPostSeqOutputPD_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_preSeqInputPDBuffer(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t length,
    void** ppInProcData);

extern ECATSLV_API uint32_t EC_API_SLV_preSeqOutputPDBuffer(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t length,
    void** ppOutProcData);

extern ECATSLV_API uint32_t EC_API_SLV_postSeqInputPDBuffer(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t length,
    void* pInProcData);

extern ECATSLV_API uint32_t EC_API_SLV_postSeqOutputPDBuffer(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t length,
    void* pOutProcData);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_create(
    EC_API_SLV_SHandle_t* pHandle,
    char* pName,
    uint16_t mapIndex,
    EC_API_SLV_Pdo_t** pOutPdo);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_get(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t index,
    EC_API_SLV_Pdo_t** pOutPdo);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getOffset(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint16_t* pOffset);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getLength(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint16_t* pLength);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_createEntry(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    char* pName,
    EC_API_SLV_SCoE_ObjEntry_t* pObjectEntry);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getEntry(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint8_t subIndex,
    EC_API_SLV_SPdoEntry_t** ppPdoEntry);

extern ECATSLV_API uint32_t EC_API_SLV_getInputProcDataLength(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t* pLength);

extern ECATSLV_API uint32_t EC_API_SLV_getOutputProcDataLength(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t* pLength);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getEntryCount(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint8_t* pCount);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getEntryDataLength(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint8_t subIndex,
    uint32_t* pLength);

extern ECATSLV_API uint32_t EC_API_SLV_setInputData(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t length,
    uint8_t* pInputData);
extern ECATSLV_API uint32_t EC_API_SLV_getOutputData(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t length,
    uint8_t* pOutputData);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_setData(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint32_t length,
    uint8_t* pData);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getData(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint32_t length,
    uint8_t* pData);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_setEntryData(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint8_t subIndex,
    uint32_t length,
    uint8_t* pData);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getEntryData(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint8_t subIndex,
    uint32_t length,
    uint8_t* pData);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_setFixed(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    bool fixed);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_enable(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_disable(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_enabled(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    bool* pEnabled);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_setAssignment(
    EC_API_SLV_SHandle_t* pHandle,
    bool assignment);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_addPadding(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint8_t length);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_registerAssignmentChanges(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_PDO_CBAssignmentChanges_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_registerMappingChanges(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_PDO_CBMappingChanges_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getMappingInfo(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint8_t* pCount,
    EC_API_SLV_PDO_SEntryMap_t* pPdoMapArray);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_getAssignmentInfo(
    EC_API_SLV_SHandle_t* pHandle,
    bool rx,
    uint8_t* pCount,
    uint16_t* pPdoIndexArray);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_setMaxSubIndex(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_Pdo_t* pPdo,
    uint8_t maxSubIndex);

extern ECATSLV_API uint32_t EC_API_SLV_PDO_setSyncManMaxSubIndex(
    EC_API_SLV_SHandle_t* pHandle,
    uint8_t syncMan,
    uint8_t maxSubIndex);

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_PDO_H__ */
