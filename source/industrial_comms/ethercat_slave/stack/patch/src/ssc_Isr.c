/*!
 *  \file ssc_Isr.c
 *
 *  \brief
 *  Beckhoff SSC Integration ISR.
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

#include <ssc.h>
#include <ssc_Isr.h>

/* PRU */
#include <pru.h>

/* EtherCATSlave PRU */
#include <protocols/ethercat_slave/include/tiesc_pruss_intc_mapping.h>

/* BKHF */
#include <ecatappl.h>

/*! <!-- Description: -->
 *
 *  \brief
 *  Generic EtherCAT ISR
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pArgs_p     For future enhancement.
 *
 *  <!-- Group: -->
 *
 *  \ingroup ssc
 *
 * */
inline void SSC_ISR_ecat(void *pArgs_p)
{
    OSALUNREF_PARM(pArgs_p);

#ifndef ENABLE_PDI_TASK
#ifndef ENABLE_PDI_SWI
    PDI_Isr();
#else
    PDI_Swi();
#endif
#endif
}

/*! <!-- Description: -->
 *
 *  \brief
 *  EtherCAT SyncManager 0 ISR
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pArgs_p     For future enhancement.
 *
 *  <!-- Group: -->
 *
 *  \ingroup ssc
 *
 * */
inline void SSC_ISR_sync0(void *pArgs_p)
{
    OSALUNREF_PARM(pArgs_p);

    Sync0_Isr();
}

/*! <!-- Description: -->
 *
 *  \brief
 *  EtherCAT SyncManager 1 ISR
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pArgs_p     For future enhancement.
 *
 *  <!-- Group: -->
 *
 *  \ingroup ssc
 *
 * */
inline void SSC_ISR_sync1(void *pArgs_p)
{
    OSALUNREF_PARM(pArgs_p);

    Sync1_Isr();
}

/*! <!-- Description: -->
 *
 *  \brief
 *  External call interface for PDI_Isr
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  #include <ssc.h>
 *
 *  // the Call
 *  SSC_ISR_pdi();
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup ssc
 *
 * */
inline void SSC_ISR_pdi(void)
{
    PDI_Isr();
}

//*************************************************************************************************
