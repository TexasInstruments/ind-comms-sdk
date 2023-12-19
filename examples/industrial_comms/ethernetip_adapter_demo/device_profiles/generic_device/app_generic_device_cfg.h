/*!
 *  \file app_generic_device_cfg.h
 *
 *  \brief
 *  Configuration declarations specific to generic device profile.
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

#ifndef APP_GENERIC_DEVICE_CFG_H
#define APP_GENERIC_DEVICE_CFG_H

#if (defined BOARD_TYPE_TMDS64EVM) && (1==BOARD_TYPE_TMDS64EVM)

#define EI_APP_GENERIC_DEVICE_PRODUCT_CODE     0x1901
#define EI_APP_GENERIC_DEVICE_PRODUCT_NAME     "TMDS64EVM Generic Device"

#elif (defined BOARD_TYPE_TMDS243EVM) && (1==BOARD_TYPE_TMDS243EVM)

#define EI_APP_GENERIC_DEVICE_PRODUCT_CODE     0x5EED
#define EI_APP_GENERIC_DEVICE_PRODUCT_NAME     "TMDS243EVM Generic Device"

#elif (defined BOARD_TYPE_LP_AM243) && (1==BOARD_TYPE_LP_AM243)

#define EI_APP_GENERIC_DEVICE_PRODUCT_CODE     0x5EF7
#define EI_APP_GENERIC_DEVICE_PRODUCT_NAME     "LP-AM243 Generic Device"

#else

#define EI_APP_GENERIC_DEVICE_PRODUCT_CODE     0xFFFF
#define EI_APP_GENERIC_DEVICE_PRODUCT_NAME     "Generic Device"

#endif

#define EI_APP_GENERIC_DEVICE_VENDOR_ID        806
#define EI_APP_GENERIC_DEVICE_DEVICE_TYPE      0x002B
#define EI_APP_GENERIC_DEVICE_SERIAL_NUMBER    0x00000065
#define EI_APP_GENERIC_DEVICE_REVISION_MAJOR   0x03
#define EI_APP_GENERIC_DEVICE_REVISION_MINOR   0x06

#define EI_APP_GENERIC_DEVICE_CFG_HEADER_VERSION     5
#define EI_APP_GENERIC_DEVICE_CFG_HEADER_MAGICNUMBER (('M' << 8) | 'R')

/*!
 *  \brief
 *  Declaration of non volatile data structure related to generic device profile.
 */
typedef struct EI_APP_GENERIC_DEVICE_CFG_ProfileData
{
    // add device profile specific data here
}EI_APP_GENERIC_DEVICE_CFG_ProfileData_t;

/*!
 *  \brief
 *  Declaration of whole non volatile data structure.
 */
typedef struct EI_APP_GENERIC_DEVICE_CFG_Data
{
    EI_APP_CFG_Header_t                     header;
    EI_APP_CFG_AdapterData_t                adapter;
    EI_APP_GENERIC_DEVICE_CFG_ProfileData_t profile;
} EI_APP_GENERIC_DEVICE_CFG_Data_t;

#ifdef __cplusplus
extern "C" {
#endif

extern bool     EI_APP_GENERIC_DEVICE_CFG_init                  (EI_API_ADP_T        *pAdapter);
extern void     EI_APP_GENERIC_DEVICE_CFG_apply                 (EI_API_ADP_T        *pAdapter);
extern bool     EI_APP_GENERIC_DEVICE_CFG_callback              (EI_API_CIP_NODE_T   *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
extern void     EI_APP_GENERIC_DEVICE_CFG_setHeader             (EI_APP_CFG_Header_t *pHeader);
extern void     EI_APP_GENERIC_DEVICE_CFG_setDefaultWithoutComm (void);
extern bool     EI_APP_GENERIC_DEVICE_CFG_isValid               (void);
extern void*    EI_APP_GENERIC_DEVICE_CFG_getRuntimeData        (void);
extern void*    EI_APP_GENERIC_DEVICE_CFG_getFactoryResetData   (void);
extern uint32_t EI_APP_GENERIC_DEVICE_CFG_getLength             (void);

#ifdef  __cplusplus
}
#endif

#endif  // APP_GENERIC_DEVICE_CFG_H
