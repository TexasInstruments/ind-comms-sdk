/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API CiA402 interface.
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

#if !(defined __ECSLVAPI_CIA402_H__)
#define __ECSLVAPI_CIA402_H__		1

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

extern ECATSLV_API uint32_t EC_API_SLV_CiA402_setAxisNumber(
    EC_API_SLV_SHandle_t *pHandle,
    uint8_t axisNo);
extern ECATSLV_API uint32_t EC_API_SLV_CiA402_activateAxis(
    EC_API_SLV_SHandle_t *pHandle,
    uint8_t axisNo,
    bool active);
extern ECATSLV_API uint32_t EC_API_SLV_CiA402_SM_clearErrorCode(
    EC_API_SLV_SHandle_t *pHandle,
    uint8_t axisNo);
extern ECATSLV_API uint32_t EC_API_SLV_CiA402_SM_getErrorCode(
    EC_API_SLV_SHandle_t *pHandle,
    uint8_t axisNo,
    uint16_t *pErrorCode);
extern ECATSLV_API uint32_t EC_API_SLV_CiA402_registerSetDictionary(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_CiA402_CBUsrApplSetDict_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_CiA402_registerSetDictValues(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_CiA402_CBUsrApplSetDictValues_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_CiA402_registerStateMachine(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_CiA402_CBUsrApplSM_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_CiA402_registerApplication(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_CiA402_CBUsrApplApplication_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_CiA402_registerLocalError(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_CiA402_CBUsrApplLocalError_t cbFunc,
    void *pContext);

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_CIA402_H__ */
