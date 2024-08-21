/*!
 *  \file CUST_PHY_dp83826b.c
 *
 *  \brief
 *  PHY implementation for TI DP83826 basic mode.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \date
 *  2022-02-24
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
 *  All rights reserved.<br />
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:<br />
 *  <ol>
 *  <li>Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.</li>
 *  <li>Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.</li>
 *  <li>Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.</li>
 *  </ol>
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <CUST_PHY_dp83826b.h>
#include <custom_phy.h>

#define CUST_PHY_DP83826B_IEEE_IDENT                        (0x2000A110u)
#define CUST_PHY_DP83826B_IEEE_IDENT_MASK                   (0xFFFFFFF0u)
#define CUST_PHY_DP83826B_AUTONEGTIMEOUT                    (1000u)

#define CUST_PHY_DP83826B_BMCR_REG                          (0x00)
#define CUST_PHY_DP83826B_BMSR_REG                          (0x01)
#define CUST_PHY_DP83826B_ANAR_REG                          (0x04)
#define CUST_PHY_DP83826B_CR1_REG                           (0x09)
#define CUST_PHY_DP83826B_CR2_REG                           (0x0A)
#define CUST_PHY_DP83826B_PHYSTS_REG                        (0x10)
#define CUST_PHY_DP83826B_LEDCR_REG                         (0x18)
#define CUST_PHY_DP83826B_PHYCR_REG                         (0x19)
#define CUST_PHY_DP83826B_RCR_REG                           (0x1f)
#define CUST_PHY_DP83826B_MLED_REG                          (0x25)
#define CUST_PHY_DP83826B_LEDCFG_REG                        (0x460)
#define CUST_PHY_DP83826B_SOR2_REG                          (0x468)

#define CUST_PHY_DP83826B_BMCR_REG_RESET                    (0x8000)
#define CUST_PHY_DP83826B_BMCR_REG_MII_LOOPBACK             (0x4000)
#define CUST_PHY_DP83826B_BMCR_REG_SPEED_SEL_LSB            (0x2000)
#define CUST_PHY_DP83826B_BMCR_REG_ANEG_ENABLE              (0x1000)
#define CUST_PHY_DP83826B_BMCR_POWER_DOWN                   (0x0800)
#define CUST_PHY_DP83826B_BMCR_REG_ISOLATE                  (0x0400)
#define CUST_PHY_DP83826B_BMCR_REG_ANEG_RESTART             (0x0200)
#define CUST_PHY_DP83826B_BMCR_REG_DUPLEX_EN                (0x0100)
#define CUST_PHY_DP83826B_BMCR_REG_COL_TST                  (0x0080)
#define CUST_PHY_DP83826B_BMCR_REG_SPEED_SEL_MSB            (0x0040)

#define CUST_PHY_DP83826B_BMSR_REG_ANEG_COMPLETE            (0x0020)
#define CUST_PHY_DP83826B_BMSR_REG_LINK_STS                 (0x0004)

#define CUST_PHY_DP83826B_ANAR_100FD_SUPPORT                (0x0100)
#define CUST_PHY_DP83826B_ANAR_100HD_SUPPORT                (0x0080)
#define CUST_PHY_DP83826B_ANAR_10FD_SUPPORT                 (0x0040)
#define CUST_PHY_DP83826B_ANAR_10HD_SUPPORT                 (0x0020)

#define CUST_PHY_DP83826B_CR1_REG_FAST_RXDV                 (0x0002)

#define CUST_PHY_DP83826B_CR2_REG_ENA_EXTFD                 (0x0020)
#define CUST_PHY_DP83826B_CR2_REG_RX_ERR_IDLE               (0x0004)
#define CUST_PHY_DP83826B_CR2_REG_ODDN_ENA                  (0x0002)

#define CUST_PHY_DP83826B_PHYSTS_REG_SPEED                  (0x0002)
#define CUST_PHY_DP83826B_PHYSTS_REG_DUPLEX                 (0x0004)

#define CUST_PHY_DP83826B_PHYSTS_REG_SPEED_100M             (0x0000)
#define CUST_PHY_DP83826B_PHYSTS_REG_SPEED_10M              (0x0002)

#define CUST_PHY_DP83826B_LEDCR_LEDBLINK_50MS               (0x0)
#define CUST_PHY_DP83826B_LEDCR_LEDBLINK_100MS              (0x1)
#define CUST_PHY_DP83826B_LEDCR_LEDBLINK_200MS              (0x2)
#define CUST_PHY_DP83826B_LEDCR_LEDBLINK_500MS              (0x3)
#define CUST_PHY_DP83826B_LEDCR_LEDBLINK_MASK               (0x3)
#define CUST_PHY_DP83826B_LEDCR_LEDBLINK_SHIFT              (0x9)
#define CUST_PHY_DP83826B_LEDCR_LINKLEDPOLARITY_HIGH        (0x2)

#define CUST_PHY_DP83826B_PHYCR_REG_MDI_AUTO_CROSSOVER      (0x8000)
#define CUST_PHY_DP83826B_PHYCR_REG_MDI_MANUAL_CROSSOVER    (0x4000)

#define CUST_PHY_DP83826B_RCR_SW_RESET                      (0x8000)
#define CUST_PHY_DP83826B_RCR_SW_RESTART                    (0x4000)

#define CUST_PHY_DP83826B_MLED_REG_LINK_OK                  (0x0u)
#define CUST_PHY_DP83826B_MLED_REG_RX_TX_ACTIVITY           (0x1u)
#define CUST_PHY_DP83826B_MLED_REG_TX_ACTIVITY              (0x2u)
#define CUST_PHY_DP83826B_MLED_REG_RX_ACTIVITY              (0x3u)
#define CUST_PHY_DP83826B_MLED_REG_COLLISION                (0x4u)
#define CUST_PHY_DP83826B_MLED_REG_HIGHSPEED_100TX          (0x5u)
#define CUST_PHY_DP83826B_MLED_REG_HIGHSPEED_10TX           (0x6u)
#define CUST_PHY_DP83826B_MLED_REG_FDX                      (0x7u)
#define CUST_PHY_DP83826B_MLED_REG_LINK_OK_BLINK_ACT        (0x8u)
#define CUST_PHY_DP83826B_MLED_REG_ACTIVE_STRETCH_SIGNAL    (0x9u)
#define CUST_PHY_DP83826B_MLED_REG_MII_LINK                 (0xAu)
#define CUST_PHY_DP83826B_MLED_REG_LPI_MODE                 (0xBu)
#define CUST_PHY_DP83826B_MLED_REG_TX_RX_MII_ERROR          (0xCu)
#define CUST_PHY_DP83826B_MLED_REG_LINK_LOST                (0xDu)
#define CUST_PHY_DP83826B_MLED_REG_PRBS_ERROR               (0xEu)

#define CUST_PHY_DP83826B_LEDCFG_REG_LEDCTL_MASK            (0xFu)
#define CUST_PHY_DP83826B_LEDCFG_REG_LED3CTL_SHIFT          (0x4u)
#define CUST_PHY_DP83826B_LEDCFG_REG_LED2CTL_SHIFT          (0x8u)
#define CUST_PHY_DP83826B_LEDCFG_REG_LED1CTL_SHIFT          (0x12u)

#define CUST_PHY_DP83826B_SOR2_REG_CFG_FLD_ENA              (0x100u)

#define CUST_PHY_DP83826B_LINK_PRESENT_AUTONEG_POLL_STEP    (10u)
#define CUST_PHY_DP83826B_LINK_ABSENT_AUTONEG_POLL_STEP     (1u)

#define CUST_PHY_DP83826B_AUTONEG_REG_ADVERT                (0x04)

/* DP8 global */
#define CLEARREGBIT(reg, bitNum) \
    (reg) = ((reg) & ~(1 << (bitNum)))

#define SETREGBIT(reg, bitNum) \
    (reg) = ((reg) | (1 << (bitNum)))

#define SETREGVAL(reg, shift, mask, value) \
    (reg) = (((reg) & (~((mask) << (shift)))) | ((value) << (shift)))

static void     CUST_PHY_DP83826B_softwareReset             (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_softwareRestart           (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_enableAutoMDIX            (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_setMIIMode                (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_configMLED                (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_enableExtFD               (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_enableODDNibbleDet        (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_enableRxErrIdle           (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_configLed                 (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_configLedBlink            (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_enableFastLinkDownDet     (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_enableFastRXDVDet         (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_cofigSwStrapDone          (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_setPowerMode              (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p
                                                            ,bool       powerDown_p);
static bool     CUST_PHY_DP83826B_getPowerMode              (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_setLinkConfig             (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p
                                                            ,bool       autoNeg_p
                                                            ,uint16_t   linkSpeed_p
                                                            ,bool       fullDuplex_p
                                                            ,uint32_t*  pResult_p);
static bool     CUST_PHY_DP83826B_getAutoNegotiation        (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_setAutoNegotiation        (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_setMdixMode               (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p
                                                            ,uint32_t   mdixMode_p);
static uint32_t CUST_PHY_DP83826B_getMdixMode               (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_disable1GbAdver           (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_rgmiiLowLatencyEnable     (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);
static void     CUST_PHY_DP83826B_rgmiiTxHalfFullThreshold  (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p
                                                            ,uint32_t   threshold_p);
static void     CUST_PHY_DP83826B_rgmiiRxHalfFullThreshold  (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p
                                                            ,uint32_t   threshold_p);
static void     CUST_PHY_DP83826B_getSpeedDuplex            (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p
                                                            ,void*      pData_p
                                                            ,uint32_t   dataSize_p);
static uint32_t CUST_PHY_DP83826B_openFxn                   (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p
                                                            ,void*      pParam_p);
static uint32_t CUST_PHY_DP83826B_commandFxn                (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p
                                                            ,uint32_t   command_p
                                                            ,void*      pData_p
                                                            ,uint32_t   dataSize_p);
static void     CUST_PHY_DP83826B_closeFxn                  (void*      pAppCtxt_p
                                                            ,void*      pStackCtxt_p);

/*! <!-- Description: -->
 *
 *  \brief
 *  Detect Phy Type and setup access structures accordingly
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pPhyLibCtxt_p   Context of External PhyLib. (not used)
 *  \param[in]  phyId_p         Phy ID read from hardware
 *  \param[in]  pPhyLibDesc_p   External PhyLib Hooks
 *  \return     0 on success and Phy detected, error code otherwise
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
int16_t CUST_PHY_DP83826B_detect(void* pPhyLibCtxt_p, uint32_t phyId_p, CUST_PHY_SPhyDescriptor_t *pPhyLibDesc_p)
{
    int16_t     retVal  = -CUST_PHY_STATUS_ERROR_UNKNOWN_PHY;

    OSALUNREF_PARM(pPhyLibCtxt_p);

    /* exact match */
    if ((phyId_p & CUST_PHY_DP83826B_IEEE_IDENT_MASK) == CUST_PHY_DP83826B_IEEE_IDENT)
    {
        OSAL_printf("DP83826B detected\r\n");
        pPhyLibDesc_p->softwareReset                 = CUST_PHY_DP83826B_softwareReset;
        pPhyLibDesc_p->softwareRestart               = CUST_PHY_DP83826B_softwareRestart;
        pPhyLibDesc_p->enableAutoMDIX                = CUST_PHY_DP83826B_enableAutoMDIX;
        pPhyLibDesc_p->setMiiMode                    = CUST_PHY_DP83826B_setMIIMode;
        pPhyLibDesc_p->setPowerMode                  = CUST_PHY_DP83826B_setPowerMode;
        pPhyLibDesc_p->getPowerMode                  = CUST_PHY_DP83826B_getPowerMode;
        pPhyLibDesc_p->configMLED                    = CUST_PHY_DP83826B_configMLED;
        pPhyLibDesc_p->enableExtFD                   = CUST_PHY_DP83826B_enableExtFD;
        pPhyLibDesc_p->enableODDNibbleDet            = CUST_PHY_DP83826B_enableODDNibbleDet;
        pPhyLibDesc_p->enableRxErrIdle               = CUST_PHY_DP83826B_enableRxErrIdle;
        pPhyLibDesc_p->configLed                     = CUST_PHY_DP83826B_configLed;
        pPhyLibDesc_p->configLedBlink                = CUST_PHY_DP83826B_configLedBlink;
        pPhyLibDesc_p->enableFastLinkDownDet         = CUST_PHY_DP83826B_enableFastLinkDownDet;
        pPhyLibDesc_p->enableFastRXDVDet             = CUST_PHY_DP83826B_enableFastRXDVDet;
        pPhyLibDesc_p->configSwStrapDone             = CUST_PHY_DP83826B_cofigSwStrapDone;
        pPhyLibDesc_p->setLinkConfig                 = CUST_PHY_DP83826B_setLinkConfig;
        pPhyLibDesc_p->getAutoNegotiation            = CUST_PHY_DP83826B_getAutoNegotiation;
        pPhyLibDesc_p->setAutoNegotiation            = CUST_PHY_DP83826B_setAutoNegotiation;
        pPhyLibDesc_p->setMdixMode                   = CUST_PHY_DP83826B_setMdixMode;
        pPhyLibDesc_p->getMdixMode                   = CUST_PHY_DP83826B_getMdixMode;
        pPhyLibDesc_p->disable1GbAdver               = CUST_PHY_DP83826B_disable1GbAdver;
        pPhyLibDesc_p->rgmiiLowLatencyEnable         = CUST_PHY_DP83826B_rgmiiLowLatencyEnable;
        pPhyLibDesc_p->rgmiiTxHalfFullThreshold      = CUST_PHY_DP83826B_rgmiiTxHalfFullThreshold;
        pPhyLibDesc_p->rgmiiRxHalfFullThreshold      = CUST_PHY_DP83826B_rgmiiRxHalfFullThreshold;
        pPhyLibDesc_p->getSpeedDuplex                = CUST_PHY_DP83826B_getSpeedDuplex;
        pPhyLibDesc_p->openFxn                       = CUST_PHY_DP83826B_openFxn;
        pPhyLibDesc_p->commandFxn                    = CUST_PHY_DP83826B_commandFxn;
        pPhyLibDesc_p->closeFxn                      = CUST_PHY_DP83826B_closeFxn;

        retVal = 0;
    }

    return retVal;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Issue PHY Reset by software (used if no Hard Reset is available)
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p      application context (not used)
 *  \param[in]  pStackCtxt_p    stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_softwareReset(void* pAppCtxt_p, void* pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_RCR_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_RCR_SW_RESET;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_RCR_REG, phyRegVal);

    do
    {
        phyRegVal = 0;
        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_RCR_REG, &phyRegVal);
    }
    while (phyRegVal & CUST_PHY_DP83826B_RCR_SW_RESET);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Issue PHY Restart by software (register left intact)
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p      application context (not used)
 *  \param[in]  pStackCtxt_p    stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_softwareRestart(void* pAppCtxt_p, void* pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;
    bool        hadLink     = false;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMSR_REG, &phyRegVal);
    if ( phyRegVal & CUST_PHY_DP83826B_BMSR_REG_LINK_STS)
    {
        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMSR_REG, &phyRegVal);
        if ( phyRegVal & CUST_PHY_DP83826B_BMSR_REG_LINK_STS)
        {
            OSAL_printf("0x%x:Link up\r\n", CUST_PHY_getPhyAddr(pStackCtxt_p));
            hadLink = true;
        }
    }

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_BMCR_REG_ANEG_ENABLE;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, phyRegVal);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_RCR_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_RCR_SW_RESTART;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_RCR_REG, phyRegVal);

    do
    {
        phyRegVal = 0;
        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_RCR_REG, &phyRegVal);
    }
    while (phyRegVal & CUST_PHY_DP83826B_RCR_SW_RESTART);
    OSAL_SCHED_sleep(10); /* just for settle */

    if (hadLink)
    {
        /* we expect to gain link agn */
        do
        {
            CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMSR_REG, &phyRegVal);
            if ( !(phyRegVal & CUST_PHY_DP83826B_BMSR_REG_LINK_STS))
            {
                OSAL_SCHED_sleep(100); /* if we got no re-link, waiting is not too bad at all */
            }
        } while (!(phyRegVal & CUST_PHY_DP83826B_BMSR_REG_LINK_STS));
    }
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Enable Auto MDIX
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_enableAutoMDIX(void* pAppCtxt_p, void* pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;
    uint32_t    timeoutStep = 10;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMSR_REG, &phyRegVal);
    if ( phyRegVal & CUST_PHY_DP83826B_BMSR_REG_LINK_STS)
    {
        timeoutStep = CUST_PHY_DP83826B_LINK_PRESENT_AUTONEG_POLL_STEP;
    }
    else
    {
        timeoutStep = CUST_PHY_DP83826B_LINK_ABSENT_AUTONEG_POLL_STEP;
    }

    OSAL_printf("Phy %lu : Enable AutoMDIX\r\n", CUST_PHY_getPhyAddr(pStackCtxt_p));
    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_BMCR_REG_ANEG_ENABLE;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, phyRegVal);

    OSAL_printf("Phy %lu : Restart ANEG\r\n", CUST_PHY_getPhyAddr(pStackCtxt_p));
    phyRegVal |= CUST_PHY_DP83826B_BMCR_REG_ANEG_RESTART;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, phyRegVal);

    do
    {
        phyRegVal = 0;
        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);
    }
    while (phyRegVal & CUST_PHY_DP83826B_BMCR_REG_ANEG_RESTART);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMSR_REG, &phyRegVal);
    {
        uint16_t    timeout     = CUST_PHY_DP83826B_AUTONEGTIMEOUT;

        do
        {
            phyRegVal = 0;
            --timeout;
            CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMSR_REG, &phyRegVal);
            if (((phyRegVal & CUST_PHY_DP83826B_BMSR_REG_ANEG_COMPLETE) != CUST_PHY_DP83826B_BMSR_REG_ANEG_COMPLETE))
            {
                OSAL_SCHED_sleep(timeoutStep); /* delay for 10 msecs steps */
            }
        }
        while (timeout && ((phyRegVal & CUST_PHY_DP83826B_BMSR_REG_ANEG_COMPLETE) != CUST_PHY_DP83826B_BMSR_REG_ANEG_COMPLETE));
    }
    OSAL_printf("Phy %lu: BMSR post ANEG: %x ANEG:%s Link:%s\r\n", CUST_PHY_getPhyAddr(pStackCtxt_p), phyRegVal,
                ((phyRegVal&CUST_PHY_DP83826B_BMSR_REG_ANEG_COMPLETE)==CUST_PHY_DP83826B_BMSR_REG_ANEG_COMPLETE)?"Complete":"NComplete",
                ((phyRegVal & CUST_PHY_DP83826B_BMSR_REG_LINK_STS)==CUST_PHY_DP83826B_BMSR_REG_LINK_STS)?"Yes":"No");
    return;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Set MII Mode
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context (not used)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_setMIIMode(void *pAppCtxt_p, void *pStackCtxt_p)
{
    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(pStackCtxt_p);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Configure PhyMLED to detect RxLink by MLED (e.g. TLK)
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_configMLED(void *pAppCtxt_p, void *pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);
    /* LED 0 */
    phyRegVal = CUST_PHY_DP83826B_MLED_REG_LINK_LOST;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_MLED_REG, phyRegVal);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_LEDCFG_REG, &phyRegVal);

    /* LED 1 */
    SETREGVAL(phyRegVal, CUST_PHY_DP83826B_LEDCFG_REG_LED1CTL_SHIFT,
                                                       CUST_PHY_DP83826B_LEDCFG_REG_LEDCTL_MASK,
                                                       CUST_PHY_DP83826B_MLED_REG_LINK_OK);

    /* LED 2 */
    SETREGVAL(phyRegVal, CUST_PHY_DP83826B_LEDCFG_REG_LED1CTL_SHIFT,
                                                       CUST_PHY_DP83826B_LEDCFG_REG_LEDCTL_MASK,
                                                       CUST_PHY_DP83826B_MLED_REG_LINK_OK_BLINK_ACT);

    /* LED 3 */
    SETREGVAL(phyRegVal, CUST_PHY_DP83826B_LEDCFG_REG_LED1CTL_SHIFT,
                                                       CUST_PHY_DP83826B_LEDCFG_REG_LEDCTL_MASK,
                                                       CUST_PHY_DP83826B_MLED_REG_MII_LINK);

    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_LEDCFG_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Set Ext Full Duplex enable
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_enableExtFD(void *pAppCtxt_p, void *pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_CR2_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_CR2_REG_ENA_EXTFD;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_CR2_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Set ODD Nibble detection enable
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_enableODDNibbleDet(void *pAppCtxt_p, void *pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_CR2_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_CR2_REG_ODDN_ENA;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_CR2_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Set Rx Error Idle enable
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_enableRxErrIdle(void *pAppCtxt_p, void *pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_CR2_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_CR2_REG_RX_ERR_IDLE;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_CR2_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Configure PHY LEDs
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_configLed(void *pAppCtxt_p, void *pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_LEDCR_REG, &phyRegVal);

    phyRegVal |= CUST_PHY_DP83826B_LEDCR_LINKLEDPOLARITY_HIGH;

    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_LEDCR_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Configure PHY Blink LED mode
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_configLedBlink(void *pAppCtxt_p, void *pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;
    uint16_t    value       = CUST_PHY_DP83826B_LED_BLINK_50;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_LEDCR_REG, &phyRegVal);

    switch (value)
    {
    case CUST_PHY_DP83826B_LED_BLINK_500: SETREGVAL(phyRegVal, CUST_PHY_DP83826B_LEDCR_LEDBLINK_SHIFT,
                                                   CUST_PHY_DP83826B_LEDCR_LEDBLINK_MASK,
                                                   CUST_PHY_DP83826B_LEDCR_LEDBLINK_500MS); break;
    case CUST_PHY_DP83826B_LED_BLINK_200: SETREGVAL(phyRegVal, CUST_PHY_DP83826B_LEDCR_LEDBLINK_SHIFT,
                                                   CUST_PHY_DP83826B_LEDCR_LEDBLINK_MASK,
                                                   CUST_PHY_DP83826B_LEDCR_LEDBLINK_200MS); break;
    case CUST_PHY_DP83826B_LED_BLINK_100: SETREGVAL(phyRegVal, CUST_PHY_DP83826B_LEDCR_LEDBLINK_SHIFT,
                                                   CUST_PHY_DP83826B_LEDCR_LEDBLINK_MASK,
                                                   CUST_PHY_DP83826B_LEDCR_LEDBLINK_100MS); break;
    case CUST_PHY_DP83826B_LED_BLINK_50:  SETREGVAL(phyRegVal, CUST_PHY_DP83826B_LEDCR_LEDBLINK_SHIFT,
                                                   CUST_PHY_DP83826B_LEDCR_LEDBLINK_MASK,
                                                   CUST_PHY_DP83826B_LEDCR_LEDBLINK_50MS); break;
    }

    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_LEDCR_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Set fast link down Detection enable
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_enableFastLinkDownDet(void *pAppCtxt_p, void *pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_SOR2_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_SOR2_REG_CFG_FLD_ENA;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_SOR2_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Set Fast RX DV detection enable
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_enableFastRXDVDet(void *pAppCtxt_p, void *pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_CR1_REG, &phyRegVal);
    phyRegVal |= CUST_PHY_DP83826B_CR1_REG_FAST_RXDV;
    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_CR1_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Set SW Strap config done
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p         application context (not used)
 *  \param[in]  pStackCtxt_p       stack context (not used)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_cofigSwStrapDone(void *pAppCtxt_p, void *pStackCtxt_p)
{
    /* no SW strap on DP 83869 */
    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(pStackCtxt_p);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Set power mode
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p      application context (not used)
 *  \param[in]  pStackCtxt_p    stack context
 *  \param[in]  powerDown_p     False = Normal Operation - True = Power Down
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_setPowerMode(void* pAppCtxt_p, void *pStackCtxt_p, bool powerDown_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    if(powerDown_p)
    {
        //Power Down mode
        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);
        phyRegVal |= CUST_PHY_DP83826B_BMCR_POWER_DOWN;
        CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, phyRegVal);

        do
        {
            phyRegVal = 0;
            CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);
        }
        while (!(phyRegVal & CUST_PHY_DP83826B_BMCR_POWER_DOWN));
    }
    else
    {
        //Normal Operation
        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);
        phyRegVal &= ~CUST_PHY_DP83826B_BMCR_POWER_DOWN;
        CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, phyRegVal);

        do
        {
            phyRegVal = 0;
            CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);
        }
        while (phyRegVal & CUST_PHY_DP83826B_BMCR_POWER_DOWN);
    }
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Get power down
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p      application context (not used)
 *  \param[in]  pStackCtxt_p    stack context
 *
 *  \return False = Normal Operation - True = Power Down
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
bool CUST_PHY_DP83826B_getPowerMode(void* pAppCtxt_p, void* pStackCtxt_p)
{
    uint16_t    phyRegVal   = 0;
    bool        powerDown   = false;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);

    if ((phyRegVal & CUST_PHY_DP83826B_BMCR_POWER_DOWN) == CUST_PHY_DP83826B_BMCR_POWER_DOWN)
    {
        powerDown = true;
    }

    return powerDown;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Configures PHY for link connection.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context
 *  \param[in]  pStackCtxt_p        stack context
 *  \param[in]  autoNeg_p           selected auto-negotiation mode (false - auto-negotiation inactive, true - auto-negotiation active)
 *  \param[in]  linkSpeed_p         required link speed (10, 100 or 1000 Mbps)
 *  \param[in]  fullDuplex_p        required duplex mode (false - half-duplex, true - full duplex)
 *  \param[out] pResult_p           pointer to variable where result of configuration needs to be stored (0 - Success, other - Error)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_setLinkConfig(void* pAppCtxt_p, void* pStackCtxt_p, bool autoNeg_p, uint16_t linkSpeed_p, bool fullDuplex_p, uint32_t* pResult_p)
{
    uint16_t    phyRegVal   = 0;
    uint32_t    error       = 0;

    CUST_PHY_DP83826B_setPowerMode(pAppCtxt_p, pStackCtxt_p, true);

    if (autoNeg_p == true)
    {
        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_ANAR_REG, &phyRegVal);

        phyRegVal |= CUST_PHY_DP83826B_ANAR_100FD_SUPPORT;
        phyRegVal |= CUST_PHY_DP83826B_ANAR_100HD_SUPPORT;
        phyRegVal |= CUST_PHY_DP83826B_ANAR_10FD_SUPPORT;
        phyRegVal |= CUST_PHY_DP83826B_ANAR_10HD_SUPPORT;

        CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_ANAR_REG, phyRegVal);

        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);

        phyRegVal |=  CUST_PHY_DP83826B_BMCR_REG_ANEG_ENABLE |
                      CUST_PHY_DP83826B_BMCR_REG_ANEG_RESTART;

        CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, phyRegVal);
    }
    else
    {
        CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);

        phyRegVal &= ~CUST_PHY_DP83826B_BMCR_REG_ANEG_ENABLE;
        phyRegVal &= ~CUST_PHY_DP83826B_BMCR_REG_SPEED_SEL_LSB;
        phyRegVal &= ~CUST_PHY_DP83826B_BMCR_REG_DUPLEX_EN;
        phyRegVal &= ~CUST_PHY_DP83826B_BMCR_REG_SPEED_SEL_MSB;

        switch(linkSpeed_p)
        {
            case 10:
                if (fullDuplex_p == true)
                {
                    phyRegVal |= CUST_PHY_DP83826B_BMCR_REG_DUPLEX_EN;
                }
                break;
            case 100:
                phyRegVal |= CUST_PHY_DP83826B_BMCR_REG_SPEED_SEL_LSB;

                // duplex mode
                if (fullDuplex_p == true)
                {
                    phyRegVal |= CUST_PHY_DP83826B_BMCR_REG_DUPLEX_EN;
                }
                break;
            default:
                // 1000 Mbps not supported (PRU-CODE limitation)
                error = ~0;
                break;
        }

        if (error == 0)
        {
            CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, phyRegVal);
        }
    }

    OSAL_waitTimerUs(10);

    CUST_PHY_DP83826B_setPowerMode(pAppCtxt_p, pStackCtxt_p, false);

    if (pResult_p != NULL)
    {
        *pResult_p = error;
    }
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Provides auto-negotiation enabled flag.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context
 *  \return     bool                auto-negotiation enabled flag (false - auto-negotiation disabled, true - auto-negotiation enabled)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
bool CUST_PHY_DP83826B_getAutoNegotiation(void* pAppCtxt_p, void* pStackCtxt_p)
{
    uint16_t    phyRegVal = 0;
    bool        autoNeg     = false;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_BMCR_REG, &phyRegVal);

    phyRegVal = phyRegVal & CUST_PHY_DP83826B_BMCR_REG_ANEG_ENABLE;

    if (phyRegVal != 0)
    {
        autoNeg = true;
    }

    return autoNeg;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Sets auto-negotiation flags manually.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_setAutoNegotiation(void* pAppCtxt_p, void* pStackCtxt_p)
{
    uint16_t    phyRegVal = 0;
    uint32_t    status = CUST_PHY_eSTATUS_FAIL;

    OSALUNREF_PARM(pAppCtxt_p);

    status = CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_AUTONEG_REG_ADVERT, &phyRegVal);

    if (status == CUST_PHY_eSTATUS_SUCCESS)
    {
        CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_AUTONEG_REG_ADVERT, phyRegVal | 1<<6 | 1<<8);
    }
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Configures PHY MDI crossover mode.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context
 *  \param[in]  mdixMode_p          required MDI crossover mode (0 - Manual MDI configuration, 1 - Manual MDI-X configuration, 2 - Enable automatic crossover)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_setMdixMode(void* pAppCtxt_p, void* pStackCtxt_p, uint32_t mdixMode_p)
{
    uint16_t    phyRegVal   = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_PHYCR_REG, &phyRegVal);

    phyRegVal &= ~CUST_PHY_DP83826B_PHYCR_REG_MDI_MANUAL_CROSSOVER;
    phyRegVal &= ~CUST_PHY_DP83826B_PHYCR_REG_MDI_AUTO_CROSSOVER;

    switch(mdixMode_p)
    {
        case CUST_PHY_MDI_MANUAL_CONFIG:
            break;
        case CUST_PHY_MDIX_MANUAL_CONFIG:
            phyRegVal |= CUST_PHY_DP83826B_PHYCR_REG_MDI_MANUAL_CROSSOVER;
            break;
        default:
            phyRegVal |= CUST_PHY_DP83826B_PHYCR_REG_MDI_AUTO_CROSSOVER;
            break;
    }

    CUST_PHY_writeReg(pStackCtxt_p, CUST_PHY_DP83826B_PHYCR_REG, phyRegVal);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Provides actually used PHY MDI crossover mode.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context
 *  \return     uint32_t            value of used MDI crossover mode (0 - Manual MDI configuration, 1 - Manual MDI-X configuration, 2 - Enable automatic crossover)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
uint32_t CUST_PHY_DP83826B_getMdixMode(void* pAppCtxt_p, void* pStackCtxt_p)
{
    uint16_t   phyRegVal   = 0;
    uint32_t   mdixMode    = 0;

    OSALUNREF_PARM(pAppCtxt_p);

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_PHYCR_REG, &phyRegVal);

    phyRegVal = phyRegVal & (CUST_PHY_DP83826B_PHYCR_REG_MDI_MANUAL_CROSSOVER | CUST_PHY_DP83826B_PHYCR_REG_MDI_AUTO_CROSSOVER);
    phyRegVal = phyRegVal >> 12;

    switch(phyRegVal)
    {
        case 0:
            mdixMode = CUST_PHY_MDI_MANUAL_CONFIG;
            break;
        case 1:
            mdixMode = CUST_PHY_MDIX_MANUAL_CONFIG;
            break;
        default:
            mdixMode = CUST_PHY_MDIX_AUTO_CROSSOVER;
            break;
    }

    return mdixMode;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Disables 1Gbit Advertisment during autonegotiation.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context (not used)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_disable1GbAdver(void* pAppCtxt_p, void* pStackCtxt_p)
{
    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(pStackCtxt_p);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Enables low latency in RGMII mode
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context (not used)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_rgmiiLowLatencyEnable(void* pAppCtxt_p, void* pStackCtxt_p)
{
    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(pStackCtxt_p);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  RGMII_TX_HALF_FULL_THR[1:0] can be changed from the default of 0x2 to 0x1.
 *    This will reduce the latency by one clock period.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context (not used)
 *  \param[in]  threshold_p         Number of cycles for threshold (2, 1) (not used)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_rgmiiTxHalfFullThreshold(void* pAppCtxt_p, void* pStackCtxt_p, uint32_t threshold_p)
{
    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(pStackCtxt_p);
    OSALUNREF_PARM(threshold_p);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  RGMII_RX_HALF_FULL_THR[1:0] can be changed from the default of 0x2 to 0x1.
 *    This will reduce the latency by one clock period.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context (not used)
 *  \param[in]  threshold_p         Number of cycles for threshold (2, 1) (not used)
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_rgmiiRxHalfFullThreshold(void* pAppCtxt_p, void* pStackCtxt_p, uint32_t threshold_p)
{
    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(pStackCtxt_p);
    OSALUNREF_PARM(threshold_p);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Provides actually used link speed and duplex mode.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p          application context (not used)
 *  \param[in]  pStackCtxt_p        stack context
 *  \param[in]  pData_p             pointer to data structure which contains link speed and duplex mode values
 *  \param[in]  dataSize_p          size of data structure
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
void CUST_PHY_DP83826B_getSpeedDuplex (void* pAppCtxt_p, void* pStackCtxt_p, void* pData_p, uint32_t dataSize_p)
{
    CUST_PHY_SSpeedDuplexConfig_t *pSpeedDuplexCfg = NULL;

    uint16_t  phyRegVal    = 0;

    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(dataSize_p);

    pSpeedDuplexCfg = (CUST_PHY_SSpeedDuplexConfig_t*) pData_p;

    if (pSpeedDuplexCfg == NULL)
    {
        return;
    }

    CUST_PHY_readReg(pStackCtxt_p, CUST_PHY_DP83826B_PHYSTS_REG, &phyRegVal);

    switch(phyRegVal & CUST_PHY_DP83826B_PHYSTS_REG_SPEED)
    {
        case CUST_PHY_DP83826B_PHYSTS_REG_SPEED_100M:
            if (phyRegVal & CUST_PHY_DP83826B_PHYSTS_REG_DUPLEX)
            {
                pSpeedDuplexCfg->config = CUST_PHY_SPEED_DUPLEX_eCONFIG_100FD;
            }
            else
            {
                pSpeedDuplexCfg->config = CUST_PHY_SPEED_DUPLEX_eCONFIG_100HD;
            }
            break;
        case CUST_PHY_DP83826B_PHYSTS_REG_SPEED_10M:
            if (phyRegVal & CUST_PHY_DP83826B_PHYSTS_REG_DUPLEX)
            {
                pSpeedDuplexCfg->config = CUST_PHY_SPEED_DUPLEX_eCONFIG_10FD;
            }
            else
            {
                pSpeedDuplexCfg->config = CUST_PHY_SPEED_DUPLEX_eCONFIG_10HD;
            }
            break;
        default:
            break;
    }
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Open PHY interface function to be able connect to TI ETHPHY interface.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p                   application context (not used)
 *  \param[in]  pStackCtxt_p                 stack context
 *  \param[in]  pParam_p                     parameters (not used)
 *  \return     uint32_t                     status
 *  \retval     CUST_PHY_eSTATUS_FAIL        function failed
 *  \retval     CUST_PHY_eSTATUS_SUCCESS     function success
 *
 *  <!-- Group: -->
 *
 *  \ingroup CUST_PHY
 *
 * */
uint32_t CUST_PHY_DP83826B_openFxn (void* pAppCtxt_p, void* pStackCtxt_p, void* pParam_p)
{
    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(pParam_p);

    return CUST_PHY_open(pStackCtxt_p);
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Command PHY interface function to be able connect to TI ETHPHY interface.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p                   application context
 *  \param[in]  pStackCtxt_p                 stack context
 *  \param[in]  command_p                    command id
 *  \param[in]  pData_p                      data
 *  \param[in]  dataSize_p                   data size
 *  \return     uint32_t                     status
 *  \retval     CUST_PHY_eSTATUS_FAIL        function failed
 *  \retval     CUST_PHY_eSTATUS_SUCCESS     function success
 *
 *  <!-- Group: -->
 *
 *  \ingroup PRU
 *
 * */
uint32_t CUST_PHY_DP83826B_commandFxn (void* pAppCtxt_p, void* pStackCtxt_p, uint32_t command_p, void* pData_p, uint32_t dataSize_p)
{
    int32_t status = CUST_PHY_eSTATUS_SUCCESS;

    switch(command_p)
    {
        case CUST_PHY_eCOMMAND_GET_SPEED_AND_DUPLEX_CONFIG:
            CUST_PHY_DP83826B_getSpeedDuplex(pAppCtxt_p, pStackCtxt_p, pData_p, dataSize_p);
            break;
        default:
            status = CUST_PHY_eSTATUS_FAIL;
            break;
    }

    return status;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Close PHY interface function to be able connect to TI ETHPHY interface.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppCtxt_p                  application context (not used)
 *  \param[in]  pStackCtxt_p                stack context (not used)
 *
 *  <!-- Group: -->
 *
 *  \ingroup PRU
 *
 * */
void CUST_PHY_DP83826B_closeFxn (void* pAppCtxt_p, void* pStackCtxt_p)
{
    OSALUNREF_PARM(pAppCtxt_p);
    OSALUNREF_PARM(pStackCtxt_p);

    return;
}

//*************************************************************************************************
