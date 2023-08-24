/*!
 *  \file FBTL_channel.h
 *
 *  \brief
 *  FBTL channel access interface.
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

#if !(defined __FBTL_CHANNEL_H__)
#define __FBTL_CHANNEL_H__		1

#include <FBTL_api.h>
#include <FBTL_queue.h>

#if (defined __cplusplus)
extern "C" {
#endif

extern bool     FBTL_CHAN_isAcycWriteFree   (void*                  pFbtlHandle_p);
extern bool     FBTL_CHAN_isAcycReadAvail   (void*                  pFbtlHandle_p);
extern uint32_t FBTL_CHAN_writeAcyc         (void*                  pFbtlHandle_p
                                            ,void*                  pHeader_p
                                            ,uint8_t*               pData_p
                                            ,uint32_t               length_p);
extern uint32_t FBTL_CHAN_readAcyc          (void*                  pFbtlHandle_p);

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_CHANNEL_H__ */
