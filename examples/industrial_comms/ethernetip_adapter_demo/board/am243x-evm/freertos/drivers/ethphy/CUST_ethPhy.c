/*!
 *  \file CUST_ethPhy.c
 *
 *  \brief
 *  Provides initialization of custom ETHPHY's.
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

#include "drivers/mdio/v0/mdio.h"

#include "drivers/pinmux/am64x_am243x/pinmux.h"

#include "drivers/CUST_drivers.h"
#include "drivers/ethphy/CUST_ethPhy.h"

#include "CUST_PHY_base.h"

extern ETHPHY_Config  gEthPhyConfig[];
extern ETHPHY_Handle  gEthPhyHandle[];

ETHPHY_Fxns gEthPhyFxns_DP83869_0 = { .openFxn    = NULL,
                                      .closeFxn   = NULL,
                                      .commandFxn = NULL};

ETHPHY_Fxns gEthPhyFxns_DP83869_1 = { .openFxn    = NULL,
                                      .closeFxn   = NULL,
                                      .commandFxn = NULL };

/*!
*
* \brief
* Custom ETHPHY initialization.
*
* \param[in]  pParams    Pointer to initialization parameters.
*
* \return     Error code as uint32_t.
*
* \retval     CUST_ETHPHY_eERR_NOERROR       Success.
* \retval     CUST_ETHPHY_eERR_GENERALERROR  Failed.
*
*/
uint32_t CUST_ETHPHY_init(CUST_ETHPHY_SParams_t* pParams)
{
    PRUICSS_Config* pPruIcssCfg                     = NULL;
    ETHPHY_Config*  pEthPhyCfg[CUST_ETHPHY_MAX_NUM] = {NULL};
    uint32_t        error                           = (uint32_t) CUST_ETHPHY_eERR_GENERALERROR;

    int32_t  status          = SystemP_FAILURE;
    uint32_t mdioBaseAddress = 0;

    pPruIcssCfg = CUST_PRUICSS_getConfig(pParams->pruIcssSysConfigId);

    if (NULL == pPruIcssCfg)
    {
        goto laError;
    }

    for (uint32_t i = 0; i < CUST_ETHPHY_MAX_NUM; i++)
    {
        switch(i)
        {
            case 0:
            {
                pEthPhyCfg[0] = CUST_ETHPHY_getConfig(pParams->ethPhySysConfigId_0);
                break;
            }
            case 1:
            {
                pEthPhyCfg[1] = CUST_ETHPHY_getConfig(pParams->ethPhySysConfigId_1);
                break;
            }
            default:
            {
                break;
            }
        }

        if (NULL == pEthPhyCfg[i])
        {
            goto laError;
        }

        pEthPhyCfg[i]->fxns->openFxn    = NULL;
        pEthPhyCfg[i]->fxns->closeFxn   = NULL;
        pEthPhyCfg[i]->fxns->commandFxn = NULL;
    }

    if (true == CUST_ETHPHY_MDIOMANUALMODE_isEnabled())
    {
        if (true == CUST_ETHPHY_MDIOMANUALMODE_isMlinkBased())
        {
            // Pinmux for the link interrupt pins, we will set it to MLINK later.
            Pinmux_PerCfg_t tempPinMuxCfg[] = {
                /* PR1_MII0_RXLINK -> PRG1_PRU0_GPO8 (W13) */
                { PIN_PRG1_PRU0_GPO8, (PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE) },
                /* PR1_MII1_RXLINK -> PRG1_PRU1_GPO8 (U12) */
                { PIN_PRG1_PRU1_GPO8, (PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE) },
                {PINMUX_END, PINMUX_END}
            };

            Pinmux_config(tempPinMuxCfg, PINMUX_DOMAIN_ID_MAIN);
        }
    }
    else
    {
        mdioBaseAddress = (uint32_t) pPruIcssCfg->hwAttrs->miiMdioRegBase;

        status = MDIO_initClock(mdioBaseAddress);

        if (SystemP_SUCCESS != status)
        {
            goto laError;
        }
    }

    CUST_PHY_CBregisterLibDetect(CUST_PHY_detect, NULL);

    error = (uint32_t) CUST_ETHPHY_eERR_NOERROR;

laError:
    return error;
}

/*!
*
* \brief
* Custom PHY deinitialization.
*
* \return     uint32_t                    Error code.
*
* \retval     CUST_ETHPHY_eERR_NOERROR       Success.
* \retval     CUST_ETHPHY_eERR_GENERALERROR  Failed.
*
*/
uint32_t CUST_ETHPHY_deInit(void)
{
    return (uint32_t) CUST_ETHPHY_eERR_NOERROR;
}

/*!
*
* \brief
* Provides pointer to specific SysConfig ETHPHY configuration defined by instance.
*
*  \param[in]  instance        SysConfig ID of ETHPHY
*
*  \return     pointer to requested ETHPHY configuration
*
*/
ETHPHY_Config* CUST_ETHPHY_getConfig (uint32_t instance)
{
    ETHPHY_Config* pEthPhyCfg = NULL;

    if (CONFIG_ETHPHY_NUM_INSTANCES > instance)
    {
        pEthPhyCfg = &gEthPhyConfig[instance];
    }

    return pEthPhyCfg;
}

/*!
*
* \brief
* Provides pointer to specific ETHPHY handler defined by instance.
*
*  \param[in]  instance       SysConfig ID of ETHPHY
*
*  \return     requested ETHPHY handler
*
*/
ETHPHY_Handle CUST_ETHPHY_getHandle (uint32_t instance)
{
    ETHPHY_Handle ethPhyHandle = NULL;

    if (CONFIG_ETHPHY_NUM_INSTANCES > instance)
    {
        ethPhyHandle = gEthPhyHandle[instance];
    }

    return ethPhyHandle;
}

/*!
*
* \brief
* Provides information whether MDIO manual mode is enabled.
*
*  \return     MDIO manual mode ON/OFF setting
*
*  \retval     true   MDIO Manual Mode is ON.
*  \retval     false  MDIO Manual Mode is OFF.
*/
bool CUST_ETHPHY_MDIOMANUALMODE_isEnabled (void)
{
    bool isEnabled = false;

#if (defined MDIO_MANUAL_MODE_ENABLED) && (defined MDIO_MANUAL_MODE_BASE_ADDRESS)
    isEnabled = true;
#endif

    return isEnabled;
}

/*!
*
* \brief
* Provides information whether in MDIO Manual Mode Link Status Update
* is set to MLINK based setting.
*
*  \return     MLINK based flag
*
*  \retval     true   MDIO Manual Mode Link Status Update is set to MLINK based.
*  \retval     false  MDIO Manual Mode Link Status Update is not set to MLINK based.
*/
bool CUST_ETHPHY_MDIOMANUALMODE_isMlinkBased (void)
{
    bool     isMlinkBased = false;
    uint32_t config       = 0;

#if (defined MDIO_MANUAL_MODE_FW_CONFIG_VALUE)
    config = MDIO_MANUAL_MODE_FW_CONFIG_VALUE;
#endif

#if (defined MDIO_MANUAL_MODE_LINK_POLLING_DISABLE)
    if (MDIO_MANUAL_MODE_LINK_POLLING_DISABLE == (config & MDIO_MANUAL_MODE_LINK_POLLING_ENABLE))
    {
        isMlinkBased = true;
    }
#endif

    return isMlinkBased;
}

/*!
*
* \brief
* Provides MDIO manual mode base address as defined in SysConfig.
*
*  \return     MDIO manual mode base address
*
*  \retval     0      MDIO Manual Mode Base Address is not defined.
*  \retval     other  MDIO Manual Mode Base Address as defined.
*/
uint32_t CUST_ETHPHY_MDIOMANUALMODE_getBaseAddress (void)
{
    uint32_t baseAddress = 0;

#if (defined MDIO_MANUAL_MODE_ENABLED) && (defined MDIO_MANUAL_MODE_BASE_ADDRESS)
    baseAddress = MDIO_MANUAL_MODE_BASE_ADDRESS;
#endif

    return baseAddress;
}
