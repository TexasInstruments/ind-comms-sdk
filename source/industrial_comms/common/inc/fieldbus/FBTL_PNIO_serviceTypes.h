/*!
 *  \file FBTL_PNIO_serviceTypes.h
 *
 *  \brief
 *  FBTL ProfiNET slave service Types.
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

#if !(defined __FBTL_PNIO_SERVICETYPES_H__)
#define __FBTL_PNIO_SERVICETYPES_H__		1

/**

\brief Service Code Enum

This enumeration is used to identify the service.
The Service IDs are splitted into groups. The 4 bit MSB are used to idetify the group
- 0 = General Service
- 1 = EtherCAT Service
- ...

\ingroup grp_gen_service

*/
// Service 16 bit -> highest 4 Bit = Unit (General or Bus)
typedef enum FBTL_PNIO_EService
{
    FBTL_SVC_ePNIO_Unknown = 0x3000,

    FBTL_SVC_ePNIO_Max,
} FBTL_PNIO_EService_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern char* FBTL_SVC_PNIO_enumString(FBTL_PNIO_EService_t enumVal);

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_PNIO_SERVICETYPES_H__ */
