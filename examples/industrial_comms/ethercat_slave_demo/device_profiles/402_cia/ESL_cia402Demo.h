/*!
 *  \example ESL_cia402Demo.h
 *
 *  \brief
 *  CiA 402 Callbacks Example.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2020, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2024 KUNBUS GmbH.
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

#if !(defined PROTECT_ESL_CIA402DEMO_H)
#define PROTECT_ESL_CIA402DEMO_H 1


#include <ecSlvApi.h>
#include <defines/ecSlvApiDef_error.h>
#include <defines/ecSlvApiDef.h>
#include <stdint.h>

#if (defined __cplusplus)
extern "C" {
#endif

 extern uint16_t EC_SLV_APP_CIA_startInputHandler(void* ctxt, uint16_t* intMask);
 extern void EC_SLV_APP_setObdValues(void* ctxt);
 extern void EC_SLV_APP_cia402Application(void* ctxt);
 extern void EC_SLV_APP_cia402LocalError(void* ctxt, uint16_t errorCode);
 extern uint32_t EC_SLV_APP_getCiA402ObjectEntryValue(void* pAppCtxt_p, EC_API_SLV_SCoE_ObjEntry_t* pObjectEntry_p,
                                                      uint16_t length_p, uint16_t* pValue_p);

#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_ESL_CIA402DEMO_H */
