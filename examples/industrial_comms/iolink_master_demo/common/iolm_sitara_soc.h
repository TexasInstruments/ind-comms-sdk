/*!
 *  \file iolm_sitara_soc.h
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

#ifndef IO_LINK_IOLINK_SOC_H_
#define IO_LINK_IOLINK_SOC_H_

#include <stdint.h>
#include <stdbool.h>

#include "ti_board_config.h"
#include "IOLM_Phy.h"
#include "IOLM_PRU.h"
#include "iolm_example.h"
#include "kernel/dpl/SemaphoreP.h"

/** \brief register address offset between GPIO_SET_DATA01 and GPIO_SET_DATA23 */
#define SOC_GPIO_REG_BANK_OFFSET        (0x28)
/** \brief Number of pins per bank - shift value - used instead of divide operator */
#define SOC_GPIO_PINS_PER_BANK_SHIFT    (5U)
/** \brief Returns the bank index based on pin number */
#define SOC_GPIO_GET_BANK_INDEX(pinNum) (((uint32_t)pinNum) >> SOC_GPIO_PINS_PER_BANK_SHIFT)
/** \brief Returns the base address offset based on pin number */
#define SOC_GPIO_GET_BASE_ADDR_OFS(pinNum)                                                         \
    (SOC_GPIO_GET_BANK_INDEX((uint32_t)pinNum) * SOC_GPIO_REG_BANK_OFFSET)
/** \brief Returns the bit mask within a bank based on pin number */
#define SOC_GPIO_GET_PIN_MASK(pinNum) (((uint32_t)1U) << (pinNum % 32))

// Modes to change pinmux
#if defined(SOC_AM64X) || defined(SOC_AM243X)
#define IOL_MUX_PRU_RX       (1U)
#define IOL_MUX_PRU_TX       (0U)

#define IOL_MUX_GPIO_RX      (7U)
#define IOL_MUX_GPIO_TX      (7U)

#define IOL_PRU_INSTANCE_MAX CONFIG_PRUICSS_NUM_INSTANCES
#elif defined(SOC_AM263PX) || defined(SOC_AM261X)
#define IOL_MUX_PRU_RX       (PIN_MODE(0U))
#define IOL_MUX_PRU_TX       (PIN_MODE(0U))

#define IOL_MUX_GPIO_RX      PIN_MODE(7U)
#define IOL_MUX_GPIO_TX      PIN_MODE(7U)

#define IOL_PRU_INSTANCE_MAX CONFIG_PRUICSS_NUM_INSTANCES
#else
#error SOC not supported
#endif

#define IOLM_PHY_INTERFACE_SITARAPRU                                                               \
    .pfuInit = NULL, .pfuWakeup = NULL, .pfuSwitchPortPower = NULL, .pfuSetMode = NULL,            \
    .pfuSetIQMode = NULL, .pfuTransferPrepare = NULL, .pfuTransferTrig = NULL, .pfuSetDO = NULL,   \
    .pfuGetDI = NULL, .pfuSetIQ = NULL, .pfuGetIQ = NULL, .pfuSetFHCfg = NULL,                     \
    .pfuSetPhyCycleTimer = NULL

/**
\brief This enumeration indicates the requested CQ mode of the port.
*/
typedef enum IOLM_SOC_ECqMode
{
    /** \brief CQ disabled. == SIO disabled */
    IOLM_SOC_ECqMode_INACTIVE = 0,
    /** \brief CQ is in NPN mode. */
    IOLM_SOC_ECqMode_NPN,
    /** \brief CQ is in PNP mode. */
    IOLM_SOC_ECqMode_PNP,
    /** \brief CQ is in Push Pull mode. */
    IOLM_SOC_ECqMode_PUSHPULL,
    IOLM_SOC_ECqMode_FORCE32BIT = 0xffffffff
} IOLM_SOC_ECqMode_t;

/**
\brief Enumeration for binary power states
*/
typedef enum IOLM_SOC_EPowerState
{
    /** \brief Power off */
    IOLM_SOC_EPowerState_OFF = 0,
    /** \brief Power on */
    IOLM_SOC_EPowerState_ON         = 1,
    IOLM_SOC_EPowerState_FORCE32BIT = 0xffffffff
} IOLM_SOC_EPowerState_t;

extern const IOLM_SPhyGeneric      IOLM_SOC_phyPortCfgPru_g[IOLM_EXMPL_MAX_PORTS];
extern const IOLM_PL_sPortConfig_t iolPinSetup_g[IOLM_EXMPL_MAX_PORTS];
extern SemaphoreP_Object           mutexIolPeriphery;

void IOLM_SOC_init(void);
void BOARD_initPruss(void);
void IOLM_SOC_setMode(uint8_t instance, uint8_t portNum, IOLM_PL_ePortMode_t mode);
const IOLM_PL_sPortConfig_t *IOLM_SOC_getPortCfg(uint8_t instance, uint8_t portNum);
void IOLM_SOC_setDO(uint8_t instance, uint8_t portNum, bool boPortTargetState);
bool IOLM_SOC_getDI(uint8_t instance, uint8_t portNum);
void IOLM_SOC_setPower(uint8_t instance, uint8_t portNum, bool powerState);
void IOLM_SOC_setIQ(uint8_t instance, uint8_t portNum, bool boOutValue);
bool IOLM_SOC_getIQ(uint8_t instance, uint8_t portNum);
void IOLM_SOC_setIQMode(uint8_t instance, uint8_t portNum, IOL_EIQMode eIQMode);
OSAL_Error_t IOLM_SOC_checkInstPortValid(uint8_t instance, uint8_t portNum);
bool IOLM_SOC_IsInstanceValid(uint8_t instance);
void IOLM_SOC_setCqMode(INT8U port, IOLM_SOC_ECqMode_t eCqMode);
IOLM_SOC_ECqMode_t IOLM_SOC_getCqMode(INT8U port);

extern bool IOLM_IQ_DRV_get(uint8_t portNum);
extern void IOLM_IQ_DRV_set(uint8_t portNum, bool value);
extern void IOLM_IQ_DRV_setMode(uint8_t portNum, IOL_EIQMode eIQMode);

#endif /* IO_LINK_IOLINK_SOC_H_ */
