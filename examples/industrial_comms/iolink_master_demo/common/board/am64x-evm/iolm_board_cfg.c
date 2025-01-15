/*!
 *  \file iolm_board_cfg.c
 *
 *  \brief
 *  board specific initalization and configuration
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

#include "iolm_sitara_soc.h"
#include "iolm_example.h"
#include "drivers/pinmux.h"
#include "ti_board_config.h"

const IOLM_PL_sPortConfig_t iolPinSetup_g[] =
{
 { // Port 0
    .rx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO0,
        .gpio.gpioBase = CONFIG_IOL_RX1_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX1_PIN
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO8,
        .gpio.gpioBase = CONFIG_IOL_TX1_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX1_PIN,
        .pruPin = 8,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO6,
        .gpio.gpioBase = CONFIG_IOL_TXEN1_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN1_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN1_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN1_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN1_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_PRG0_PRU0_GPO18,
        .gpioBase = CONFIG_IOL_ENL1_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL1_PIN
    }
 },
 { // Port 1
    .rx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO1,
        .gpio.gpioBase = CONFIG_IOL_RX2_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX2_PIN
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO11,
        .gpio.gpioBase = CONFIG_IOL_TX2_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX2_PIN,
        .pruPin = 11,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO8,
        .gpio.gpioBase = CONFIG_IOL_TXEN2_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN2_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN2_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN2_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN2_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_PRG0_PRU0_GPO19,
        .gpioBase = CONFIG_IOL_ENL2_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL2_PIN
    }
 },
 { // Port 2
    .rx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO2,
        .gpio.gpioBase = CONFIG_IOL_RX3_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX3_PIN
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO12,
        .gpio.gpioBase = CONFIG_IOL_TX3_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX3_PIN,
        .pruPin = 12,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO11,
        .gpio.gpioBase = CONFIG_IOL_TXEN3_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN3_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN3_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN3_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN3_PIN),
    },
  . pwrEn = {
        .ctlRegOffset = PIN_PRG0_PRU1_GPO0,
        .gpioBase = CONFIG_IOL_ENL3_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL3_PIN
    }
 },
 { // Port 3
    .rx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO3,
        .gpio.gpioBase = CONFIG_IOL_RX4_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX4_PIN
     },
    .tx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO13,
        .gpio.gpioBase = CONFIG_IOL_TX4_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX4_PIN,
        .pruPin = 13,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO12,
        .gpio.gpioBase = CONFIG_IOL_TXEN4_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN4_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN4_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN4_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN4_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_PRG0_PRU1_GPO1,
        .gpioBase = CONFIG_IOL_ENL4_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL4_PIN
    }
 },
 { // Port 4
    .rx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO4,
        .gpio.gpioBase = CONFIG_IOL_RX5_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX5_PIN
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO14,
        .gpio.gpioBase = CONFIG_IOL_TX5_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX5_PIN,
        .pruPin = 14,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO13,
        .gpio.gpioBase = CONFIG_IOL_TXEN5_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN5_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN5_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN5_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN5_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_PRG0_PRU1_GPO2,
        .gpioBase = CONFIG_IOL_ENL5_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL5_PIN
    }
 },
 { // Port 5
    .rx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO5,
        .gpio.gpioBase = CONFIG_IOL_RX6_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX6_PIN
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO15,
        .gpio.gpioBase = CONFIG_IOL_TX6_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX6_PIN,
        .pruPin = 15,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO14,
        .gpio.gpioBase = CONFIG_IOL_TXEN6_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN6_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN6_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN6_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN6_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_PRG0_PRU1_GPO3,
        .gpioBase = CONFIG_IOL_ENL6_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL6_PIN
    }
 },
 { // Port 6
    .rx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO6,
        .gpio.gpioBase = CONFIG_IOL_RX7_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX7_PIN
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO16,
        .gpio.gpioBase = CONFIG_IOL_TX7_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX7_PIN,
        .pruPin = 16,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO15,
        .gpio.gpioBase = CONFIG_IOL_TXEN7_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN7_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN7_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN7_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN7_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_PRG0_PRU1_GPO4,
        .gpioBase = CONFIG_IOL_ENL7_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL7_PIN
    }
 },
 { // Port 7
    .rx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO7,
        .gpio.gpioBase = CONFIG_IOL_RX8_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX8_PIN
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO17,
        .gpio.gpioBase = CONFIG_IOL_TX8_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX8_PIN,
        .pruPin = 17,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO16,
        .gpio.gpioBase = CONFIG_IOL_TXEN8_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN8_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN8_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN8_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN8_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_PRG0_PRU1_GPO5,
        .gpioBase = CONFIG_IOL_ENL8_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL8_PIN
    }
 }
};

void BOARD_initPruss(void)
{
}
