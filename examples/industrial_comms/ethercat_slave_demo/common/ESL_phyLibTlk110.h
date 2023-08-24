/*
 *  Copyright (c) 2020, Kunbus GmbH
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  3. Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
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

#if !(defined __ESL_PHYLIBTLK110_H__)
#define __ESL_PHYLIBTLK110_H__		1

#include <osal.h>
#include <ecSlvApi.h>
#include <custom_phy.h>

#if (defined __cplusplus)
extern "C" {
#endif

extern int16_t  EC_SLV_APP_TLK110_phyLibDetect          (void*                          pPhyLibCtxt_p
                                                        ,uint32_t                       phyId_p
                                                        ,CUST_PHY_SPhyDescriptor_t*     pPhyLibDesc_p);
extern void     EC_SLV_APP_TLK110_softwareReset         (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_enableAutoMDIX     (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_setMIIMode            (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_configMLED            (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_enableExtFD           (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_enableODDNibbleDet    (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_enableRxErrIdle       (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_configLed             (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_configLedBlink        (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_enableFastLinkDownDet (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_enableFastRXDVDet     (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_configSwStrapDone     (void*                          pAppCtxt_p
                                                        ,void*                          pStackCtxt_p);
extern void     EC_SLV_APP_TLK110_setPowerMode             (void*                          pCtxt_p
                                                        ,void*                          pStackCtxt_p
                                                        ,bool                           powerDown_p);

#if (defined __cplusplus)
}
#endif

#endif /* __ESL_PHYLIBTLK110_H__ */
