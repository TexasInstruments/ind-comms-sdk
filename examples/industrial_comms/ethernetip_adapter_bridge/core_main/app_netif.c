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
 * \file  app_netif.c
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


#include <lwipific/inc/lwip_ic.h>
#include <lwipific/inc/lwip2lwipif_ic.h>

#include <lwip2lwipif.h>
//#include <custom_pbuf.h>
#include "enet_netific.h"
#include "app_netif.h"
#include "emac_lwipif.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* BridgeIf configuration parameters */
#define ETHAPP_LWIP_BRIDGE_MAX_PORTS (4U)
#define ETHAPP_LWIP_BRIDGE_MAX_DYNAMIC_ENTRIES (32U)
#define ETHAPP_LWIP_BRIDGE_MAX_STATIC_ENTRIES (8U)

/* BridgeIf port IDs
 * Used for creating CoreID to Bridge PortId Map
 */
#define ETHAPP_BRIDGEIF_PORT1_ID        (1U)
#define ETHAPP_BRIDGEIF_PORT2_ID        (2U)
#define ETHAPP_BRIDGEIF_CPU_PORT_ID     BRIDGEIF_MAX_PORTS

/* Inter-core netif IDs */
#define ETHAPP_NETIF_IC_MCU2_0_MCU2_1_IDX   (0U)
#define ETHAPP_NETIF_IC_MCU2_1_MCU2_0_IDX   (1U)
#define ETHAPP_NETIF_IC_MCU2_0_A72_IDX      (2U)
#define ETHAPP_NETIF_IC_MAX_IDX             (3U)

/* Max length of shared mcast address list */
#define ETHAPP_MAX_SHARED_MCAST_ADDR        (8U)

/* Required size of the MAC address pool (specific to the TI EVM configuration):
 *  1 x MAC address for Ethernet Firmware
 *  2 x MAC address for mpu1_0 virtual switch and MAC-only ports (Linux, 1 for QNX)
 *  2 x MAC address for mcu2_1 virtual switch and MAC-only ports (RTOS)
 *  1 x MAC address for mcu2_1 virtual switch port (AUTOSAR) */
#define ETHAPP_MAC_ADDR_POOL_SIZE               (6U)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


struct netif netif_bridge;

static struct netif gEmacNetif;
// static struct netif *g_pNetif[ENET_SYSCFG_NETIF_COUNT];

static struct netif netif_ic[ETHAPP_NETIF_IC_MAX_IDX];

/* Array to store coreId to lwip bridge portId map */
static uint8_t gEthApp_lwipBridgePortIdMap[IPC_MAX_PROCS];

/* dhcp struct for the ethernet netif */
static struct dhcp g_netifDhcp[3];

static uint32_t netif_ic_state[IC_ETH_MAX_VIRTUAL_IF] =
{
    IC_ETH_IF_MCU2_0_MCU2_1,
    IC_ETH_IF_MCU2_1_MCU2_0,
    IC_ETH_IF_MCU2_0_A72
};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void EthApp_initEmacNetif(const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw);

// static void App_asyncIoctlCb(Enet_Event evt,
//                             uint32_t evtNum,
//                             void *evtCbArgs,
//                             void *arg1,
//                             void *arg2);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void EthApp_initNetif(void)
{
    ip4_addr_t ipaddr, netmask, gw;
    err_t err;

    ip4_addr_set_zero(&gw);
    ip4_addr_set_zero(&ipaddr);
    ip4_addr_set_zero(&netmask);

    DebugP_log("\r\nStarting lwIP, local interface IP is dhcp-enabled\n");

    /* Create ICSS EMAC ethernet interface */
    // EthApp_initEmacNetif(&ipaddr, &netmask, &gw);// - open_icss_netif() //FIX ME = add later

    // netif_add(&gEmacNetif, &ipaddr, &netmask, &gw, NULL, LWIPIF_LWIP_init, tcpip_input);
    netif_add(&gEmacNetif, NULL, NULL, NULL, NULL, LWIPIF_LWIP_init, tcpip_input);

    /* Create inter-core virtual ethernet interface: MCU2_0 <-> MCU2_1 */
    netif_add(&netif_ic[ETHAPP_NETIF_IC_MCU2_0_MCU2_1_IDX], NULL, NULL, NULL,
              (void*)&netif_ic_state[IC_ETH_IF_MCU2_0_MCU2_1],
              LWIPIF_LWIP_IC_init, tcpip_input);

    gEmacNetif.flags |= NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;
    netif_ic[ETHAPP_NETIF_IC_MCU2_0_MCU2_1_IDX].flags |= NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;

    /* Initialize data for bridge as (no. of ports, no. of dynamic entries, no. of static entries, MAC address of the bridge) */
    bridgeif_initdata_t mybridge_initdata = BRIDGEIF_INITDATA1 (ETHAPP_LWIP_BRIDGE_MAX_PORTS,
                                                                ETHAPP_LWIP_BRIDGE_MAX_DYNAMIC_ENTRIES,
                                                                ETHAPP_LWIP_BRIDGE_MAX_STATIC_ENTRIES,
                                                                ETH_ADDR(0xF4, 0x84, 0x4C, 0xF9, 0x4D, 0x29));

    netif_add(&netif_bridge, &ipaddr, &netmask, &gw, &mybridge_initdata, bridgeif_init, netif_input);

    /* Add all netifs to the bridge and create coreId to bridge portId map */
    bridgeif_add_port(&netif_bridge, &gEmacNetif);
    gEthApp_lwipBridgePortIdMap[IPC_MCU1_0] = ETHAPP_BRIDGEIF_CPU_PORT_ID;

    bridgeif_add_port(&netif_bridge, &netif_ic[ETHAPP_NETIF_IC_MCU2_0_MCU2_1_IDX]);
    gEthApp_lwipBridgePortIdMap[IPC_MCU1_1] = ETHAPP_BRIDGEIF_PORT1_ID;

    /* Set bridge interface as the default */
    netif_set_default(&netif_bridge);
    dhcp_set_struct(&netif_bridge, &g_netifDhcp[0]);

    EthApp_setNetifCbs(&gEmacNetif);
    // netif_set_status_callback(&gEmacNetif, hsrprp_LwipStatus_callback);
    // netif_set_link_callback(&gEmacNetif, hsrprp_LwipLink_callback);
    EthApp_setNetifCbs(netif_default);

    netif_set_up(&gEmacNetif);
    netif_set_up(&netif_ic[ETHAPP_NETIF_IC_MCU2_0_MCU2_1_IDX]);
    netif_set_up(&netif_bridge);

    sys_lock_tcpip_core();
    err = dhcp_start(&netif_bridge);
    sys_unlock_tcpip_core();


// SICK debug - Check emac netif getting IP address
    // netif_add(&gEmacNetif, &ipaddr, &netmask, &gw, NULL, LWIPIF_LWIP_init, tcpip_input);
    // netif_set_default(&gEmacNetif);
    // dhcp_set_struct(&gEmacNetif, &g_netifDhcp[0]);
    // EthApp_setNetifCbs(netif_default);
    // netif_set_up(&gEmacNetif);
    // sys_lock_tcpip_core();
    // err = dhcp_start(&gEmacNetif);
    // sys_unlock_tcpip_core();


    if (err != ERR_OK)
    {
        DebugP_log("Failed to start DHCP: %d\n", err);
    }
}

static void EthApp_waitForNetifUp(struct netif *netif)
{
    while (!App_isNetworkUp(netif))
    {
        DebugP_log("[%d]Waiting for network UP ...\r\n",netif->num);
        ClockP_sleep(2);
    }
}

void App_waitForBridgeUp()
{
    EthApp_waitForNetifUp(&netif_bridge);
// SICK debug - Check emac netif getting IP address
    // EthApp_waitForNetifUp(&gEmacNetif);
}

// static void EthApp_initEmacNetif(const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw)
// {
//     hsrprp_LwipTest_netif_addCb(/*ipaddr, netmask, gw*/);
//     gEmacNetif = hsrprp_pass_netif();
// }

// static void App_asyncIoctlCb(Enet_Event evt,
//                             uint32_t evtNum,
//                             void *evtCbArgs,
//                             void *arg1,
//                             void *arg2)
// {
//     SemaphoreP_Object *pAsyncSem = (SemaphoreP_Object *)evtCbArgs;
//     SemaphoreP_post(pAsyncSem);
// }

// int32_t App_addMacFdbEntry(Enet_Type enetType, uint32_t instId, Icssg_MacAddr mac)
// {
//     Icssg_FdbEntry fdbEntry;
//     Enet_IoctlPrms prms;
//     int32_t status = ENET_SOK;
//     SemaphoreP_Object asyncIoctlSemObj;
//     EnetApp_HandleInfo handleInfo;

//     EnetApp_acquireHandleInfo(enetType, instId, &handleInfo);

//     EnetAppUtils_assert(handleInfo.hEnet->enetPer->enetType == ENET_ICSSG_SWITCH);
//     EnetAppUtils_print("For ICSSG, EthType and VlanId are not used to match the packet only dest addr is used \r\n");

//     status = SemaphoreP_constructBinary(&asyncIoctlSemObj, 0);
//     DebugP_assert(SystemP_SUCCESS == status);

//     Enet_registerEventCb(handleInfo.hEnet,
//                         ENET_EVT_ASYNC_CMD_RESP,
//                         0U,
//                         App_asyncIoctlCb,
//                         (void *)&asyncIoctlSemObj);

//     memset(&fdbEntry, 0, sizeof(fdbEntry));
//     fdbEntry.vlanId = ((int16_t)-1);
//     memcpy(&fdbEntry.macAddr, mac.macAddr, 6U);

//     fdbEntry.fdbEntry[0] = (uint8_t)((ICSSG_FDB_ENTRY_P0_MEMBERSHIP |
//                                      ICSSG_FDB_ENTRY_VALID) & 0xFF);
//     fdbEntry.fdbEntry[1] = (uint8_t)((ICSSG_FDB_ENTRY_P0_MEMBERSHIP |
//                                      ICSSG_FDB_ENTRY_VALID) & 0xFF);
//     ENET_IOCTL_SET_IN_ARGS(&prms, &fdbEntry);
//     ENET_IOCTL(handleInfo.hEnet, CSL_CORE_ID_R5FSS0_0, ICSSG_FDB_IOCTL_ADD_ENTRY, &prms, status);
//     if (status == ENET_SINPROGRESS)
//     {
//         /* Wait for asyc ioctl to complete */
//         do
//         {
//             Enet_poll(handleInfo.hEnet, ENET_EVT_ASYNC_CMD_RESP, NULL, 0U);
//             status = SemaphoreP_pend(&asyncIoctlSemObj, SystemP_WAIT_FOREVER);
//             if (SystemP_SUCCESS == status)
//             {
//                 break;
//             }
//         } while (1);

//         status = ENET_SOK;
//     }
//     else
//     {
//         EnetAppUtils_print("Failed to set the SPL mac entry: %d\n", status);
//     }

//     return status;
// }

// static void EthApp_initEnetNetif(LwipifEnetApp_Handle handle, uint32_t netifIdx, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw)
// {
//     LWIP_ASSERT("Invalid Netif Index", (netifIdx <= ENET_SYSCFG_NETIF_COUNT));
//     g_pNetif[netifIdx] = LwipifEnetApp_netifOpen(hlwipIfApp, netifIdx, ipaddr, netmask, gw);
//     LwipifEnetApp_startSchedule(hlwipIfApp, g_pNetif[netifIdx]);
// }

// void App_shutdownNetworkStack()
// {
//     for (uint32_t netifIdx = 0U; netifIdx < ENET_SYSCFG_NETIF_COUNT; netifIdx++)
//     {
//         LwipifEnetApp_netifClose(hlwipIfApp, NETIF_INST_ID0 + netifIdx);
//     }
//     return;
// }