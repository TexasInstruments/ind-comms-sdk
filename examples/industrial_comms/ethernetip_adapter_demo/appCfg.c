/*!
 *  \file appCfg.c
 *
 *  \brief
 *  Implementation of EtherNet/IP adapter configuration feature.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br><br>
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

#include "EI_API.h"
#include "EI_API_def.h"

#include "drivers/CUST_drivers.h"

#include "appCipCodes.h"
#include "appNV.h"
#include "appCfg.h"

#include "device_profiles/app_device_profile.h"


static bool EI_APP_CFG_getTcpip   (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
static bool EI_APP_CFG_getTsync   (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
static bool EI_APP_CFG_getQos     (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
static bool EI_APP_CFG_getEthLink (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
static bool EI_APP_CFG_getLldpMng (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);

static uint32_t EI_APP_CFG_setTcpip   (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData);
static uint32_t EI_APP_CFG_setTsync   (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData);
static uint32_t EI_APP_CFG_setQos     (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData);
static uint32_t EI_APP_CFG_setEthLink (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData);
static uint32_t EI_APP_CFG_setLldpMng (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData);

static EI_API_ADP_T                *EI_APP_CFG_pAdapter_s      = NULL;
static bool                         EI_APP_CFG_isChanged_s     = false;

/*!
 * \brief
 * Short description. Remove all tags that are not needed.
 *
 * \details
 * Detailed description.
 *
 * \remarks
 * Some remarks
 *
 * \pre
 * description of the precondition
 *
 * \post
 * description of the postcondition
 *
 * \warning
 * Some warning
 *
 * \param[in]     node                         A node.
 * \param[inout]  data                         A given non formatted data and returned formatted data.
 * \param[out]    pRet                         Pointer to a return value.
 *
 * \return        #NAMESPACE_Error_t as uint32_t.
 * \retval        #NAMESPACE_ERR_OK            Success.
 * \retval        #NAMESPACE_ERR_FAIL          Something went wrong.
 *
 * \par Example
 * \code{.c}
 * #include <api/NAMESPACE.h>
 *
 * Short code example demonstrating the call sequence
 *
 * \endcode
 *
 * \see ReferenceA  ReferenceB
 *
 * \ingroup MyGroup
 *
 */
bool EI_APP_CFG_init (EI_API_ADP_T *pAdapter)
{
    bool ret = false;

    EI_APP_CFG_pAdapter_s = pAdapter;

    ret =  EI_APP_DEVICE_PROFILE_CFG_init(pAdapter);

    return ret;
}

bool EI_APP_CFG_isValid(void)
{
    return EI_APP_DEVICE_PROFILE_CFG_isValid();
}

uint32_t EI_APP_CFG_getLength (void)
{
    return EI_APP_DEVICE_PROFILE_CFG_getLength();
}

void* EI_APP_CFG_getRuntimeData (void)
{
    return EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();
}

void* EI_APP_CFG_getFactoryResetData (void)
{
    return EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData();
}

void EI_APP_CFG_setHeader(EI_APP_CFG_Header_t *pHeader)
{
    return EI_APP_DEVICE_PROFILE_CFG_setHeader(pHeader);
}

void EI_APP_CFG_setDefault (void)
{
    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();
    EI_APP_CFG_Data_t *pResetData   = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData();
    uint32_t           length       = EI_APP_DEVICE_PROFILE_CFG_getLength();

    OSAL_MEMORY_memcpy(pRuntimeData, pResetData, length);

    return;
}

void EI_APP_CFG_setDefaultWithoutComm (void)
{
    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();
    EI_APP_CFG_Data_t *pResetData   = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData();

    // Restore default data, except communication link attributes, these are:
    // TCP/IP object 0xF5, attributes 3, 5 and 6.
    // Ethernet Link object 0xF6, attribute 6.
    pRuntimeData->adapter.ttlValue  = pResetData->adapter.ttlValue;
    pRuntimeData->adapter.acdActive = pResetData->adapter.acdActive;
    pRuntimeData->adapter.acdState  = pResetData->adapter.acdState;

    OSAL_MEMORY_memcpy(pRuntimeData->adapter.aAcdAddr, pResetData->adapter.aAcdAddr, sizeof(pResetData->adapter.aAcdAddr));
    OSAL_MEMORY_memcpy(pRuntimeData->adapter.aAcdHdr,  pResetData->adapter.aAcdHdr,  sizeof(pResetData->adapter.aAcdHdr));

    pRuntimeData->adapter.encapInactTimeout = pResetData->adapter.encapInactTimeout;
    OSAL_MEMORY_memcpy(&pRuntimeData->adapter.qosParameter, &pResetData->adapter.qosParameter, sizeof(pResetData->adapter.qosParameter));

    //timeSync attributes
    pRuntimeData->adapter.ptpEnable               = pResetData->adapter.ptpEnable;
    pRuntimeData->adapter.portEnable              = pResetData->adapter.portEnable;
    pRuntimeData->adapter.portLogSyncInterval     = pResetData->adapter.portLogSyncInterval;
    pRuntimeData->adapter.portLogAnnounceInterval = pResetData->adapter.portLogAnnounceInterval;
    pRuntimeData->adapter.domainNumber            = pResetData->adapter.domainNumber;

    OSAL_MEMORY_memcpy(pRuntimeData->adapter.aUserDescription, pResetData->adapter.aUserDescription, 128);

    OSAL_MEMORY_memcpy(&pRuntimeData->adapter.lldpParameter, &pResetData->adapter.lldpParameter, sizeof(pResetData->adapter.lldpParameter));

#if defined(EIP_QUICK_CONNECT) && (EIP_QUICK_CONNECT == 1)
    pRuntimeData->adapter.quickConnectEnabled = pResetData->adapter.quickConnectEnabled;
#endif

    EI_APP_DEVICE_PROFILE_CFG_setDefaultWithoutComm();

    return;
}

uint32_t EI_APP_CFG_apply(EI_API_ADP_T *pAdapter)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();

    EI_APP_DEVICE_PROFILE_CFG_apply(pAdapter);

    // common area settings
    errCode = EI_APP_CFG_setTcpip(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_APP_CFG_setEthLink(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_APP_CFG_setQos(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_APP_CFG_setTsync(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_APP_CFG_setLldpMng(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

#if defined(EIP_QUICK_CONNECT) && (EIP_QUICK_CONNECT == 1)
    // Enable QuickConnect
    EI_API_ADP_setQuickConnectEnabled(pAdapter, pRuntimeData->adapter.quickConnectEnabled);
#endif

laError:
    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return errCode;
}

bool EI_APP_CFG_isChanged(void)
{
    bool ret = EI_APP_CFG_isChanged_s;

    if (true == EI_APP_CFG_isChanged_s)
    {
        EI_APP_CFG_isChanged_s = false;
    }

    return ret;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function for write accesses of several attributes.
 *
 *  \details
 *  Callback function for write accesses of several attributes. Saves the new permanent data.
 *  Sets new network configuration, if necessary. Sets aHostName, if necessary.
 */
void EI_APP_CFG_callback ( EI_API_CIP_NODE_T *pCipNode,
                           uint16_t           classId,
                           uint16_t           instanceId,
                           uint16_t           attrId,
                           EI_API_CIP_ESc_t   serviceCode,
                           int16_t            serviceFlag)
{
   static uint32_t lastCalled;

   // Early exit, because we are only interested if Set_Attribute was executed.
   if (EI_API_CIP_eSC_SETATTRSINGLE != serviceCode)
   {
       return;
   }

   if (100 > (OSAL_getMsTick() - lastCalled))
   {
       OSAL_SCHED_sleep(100);
   }

   lastCalled = OSAL_getMsTick();

   switch(classId)
   {
       case EI_APP_CIP_CLASS_ID_TCPIP_INTERFACE:
       {
           EI_APP_CFG_isChanged_s = EI_APP_CFG_getTcpip(EI_APP_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       case EI_APP_CIP_CLASS_ID_TIME_SYNC:
       {
           EI_APP_CFG_isChanged_s = EI_APP_CFG_getTsync(EI_APP_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       case EI_APP_CIP_CLASS_ID_QUALITY_OF_SERVICE:
       {
           EI_APP_CFG_isChanged_s = EI_APP_CFG_getQos(EI_APP_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       case EI_APP_CIP_CLASS_ID_ETHERNET_LINK:
       {
           // Ethernet link object has changed for instance 1 or 2.
           EI_APP_CFG_isChanged_s = EI_APP_CFG_getEthLink(EI_APP_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       case EI_APP_CIP_CLASS_ID_LLDP_MANAGEMENT:
       {
           EI_APP_CFG_isChanged_s = EI_APP_CFG_getLldpMng(EI_APP_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       default:
       {
           // Nothing has changed.
           EI_APP_CFG_isChanged_s = false;
           break;
       }
   }

   if (false == EI_APP_CFG_isChanged_s)
   {
       EI_APP_CFG_isChanged_s = EI_APP_DEVICE_PROFILE_CFG_callback(pCipNode, classId, instanceId, attrId, serviceCode, serviceFlag);
   }
}

static bool EI_APP_CFG_getTcpip (EI_API_ADP_T      *pAdapter,
                                 uint16_t           instanceId,
                                 uint16_t           attrId,
                                 EI_API_CIP_ESc_t   serviceCode,
                                 int16_t            serviceFlag)
{
    bool               ret          = false;
    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();

    switch(attrId)
    {
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03:
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_05:
        {
            bool hwConfigEnabled;
            char aDomainName[49];
            uint32_t ipAddr;                 // IP address
            uint32_t ipNwMask;               // Network mask
            uint32_t ipGateway;              // Gateway address
            uint32_t nameServer1;            // First name server address
            uint32_t nameServer2;            // Second name server address
            EIP_SConfigurationControl_t configurationControl; // TCP/IP object attribute 3

            EI_API_ADP_isHwSettingEnabled(pAdapter, &hwConfigEnabled);
            if (hwConfigEnabled)
            {
                // Network configuration is controlled by hardware, do nothing.
                break;
            }

            // Attribute 3 (configuration control) or
            // Attribute 5 (interface configuration) is set.
            EI_API_ADP_getIpAddr(pAdapter, &ipAddr);
            EI_API_ADP_getIpNwMask(pAdapter, &ipNwMask);
            EI_API_ADP_getIpGateway(pAdapter, &ipGateway);
            EI_API_ADP_getIpPriNameServer(pAdapter, &nameServer1);
            EI_API_ADP_getIpSecNameServer(pAdapter, &nameServer2);
            EI_API_ADP_getDomainName(pAdapter, aDomainName);
            EI_API_ADP_getConfigurationControl(pAdapter, &configurationControl);

            if ( (ipAddr      != pRuntimeData->adapter.ipAddr)      ||
                 (ipNwMask    != pRuntimeData->adapter.ipNwMask)    ||
                 (ipGateway   != pRuntimeData->adapter.ipGateway)   ||
                 (nameServer1 != pRuntimeData->adapter.nameServer1) ||
                 (nameServer2 != pRuntimeData->adapter.nameServer2) ||
                 (configurationControl.configurationMethod != pRuntimeData->adapter.configurationMethod) ||
                 (strncmp(aDomainName, pRuntimeData->adapter.aDomainName, sizeof(pRuntimeData->adapter.aDomainName)) != 0) )
            {
                // Attribute 3 (configuration control) or
                // Attribute 5 (interface configuration) is set.
                pRuntimeData->adapter.ipAddr              = ipAddr;
                pRuntimeData->adapter.ipNwMask            = ipNwMask;
                pRuntimeData->adapter.ipGateway           = ipGateway;
                pRuntimeData->adapter.nameServer1         = nameServer1;
                pRuntimeData->adapter.nameServer2         = nameServer2;
                pRuntimeData->adapter.configurationMethod = configurationControl.configurationMethod;

                strncpy(pRuntimeData->adapter.aDomainName, aDomainName, sizeof(pRuntimeData->adapter.aDomainName));

                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06:
        {
            char aHostName[65];
            EI_API_ADP_getHostName(pAdapter, aHostName);
            if(0 != strcmp(aHostName, pRuntimeData->adapter.aHostName))
            {
                strncpy(pRuntimeData->adapter.aHostName, aHostName, sizeof(pRuntimeData->adapter.aHostName));
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_08:
        {
            uint8_t ttlValue;
            EI_API_ADP_getIpTTL(pAdapter, &ttlValue);
            if(ttlValue != pRuntimeData->adapter.ttlValue)
            {
                pRuntimeData->adapter.ttlValue = ttlValue;
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_09:
        {
            EI_API_ADP_SMcastConfig_t mcastConfig;
            EI_API_ADP_getMcastConfiguration(pAdapter, &mcastConfig);

            if (0 != OSAL_MEMORY_memcmp(&mcastConfig, &pRuntimeData->adapter.mcastConfig, sizeof(pRuntimeData->adapter.mcastConfig)))
            {
                OSAL_MEMORY_memcpy (&pRuntimeData->adapter.mcastConfig, &mcastConfig, sizeof(pRuntimeData->adapter.mcastConfig));
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_10:
        {
            bool acdActive;
            EI_API_ADP_getACD(pAdapter, &acdActive);
            if(acdActive != pRuntimeData->adapter.acdActive)
            {
                pRuntimeData->adapter.acdActive = acdActive;
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_11:
        {
            uint8_t             acdState;  // Attribute 11, state of acd
            EI_API_ADP_SParam_t acdAddr;
            EI_API_ADP_SParam_t acdHdr;

            EI_API_ADP_getEnipAcdState(pAdapter, &acdState);
            EI_API_ADP_getEnipAcdAddr (pAdapter, &acdAddr);
            EI_API_ADP_getEnipAcdHdr  (pAdapter, &acdHdr);

            if ( (acdState != pRuntimeData->adapter.acdState)                                                                    ||
                 (0        != OSAL_MEMORY_memcmp(acdAddr.data, pRuntimeData->adapter.aAcdAddr, sizeof(pRuntimeData->adapter.aAcdAddr))) ||
                 (0        != OSAL_MEMORY_memcmp(acdHdr.data,  pRuntimeData->adapter.aAcdHdr,  sizeof(pRuntimeData->adapter.aAcdHdr)))
               )
            {
                pRuntimeData->adapter.acdState = acdState;

                OSAL_MEMORY_memcpy(pRuntimeData->adapter.aAcdAddr, acdAddr.data, sizeof(pRuntimeData->adapter.aAcdAddr));
                OSAL_MEMORY_memcpy(pRuntimeData->adapter.aAcdHdr,  acdHdr.data,  sizeof(pRuntimeData->adapter.aAcdHdr));

                ret = true;
            }
            break;
        }
#if defined(EIP_QUICK_CONNECT) && (EIP_QUICK_CONNECT == 1)
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_12:
        {
            bool quickConnectEnabled;
            // Enable/Disable QuickConnect
            EI_API_ADP_getQuickConnectEnabled(pAdapter, &quickConnectEnabled);
            if(quickConnectEnabled != pRuntimeData->adapter.quickConnectEnabled)
            {
                pRuntimeData->adapter.quickConnectEnabled = quickConnectEnabled;
                ret = true;
            }
            break;
        }
#endif
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_13:
        {
            uint16_t encapInactTimeout;
            EI_API_ADP_getEncapInactTimeout(pAdapter, &encapInactTimeout);
            if(encapInactTimeout != pRuntimeData->adapter.encapInactTimeout)
            {
                pRuntimeData->adapter.encapInactTimeout = encapInactTimeout;
                ret = true;
            }
            break;
        }
        default:
        {
            // Nothing has changed.
            ret = false;
            break;
        }
    }

    return ret;
}

static uint32_t EI_APP_CFG_setTcpip (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    EIP_SConfigurationControl_t configurationControl;

    // Attribute 3 and 5
    configurationControl.configurationMethod = pRuntimeData->adapter.configurationMethod;
    configurationControl.dnsEnable = 0;
    configurationControl.reserved = 0;

    errCode = EI_API_ADP_setIpConfig (pAdapter,
                                      configurationControl,
                                      pRuntimeData->adapter.ipAddr,
                                      pRuntimeData->adapter.ipNwMask,
                                      pRuntimeData->adapter.ipGateway,
                                      pRuntimeData->adapter.nameServer1,
                                      pRuntimeData->adapter.nameServer2,
                                      pRuntimeData->adapter.aDomainName,
                                      false);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    // Attribute 6 - Host Name
    errCode = EI_API_ADP_setHostName(pAdapter, pRuntimeData->adapter.aHostName);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    // Attribute 8 - TTL Value
    errCode = EI_API_ADP_setIpTTL(pAdapter, pRuntimeData->adapter.ttlValue);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    // Attribute 9 - MCAST Config
    errCode = EI_API_ADP_setMcastConfiguration(pAdapter, &pRuntimeData->adapter.mcastConfig);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    // Attribute 10 - Select ACD
    errCode = EI_API_ADP_setACD(pAdapter, pRuntimeData->adapter.acdActive);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    // Attribute 11 - LastConflictDetected - ACD Activity
    errCode = EI_API_ADP_setEnipAcdState(pAdapter, pRuntimeData->adapter.acdState);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    EI_API_ADP_SParam_t aAcdAddr = { 6, (uint8_t*)&pRuntimeData->adapter.aAcdAddr };
    errCode = EI_API_ADP_setEnipAcdAddr(pAdapter, &aAcdAddr);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    EI_API_ADP_SParam_t aAcdHdr = { 28, (uint8_t*)&pRuntimeData->adapter.aAcdHdr };
    errCode = EI_API_ADP_setEnipAcdHdr(pAdapter, &aAcdHdr);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    // Attribute 13 - Encapsulation Inactivity Timeout
    errCode = EI_API_ADP_setEncapInactTimeout(pAdapter, pRuntimeData->adapter.encapInactTimeout);

laError:
    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return errCode;
}

static bool EI_APP_CFG_getTsync (EI_API_ADP_T      *pAdapter,
                                 uint16_t           instanceId,
                                 uint16_t           attrId,
                                 EI_API_CIP_ESc_t   serviceCode,
                                 int16_t            serviceFlag)
{
    bool ret = false;

    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();

    switch(attrId)
    {
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_01:
        {
            bool ptpEnable;
            EI_API_ADP_getPtpEnable(pAdapter, &ptpEnable);
            if(ptpEnable != pRuntimeData->adapter.ptpEnable)
            {
                pRuntimeData->adapter.ptpEnable = ptpEnable;
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_13:
        {
            bool portEnable;
            EI_API_ADP_getPortEnable(pAdapter, &portEnable);
            if(portEnable != pRuntimeData->adapter.portEnable)
            {
                pRuntimeData->adapter.portEnable = portEnable;
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_14:
        {
            uint16_t portLogAnnounceInterval;
            EI_API_ADP_getPortLogAnnounceInterval(pAdapter, &portLogAnnounceInterval);
            if(portLogAnnounceInterval != pRuntimeData->adapter.portLogAnnounceInterval)
            {
                pRuntimeData->adapter.portLogAnnounceInterval = portLogAnnounceInterval;
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_15:
        {
            int16_t portLogSyncInterval;
            EI_API_ADP_getPortLogSyncInterval(pAdapter, &portLogSyncInterval);
            if(portLogSyncInterval != pRuntimeData->adapter.portLogSyncInterval)
            {
                pRuntimeData->adapter.portLogSyncInterval = portLogSyncInterval;
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_18:
        {
            uint8_t domainNumber;

            EI_API_ADP_getDomainNumber(pAdapter, &domainNumber);
            if(domainNumber != pRuntimeData->adapter.domainNumber)
            {
                pRuntimeData->adapter.domainNumber = domainNumber;
                ret = true;
            }
            break;
        }
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_23:
        {
            char aUserDescription[128];
            EI_API_ADP_getTimeSyncUserDescription(pAdapter, aUserDescription);
            if (0 != OSAL_MEMORY_memcmp(aUserDescription, pRuntimeData->adapter.aUserDescription, sizeof(pRuntimeData->adapter.aUserDescription)))
            {
                OSAL_MEMORY_memcpy (pRuntimeData->adapter.aUserDescription, aUserDescription, sizeof(pRuntimeData->adapter.aUserDescription));
                ret = true;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return ret;
}

static uint32_t EI_APP_CFG_setTsync (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    errCode = EI_API_ADP_setPtpEnable(pAdapter, pRuntimeData->adapter.ptpEnable);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setPortEnable(pAdapter, pRuntimeData->adapter.portEnable);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setPortLogAnnounceInterval(pAdapter, pRuntimeData->adapter.portLogAnnounceInterval);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setPortLogSyncInterval(pAdapter, pRuntimeData->adapter.portLogSyncInterval);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setDomainNumber(pAdapter, pRuntimeData->adapter.domainNumber);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setTimeSyncUserDescription(pAdapter, pRuntimeData->adapter.aUserDescription);

laError:
    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return errCode;
}

static bool EI_APP_CFG_getQos (EI_API_ADP_T      *pAdapter,
                               uint16_t           instanceId,
                               uint16_t           attrId,
                               EI_API_CIP_ESc_t   serviceCode,
                               int16_t            serviceFlag)
{
    EI_API_ADP_SQos_t qos;

    bool               ret          = false;
    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();

    EI_API_ADP_getQoS(pAdapter, &qos);

    if(OSAL_MEMORY_memcmp(&pRuntimeData->adapter.qosParameter, &qos, sizeof(EI_API_ADP_SQos_t)) != 0)
    {
        OSAL_MEMORY_memcpy (&pRuntimeData->adapter.qosParameter, &qos, sizeof(EI_API_ADP_SQos_t));
        ret = true;
    }

    return ret;
}

static uint32_t EI_APP_CFG_setQos (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    errCode = EI_API_ADP_setQoS(pAdapter, &pRuntimeData->adapter.qosParameter);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return errCode;
}

static bool EI_APP_CFG_getEthLink (EI_API_ADP_T      *pAdapter,
                                   uint16_t           instanceId,
                                   uint16_t           attrId,
                                   EI_API_CIP_ESc_t   serviceCode,
                                   int16_t            serviceFlag)
{
    bool               ret          = false;
    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();

    switch(instanceId)
    {
        case EI_APP_CIP_INSTANCE_ID_01:
        case EI_APP_CIP_INSTANCE_ID_02:
        {
            EI_API_ADP_UIntfConf_t intfConf;

            EI_API_ADP_getIntfConfig(pAdapter, instanceId - 1, &intfConf);

            if(intfConf.all != pRuntimeData->adapter.intfConfig[instanceId - 1].all)
            {
                pRuntimeData->adapter.intfConfig[instanceId - 1] = intfConf;
                ret = true;
            }
            break;
        }
        default:
        {
            // Nothing has changed.
            break;
        }
    }

    return ret;
}

static uint32_t EI_APP_CFG_setEthLink (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    // Ethernet Link
    errCode = EI_API_ADP_setIntfConfig(pAdapter, 0, pRuntimeData->adapter.intfConfig[0]);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setIntfConfig(pAdapter, 1, pRuntimeData->adapter.intfConfig[1]);

laError:

    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return errCode;
}

static bool EI_APP_CFG_getLldpMng (EI_API_ADP_T      *pAdapter,
                                   uint16_t           instanceId,
                                   uint16_t           attrId,
                                   EI_API_CIP_ESc_t   serviceCode,
                                   int16_t            serviceFlag)
{
    bool               ret          = false;
    EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();

    switch (attrId)
    {
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_01:
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_02:
        case EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03:
        {
            EI_API_ADP_SLldp_Parameter_t lldpParameter;

            EI_API_ADP_getLldpParameter(pAdapter, &lldpParameter);
            if(OSAL_MEMORY_memcmp(&pRuntimeData->adapter.lldpParameter, &lldpParameter, sizeof(EI_API_ADP_SLldp_Parameter_t)) != 0)
            {
                OSAL_MEMORY_memcpy (&pRuntimeData->adapter.lldpParameter, &lldpParameter, sizeof(EI_API_ADP_SLldp_Parameter_t));
                ret = true;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return ret;
}

static uint32_t EI_APP_CFG_setLldpMng (EI_API_ADP_T *pAdapter, EI_APP_CFG_Data_t *pRuntimeData)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    errCode = EI_API_ADP_setLldpParameter(pAdapter, pRuntimeData->adapter.lldpParameter);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return errCode;
}

