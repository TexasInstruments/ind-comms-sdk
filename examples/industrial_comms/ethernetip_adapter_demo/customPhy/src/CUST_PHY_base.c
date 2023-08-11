/*!
 *  \file CUST_PHY_base.c
 *
 *  \brief
 *  Custom PHY implementation.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
 *  @KUNBUS_LICENSE@
 *
 */

#include <CUST_PHY_base.h>
#include <CUST_PHY_dp83869.h>

static CUST_PHY_CBextPhyLibDetect_t CUST_PHY_knownImplementation_s [] =
{
    CUST_PHY_DP83869_detect,
    NULL
};

/*! <!-- Description: -->
 *
 *  \brief
 *  Detect Phy Type and setup access structures accordingly
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pPhyLibCtxt_p	Context of External PhyLib.
 *  \param[in]  phyId_p         Phy ID read from hardware
 *  \param[in]  pPhyLibDesc_p   External PhyLib Hooks
 *  \return     0 on success and Phy detected, error code otherwise
 *
 *  <!-- Group: -->
 *
 *  \ingroup APPPHYLIB
 *
 * */
int16_t CUST_PHY_detect(void* pPhyLibCtxt_p, uint32_t phyId_p, CUST_PHY_SPhyDescriptor_t *pPhyLibDesc_p)
{
    int16_t                         retVal      = -CUST_PHY_STATUS_ERROR_UNKNOWN_PHY;

    CUST_PHY_CBextPhyLibDetect_t*   iterator    = CUST_PHY_knownImplementation_s;

    while ((NULL != iterator) && (NULL != iterator[0]))
    {
        retVal = iterator[0](pPhyLibCtxt_p, phyId_p, pPhyLibDesc_p);

        if (0 == retVal)
        {
            break;
        }
        (++iterator);
    }

    return retVal;
}

//*************************************************************************************************
