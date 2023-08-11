/*!
 *  \file appRst.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application, implementation of EtherNet/IP adapter reset feature.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 None.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  <ol>
 *  <li>Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer./<li>
 *  <li>Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.</li>
 *  <li>Neither the name of the copyright holder nor the names of its contributors
 *  may be used to endorse or promote products derived from this software without
 *  specific prior written permission.</li>
 *  </ol>
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 *  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "api/EI_API.h"
#include "api/EI_API_def.h"

#include "drivers/CUST_drivers.h"

#include "appCipCodes.h"
#include "appNV.h"
#include "appCfg.h"
#include "appRst.h"

#include <deviceProfiles/app_device_profile.h>

const EI_APP_CFG_AdapterData_t  EI_APP_RST_adapterFactoryDefaultValues =
{
        .ipAddr =       0xc0a8010a,
        .ipNwMask =     0xffffff00,
        .ipGateway =    0xc0a80101,
        .nameServer1 =  0x00000000,
        .nameServer2 =  0x00000000,
        .aDomainName = "",
        .aHostName = "",
        .configurationMethod = EIP_eCFGMETHOD_STATIC,
        .ttlValue = 1,
        .acdActive = true,
        .aAcdAddr = { 0,0,0,0,0,0 },
        .aAcdHdr = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
        .intfConfig[0].bit.ETHIntfActive = 1,
        .intfConfig[0].bit.ETHIntfAutoNeg = 1,
        .intfConfig[0].bit.ETHIntfFDuplex = 0,
        .intfConfig[0].bit.ETHIntf100MB = 0,
        .intfConfig[1].bit.ETHIntfActive = 1,
        .intfConfig[1].bit.ETHIntfAutoNeg = 1,
        .intfConfig[1].bit.ETHIntfFDuplex = 0,
        .intfConfig[1].bit.ETHIntf100MB = 0,
        .qosParameter.Q_Tag_Enable = EI_API_ADP_DEFAULT_8021Q,
        .qosParameter.DSCP_PTP_Event = EI_API_ADP_DEFAULT_DSCP_PTP_EVENT,
        .qosParameter.DSCP_PTP_General = EI_API_ADP_DEFAULT_DSCP_PTP_GENERAL,
        .qosParameter.DSCP_Urgent = EI_API_ADP_DEFAULT_DSCP_URGENT,
        .qosParameter.DSCP_Scheduled = EI_API_ADP_DEFAULT_DSCP_SCHEDULED,
        .qosParameter.DSCP_High = EI_API_ADP_DEFAULT_DSCP_HIGH,
        .qosParameter.DSCP_Low = EI_API_ADP_DEFAULT_DSCP_LOW,
        .qosParameter.DSCP_Explicit = EI_API_ADP_DEFAULT_DSCP_EXPLICIT,

        .encapInactTimeout = 120,
        .mcastConfig.allocControl = 0,

#if defined(TIME_SYNC)
        .ptpEnable                  = 1,
        .portEnable                 = 1,
        .portLogAnnounceInterval    = 1,
        .portLogSyncInterval        = 0,
        .domainNumber               = 0,
        .aUserDescription           = {"Kunbus;EthernetIP Adapter;"},
#else
        .ptpEnable                  = 0,
        .portEnable                 = 1,
        .portLogAnnounceInterval    = 1,
        .portLogSyncInterval        = 0,
        .domainNumber               = 0,
        .aUserDescription           = {"Kunbus;EthernetIP Adapter;"},
#endif

#if defined(QUICK_CONNECT)
        .quickConnectEnabled       = true,
#else
        .quickConnectEnabled       = false,
#endif
        .lldpParameter.enableArrayLength   = 3,
        .lldpParameter.enableArray.allBits = 7,
        .lldpParameter.msgTxInterval       = 30,
        .lldpParameter.msgTxHold           = 4,
};

static int16_t  EI_APP_RST_serviceFlag_s;

static bool     EI_APP_RST_isRequired_s = false;
static uint32_t EI_APP_RST_time_s       = 0;

bool EI_APP_RST_init (EI_API_ADP_T *pAdapter)
{
    bool ret = true;

    return ret;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Performs a factory reset.
 *
 *  \details
 *  Performs a factory reset
 *  - With serviceFlag == 1: restore default data
 *  - With serviceFlag == 2: restore default data, except communication link attributes.
 *  - Other values:          do nothing
 *
 */
bool EI_APP_RST_execute(int16_t serviceFlag)
{
    bool ret = false;

    switch (serviceFlag)
    {
        case 1:
        {
            // Restore default data.
            EI_APP_CFG_setDefault();
            break;
        }
        case 2:
        {
            // Restore default data except communication link attributes
            EI_APP_CFG_setDefaultWithoutComm();
            break;
        }
        default:
        {
            return ret;
        }
    }

    return EI_APP_NV_write(true);
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback for reset service of class 0x01.
 *
 *  \details
 *  Callback for reset service of class 0x01. Sets the timestamp for a delayed reset.
 */
void EI_APP_RST_service (EI_API_CIP_NODE_T *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag)
{
    if (serviceFlag == 1 || serviceFlag == 2)
    {
        // Reset with parameter 1 or 2 means factory reset.
        EI_APP_RST_execute(serviceFlag);
    }

    EI_APP_RST_time_s        = OSAL_getMsTick();
    EI_APP_RST_isRequired_s  = true;
    EI_APP_RST_serviceFlag_s = serviceFlag;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Checks if a reset is required.
 *
 *  \details
 *  Checks if a reset is required and 2 seconds are expired.
 +
 */
int16_t EI_APP_RST_getServiceFlag(void)
{
    uint32_t actTime = OSAL_getMsTick();
    uint32_t difTime = 0;

    if (EI_APP_RST_isRequired_s)
    {
        // Wait 2 seconds for reset:
        if (actTime < EI_APP_RST_time_s)
        {
            difTime = (0xFFFFFFFF - EI_APP_RST_time_s) + actTime;
        }
        else
        {
            difTime = actTime - EI_APP_RST_time_s;
        }

        if ( (difTime > 2000) &&
             (false  == CUST_DRIVERS_PRM_isWritePending()) )
        {
            EI_APP_RST_isRequired_s = false;
            return EI_APP_RST_serviceFlag_s;
        }
    }
    return -1;
}
