/*!
 *  \file device_profile_intf.h
 *
 *  \brief
 *  Device profile interface declaration.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2025 Texas Instruments Incorporated
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

#ifndef DEVICE_PROFILE_INTF_H
#define DEVICE_PROFILE_INTF_H

typedef enum DEVICE_PROFILE_INTF_Err
{
    DEVICE_PROFILE_INTF_ERR_OK         = 0x00000000,   /*!< No error, everything should be fine. */
    DEVICE_PROFILE_INTF_ERR_PtrInvalid = 0xFFFFFFFE,   /*!< Invalid pointer error. */
    DEVICE_PROFILE_INTF_ERR_General    = 0xFFFFFFFF,   /*!< General error, initial value. */
}DEVICE_PROFILE_INTF_Err_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t DEVICE_PROFILE_INTF_init (void);

extern bool  DEVICE_PROFILE_INTF_taskInit (EI_API_ADP_T      *pAdapter, EI_API_CIP_NODE_T *pCipNode);
extern void  DEVICE_PROFILE_INTF_taskRun  (EI_API_CIP_NODE_T *pCipNode);

extern bool     DEVICE_PROFILE_INTF_cfgInit                  (EI_API_ADP_T                *pAdapter);
extern void     DEVICE_PROFILE_INTF_cfgApply                 (EI_API_ADP_T                *pAdapter);
extern bool     DEVICE_PROFILE_INTF_cfgCallback              (EI_API_CIP_NODE_T           *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
extern void     DEVICE_PROFILE_INTF_cfgSetHeader             (DEVICE_PROFILE_CFG_Header_t *pHeader);
extern void     DEVICE_PROFILE_INTF_cfgSetDefaultWithoutComm (void);
extern void*    DEVICE_PROFILE_INTF_cfgGetRuntimeData        (void);
extern void*    DEVICE_PROFILE_INTF_cfgGetFactoryResetData   (void);
extern uint32_t DEVICE_PROFILE_INTF_cfgGetLength             (void);
extern bool     DEVICE_PROFILE_INTF_cfgIsValid               (void);

#ifdef  __cplusplus
}
#endif

#endif  // DEVICE_PROFILE_INTF_H
