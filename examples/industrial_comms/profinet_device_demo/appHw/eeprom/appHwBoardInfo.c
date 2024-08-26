/*!
 *  \example appHwBoardInfo.c
 *
 *  \brief
 *  Common implementation to get the board info from eeprom
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

#include <stdio.h>
#include <string.h>
#include "board/eeprom.h"
#include "ti_board_open_close.h"
#include "appHwBoardInfo.h"
#include "appHwError.h"

/*!
 *  \brief We store the board info data here.
 */
static APP_HW_BOARD_INFO_SData_t APP_HW_BOARD_INFO_data_g = { 0 };

/*!
 *  Function: APP_HW_BOARD_INFO_read
 *
 *  \brief
 *  Reads the board info data from the AT24C EEPROM.
 *
 *  \details
 *  Reads the board info data from the AT24C EEPROM (I2C address 0x50) and updates the
 *  global variable #APP_HW_BOARD_INFO_data_g
 *
 *  \remarks
 *  This is a blocking function, since i2c transfers are blocking. Ideally this function
 *  should only be called once during startup.
 *
 *  \return        Error code as #APP_HW_EError_t
 *  \retval        #APP_HW_eNO_ERROR          No error.
 *  \retval        #APP_HW_eFATAL_ERROR       A fatal error occurred.
 */
APP_HW_EError_t APP_HW_BOARD_INFO_read(void)
{
    int32_t status = SystemP_SUCCESS;
    APP_HW_EError_t result = APP_HW_eNO_ERROR;

    status = EEPROM_read(gEepromHandle[CONFIG_EEPROM0], 0, (uint8_t *)&APP_HW_BOARD_INFO_data_g, sizeof(APP_HW_BOARD_INFO_data_g));

    if (status != SystemP_SUCCESS)
    {
        result = APP_HW_eFATAL_ERROR;
    }
    return result;
}

/*!
 *  Function: APP_HW_BOARD_INFO_getMacInfo
 *
 *  \brief
 *  Gets a copy of the read mac address section from the board info EEPROM.
 *
 *  \remarks
 *  Call #APP_HW_BOARD_INFO_read first to read the data from EEPROM.
 *
 *  \return        copy of the member "mac" of #APP_HW_BOARD_INFO_data_g
 */
APP_HW_BOARD_INFO_SMAC_Addr_t *APP_HW_BOARD_INFO_getMacInfo(void)
{
    return &APP_HW_BOARD_INFO_data_g.mac;
}
