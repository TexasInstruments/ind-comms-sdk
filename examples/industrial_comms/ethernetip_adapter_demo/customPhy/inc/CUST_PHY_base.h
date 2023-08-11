/*!
 *  \file CUST_PHY_base.h
 *
 *  \brief
 *  Custom PHY interface.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
 *  @KUNBUS_LICENSE@
 *
 */

#if !(defined PROTECT_CUST_PHY_BASE_H)
#define PROTECT_CUST_PHY_BASE_H		1

#include <osal.h>
#include <custom_phy.h>

#if (defined __cplusplus)
extern "C" {
#endif

extern int16_t  CUST_PHY_detect                 (void*                          pPhyLibCtxt_p
                                                ,uint32_t                       phyId_p
                                                ,CUST_PHY_SPhyDescriptor_t*     pPhyLibDesc_p);
#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_CUST_PHY_BASE_H */
