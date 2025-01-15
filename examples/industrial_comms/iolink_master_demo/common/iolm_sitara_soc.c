/*!
 *  \file iolm_sitara_soc.c
 *
 *  \brief
 *  SOC specific IO Link functions.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2021 Texas Instruments Incorporated
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

#include "cfg_board.h"
#include "iolm_sitara_soc.h"
#include "iolm_example.h"
#include "drivers/pinmux.h"

SemaphoreP_Object  mutexIolPeriphery;
IOLM_SOC_ECqMode_t iolEqMode_g[IOLM_EXMPL_MAX_PORTS];

static IOLM_PL_PRU_Config_t iolmPruExampleConfig
    = { .pruIcssInstanceNumber       = IOLM_PRUICSS_INST,
        .isPru1FrameHandler          = IOLM_PRUICSS_PRU1FH,
        .customFirmware[0].pFirmware = NULL, // use the default PRU Firmware on core-0
        .customFirmware[0].frmLength = 0,
        .customFirmware[1].pFirmware = NULL, // use the default PRU Firmware on core-1
        .customFirmware[1].frmLength = 0 };

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

static const IOLM_SPhyGeneric *pPhyStackCallbacks[IOLM_EXMPL_MAX_PORTS];

void IOLM_SOC_setPinMux(int16_t regOffset, uint32_t mode, bool isRx)
{
#if defined(SOC_AM65XX) && (SOC_AM65XX == 1)
    uint32_t value;

    value = *((uint32_t *)(MAIN_PMUX_CTRL + regOffset));

    value &= ~((1 << 18) | (1 << 21) | (0xff)); // remove mode, tx dis, rxactive
    value |= mode;
    if (isRx)
    {
        value |= (1 << 18); // rx active
    }

    *((uint32_t *)(MAIN_PMUX_CTRL + regOffset)) = value;
#elif defined(SOC_AM437x) && (SOC_AM437x == 1)
    uint32_t value;

    value = *((uint32_t *)(SOC_CONTROL_MODULE_REG + regOffset));

    value &= ~((1 << 18) | (0xff)); // remove mode, rxactive
    value |= mode;
    if (isRx)
    {
        value |= (1 << 18); // rx active
    }

    *((uint32_t *)(SOC_CONTROL_MODULE_REG + regOffset)) = value;
#elif defined(SOC_AM64X) || defined(SOC_AM243X)
    Pinmux_PerCfg_t pinMuxCfg[2] = { { PINMUX_END, PINMUX_END }, { PINMUX_END, PINMUX_END } };

    pinMuxCfg[0].offset   = regOffset;
    pinMuxCfg[0].settings = PIN_MODE(mode) | PIN_PULL_DISABLE;

    if (isRx)
    {
        pinMuxCfg[0].settings |= PIN_INPUT_ENABLE; // rx active
    }

    Pinmux_config(pinMuxCfg, PINMUX_DOMAIN_ID_MAIN);
#elif defined(SOC_AM263PX)
    Pinmux_PerCfg_t pinMuxCfg[2] = { { PINMUX_END, PINMUX_END }, { PINMUX_END, PINMUX_END } };

    pinMuxCfg[0].offset   = regOffset;
    pinMuxCfg[0].settings = mode | PIN_PULL_DISABLE | PIN_SLEW_RATE_HIGH | PIN_GPIO_R5SS0_0;

    Pinmux_config(pinMuxCfg, PINMUX_DOMAIN_ID_MAIN);
#elif defined(SOC_AM261X)
    Pinmux_PerCfg_t pinMuxCfg[2] = { { PINMUX_END, PINMUX_END }, { PINMUX_END, PINMUX_END } };

    if ((mode == IOL_MUX_PRU_RX) || (mode == IOL_MUX_PRU_TX))
    {
        switch (regOffset)
        {
            case PIN_GPIO20:
                mode |= PIN_MODE(3U);
                break;
            case PIN_GPIO74:
                mode |= PIN_MODE(0U);
                break;
            case PIN_GPIO125:
                mode |= PIN_MODE(1U);
                break;
            case PIN_GPIO123:
                mode |= PIN_MODE(1U);
                break;
            case PIN_GPIO18:
                mode |= PIN_MODE(5U);
                break;
            case PIN_GPIO17:
                mode |= PIN_MODE(5U);
                break;
            case PIN_GPIO4:
                mode |= PIN_MODE(2U);
                break;
            case PIN_GPIO3:
                mode |= PIN_MODE(2U);
                break;

            case PIN_GPIO71:
                mode |= PIN_MODE(0U);
                break;
            case PIN_GPIO72:
                mode |= PIN_MODE(0U);
                break;
            case PIN_GPIO73:
                mode |= PIN_MODE(0U);
                break;
            case PIN_GPIO16:
                mode |= PIN_MODE(5U);
                break;
            case PIN_GPIO15:
                mode |= PIN_MODE(5U);
                break;
            case PIN_GPIO54:
                mode |= PIN_MODE(2U);
                break;
            case PIN_GPIO19:
                mode |= PIN_MODE(3U);
                break;
            case PIN_GPIO133:
                mode |= PIN_MODE(1U);
                break;
            default:
                break;
        }
    }
    pinMuxCfg[0].offset   = regOffset;
    pinMuxCfg[0].settings = mode | PIN_PULL_DISABLE | PIN_SLEW_RATE_HIGH | PIN_GPIO_R5SS0_0;

    Pinmux_config(pinMuxCfg, PINMUX_DOMAIN_ID_MAIN);

#endif /* SoC */
}

void IOLM_SOC_gpioWrite(const IOLM_PL_sGpioConfig_t *pGpio, bool value)
{
    uint32_t baseAddr = (uint32_t)AddrTranslateP_getLocalAddr(pGpio->gpioBase);
    if (value)
    {
        GPIO_pinWriteHigh(baseAddr, pGpio->gpioPin);
    }
    else
    {
        GPIO_pinWriteLow(baseAddr, pGpio->gpioPin);
    }
}

bool IOLM_SOC_gpioRead(const IOLM_PL_sGpioConfig_t *pGpio)
{
    bool value;

    uint32_t baseAddr = (uint32_t)AddrTranslateP_getLocalAddr(pGpio->gpioBase);
    value             = GPIO_pinRead(baseAddr, pGpio->gpioPin);

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

    SemaphoreP_constructMutex(&mutexIolPeriphery);

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

    for (portNum = 0; portNum < IOLM_EXMPL_MAX_PORTS; portNum++)
    {
        IOLM_SOC_setPinMux(iolPinSetup_g[portNum].rx.gpio.ctlRegOffset, IOL_MUX_PRU_RX, true);
        IOLM_SOC_setPinMux(iolPinSetup_g[portNum].tx.gpio.ctlRegOffset, IOL_MUX_PRU_TX, false);
        IOLM_SOC_setPinMux(iolPinSetup_g[portNum].txEn.gpio.ctlRegOffset, IOL_MUX_GPIO_TX, false);
        IOLM_SOC_setPinMux(iolPinSetup_g[portNum].pwrEn.ctlRegOffset, IOL_MUX_GPIO_TX, false);

        // setup the GPIO direction
        GPIO_setDirMode(
            iolPinSetup_g[portNum].rx.gpio.gpioBase,
            iolPinSetup_g[portNum].rx.gpio.gpioPin,
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

        pPhyStackCallbacks[portNum] = &IOLM_SOC_phyPortCfgPru_g[portNum];
    }

    PRU_IOLM_init(&iolmPruExampleConfig);
    IOLM_Phy_Init(pPhyStackCallbacks);
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
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].rx.gpio.ctlRegOffset, IOL_MUX_GPIO_RX, true);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].tx.gpio.ctlRegOffset, IOL_MUX_GPIO_TX, false);
            break;
        case IOLM_PL_eModeSioDO:
            IOLM_SOC_setCqMode(instance * 8 + portNum, cqMode);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].rx.gpio.ctlRegOffset, IOL_MUX_GPIO_RX, true);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].tx.gpio.ctlRegOffset, IOL_MUX_GPIO_TX, false);
            IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 1);
            break;
        case IOLM_PL_eModeSdci:
            IOLM_SOC_gpioWrite(&iolPinSetup_g[portNum].txEn.gpio, 0);
            IOLM_SOC_setPinMux(iolPinSetup_g[portNum].rx.gpio.ctlRegOffset, IOL_MUX_PRU_RX, true);
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
    if (portNum < IOLM_EXMPL_MAX_PORTS)
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
        boDInputValue = IOLM_SOC_gpioRead(&iolPinSetup_g[portNum].rx.gpio);
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
    if (IOLM_SOC_checkInstPortValid(instance, portNum) == OSAL_eERR_NOERROR)
    {
        IOLM_IQ_DRV_set(instance * 8 + portNum, boOutValue);
    }
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
        iqState = IOLM_IQ_DRV_get(instance * 8 + portNum);
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

    if (IOLM_SOC_checkInstPortValid(instance, portNum) == OSAL_eERR_NOERROR)
    {
        IOLM_IQ_DRV_setMode(instance * 8 + portNum, eIQMode);
    }
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
