/*!
 *  \file CUST_pruIcss.c
 *
 *  \brief
 *  Provides initialization of custom PRU-ICSS.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
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

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#include "drivers/CUST_drivers.h"
#include "drivers/ethphy/CUST_ethPhy.h"
#include "drivers/pru_icss/CUST_pruIcss.h"

#if defined(EIP_RGMII)
  #define PRU0_FIRMWARE CUST_PRUICSS_PRU_Firmware_0
  #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/rgmii/ethernetip_adapter_pru0_bin.h>
  #define PRU1_FIRMWARE CUST_PRUICSS_PRU_Firmware_1
  #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/rgmii/ethernetip_adapter_pru1_bin.h>
#else
  #define PRU0_FIRMWARE CUST_PRUICSS_PRU_Firmware_0
  #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/mii/ethernetip_adapter_pru0_bin.h>
  #define PRU1_FIRMWARE CUST_PRUICSS_PRU_Firmware_1
  #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/mii/ethernetip_adapter_pru1_bin.h>
#endif

#if (defined MDIO_MANUAL_MODE_ENABLED) && (!defined SOC_AM263X)
    #define PRUFirmware CUST_PRUICSS_MDIO_Firmware
    #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/mdio_fw_bin.h>
#endif

extern PRUICSS_Config gPruIcssConfig[];

static CUST_PRUICSS_ConfigParams_t CUST_PRUICSS_pConfigParams_s = {0};

/*!
*
* \brief
* Initialize PRU-ICSS configuration parameters.
*
*  \return     error code as uint32_t
*  \retval     #CUST_PRUICSS_eERR_NOERROR Success.
*
*/
uint32_t CUST_PRUICSS_init (CUST_PRUICSS_InitParams_t* pParams)
{
    CUST_PRUICSS_pConfigParams_s.sysConfigId = pParams->pruIcssId;
    CUST_PRUICSS_pConfigParams_s.pPruIcssCfg = CUST_PRUICSS_getConfig(pParams->pruIcssId);

#if (defined PRU0_FIRMWARE)
    CUST_PRUICSS_pConfigParams_s.pru[0].pFirmware  = (uint32_t*) CUST_PRUICSS_PRU_Firmware_0;
    CUST_PRUICSS_pConfigParams_s.pru[0].frmLength  = sizeof(CUST_PRUICSS_PRU_Firmware_0);
#endif

    CUST_PRUICSS_pConfigParams_s.pru[0].ethPhy.sysConfigId = pParams->ethPhy0Id;
    CUST_PRUICSS_pConfigParams_s.pru[0].ethPhy.pConfig     = CUST_ETHPHY_getConfig(pParams->ethPhy0Id);
    CUST_PRUICSS_pConfigParams_s.pru[0].ethPhy.handle      = CUST_ETHPHY_getHandle(pParams->ethPhy0Id);

#if (defined PRU1_FIRMWARE)
    CUST_PRUICSS_pConfigParams_s.pru[1].pFirmware  = (uint32_t*) CUST_PRUICSS_PRU_Firmware_1;
    CUST_PRUICSS_pConfigParams_s.pru[1].frmLength  = sizeof(CUST_PRUICSS_PRU_Firmware_1);
#endif

    CUST_PRUICSS_pConfigParams_s.pru[1].ethPhy.sysConfigId = pParams->ethPhy1Id;
    CUST_PRUICSS_pConfigParams_s.pru[1].ethPhy.pConfig     = CUST_ETHPHY_getConfig(pParams->ethPhy1Id);
    CUST_PRUICSS_pConfigParams_s.pru[1].ethPhy.handle      = CUST_ETHPHY_getHandle(pParams->ethPhy1Id);

    CUST_PRUICSS_pConfigParams_s.manualMdioMode.isEnabled   = CUST_ETHPHY_MDIOMANUALMODE_isEnabled();

    if (true == CUST_PRUICSS_pConfigParams_s.manualMdioMode.isEnabled)
    {
        CUST_PRUICSS_pConfigParams_s.manualMdioMode.baseAddress = CUST_ETHPHY_MDIOMANUALMODE_getBaseAddress();

#if (defined PRUFirmware)
        CUST_PRUICSS_pConfigParams_s.manualMdioMode.pFirmware   = (uint32_t*) CUST_PRUICSS_MDIO_Firmware;
        CUST_PRUICSS_pConfigParams_s.manualMdioMode.frmLength   = sizeof(CUST_PRUICSS_MDIO_Firmware);
        CUST_PRUICSS_pConfigParams_s.manualMdioMode.frmConfig   = MDIO_MANUAL_MODE_FW_CONFIG_VALUE;
#endif
    }

    // nothing needs to be done for this board
    return (uint32_t) CUST_PRUICSS_eERR_NOERROR;
}

/*!
*
* \brief
* Provides pointer to configuration parameters of PRU-ICSS block.
*
*  \return     pointer to requested PRU-ICSS block configuration
*
*/
CUST_PRUICSS_ConfigParams_t* CUST_PRUICSS_getCfgParams (void)
{
    CUST_PRUICSS_ConfigParams_t* pPruIcssCfgParams = NULL;

    pPruIcssCfgParams = &CUST_PRUICSS_pConfigParams_s;

    return pPruIcssCfgParams;
}

/*!
*
* \brief
* Provides pointer to PRU-ICSS configuration.
*
*  \return     pointer to requested PRU-ICSS block configuration
*
*/
PRUICSS_Config* CUST_PRUICSS_getConfig (uint32_t sysConfigId)
{
    PRUICSS_Config* pPruIcssCfg = NULL;

    pPruIcssCfg = &gPruIcssConfig[sysConfigId];

    return pPruIcssCfg;
}


