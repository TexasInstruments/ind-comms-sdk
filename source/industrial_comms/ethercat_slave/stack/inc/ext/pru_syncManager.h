/*!
 *  \file pru_syncManager.h
 *
 *  \brief
 *  PRU Integration: SyncManager interface.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2024 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#if !(defined __PRU_SYNCMANAGER_H__)
#define __PRU_SYNCMANAGER_H__		1

#include <osal.h>

#define    MAX_SYNC_MAN                         8
#define    SIZEOF_SM_REGISTER                   8

typedef struct SYNCMAN_SProperties {
    uint16_t    physical_start_addr;
    uint16_t    length;
} SYNCMAN_SProperties_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern void     PRU_SM_zeroInit             (void);
extern uint16_t PRU_SM_endAddress           (uint8_t                    syncManNo_p);
extern uint16_t PRU_SM_startAddress         (uint8_t                    syncManNo_p);
extern uint16_t PRU_SM_length               (uint8_t                    syncManNo_p);
extern int16_t  PRU_SM_getIndex             (uint16_t                   address_p
                                            ,uint16_t                   length_p);
extern uint8_t  PRU_SM_PDI_configOngoing    (void);
extern void     PRU_SM_setProperties        (uint8_t                    syncMan_p
                                            ,uint16_t                   address_p
                                            ,uint16_t                   length_p);

#if (defined __cplusplus)
}
#endif

#endif /* __PRU_SYNCMANAGER_H__ */
