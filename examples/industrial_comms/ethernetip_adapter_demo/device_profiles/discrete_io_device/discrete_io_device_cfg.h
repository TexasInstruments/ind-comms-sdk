/*!
 *  \file discrete_io_device_cfg.h
 *
 *  \brief
 *  Configuration declarations specific to discrete IO device profile.
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

#ifndef DISCRETE_IO_DEVICE_CFG_H
#define DISCRETE_IO_DEVICE_CFG_H

#include "device_profiles/discrete_io_device/discrete_io_device_asm.h"

#define DIO_DEVICE_CFG_HEADER_VERSION      1
#define DIO_DEVICE_CFG_HEADER_MAGICNUMBER  (('N' << 8) | 'K')

/*!
 *  \brief
 *  Declaration of non volatile data structure related to discrete IO device profile.
 */
typedef struct DIO_DEVICE_CFG_ProfileData
{
    // add device profile specific data here
} DIO_DEVICE_CFG_ProfileData_t;

/*!
 *  \brief
 *  Declaration of whole non volatile data structure.
 */
typedef struct DIO_DEVICE_CFG_Data
{
    DEVICE_PROFILE_CFG_Header_t       header;
    DEVICE_PROFILE_CFG_AdapterData_t  adapter;
    DIO_DEVICE_CFG_ProfileData_t      profile;
} DIO_DEVICE_CFG_Data_t;


#ifdef __cplusplus
extern "C" {
#endif

extern bool     DIO_DEVICE_CFG_init                  (EI_API_ADP_T                *pAdapter);
extern void     DIO_DEVICE_CFG_apply                 (EI_API_ADP_T                *pAdapter);
extern bool     DIO_DEVICE_CFG_callback              (EI_API_CIP_NODE_T           *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
extern void     DIO_DEVICE_CFG_setHeader             (DEVICE_PROFILE_CFG_Header_t *pHeader);
extern void     DIO_DEVICE_CFG_setDefaultWithoutComm (void);
extern bool     DIO_DEVICE_CFG_isValid               (void);
extern void*    DIO_DEVICE_CFG_getRuntimeData        (void);
extern void*    DIO_DEVICE_CFG_getFactoryResetData   (void);
extern uint32_t DIO_DEVICE_CFG_getLength             (void);


extern void DIO_DEVICE_CFG_getConfigurationAssemblyCb(uint8_t *faultAction, uint8_t *idleAction);

#ifdef  __cplusplus
}
#endif

#endif  // DISCRETE_IO_DEVICE_CFG_H
