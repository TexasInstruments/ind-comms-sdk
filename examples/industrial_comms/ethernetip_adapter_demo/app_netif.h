/*
 *  Copyright (C) 2021-2024 Texas Instruments Incorporated
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
 * \file  app_netif.h
 *
 * \brief This file contains the implementation of bridge netif and related APIs.
 */
#ifndef _APP_NETIF_H_
#define _APP_NETIF_H_

/* Use DHCP mode for IP assignment
 * 0 - static IP mode, 1 - DHCP mode
 */
#define USE_DHCP        (0U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
typedef struct Icss_MacAddr_s
{
    /*! MAC address octets */
    uint8_t macAddr[6];
} Icss_MacAddr;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

void EthApp_initNetif(void);

void App_waitForBridgeUp();

int32_t AddNetif_addBridgeMcastEntry(Icss_MacAddr mac);

int32_t AddNetif_delBridgeMcastEntry(Icss_MacAddr mac);

#endif /* _APP_NETIF_H_ */