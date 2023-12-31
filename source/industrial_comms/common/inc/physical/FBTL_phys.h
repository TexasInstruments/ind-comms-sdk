/*!
 *  \file FBTL_phys.c
 *
 *  \brief
 *  FBTL physical layer common part.
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

#if !(defined __FBTL_PHYS_H__)
#define __FBTL_PHYS_H__		1

#include <system/FBTL_sys.h>
#include <FBTL_api.h>

/*!
    \brief FBTL Channel Types

    \ingroup FBTL_PHYS
*/
typedef enum FBTL_EChanType
{
    FBTL_eCT_unknown                = 0     ///!< Initial value
   ,FBTL_eCT_irqChannel                     ///!< Interrupt/Status registers
   ,FBTL_eCT_acyclicToBus                   ///!< Acyclic channel down
   ,FBTL_eCT_acyclicFromBus                 ///!< Acyclic channel up
   ,FBTL_eCT_pdToBus                        ///!< ProcessData channel down
   ,FBTL_eCT_pdFromBus                      ///!< ProcessData channel up

   ,FBTL_eCT_max                            ///!< For arrays of type
    /// @cond INTERNAL
   ,FBTL_eCT_32BIT                  = 0xffffffff
    /// @endcond
} FBTL_EChanType_t;

/*!
    \brief FBTL channel pairing

    \ingroup FBTL_PHYS
*/
typedef struct FBTL_SChannelBar
{
    uint64_t                offset;         ///!< Offset of BAR
    uint64_t                length;         ///!< Length of BAR
} FBTL_SChannelBar_t;

/*!
    \brief Channel Definition

    \ingroup FBTL_PHYS
*/
typedef struct FBTL_SChannelDefinition
{
    uint8_t     chanType;                   ///!< Channel Type according \ref FBTL_EChanType_t
    uint8_t     reserved0;                  ///!< reserved
    uint16_t    reserved1;                  ///!< reserved
    uint32_t    reserved2;                  ///!< reserved
    uint64_t    offset;                     ///!< Offset in FBTL
    uint64_t    length;                     ///!< Length of this Channel
} FBTL_STRUCT_PACKED FBTL_SChannelDefinition_t;

/*!
    \brief Channel configuration handle

    \ingroup FBTL_PHYS
*/
typedef struct  FBTL_SChanCfgHandle
{
    void*               sysLib;             /*!< sysLib handle for OS/platform specific functions */

    uint32_t            size;               /*!< size of FBTL (shared) memory */
    uint32_t            toBusAcyclicSize;   /*!< size of acyclic channel from application to bus */
    uint32_t            fmBusAcyclicSize;   /*!< size of acyclic channel from bus to application */
    uint32_t            toBusPDSize;        /*!< size of cyclic channel from application to bus */
    uint32_t            fmBusPDSize;        /*!< size of cyclic channel from bus to application */
    FBTL_API_EPDMode_t  procDataMode;       /*!< operation mode of process data buffers \ref FBTL_API_EPDMode_t */
} FBTL_SChanCfgHandle_t;

/*!
    \brief Control/Status structure

    \details
    IRQ is performed fully handshaked:
                   RTS | TxIRQ | IrqAck|AACK    |RTS
                        _______________
    irqStatus(tx)  ____/               \________|___
                                _____________
    irqAck(rx)     ____________/             \__|___

    \ingroup FBTL_PHYS
*/
typedef struct FBTL_PHYS_SCtrlStatus
{
    volatile    uint32_t    irqStatus;              ///!< IRQ status
    volatile    uint32_t    irqAck;                 ///!< IRQ acknowledge
    volatile    uint32_t    status;                 ///!< Status of component e.g. DTK state

    volatile    uint32_t    aSpecific[5];           ///!< For bus specific use and future enhancements
} FBTL_STRUCT_PACKED FBTL_PHYS_SCtrlStatus_t;
#define STATUS_FLAG_LED0        ((uint32_t)(1u<<0u))
#define STATUS_FLAG_LED1        ((uint32_t)(1u<<1u))

typedef struct FBTL_PHYS_SHandle
{
    FBTL_SYS_EUnderlayIf_t  underlayType;
    void*                   pPhysics;
} FBTL_PHYS_SHandle_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern uint32_t FBTL_PHYS_initCtrl                  (void*                      pFbtlHandle_p
                                                    ,void*                      pFbtlConfig_p);
extern void     FBTL_PHYS_run                       (void*                      pFbtlHandle_p);

extern void     FBTL_PHYS_getCurrentStatusControl   (void*                      pFbtlHandle_p
                                                    ,FBTL_PHYS_SCtrlStatus_t*   pIrqStatus_p);

extern void     FBTL_PHYS_setCurrentStatusControl   (void*                      pFbtlHandle_p
                                                    ,FBTL_PHYS_SCtrlStatus_t*   pIrqStatus_p);

extern uint32_t FBTL_PHYS_getAcycChannelSize        (void*                      pFbtlHandle_p);

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_PHYS_H__ */
