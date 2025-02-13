/*!
 *  \file drv_pruicss.c
 *
 *  \brief
 *  PRU-ICSS application driver.
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

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#include "osal.h"

#include "ethphy/cust_ethphy.h"

#include "drivers/pruicss/drv_pruicss.h"

#if defined(EIP_RGMII)
  #define PRU0_FIRMWARE DRV_PRUICSS_PRU_Firmware_0
  #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/rgmii/ethernetip_adapter_pru0_bin.h>
  #define PRU1_FIRMWARE DRV_PRUICSS_PRU_Firmware_1
  #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/rgmii/ethernetip_adapter_pru1_bin.h>
#else
  #define PRU0_FIRMWARE DRV_PRUICSS_PRU_Firmware_0
  #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/mii/ethernetip_adapter_pru0_bin.h>
  #define PRU1_FIRMWARE DRV_PRUICSS_PRU_Firmware_1
  #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/mii/ethernetip_adapter_pru1_bin.h>
#endif

#if (defined MDIO_MANUAL_MODE_ENABLED) && (!defined SOC_AM263X)
    #define PRUFirmware DRV_PRUICSS_MDIO_Firmware
    #include <industrial_comms/ethernetip_adapter/icss_fwhal/firmware/mdio_fw_bin.h>
#endif

#if !(defined FBTL_REMOTE) || (0 == FBTL_REMOTE)
extern PRUICSS_Config gPruIcssConfig[];
#else
int32_t        gPruIcssConfigNum = 0;
PRUICSS_Config gPruIcssConfig[0];
#endif

static DRV_PRUICSS_ConfigParams_t DRV_PRUICSS_pConfigParams_s = {0};

/*!
*
* \brief
* Initialize PRU-ICSS configuration parameters.
*
*  \return     error code as uint32_t
*  \retval     #CUST_PRUICSS_eERR_NOERROR Success.
*
*/
uint32_t DRV_PRUICSS_init (DRV_PRUICSS_SInit_t* pParams)
{
    DRV_PRUICSS_pConfigParams_s.sysConfigId = pParams->instance;
    DRV_PRUICSS_pConfigParams_s.pPruIcssCfg = DRV_PRUICSS_getConfig(pParams->instance);

#if (defined PRU0_FIRMWARE)
    DRV_PRUICSS_pConfigParams_s.pru[0].pFirmware  = (uint32_t*) DRV_PRUICSS_PRU_Firmware_0;
    DRV_PRUICSS_pConfigParams_s.pru[0].frmLength  = sizeof(DRV_PRUICSS_PRU_Firmware_0);
#endif

    DRV_PRUICSS_pConfigParams_s.pru[0].ethPhy.sysConfigId = pParams->ethPhy0Id;
    DRV_PRUICSS_pConfigParams_s.pru[0].ethPhy.pConfig     = CUST_ETHPHY_getConfig(pParams->ethPhy0Id);
    DRV_PRUICSS_pConfigParams_s.pru[0].ethPhy.handle      = CUST_ETHPHY_getHandle(pParams->ethPhy0Id);

#if (defined PRU1_FIRMWARE)
    DRV_PRUICSS_pConfigParams_s.pru[1].pFirmware  = (uint32_t*) DRV_PRUICSS_PRU_Firmware_1;
    DRV_PRUICSS_pConfigParams_s.pru[1].frmLength  = sizeof(DRV_PRUICSS_PRU_Firmware_1);
#endif

    DRV_PRUICSS_pConfigParams_s.pru[1].ethPhy.sysConfigId = pParams->ethPhy1Id;
    DRV_PRUICSS_pConfigParams_s.pru[1].ethPhy.pConfig     = CUST_ETHPHY_getConfig(pParams->ethPhy1Id);
    DRV_PRUICSS_pConfigParams_s.pru[1].ethPhy.handle      = CUST_ETHPHY_getHandle(pParams->ethPhy1Id);

    DRV_PRUICSS_pConfigParams_s.manualMdioMode.isEnabled   = CUST_ETHPHY_MDIOMANUALMODE_isEnabled();

    if (true == DRV_PRUICSS_pConfigParams_s.manualMdioMode.isEnabled)
    {
        DRV_PRUICSS_pConfigParams_s.manualMdioMode.baseAddress = CUST_ETHPHY_MDIOMANUALMODE_getBaseAddress();

#if (defined PRUFirmware)
        DRV_PRUICSS_pConfigParams_s.manualMdioMode.pFirmware   = (uint32_t*) DRV_PRUICSS_MDIO_Firmware;
        DRV_PRUICSS_pConfigParams_s.manualMdioMode.frmLength   = sizeof(DRV_PRUICSS_MDIO_Firmware);
        DRV_PRUICSS_pConfigParams_s.manualMdioMode.frmConfig   = MDIO_MANUAL_MODE_FW_CONFIG_VALUE;
#endif
    }


    {
        #define MSS_CTRL_ICSSM_PRU_GPIO_OUT_CTRL_VALUE  (0x0001077F)

#if (defined SOC_AM261X)
        HW_WR_REG32(CSL_MSS_CTRL_U_BASE + CSL_MSS_CTRL_ICSSM0_PRU0_GPIO_OUT_CTRL, MSS_CTRL_ICSSM_PRU_GPIO_OUT_CTRL_VALUE);
        HW_WR_REG32(CSL_MSS_CTRL_U_BASE + CSL_MSS_CTRL_ICSSM0_PRU1_GPIO_OUT_CTRL, MSS_CTRL_ICSSM_PRU_GPIO_OUT_CTRL_VALUE);

#elif (defined SOC_AM263PX)
        HW_WR_REG32(CSL_MSS_CTRL_U_BASE + CSL_MSS_CTRL_ICSSM_PRU0_GPIO_OUT_CTRL, MSS_CTRL_ICSSM_PRU_GPIO_OUT_CTRL_VALUE);
        HW_WR_REG32(CSL_MSS_CTRL_U_BASE + CSL_MSS_CTRL_ICSSM_PRU1_GPIO_OUT_CTRL, MSS_CTRL_ICSSM_PRU_GPIO_OUT_CTRL_VALUE);

        OSAL_SCHED_sleep(300);
#endif
    }

    // nothing needs to be done for this board
    return (uint32_t) DRV_PRUICSS_eERR_NOERROR;
}

/*!
*
* \brief
* Provides pointer to configuration parameters of PRU-ICSS block.
*
*  \return     pointer to requested PRU-ICSS block configuration
*
*/
DRV_PRUICSS_ConfigParams_t* DRV_PRUICSS_getCfgParams (void)
{
    DRV_PRUICSS_ConfigParams_t* pPruIcssCfgParams = NULL;

#if !(defined FBTL_REMOTE) || (0 == FBTL_REMOTE)
    pPruIcssCfgParams = &DRV_PRUICSS_pConfigParams_s;
#endif

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
PRUICSS_Config* DRV_PRUICSS_getConfig (uint32_t sysConfigId)
{
    PRUICSS_Config* pPruIcssCfg = NULL;

#if !(defined FBTL_REMOTE) || (0 == FBTL_REMOTE)
    pPruIcssCfg = &gPruIcssConfig[sysConfigId];
#endif

    return pPruIcssCfg;
}
