/*!
 *  \file ecSlvApi_pdo.h
 *
 *  \brief
 *  EtherCAT User API MDIO interface.
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

#if !(defined __ECSLVAPI_MDIO_H__)
#define __ECSLVAPI_MDIO_H__		1

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
extern ECATSLV_API uint32_t EC_API_SLV_stackInsertPruFirmware(
    uint32_t *pFirmwareIn,
    uint32_t lenFirmwareIn,
    uint32_t *pFirmwareOut,
    uint32_t lenFirmwareOut);

extern ECATSLV_API uint32_t EC_API_SLV_stackInsertMdioManualFirmware(
    uint32_t *pFirmware,
    uint32_t lenFirmware);

extern ECATSLV_API uint32_t EC_API_SLV_enableMdioManualMode(
    EC_API_SLV_SHandle_t *pHandle,
    uint32_t manualMdioAddress,
    uint32_t firmwareConfig);
#endif

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_MDIO_H__ */
