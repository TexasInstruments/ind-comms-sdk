/*!
 *  \file pru_IOLink.h
 *
 *  \brief
 *  PRU Integration: IOLink specific interface.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2024 KUNBUS GmbH
 *  All rights reserved.
 *
 * 
 */

#if !(defined __PRU_IOLINK_H__)
#define __PRU_IOLINK_H__		1

#include <stdint.h>
#include <stdbool.h>

#include <IOLM_Types.h>
#include "pru.h"


// structures
typedef enum IOLM_PL_ePortMode {
    IOLM_PL_eModeSioInactive = 0,
    IOLM_PL_eModeSdci,
    IOLM_PL_eModeSioDI,
    IOLM_PL_eModeSioDO,
    IOLM_PL_eMode_Force32Bit = 0x7fffffff
} IOLM_PL_ePortMode_t;


typedef struct IOLM_PL_sGpioConfig
{
    uint32_t    gpioBase;
    uint32_t    gpioPin;
    int16_t     ctlRegOffset;
} IOLM_PL_sGpioConfig_t;

typedef struct IOLM_PL_sGpioTxConfig
{
    IOLM_PL_sGpioConfig_t   gpio;
    uint32_t                pruPin; // pru pin number
} IOLM_PL_sGpioTxConfig_t;

typedef struct IOLM_PL_sGpioEnConfig
{
    IOLM_PL_sGpioConfig_t   gpio;
    uint32_t                gpioPhysAddr; // physical address for pru to access gpio
    uint32_t                gpioPinMask; // mask to write on gpioPhysAddr
} IOLM_PL_sGpioEnConfig_t;

typedef struct IOLM_PL_sPortConfig
{
    IOLM_PL_sGpioConfig_t       rx;
    IOLM_PL_sGpioTxConfig_t     tx;
    IOLM_PL_sGpioEnConfig_t     txEn;
    IOLM_PL_sGpioConfig_t       pwrEn;
} IOLM_PL_sPortConfig_t;


typedef struct IOLM_PL_PRU_Config
{
    uint32_t pruIcssInstanceNumber;
    PRU_PruFirmware_t customFirmware[2];
}IOLM_PL_PRU_Config_t;

typedef void (*IOLM_PL_cbPRUsetMode_t)        (uint8_t instance_p, uint8_t portNum_p, IOLM_PL_ePortMode_t mode_p);
typedef const IOLM_PL_sPortConfig_t* (*IOLM_PL_cbPRUgetPortCfg_t)     (uint8_t instance_p, uint8_t portNum_p);
typedef void (*IOLM_PL_cbPRUsetDO_t)          (uint8_t instance_p, uint8_t portNum_p, bool portTargetState_p);
typedef bool (*IOLM_PL_cbPRUgetDI_t)          (uint8_t instance_p, uint8_t portNum_p);
typedef void (*IOLM_PL_cbPRUsetPower_t)       (uint8_t instance_p, uint8_t portNum_p, bool powerState_p);
typedef void (*IOLM_PL_cbPRUSetIQ_t)          (uint8_t instance_p, uint8_t portNum_p, bool outValue_p);
typedef bool (*IOLM_PL_cbPRUGetIQ_t)          (uint8_t instance_p, uint8_t portNum_p);
typedef bool (*IOLM_PL_cbPRUCheckInstValid_t) (uint8_t instance_p);
typedef void (*IOLM_PL_cbPRUSetIQMode_t)      (uint8_t instance_p, uint8_t portNum_p, IOL_EIQMode eIQMode_p);

typedef struct IOLM_PL_sCallbacks
{
    IOLM_PL_cbPRUsetMode_t      cbSetMode;
    IOLM_PL_cbPRUgetPortCfg_t   cbGetPortCfg;
    IOLM_PL_cbPRUsetDO_t        cbSetDo;
    IOLM_PL_cbPRUgetDI_t        cbGetDi;
    IOLM_PL_cbPRUsetPower_t     cbSetPower;
    IOLM_PL_cbPRUSetIQ_t        cbSetIQ;
    IOLM_PL_cbPRUGetIQ_t        cbGetIQ;
    IOLM_PL_cbPRUSetIQMode_t    cbSetIQMode;
    IOLM_PL_cbPRUCheckInstValid_t cbCheckInst;
}IOLM_PL_sCallbacks_t;

void PRU_IOLM_registerSetModeCallback(IOLM_PL_cbPRUsetMode_t pCallback_p);
void PRU_IOLM_registerGetPortCfgCallback(IOLM_PL_cbPRUgetPortCfg_t pCallback_p);
void PRU_IOLM_registerSetDoCallback(IOLM_PL_cbPRUsetDO_t pCallback_p);
void PRU_IOLM_registerGetDiCallback(IOLM_PL_cbPRUgetDI_t pCallback_p);
void PRU_IOLM_registerSetPowerCallback(IOLM_PL_cbPRUsetPower_t pCallback_p);
void PRU_IOLM_registerSetIQCallback(IOLM_PL_cbPRUSetIQ_t pCallback_p);
void PRU_IOLM_registerGetIQCallback(IOLM_PL_cbPRUGetIQ_t pCallback_p);
void PRU_IOLM_registerSetIQModeCallback(IOLM_PL_cbPRUSetIQMode_t pCallback_p);
void PRU_IOLM_registerCheckInstanceCallback(IOLM_PL_cbPRUCheckInstValid_t pCallback_p);

void PRU_IOLM_init(IOLM_PL_PRU_Config_t *pConfig);
void PRU_IOLM_deInit(uint32_t pruIcssInstanceNumber);
void PRU_IOLM_initSitara(void);

#endif /* __PRU_IOLINK_H__ */
