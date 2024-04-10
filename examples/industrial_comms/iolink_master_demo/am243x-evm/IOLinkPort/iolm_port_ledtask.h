/*!
 *  \file iolm_port_ledtask.h
 *
 *  \brief
 *  Interface for LED Handling on IOLink Board.
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

#ifndef IO_LINK_IOLINK_LEDTASK_H_
#define IO_LINK_IOLINK_LEDTASK_H_

/* ========================================================================== */
/*                         Includes                                           */
/* ========================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================== */
/*                          LED typedefs/structs                              */
/* ========================================================================== */

typedef enum IOLM_LEDeState
{
    IOLM_ELEDState_Off,
    IOLM_ELEDState_On,
    IOLM_ELEDState_Slow,
    IOLM_ELEDState_Fast,
    IOLM_ELEDState_Data
} IOLM_LED_eState_t;

typedef enum IOLM_LED_EColor
{
    IOLM_ELEDColor_Green,
    IOLM_ELEDColor_Red
} IOLM_LED_eColor_t;

typedef struct IOLM_LED_SLedMapping
{
    uint8_t           port;
    IOLM_LED_eColor_t eLedColor;
} IOLM_LED_sLedMapping_t;

typedef struct IOLM_LED_SLedCfg
{
    /** \brief init LED communication interface */
    void (*cbInit)(void);
    /** \brief close LED communication interface */
    void (*cbClose)(void);
    /** \brief set LEDs on add on board via 16 Bit bitmask.
     * With Bit0=port0-red, Bit1=port0-green,... */
    int32_t (*cbSetLEDs)(uint16_t bitmask);
    /** \brief set individual LEDs on the base board */
    int32_t (*cbBaseBoardLED)(uint32_t portNum, bool targetLedState);
} IOLM_LED_sLedCfg_t;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

void IOLM_LED_setLedColorState(uint8_t port, IOLM_LED_eColor_t color, IOLM_LED_eState_t state);
extern void IOLM_LED_switchingTask(void);

#endif /* IO_LINK_IOLINK_LEDTASK_H_ */
