/*!
 *  \file cust_ethphy.c
 *
 *  \brief
 *  Provides initialization of custom ETHPHY's connected to PRU-ICSS block.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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
/*!
 *  \details
 *  Custom ETHPHY driver requires following:
 *  - PRU-ICSS block is configured in SysConfig.
 *  - 2x ETHPHY's configured in SysConfig as CUSTOM
 */

#include "ti_board_open_close.h"

#include "board/ethphy.h"

#if defined (SOC_AM243X) || defined (SOC_AM64X)
#include "drivers/pinmux/am64x_am243x/pinmux.h"
#include "drivers/pruicss/g_v0/pruicss.h"
#include "drivers/mdio/v0/mdio.h"
#elif defined (SOC_AM263PX)
#include "drivers/pinmux/am263px/pinmux.h"
#include "drivers/pruicss/m_v0/pruicss.h"
#include "drivers/mdio/v0/mdio.h"
#elif defined (SOC_AM261X)
#include "drivers/pinmux/am261x/pinmux.h"
#include "drivers/pruicss/m_v0/pruicss.h"
#include "drivers/mdio/v0/mdio.h"
#else
#error "Undefined SOC"
#endif

#include "osal.h"

#include "ethphy/cust_ethphy.h"

#if !(defined FBTL_REMOTE) || (0 == FBTL_REMOTE)
#include "CUST_PHY_base.h"

#if (defined CONFIG_PRUICSS_NUM_INSTANCES) && (CONFIG_PRUICSS_NUM_INSTANCES > 0)
extern PRUICSS_Config gPruIcssConfig[];

#if (defined CONFIG_ETHPHY_NUM_INSTANCES) && (CONFIG_ETHPHY_NUM_INSTANCES > 1)
extern ETHPHY_Config  gEthPhyConfig[];
extern ETHPHY_Handle  gEthPhyHandle[];

#if defined (SOC_AM243X) || defined (SOC_AM64X) || defined (SOC_AM263PX)
ETHPHY_Fxns gEthPhyFxns_DP83869_0 = { .openFxn    = NULL,
                                      .closeFxn   = NULL,
                                      .commandFxn = NULL};

ETHPHY_Fxns gEthPhyFxns_DP83869_1 = { .openFxn    = NULL,
                                      .closeFxn   = NULL,
                                      .commandFxn = NULL };
#elif defined (SOC_AM261X)
ETHPHY_Fxns gEthPhyFxns_DP83826_0 = { .openFxn    = NULL,
                                      .closeFxn   = NULL,
                                      .commandFxn = NULL};

ETHPHY_Fxns gEthPhyFxns_DP83826_1 = { .openFxn    = NULL,
                                      .closeFxn   = NULL,
                                      .commandFxn = NULL };
#endif // defined (SOC_AM243X) || defined (SOC_AM64X) || defined (SOC_AM263PX)

#endif // (defined CONFIG_ETHPHY_NUM_INSTANCES) && (CONFIG_ETHPHY_NUM_INSTANCES > 1)

#endif // (defined CONFIG_PRUICSS_NUM_INSTANCES) && (CONFIG_PRUICSS_NUM_INSTANCES > 0)

#endif // !(defined FBTL_REMOTE) || (0 == FBTL_REMOTE)

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

#if !(defined FBTL_REMOTE) || (0 == FBTL_REMOTE)
    int32_t  status          = SystemP_FAILURE;
    uint32_t mdioBaseAddress = 0;

#if (defined CONFIG_PRUICSS_NUM_INSTANCES) && (CONFIG_PRUICSS_NUM_INSTANCES > 0)
    pPruIcssCfg = &gPruIcssConfig[pParams->pruIcssSysConfigId];
#endif

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
#if defined (SOC_AM243X) || defined (SOC_AM64X)
            // Pinmux for the link interrupt pins, we will set it to MLINK later.
            Pinmux_PerCfg_t tempPinMuxCfg[] = {
                { PIN_PRG1_PRU0_GPO8, (PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE) },
                { PIN_PRG1_PRU1_GPO8, (PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE) },
                {PINMUX_END, PINMUX_END}
            };

            Pinmux_config(tempPinMuxCfg, PINMUX_DOMAIN_ID_MAIN);
#endif
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

    switch(pPruIcssCfg->hwAttrs->instance)
    {
        case 0:
        {
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
        }
        break;
        default:
            break;
    }

    CUST_PHY_CBregisterLibDetect(CUST_PHY_detect, NULL);

#endif // !(defined FBTL_REMOTE) || (0 == FBTL_REMOTE)

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

#if !(defined FBTL_REMOTE) || (0 == FBTL_REMOTE)

#if (defined CONFIG_ETHPHY_NUM_INSTANCES) && (CONFIG_ETHPHY_NUM_INSTANCES > 0)
    if (CONFIG_ETHPHY_NUM_INSTANCES > instance)
    {
        pEthPhyCfg = &gEthPhyConfig[instance];
    }
#endif

#else
    OSALUNREF_PARM(instance);
#endif

    return pEthPhyCfg;
}

/*!
*
*  \brief
*  Provides specific ETHPHY handle defined by instance.
*
*  \param[in]  instance       SysConfig ID of ETHPHY
*
*  \return     requested ETHPHY handler
*
*  \retval     NULL           Failed.
*  \retval     Other          Success.
*
*/
ETHPHY_Handle CUST_ETHPHY_getHandle (uint32_t instance)
{
    ETHPHY_Handle handle = NULL;

#if (defined CONFIG_ETHPHY_NUM_INSTANCES) && (CONFIG_ETHPHY_NUM_INSTANCES > 0)
    if (CONFIG_ETHPHY_NUM_INSTANCES > instance)
    {
        handle = gEthPhyHandle[instance];
    }
#else
    OSALUNREF_PARM(instance);
#endif

    return handle;
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
    bool isMlinkBased = false;

#if (defined MDIO_MANUAL_MODE_FW_CONFIG_VALUE)
    uint32_t config = MDIO_MANUAL_MODE_FW_CONFIG_VALUE;
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
