/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API FoE interface.
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

#if !(defined __ECSLVAPI_FOE_H__)
#define __ECSLVAPI_FOE_H__		1

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

extern ECATSLV_API uint32_t EC_API_SLV_FoE_cbRegisterOpenFileHandler(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_FoE_CBOpenFileHandler_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_FoE_cbRegisterReadFileHandler(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_FoE_CBReadFileHandler_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_FoE_cbRegisterWriteFileHandler(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_FoE_CBWriteFileHandler_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_FoE_cbRegisterCloseFileHandler(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_FoE_CBCloseFileHandler_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_FoE_cbRegisterFwUpdate(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_FoE_CBFwUpdateHandler_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterSystemReboot(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_CBSystemRebootHandler_t cbFunc,
    void *pContext);
extern ECATSLV_API uint32_t EC_API_SLV_setFwUpdateMode(
    EC_API_SLV_SHandle_t *pHandle,
    bool fwUpdateRequired);

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_FOE_H__ */
