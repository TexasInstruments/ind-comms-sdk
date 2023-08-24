/*!
 *  \file FBTL_IOLM_serviceTypes.h
 *
 *  \brief
 *  FBTL IO-Link master service Types.
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

#if !(defined __FBTL_IOLM_SERVICETYPES_H__)
#define __FBTL_IOLM_SERVICETYPES_H__		1

/**

\brief Service Code Enum

This enumeration is used to identify the service.
The Service IDs are splitted into groups. The 4 bit MSB are used to idetify the group
- 0 = General Service
- 1 = EtherCAT Service
- ...
- 4 = IOLM SMI Service

\ingroup grp_gen_service

*/
// Service 16 bit -> highest 4 Bit = Unit (General or Bus)
typedef enum FBTL_IOLM_EService
{
    FBTL_SVC_eIOLM_Unknown = 0x4000,

    FBTL_SVC_eIOLM_SMI_genericReq,
    FBTL_SVC_eIOLM_SMI_cbGeneric,

    FBTL_SVC_eIOLM_Max,
} FBTL_IOLM_EService_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern char* FBTL_SVC_IOLM_enumString(FBTL_IOLM_EService_t enumVal);

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_IOLM_SERVICETYPES_H__ */
