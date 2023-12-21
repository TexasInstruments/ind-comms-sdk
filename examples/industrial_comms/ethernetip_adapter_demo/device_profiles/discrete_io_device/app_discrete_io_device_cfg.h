/*!
 *  \file app_discrete_io_device_cfg.h
 *
 *  \brief
 *  Configuration declarations specific to discrete IO device profile.
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

#ifndef APP_DISCRETE_IO_DEVICE_CFG_H
#define APP_DISCRETE_IO_DEVICE_CFG_H

#if (defined BOARD_TYPE_TMDS64EVM) && (1==BOARD_TYPE_TMDS64EVM)

#define EI_APP_DIO_DEVICE_PRODUCT_CODE     0x1902
#define EI_APP_DIO_DEVICE_PRODUCT_NAME     "TMDS64EVM Discrete I/O Device"

#elif (defined BOARD_TYPE_TMDS243EVM) && (1==BOARD_TYPE_TMDS243EVM)

#define EI_APP_DIO_DEVICE_PRODUCT_CODE     0x5EEE
#define EI_APP_DIO_DEVICE_PRODUCT_NAME     "TMDS243EVM Discrete I/O Device"

#elif (defined BOARD_TYPE_LP_AM243) && (1==BOARD_TYPE_LP_AM243)

#define EI_APP_DIO_DEVICE_PRODUCT_CODE     0x5EF8
#define EI_APP_DIO_DEVICE_PRODUCT_NAME     "LP-AM243 Discrete I/O Device"

#else

#define EI_APP_DIO_DEVICE_PRODUCT_CODE     0xFFFF
#define EI_APP_DIO_DEVICE_PRODUCT_NAME     "Discrete I/O Device"

#endif

#define EI_APP_DIO_DEVICE_VENDOR_ID        806
#define EI_APP_DIO_DEVICE_DEVICE_TYPE      0x0007
#define EI_APP_DIO_DEVICE_SERIAL_NUMBER    0x00000065
#define EI_APP_DIO_DEVICE_REVISION_MAJOR   0x03
#define EI_APP_DIO_DEVICE_REVISION_MINOR   0x06

#define EI_APP_DIO_DEVICE_ASSEMBLY_PRODUCING     0x64 /*!< Producing Assembly Instance */
#define EI_APP_DIO_DEVICE_ASSEMBLY_CONSUMING     0x65 /*!< Consuming Assembly Instance */
#define EI_APP_DIO_DEVICE_ASSEMBLY_CONFIGURATION 0x28 /*!< Configuration Assembly Instance */

#define EI_APP_DIO_DEVICE_DOP_NUM_OF_INST        2 /*!< Number of instances of Discrete Output Point Object */
#define EI_APP_DIO_DEVICE_DIP_NUM_OF_INST        2 /*!< Number of instances of Discrete Input Point Object */
#define EI_APP_DIO_DEVICE_DOG_NUM_OF_INST        1 /*!< Number of instances of Discrete Output Group Object */

#define EI_APP_DIO_DEVICE_DIP_CLASS_ID           0x08 /*!< Class Number for DIP Objects */
#define EI_APP_DIO_DEVICE_DOP_CLASS_ID           0x09 /*!< Class Number for DOP Objects */
#define EI_APP_DIO_DEVICE_DOG_CLASS_ID           0x1E /*!< Class Number for DOG Objects */

#define EI_APP_DIO_DEVICE_DIP_REVISION_NUMBER    2    /*!< Revision Number for DIP Object */
#define EI_APP_DIO_DEVICE_DOP_REVISION_NUMBER    1    /*!< Revision Number for DOP Object */
#define EI_APP_DIO_DEVICE_DOG_REVISION_NUMBER    1    /*!< Revision Number for DOG Object */

#define EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH 9
#define EI_APP_DIO_DEVICE_IO_CONNECTION          0x01 /*!< IO Connection Type */
#define EI_APP_DIO_DEVICE_DATA_RECEIVED          1
#define EI_APP_DIO_DEVICE_FAULT_ACTION_BIT       0
#define EI_APP_DIO_DEVICE_IDLE_ACTION_BIT        1

#define EI_APP_DIO_DEVICE_CFG_HEADER_VERSION      1
#define EI_APP_DIO_DEVICE_CFG_HEADER_MAGICNUMBER  (('N' << 8) | 'K')


/*!
 *  \brief
 *  Declaration of non volatile data structure related to discrete IO device profile.
 */
typedef struct EI_APP_DIO_DEVICE_CFG_ProfileData
{
    // add device profile specific data here
} EI_APP_DIO_DEVICE_CFG_ProfileData_t;

/*!
 *  \brief
 *  Declaration of whole non volatile data structure.
 */
typedef struct EI_APP_DIO_DEVICE_CFG_Data
{
    EI_APP_CFG_Header_t                         header;
    EI_APP_CFG_AdapterData_t                    adapter;
    EI_APP_DIO_DEVICE_CFG_ProfileData_t profile;
} EI_APP_DIO_DEVICE_CFG_Data_t;


#ifdef __cplusplus
extern "C" {
#endif

extern bool     EI_APP_DIO_DEVICE_CFG_init                  (EI_API_ADP_T        *pAdapter);
extern void     EI_APP_DIO_DEVICE_CFG_apply                 (EI_API_ADP_T        *pAdapter);
extern bool     EI_APP_DIO_DEVICE_CFG_callback              (EI_API_CIP_NODE_T   *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
extern void     EI_APP_DIO_DEVICE_CFG_setHeader             (EI_APP_CFG_Header_t *pHeader);
extern void     EI_APP_DIO_DEVICE_CFG_setDefaultWithoutComm (void);
extern bool     EI_APP_DIO_DEVICE_CFG_isValid               (void);
extern void*    EI_APP_DIO_DEVICE_CFG_getRuntimeData        (void);
extern void*    EI_APP_DIO_DEVICE_CFG_getFactoryResetData   (void);
extern uint32_t EI_APP_DIO_DEVICE_CFG_getLength             (void);

extern void EI_APP_DOG_init(EI_API_CIP_NODE_T  *pCipNode);
extern void EI_APP_DOG_run(EI_API_CIP_NODE_T   *pCipNode);
extern void EI_APP_DIP_init(EI_API_CIP_NODE_T  *pCipNode);
extern void EI_APP_DIP_run(EI_API_CIP_NODE_T   *pCipNode);
extern void EI_APP_DOP_SM_init(void);
extern void EI_APP_DOP_SM_run(void);

extern void EI_APP_DIO_DEVICE_getConnectionInfo(uint32_t *connectionState, uint32_t *receiveData);
extern void EI_APP_DIO_DEVICE_getConfigurationAssemblyCb(uint8_t *faultAction, uint8_t *idleAction);

#ifdef  __cplusplus
}
#endif

#endif  // APP_DISCRETE_IO_DEVICE_CFG_H
