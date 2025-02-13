/*!
 *  \file device_profile_cfg.c
 *
 *  \brief
 *  Implementation of device profile configuration feature.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2025 Texas Instruments Incorporated
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

#include <stdbool.h>
#include <stdint.h>

#if defined (SOC_AM243X) || defined (SOC_AM64X)
#include "drivers/pruicss/g_v0/pruicss.h"
#elif defined (SOC_AM263PX) || defined (SOC_AM261X)
#include "drivers/pruicss/m_v0/pruicss.h"
#else
#error "Undefined SOC"
#endif

#include "EI_API.h"
#include "EI_API_def.h"

#include "cfg_example.h"

#include "cust_drivers.h"

#include "drivers/drivers.h"

#include "device_profiles/common/device_profile_cip_codes.h"
#include "device_profiles/common/device_profile_nvm.h"
#include "device_profiles/common/device_profile_cfg.h"
#include "device_profiles/common/device_profile_reset.h"

#include "device_profiles/device_profile_intf.h"


static bool DEVICE_PROFILE_CFG_getTcpip   (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
static bool DEVICE_PROFILE_CFG_getTsync   (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
static bool DEVICE_PROFILE_CFG_getQos     (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
static bool DEVICE_PROFILE_CFG_getEthLink (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);
static bool DEVICE_PROFILE_CFG_getLldpMng (EI_API_ADP_T *pAdapter, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);

static uint32_t DEVICE_PROFILE_CFG_setTcpip   (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData);
static uint32_t DEVICE_PROFILE_CFG_setTsync   (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData);
static uint32_t DEVICE_PROFILE_CFG_setQos     (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData);
static uint32_t DEVICE_PROFILE_CFG_setEthLink (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData);
static uint32_t DEVICE_PROFILE_CFG_setLldpMng (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData);

static EI_API_ADP_T       *DEVICE_PROFILE_CFG_pAdapter_s  = NULL;
static bool                DEVICE_PROFILE_CFG_isChanged_s = false;
static DEVICE_PROFILE_CFG_SInit_t *DEVICE_PROFILE_CFG_pNvConfig_s = {0};
static volatile char DEVICE_PROFILE_CFG_OUI[4] = {0};

/*!
 * \brief
 * Initialization of non-volatile configuration data
 *
 * \param[in]     pAdapter   Pointer to EtherNet/IP Adapter object.
 * \param[in]     pParam     Pointer to initialization parameters.
 *
 * \return        return status as bool.
 * \retval        true            Success.
 * \retval        false           Fail.
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
bool DEVICE_PROFILE_CFG_init (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_SInit_t* pParam)
{
    bool ret = false;

    if (NULL == pParam)
    {
        goto laError;
    }

    DEVICE_PROFILE_CFG_pAdapter_s = pAdapter;

    DEVICE_PROFILE_CFG_pNvConfig_s = pParam;

    ret =  DEVICE_PROFILE_INTF_cfgInit(pAdapter);

laError:

    return ret;
}

bool DEVICE_PROFILE_CFG_isValid(void)
{
    return DEVICE_PROFILE_INTF_cfgIsValid();
}

uint32_t DEVICE_PROFILE_CFG_getLength (void)
{
    return DEVICE_PROFILE_INTF_cfgGetLength();
}

void* DEVICE_PROFILE_CFG_getRuntimeData (void)
{
    return DEVICE_PROFILE_INTF_cfgGetRuntimeData();
}

void* DEVICE_PROFILE_CFG_getFactoryResetData (void)
{
    return DEVICE_PROFILE_INTF_cfgGetFactoryResetData();
}

void DEVICE_PROFILE_CFG_setHeader(DEVICE_PROFILE_CFG_Header_t *pHeader)
{
    return DEVICE_PROFILE_INTF_cfgSetHeader(pHeader);
}

const char*    DEVICE_PROFILE_CFG_getOUI  (void)
{
    const uint32_t mac_address_upper_16_bits = *CFG_BOARD_REG_MAC_ID1;
    const uint32_t mac_address_lower_32_bits = *CFG_BOARD_REG_MAC_ID0;
    DEVICE_PROFILE_CFG_OUI[0] = (char) (mac_address_upper_16_bits >> 8);
    DEVICE_PROFILE_CFG_OUI[1] = (char) (mac_address_upper_16_bits);
    DEVICE_PROFILE_CFG_OUI[2] = (char) (mac_address_lower_32_bits >> 24);
    DEVICE_PROFILE_CFG_OUI[3] = 0;
    return (const char *)DEVICE_PROFILE_CFG_OUI;
}

void DEVICE_PROFILE_CFG_setDefault (void)
{
    DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();
    DEVICE_PROFILE_CFG_Data_t *pResetData   = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetFactoryResetData();
    uint32_t           length       = DEVICE_PROFILE_INTF_cfgGetLength();

    OSAL_MEMORY_memcpy(pRuntimeData, pResetData, length);

    return;
}

void DEVICE_PROFILE_CFG_setDefaultWithoutComm (void)
{
    DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();
    DEVICE_PROFILE_CFG_Data_t *pResetData   = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetFactoryResetData();

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

    DEVICE_PROFILE_INTF_cfgSetDefaultWithoutComm();

    return;
}

uint32_t DEVICE_PROFILE_CFG_apply(EI_API_ADP_T *pAdapter)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();

    DEVICE_PROFILE_INTF_cfgApply(pAdapter);

    // common area settings
    errCode = DEVICE_PROFILE_CFG_setTcpip(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = DEVICE_PROFILE_CFG_setEthLink(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = DEVICE_PROFILE_CFG_setQos(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = DEVICE_PROFILE_CFG_setTsync(pAdapter, pRuntimeData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = DEVICE_PROFILE_CFG_setLldpMng(pAdapter, pRuntimeData);

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

/*!
 *
 *  \brief
 *  Reads non-volatile configuration data.
 *
 *  \details
 *  Reads non-volatile configuration data. If no data are present,
 *  a new structure is created with default data.
 *
 */
bool DEVICE_PROFILE_CFG_read (void)
{
    const void*    pHandle = NULL;
    const uint8_t* pData   = (uint8_t*) DEVICE_PROFILE_CFG_getRuntimeData();
    uint32_t       length  = DEVICE_PROFILE_CFG_getLength();
    uint32_t       err     = OSAL_GENERAL_ERROR;
    bool           ret     = false;

    if ( (DEVICE_PROFILE_NVM_eTYPE_FLASH  == DEVICE_PROFILE_CFG_pNvConfig_s->type) ||
         (DEVICE_PROFILE_NVM_eTYPE_EEPROM == DEVICE_PROFILE_CFG_pNvConfig_s->type) )
    {
        pHandle = DEVICE_PROFILE_NVM_getHandle(DEVICE_PROFILE_CFG_pNvConfig_s->type, DEVICE_PROFILE_CFG_pNvConfig_s->instance);

        if ( (NULL != pHandle) &&
             (NULL != pData)    &&
             (0    != length) )
        {
            err = DEVICE_PROFILE_NVM_read(DEVICE_PROFILE_CFG_pNvConfig_s->type,
                                 DEVICE_PROFILE_CFG_pNvConfig_s->instance,
                                 DEVICE_PROFILE_CFG_pNvConfig_s->address,
                                 pData,
                                 length);
        }
    }
    else
    {
        err = DEVICE_PROFILE_NVM_NO_MEMORY_DEFINED;
    }

    if ( (OSAL_NO_ERROR               != err) &&
         (DEVICE_PROFILE_NVM_NO_MEMORY_DEFINED != err) )
    {
        OSAL_printf ("Non-Volatile configuration data read failed\r\n");
        goto laError;
    }

    ret = DEVICE_PROFILE_CFG_isValid();

    if (false == ret)
    {
        OSAL_printf ("The configuration data is corrupted, write default values.\r\n");
        DEVICE_PROFILE_RESET_execute(1);
    }

    return ret;

    //-------------------------------------------------------------------------------------------------
    laError:

        exit (-1);
}

/*!
 *
 *  \brief
 *  Write non-volatile configuration data.
 *
 *  \details
 *  Write non-volatile configuration data to memory. Operation can be blocking or non-blocking.
 *
 */
bool DEVICE_PROFILE_CFG_write (bool blocking)
{
    DEVICE_PROFILE_CFG_Data_t *pCfgData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_CFG_getRuntimeData();
    uint32_t           length   = DEVICE_PROFILE_CFG_getLength();
    uint32_t           error    = OSAL_GENERAL_ERROR;
    bool               ret      = false;

    if (DEVICE_PROFILE_NVM_eTYPE_UNDEFINED == DEVICE_PROFILE_CFG_pNvConfig_s->type)
    {
        goto laError;
    }

    DEVICE_PROFILE_CFG_setHeader(&pCfgData->header);

    error = DEVICE_PROFILE_NVM_write (DEVICE_PROFILE_CFG_pNvConfig_s->type,
                             DEVICE_PROFILE_CFG_pNvConfig_s->instance,
                             DEVICE_PROFILE_CFG_pNvConfig_s->address,
                             (uint8_t*) pCfgData,
                             length,
                             blocking);

    if (OSAL_NO_ERROR == error)
    {
        ret = true;
    }

laError:

    return ret;
}

bool DEVICE_PROFILE_CFG_isChanged(void)
{
    bool ret = DEVICE_PROFILE_CFG_isChanged_s;

    if (true == DEVICE_PROFILE_CFG_isChanged_s)
    {
        DEVICE_PROFILE_CFG_isChanged_s = false;
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
 *  Callback function for write accesses of several attributes. Saves the new non-volatile configuration data.
 *  Sets new network configuration, if necessary. Sets aHostName, if necessary.
 */
void DEVICE_PROFILE_CFG_callback ( EI_API_CIP_NODE_T *pCipNode,
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
       case DEVICE_PROFILE_CIP_CLASS_ID_TCPIP_INTERFACE:
       {
           DEVICE_PROFILE_CFG_isChanged_s = DEVICE_PROFILE_CFG_getTcpip(DEVICE_PROFILE_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       case DEVICE_PROFILE_CIP_CLASS_ID_TIME_SYNC:
       {
           DEVICE_PROFILE_CFG_isChanged_s = DEVICE_PROFILE_CFG_getTsync(DEVICE_PROFILE_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       case DEVICE_PROFILE_CIP_CLASS_ID_QUALITY_OF_SERVICE:
       {
           DEVICE_PROFILE_CFG_isChanged_s = DEVICE_PROFILE_CFG_getQos(DEVICE_PROFILE_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       case DEVICE_PROFILE_CIP_CLASS_ID_ETHERNET_LINK:
       {
           // Ethernet link object has changed for instance 1 or 2.
           DEVICE_PROFILE_CFG_isChanged_s = DEVICE_PROFILE_CFG_getEthLink(DEVICE_PROFILE_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       case DEVICE_PROFILE_CIP_CLASS_ID_LLDP_MANAGEMENT:
       {
           DEVICE_PROFILE_CFG_isChanged_s = DEVICE_PROFILE_CFG_getLldpMng(DEVICE_PROFILE_CFG_pAdapter_s, instanceId, attrId, serviceCode, serviceFlag);
           break;
       }
       default:
       {
           // Nothing has changed.
           DEVICE_PROFILE_CFG_isChanged_s = false;
           break;
       }
   }

   if (false == DEVICE_PROFILE_CFG_isChanged_s)
   {
       DEVICE_PROFILE_CFG_isChanged_s = DEVICE_PROFILE_INTF_cfgCallback(pCipNode, classId, instanceId, attrId, serviceCode, serviceFlag);
   }
}

static bool DEVICE_PROFILE_CFG_getTcpip (EI_API_ADP_T      *pAdapter,
                                 uint16_t           instanceId,
                                 uint16_t           attrId,
                                 EI_API_CIP_ESc_t   serviceCode,
                                 int16_t            serviceFlag)
{
    bool               ret          = false;
    DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();

    switch(attrId)
    {
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_03:
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_05:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_06:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_08:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_09:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_10:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_11:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_12:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_13:
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

static uint32_t DEVICE_PROFILE_CFG_setTcpip (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData)
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

static bool DEVICE_PROFILE_CFG_getTsync (EI_API_ADP_T      *pAdapter,
                                 uint16_t           instanceId,
                                 uint16_t           attrId,
                                 EI_API_CIP_ESc_t   serviceCode,
                                 int16_t            serviceFlag)
{
    bool ret = false;

    DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();

    switch(attrId)
    {
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_01:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_13:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_14:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_15:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_18:
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
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_23:
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

static uint32_t DEVICE_PROFILE_CFG_setTsync (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData)
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

static bool DEVICE_PROFILE_CFG_getQos (EI_API_ADP_T      *pAdapter,
                               uint16_t           instanceId,
                               uint16_t           attrId,
                               EI_API_CIP_ESc_t   serviceCode,
                               int16_t            serviceFlag)
{
    EI_API_ADP_SQos_t qos;

    bool               ret          = false;
    DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();

    EI_API_ADP_getQoS(pAdapter, &qos);

    if(OSAL_MEMORY_memcmp(&pRuntimeData->adapter.qosParameter, &qos, sizeof(EI_API_ADP_SQos_t)) != 0)
    {
        OSAL_MEMORY_memcpy (&pRuntimeData->adapter.qosParameter, &qos, sizeof(EI_API_ADP_SQos_t));
        ret = true;
    }

    return ret;
}

static uint32_t DEVICE_PROFILE_CFG_setQos (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    errCode = EI_API_ADP_setQoS(pAdapter, &pRuntimeData->adapter.qosParameter);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return errCode;
}

static bool DEVICE_PROFILE_CFG_getEthLink (EI_API_ADP_T      *pAdapter,
                                   uint16_t           instanceId,
                                   uint16_t           attrId,
                                   EI_API_CIP_ESc_t   serviceCode,
                                   int16_t            serviceFlag)
{
    bool               ret          = false;
    DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();

    switch(instanceId)
    {
        case DEVICE_PROFILE_CIP_INSTANCE_ID_01:
        case DEVICE_PROFILE_CIP_INSTANCE_ID_02:
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

static uint32_t DEVICE_PROFILE_CFG_setEthLink (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData)
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

static bool DEVICE_PROFILE_CFG_getLldpMng (EI_API_ADP_T      *pAdapter,
                                   uint16_t           instanceId,
                                   uint16_t           attrId,
                                   EI_API_CIP_ESc_t   serviceCode,
                                   int16_t            serviceFlag)
{
    bool               ret          = false;
    DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();

    switch (attrId)
    {
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_01:
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_02:
        case DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_03:
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

static uint32_t DEVICE_PROFILE_CFG_setLldpMng (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_Data_t *pRuntimeData)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    errCode = EI_API_ADP_setLldpParameter(pAdapter, pRuntimeData->adapter.lldpParameter);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return errCode;
}
