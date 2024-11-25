/*!
 *  \file ssc_Isr.h
 *
 *  \brief
 *  Beckhoff SSC Integration ISR interface.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#if !(defined __SSC_ISR_H__)
#define __SSC_ISR_H__       1

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
