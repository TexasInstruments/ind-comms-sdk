/*!
 *  \file ssc_cfg.h
 *
 *  \brief
 *  Beckhoff SSC Integration configuration.
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

#if !(defined __SSC_CFG_H__)
#define __SSC_CFG_H__		1


/* Uncomment following to enable DC feature of system time compensation via PDI interface instead of ECAT interface
   for synchronizing two independent EtherCAT networks */
//#define SYSTEM_TIME_PDI_CONTROLLED

/*Comment to following to enable PDI  ISR and SYNC ISR in HWI context */
#define  ENABLE_PDI_TASK
/* #define ENABLE_PDI_SWI */
#define  ENABLE_SYNC_TASK

#if (defined __cplusplus)
extern "C" {
#endif


#if (defined __cplusplus)
}
#endif

#endif /* __SSC_CFG_H__ */
