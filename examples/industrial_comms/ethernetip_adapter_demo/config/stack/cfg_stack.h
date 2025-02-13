/*!
 *  \file cfg_stack.h
 *
 *  \brief
 *  Stack configuration.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2022 Texas Instruments Incorporated
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

#ifndef CFG_STACK_H_INC
#define CFG_STACK_H_INC

// HWAL configuration
#define CFG_STACK_HWAL_WATCHDOG_TASK_PRIO OSAL_TASK_Prio_HWAL_WATCHDOG // Value change will have no impact.
#define CFG_STACK_HWAL_LICENSE_TASK_PRIO  OSAL_TASK_Prio_HWAL_LICENSE  // Value change will have no impact.

// LWIP configuration
#define CFG_STACK_LWIP_TCPIP_TASK_PRIO OSAL_TASK_Prio_EIP_LWIP_TCPIP   // Value change will have no impact.

// CIP configuration
#define CFG_STACK_CIP_INSTANCES_MAX   256 // Maximum number of instances defined as CIP object. Set not lower as 70.

// EtherNet/IP Adapter configuration
#define CFG_STACK_CYCLIC_IO_TASK_PRIO OSAL_TASK_Prio_EIP_CYCLICIO
#define CFG_STACK_PACKET_TASK_PRIO    OSAL_TASK_Prio_EIP_PACKET
#define CFG_STACK_STATISTIC_TASK_PRIO OSAL_TASK_Prio_EIP_STATISTIC

// TimeSync Configuration
#define CFG_STACK_TIMESYNC_DEL_TASK_PRIO        OSAL_TASK_Prio_EIP_TIMESYNC_DEL
#define CFG_STACK_TIMESYNC_TS_TASK_PRIO         OSAL_TASK_Prio_EIP_TIMESYNC_TS
#define CFG_STACK_TIMESYNC_NRT_TASK_PRIO        OSAL_TASK_Prio_EIP_TIMESYNC_NRT
#define CFG_STACK_TIMESYNC_BACKGROUND_TASK_PRIO OSAL_TASK_Prio_EIP_TIMESYNC_BAC

#define CFG_STACK_LLDP_RX_TASK_PRIO         OSAL_TASK_Prio_Normal
#define CFG_STACK_LLDP_NEIGHBOR_DEVICES_MAX 16

// Drivers configuration
#define CFG_STACK_DRIVERS_ETHPHY_MDIX_TASK_PRIO OSAL_TASK_Prio_EIP_PHYMDIX

// ACD - Address Conflict Detection
#define CFG_STACK_ACD_INTIAL_DELAY 200

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __cplusplus
}
#endif

#endif // CFG_STACK_H_INC
