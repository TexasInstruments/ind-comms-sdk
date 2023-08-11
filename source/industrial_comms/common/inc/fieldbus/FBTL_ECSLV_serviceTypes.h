/*!
 *  \file FBTL_ECSLV_serviceTypes.h
 * 
 *  \brief
 *  FBTL services EtherCAT slave.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2023 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#if !(defined PROTECT_FBTL_ECSLV_SERVICETYPES_H)
#define PROTECT_FBTL_ECSLV_SERVICETYPES_H    1

#include <FBTL_service.h>
#include <ecSlvApi.h>

#define FBTL_CREATE_PDO_NAME_LENGTH         (0x80u)
#define FBTL_CREATE_PDO_ENTRYNAME_LENGTH    (0x80u)
#define FBTL_CREATE_SDO_VARNAME_LENGTH      (0x80u)
#define FBTL_CREATE_SDO_ARRNAME_LENGTH      (0x80u)
#define FBTL_CREATE_SDO_RECNAME_LENGTH      (0x80u)
#define FBTL_CREATE_SDO_RECSINAME_LENGTH    (0x80u)
#define FBTL_CREATE_SDO_ENUMNAME_LENGTH     (0x80u)
#define FBTL_COE_EMCY_DATA_LENGTH           (0x3u)
#define FBTL_FOE_FILENAME_LENGTH            (0x200u)

#define FBTL_MAX_PDO_DATA_LENGTH            (0x400u)
#define FBTL_MAX_PDO_DATAENTRY_LENGTH       (0x400u)
#define FBTL_MAX_EOE_FRAMELENGTH            (0x640u)
#define FBTL_MAX_SDO_FRAME_LENGTH           (0x200u)
#define FBTL_MAX_ERRHANDLER_DATA_LENGTH     (0x200u)
#define FBTL_EEPROM_DATA_LENGTH             (0x800u)
#define FBTL_FOE_FILEDATA_LENGTH            (0x640u)
#define FBTL_SOE_DATA_LENGTH                (0x400u)
#define FBTL_COE_OBJECTDATA_LENGTH          (0x400u)
#define FBTL_PDO_MAX_ENTRIES                (0xFFu)
#define FBTL_DIAGNOSIS_MAX_PARAMS           (0x10u)
#define FBTL_DIAGNOSIS_MAX_PARAMS_DATA      (0x400u)

/**
\brief Create PDO

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SCreatePdoHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            pdoMapIndex;                            /*!< PDO mapping index */
    char                aName[FBTL_CREATE_PDO_NAME_LENGTH];     /*!< PDO name */
} FBTL_SVC_ECSLV_SCreatePdoHeader_t;

/**
\brief Create SubEntry of PDO

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SCreatePdoEntryHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint8_t             reserved;                               /*!< reserved */
    uint16_t            pdoMapIndex;                            /*!< PDO mapping index */
    uint16_t            sdoIndex;                               /*!< mapped SDO index */
    uint8_t             sdoSubIndex;                            /*!< mapped SDO subindex */
    char                aName[FBTL_CREATE_PDO_ENTRYNAME_LENGTH];/*!< PDO entry name */
} FBTL_SVC_ECSLV_SCreatePdoEntryHeader_t;

/**
\brief PDO Payload

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SPdoDataHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            pdoMapIndex;                            /*!< PDO mapping index */
    uint16_t            dataLen;                                /*!< PDO data length */
    uint8_t             aData[FBTL_MAX_PDO_DATA_LENGTH];        /*!< PDO data */
} FBTL_SVC_ECSLV_SPdoDataHeader_t;

/**
\brief PDO Entry Payload

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SPdoEntryDataHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            pdoMapIndex;                            /*!< PDO mapping index */
    uint8_t             pdoMapSubIndex;                         /*!< PDO mapping subindex */
    uint8_t             reserved;                               /*!< reserved */
    uint16_t            dataLen;                                /*!< PDO entry data length */
    uint8_t             aData[FBTL_MAX_PDO_DATAENTRY_LENGTH];   /*!< PDO entry data */
} FBTL_SVC_ECSLV_SPdoEntryDataHeader_t;

/**
\brief Create Variable SDO

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SCreateVariableSdoHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            index;                                  /*!< SDO variable index */
    uint16_t            type;                                   /*!< datatype */
    uint16_t            bitLen;                                 /*!< entrysize in bits */
    uint16_t            flags;                                  /*!< entry flags */
    uint8_t             readCallback;                           /*!< 1: assign read callback, 0: do not */
    uint8_t             writeCallback;                          /*!< 1: assign write callback, 0: do not */
    char                aName[FBTL_CREATE_SDO_VARNAME_LENGTH];  /*!< SDO variable name */
} FBTL_SVC_ECSLV_SCreateVariableSdoHeader_t;

/**
\brief Create Array SDO

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SCreateArraySdoHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            index;                                  /*!< SDO array index */
    uint16_t            type;                                   /*!< datatype */
    uint16_t            bitLen;                                 /*!< arraysize in bits */
    uint16_t            flags;                                  /*!< array flags */
    uint8_t             readCallback;                           /*!< 1: assign read callback, 0: do not */
    uint8_t             writeCallback;                          /*!< 1: assign write callback, 0: do not */
    uint8_t             arrayLen;                               /*!< length of array */
    char                aName[FBTL_CREATE_SDO_ARRNAME_LENGTH];  /*!< array name */
} FBTL_SVC_ECSLV_SCreateArraySdoHeader_t;

/**
\brief Create Record SDO

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SCreateRecordSdoHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            index;                                  /*!< SDO record index */
    uint8_t             readCallback;                           /*!< 1: assign read callback, 0: do not */
    uint8_t             writeCallback;                          /*!< 1: assign write callback, 0: do not */
    char                aName[FBTL_CREATE_SDO_RECNAME_LENGTH];  /*!< record name */
} FBTL_SVC_ECSLV_SCreateRecordSdoHeader_t;

/**
\brief Create Record Subindex

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SConfigRecordSubIndexHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            index;                                  /*!< SDO record index */
    uint8_t             subIndex;                               /*!< SDO record subindex */
    uint16_t            type;                                   /*!< datatype */
    uint16_t            bitLen;                                 /*!< recordsize in bits */
    uint16_t            flags;                                  /*!< record flags */
    char                aName[FBTL_CREATE_SDO_RECSINAME_LENGTH];/*!< name of record element */
} FBTL_SVC_ECSLV_SConfigRecordSubIndexHeader_t;

/**
\brief EOE Payload

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SEOEFrameHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            length;                                 /*!< length of EoE data */
    uint8_t             aData[FBTL_MAX_EOE_FRAMELENGTH];        /*!< EoE frame data */
} FBTL_SVC_ECSLV_SEOEFrameHeader_t;
#define FBTL_SVC_ECSLV_EoEFrame_Size(x) \
(sizeof(FBTL_SVC_ECSLV_SEOEFrameHeader_t) - (FBTL_MAX_EOE_FRAMELENGTH) + (x))

/**
\brief Send Emergency

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SEmergencyHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            code;                                   /*!< CoE emergency code */
    uint16_t            aData[FBTL_COE_EMCY_DATA_LENGTH];       /*!< CoE emergency data */
} FBTL_SVC_ECSLV_SEmergencyHeader_t;

/**
\brief SDO Payload

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SSDOFrameHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            index;                                  /*!< SDO frame index */
    uint8_t             subindex;                               /*!< SDO fram subindex */
    uint8_t             completeAccess;                         /*!< 1: SDO complete access, 0: single item */
    uint16_t            dataLen;                                /*!< length of data */
    uint16_t            aData[FBTL_MAX_SDO_FRAME_LENGTH];       /*!< SDO fram data */
} FBTL_SVC_ECSLV_SSDOFrameHeader_t;
#define FBTL_SVC_ECSLV_SdoMsg_Size(x) \
    (sizeof(FBTL_SVC_ECSLV_SSDOFrameHeader_t) - FBTL_MAX_SDO_FRAME_LENGTH + x)

/**
\brief EtherCAT Stats

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SStatsHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            stationAddress;                         /*!< Address used for node addressing*/
    uint16_t            stationAlias;                           /*!< Alias Address used for node addressing*/
    uint8_t             loopPort0;                              /*!< Loop open means sending and receiving is enabled*/
    uint8_t             loopPort1;                              /*!< Loop open means sending and receiving is enabled*/
    uint8_t             physicalLinkPort0;                      /*!< Port 0 Link Activity*/
    uint8_t             physicalLinkPort1;                      /*!< Port 1 Link Activity*/
    uint8_t             invFrameCountPort0;                     /*!< Port 0 Invalid frame Counter*/
    uint8_t             invFrameCountPort1;                     /*!< Port 1 Invalid frame Counter*/
    uint8_t             rxErrCountPort0;                        /*!< Port 0 Rx Error Counter*/
    uint8_t             rxErrCountPort1;                        /*!< Port 1 Rx Error Counter*/
    uint8_t             fwdErrCountPort0;                       /*!< Port 0 Forward error Counter*/
    uint8_t             fwdErrCountPort1;                       /*!< Port 1 Forward error Counter*/
    uint8_t             procUnitErrCounter;                     /*!< Processing Unit error Counter*/
    uint8_t             lostLinkErrCounterPort0;                /*!< Lost link Counter*/
    uint8_t             lostLinkErrCounterPort1;                /*!< Lost link Counter*/
} FBTL_SVC_ECSLV_SStatsHeader_t;

/**
\brief Write ESC Register (Byte)

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SWriteESCByteHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            addr;                                   /*!< ESC access address */
    uint8_t             data;                                   /*!< ESC data */
} FBTL_SVC_ECSLV_SWriteESCByteHeader_t;

/**
\brief Write ESC Register (Word)

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SWriteESCWordHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            addr;                                   /*!< ESC access address */
    uint16_t            data;                                   /*!< ESC data */
} FBTL_SVC_ECSLV_SWriteESCWordHeader_t;

/**
\brief Write ESC Register (DWord)

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SWriteESCDwordHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            addr;                                   /*!< ESC access address */
    uint32_t            data;                                   /*!< ESC data */
} FBTL_SVC_ECSLV_SWriteESCDwordHeader_t;

/**
\brief Error Handler Callback data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SErrorHandlerCBHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint32_t            errorCode;                              /*!< error code */
    uint8_t             fatal;                                  /*!< 1: is fatal error, 0: is non fatal */
    uint8_t             aData[FBTL_MAX_ERRHANDLER_DATA_LENGTH]; /*!< error handler data */
} FBTL_SVC_ECSLV_SErrorHandlerCBHeader_t;

/**
\brief Eeprom Load Callback data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SEepromLoadCBHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint32_t            length;                                 /*!< length of EEPROM data */
    uint8_t             aData[FBTL_EEPROM_DATA_LENGTH];         /*!< EEPROM data */
} FBTL_SVC_ECSLV_SEepromLoadCBHeader_t;

/**
\brief Eeprom Write Callback data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SEepromWriteCBHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint32_t            length;                                 /*!< length of EEPROM data */
    uint8_t             aData[FBTL_EEPROM_DATA_LENGTH];         /*!< EEPROM data */
} FBTL_SVC_ECSLV_SEepromWriteCBHeader_t;

/**
\brief Set Pending Error Option Code Data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SMsgClearErrorCodeHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint8_t             axisNo;                                 /*!< axis number to clear error from */
} FBTL_SVC_ECSLV_SMsgClearErrorCodeHeader_t;

/**
\brief EoE Settings Indicator Data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SEoESettingsIndicatorHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            aMac[3];                                /*!< EoE MAC address */
    uint32_t            ip;                                     /*!< EoE IP address */
    uint32_t            subNet;                                 /*!< EoE subnet mask */
    uint32_t            defaultGateway;                         /*!< EoE default gateway IP */
    uint32_t            dnsIp;                                  /*!< EoE name server IP */
} FBTL_SVC_ECSLV_SEoESettingsIndicatorHeader_t;

/**
\brief FoE Open File Data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SFoEOpenFileHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    char                aName[FBTL_FOE_FILENAME_LENGTH];        /*!< name of FoE file */
    uint16_t            nameLen;                                /*!< length of FoE filename */
    uint8_t             isRead;                                 /*!< 1: read access, 0: write access */
    uint32_t            password;                               /*!< FoE access passcode */
} FBTL_SVC_ECSLV_SFoEOpenFileHeader_t;

/**
\brief FoE File Content Data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SFoEFileData
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint8_t             aData[FBTL_FOE_FILEDATA_LENGTH];        /*!< FoE file datra length */
    uint16_t            blockSize;                              /*!< FoE block size */
    uint32_t            offset;                                 /*!< FoE block offset */
} FBTL_SVC_ECSLV_SFoEFileData_t;

/**
\brief FoE Write File Data

\ingroup grp_ec_serviceTypes
        */
typedef struct FBTL_SVC_ECSLV_SFoEWriteFile
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint8_t             aData[FBTL_FOE_FILEDATA_LENGTH];        /*!< FoE file data block */
    uint16_t            size;                                   /*!< size of FoE data block */
} FBTL_SVC_ECSLV_SFoEWriteFile_t;

/**
\brief Param Set Mailbox Data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SSetMailboxHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            rxOffset;                               /*!< receive mailbox offset */
    uint16_t            rxSize;                                 /*!< receive mailbox size */
    uint16_t            txOffset;                               /*!< transmit mailbox offset */
    uint16_t            txSize;                                 /*!< transmit mailbox size */
} FBTL_SVC_ECSLV_SSetMailboxHeader_t;

/**
\brief Param Set Mailbox Data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SSetSyncManConfigHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint8_t             syncManIdx;                             /*!< sync manager index */
    uint16_t            offset;                                 /*!< sync manager memory offset */
    uint16_t            size;                                   /*!< sync manager memory size */
    uint8_t             control;                                /*!< sync manager control code */
    uint8_t             enable;                                 /*!< 1: enable syncmanager, 0: disable */
} FBTL_SVC_ECSLV_SSetSyncManConfigHeader_t;

/**
\brief SoE SendData

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SSoEDataHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            soeService;                             /*!< SoE service code */
    uint16_t            soeFlags;                               /*!< SoE flags */
    uint16_t            size;                                   /*!< SoE data size */
    uint8_t             aData[FBTL_SOE_DATA_LENGTH];            /*!< SoE data */
} FBTL_SVC_ECSLV_SSoEDataHeader_t;

/**
\brief SoE Notificataion Response

\ingroup grp_ec_serviceTypes
        */
typedef struct FBTL_SVC_ECSLV_SSoENotificationHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            idn;                                    /*!< SoE ident */
    uint16_t            channel;                                /*!< SoE channel */
    uint16_t            dataState;                              /*!< data state */
} FBTL_SVC_ECSLV_SSoENotificationHeader_t;

/**
\brief CoE Object Entry data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SCoEObjectDataHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            index;                                  /*!< CoE object index */
    uint8_t             subindex;                               /*!< CoE object subindex */
    uint8_t             completeAccess;                         /*!< CoE object access mode */
    uint16_t            dataLen;                                /*!< length of data */
    uint8_t             aData[FBTL_COE_OBJECTDATA_LENGTH];      /*!< data */
} FBTL_SVC_ECSLV_SCoEObjectDataHeader_t;

/**
\brief CoE Object SDO callback.
\details This data structure is transferred within a SDO frame.

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SCoECallback
{
    uint16_t    index;                                          /*!< SDO index */
    uint8_t     subIndex;                                       /*!< SDO subindex */
    uint8_t     completeAccess;                                 /*!< 1: complete access, 0: single access */
    uint8_t     status;                                         /*!< state */
} FBTL_SVC_ECSLV_SCoECallback_t;

/**
\brief AoE Object data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SAoEObjectDataHeader
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            port;                                   /*!< port number */
    uint16_t            index;                                  /*!< CoE object index */
    uint8_t             subindex;                               /*!< CoE object subindex */
    uint8_t             completeAccess;                         /*!< CoE object access mode */
    uint16_t            dataLen;                                /*!< length of data */
    uint8_t             aData[FBTL_COE_OBJECTDATA_LENGTH];      /*!< data */
} FBTL_SVC_ECSLV_SAoEObjectDataHeader_t;

/**
\brief PDO assignment data

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SPdoAssignment
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint8_t             rx;                                     /*!< RxPDOs or TxPDOs */
    uint8_t             count;                                  /*!< Number of PDOs */
    uint16_t            aData[FBTL_PDO_MAX_ENTRIES];            /*!< Array of PDOs */
} FBTL_SVC_ECSLV_SPdoAssignment_t;
#define FBTL_SVC_ECSLV_PdoAssignMsg_Size(x) \
    (sizeof(FBTL_SVC_ECSLV_SPdoAssignment_t) - (sizeof(uint16_t) * FBTL_PDO_MAX_ENTRIES) + (sizeof(uint16_t) * x))

/**
\brief New PDO Mapping configuration

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SMsgPdoMapping
{
    FBTL_SVC_SHeader_t              header;                         /*!< generic service header */
    uint16_t                        index;                          /*!< PDO index */
    uint8_t                         count;                          /*!< PDO entries*/
    EC_SLV_API_PDO_SEntryMap_t      aData[FBTL_PDO_MAX_ENTRIES];    /*!< mapped SDO array */
} FBTL_SVC_ECSLV_SMsgPdoMapping_t;

#define FBTL_SVC_ECSLV_PdoMapMsg_Size(x) \
    (sizeof(FBTL_SVC_ECSLV_SMsgPdoMapping_t) - (sizeof(EC_SLV_API_PDO_SEntryMap_t) * FBTL_PDO_MAX_ENTRIES) \
    + (sizeof(EC_SLV_API_PDO_SEntryMap_t) * x))

/**
\brief New PDO Mapping configuration

\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SMsgDiagMessage
{
    FBTL_SVC_SHeader_t              header;                                         /*!< generic service header */
    uint32_t                        diagCode;                                       /*!< Diagnosis code */
    uint8_t                         type;                                           /*!< Diagnosis message type */
    uint16_t                        textID;                                         /*!< Diagnosis textID */
    uint8_t                         numParams;                                      /*!< Count diagnosis parameter */
    EC_API_SLV_SDIAG_parameter_t    params[FBTL_DIAGNOSIS_MAX_PARAMS];              /*!< Diagnosis parameter array */
    uint8_t                         paramData[FBTL_DIAGNOSIS_MAX_PARAMS_DATA];      /*!< Count diagnosis parameter */
} FBTL_SVC_ECSLV_SMsgDiagMessage_t;
#define FBTL_SVC_ECSLV_DiagnosisMsg_Size(x) \
    (sizeof(FBTL_SVC_ECSLV_SMsgDiagMessage_t)  \
    - (sizeof(uint8_t) * FBTL_DIAGNOSIS_MAX_PARAMS_DATA) + (sizeof(uint8_t) * x))

/**
\brief Create ENUM subentry
\ingroup grp_ec_serviceTypes
*/
typedef struct FBTL_SVC_ECSLV_SMsgConfigEnum
{
    FBTL_SVC_SHeader_t  header;                                 /*!< generic service header */
    uint16_t            index;                                  /*!< Enum object index number */
    uint32_t            value;                                  /*!< Enum value */
    char                name[FBTL_CREATE_SDO_ENUMNAME_LENGTH];  /*!< Enum name */
} FBTL_SVC_ECSLV_SMsgConfigEnum;
#define FBTL_SVC_ECSLV_ConfigEnum_Size(x) \
    ((sizeof(FBTL_SVC_SHeader_t) + sizeof(uint16_t) + sizeof(uint32_t) + x + sizeof(char)))


#if (defined __cplusplus)
extern "C" {
#endif

//extern void xyz(void);

#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_FBTL_ECSLV_SERVICETYPES_H */
