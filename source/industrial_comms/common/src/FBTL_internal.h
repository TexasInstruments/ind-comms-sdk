/*!
 *  \file FBTL_internal.h
 *
 *  \brief
 *  FBTL internal interface.
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

#if !(defined __FBTL_INTERNAL_H__)
#define __FBTL_INTERNAL_H__		1

#include <FBTL_api.h>
#include <FBTL_queue.h>
#include <FBTL_service.h>
#include <system/FBTL_sys.h>
#include <physical/FBTL_phys.h>

#define TRIPLEBUFFER_ATOMIC     0

#if (defined TRIPLEBUFFER_ATOMIC) && (1==TRIPLEBUFFER_ATOMIC)
#include <stdatomic.h>
#endif

#define FUNCLOC /*static*/

#define FBTL_NUM_OF_PENDING_REQ  0x20

#ifdef FBTL_CPU_COMMUNICATION
// todo: maximum one protocoll support in the firmware

// determine protocoll
#if (defined FBTL_EC_SLV_SUPPORT) && (FBTL_EC_SLV_SUPPORT==1)
#define FBTL_PROTOCOL FBTL_eSRV_EtherCAT
#elif (defined FBTL_IOLM_SUPPORT) && (FBTL_IOLM_SUPPORT==1)
#define FBTL_PROTOCOL FBTL_eSRV_IoLinkMaster
#else
#define FBTL_PROTOCOL FBTL_eSRV_General
#endif

#else
// application cpu
#endif

/*!
    \brief FBTL interrupt handling definition

\ingroup FBTL_API
*/
typedef struct FBTL_API_SIrqDefinition
{
    FBTL_API_EInterruptStatus_t     interruptMask;              /*!< interrupt flag (single bit set) */
    void*                           pIrqContext;                /*!< IRQ handler context */
    FBTL_API_CBInterruptHandler_t   cbIrqHandler;               /*!< IRQ handler function */
    void*                           pIrqSignal;                 /*!< external IRQ handler signal */
} FBTL_API_SIrqDefinition_t;

/*!
    \brief FBTL instance handle

    \ingroup FBTL_API
*/
typedef struct FBTL_SHandle
{
    void*                           sysIf;                      ///!<   System underlay library instance

    /* underlay locations */
    FBTL_SChannelBar_t              metaBar;                    ///!<   RAM header BAR
    FBTL_SChannelBar_t              toBusStatCtrlBar;           ///!<   ToBus StatusControl BAR
    FBTL_SChannelBar_t              fromBusStatCtrlBar;         ///!<   FromBus StatusControl BAR
    FBTL_SChannelBar_t              channelDescBar;             ///!<   Channel descriptors BAR

    FBTL_SChannelDefinition_t       channelDesc[FBTL_eCT_max];  ///!<   Channel descriptors

    /* IRQ directional bars */
    FBTL_SChannelBar_t*             pReadStatCtrlBar;           ///!<   Directioned BAR read
    FBTL_SChannelBar_t*             pWriteStatCtrlBar;          ///!<   Directioned BAR write

    FBTL_SChannelDefinition_t*      pReadAcycBar;               ///!<   Acyclic Read Channel BAR
    FBTL_SChannelDefinition_t*      pWriteAcycBar;              ///!<   Acyclic Write Channel BAR
    FBTL_SChannelDefinition_t*      pReadCycBar;                ///!<   Cyclic Read Channel BAR
    FBTL_SChannelDefinition_t*      pWriteCycBar;               ///!<   Cyclic Write Channel BAR

#if (defined FBTL_USE_INTERRUPT) && (1==FBTL_USE_INTERRUPT)
    void*                           triggerRun;                 ///!<   Run trigger
#endif
    void*                           protectRun;                 ///!<   do not interrupt protectRun
#if (defined REQUIRE_IRQ_LOCK) && (1==REQUIRE_IRQ_LOCK)
    void*                           protectIrqSet;              ///!<   do not interrupt IRQ set
#endif

    void*                           pLocalRxIrq;                ///!<   Receive channel IRQ
    void*                           pLocalTxIrq;                ///!<   Transmit channel IRQ

    FBTL_PHYS_SCtrlStatus_t         ourIrqStatusControl;        ///!<   Local IRQ Status
    FBTL_PHYS_SCtrlStatus_t         remIrqStatusControl;        ///!<   Remote IRQ Status

    FBTL_API_SIrqDefinition_t       aIrqHandlerList[0x20];      ///!<   Interrupt handler list

    FBTL_SVC_SElement_t             aAcylicEntries[FBTL_NUM_OF_PENDING_REQ]; ///!< Amount of sync pending requests

    FBTL_QUEUE_SHeader_t            unusedQueue;                ///!<   unused elements
    FBTL_QUEUE_SHeader_t            sendQueue;                  ///!<   services to send
    FBTL_QUEUE_SHeader_t            timerQueue;                 ///!<   services enqueued for timeout
    FBTL_QUEUE_SHeader_t            pendingQueue;               ///!<   services waiting for response
    FBTL_QUEUE_SHeader_t            receivedQueue;              ///!<   services received
    FBTL_QUEUE_SHeader_t            serviceQueue;               ///!<   services to work
    FBTL_QUEUE_SHeader_t            slowServiceQueue;           ///!<   slow services to work
    FBTL_QUEUE_SHeader_t            doneQueue;                  ///!<   services done

    FBTL_SVC_SElement_t*            curReadObject;              ///!<   Current read object (sending)
    FBTL_SVC_SElement_t*            curWriteObject;             ///!<   Current write object (receiving)

    struct FBTL_API_SProcData
    {
        FBTL_API_EPDMode_t          procDataMode;               ///!<   Cyclic process data mode

        bool                        pdInDataDirect;             ///!<   Current process data in is direct access
        uint8_t*                    pdInData;                   ///!<   Current process data in
        uint64_t                    pdInDataOffset;             ///!<   Current process data in offset

        bool                        pdOutDataDirect;            ///!<   Current process data out is direct access
        uint8_t*                    pdOutData;                  ///!<   Current process data out
        uint64_t                    pdOutDataOffset;            ///!<   Current process data out offset

        void*                       pProcessDataHandlerCtxt;    ///!<   Process data changed handler context
        FBTL_SVC_CBpdChange_t       cbProcessDataHandler;       ///!<   Process data changed handler

        uint32_t*                   pStackState;                ///!<   Application interface state offload
        FBTL_SVC_CBledChange_t      cbLedChange;                ///!<   LED update callback
    }                               procData;

    struct FBTL_API_SServiceHandler
    {
        FBTL_SVC_CBdataReceive_t    generalSvcHandler[FBTL_eService_GeneralMax - FBTL_eService_unknown];
                                                                ///!<   Service handler for general services
#if (defined FBTL_EC_SLV_SUPPORT) && (FBTL_EC_SLV_SUPPORT == 1)
        FBTL_SVC_CBdataReceive_t    ecSlvSvcHandler[FBTL_SVC_eECSLV_Max - FBTL_SVC_eECSLV_Unknown];
                                                                ///!<   Service handler for EtherCAT slave
        void*                       pEcSlvHandle;               ///!<   EtherCAT slave handle
        //bool                      usrApplRunRegistered;       ///!<   hold whether a user application run callback is registered
        void*                       pSdoCbHandler;              ///<!   SDO Callback context
#endif
#if (defined FBTL_PNIO_SUPPORT) && (FBTL_PNIO_SUPPORT == 1)
        FBTL_SVC_CBdataReceive_t    pnioSlvSvcHandler[FBTL_SVC_ePNIO_Max - FBTL_SVC_ePNIO_Unknown];
                                                                ///!<   Service handler for ProfiNET slave
        void*                       pPnioHandle;                ///!<   ProfiNET handle
#endif
#if (defined FBTL_ENIP_SUPPORT) && (FBTL_ENIP_SUPPORT == 1)
        FBTL_SVC_CBdataReceive_t    enipSlvSvcHandler[FBTL_SVC_eENIP_Max - FBTL_SVC_eENIP_Unknown];
                                                                ///!<   Service handler for EtherNET/IP slave
        void*                       pEnipADPHandle;             ///!<   EtherNET/IP ADP handle
        void*                       pEnipCIPHandle;             ///!<   EtherNET/IP CIP handle
#endif
#if (defined FBTL_IOLM_SUPPORT) && (FBTL_IOLM_SUPPORT == 1)
        FBTL_SVC_CBdataReceive_t    iolmSvcHandler[FBTL_SVC_eIOLM_Max - FBTL_SVC_eIOLM_Unknown];
        ///!<   Service handler for IO-Link Master
        void*                       pIolmHandle;                ///!<   IO-Link Master handle, currently not used
#endif
        void*                       pSyncSignal;                ///!<   Synchronized application trigger
        void*                       pSyncFuncHandlerCtxt;       ///!<   Synchronized application context
        FBTL_SVC_CBsyncFunc_t       cbSyncFunctionHandler;      ///!<   Synchronized application call

        void*                       pIsSlowServiceFuncCtxt;     ///!<   Slow service detection context
        FBTL_SVC_CBisSlowFunc_t     cbIsSlowServiceFunc;        ///!<   Slow service detection call
    }                               serviceHandler;             ///!<   Different service handlers

    uint32_t                        nextTransmitService;        ///!<   storage to determine next service ident

    /** @cond(DOCINTERNAL) */
    struct FBTL_SHandle*            next;                       ///!<   Multi instance keeper chain next
    struct FBTL_SHandle*            prev;                       ///!<   Multi instance keeper chain prev
    /** @endcond */
} FBTL_SHandle_t;

#if (defined __cplusplus)
extern "C" {
#endif

/*extern uint32_t                 FBTL_API_receiveAcycMessage (FBTL_SHandle_t* pHandle_p, uint8_t* pData_p, uint32_t length_p);*/
extern FBTL_SVC_CBdataReceive_t FBTL_API_getCallback        (FBTL_SHandle_t* pFbtlHandle_p, uint16_t service_p);
extern void                     FBTL_API_releaseObject      (FBTL_SHandle_t* pFbtlHandle_p, FBTL_SVC_SElement_t* pObject_p);
extern bool                     FBTL_API_procInterrupt      (FBTL_SHandle_t* pFbtlHandle_p);
extern void                     FBTL_API_emitInterrupt      (FBTL_SHandle_t* pFbtlHandle_p);
void                            FBTL_API_internalTriggerRun (void* pFbtlHandle_p);

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_INTERNAL_H__ */
