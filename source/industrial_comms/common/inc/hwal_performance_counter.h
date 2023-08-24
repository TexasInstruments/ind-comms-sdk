/*!
 *  \file hwal_performance_counter.h
 *
 *  \brief
 *  HWAL performance counter interface.
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

#if !(defined __HWAL_PERFORMANCE_COUNTER_H__)
#define __HWAL_PERFORMANCE_COUNTER_H__		1

#include <stdint.h>
#include <hwal.h>

typedef uint32_t (*HWAL_PRFCNT_init_t)               (void*                                 pHwalSpecHdl_p
                                                     ,HWAL_PRFCNT_EPrecision_t              precision_p);
typedef uint32_t (*HWAL_PRFCNT_getMpuCycles_t)       (void*                                 pHwalSpecHdl_p
                                                     ,uint32_t*                             pOverflows_p
                                                     ,uint32_t*                             pMpuClockCycles_p);
typedef uint32_t (*HWAL_PRFCNT_getTimeFromCycles_t)  (void*                                 pHwalSpecHdl_p
                                                     ,uint32_t                              overflows_p
                                                     ,uint32_t                              cyclesNum_p
                                                     ,HWAL_PRFCNT_ETimeUnit_t               timeUnit_p
                                                     ,uint32_t*                             pTimeHighDWord_p
                                                     ,uint32_t*                             pTimeLowDWord_p);
typedef uint32_t (*HWAL_PRFCNT_getDeltaTime_t)       (void*                                 pHwalSpecHdl_p
                                                     ,uint32_t                              startCyclesHighDWord_p
                                                     ,uint32_t                              startCyclesLowDword_p
                                                     ,uint32_t                              endCyclesHighDWord_p
                                                     ,uint32_t                              endCyclesLowDWord_p
                                                     ,HWAL_PRFCNT_ETimeUnit_t               timeUnit_p
                                                     ,uint32_t*                             pDeltaTimeHighDWord_p
                                                     ,uint32_t*                             pDeltaTimeLowDWord_p );
typedef uint32_t (*HWAL_PRFCNT_registerCallback_t)   (void*                                 pHwalSpecHdl_p
                                                     ,HWAL_PRFCNT_CBPrfCntOverflowHandler_t cbHandler_p);

typedef struct HWAL_PRFCNT_SHandle
{
    uint32_t                            mcuFreqInHz;
    uint32_t                            divider;
}HWAL_PRFCNT_SHandle_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern uint32_t HWAL_PRFCNT_register                            (void*                      pHwalPrfCntHandle_p);
extern uint32_t HWAL_PRFCNT_unregister                          (void*                      pHwalPrfCntHandle_p);

#if (defined __cplusplus)
}
#endif

#endif /* __HWAL_PERFORMANCE_COUNTER_H__ */

