/*!
 *  \file FBTL_FB_etherCAT.h
 *
 *  \brief
 *  FBTL fieldbus interface EtherCAT.
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

#if !(defined __FBTL_FB_ETHERCAT_H__)
#define __FBTL_FB_ETHERCAT_H__		1

#include <stdbool.h>

#if (defined __cplusplus)
extern "C" {
#endif

extern bool FBTL_EC_getState(void* pFbtlHandle_p);
extern void FBTL_EC_run(void* pFbtlHandle_p);

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_FB_ETHERCAT_H__ */
