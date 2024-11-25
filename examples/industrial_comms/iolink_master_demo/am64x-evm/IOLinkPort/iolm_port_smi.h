/*!
 *  \file iolm_port_smi.h
 *
 *  \brief
 *  SOC/OS specific IO Link SMI functions.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

#ifndef IOLINK_IOLM_PORT_SMI_H_
#define IOLINK_IOLM_PORT_SMI_H_

#include <IOLM_SMI_Types.h>

#define IOLM_SERIAL_STD (1U)

#define UART_SIZE_RX_BUFFER (1024U)
#define UART_RX_TIMEOUT     (100U)

#define UART_NUM_TX_BUFFER  (36U)
#define UART_SIZE_TX_BUFFER (64U)

#if 1
// use eeprom
#define IOL_APP_IOD_NVM_TYPE             NVM_TYPE_EEPROM
#define IOL_APP_IOD_NVM_INSTANCE         CONFIG_EEPROM0
#define IOL_APP_IOD_NVM_OFFSET           512
#else
// use flash
#define IOL_APP_IOD_NVM_TYPE             NVM_TYPE_FLASH
#define IOL_APP_IOD_NVM_INSTANCE         CONFIG_FLASH0
#define IOL_APP_IOD_NVM_OFFSET           0x500000
#endif

void IOLM_SMI_cbGenericCnf(IOLM_SMI_SHeader *pHeader, INT8U *pArgBlock);
void IOLM_SMI_cbStdInd(IOLM_SMI_SStdHeader *pHeader, INT8U *pArgBlock);
void IOLM_SMI_portInit(void);

#endif /* IOLINK_IOLM_PORT_BSP_H_ */
