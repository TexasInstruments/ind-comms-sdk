/*!
 *  \file ssc_Isr.h
 *
 *  \brief
 *  Beckhoff SSC Integration ISR interface.
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

#if !(defined __SSC_ISR_H__)
#define __SSC_ISR_H__		1

#include <osal.h>
#include <ssc.h>

#if (defined __cplusplus)
extern "C" {
#endif

extern BKHFSSC_API void SSC_ISR_pdi     (void);
extern BKHFSSC_API void SSC_ISR_sync0   (void*  pArgs_p);
extern BKHFSSC_API void SSC_ISR_sync1   (void*  pArgs_p);
extern BKHFSSC_API void SSC_ISR_ecat    (void*  pArgs_p);

#if (defined __cplusplus)
}
#endif

#endif /* __SSC_ISR_H__ */
