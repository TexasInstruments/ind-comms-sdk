/*!
 *  \example ecSlvSimple.h
 *
 *  \brief
 *  EtherCAT<sup>&reg;</sup> Slave Example interface.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 KUNBUS GmbH.
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

#if !(defined PROTECT_ECSLVSIMPLE_H)
#define PROTECT_ECSLVSIMPLE_H		1

#include <osal.h>
#include <ecSlvApi.h>

#include <ESL_os.h>

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/

typedef struct EC_SLV_APP_CTT_Application
{
    uint32_t                        selectedPruInstance;

    /* Threads */
    TaskP_Object                    mainThreadHandle;
    TaskP_Params                    mainThreadParam;
    void *loopThreadHandle;

    /* Resources */
    void *gpioHandle;
    void *remoteHandle;
    void *ioexpLedHandle;

    int32_t                         msec;
    int32_t                         trigger;

    uint8_t                         state;
    uint8_t                         rsvd[3]; /* better be uint32_t aligned */
    clock_t                         prev;
    clock_t                         diff;

    EC_API_SLV_SCoE_Object_t *ptRecObjOut;
    EC_API_SLV_SCoE_Object_t *pt2002RecObj;
    EC_API_SLV_SCoE_Object_t *pt2007RecObj;
    EC_API_SLV_SCoE_Object_t *pt200FRecObj;

    EC_API_SLV_Pdo_t *ptRxPdo1600;
    EC_API_SLV_Pdo_t *ptTxPdo1A00;

    EC_API_SLV_SHandle_t *ptEcSlvApi;
} EC_SLV_APP_CTT_Application_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern void EC_SLV_APP_CTT_initBoardFunctions             (EC_SLV_APP_CTT_Application_t *pAppInstance_p);
extern void EC_SLV_APP_CTT_registerStacklessBoardFunctions(const EC_SLV_APP_CTT_Application_t *pAppInstance_p);
extern void EC_SLV_APP_CTT_applicationInit                (EC_SLV_APP_CTT_Application_t *pAppInstance_p);

#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_ECSLVSIMPLE_H */
