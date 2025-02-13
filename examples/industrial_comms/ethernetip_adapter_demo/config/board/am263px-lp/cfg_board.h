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

#define CFG_BOARD_TYPE_LP_AM263PX  1

// Board HW configuration
#define CFG_BOARD_INSTANCE_INVALID   ~0  // Used when instance is not defined

// Processor register address defines (IDK_CTRLMMR0_MAC_ID0, IDK_CTRLMMR0_MAC_ID1)
#define CFG_BOARD_REG_MAC_ID0 ((uint32_t*) 0x50D80500u) // Ethernet MAC address lower 32 bit
#define CFG_BOARD_REG_MAC_ID1 ((uint32_t*) 0x50D80504u) // Ethernet MAC address upper 16 bit

// UART SysConfig instance used by application/stack
#define CFG_BOARD_UART_INSTANCE CONFIG_UART1

// LED SysConfig instance used by application/stack
#define CFG_BOARD_LED_INSTANCE CONFIG_LED0

// FLASH SysConfig instance used by application/stack
#define CFG_BOARD_FLASH_INSTANCE CONFIG_FLASH0

// EEPROM SysConfig instance used by application/stack
#define CFG_BOARD_EEPROM_INSTANCE CONFIG_EEPROM0

#if (defined FBTL_REMOTE) && (FBTL_REMOTE == 1)
#define CFG_BOARD_PRU_ICSS_BLOCK_INSTANCE         CFG_BOARD_INSTANCE_INVALID
#define CFG_BOARD_PRU_ICSS_ETHPHY_0_INSTANCE      CFG_BOARD_INSTANCE_INVALID
#define CFG_BOARD_PRU_ICSS_ETHPHY_1_INSTANCE      CFG_BOARD_INSTANCE_INVALID
#else
#define CFG_BOARD_PRU_ICSS_BLOCK_INSTANCE         CONFIG_PRU_ICSS0
#define CFG_BOARD_PRU_ICSS_ETHPHY_0_INSTANCE      CONFIG_ETHPHY0
#define CFG_BOARD_PRU_ICSS_ETHPHY_1_INSTANCE      CONFIG_ETHPHY1
#endif /* FBTL_REMOTE */

#ifdef  __cplusplus
}
#endif

#endif // CFG_BOARD_H_INC
