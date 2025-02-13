/*!
 *  \file cfg_profile_discrete_io_device.h
 *
 *  \brief
 *  Configuration of discrete IO device object parameters.
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

#ifndef CFG_PROFILE_DISCRETE_IO_DEVICE_H
#define CFG_PROFILE_DISCRETE_IO_DEVICE_H

#include "device_profiles/discrete_io_device/discrete_io_device_asm.h"

#if (defined CFG_BOARD_TYPE_TMDS64EVM) && (1==CFG_BOARD_TYPE_TMDS64EVM)

#define CFG_PROFILE_DIO_DEVICE_PRODUCT_CODE     0x1902
#define CFG_PROFILE_DIO_DEVICE_PRODUCT_NAME     "TMDS64EVM Discrete I/O Device"

#elif (defined CFG_BOARD_TYPE_TMDS243EVM) && (1==CFG_BOARD_TYPE_TMDS243EVM)

#define CFG_PROFILE_DIO_DEVICE_PRODUCT_CODE     0x5EEE
#define CFG_PROFILE_DIO_DEVICE_PRODUCT_NAME     "TMDS243EVM Discrete I/O Device"

#elif (defined CFG_BOARD_TYPE_LP_AM243) && (1==CFG_BOARD_TYPE_LP_AM243)

#define CFG_PROFILE_DIO_DEVICE_PRODUCT_CODE     0x5EF8
#define CFG_PROFILE_DIO_DEVICE_PRODUCT_NAME     "LP-AM243 Discrete I/O Device"

#elif (defined CFG_BOARD_TYPE_LP_AM263PX) && (1==CFG_BOARD_TYPE_LP_AM263PX)

#define CFG_PROFILE_DIO_DEVICE_PRODUCT_CODE     0x66C8
#define CFG_PROFILE_DIO_DEVICE_PRODUCT_NAME     "LP-AM263P Discrete I/O Device"

#elif (defined CFG_BOARD_TYPE_LP_AM261X) && (1==CFG_BOARD_TYPE_LP_AM261X)

#define CFG_PROFILE_DIO_DEVICE_PRODUCT_CODE     0x6600
#define CFG_PROFILE_DIO_DEVICE_PRODUCT_NAME     "LP-AM261 Discrete I/O Device"

#else

#define CFG_PROFILE_DIO_DEVICE_PRODUCT_CODE     0xFFFF
#define CFG_PROFILE_DIO_DEVICE_PRODUCT_NAME     "Discrete I/O Device"

#endif

#define CFG_PROFILE_DIO_DEVICE_TIMESYNC_PRODUCT_DESCRIPTION_OF_CONFIGURATION   "Texas Instruments Ethernet/IP CIP Sync;buf_serial_number;";
#define CFG_PROFILE_DIO_DEVICE_TIMESYNC_REVISION_DATA_OF_CONFIGURATION         "1.0;1.0;1.00";

#define CFG_PROFILE_DIO_DEVICE_VENDOR_ID        806
#define CFG_PROFILE_DIO_DEVICE_DEVICE_TYPE      0x0007
#define CFG_PROFILE_DIO_DEVICE_SERIAL_NUMBER    0x00000065
#define CFG_PROFILE_DIO_DEVICE_REVISION_MAJOR   0x03
#define CFG_PROFILE_DIO_DEVICE_REVISION_MINOR   0x08

#define CFG_PROFILE_DIO_DEVICE_ASSEMBLY_PRODUCING     DIO_DEVICE_ASM_4_Input_Point_No_Status /*!< Producing Assembly Instance */
#define CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONSUMING     DIO_DEVICE_ASM_4_Output_Point /*!< Consuming Assembly Instance */
#define CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONFIGURATION DIO_DEVICE_ASM_Output_Configuration /*!< Configuration Assembly Instance */

#define CFG_PROFILE_DIO_DEVICE_DOP_NUM_OF_INST        4 /*!< Number of instances of Discrete Output Point Object */
#define CFG_PROFILE_DIO_DEVICE_DIP_NUM_OF_INST        4 /*!< Number of instances of Discrete Input Point Object */

#define CFG_PROFILE_DIO_DEVICE_DIP_CLASS_ID           0x08 /*!< Class Number for DIP Objects */
#define CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID           0x09 /*!< Class Number for DOP Objects */
#define CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID           0x1E /*!< Class Number for DOG Objects */

#define CFG_PROFILE_DIO_DEVICE_DIP_REVISION_NUMBER    2    /*!< Revision Number for DIP Object */
#define CFG_PROFILE_DIO_DEVICE_DOP_REVISION_NUMBER    1    /*!< Revision Number for DOP Object */
#define CFG_PROFILE_DIO_DEVICE_DOG_REVISION_NUMBER    1    /*!< Revision Number for DOG Object */

#define EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH 8
#define EI_APP_DIO_DEVICE_IO_CONNECTION          0x01 /*!< IO Connection Type */
#define EI_APP_DIO_DEVICE_FAULT_ACTION_BIT       0
#define EI_APP_DIO_DEVICE_IDLE_ACTION_BIT        1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __cplusplus
}
#endif

#endif  // CFG_PROFILE_DISCRETE_IO_DEVICE_H
