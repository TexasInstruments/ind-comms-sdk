/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API PHY interface.
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

#if !(defined __ECSLVAPI_PHY_H__)
#define __ECSLVAPI_PHY_H__		1

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

#if !(defined FBTL_REMOTE) && !(defined DPRAM_REMOTE)
extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterPhyReset(
    EC_API_SLV_SHandle_t* pHandle,
    CUST_PHY_CBreset_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_registerPhy(
    EC_API_SLV_SHandle_t* pHandle,
    uint8_t phyIdx,
    uint8_t phyAddr,
    bool invertLinkPolarity,
    bool useRxLinkPin);

extern ECATSLV_API uint32_t EC_API_SLV_cbRegisterPhyLibDetect(
    EC_API_SLV_SHandle_t* pHandle,
    CUST_PHY_CBextPhyLibDetect_t cbFunc,
    void* pContext);

extern ECATSLV_API uint32_t EC_API_SLV_phyRegRead(
    EC_API_SLV_SHandle_t* pHandle,
    void* pStackContext,
    uint32_t regNum,
    uint16_t* pData);

extern ECATSLV_API uint32_t EC_API_SLV_phyRegWrite(
    EC_API_SLV_SHandle_t* pHandle,
    void* pStackContext,
    uint32_t regNum,
    uint16_t wrVal);
#endif

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_PHY_H__ */
