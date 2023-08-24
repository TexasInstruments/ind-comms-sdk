/*!
 *  \file ecSlvApi_SyncMan.h
 *
 *  \brief
 *  EtherCAT Slave SyncManager configuration.
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

#if !(defined __ECSYNCMAN_H__)
#define __ECSYNCMAN_H__		1

#if (defined __cplusplus)
extern "C" {
#endif

#include <osal.h>

typedef struct EC_SLV_API_SSyncManDesc {
    uint16_t   smOffset;
    uint16_t   smSize;
    uint8_t    smCtrlRegister;
    uint8_t    smEnableSyncManager;
} EC_SLV_API_SSyncManDesc_t;

#if (defined __cplusplus)
}
#endif

#endif /* __ECSYNCMAN_H__ */

