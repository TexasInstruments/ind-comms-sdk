/*!
 *  \file iolm_port_sitara_soc.c
 *
 *  \brief
 *  SOC specific IO Link functions.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2024 KUNBUS GmbH.
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

#include "iolm_port_sitara_soc.h"
#include "iolm_port_smi_example.h"


#define IOLM_SOC_PORT_COUNT (uint8_t)(sizeof(iolPinSetup_g) / sizeof(iolPinSetup_g[0]))

IOLM_SOC_ECqMode_t iolEqMode_g[IOLM_EXMPL_MAX_PORTS];

const IOLM_PL_sPortConfig_t iolPinSetup_g[] =
{
    { // Port 0
        .rx = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO0,
            .gpioBase = CONFIG_IOL_RX1_BASE_ADDR,
            .gpioPin = CONFIG_IOL_RX1_PIN
        },
            .tx = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO11,
            .gpio.gpioBase = CONFIG_IOL_TX1_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TX1_PIN,
            .pruPin = 11,
        },
        .txEn = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO15,
            .gpio.gpioBase = CONFIG_IOL_TXEN1_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TXEN1_PIN,
            .gpioPhysAddr = CONFIG_IOL_TXEN1_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN1_PIN),
            .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN1_PIN),
        },
        .pwrEn = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO19,
            .gpioBase = CONFIG_IOL_ENL1_BASE_ADDR,
            .gpioPin = CONFIG_IOL_ENL1_PIN
        }
    },
    { // Port 1
        .rx = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO1,
            .gpioBase = CONFIG_IOL_RX2_BASE_ADDR,
            .gpioPin = CONFIG_IOL_RX2_PIN
         },
        .tx = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO12,
            .gpio.gpioBase = CONFIG_IOL_TX2_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TX2_PIN,
            .pruPin = 12,
        },
        .txEn = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU1_GPO16,
            .gpio.gpioBase = CONFIG_IOL_TXEN2_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TXEN2_PIN,
            .gpioPhysAddr = CONFIG_IOL_TXEN2_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN2_PIN),
            .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN2_PIN),
        },
        .pwrEn = {
            .ctlRegOffset = PIN_PRG0_PRU1_GPO0,
            .gpioBase = CONFIG_IOL_ENL2_BASE_ADDR,
            .gpioPin = CONFIG_IOL_ENL2_PIN
        }
    },
    { // Port 2
        .rx = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO2,
            .gpioBase = CONFIG_IOL_RX3_BASE_ADDR,
            .gpioPin = CONFIG_IOL_RX3_PIN
        },
        .tx = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO13,
            .gpio.gpioBase = CONFIG_IOL_TX3_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TX3_PIN,
            .pruPin = 13,
        },
        .txEn = {
            .gpio.ctlRegOffset = PIN_PRG0_MDIO0_MDIO,
            .gpio.gpioBase = CONFIG_IOL_TXEN3_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TXEN3_PIN,
            .gpioPhysAddr = CONFIG_IOL_TXEN3_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN3_PIN),
            .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN3_PIN),
        },
        .pwrEn = {
            .ctlRegOffset = PIN_PRG0_PRU1_GPO1,
            .gpioBase = CONFIG_IOL_ENL3_BASE_ADDR,
            .gpioPin = CONFIG_IOL_ENL3_PIN
        }
    },
    { // Port 3
        .rx = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO3,
            .gpioBase = CONFIG_IOL_RX4_BASE_ADDR,
            .gpioPin = CONFIG_IOL_RX4_PIN
        },
        .tx = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO14,
            .gpio.gpioBase = CONFIG_IOL_TX4_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TX4_PIN,
            .pruPin = 14,
        },
        .txEn = {
            .gpio.ctlRegOffset = PIN_PRG0_MDIO0_MDC,
            .gpio.gpioBase = CONFIG_IOL_TXEN4_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TXEN4_PIN,
            .gpioPhysAddr = CONFIG_IOL_TXEN4_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN4_PIN),
            .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN4_PIN),
        },
        .pwrEn = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO2,
            .gpioBase = CONFIG_IOL_ENL4_BASE_ADDR,
            .gpioPin = CONFIG_IOL_ENL4_PIN
        }
    },
    { // Port 4
        .rx = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO4,
            .gpioBase = CONFIG_IOL_RX5_BASE_ADDR,
            .gpioPin = CONFIG_IOL_RX5_PIN
        },
        .tx = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO15,
            .gpio.gpioBase = CONFIG_IOL_TX5_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TX5_PIN,
            .pruPin = 15,
        },
        .txEn = {
            .gpio.ctlRegOffset = PIN_SPI0_D0,
            .gpio.gpioBase = CONFIG_IOL_TXEN5_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TXEN5_PIN,
            .gpioPhysAddr = CONFIG_IOL_TXEN5_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN5_PIN),
            .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN5_PIN),
         },
        .pwrEn = {
            .ctlRegOffset = PIN_PRG0_PRU1_GPO6,
            .gpioBase = CONFIG_IOL_ENL5_BASE_ADDR,
            .gpioPin = CONFIG_IOL_ENL5_PIN
        }
    },
    { // Port 5
        .rx = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO5,
            .gpioBase = CONFIG_IOL_RX6_BASE_ADDR,
            .gpioPin = CONFIG_IOL_RX6_PIN
        },
        .tx = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO16,
            .gpio.gpioBase = CONFIG_IOL_TX6_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TX6_PIN,
            .pruPin = 16,
        },
        .txEn = {
            .gpio.ctlRegOffset = PIN_PRG1_PRU0_GPO17,
            .gpio.gpioBase = CONFIG_IOL_TXEN6_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TXEN6_PIN,
            .gpioPhysAddr = CONFIG_IOL_TXEN6_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN6_PIN),
            .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN6_PIN),
        },
        .pwrEn = {
            .ctlRegOffset = PIN_PRG0_PRU1_GPO11,
            .gpioBase = CONFIG_IOL_ENL6_BASE_ADDR,
            .gpioPin = CONFIG_IOL_ENL6_PIN
        }
    },
    { // Port 6
        .rx = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO6,
            .gpioBase = CONFIG_IOL_RX7_BASE_ADDR,
            .gpioPin = CONFIG_IOL_RX7_PIN
        },
        .tx = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO17,
            .gpio.gpioBase = CONFIG_IOL_TX7_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TX7_PIN,
            .pruPin = 17,
        },
        .txEn = {
            .gpio.ctlRegOffset = PIN_PRG1_PRU0_GPO18,
            .gpio.gpioBase = CONFIG_IOL_TXEN7_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TXEN7_PIN,
            .gpioPhysAddr = CONFIG_IOL_TXEN7_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN7_PIN),
            .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN7_PIN),
        },
        .pwrEn = {
            .ctlRegOffset = PIN_PRG0_PRU1_GPO12,
            .gpioBase = CONFIG_IOL_ENL7_BASE_ADDR,
            .gpioPin = CONFIG_IOL_ENL7_PIN
        }
    },
    { // Port 7
        .rx = {
            .ctlRegOffset = PIN_PRG0_PRU0_GPO7,
            .gpioBase = CONFIG_IOL_RX8_BASE_ADDR,
            .gpioPin = CONFIG_IOL_RX8_PIN
        },
        .tx = {
            .gpio.ctlRegOffset = PIN_PRG0_PRU0_GPO18,
            .gpio.gpioBase = CONFIG_IOL_TX8_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TX8_PIN,
            .pruPin = 18,
        },
        .txEn = {
            .gpio.ctlRegOffset = PIN_PRG1_PRU0_GPO19,
            .gpio.gpioBase = CONFIG_IOL_TXEN8_BASE_ADDR,
            .gpio.gpioPin = CONFIG_IOL_TXEN8_PIN,
            .gpioPhysAddr = CONFIG_IOL_TXEN8_BASE_ADDR + SOC_GPIO_GET_BASE_ADDR_OFS(CONFIG_IOL_TXEN8_PIN),
            .gpioPinMask = SOC_GPIO_GET_PIN_MASK(CONFIG_IOL_TXEN8_PIN),
        },
        .pwrEn = {
            .ctlRegOffset = PIN_PRG0_PRU1_GPO13,
            .gpioBase = CONFIG_IOL_ENL8_BASE_ADDR,
            .gpioPin = CONFIG_IOL_ENL8_PIN
        }
    }
};

const IOLM_SPhyGeneric IOLM_SOC_phyPortCfgPru_g[IOLM_EXMPL_MAX_PORTS] = {
    { .u8Port = 0, .eType = IOLM_Phy_eType_Pru, IOLM_PHY_INTERFACE_SITARAPRU },
    { .u8Port = 1, .eType = IOLM_Phy_eType_Pru, IOLM_PHY_INTERFACE_SITARAPRU },
    { .u8Port = 2, .eType = IOLM_Phy_eType_Pru, IOLM_PHY_INTERFACE_SITARAPRU },
    { .u8Port = 3, .eType = IOLM_Phy_eType_Pru, IOLM_PHY_INTERFACE_SITARAPRU },
    { .u8Port = 4, .eType = IOLM_Phy_eType_Pru, IOLM_PHY_INTERFACE_SITARAPRU },
    { .u8Port = 5, .eType = IOLM_Phy_eType_Pru, IOLM_PHY_INTERFACE_SITARAPRU },
    { .u8Port = 6, .eType = IOLM_Phy_eType_Pru, IOLM_PHY_INTERFACE_SITARAPRU },
    { .u8Port = 7, .eType = IOLM_Phy_eType_Pru, IOLM_PHY_INTERFACE_SITARAPRU },
};


void BOARD_initPruss(void)
{
    //nothing to do
}

void IOLM_SOC_setPinMux(int16_t regOffset, uint32_t mode, bool isRx)
{
    Pinmux_PerCfg_t pinMuxCfg[2] = { { PINMUX_END, PINMUX_END }, { PINMUX_END, PINMUX_END } };

    pinMuxCfg[0].offset   = regOffset;
    pinMuxCfg[0].settings = PIN_MODE(mode) | PIN_PULL_DISABLE;

    if (isRx)
    {
        pinMuxCfg[0].settings |= PIN_INPUT_ENABLE; // rx active
    }

    Pinmux_config(pinMuxCfg, PINMUX_DOMAIN_ID_MAIN);
}

void IOLM_SOC_gpioWrite(const IOLM_PL_sGpioConfig_t *pGpio, bool value)
{
    if (value)
    {
        GPIO_pinWriteHigh(pGpio->gpioBase, pGpio->gpioPin);
    }
    else
    {
        GPIO_pinWriteLow(pGpio->gpioBase, pGpio->gpioPin);
    }
}

bool IOLM_SOC_gpioRead(const IOLM_PL_sGpioConfig_t *pGpio)
{
    bool value;

    value = GPIO_pinRead(pGpio->gpioBase, pGpio->gpioPin);

    return value;
}

/**
 * \brief  Initialize SoC related hardware
 *
 */
void IOLM_SOC_init(void)
{
    uint8_t portNum;

    OSAL_MEMORY_memset(iolEqMode_g, 0, sizeof(iolEqMode_g));

    PRU_IOLM_registerSetModeCallback(IOLM_SOC_setMode);
    PRU_IOLM_registerGetPortCfgCallback(IOLM_SOC_getPortCfg);
    PRU_IOLM_registerSetDoCallback(IOLM_SOC_setDO);
    PRU_IOLM_registerGetDiCallback(IOLM_SOC_getDI);
    PRU_IOLM_registerSetPowerCallback(IOLM_SOC_setPower);
    PRU_IOLM_registerSetIQCallback(IOLM_SOC_setIQ);
    PRU_IOLM_registerGetIQCallback(IOLM_SOC_getIQ);
    PRU_IOLM_registerSetIQModeCallback(IOLM_SOC_setIQMode);
    PRU_IOLM_registerCheckInstanceCallback(IOLM_SOC_IsInstanceValid);

    BOARD_initPruss();

    for (portNum = 0; portNum < IOLM_SOC_PORT_COUNT; portNum++)
    {
        IOLM_SOC_setPinMux(iolPinSetup_g[portNum].rx.ctlRegOffset, IOL_MUX_PRU_RX, true);
        IOLM_SOC_setPinMux(iolPinSetup_g[portNum].tx.gpio.ctlRegOffset, IOL_MUX_PRU_TX, false);
        IOLM_SOC_setPinMux(iolPinSetup_g[portNum].txEn.gpio.ctlRegOffset, IOL_MUX_GPIO_TX, false);
        IOLM_SOC_setPinMux(iolPinSetup_g[portNum].pwrEn.ctlRegOffset, IOL_MUX_GPIO_TX, false);

        // setup the GPIO direction
        GPIO_setDirMode(
            iolPinSetup_g[portNum].rx.gpioBase,
            iolPinSetup_g[portNum].rx.gpioPin,
            GPIO_DIRECTION_INPUT);
        GPIO_setDirMode(
            iolPinSetup_g[portNum].txEn.gpio.gpioBase,
            iolPinSetup_g[portNum].txEn.gpio.gpioPin,
            GPIO_DIRECTION_OUTPUT);
        GPIO_setDirMode(
            iolPinSetup_g[portNum].pwrEn.gpioBase,
            iolPinSetup_g[portNum].pwrEn.gpioPin,
            GPIO_DIRECTION_OUTPUT);
        GPIO_setDirMode(
            iolPinSetup_g[portNum].tx.gpio.gpioBase,
            iolPinSetup_g[portNum].tx.gpio.gpioPin,
            GPIO_DIRECTION_OUTPUT);

        IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].tx.gpio, 0);
        IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 0);
        IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].pwrEn, 0);
        IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].pwrEn, 1);
    }
}

void IOLM_SOC_setMode(uint8_t instance, uint8_t portNum, IOLM_PL_ePortMode_t mode)
{
    IOLM_SOC_ECqMode_t cqMode = IOLM_SOC_ECqMode_PUSHPULL;
    // tx enable

    if (IOLM_SOC_checkInstPortValid(instance, portNum) != OSAL_eERR_NOERROR)
    {
        goto laExit;
    }

    IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 0);
    IOLM_SOC_setPinMux(iolPinSetup_g[portNum].txEn.gpio.ctlRegOffset, IOL_MUX_GPIO_TX, false);

    switch (mode)
    {
        case IOLM_PL_eModeSioInactive:
        case IOLM_PL_eModeSioDI:
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].rx.ctlRegOffset, IOL_MUX_GPIO_RX, true);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].tx.gpio.ctlRegOffset, IOL_MUX_GPIO_TX, false);
            break;
        case IOLM_PL_eModeSioDO:
            IOLM_SOC_setCqMode(instance * 8 + portNum, cqMode);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].rx.ctlRegOffset, IOL_MUX_GPIO_RX, true);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].tx.gpio.ctlRegOffset, IOL_MUX_GPIO_TX, false);
            IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 1);
            break;
        case IOLM_PL_eModeSdci:
            IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 0);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].rx.ctlRegOffset, IOL_MUX_PRU_RX, true);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].tx.gpio.ctlRegOffset, IOL_MUX_PRU_TX, false);
            break;
        default:
            break;
    }
laExit:
    return;
}

const IOLM_PL_sPortConfig_t *IOLM_SOC_getPortCfg(uint8_t instance, uint8_t portNum)
{
    (void)instance;
    if (portNum < IOLM_SOC_PORT_COUNT)
    {
        return &iolPinSetup_g[portNum];
    }
    else
    {
        return NULL;
    }
}


/**
 * \brief  Set the output value of specific port in SIO-mode
 *
 * \param  portNum            Output port which should be changed.
 *
 * \param  boPortTargetState  Output state (0=>LOW, 1=>HIGH).
 *
 */
void IOLM_SOC_setDO(uint8_t instance, uint8_t portNum, bool boPortTargetState)
{
    if (IOLM_SOC_checkInstPortValid(instance, portNum) != OSAL_eERR_NOERROR)
    {
        goto laExit;
    }

    IOLM_SOC_ECqMode_t cqMode = IOLM_SOC_getCqMode(instance * 8 + portNum);
    switch (cqMode)
    {
        case IOLM_SOC_ECqMode_NPN:
            if (boPortTargetState == true)
            {
                IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 1);
            }
            else
            {
                IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 0);
            }
            IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].tx.gpio, 1);
            break;
        case IOLM_SOC_ECqMode_PNP:
            if (boPortTargetState == true)
            {
                IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 1);
            }
            else
            {
                IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 0);
            }
            IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].tx.gpio, 0);
            break;
        case IOLM_SOC_ECqMode_PUSHPULL:
            if (boPortTargetState == true)
            {
                IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].tx.gpio, 0);
            }
            else
            {
                IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].tx.gpio, 1);
            }
            IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 1);
            break;
        default:
            OSAL_printf("SIO Mode defined but no CQ Mode defined");
            break;
    }
laExit:
    return;
}

/**
 * \brief  Read the input value of specific port in SIO-mode
 *
 * \param  instance    pru instance
 * \param  portNum     Input port which should be read.
 *
 * \return 0=>LOW, 1=>HIGH.
 *
 */
bool IOLM_SOC_getDI(uint8_t instance, uint8_t portNum)
{
    bool boDInputValue = true;

    if (IOLM_SOC_checkInstPortValid(instance, portNum) == OSAL_eERR_NOERROR)
    {
        IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 0);
        boDInputValue = IOLM_SOC_gpioRead(&iolPinSetup_g[portNum].rx);
    }

    return !boDInputValue;
}

/**
 * \brief  Turn the power of a specific port on or off
 *
 * \param  instance    pru instance
 * \param  portNum    IO-Link port which should be turned on/off.
 *
 * \param  powerState      target state
 *
 */
void IOLM_SOC_setPower(uint8_t instance, uint8_t portNum, bool powerState)
{
    uint32_t powerStateVar = (powerState ? 1 : 0);

    if (IOLM_SOC_checkInstPortValid(instance, portNum) == OSAL_eERR_NOERROR)
    {
        IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].pwrEn, powerStateVar);
    }
}

/**
 * \brief  Set the output value of specific ports IQ signal
 *
 * \param  instance    pru instance
 * \param  portNum     port number of the pru
 *
 * \param  boOutValue  Output state (0=>LOW, 1=>HIGH).
 *
 */
void IOLM_SOC_setIQ(uint8_t instance, uint8_t portNum, bool boOutValue)
{
    /* IQ Output is not supported on AM437x, AM64x and AM65x hardware. */
    (void)instance;
    (void)portNum;
    (void)boOutValue;
}

/**
 * \brief  Read the input value of specific ports IQ signal
 *
 * \param  instance    pru instance
 * \param  portNum     port number of the pru
 *
 * \return 0=>LOW, 1=>HIGH.
 *
 */
bool IOLM_SOC_getIQ(uint8_t instance, uint8_t portNum)
{
    bool iqState = false;

    if (IOLM_SOC_checkInstPortValid(instance, portNum) == OSAL_eERR_NOERROR)
    {
        iqState = IOLM_SPI_getIq(portNum);
    }
    return iqState;
}

/**
 * \brief  Set the mode of specific ports IQ behaviour
 *
 * \param  instance    pru instance
 * \param  portNum     port number of the pru
 *
 * \param  eIQMode_p     Desired target mode.
 *
 */
void IOLM_SOC_setIQMode(uint8_t instance, uint8_t portNum, IOL_EIQMode eIQMode)
{
    /* No hardware configuration steps necessary for mode switch */
    (void)instance;
    (void)portNum;
    (void)eIQMode;
}

/**
 * \brief  Set the mode of specific ports CQ behaviour
 *
 * \param  port       port number
 * \param  eCqMode_p    desired target mode
 *
 * \return  void
 *
 */
void IOLM_SOC_setCqMode(INT8U port, IOLM_SOC_ECqMode_t eCqMode)
{
    iolEqMode_g[port] = eCqMode;
}

/**
 * \brief  Set the mode of specific ports CQ behaviour
 *
 * \param  port       port number
 *
 * \return   target mode.
 *
 */
IOLM_SOC_ECqMode_t IOLM_SOC_getCqMode(INT8U port)
{
    IOLM_SOC_ECqMode_t eEqMode = iolEqMode_g[port];

    return eEqMode;
}

/**
 * \brief  Check if the given arguments are within supported range
 *
 * \param  instance    pru instance
 * \param  portNum     port number of the pru
 *
 * \return  OSAL_Error_t  OSAL_eERR_NOERROR if successful, otherwise error code.
 *
 */
OSAL_Error_t IOLM_SOC_checkInstPortValid(uint8_t instance, uint8_t portNum)
{
    if (instance >= IOL_PRU_INSTANCE_MAX)
    {
        return OSAL_eERR_EINVAL;
    }

    if (portNum >= IOLM_PORT_COUNT)
    {
        return OSAL_eERR_EINVAL;
    }

    return OSAL_eERR_NOERROR;
}

/**
 * \brief  Check if the given arguments are within supported range
 *
 * \param  instance    pru instance
 * \param  portNum     port number of the pru
 *
 * \return  bool     true if the arguments where within range, otherwise false.
 *
 */
bool IOLM_SOC_IsInstanceValid(uint8_t instance)
{
    bool retval = false;

    if (instance < IOL_PRU_INSTANCE_MAX)
    {
        retval = true;
    }

    return retval;
}
