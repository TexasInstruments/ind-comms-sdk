/*!
 *  \file device_profile_cfg.h
 *
 *  \brief
 *  Declarations related to configuration of EtherNet/IP adapter.
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

#ifndef DEVICE_PROFILE_CFG_H
#define DEVICE_PROFILE_CFG_H

typedef struct DEVICE_PROFILE_CFG_Header
{
    uint16_t magicNumber;
    uint32_t version;
    uint32_t checksum;
}DEVICE_PROFILE_CFG_Header_t;

typedef struct DEVICE_PROFILE_CFG_AdapterData
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
} DEVICE_PROFILE_CFG_AdapterData_t;

typedef struct DEVICE_PROFILE_CFG_ProfileData
{

}DEVICE_PROFILE_CFG_ProfileData_t;

typedef struct DEVICE_PROFILE_CFG_Data
{
    DEVICE_PROFILE_CFG_Header_t       header;
    DEVICE_PROFILE_CFG_AdapterData_t  adapter;
    DEVICE_PROFILE_CFG_ProfileData_t  profile;
}DEVICE_PROFILE_CFG_Data_t;

/*!
 *  \brief
 *  Initialization parameters of non-volatile configuration data.
 */
typedef struct DEVICE_PROFILE_CFG_SInit
{
    DEVICE_PROFILE_NVM_EType_t      type;             /* Defines type of non-volatile memory FLASH/EEPROM */
    uint32_t               instance;         /* Non-volatile memory driver instance */
    uint32_t               address;          /* Non-volatile data address */
}DEVICE_PROFILE_CFG_SInit_t;

#ifdef __cplusplus
extern "C" {
#endif

extern bool         DEVICE_PROFILE_CFG_init                  (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_CFG_SInit_t* pParam);
extern uint32_t     DEVICE_PROFILE_CFG_apply                 (EI_API_ADP_T *pAdapter);
extern bool         DEVICE_PROFILE_CFG_isValid               (void);
extern bool         DEVICE_PROFILE_CFG_isChanged             (void);
extern void         DEVICE_PROFILE_CFG_setHeader             (DEVICE_PROFILE_CFG_Header_t *pHeader);
extern void         DEVICE_PROFILE_CFG_setDefault            (void);
extern void         DEVICE_PROFILE_CFG_setDefaultWithoutComm (void);
extern uint32_t     DEVICE_PROFILE_CFG_getLength             (void);
extern void*        DEVICE_PROFILE_CFG_getRuntimeData        (void);
extern void*        DEVICE_PROFILE_CFG_getFactoryResetData   (void);
extern const char*  DEVICE_PROFILE_CFG_getOUI                (void);
extern bool         DEVICE_PROFILE_CFG_read                  (void);
extern bool         DEVICE_PROFILE_CFG_write                 (bool blocking);
extern void         DEVICE_PROFILE_CFG_callback              (EI_API_CIP_NODE_T *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag);

#ifdef  __cplusplus
}
#endif

#endif // DEVICE_PROFILE_CFG_H
