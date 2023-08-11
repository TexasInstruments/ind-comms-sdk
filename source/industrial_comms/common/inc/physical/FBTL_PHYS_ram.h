/*!
 *  \file FBTL_PHYS_ram.h
 *
 *  \brief
 *  FBTL physical layer for RAM like underlay.
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

#if !(defined __FBTL_PHYS_RAM_H__)
#define __FBTL_PHYS_RAM_H__		1

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <physical/FBTL_phys.h>
#include <system/FBTL_sys.h>

#define FBTL_RAM_VERSION        ((uint32_t)(0x02000000u)) /* 1= DPR, 2= FBTL v1 */
#define FBTL_READY_FLAG         ((uint32_t)(0xAA5555AAu))

#define FBTL_IRQMANINGUARDSHM   ((uint64_t)(0x55aacc3355aacc33ull))
#define FBTL_IRQMANOUTGUARDSHM  ((uint64_t)(0x33ccaa5533ccaa55ull))

/*!
    \brief RAM like Header structure

    \ingroup FBTL_PHYS
*/
typedef struct FBTL_PHYS_RAM_SHeader
{
    uint32_t    version;                ///!< Version of RAM structure
    uint32_t    appReady;               ///!< Application ready signal
    uint32_t    size;                   ///!< Size of RAM / can be changed by app cpu
    uint32_t    chanListOffset;         ///!< Offset to a list of \ref DPR_SChannelDef
} FBTL_STRUCT_PACKED FBTL_PHYS_RAM_SHeader_t;

/*!
    \brief Control/Status buffer control structure

    \ingroup FBTL_PHYS
*/
typedef struct FBTL_PHYS_RAM_SIrqManager
{
    volatile    uint64_t                    inGuard;            ///!< IRQ guard

    volatile    uint32_t                    latest;             ///!< Simpson latest
    volatile    uint32_t                    reading;            ///!< Simpson reading

    volatile    uint32_t                    aSlot[2];           ///!< Simpson buffer matrix

    volatile    FBTL_PHYS_SCtrlStatus_t     aCtrlStatus[2][2];  ///!< Control status buffers

    volatile    uint64_t                    outGuard;           ///!< IRQ guard
} FBTL_STRUCT_PACKED FBTL_PHYS_RAM_SIrqManager_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern uint32_t FBTL_PHYS_RAM_initCtrl                  (void*                      pFbtlHandle_p
                                                        ,void*                      pFbtlConfig_p);
extern void     FBTL_PHYS_RAM_run                       (void*                      pFbtlHandle_p);

extern void     FBTL_PHYS_RAM_getCurrentStatusControl   (void*                      pFbtlHandle_p
                                                        ,FBTL_PHYS_SCtrlStatus_t*   pIrqStatus_p);

extern void     FBTL_PHYS_RAM_setCurrentStatusControl   (void*                      pFbtlHandle_p
                                                        ,FBTL_PHYS_SCtrlStatus_t*   pIrqStatus_p);
extern uint32_t FBTL_PHYS_RAM_getAcycChannelSize        (void*                      pFbtlHandle_p);

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_PHYS_RAM_H__ */
