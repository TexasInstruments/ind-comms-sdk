/*
 * Copyright (C) 2023 Texas Instruments Incorporated
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "emac_lwipif.h"
//#include "enet_apputils.h"
#include "default_netif.h"
#include "lwip2icss_emac.h"
#include "lwip2lwipif.h"
#include "hsr_prp_lwipcfg.h"


// static void hsrprp_LwipStatus_callback(struct netif *state_netif);
// static void hsrprp_LwipLink_callback(struct netif *state_netif);

int32_t app_getEmacHandle(Lwip2Emac_Handle hLwip2Emac);

/** \brief ICSSEmac LLD handle */
ICSS_EMAC_Handle emachandle;

struct netif gEmacNetif;

struct netif* hsrprp_pass_netif(void)
{
    return (&gEmacNetif);
}

//void hsrprp_LwipTest_netif_init(void)
//{
//    sys_sem_t pInitSem;
//    const err_t err = sys_sem_new(&pInitSem, 0);
//    DebugP_assert(err == ERR_OK);
//

//    tcpip_init(hsrprp_LwipTest_netif_addCb, &pInitSem);
//
//    /* wait for TCP/IP initialization to complete */
//    sys_sem_wait(&pInitSem);
//    sys_sem_free(&pInitSem);
//}

void hsrprp_LwipStatus_callback(struct netif *state_netif) // FIX ME - make static later
{
    if (netif_is_up(state_netif))
    {
#if LWIP_IPV4
        DebugP_log("status_callback==UP, local interface IP is %s\r\n", ip4addr_ntoa(netif_ip4_addr(state_netif)));
#endif
    }
    else
    {
        DebugP_log("status_callback==DOWN\r\n");
    }
}

void hsrprp_LwipLink_callback(struct netif *state_netif) // FIX ME - make static later
{
    if (netif_is_link_up(state_netif))
    {
        DebugP_log("link_callback==UP\r\n");
    }
    else
    {
        DebugP_log("link_callback==DOWN\r\n");
    }
}

int32_t app_getEmacHandle(Lwip2Emac_Handle hLwip2Emac)
{
    int32_t ret_val = SystemP_FAILURE;
    if(hLwip2Emac != NULL)
    {
        hLwip2Emac->emacHandle = emachandle;
        ret_val = SystemP_SUCCESS;
    }

    return (ret_val);
}
