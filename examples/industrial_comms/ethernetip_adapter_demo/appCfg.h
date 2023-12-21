/*!
 *  \file appCfg.h
 *
 *  \brief
 *  Declarations related to configuration of EtherNet/IP adapter.
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

#ifndef APPCFG_H
#define APPCFG_H

typedef struct EI_APP_CFG_Header
{
    uint16_t magicNumber;
    uint32_t version;
    uint32_t checksum;
}EI_APP_CFG_Header_t;

typedef struct EI_APP_CFG_AdapterData
{
    // TCP/IP object 0xF5 attribute 5
    uint32_t ipAddr;                 // IP address
    uint32_t ipNwMask;               // Network mask
    uint32_t ipGateway;              // Gateway address
    uint32_t nameServer1;            // First name server address
    uint32_t nameServer2;            // Second name server address
    char     aDomainName[48];        // Domain name

    // TCP/IP object 0xF5
    uint8_t  configurationMethod;    // attribute 3
    char     aHostName[64];          // Attribute 6
    uint8_t  ttlValue;               // attribute 8
    bool     acdActive;              // Attribute 10, select acd
    uint8_t  acdState;               // Attribute 11, state of acd
    uint8_t  aAcdAddr[6];            // Attribute 11, acd remote mac
    uint8_t  aAcdHdr[28];            // Attribute 11, arp pdu
    uint16_t encapInactTimeout;      // attribute 13

    // Ethernet Link object 0xF6
    EI_API_ADP_UIntfConf_t intfConfig[2];  // attribute 6

    // TimeSync object 0x43
    bool     ptpEnable;
    bool     portEnable;
    uint16_t portLogAnnounceInterval;
    int16_t  portLogSyncInterval;
    uint8_t  domainNumber;
    char     aUserDescription[128];

    // QoS object 0x48
    EI_API_ADP_SQos_t qosParameter;         // QoS Object 0x48

    EI_API_ADP_SMcastConfig_t mcastConfig; // Multicast Configuration 0xF5 Attribute 9

    bool quickConnectEnabled;

    EI_API_ADP_SLldp_Parameter_t lldpParameter;
} EI_APP_CFG_AdapterData_t;

typedef struct EI_APP_CFG_ProfileData
{

}EI_APP_CFG_ProfileData_t;

typedef struct EI_APP_CFG_Data
{
    EI_APP_CFG_Header_t       header;
    EI_APP_CFG_AdapterData_t  adapter;
    EI_APP_CFG_ProfileData_t  profile;
}EI_APP_CFG_Data_t;

#ifdef __cplusplus
extern "C" {
#endif

extern bool     EI_APP_CFG_init                  (EI_API_ADP_T        *pAdapter);
extern uint32_t EI_APP_CFG_apply                 (EI_API_ADP_T        *pAdapter);
extern bool     EI_APP_CFG_isValid               (void);
extern bool     EI_APP_CFG_isChanged             (void);
extern void     EI_APP_CFG_setHeader             (EI_APP_CFG_Header_t *pHeader);
extern void     EI_APP_CFG_setDefault            (void);
extern void     EI_APP_CFG_setDefaultWithoutComm (void);
extern uint32_t EI_APP_CFG_getLength             (void);
extern void*    EI_APP_CFG_getRuntimeData        (void);
extern void*    EI_APP_CFG_getFactoryResetData   (void);
extern void     EI_APP_CFG_callback              (EI_API_CIP_NODE_T   *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);

#ifdef  __cplusplus
}
#endif

#endif // APPCFG_h
