/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API ESC interface.
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

#if !(defined __ECSLVAPI_ESC_H__)
#define __ECSLVAPI_ESC_H__		1

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

extern ECATSLV_API uint32_t EC_API_SLV_getState(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_EEsmState_t* pState,
    uint16_t* pAlErrorCode);

extern ECATSLV_API uint32_t  EC_API_SLV_setState(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_EEsmState_t state,
    uint16_t alErrorCode);

extern ECATSLV_API uint32_t EC_API_SLV_setStationAlias(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t stationAlias);

extern ECATSLV_API uint32_t EC_API_SLV_readByteEscRegister(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t escAddress,
    uint8_t* pValue);

extern ECATSLV_API uint32_t EC_API_SLV_readWordEscRegister(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t escAddress,
    uint16_t* pValue);

extern ECATSLV_API uint32_t EC_API_SLV_readDoubleWordEscRegister(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t escAddress,
    uint32_t* pValue);

extern ECATSLV_API uint32_t EC_API_SLV_writeByteEscRegister(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t escAddress,
    uint8_t value);

extern ECATSLV_API uint32_t EC_API_SLV_writeWordEscRegister(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t escAddress,
    uint16_t value);

extern ECATSLV_API uint32_t EC_API_SLV_writeDoubleWordEscRegister(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t escAddress,
    uint32_t value);


#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_ESC_H__ */
