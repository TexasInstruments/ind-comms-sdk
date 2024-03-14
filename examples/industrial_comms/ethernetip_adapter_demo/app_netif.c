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

#include <lwip_ic.h>
#include <lwip2lwipif_ic.h>
#include <lwip2lwipif.h>

#include "netif_common.h"
#include "app_netif.h"
#include "ti_ic_open_close.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* BridgeIf configuration parameters */
#define ETHAPP_LWIP_BRIDGE_MAX_PORTS (4U)
#define ETHAPP_LWIP_BRIDGE_MAX_DYNAMIC_ENTRIES (32U)
#define ETHAPP_LWIP_BRIDGE_MAX_STATIC_ENTRIES (32U)

/* BridgeIf port IDs
 * Used for creating CoreID to Bridge PortId Map
 */
#define ETHAPP_BRIDGEIF_PORT1_ID        (1U)
#define ETHAPP_BRIDGEIF_PORT2_ID        (2U)
#define ETHAPP_BRIDGEIF_CPU_PORT_ID     BRIDGEIF_MAX_PORTS


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

static struct netif netif_ic[IC_ETH_MAX_VIRTUAL_IF];

/* Array to store coreId to lwip bridge portId map */
static uint8_t gEthApp_lwipBridgePortIdMap[IPC_MAX_PROCS];

/* dhcp struct for the ethernet netif */
static struct dhcp g_netifDhcp[IC_ETH_MAX_VIRTUAL_IF];

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void EthApp_waitForNetifUp(struct netif *netif);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void EthApp_initNetif(void)
{
    ip4_addr_t ipaddr, netmask, gw;
    Ic_Object_Handle hIcObj;
    err_t err;

    ip4_addr_set_zero(&gw);
    ip4_addr_set_zero(&ipaddr);
    ip4_addr_set_zero(&netmask);

    DebugP_log("\r\nStarting lwIP, local interface IP is dhcp-enabled\n");

    netif_add(&gEmacNetif, &ipaddr, &netmask, &gw, NULL, LWIPIF_LWIP_init, tcpip_input);
    // netif_add(&gEmacNetif, NULL, NULL, NULL, NULL, LWIPIF_LWIP_init, tcpip_input);

    /* Create and initialise Intercore shared memory driver */
    hIcObj = App_doIcOpen(IC_ETH_IF_R5_0_0_R5_0_1);
    DebugP_assert(hIcObj != NULL);

    /* Create inter-core virtual ethernet interface: MCU2_0 <-> MCU2_1 */
    netif_add(&netif_ic[IC_ETH_IF_R5_0_0_R5_0_1], &ipaddr, &netmask, &gw,
              NULL, LWIPIF_LWIP_IC_init, tcpip_input);

    err = LWIPIF_LWIP_IC_start(&netif_ic[IC_ETH_IF_R5_0_0_R5_0_1], IC_ETH_IF_R5_0_0_R5_0_1, hIcObj);
    DebugP_assert(err == ERR_OK);

    gEmacNetif.flags |= NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;
    netif_ic[IC_ETH_IF_R5_0_0_R5_0_1].flags |= NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;

    /* Initialize data for bridge as (no. of ports, no. of dynamic entries, no. of static entries, MAC address of the bridge) */
    bridgeif_initdata_t mybridge_initdata = BRIDGEIF_INITDATA1 (ETHAPP_LWIP_BRIDGE_MAX_PORTS,
                                                                ETHAPP_LWIP_BRIDGE_MAX_DYNAMIC_ENTRIES,
                                                                ETHAPP_LWIP_BRIDGE_MAX_STATIC_ENTRIES,
                                                                ETH_ADDR(0xF4, 0x84, 0x4C, 0xF9, 0x4D, 0x29));

    /* Netif state of the bridge netif is filled with the bridge handle,
     * which is used to operate on bridge settings from the application.
     * Don't pass any arguement to be set as netif state
     */
    netif_add(&netif_bridge, &ipaddr, &netmask, &gw, &mybridge_initdata, bridgeif_init, netif_input);

    /* Add all netifs to the bridge and create coreId to bridge portId map */
    bridgeif_add_port(&netif_bridge, &gEmacNetif);
    gEthApp_lwipBridgePortIdMap[IPC_R5_0_0] = ETHAPP_BRIDGEIF_PORT1_ID;

    bridgeif_add_port(&netif_bridge, &netif_ic[IC_ETH_IF_R5_0_0_R5_0_1]);
    gEthApp_lwipBridgePortIdMap[IPC_R5_0_1] = ETHAPP_BRIDGEIF_PORT2_ID;

    /* Set bridge interface as the default */
    netif_set_default(&netif_bridge);
    dhcp_set_struct(&netif_bridge, &g_netifDhcp[0]);

    EthApp_setNetifCbs(&gEmacNetif);
    EthApp_setNetifCbs(netif_default);

    netif_set_up(&gEmacNetif);
    netif_set_up(&netif_ic[IC_ETH_IF_R5_0_0_R5_0_1]);
    netif_set_up(netif_default);

    sys_lock_tcpip_core();
    err = dhcp_start(netif_default);
    sys_unlock_tcpip_core();

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
}

int32_t AddNetif_addBridgeMcastEntry(Icss_MacAddr mac)
{
    int32_t status;
    struct eth_addr ethAddr;
    for (uint32_t i = 0; i < 6U; i++)
    {
        ethAddr.addr[i] = mac.macAddr[i];
    }

    status= bridgeif_fdb_add(netif_default, &ethAddr,
                             (gEthApp_lwipBridgePortIdMap[IPC_R5_0_1] |
                              gEthApp_lwipBridgePortIdMap[IPC_R5_0_1]));
    return status;
}

int32_t AddNetif_delBridgeMcastEntry(Icss_MacAddr mac)
{
    int32_t status;
    struct eth_addr ethAddr;
    for (uint32_t i = 0; i < 6U; i++)
    {
        ethAddr.addr[i] = mac.macAddr[i];
    }

    status= bridgeif_fdb_remove(netif_default, &ethAddr);
    return status;
}
