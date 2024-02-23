/*
 *  Copyright (c) Texas Instruments Incorporated 2024
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
 * \file  enet_netific.c
 *
 * \brief This file contains the implementation of the Enet Remote Connect example.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "netif/bridgeif.h"

// #include <enet.h>
// #include <networking/enet/utils/include/enet_appmemutils_cfg.h>
// #include <networking/enet/utils/include/enet_apputils.h>
// #include <networking/enet/utils/include/enet_appmemutils.h>
// #include <networking/enet/utils/include/enet_appboardutils.h>
// #include <networking/enet/utils/include/enet_appsoc.h>
// #include <networking/enet/utils/include/enet_apprm.h>
// #include <networking/enet/core/lwipif/inc/pbufQ.h>
// #include <networking/enet/core/include/per/icssg.h>

#include <lwipific/inc/lwip_ic.h>
#include <lwipific/inc/lwip2lwipif_ic.h>

#include <lwip2lwipif.h>
// #include <custom_pbuf.h>
// #include "ti_enet_config.h"
// #include "ti_enet_lwipif.h"
#include "enet_netific.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Handle to the Application interface for the LwIPIf Layer
 */
//LwipifEnetApp_Handle hlwipIfApp = NULL;
//struct netif netif_bridge;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void EthApp_netifStatusCb(struct netif *netif);
void EthApp_setNetifCbs(struct netif *netif);
static void App_netifLinkChangeCb(struct netif *pNetif);

static void EthApp_initLwip(void *arg);
void EthApp_initNetif(void);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static void EthApp_netifStatusCb(struct netif *netif)
{

    LWIP_ASSERT("Invalid Input Netif for Callback", netif != NULL);
    if (netif_is_up(netif))
    {
        const ip4_addr_t *ipAddr = netif_ip4_addr(netif);

        DebugP_log("Added interface '%c%c%d', IP is %s \r\n",
                     netif->name[0], netif->name[1], netif->num, ip4addr_ntoa(ipAddr));

        if (ipAddr->addr != 0)
        {
#if defined(ETHFW_BOOT_TIME_PROFILING)
            /* Timestamp when EthFw got an IP */
            EthApp_setBootTs(ETHFW_BOOT_PROFILING_TS_TCPIP);
#endif

#if defined(ETHFW_DEMO_SUPPORT)
            int32_t status;
            /* Assign functions that are to be called based on actions in GUI.
             * These cannot be dynamically pushed to function pointer array, as the
             * index is used in GUI as command */
            EnetCfgServer_fxn_table[9] = &EthApp_startSwInterVlan;
            EnetCfgServer_fxn_table[10] = &EthApp_startHwInterVlan;

            /* Start Configuration server */
            status = EnetCfgServer_init(gEthAppObj.enetType, gEthAppObj.instId);
            DebugP_assert(ENET_SOK == status);

            /* Start the software-based interVLAN routing */
            EthSwInterVlan_setupRouting(gEthAppObj.enetType, ETH_SWINTERVLAN_TASK_PRI);
#endif
        }
    }
    else
    {
        DebugP_log("Removed interface '%c%c%d'\n", netif->name[0], netif->name[1], netif->num);
    }
}

void EthApp_setNetifCbs(struct netif *netif)
{
    netif_set_status_callback(netif, EthApp_netifStatusCb);
    netif_set_link_callback(netif, App_netifLinkChangeCb);
}

static void App_netifLinkChangeCb(struct netif *pNetif)
{
    if (netif_is_link_up(pNetif))
    {
        DebugP_log("[%d]Network Link UP Event\r\n", pNetif->num);
    }
    else
    {
        DebugP_log("[%d]Network Link DOWN Event\r\n", pNetif->num);
    }
    return;
}

int32_t App_isNetworkUp(struct netif* netif_)
{
    return (netif_is_up(netif_) && netif_is_link_up(netif_) && !ip4_addr_isany_val(*netif_ip4_addr(netif_)));
}

void EthApp_lwipMain(void *a0,
                           void *a1)
{
    err_t err;
    sys_sem_t initSem;

    /* initialize lwIP stack and network interfaces */
    err = sys_sem_new(&initSem, 0);
    LWIP_ASSERT("failed to create initSem", err == ERR_OK);
    LWIP_UNUSED_ARG(err);

    tcpip_init(EthApp_initLwip, &initSem);

    /* we have to wait for initialization to finish before
     * calling update_adapter()! */
    sys_sem_wait(&initSem);
    sys_sem_free(&initSem);

#if (LWIP_SOCKET || LWIP_NETCONN) && LWIP_NETCONN_SEM_PER_THREAD
    netconn_thread_init();
#endif
}

static void EthApp_initLwip(void *arg)
{
    sys_sem_t *initSem;

    LWIP_ASSERT("arg != NULL", arg != NULL);
    initSem = (sys_sem_t*)arg;

    /* init randomizer again (seed per thread) */
    srand((unsigned int)sys_now()/1000);

    /* init network interfaces */
    EthApp_initNetif();

#if defined(ETHAPP_ENABLE_IPERF_SERVER)
    /* Enable iperf */
    lwiperf_example_init();
#endif

    sys_sem_signal(initSem);
}