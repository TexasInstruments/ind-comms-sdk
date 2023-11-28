/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API base interface.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2023 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#if !(defined __ECSLVAPI_BASE_H__)
#define __ECSLVAPI_BASE_H__		1

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

extern ECATSLV_API uint32_t EC_API_SLV_getVersion(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t *pVersion);

extern ECATSLV_API uint32_t EC_API_SLV_getVersionStr(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t bufferLen,
    char* pBuffer,
    uint32_t* pBufferUsedLen);

extern ECATSLV_API uint32_t EC_API_SLV_getVersionId(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t bufferLen,
    char* pBuffer,
    uint32_t* pBufferUsedLen);

extern ECATSLV_API uint32_t EC_API_SLV_FBTL_configuration(void* pFbtlHandle);
extern ECATSLV_API uint32_t EC_API_SLV_load(
    OSAL_PJumpBuf_t* pExceptionPoint,
    OSAL_ERR_CBHandler_t cbErrHandler,
    uint32_t pruSelect);

extern ECATSLV_API uint32_t EC_API_SLV_unLoad(void);

extern ECATSLV_API uint32_t EC_API_SLV_prepareTasks(
    OSAL_TASK_Priority_t pdiTaskPrio,
    OSAL_TASK_Priority_t statusLEDTaskPrio,
    OSAL_TASK_Priority_t sync0TaskPrio,
    OSAL_TASK_Priority_t sync1TaskPrio,
    OSAL_TASK_Priority_t eoeTaskPrio);

extern ECATSLV_API uint32_t EC_API_SLV_stackInit(void);

extern ECATSLV_API EC_API_SLV_SHandle_t* EC_API_SLV_new(void);

extern ECATSLV_API uint32_t EC_API_SLV_obdCleanUp(EC_API_SLV_SHandle_t* pHandle);

extern ECATSLV_API uint32_t EC_API_SLV_delete(EC_API_SLV_SHandle_t* pHandle);

extern ECATSLV_API uint32_t EC_API_SLV_init(EC_API_SLV_SHandle_t* pHandle);

extern ECATSLV_API uint32_t EC_API_SLV_run(EC_API_SLV_SHandle_t* pHandle);

extern ECATSLV_API uint32_t EC_API_SLV_stop(EC_API_SLV_SHandle_t* pHandle);

extern ECATSLV_API uint32_t EC_API_SLV_reset(EC_API_SLV_SHandle_t* pHandle);

extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterErrorHandler(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_CBStackError_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_getConfigStatus(EC_API_SLV_SHandle_t* pHandle);

extern ECATSLV_API uint32_t EC_API_SLV_getCyclicStatus(EC_API_SLV_SHandle_t* pHandle);

extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterMeasurement(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_CBMsrmt_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_mainLoopCyclic(void);

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_BASE_H__ */
