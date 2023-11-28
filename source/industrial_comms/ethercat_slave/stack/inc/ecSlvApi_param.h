/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API parametrization interface.
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

#if !(defined __ECSLVAPI_PARAM_H__)
#define __ECSLVAPI_PARAM_H__		1

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
#include <ecSlvApi_types.h>

#if (defined __cplusplus)
extern "C" {
#endif

extern ECATSLV_API uint32_t EC_API_SLV_setDeviceType(
    EC_API_SLV_SHandle_t* pHandle,
    EC_API_SLV_EDevType_t devType);

extern ECATSLV_API uint32_t EC_API_SLV_setErrorRegister(
    EC_API_SLV_SHandle_t *pHandle,
    EC_API_SLV_EErrorRegister_t    errorRegister);

extern ECATSLV_API uint32_t EC_API_SLV_setVendorId(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t vendorId);

extern ECATSLV_API uint32_t EC_API_SLV_setProductCode(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t productCode);

extern ECATSLV_API uint32_t EC_API_SLV_setRevisionNumber(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t revisionNumber);

extern ECATSLV_API uint32_t EC_API_SLV_setSerialNumber(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t serialNumber);

extern ECATSLV_API uint32_t EC_API_SLV_setProductName(
    EC_API_SLV_SHandle_t* pHandle,
    char* pProductName);

extern ECATSLV_API uint32_t EC_API_SLV_setGroupType(
    EC_API_SLV_SHandle_t* pHandle,
    char* pGroupType);

extern ECATSLV_API uint32_t EC_API_SLV_setHwVersion(
    EC_API_SLV_SHandle_t* pHandle,
    char* pHardwareVersion);

extern ECATSLV_API uint32_t EC_API_SLV_setSwVersion(
    EC_API_SLV_SHandle_t* pHandle,
    char* pSoftwareVersion);

extern ECATSLV_API uint32_t EC_API_SLV_setPDICfg(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t pdiControl,
    uint16_t pdiConfig);

extern ECATSLV_API uint32_t EC_API_SLV_setBootStrapMailbox(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t rxOffset,
    uint16_t rxSize,
    uint16_t txOffset,
    uint16_t txSize);

extern ECATSLV_API uint32_t EC_API_SLV_setStandardMailbox(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t rxOffset,
    uint16_t rxSize,
    uint16_t txOffset,
    uint16_t txSize);

extern ECATSLV_API uint32_t EC_API_SLV_setSyncManConfig(
    EC_API_SLV_SHandle_t* pHandle,
    uint8_t syncManIdx,
    uint16_t offset,
    uint16_t size,
    uint8_t control,
    uint8_t enable);

extern ECATSLV_API uint32_t EC_API_SLV_setSyncManErrLimit(
    EC_API_SLV_SHandle_t* pHandle,
    uint16_t smErrorLimit);

extern ECATSLV_API uint32_t EC_API_SLV_getVendorId(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t* pVendorId);

extern ECATSLV_API uint32_t EC_API_SLV_getProductCode(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t* pProductCode);

extern ECATSLV_API uint32_t EC_API_SLV_getRevisionNumber(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t* pRevisionNumber);

extern ECATSLV_API uint32_t EC_API_SLV_getSerialNumber(
    EC_API_SLV_SHandle_t* pHandle,
    uint32_t* pSerialNumber);

extern ECATSLV_API uint32_t EC_API_SLV_getProductName(
    EC_API_SLV_SHandle_t* pHandle,
    char** ppProductName);

extern ECATSLV_API uint32_t EC_API_SLV_getGroupType(
    EC_API_SLV_SHandle_t* pHandle,
    char** ppGroupType);

extern ECATSLV_API uint32_t EC_API_SLV_getSwVersion(
    EC_API_SLV_SHandle_t* pHandle,
    char** ppSwVersion);

extern ECATSLV_API uint32_t EC_API_SLV_getHwVersion(
    EC_API_SLV_SHandle_t* pHandle,
    char** ppHwVersion);

#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPI_PARAM_H__ */
