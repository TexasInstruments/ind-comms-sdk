/*!
 *  \file iolm_port_led.c
 *
 *  \brief
 *  Interface for LED IO-Expander Communication on IOLink Board.
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

#include <osal.h>
#include <board/led.h>
#include "iolm_port_led.h"
#include "ti_board_open_close.h"
#include <board/ioexp/ioexp_tca6424.h>
TCA6424_Config ioexpBoostxl;

extern LED_Handle gLedHandle[]; /* LED handle is created by sysCfg */

/**
 * \brief  Initialize the IO-Expander instance
 * Init is done on bootup via SysCfg and doesn't need to be executed again
 *
 */
void IOLM_LED_IOEXP_init(void)
{
    TCA6424_Params params;

    TCA6424_Params_init(&params);
    params.i2cAddress = 0x22;
    params.i2cInstance = CONFIG_I2C_IOL;

    TCA6424_open(&ioexpBoostxl, &params);

    for (int index=0; index < 16; index++)
    {
        TCA6424_config(&ioexpBoostxl, index, TCA6424_MODE_OUTPUT);
    }
}

/**
 * \brief  Close the IO-Expander instance
 *
 */
void IOLM_LED_IOEXP_close(void)
{
}

/**
 * \brief  Transfer LED Bitmask to IOLink IO-Expander Interface
 * address_p is not used here. Other targets use the same interface for MCSPI,
 * which needs an address of the recipient.
 *
 * \param  tx_p      bitmask data to send
 *
 * \return error if not equal 0
 *
 */
int32_t IOLM_LED_IOEXP_transfer(uint16_t bitmask)
{
    int32_t  error = SystemP_SUCCESS;
    uint32_t shiftedBitmask = bitmask;
    uint32_t currentLedNumber;

    for (currentLedNumber = 0; currentLedNumber < 16;
         currentLedNumber++) /* walk though bitmask */
    {
        if (shiftedBitmask & 0x01)
        {
            error += TCA6424_setOutput(&ioexpBoostxl, currentLedNumber, TCA6424_OUT_STATE_LOW); /* off -> on on TCA6424 */
        }
        else
        {
            error += TCA6424_setOutput(&ioexpBoostxl, currentLedNumber, TCA6424_OUT_STATE_HIGH); /* on -> off on TCA6424 */
        }
        shiftedBitmask >>= 1;
    }
    return error;
}

/**
 * \brief  Set state of LED on base board
 *
 * \param[in]   ledNumber     number of LED
 * \param[in]   ledState      target state
 *
 * \return     error                           as uint32_t
 * \retval     #OSAL_eERR_NOERROR              Success.
 * \retval     #else                           Something went wrong.
 *
 */
int32_t IOLM_LED_IOEXP_baseBoardLED(uint32_t ledNumber, bool ledState)
{
    int32_t  error    = SystemP_SUCCESS;
    uint32_t instance = IOLM_LED_IOEXP_LED_NUM_IOL + ledNumber;

    if (ledState == true)
    {
        error = LED_on(gLedHandle[instance], ledNumber);
    }
    else
    {
        error = LED_off(gLedHandle[instance], ledNumber);
    }

    return error;
}

