/*!
 *  \file CUST_led.c
 *
 *  \brief
 *  Provides initialization of custom LED's.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 None.
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
#include <stdio.h>
#include <string.h>

#include <board/led.h>
#include <drivers/i2c.h>

#include <drivers/led/CUST_led.h>
#include "ti_board_config.h"
#include "ti_board_open_close.h"

/*!
 *
 *  \brief
 *  Initialization of custom LEDs on board.
 *
 *  \return     uint32_t                            Error code.
 *
 *  \retval     CUST_LED_eERR_NOERROR               Success.
 *
 */
uint32_t CUST_LED_init(void)
{
    // Custom LED's are currently not supported by SysConfig

    return (uint32_t) CUST_LED_eERR_NOERROR;
}

/*!
 *
 *  \brief
 *  Deinitialization of custom LEDs on board.
 *
 *  \return     uint32_t                        Error code.
 *
 *  \retval     CUST_LED_eERR_NOERROR           Success.
 *
 */
uint32_t CUST_LED_deInit(void)
{
    // Custom LED's are currently not supported by SysConfig

    return (uint32_t) CUST_LED_eERR_NOERROR;
}

/*!
*
*  \brief
*  Provides handle to LED driver.
*
*  \return     LED_Handle  Handle to LED driver.
*
*  \retval     NULL           Failed.
*  \retval     Other          Success.
*
*/
LED_Handle CUST_LED_getHandle(uint32_t instanceId)
{
    LED_Handle handle = NULL;

    if (CONFIG_LED_NUM_INSTANCES > instanceId)
    {
        handle = gLedHandle[instanceId];
    }

    return handle;
}
