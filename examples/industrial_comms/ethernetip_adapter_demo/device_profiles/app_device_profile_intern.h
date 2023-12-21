/*!
 *  \file app_device_profile_intern.h
 *
 *  \brief
 *  Device profile internal interface declaration.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br><br>
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

#ifndef APP_DEVICE_PROFILE_INTERN_H
#define APP_DEVICE_PROFILE_INTERN_H

typedef struct EI_APP_DEVICE_PROFILE_Interface
{
    struct {
        bool  (*init) (EI_API_ADP_T      *pAdapter, EI_API_CIP_NODE_T *pCipNode);
        void  (*run)  (EI_API_CIP_NODE_T *pCipNode);
    }task;

    struct {
        bool     (*init)                  (EI_API_ADP_T        *pAdapter);
        void     (*apply)                 (EI_API_ADP_T        *pAdapter);
        bool     (*callback)              (EI_API_CIP_NODE_T   *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
        void     (*setHeader)             (EI_APP_CFG_Header_t *pHeader);
        void     (*setDefaultWithoutComm) (void);
        bool     (*isValid)               (void);
        void*    (*getRuntimeData)        (void);
        void*    (*getFactoryResetData)   (void);
        uint32_t (*getLength)             (void);
    }cfg;
}EI_APP_DEVICE_PROFILE_Interface_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf);

#ifdef  __cplusplus
}
#endif

#endif  // APP_DEVICE_PROFILE_INTERN_H
