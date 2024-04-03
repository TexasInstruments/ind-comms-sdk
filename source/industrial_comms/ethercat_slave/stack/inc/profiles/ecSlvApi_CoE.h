/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API CoE interface.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2024 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#if !(defined __ECSLVAPI_COE_H__)
#define __ECSLVAPI_COE_H__		1

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

#include "osal.h"
#include "defines/ecSlvApiDef_error.h"
#include "ecSlvApi_types.h"

#if (defined __cplusplus)
extern "C" {
#endif

extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObject(
    EC_API_SLV_SHandle_t *pHandle,
    uint16_t index,
    EC_API_SLV_SCoE_Object_t **ppObject);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObjectData(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    uint32_t length,
    uint16_t *pData);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_setObjectData(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    uint8_t subIndex,
    uint32_t length,
    uint16_t *pData);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObjectEntryCount(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    uint8_t *pCount);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObjectLength(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    uint32_t *length);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObjectEntryLength(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_ObjEntry_t *pObjEntry,
    uint32_t *length);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObjectType(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    uint8_t *pType);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObjectEntry(
    EC_API_SLV_SHandle_t *pHandle,
    uint16_t index,
    uint8_t subIndex,
    EC_API_SLV_SCoE_ObjEntry_t **ppObjectEntry);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObjectEntryByObject(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    uint8_t subIndex,
    EC_API_SLV_SCoE_ObjEntry_t **ppObjectEntry);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_getObjectEntryData(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_ObjEntry_t *pObjectEntry,
    uint32_t length,
    uint16_t *pData);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_setObjectEntryData(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_ObjEntry_t *pObjectEntry,
    uint32_t length,
    uint16_t *pData);

extern ECATSLV_API uint32_t EC_API_SLV_CoE_odAddVariable(
    EC_API_SLV_SHandle_t *pHandle,
    uint16_t index,
    char *pName,
    uint16_t type,
    uint16_t bitLen,
    uint16_t flags,
    EC_API_SLV_CBObjRead_t cbRead,
    void *pReadCtxt,
    EC_API_SLV_CBObjWrite_t cbWrite,
    void *pWriteCtxt);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_odAddArray(
    EC_API_SLV_SHandle_t *pHandle,
    uint16_t index,
    char *pName,
    uint8_t arrayLen,
    uint16_t type,
    uint16_t bitLen,
    uint16_t flags,
    EC_API_SLV_CBObjRead_t cbRead,
    void *pReadCtxt,
    EC_API_SLV_CBObjWrite_t cbWrite,
    void *pWriteCtxt);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_odAddRecord(
    EC_API_SLV_SHandle_t *pHandle,
    uint16_t index,
    char *pName,
    EC_API_SLV_CBObjRead_t cbRead,
    void *pReadCtxt,
    EC_API_SLV_CBObjWrite_t cbWrite,
    void *pWriteCtxt,
    EC_API_SLV_SCoE_Object_t **ppOutSdo);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_configRecordSubIndex(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    uint8_t subIndex,
    char *pName,
    uint16_t type,
    uint16_t bitLen,
    uint16_t flags);

extern ECATSLV_API uint32_t EC_API_SLV_CoE_odAddEnum(
    EC_API_SLV_SHandle_t *pHandle,
    uint16_t index,
    EC_API_SLV_SCoE_Object_t **ppObject);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_configEnum(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    uint32_t value,
    char *pName);

extern ECATSLV_API uint32_t EC_API_SLV_CoE_subIdx0WrFlag(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_SCoE_Object_t *pObject,
    bool readWriteFlag);
extern ECATSLV_API uint32_t EC_API_SLV_CoE_writeEmergency(
    EC_API_SLV_SHandle_t *pHandle,
    uint16_t code,
    uint32_t length,
    uint8_t *pData);

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_COE_H__ */
