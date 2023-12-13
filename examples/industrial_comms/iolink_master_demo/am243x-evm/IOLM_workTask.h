/*!
 *  \file 
 *
 *  \brief
 *  IO-Link Master Example Working Task for asynchroneous jobs
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
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
 *
 */

#if !(defined PROTECT_IOLMWORKTASK_H)
#define PROTECT_IOLMWORKTASK_H      1

#include <stdint.h>
#include <osal.h>

// function type for the async work queue
typedef uint32_t (*asyncWorkFunc_t)(void * const);

// element type for the async work queue
typedef struct {
    asyncWorkFunc_t func;   // worker function to be called
    asyncWorkFunc_t cbDone; // callback function called after the worker is done
    void *pLoad;            // data for func and/or cbDone
} asyncWorkMsg_t;

typedef enum IOLM_worktask_err_t {
    IOLM_WT_ERRCODE_OK = 0,
    IOLM_WT_ERRCODE_ERROR
} IOLM_worktask_err_t;

#if (defined __cplusplus)
extern "C" {
#endif

// functions for message queue handled in worktask
extern uint32_t IOLM_doWriteNvram(void * const pLoad);
extern uint32_t IOLM_queueWriteNvram(const char * const name, const int32_t mode, const uint32_t len, const void * const pData, asyncWorkFunc_t cbDone);
extern uint32_t IOLM_doClearNvram(void * const unused);
extern uint32_t IOLM_queueClearNvram(void);
extern uint32_t IOLM_pushWorkMsg(asyncWorkFunc_t const func, asyncWorkFunc_t const cbDone, void * const pLoad);

extern void OSAL_FUNC_NORETURN IOLM_workTask(void *pArg_p);

#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_IOLMWORKTASK_H */
