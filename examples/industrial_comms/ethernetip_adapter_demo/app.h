/*!
 *  \file app.h
 *
 *  \brief
 *  Declarations of EtherNet/IP&trade; Adapter Example Application.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 None.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  <ol>
 *  <li>Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer./<li>
 *  <li>Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.</li>
 *  <li>Neither the name of the copyright holder nor the names of its contributors
 *  may be used to endorse or promote products derived from this software without
 *  specific prior written permission.</li>
 *  </ol>
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 *  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 */


#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

#define EI_APP_STACK_MAIN_TASK_STACK_SIZE_BYTE    0x1000
#define EI_APP_STACK_MAIN_TASK_STACK_SIZE         (EI_APP_STACK_MAIN_TASK_STACK_SIZE_BYTE/sizeof(configSTACK_DEPTH_TYPE))

typedef struct APP_SApplication
{
    OSAL_TASK_Priority_t taskPrio;
}APP_SApplication_t;

typedef struct APP_SHwal
{
    OSAL_TASK_Priority_t taskPrioWatchDog;
    OSAL_TASK_Priority_t taskPrioLicense;
}APP_SHwal_t;

typedef struct APP_SLwip
{
    OSAL_TASK_Priority_t taskPrio;
}APP_SLwip_t;

typedef struct APP_SAdapter
{
    OSAL_TASK_Priority_t taskPrioCyclicIo;
    OSAL_TASK_Priority_t taskPrioPacket;
    OSAL_TASK_Priority_t taskPrioStatistic;

    OSAL_TASK_Priority_t taskPrioPtpDelayRqTx;      /* Task priority for TX Delay Request */
    OSAL_TASK_Priority_t taskPrioPtpTxTimeStamp;    /* Task priority for TX Time Stamp P1 and P2 */
    OSAL_TASK_Priority_t taskPrioPtpNRT;            /* Task priority for NRT */
    OSAL_TASK_Priority_t taskPrioPtpBackground;     /* Task priority for Background thread */

    OSAL_TASK_Priority_t taskPrioLldpReceive;       /* Task priority for receive thread */
}APP_SAdapter_t;

typedef struct APP_SParams
{
    APP_SApplication_t      application;
    APP_SHwal_t             hwal;
    APP_SLwip_t             lwip;
    APP_SAdapter_t          adapter;
    CUST_DRIVERS_SInit_t    customDrivers;

#if (defined CPU_LOAD_MONITOR) && (1==CPU_LOAD_MONITOR)
    CMN_CPU_API_SParams_t   cpuLoad;
    APP_WEBSRV_SParams_t    webServer;
#endif
}APP_SParams_t;

typedef struct APP_SInstance
{
    APP_SParams_t                   config;

    void*                           remoteHandle;
}APP_SInstance_t;

extern void EI_APP_TASK_main (void* pvTaskArg_p);

#ifdef  __cplusplus
}
#endif

#endif // APP_H
