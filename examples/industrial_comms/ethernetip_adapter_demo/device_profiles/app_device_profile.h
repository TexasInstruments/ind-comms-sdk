/*!
 *  \file app_device_profile.h
 *
 *  \brief
 *  Device profile interface declaration.
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

#ifndef APP_DEVICE_PROFILE_H
#define APP_DEVICE_PROFILE_H

typedef enum EI_APP_DEVICE_PROFILE_Err
{
    EI_APP_DEVICE_PROFILE_ERR_OK         = 0x00000000,   /*!< No error, everything should be fine. */
    EI_APP_DEVICE_PROFILE_ERR_PtrInvalid = 0xFFFFFFFE,   /*!< Invalid pointer error. */
    EI_APP_DEVICE_PROFILE_ERR_General    = 0xFFFFFFFF,   /*!< General error, initial value. */
}EI_APP_DEVICE_PROFILE_Err_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t EI_APP_DEVICE_PROFILE_init (void);

extern bool  EI_APP_DEVICE_PROFILE_TASK_init (EI_API_ADP_T      *pAdapter, EI_API_CIP_NODE_T *pCipNode);
extern void  EI_APP_DEVICE_PROFILE_TASK_run  (EI_API_CIP_NODE_T *pCipNode);

extern bool     EI_APP_DEVICE_PROFILE_CFG_init                  (EI_API_ADP_T        *pAdapter);
extern void     EI_APP_DEVICE_PROFILE_CFG_apply                 (EI_API_ADP_T        *pAdapter);
extern bool     EI_APP_DEVICE_PROFILE_CFG_callback              (EI_API_CIP_NODE_T   *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
extern void     EI_APP_DEVICE_PROFILE_CFG_setHeader             (EI_APP_CFG_Header_t *pHeader);
extern void     EI_APP_DEVICE_PROFILE_CFG_setDefaultWithoutComm (void);
extern void*    EI_APP_DEVICE_PROFILE_CFG_getRuntimeData        (void);
extern void*    EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData   (void);
extern uint32_t EI_APP_DEVICE_PROFILE_CFG_getLength             (void);
extern bool     EI_APP_DEVICE_PROFILE_CFG_isValid               (void);

#ifdef  __cplusplus
}
#endif

#endif  // APP_DEVICE_PROFILE_H
