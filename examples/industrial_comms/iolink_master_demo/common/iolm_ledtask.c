/*!
 *  \file iolm_ledtask.c
 *
 *  \brief
 *  Interface for LED Handling on IOLink Board.
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

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "ti_board_open_close.h"
#include <IOLM_Types.h>
#include "iolm_ledtask.h"
#include "iolm_sitara_soc.h"

/* ========================================================================== */
/*                                Defines                                     */
/* ========================================================================== */

#define IOLM_LEDSPERPORT             (2U)
#define IOLM_LED_TIMERCOUNT          (3U)
#define IOLM_TIMER_COUNTER_SLOW      (0U)
#define IOLM_TIMER_COUNTER_FAST      (1U)
#define IOLM_TIMER_COUNTER_DATA      (2)
#define IOLM_LED_BASEBOARD_HEARTBEAT (0U)

/* times in ms (MUST be multiples of IOL_LED_TASK_TICKRATE) */
#define IOLM_LED_TASK_TICKRATE       (100U)
#define IOLM_LED_SLOWBLINKTIME       (500U)
#define IOLM_LED_FASTBLINKTIME       (200U)
#define IOLM_LED_DATA_EXCHANGE_ON    (1000U)
#define IOLM_LED_DATA_EXCHANGE_OFF   (100U)

/* ========================================================================== */
/*                          Local Variables                                   */
/* ========================================================================== */

static IOLM_LED_eState_t IOLM_LED_aStates_s[IOLM_PORT_COUNT][IOLM_LEDSPERPORT];
static uint32_t          IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_LED_TIMERCOUNT];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void IOLM_LED_setLedColorState(uint8_t smiPort, IOLM_LED_eColor_t color, IOLM_LED_eState_t state)
{
    uint8_t portInternal = smiPort - 1;

    if ((portInternal < IOLM_PORT_COUNT) && (color < IOLM_LEDSPERPORT))
    {
        IOLM_LED_aStates_s[portInternal][color] = state;
    }
}

void OSAL_FUNC_NORETURN IOLM_LED_switchingTask(void)
{
    uint8_t  i;
    uint16_t ledState = 0;

    IOLM_LED_DRV_init();

    IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_SLOW] = 0;
    IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_FAST] = 0;
    IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_DATA] = 0;

    while (1)
    {
        IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_SLOW]++;
        IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_FAST]++;
        IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_DATA]++;

        for (i = 0; i < sizeof(IOLM_LED_aPortMapping_s) / sizeof(IOLM_LED_aPortMapping_s[0]); i++)
        {
            switch (
                IOLM_LED_aStates_s[IOLM_LED_aPortMapping_s[i].port][IOLM_LED_aPortMapping_s[i].eLedColor])
            {
                case IOLM_ELEDState_On:
                    ledState |= (1 << i);
                    break;
                case IOLM_ELEDState_Off:
                    ledState &= ~(1 << i);
                    break;
                case IOLM_ELEDState_Slow:
                    if (IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_SLOW]
                        <= IOLM_LED_SLOWBLINKTIME / IOLM_LED_TASK_TICKRATE)
                    {
                        ledState |= (1 << i);
                    }
                    else
                    {
                        ledState &= ~(1 << i);
                    }
                    break;
                case IOLM_ELEDState_Fast:
                    if (IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_FAST]
                        <= IOLM_LED_FASTBLINKTIME / IOLM_LED_TASK_TICKRATE)
                    {
                        ledState |= (1 << i);
                    }
                    else
                    {
                        ledState &= ~(1 << i);
                    }
                    break;
                case IOLM_ELEDState_Data:
                    if (IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_DATA]
                        <= IOLM_LED_DATA_EXCHANGE_ON / IOLM_LED_TASK_TICKRATE)
                    {
                        ledState |= (1 << i);
                    }
                    else
                    {
                        ledState &= ~(1 << i);
                    }
                    break;
                default:
                    break;
            }
        }

        SemaphoreP_pend(&mutexIolPeriphery, SystemP_WAIT_FOREVER);
        if (IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_SLOW]
            <= IOLM_LED_SLOWBLINKTIME / IOLM_LED_TASK_TICKRATE)
        {
            LED_on(gLedHandle[IOLM_LED_BASEBOARD_HEARTBEAT], 0);
        }
        else
        {
            LED_off(gLedHandle[IOLM_LED_BASEBOARD_HEARTBEAT], 0);
        }
        SemaphoreP_post(&mutexIolPeriphery);

        IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_SLOW]
            %= 2 * (IOLM_LED_SLOWBLINKTIME / IOLM_LED_TASK_TICKRATE);
        IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_FAST]
            %= 2 * (IOLM_LED_FASTBLINKTIME / IOLM_LED_TASK_TICKRATE);
        IOLM_LED_aLedBlinkSyncTimerCount_s[IOLM_TIMER_COUNTER_DATA]
            %= (IOLM_LED_DATA_EXCHANGE_ON / IOLM_LED_TASK_TICKRATE)
               + (IOLM_LED_DATA_EXCHANGE_OFF / IOLM_LED_TASK_TICKRATE);

        IOLM_LED_DRV_setStates(ledState);

        OSAL_SCHED_sleep(IOLM_LED_TASK_TICKRATE);
    }
}
