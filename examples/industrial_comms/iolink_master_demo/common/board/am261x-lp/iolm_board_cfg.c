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
#include <board/ioexp/ioexp_tca6408.h>

const IOLM_PL_sPortConfig_t iolPinSetup_g[] =
{
 { // Port 0
    .rx = {
        .gpio.ctlRegOffset = PIN_GPIO71,
        .gpio.gpioBase = CONFIG_IOL_RX1_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX1_PIN,
        .pruPin = 0, // hard coded in firmware
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_GPIO20,
        .gpio.gpioBase = CONFIG_IOL_TX1_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX1_PIN,
        .pruPin = 8,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_GPIO126,
        .gpio.gpioBase = CONFIG_IOL_TXEN1_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN1_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN1_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN1_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN1_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_GPIO25,
        .gpioBase = CONFIG_IOL_ENL1_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL1_PIN
    }
 },
 { // Port 1
    .rx = {
        .gpio.ctlRegOffset = PIN_GPIO72,
        .gpio.gpioBase = CONFIG_IOL_RX2_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX2_PIN,
        .pruPin = 1, // hard coded in firmware
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_GPIO74,
        .gpio.gpioBase = CONFIG_IOL_TX2_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX2_PIN,
        .pruPin = 9,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_GPIO44,
        .gpio.gpioBase = CONFIG_IOL_TXEN2_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN2_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN2_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN2_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN2_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_GPIO81,
        .gpioBase = CONFIG_IOL_ENL2_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL2_PIN
    }
 },
 { // Port 2
    .rx = {
        .gpio.ctlRegOffset = PIN_GPIO73,
        .gpio.gpioBase = CONFIG_IOL_RX3_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX3_PIN,
        .pruPin = 2, // hard coded in firmware
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_GPIO125,
        .gpio.gpioBase = CONFIG_IOL_TX3_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX3_PIN,
        .pruPin = 13,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_GPIO6,
        .gpio.gpioBase = CONFIG_IOL_TXEN3_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN3_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN3_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN3_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN3_PIN),
    },
  . pwrEn = {
        .ctlRegOffset = PIN_GPIO82,
        .gpioBase = CONFIG_IOL_ENL3_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL3_PIN
    }
 },
 { // Port 3
    .rx = {
        .gpio.ctlRegOffset = PIN_GPIO16,
        .gpio.gpioBase = CONFIG_IOL_RX4_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX4_PIN,
        .pruPin = 3, // hard coded in firmware
     },
    .tx = {
        .gpio.ctlRegOffset = PIN_GPIO123,
        .gpio.gpioBase = CONFIG_IOL_TX4_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX4_PIN,
        .pruPin = 10,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_GPIO5,
        .gpio.gpioBase = CONFIG_IOL_TXEN4_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN4_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN4_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN4_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN4_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_GPIO83,
        .gpioBase = CONFIG_IOL_ENL4_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL4_PIN
    }
 },
 { // Port 4
    .rx = {
        .gpio.ctlRegOffset = PIN_GPIO15,
        .gpio.gpioBase = CONFIG_IOL_RX5_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX5_PIN,
        .pruPin = 4, // hard coded in firmware
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_GPIO18,
        .gpio.gpioBase = CONFIG_IOL_TX5_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX5_PIN,
        .pruPin = 15,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_GPIO130,
        .gpio.gpioBase = CONFIG_IOL_TXEN5_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN5_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN5_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN5_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN5_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_GPIO45,
        .gpioBase = CONFIG_IOL_ENL5_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL5_PIN
    }
 },
 { // Port 5
    .rx = {
        .gpio.ctlRegOffset = PIN_GPIO54,
        .gpio.gpioBase = CONFIG_IOL_RX6_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX6_PIN,
        .pruPin = 5, // hard coded in firmware
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_GPIO17,
        .gpio.gpioBase = CONFIG_IOL_TX6_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX6_PIN,
        .pruPin = 16,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_GPIO106,
        .gpio.gpioBase = CONFIG_IOL_TXEN6_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN6_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN6_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN6_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN6_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_GPIO54,
        .gpioBase = CONFIG_IOL_ENL6_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL6_PIN
    }
 },
 { // Port 6
    .rx = {
        .gpio.ctlRegOffset = PIN_GPIO19,
        .gpio.gpioBase = CONFIG_IOL_RX7_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX7_PIN,
        .pruPin = 6, // hard coded in firmware
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_GPIO4,
        .gpio.gpioBase = CONFIG_IOL_TX7_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX7_PIN,
        .pruPin = 12,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_GPIO90,
        .gpio.gpioBase = CONFIG_IOL_TXEN7_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN7_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN7_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN7_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN7_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_GPIO26,
        .gpioBase = CONFIG_IOL_ENL7_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL7_PIN
    }
 },
 { // Port 7
    .rx = {
        .gpio.ctlRegOffset = PIN_GPIO133,
        .gpio.gpioBase = CONFIG_IOL_RX8_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_RX8_PIN,
        .pruPin = 7, // hard coded in firmware
    },
    .tx = {
        .gpio.ctlRegOffset = PIN_GPIO3,
        .gpio.gpioBase = CONFIG_IOL_TX8_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TX8_PIN,
        .pruPin = 11,
    },
    .txEn = {
        .gpio.ctlRegOffset = PIN_GPIO46,
        .gpio.gpioBase = CONFIG_IOL_TXEN8_BASE_ADDR,
        .gpio.gpioPin = CONFIG_IOL_TXEN8_PIN,
        .gpioPhysAddr = CONFIG_IOL_TXEN8_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN8_PIN),
        .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN8_PIN),
    },
    .pwrEn = {
        .ctlRegOffset = PIN_GPIO80,
        .gpioBase = CONFIG_IOL_ENL8_BASE_ADDR,
        .gpioPin = CONFIG_IOL_ENL8_PIN
    }
 }
};

void BOARD_initPruss(void)
{
    enum AM261XLP_IOEXP
    {
        AM261XLP_USB2_0_MUX_SEL0 = 0,
        AM261XLP_VPP_LDO_EN,
        AM261XLP_LED_DRIVER_EN,
        AM261XLP_MCAN_MUX_SEL,
        AM261XLP_BP_BO_MUX_EN,
        AM261XLP_BP_MUX_SW_S1,
        AM261XLP_BP_BO_MUX_EN_N,
        AM261XLP_BP_MUX_SW_S0,

        AM261XLP_MAX
    };
    TCA6408_Params params;
    TCA6408_Config tcaConfig;

    TCA6408_Params_init(&params);
    params.i2cAddress  = 0x20;
    params.i2cInstance = CONFIG_I2C_IOL;

    TCA6408_open(&tcaConfig, &params);

    TCA6408_config(&tcaConfig, AM261XLP_BP_MUX_SW_S0, TCA6408_MODE_OUTPUT);
    TCA6408_config(&tcaConfig, AM261XLP_BP_MUX_SW_S1, TCA6408_MODE_OUTPUT);
    TCA6408_config(&tcaConfig, AM261XLP_BP_BO_MUX_EN, TCA6408_MODE_OUTPUT);
    TCA6408_config(&tcaConfig, AM261XLP_BP_BO_MUX_EN_N, TCA6408_MODE_OUTPUT);

    TCA6408_setOutput(&tcaConfig, AM261XLP_BP_BO_MUX_EN, 1);
    TCA6408_setOutput(&tcaConfig, AM261XLP_BP_MUX_SW_S0, 0);
    TCA6408_setOutput(&tcaConfig, AM261XLP_BP_MUX_SW_S1, 1);
    TCA6408_setOutput(&tcaConfig, AM261XLP_BP_BO_MUX_EN_N, 0);
}
