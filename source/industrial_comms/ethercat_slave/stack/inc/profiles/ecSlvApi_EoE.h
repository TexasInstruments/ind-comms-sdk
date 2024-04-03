/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API EoE interface.
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

#if !(defined __ECSLVAPI_EOE_H__)
#define __ECSLVAPI_EOE_H__		1

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

extern ECATSLV_API uint32_t EC_API_SLV_EoE_cbRegisterReceiveHandler(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_EoE_CBReceiveHandler_t cbFunc,
    void *pContext);

extern ECATSLV_API uint32_t EC_API_SLV_EoE_cbRegisterSettingIndHandler(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_EoE_CBSettingIndHandler_t cbFunc,
    void *pContext);

extern ECATSLV_API uint32_t EC_API_SLV_EoE_sendFrame(
    EC_API_SLV_SHandle_t *pHandle,
    uint16_t length,
    uint8_t *pData);

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_EOE_H__ */
