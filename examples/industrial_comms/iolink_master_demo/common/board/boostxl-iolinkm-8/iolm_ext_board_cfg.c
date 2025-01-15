/*!
 *  \file iolm_ext_board_cfg.c
 *
 *  \brief
 *  IO-Link board specific code
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

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "cfg_board.h"
#include "iolm_sitara_soc.h"
#include "iolm_ledtask.h"
#include "ti_board_open_close.h"
#include <board/ioexp/ioexp_tca6424.h>

#define IOLM_SPI_LED_DATA_SIZE (16U)
#define IOLM_SPI_IQ_DATA_SIZE  (8U)

TCA6424_Config               ioexpBoostxl;

const IOLM_LED_sLedMapping_t IOLM_LED_aPortMapping_s[2 * IOLM_PORT_COUNT] = {
    { 0, IOLM_ELEDColor_Red },   /* 1st LED on driver IC -> port 8, red LED */
    { 0, IOLM_ELEDColor_Green }, /* 2nd LED on driver IC -> port 8, green LED */
    { 1, IOLM_ELEDColor_Red },   { 1, IOLM_ELEDColor_Green }, { 2, IOLM_ELEDColor_Red },
    { 2, IOLM_ELEDColor_Green }, { 3, IOLM_ELEDColor_Green }, { 3, IOLM_ELEDColor_Red },
    { 4, IOLM_ELEDColor_Red },   { 4, IOLM_ELEDColor_Green }, { 5, IOLM_ELEDColor_Red },
    { 5, IOLM_ELEDColor_Green }, { 6, IOLM_ELEDColor_Red },   { 6, IOLM_ELEDColor_Green },
    { 7, IOLM_ELEDColor_Red },   { 7, IOLM_ELEDColor_Green }
};

void IOLM_LED_DRV_init(void)
{
    TCA6424_Params params;

    TCA6424_Params_init(&params);
    params.i2cAddress  = 0x22;
    params.i2cInstance = CONFIG_I2C_IOL;

    TCA6424_open(&ioexpBoostxl, &params);

    for (int index = 0; index < 16; index++)
    {
        TCA6424_config(&ioexpBoostxl, index, TCA6424_MODE_OUTPUT);
    }
}

void IOLM_LED_DRV_setStates(uint32_t value)
{
    int32_t  error          = SystemP_SUCCESS;
    uint32_t shiftedBitmask = value;
    uint32_t currentLedNumber;

    SemaphoreP_pend(&mutexIolPeriphery, SystemP_WAIT_FOREVER);
    for (currentLedNumber = 0; currentLedNumber < 16; currentLedNumber++) /* walk though bitmask */
    {
        if (shiftedBitmask & 0x01)
        {
            error += TCA6424_setOutput(
                &ioexpBoostxl,
                currentLedNumber,
                TCA6424_OUT_STATE_LOW); /* off -> on on TCA6424 */
        }
        else
        {
            error += TCA6424_setOutput(
                &ioexpBoostxl,
                currentLedNumber,
                TCA6424_OUT_STATE_HIGH); /* on -> off on TCA6424 */
        }
        shiftedBitmask >>= 1;
    }
    SemaphoreP_post(&mutexIolPeriphery);
}

bool IOLM_IQ_DRV_get(uint8_t portNum)
{
    uint8_t           rxBuf[2];
    int32_t           error;
    bool              iqState = false;
    MCSPI_Transaction transaction;

    // set pin nLD high in serializer to be able to read IQ/pin2
    uint32_t baseAddr = (uint32_t)AddrTranslateP_getLocalAddr(CONFIG_IOL_DI_NLD_BASE_ADDR);
    GPIO_pinWriteHigh(baseAddr, CONFIG_IOL_DI_NLD_PIN);

    MCSPI_Transaction_init(&transaction);

    transaction.channel   = IOLM_SPI_IQ_CHANNEL;
    transaction.txBuf     = NULL;
    transaction.rxBuf     = rxBuf;
    transaction.dataSize  = IOLM_SPI_IQ_DATA_SIZE;
    transaction.count     = sizeof(rxBuf) / (IOLM_SPI_IQ_DATA_SIZE / 8);
    transaction.csDisable = TRUE;

    SemaphoreP_pend(&mutexIolPeriphery, SystemP_WAIT_FOREVER);
    error = MCSPI_transfer(gMcspiConfig[IOLM_SPI_IQ_INSTANCE].object->handle, &transaction);
    SemaphoreP_post(&mutexIolPeriphery);

    if (error != SystemP_SUCCESS)
    {
        OSAL_error(
            __FILE__,
            __LINE__,
            OSAL_eERR_INVALIDSTATE,
            false,
            1,
            "Error while retrieving IQ state.\r\n");
    }

    iqState = (rxBuf[0] >> portNum) & 0x1;

    // reset pin nLD
    GPIO_pinWriteLow(baseAddr, CONFIG_IOL_DI_NLD_PIN);

    return iqState;
}

void IOLM_IQ_DRV_set(uint8_t portNum, bool value)
{
    // not supported on this hardware
    (void)portNum;
    (void)value;
}

void IOLM_IQ_DRV_setMode(uint8_t portNum, IOL_EIQMode eIQMode)
{
    // not supported on this hardware
    (void)portNum;
    (void)eIQMode;
}
