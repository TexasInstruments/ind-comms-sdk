/*!
 *  \example appPRUConfig.c
 *
 *  \brief
 *  PROFINET Device Stack Initialization, PRU Start, and Stop Functions
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

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <osal.h>
#include <osal_error.h>

#include <pru_Profinet.h>

#include "appPRUConfig.h"
#include "appHwBoardInfo.h"
#include "appHwError.h"

#include "custom_phy.h"

#ifndef UNIT_TESTS
#include "ti_board_open_close.h"
#include "CUST_PHY_base.h"
#endif // UNIT_TESTS

extern PRUICSS_Config gPruIcssConfig[];
extern ETHPHY_Config  gEthPhyConfig[];

uint8_t *APP_UI_getMacAddr (void)
{
    return &APP_HW_BOARD_INFO_getMacInfo()->macAddr[0];
}

/*!
*  <!-- Description: -->
* \brief
* PRU initialization.
*
* \details
* Here we configure relevant PRU instance for Profinet communication.
* PRU instance will be initialized and firmware will be loaded.
*
* \remarks
* Important: Configure correct MAC address.
*
*
* <!-- Parameters and return values: -->
*
*  \return     Error code of int type. See defines with namespace OSAL_PRU_ in osal_error.h
*  \retval     OSAL_NO_ERROR           Success.
*
*/
uint32_t APP_pruInit (void)
{
    PRU_PN_TPruLoadParameter    pruParam   = {0};
    uint32_t                    result     = OSAL_NO_ERROR;
    APP_HW_EError_t             status     = APP_HW_eNO_ERROR;

    status = APP_HW_BOARD_INFO_read();

    if (status == APP_HW_eNO_ERROR)
    {
        memset(&pruParam, 0, sizeof(pruParam));
        memmove(pruParam.aMacAddr, APP_UI_getMacAddr(), PRU_PN_MAC_ADDR_LEN);

        pruParam.pruIcssCfgId = CONFIG_PRU_ICSS1;

        pruParam.ethPhys.instance[0].pCfg   = &gEthPhyConfig[0];
        pruParam.ethPhys.instance[0].handle = &gEthPhyHandle[0];

        pruParam.ethPhys.instance[1].pCfg   = &gEthPhyConfig[1];
        pruParam.ethPhys.instance[1].handle = &gEthPhyHandle[1];

#ifdef MDIO_MANUAL_MODE_ENABLED
        pruParam.drivers.mdioManualMode      = true;
        pruParam.drivers.mdioBaseAddress     = MDIO_MANUAL_MODE_BASE_ADDRESS;
        pruParam.drivers.pMdioFirmware       = NULL;
        pruParam.drivers.mdioFirmwareLength  = 0;
        pruParam.drivers.mdioFirmwareConfig  = MDIO_MANUAL_MODE_FW_CONFIG_VALUE;
#endif

#if (defined GPIO_RESET_ICSS0_PHY1_PIN) && (defined GPIO_RESET_ICSS0_PHY2_PIN)
        GPIO_pinWriteHigh(GPIO_RESET_ICSS0_PHY1_BASE_ADDR, GPIO_RESET_ICSS0_PHY1_PIN);
        GPIO_pinWriteHigh(GPIO_RESET_ICSS0_PHY2_BASE_ADDR, GPIO_RESET_ICSS0_PHY2_PIN);
        ClockP_usleep(ClockP_ticksToUsec(200));

        GPIO_pinWriteLow(GPIO_RESET_ICSS0_PHY1_BASE_ADDR, GPIO_RESET_ICSS0_PHY1_PIN);
        GPIO_pinWriteLow(GPIO_RESET_ICSS0_PHY2_BASE_ADDR, GPIO_RESET_ICSS0_PHY2_PIN);
        ClockP_usleep(ClockP_ticksToUsec(200));

        GPIO_pinWriteHigh(GPIO_RESET_ICSS0_PHY1_BASE_ADDR, GPIO_RESET_ICSS0_PHY1_PIN);
        GPIO_pinWriteHigh(GPIO_RESET_ICSS0_PHY2_BASE_ADDR, GPIO_RESET_ICSS0_PHY2_PIN);
        ClockP_usleep(ClockP_ticksToUsec(200));
#endif

        CUST_PHY_CBregisterLibDetect(CUST_PHY_detect, NULL);

        /*Initialize PRU and load Firmware*/
        result = PRU_PN_loadPru(&pruParam);
        if (OSAL_NO_ERROR != result)
        {
            OSAL_printf("\r\n[APP] ERROR: Failed to init the PRU. Error code: 0x%8.8x\n", result);
        }
    }
    else
    {
        OSAL_printf("\r\n[APP] ERROR: Failed to get board info from EEPROM.\n");
        // Return a generic stack init error.
        result = OSAL_STACK_INIT_ERROR;
    }

    return result;
}


//*************************************************************************************************
