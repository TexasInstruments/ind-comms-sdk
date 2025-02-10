/*!
 *  \file pru_Profinet.h
 *
 *  \brief
 *  PRU Integration: Profinet specific interface.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#if !(defined PRU_PROFINET_H)
#define PRU_PROFINET_H 1

#include <stdint.h>
#include "pru.h"


#include <industrial_comms/profinet_device/icss_fwhal/PN_Handle.h>
#include <industrial_comms/profinet_device/icss_fwhal/iPtcpDrv.h>

#define PRU_PN_MAC_ADDR_LEN 6


#define PRU_PN_TX_QUEUE_0 0 //! used for profinet frames High Priority
#define PRU_PN_TX_QUEUE_1 1 //! used for profinet frames Low Priority
#define PRU_PN_TX_QUEUE_2 2 //! used for IP related frames High Priority
#define PRU_PN_TX_QUEUE_3 3 //! used for IP related frames Low Priority

#define PRU_PN_TX_PORT_1  1 //! Ethernet Port to send telegramm
#define PRU_PN_TX_PORT_2  2 //! Ethernet Port to send telegramm

typedef enum
{
    PRU_PN_enPORT_0,
    PRU_PN_enPORT_1,
    PRU_PN_enPORT_2,
    PRU_PN_enPORT_FORCE32BIT = 0xffffffff,
} PRU_PN_EEthernetPort;

typedef enum
{
    PRU_PN_enLST_INVALID,
    PRU_PN_enLST_UP,
    PRU_PN_enLST_DOWN,
    PRU_PN_enLST_FORCE32BIT = 0xffffffff
} PRU_PN_ELinkState;

typedef enum
{
    PRU_PN_enPHS_INVALID,
    PRU_PN_enPHS_10MB,
    PRU_PN_enPHS_100MB,
    PRU_PN_enPHS_1GB,
    PRU_PN_enPHS_FORCE32BIT = 0xffffffff
} PRU_PN_EPhySpeed;

typedef enum
{
    PRU_PN_enPHM_INVALID,
    PRU_PN_enPHM_HALF,
    PRU_PN_enPHM_FULL,
    PRU_PN_enPHM_FORCE32BIT = 0xffffffff
} PRU_PN_EPhyDuplexMode;

typedef void (*PRU_PN_TInterruptFunc)(void *pParam);
typedef void (
    *PRU_PN_TLinkInterruptFunc)(void *pParam, PRU_PN_ELinkState linkP1, PRU_PN_ELinkState linkP2);
typedef void (*PRU_PN_TRxFrameInterruptFunc)(void *pParam, int32_t packetLength, uint32_t queue);
typedef void (*PRU_PN_ptcpDelayUpdate_t)(
    void                *pThis,
    PRU_PN_EEthernetPort port,
    uint32_t             rxDelayLocal,
    uint32_t             rxDelayRemote,
    uint32_t             txDelayLocal,
    uint32_t             txDelayRemote,
    uint32_t             lineDelay);
typedef void (*PRU_PN_ptcpSyncStatus_t)(void *pThis, syncState_t syncState, ptcpSyncInfo_t *ptSyncInfo);

typedef struct PRU_PN_SEthPhyInstance
{
    ETHPHY_Config *pCfg;   /* ETHPHY board configuration pointer */
    ETHPHY_Handle  handle; /* ETHPHY handler */
} PRU_PN_SEthPhyInstance_t;

typedef struct PRU_PN_SEthPhys
{
    PRU_PN_SEthPhyInstance_t instance[2];     /* ETHPHY instance array (2 ETHPHY's per one PRU-ICSS
                                                 block) */
    OSAL_TASK_Priority_t taskPrioPhyMdixTask; /* PHY MDIX task priority */
} PRU_PN_SEthPhys_t;

typedef struct PRU_PN_SPruLoadParameter
{
    uint8_t             aMacAddr[PRU_PN_MAC_ADDR_LEN]; /* Device MAC address */
    uint32_t            pruIcssCfgId; /* PRU-ICSS block enumeration id listed in SysConfig */
    PRU_PN_SEthPhys_t   ethPhys;      /* ETHPHY objects managed by PRU-ICSS block */
    PRU_SMdioFirmware_t drivers;      /* TI driver parameters */
} PRU_PN_TPruLoadParameter;

typedef struct PRU_PN_SPortState
{
    PRU_PN_ELinkState     enLink;
    PRU_PN_EPhySpeed      enSpeed;
    PRU_PN_EPhyDuplexMode enMode;
} PRU_PN_TPortState;

typedef struct PRU_PN_SPortStatistic
{
    uint32_t rxOctets;
    uint32_t txOctets;
    uint32_t rxDiscards;
    uint32_t txDiscards;
    uint32_t rxErrors;
    uint32_t txErrors;
} PRU_PN_SPortStatistic_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern uint32_t PRU_PN_loadPru(PRU_PN_TPruLoadParameter *pParam);
extern void     PRU_PN_registerRxFrameCb(PRU_PN_TRxFrameInterruptFunc cbFunc, void *pParam);
extern void     PRU_PN_registerPtcpDelayUpdate(PRU_PN_ptcpDelayUpdate_t cbDelay, void *pThis);
extern void     PRU_PN_registerPtcpSyncStatus(PRU_PN_ptcpSyncStatus_t cbDelay, void *pThis);
extern void     PRU_PN_registerTxPpmFrameCb(PRU_PN_TInterruptFunc cbFunc, void *pParam);
extern void     PRU_PN_registerRxCpmFrameCb(PRU_PN_TInterruptFunc cbFunc, void *pvParam);
extern void     PRU_PN_registerDhtEventCb(PRU_PN_TInterruptFunc cbFunc, void *pParam);
extern void     PRU_PN_registerPtcpEventCb(PRU_PN_TInterruptFunc cbFunc, void *pParam);
extern void     PRU_PN_registerLinkStateChangeCb(PRU_PN_TLinkInterruptFunc cbFunc, void *pParam);
extern void     PRU_PN_registerIsomEventCb(PRU_PN_TInterruptFunc cbFunc, void *pParam);
extern uint32_t PRU_PN_getIoFrame(
    uint32_t  queue,
    uint8_t  *pBuffer,
    uint32_t *recData,
    uint32_t  maxBufLen,
    uint8_t  *port);
extern uint32_t PRU_PN_sendTxPacket(
    const uint8_t       *srcAddress,
    PRU_PN_EEthernetPort port,
    uint8_t              queuePriority,
    uint16_t             lengthOfPacket);
extern uint32_t  PRU_PN_getPortState(PRU_PN_EEthernetPort port, PRU_PN_TPortState *pState);
extern PN_Handle PRU_PN_getPnHandle(void);
extern PRU_PN_EEthernetPort PRU_PN_getPortFromMacAddr(const uint8_t *macAddr);
extern uint32_t PRU_PN_getPortStatistics(PRU_PN_EEthernetPort port, PRU_PN_SPortStatistic_t *state);

#if (defined __cplusplus)
}
#endif

#endif // PRU_PROFINET_H
