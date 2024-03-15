/*
 *  Copyright (c) Texas Instruments Incorporated 2021
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file     intercore.h
 *
 * \brief    Intercore transport interface data structures.
 */

#ifndef INTERCORE_H_
#define INTERCORE_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <drivers/ipc_rpmsg/ipc_rpmsg_priv.h>
#include "include/ShdMemCircularBufferP_nortos.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

#define IC_ENDPT_NAME_LEN_MAX    128

/*! MCU2_0 to MCU2_1 TX queue */
#define ICQ_R5_0_0_TO_R5_0_1    (0U)
/*! MCU2_1 to MCU2_0 TX queue */
#define ICQ_R5_0_1_TO_R5_0_0    (1U)
/*! MCU2_0 to A72 TX queue */
#define ICQ_R5_0_0_TO_A53       (2U)
/*! A72 to MCU2_0 TX queue */
#define ICQ_A53_TO_R5_0_0       (3U)
/*! Total number of queues */
#define ICQ_MAX_NUM_QUEUES      (4U)
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/*!
 * \brief Intercore transport object
 *
 * \details
 * This structure stores the information for an inter-core transport
 * object instance
 */
typedef struct Ic_Object_s
{
    /*! Self core ID */
    uint32_t selfCoreId;

    /*! Remote core ID */
    uint32_t remoteCoreId;

    /*! Owner coreId of the Ic region */
    uint32_t ownerId;

    /*! Number of remote cores */
    uint32_t numRemoteProcs; 

    /*! Our TX-Q ID */
    uint32_t txQId;

    /*! Our shared mem Tx-Q handle */
    ShdMemCircularBufferP_Handle shmTxQ;

    /*! Our RX-Q ID (=peer core's TX-Q ID) */
    uint32_t rxQId;

    /*! Our shared mem Rx-Q handle */
    ShdMemCircularBufferP_Handle shmRxQ;

    /*! Our IPC end point ID */
    uint32_t myReqEndPtId;
    
    /*! Our IPC end point ID */
    uint32_t myEndPtId;

    /*! Our IPC end point name */
    char myEndPtName[IC_ENDPT_NAME_LEN_MAX];

    /*! Our IPC end point handle */
    RPMessage_Object localEndPt_Handle;

    /*! Peer IPC end point name */
    char remoteEndPtName[IC_ENDPT_NAME_LEN_MAX];
    
    /*! Peer IPC end point handle */
    RPMessage_Object remoteEndPt_Handle;

    /*! No. of TX packets sent */
    uint32_t numTxPktsSent;
    
    /*! No. of pkts send before the receiver is notified */
    uint32_t pktNotifyThresh;
    
    /*! No. of packets pending on the RX queue */
    uint32_t numRxPktsPending;

    /*! Flag to signal init complete */
    bool initComplete; 

} Ic_Object, *Ic_Object_Handle;

#ifdef __cplusplus
}
#endif

#endif /* INTERCORE_H */
