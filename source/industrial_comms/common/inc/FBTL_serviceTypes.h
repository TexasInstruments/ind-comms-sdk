/*!
 *  \file FBTL_serviceTypes.h
 *
 *  \brief
 *  FBTL global / generic services.
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

#if !(defined __FBTL_SERVICETYPES_H__)
#define __FBTL_SERVICETYPES_H__		1
/**

\brief Service Code Enum for global services

This enumeration is used to identify the service.
The Service IDs are splitted into groups. The 4 bit MSB are used to idetify the group
- 0 = General Service
- 1 = EtherCAT Service
- ...

\ingroup grp_gen_service

*/
// Service 16 bit -> highest 4 Bit = Unit (General or Bus)
typedef enum FBTL_EService
{
    /**
    \brief Unknown Service

    Reserved for transmission and used as Generic callback indicator
    */
    FBTL_eService_unknown = 0,

    /**
    \brief Setup the used bus

    use \ref FBTL_EServiceType
    */
    FBTL_eService_setBus,

    /**
    \brief Set CPU Speed

    */
    FBTL_eService_setCpuSpeed,

    /**
    \brief Get CPU Speed

    */
    FBTL_eService_getCpuSpeed,

    /**
    \brief Set Process Data
    */
    //FBTL_eService_setProcessData,

    /**
    \brief Get Process Data
    */
    //FBTL_eService_getProcessData,

    /**
    \brief Trigger Processdata exchange
    */
    FBTL_eService_pdExchange,

    /**
    \brief Initialize the API.
    */
    FBTL_eService_init,

    /**
    \brief Start the Protocol Stack
    */
    FBTL_eService_start,

    /**
    \brief Stop the Protocol Stack
    */
    FBTL_eService_stop,

    /**
    \brief Delete the API
    */
    FBTL_eService_delete,

    /**
    \brief Set Processdata configuration

    */
    FBTL_eService_setPdConfig,

    /**
    \brief Virtual MAC tunnel

    */
    FBTL_eService_ethernet,

    FBTL_eService_GeneralMax,
} FBTL_EService_t;

#if (defined __cplusplus)
extern "C" {
#endif

/* extern void func(void); */

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_SERVICETYPES_H__ */
