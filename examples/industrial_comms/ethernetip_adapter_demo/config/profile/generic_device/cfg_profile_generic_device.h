/*!
 *  \file cfg_profile_generic_device.h
 *
 *  \brief
 *  Configuration of generic device object parameters.
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

#ifndef CFG_PROFILE_GENERIC_DEVICE_H
#define CFG_PROFILE_GENERIC_DEVICE_H

// 0x01 Identity Object Parameters
#if (defined CFG_BOARD_TYPE_TMDS64EVM) && (1==CFG_BOARD_TYPE_TMDS64EVM)

#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_CODE     0x1901
#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_NAME     "TMDS64EVM Generic Device"

#elif (defined CFG_BOARD_TYPE_TMDS243EVM) && (1==CFG_BOARD_TYPE_TMDS243EVM)

#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_CODE     0x5EED
#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_NAME     "TMDS243EVM Generic Device"

#elif (defined CFG_BOARD_TYPE_LP_AM243) && (1==CFG_BOARD_TYPE_LP_AM243)

#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_CODE     0x5EF7
#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_NAME     "LP-AM243 Generic Device"

#elif (defined CFG_BOARD_TYPE_LP_AM263PX) && (1==CFG_BOARD_TYPE_LP_AM263PX)

#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_CODE     0x66C7
#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_NAME     "LP-AM263P Generic Device"

#elif (defined CFG_BOARD_TYPE_LP_AM261X) && (1==CFG_BOARD_TYPE_LP_AM261X)

#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_CODE     0x65FF
#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_NAME     "LP-AM261 Generic Device"

#else

#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_CODE     0xFFFF
#define CFG_PROFILE_GENERIC_DEVICE_PRODUCT_NAME     "Generic Device"

#endif

#define CFG_PROFILE_GENERIC_DEVICE_VENDOR_ID        806
#define CFG_PROFILE_GENERIC_DEVICE_DEVICE_TYPE      0x002B
#define CFG_PROFILE_GENERIC_DEVICE_SERIAL_NUMBER    0x00000065
#define CFG_PROFILE_GENERIC_DEVICE_REVISION_MAJOR   0x03
#define CFG_PROFILE_GENERIC_DEVICE_REVISION_MINOR   0x08

// 0x43 TimeSync Object Parameters
#define CFG_PROFILE_GENERIC_DEVICE_TIMESYNC_PRODUCT_DESCRIPTION_OF_CONFIGURATION   "Texas Instruments Ethernet/IP CIP Sync;buf_serial_number;";
#define CFG_PROFILE_GENERIC_DEVICE_TIMESYNC_REVISION_DATA_OF_CONFIGURATION         "1.0;1.0;1.00";

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __cplusplus
}
#endif

#endif  // CFG_PROFILE_GENERIC_DEVICE_H
