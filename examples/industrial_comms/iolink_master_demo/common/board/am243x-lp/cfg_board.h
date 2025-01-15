/*!
 *  \file cfg_board.h
 *
 *  \brief
 *  Board configuration.
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

#ifndef CFG_BOARD_H_INC
#define CFG_BOARD_H_INC

#ifdef __cplusplus
extern "C" {
#endif

// PRU Instance and config for IO-Link
#define IOLM_PRUICSS_INST         CONFIG_PRU_ICSS0
#define IOLM_PRUICSS_PRU1FH       false

// NVM type to be used
#define IOL_APP_IOD_NVM_CONFIG_TYPE_FLASH  (0u)
#define IOL_APP_IOD_NVM_CONFIG_TYPE_EEPROM (1u)

#if (IOL_APP_IOD_NVM_CONFIG_TYPE_FLASH)
// use flash
#define IOL_APP_IOD_NVM_TYPE     NVM_TYPE_FLASH
#define IOL_APP_IOD_NVM_INSTANCE CONFIG_FLASH0
#define IOL_APP_IOD_NVM_OFFSET   0x500000
#elif (IOL_APP_IOD_NVM_CONFIG_TYPE_EEPROM)
// use eeprom
#define IOL_APP_IOD_NVM_TYPE     NVM_TYPE_EEPROM
#define IOL_APP_IOD_NVM_INSTANCE CONFIG_EEPROM0
#define IOL_APP_IOD_NVM_OFFSET   512
#endif

// SPI instance and channel for LED and IQ
#define IOLM_SPI_LED_INSTANCE CONFIG_MCSPI_IOL
#define IOLM_SPI_LED_CHANNEL  (1U)
#define IOLM_SPI_IQ_INSTANCE  CONFIG_MCSPI_IOL
#define IOLM_SPI_IQ_CHANNEL   (1U)

#ifdef  __cplusplus
}
#endif

#endif // CFG_BOARD_H_INC
