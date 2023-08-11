/*!
 *  \file CUST_PHY_dp83869.h
 *
 *  \brief
 *  PHY interface for TI DP83869.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
 *  @KUNBUS_LICENSE@
 *
 */

#if !(defined PROTECT_CUST_PHY_DP83869_H)
#define PROTECT_CUST_PHY_DP83869_H		1

#include <osal.h>
#include <custom_phy.h>

/*PHY LED Modes*/
#define CUST_PHY_DP83869_LED_CFG_MODE0          0
#define CUST_PHY_DP83869_LED_CFG_MODE1          1
#define CUST_PHY_DP83869_LED_CFG_MODE2          2
#define CUST_PHY_DP83869_LED_CFG_MODE3          3

/*PHY LED BLINK Rates*/
#define CUST_PHY_DP83869_LED_BLINK_500           500
#define CUST_PHY_DP83869_LED_BLINK_200           200
#define CUST_PHY_DP83869_LED_BLINK_100           100
#define CUST_PHY_DP83869_LED_BLINK_50            50

#define CUST_PHY_DP83869_FAST_LINKDOWN_SIGENERGY 1u
#define CUST_PHY_DP83869_FAST_LINKDOWN_RXERR     8u

#if (defined __cplusplus)
extern "C" {
#endif

extern int16_t  CUST_PHY_DP83869_detect         (void*                          pPhyLibCtxt_p
                                                , uint32_t                       phyId_p
                                                , CUST_PHY_SPhyDescriptor_t*     pPhyLibDesc_p);

#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_CUST_PHY_DP83869_H */
