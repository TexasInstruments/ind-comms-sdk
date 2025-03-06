/*!
 *  \file pn_api_iod_types.h
 *
 *  \brief
 *  Common defines, enums and structs.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#ifndef PN_API_IOD_TYPES_H
#define PN_API_IOD_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "pn_app_iod_cfg.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*! \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 * @{
 */
#define PN_API_IOD_MAX_DEV_TYPE_LEN   240   /*!< Must be lower than the maximum station type
                                                 length allowed by DCP (255) */
#define PN_API_IOD_IM0_ORDER_ID_LEN   20    /*!< Length of IM0 order ID */
#define PN_API_IOD_IM0_SERIAL_NUM_LEN 16    /*!< Length of IM0 serial number */
#define PN_API_IOD_SUPPORT_IM1234     0x1e  /*!< IM supported, IM1...4 */
#define PN_API_IOD_SUPPORT_IM12345    0x3e  /*!< IM supported, IM1...5 */
#define PN_API_IOD_SNMP_SYS_DESC_LEN  0xff  /*!< Length of system description */

#define PN_API_IOD_SNMP_SYSNAME_LEN   255  /*!< according to RFC1213, do not change */
#define PN_API_IOD_SNMP_SYSLOC_LEN    255  /*!< according to RFC1213, do not change */
#define PN_API_IOD_SNMP_SYSCONT_LEN   255  /*!< according to RFC1213, do not change */
#define PN_API_IOD_DEVICE_NAME_LEN    256  /*!< max. length of Device Name */
#define PN_API_IOD_ARFSU_LEN          20   /*!< 4B Mode + 16B UUID */
#define PN_API_IOD_PDEV_LEN           1024 /*!< PDEV Record length */

#define PN_API_IOD_IOXS               uint8_t   /*!< IO provider/consumer status */
#define PN_API_IOD_IO_PROP_ENUM       uint8_t   /*!< Shouldn't be an ENUM, as bitwise combinations are used */
#define PN_API_IOD_IM0_SUPP_ENUM      uint8_t   /*!< Shouldn't be an ENUM, as bitwise combinations are used */
/** @}*/

/*!
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef void*   PN_API_IOD_IsoObjHndl_t;        /*!< Iso object handle */

/*!
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct PN_API_IOD_Handle PN_API_IOD_Handle_t; /*!< API handle instance which holds IO device's info */

/* ENUMS */
/*!
 * \brief Type of IM0 support.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_Im0Nothing   = 0x00, /*!< Submodule does not contain I&M data */
    PN_API_IOD_Im0Submodule = 0x01, /*!< Submodule contains I&M data */
    PN_API_IOD_Im0Module    = 0x02, /*!< Submodule stands for the module (virtual submodule interface) */
    PN_API_IOD_Im0Device    = 0x04  /*!< Submodule stands for the device (DAP) */
} PN_API_IOD_Im0Support_t;

/*!
 * \brief Type of remanent data.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_RemaDataMacAddr,     /*!< IO device's MAC address */
    PN_API_IOD_RemaDataDevName,     /*!< IO device's name */
    PN_API_IOD_RemaDataIpSuite,     /*!< IP suite (IP address, subnet mask, default router)
                                         in network format (big endian) */
    PN_API_IOD_RemaDataPdevRecord,  /*!< PDev records */
    PN_API_IOD_RemaDataIm1,         /*!< I&M1 data */
    PN_API_IOD_RemaDataIm2,         /*!< I&M2 data */
    PN_API_IOD_RemaDataIm3,         /*!< I&M3 data */
    PN_API_IOD_RemaDataIm4,         /*!< I&M4 data */
    PN_API_IOD_RemaDataSnmpSysName, /*!< SNMP MIB2 object "system name" */
    PN_API_IOD_RemaDataSnmpSysCont, /*!< SNMP MIB2 object "system contact" */
    PN_API_IOD_RemaDataSnmpSysLoc,  /*!< SNMP MIB2 object "system location" */
    PN_API_IOD_RemaDataPrivateData, /*!< Own remanent data */
    PN_API_IOD_RemaDataArFsu,       /*!< UUID for AR-FSU functionality */
    PN_API_IOD_RemaDataSubmodCfg,   /*!< Submodule configurations */
    PN_API_IOD_RemaDataVendorParam  /*!< Vendor parameter */
} PN_API_IOD_RemaDataType_t;

/*!
 * \brief Type for LEDs
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_LedRun = 1, /*!< Run */
    PN_API_IOD_LedMaint,   /*!< Maintenance */
    PN_API_IOD_LedError,   /*!< Error */
    PN_API_IOD_LedSync,    /*!< Sync */
    PN_API_IOD_LedLinkP1,  /*!< Link Port 1 */
    PN_API_IOD_LedLinkP2,  /*!< Link Port 2 */
    PN_API_IOD_LedLinkP3,  /*!< Link Port 3 */
    PN_API_IOD_LedLinkP4,  /*!< Link Port 4 */
    PN_API_IOD_LedActivP1, /*!< Link Activ Port 1 */
    PN_API_IOD_LedActivP2, /*!< Link Activ Port 2 */
    PN_API_IOD_LedActivP3, /*!< Link Activ Port 3 */
    PN_API_IOD_LedActivP4, /*!< Link Activ Port 4 */
    PN_API_IOD_LedFo1,     /*!< Fiberoptic 1 */
    PN_API_IOD_LedFo2,     /*!< Fiberoptic 2 */
    PN_API_IOD_LedPower,   /*!< Power */
    PN_API_IOD_LedDiag,    /*!< Diag */
    PN_API_IOD_LedPenergy, /*!< Profi Energy */
    PN_API_IOD_LedBlink,   /*!< Blink */
    PN_API_IOD_LedUser00,  /*!< User 0 */
    PN_API_IOD_LedUser01,  /*!< User 1 */
    PN_API_IOD_LedUser02,  /*!< User 2 */
    PN_API_IOD_LedUser03,  /*!< User 3 */
    PN_API_IOD_LedUser04,  /*!< User 4 */
    PN_API_IOD_LedUser05,  /*!< User 5 */
    PN_API_IOD_LedUser06,  /*!< User 6 */
    PN_API_IOD_LedUser07,  /*!< User 7 */
    PN_API_IOD_LedUser08,  /*!< User 8 */
    PN_API_IOD_LedUser09,  /*!< User 9 */
    PN_API_IOD_LedUser10,  /*!< User 10 */
    PN_API_IOD_LedUser11,  /*!< User 11 */
    PN_API_IOD_LedUser12,  /*!< User 12 */
    PN_API_IOD_LedUser13,  /*!< User 13 */
    PN_API_IOD_LedUser14,  /*!< User 14 */
    PN_API_IOD_LedUser15,  /*!< User 15 */

    PN_API_IOD_LedMaxLeds /*!< Number of LEDs */
} PN_API_IOD_Led_t;

/*!
 * \brief Address type.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_AddrLog  = 0, /*!< Logical address (not supported) */
    PN_API_IOD_AddrGeo  = 1, /*!< Geographic address (slot number, subslot number) */
    PN_API_IOD_AddrHndl = 2  /*!< Handle address (slot handle, subslot handle) for faster access than geographic */
} PN_API_IOD_AddrType_t;

/*!
 * \brief Alarm type.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_AlarmProc = 1,           /*!< Process alarm (0 is not used) */
    PN_API_IOD_AlarmStatus,             /*!< Status alarm */
    PN_API_IOD_AlarmChangeDiag,         /*!< Channel diagnostic alarm */
    PN_API_IOD_AlarmExtChangeDiag,      /*!< Extended channel diagnostic alarm */
    PN_API_IOD_AlarmGenerateDiag,       /*!< Generic alarm */
    PN_API_IOD_AlarmReturnOfSubmod,     /*!< Return of submodule alarm */
    PN_API_IOD_AlarmUploadRetrieval,    /*!< Upload/Retrieval alarm */
    PN_API_IOD_AlarmNumOfEntries = 7    /*!< Must be last element in this enum struct */
} PN_API_IOD_AlarmType_t;

/*!
 * \brief Input update state.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_ArStartup = 0x0001, /*!< Ready for input update comes during AR startup */
    PN_API_IOD_ArInData  = 0x0002  /*!< Ready for input update comes after AR startup (during
                                         indata) when a submodule is replugged successfully */
} PN_API_IOD_InputUpdateState_t;

/*!
 * \brief Submodule state (currently not in use).
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_StateNoSubmod,           /*!< (currently not in use) */
    PN_API_IOD_StateWrongSubmod,        /*!< (currently not in use) */
    PN_API_IOD_StateSubstitutedSubmod,  /*!< (currently not in use) */
    PN_API_IOD_StateProperSubmod        /*!< (currently not in use) */
} PN_API_IOD_SubmodState_t;

/*!
 * \brief Submodule status.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_SubmodStatusUnknown         = 0, /*!< Reserved, do not use! */
    PN_API_IOD_SubmodStatusStop            = 1, /*!< Submodule is in "stop" state */
    PN_API_IOD_SubmodStatusRun             = 2, /*!< Submodule is in "run" state */
    PN_API_IOD_SubmodStatusAppReadyFollows = 3  /*!< "Application ready" follows later */
} PN_API_IOD_SubmodStatus_t;

/*!
 * \brief Real Submodule status.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_RealSubmodStatusStop            = 0, /*!< Submodule is in "stop" state */
    PN_API_IOD_RealSubmodStatusRun             = 1, /*!< Submodule is in "run" state */
} PN_API_IOD_RealSubmodStatus_t;

/*!
 * \brief Submodule data direction.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_SubmodPropNoData = 0x00, /*!< Submodule with no IO data */
    PN_API_IOD_SubmodPropIn     = 0x01, /*!< Submodule with input data */
    PN_API_IOD_SubmodPropOut    = 0x02, /*!< Submodule with output data */
    PN_API_IOD_SubmodPropIO     = 0x03  /*!< Submodule with input and output data */
} PN_API_IOD_SubmodProp_t;

/*!
 * \brief AR type.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_ArTypeSingle = 0x0001,              /*!< IO controller single AR */
    PN_API_IOD_ArTypeSupervisor = 0x0006,          /*!< IO supervisor single AR */
    PN_API_IOD_ArTypeSingleRtc3 = 0x0010,          /*!< IO controller single AR using RT_CLASS_3 */
    PN_API_IOD_ArTypeSingleSysRedundancy = 0x0020, /*!< IO controller system redundancy AR, uses system
                                                        system redundancy or configures it during runtime */
    PN_API_IOD_ArTypeReserved                      /*!< Reserved for future use */
} PN_API_IOD_ArType_t;

/*!
 * \brief AR disconnection reason.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_ArReasonNone           = 0,  /*!< None */
    PN_API_IOD_ArReason1              = 1,  /*!< Reserved */
    PN_API_IOD_ArReason2              = 2,  /*!< Reserved */
    PN_API_IOD_ArReasonMem            = 3,  /*!< Out of memory */
    PN_API_IOD_ArReasonFrame          = 4,  /*!< Add provider or consumer failed */
    PN_API_IOD_ArReasonMiss           = 5,  /*!< Miss (consumer) */
    PN_API_IOD_ArReasonTimer          = 6,  /*!< CMI timeout */
    PN_API_IOD_ArReasonAlarm          = 7,  /*!< Alarm-open failed */
    PN_API_IOD_ArReasonAlarmSend      = 8,  /*!< Alarm-send.cnf(-) */
    PN_API_IOD_ArReasonAlarmAck       = 9,  /*!< Alarm-ack-send.cnf(-) */
    PN_API_IOD_ArReasonAlarmLen       = 10, /*!< Alarm-data too long */
    PN_API_IOD_ArReasonAlarmInd       = 11, /*!< Alarm.ind(err) */
    PN_API_IOD_ArReasonRpc            = 12, /*!< RPC client call.cnf(-) */
    PN_API_IOD_ArReasonAbort          = 13, /*!< AR abort.req */
    PN_API_IOD_ArReasonReRun          = 14, /*!< Re-run aborts existing */
    PN_API_IOD_ArReasonRelease        = 15, /*!< Got release.ind */
    PN_API_IOD_ArReasonDevPassivated  = 16, /*!< Device passivated */
    PN_API_IOD_ArReasonDevRemoved     = 17, /*!< Device/AR removed */
    PN_API_IOD_ArReasonProtocol       = 18, /*!< Protocol violation */
    PN_API_IOD_ArReasonNare           = 19, /*!< NARE error */
    PN_API_IOD_ArReasonRpcBind        = 20, /*!< RPC-bind error */
    PN_API_IOD_ArReasonRpcConnect     = 21, /*!< RPC-connect error */
    PN_API_IOD_ArReasonRpcRead        = 22, /*!< RPC-read error */
    PN_API_IOD_ArReasonRpcWrite       = 23, /*!< RPC-write error */
    PN_API_IOD_ArReasonRpcControl     = 24, /*!< RPC-control error */
    PN_API_IOD_ArReason25             = 25, /*!< Reserved (formerly: pull or plug in forbidden window) */
    PN_API_IOD_ArReason26             = 26, /*!< Reserved (formerly: AP removed) */
    PN_API_IOD_ArReasonLinkDown       = 27, /*!< Link "down", for local purpose only */
    PN_API_IOD_ArReasonMulticastMac   = 28, /*!< Could not register multicast MAC */
    PN_API_IOD_ArReasonSync           = 29, /*!< Not synchronized (cannot start companion AR) */
    PN_API_IOD_ArReasonTopology       = 30, /*!< Wrong topology (cannot start companion AR) */
    PN_API_IOD_ArReasonDcpStationName = 31, /*!< DCP, station name changed */
    PN_API_IOD_ArReasonDcpReset       = 32, /*!< DCP, reset to factory settings */
    PN_API_IOD_ArReason33             = 33, /*!< Reserved (formerly: cannot start companion AR) */
    PN_API_IOD_ArReasonIrData         = 34, /*!< No irdata record yet */
    PN_API_IOD_ArReasonPDev           = 35, /*!< Ownership of physical device */
    PN_API_IOD_ArReasonLinkMod        = 36, /*!< Link mode not full duplex */
    PN_API_IOD_ArReasonIpSuite        = 37, /*!< IP-suite (of the IOC) changed by means of DCP-set () or local */
    PN_API_IOD_ArReasonRdht           = 38, /*!< IO controller's system redundancy AR RDHT expired */
    PN_API_IOD_ArReasonPDevParam      = 39, /*!< IO controller's system redundancy AR PDev parametrization impossible */
    PN_API_IOD_ArReasonAppReady       = 40, /*!< Remote application timeout expired. */
    PN_API_IOD_ArReasonMax                  /*!< Maximum value (not used) */
} PN_API_IOD_ArReason_t;

/*!
 * \brief Reset To Factory option.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_RtfResetAll            = 0x0000, /*!< Reset all device data */
    PN_API_IOD_RtfResetAppParam       = 0x0001, /*!< Reset application data for one interface */
    PN_API_IOD_RtfResetCommParam      = 0x0002, /*!< Reset communication data for one interface */
    PN_API_IOD_RtfResetEngParam       = 0x0003, /*!< Reset engineering data for one interface */
    PN_API_IOD_RtfResetFwUpgradeParam = 0x0004  /*!< Reset firmware upgrade parameters */
} PN_API_IOD_RtfOption_t;

/*!
 * \brief Submodule real plug status.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_RpsNotPlugged = 0, /*!< Submodule not plugged */
    PN_API_IOD_RpsPlugged    = 1  /*!< Submodule plugged */
} PN_API_IOD_RealPlugState_t;

/*!
 * \brief Submodule real status.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_RssStop = 0, /*!< Submodule is in "stop" state and has invalid data */
    PN_API_IOD_RssRun  = 1  /*!< Submodule is in "run" state and has valid data */
} PN_API_IOD_RealSubmodState_t;

/*!
 * \brief Application Protocol Data Unit status.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum {
    PN_API_IOD_ApduStatusPrimary = 0x0001,      /*!< Bit=1: primary, 0: backup */
    PN_API_IOD_ApduStatusRedundancy = 0x0002,   /*!< Bit=1: primary AR, 0: backup AR */
    PN_API_IOD_ApduStatusDataValid = 0x0004,    /*!< Bit=1: data valid, 0: data invalid */
    PN_API_IOD_ApduStatusRun = 0x0010,          /*!< Bit=1: state run, 0: state stop */
    PN_API_IOD_ApduStatusStationOk = 0x0020     /*!< Bit=1: OK, 0: problem */
} PN_API_IOD_ApduStatusByteMask_t;

/*!
 * \brief Defines for channel properties.direction.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_DiagChannelPropDirectionManuSpec  =   0,    /*!< Manufacturer specific */
    PN_API_IOD_DiagChannelPropDirectionIn        =   0x01, /*!< Input */
    PN_API_IOD_DiagChannelPropDirectionOut       =   0x02, /*!< Output */
    PN_API_IOD_DiagChannelPropDirectionInOut     =   0x03  /*!< Input output */
} PN_API_IOD_DiagChannelPropDirection_t;

/*!
 * \brief Defines for channel properties.type.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_DiagChannelPropTypeOthers   =   0,  /*!< Any other data type */
    PN_API_IOD_DiagChannelPropType1Bit     =   1,  /*!< Data type 1 Bit */
    PN_API_IOD_DiagChannelPropType2Bit     =   2,  /*!< Data type 2 Bit */
    PN_API_IOD_DiagChannelPropType4Bit     =   3,  /*!< Data type 4 Bit */
    PN_API_IOD_DiagChannelPropTypeByte     =   4,  /*!< Data type 8 Bit */
    PN_API_IOD_DiagChannelPropTypeWord     =   5,  /*!< Data type 16 Bit */
    PN_API_IOD_DiagChannelPropTypeDWord    =   6,  /*!< Data type 32 Bit */
    PN_API_IOD_DiagChannelPropTypeLWord    =   7   /*!< Data type 64 Bit */
} PN_API_IOD_DiagChannelPropType_t;

/*!
 * \brief Alarm status.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_DiagChannelPropSpecErrAppear           = 1, /*!< New error appears */
    PN_API_IOD_DiagChannelPropSpecErrDisappear        = 2, /*!< Error disappears, no more error */
    PN_API_IOD_DiagChannelPropSpecErrDisappearMore    = 3  /*!< Error disappears, but other errors remain */
} PN_API_IOD_DiagChannelPropSpecErr_t;

/*!
 * \brief IO direcition.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef enum
{
    PN_API_IOD_IoCrTypeIn       = 1, /*!< IO direction is input */
    PN_API_IOD_IoCrTypeOut      = 2  /*!< IO direction is output */
    // reserved for future use
    // PN_API_IOD_IoCrTypeMcProvider = 3, /*!< IO direction is multicast provider */
    // PN_API_IOD_IoCrTypeMcConsumer = 4  /*!< IO direction is multicast consumer */
} PN_API_IOD_IoCrType_t;

/* TYPES */

/*!
 * \brief Device annotation.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    int8_t   deviceType[PN_API_IOD_MAX_DEV_TYPE_LEN + 1];   /*!< Device type */
    int8_t   orderId[PN_API_IOD_IM0_ORDER_ID_LEN + 1];      /*!< Order ID */
    int8_t   serialNum[PN_API_IOD_IM0_SERIAL_NUM_LEN];      /*!< IM serial number */
    uint16_t hwRevision;                                    /*!< IM hardware revision */
    int8_t   swRevisionPrefix;                              /*!< Software revision prefix */
    uint16_t swRevision1;                                   /*!< IM software revision */
    uint16_t swRevision2;                                   /*!< IM software revision */
    uint16_t swRevision3;                                   /*!< IM software revision */
} PN_API_IOD_DevAnnotation_t;

/*!
 * \brief Module/Submodule address (logical/geographical).
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    PN_API_IOD_AddrType_t type;     /*!< Address type */
    union
    {
        uint32_t addr;              /*!< Logical address */
        struct
        {
            uint32_t slot;          /*!< Slot number */
            uint32_t subslot;       /*!< Subslot number */
        } geo;                      /*!< Geographical address */
    } address;                      /*!< Module/Submodule address */
} PN_API_IOD_DevAddr_t;

/*!
 * \brief Ownership information.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint16_t dataOffset; /*!< Offset of IO data */
    uint16_t dataLen;    /*!< Expected length of IO data */
    uint16_t iocsOffset; /*!< Offset of IOCS */
    uint16_t iopsOffset; /*!< Offset of IOPS */
    uint8_t  iops;       /*!< IOPS value (for internal use only) */
    uint8_t  iocs;       /*!< IOCS value (for internal use only) */
} PN_API_IOD_Ownership_t;

/*!
 * \brief Parameters submodule.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t                    api;                /*!< [in] Application Process Identifier */
    uint32_t                    slot;               /*!< [in] Slot number */
    uint32_t                    subslot;            /*!< [in] Subslot number */
    uint32_t                    modId;              /*!< [in] Module identifier (1 to 0xFFFFFFFF) */
    uint32_t                    submodId;           /*!< [in] Submodule identifier (1 to 0xFFFFFFFF) */
    uint32_t                    modProp;            /*!< [in] Module properties */
    uint32_t                    submodProp;         /*!< [in] Submodule properties */
    uint32_t                    entityIndSetCfg;    /*!< [in] Entity index */
    uint32_t                    entityState;        /*!< [in] Entity state: ELEM_FREE / ELEM_OCCUPIED */
    PN_API_IOD_Ownership_t      in;                 /*!< [in] Ownership information for input data */
    PN_API_IOD_Ownership_t      out;                /*!< [in] Ownership information for output data */
    uint8_t                     isPlugged;          /*!< [in] Subslot contains a submodule (right or wrong) */
    PN_API_IOD_SubmodProp_t     ioProp;             /*!< [in] IO properties. Valid only, if submodule is
                                                         already plugged */
    uint16_t                    ownSessionKey;      /*!< [in, out] Set to 0: ownership rejected,
                                                         unchanged: ownership assumed */
    bool                        isWrongSubmod;      /*!< [in, out] false: submodule is OK, true: submodule is wrong */
    bool                        paramEndValid;      /*!< [in] true: ParamEnd event has been executed for this submodule */
} PN_API_IOD_ExpSubmod_t;

/*!
 * \brief Module/Submodule properties.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    bool arValid;                   /*!< AR is valid (ownership indication has been processed or is under work) */
    bool firstParamEndValid;        /*!< ParamEnd event has been signaled for this submodule */
    bool readyForInputUpdateValid;  /*!< Ready to exchange input data ("ready for input update event"
                                        has been signaled) */
    uint32_t numOfPluggedSubmods;   /*!< Current number of plugged submodules/subslots */
    uint32_t ioCrLen;               /*!< Length of IOCR (data + IOxS + APDU status) */
    uint32_t lastApduStatus;        /*!< Value of last remote APDU status from PN_API_IOD_initiateDataRead() */
    PN_API_IOD_ExpSubmod_t *submod; /*!< Expected Submodule list received from the controller */
    bool ioUpdatePending;           /*!< Pending update of IO data (data exchange is in progress) */
    uint16_t arType;                /*!< AR type, see #PN_API_IOD_ArType_t */
} PN_API_IOD_Exp_t;

/*!
 * \brief AR information.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    PN_API_IOD_ArType_t arType;          /*!< AR type, see #PN_API_IOD_ArType_t */
    uint32_t            arNum;           /*!< AR number (for device access: arNum = 3) */
    uint16_t            arSessionKey;    /*!< Session key */
    uint16_t            sendClockFactor; /*!< Send clock factor */
    uint16_t            redRatioIoCrIn;  /*!< Reduction ratio of input IOCR */
    uint16_t            redRatioIoCrOut; /*!< Reduction ratio of output IOCR */
    uint32_t            hostIp;          /*!< IP address of host (IO controller) */
    uint32_t            cycleFactor;     /*!< Cycle factor */
    uint16_t            ppmEvent;        /*!< Provider Protocol Machine event */
    uint16_t            cpmEvent;        /*!< Consumer Protocol Machine event */
} PN_API_IOD_ArInfo_t;

/*!
 * \brief IO Submodule list.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct
{
    uint32_t api;        /*!< Application Process Identifier */
    uint32_t slot;       /*!< Slot number */
    uint32_t subslot;    /*!< Subslot number. */
    uint32_t modId;      /*!< Module ID (1 to 0xFFFFFFFF) */
    uint32_t submodId;   /*!< Submodule ID (1 to 0xFFFFFFFF) */
    uint32_t inDataLen;  /*!< Input data length */
    uint32_t outDataLen; /*!< Output data length */
    uint8_t  im0Support; /*!< Submodule has own I&M0 data */
} PN_API_IOD_SubmodListEntry_t;
#pragma pack(pop)

/*!
 * \brief IM software revision.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct
{
    uint8_t prefix;          /*!< Software revision prefix */
    uint8_t funcEnhancement; /*!< Software revision function enhancement */
    uint8_t bugFix;          /*!< Software revision bug fix */
    uint8_t internalChange;  /*!< Software revision internal change */
} PN_API_IOD_Im0SwRevision_t;
#pragma pack(pop)

/*!
 * \brief IM0 Data structure.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct
{
    uint16_t                   vendorId;                                 /*!< Vendor ID */
    uint8_t                    orderId[PN_API_IOD_IM0_ORDER_ID_LEN];     /*!< Order ID */
    uint8_t                    serialNum[PN_API_IOD_IM0_SERIAL_NUM_LEN]; /*!< IM serial number */
    uint16_t                   hwRevision;                               /*!< IM hardware revision */
    PN_API_IOD_Im0SwRevision_t swRevision;                               /*!< IM software revision */
    uint16_t                   revisionCnt;                              /*!< IM revision counter, notifies
                                                                              a HW modification */
    uint16_t                   profileId;                                /*!< IM profile ID */
    uint16_t                   profileSpecType;                          /*!< IM profile spec type */
    uint8_t                    versionMajor;                             /*!< IM version major */
    uint8_t                    versionMinor;                             /*!< IM version minor */
    uint16_t                   imXSupported;                             /*!< IM supported (bits for IM1 to IM15) */
} PN_API_IOD_Im0Data_t;
#pragma pack(pop)

/*!
 * \brief IM1 Data structure.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct PN_API_IOD_Im1Data
{
    uint8_t tagFunction[32]; /*!< see pnio spec, IM_Tag_Function */
    uint8_t tagLocation[22]; /*!< see pnio spec, IM_TAG_Location */
} PN_API_IOD_Im1Data_t;
#pragma pack(pop)

/*!
 * \brief IM2 Data structure.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct PN_API_IOD_Im2Data
{
    uint8_t installationDate[16]; /*!< see pnio spec, IM_Date */
} PN_API_IOD_Im2Data_t;
#pragma pack(pop)

/*!
 * \brief IM3 Data structure.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct PN_API_IOD_Im3Data
{
    uint8_t descriptor[54]; /*!< see pnio spec, IM_Descriptor */
} PN_API_IOD_Im3Data_t;
#pragma pack(pop)

/*!
 * \brief IM4 Data structure.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct PN_API_IOD_Im4Data
{
    uint8_t signature[54]; /*!< see pnio spec, IM_Signature */
} PN_API_IOD_Im4Data_t;
#pragma pack(pop)

/*!
 * \brief IM0 list
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct
{
    uint32_t             api;     /*!< Application Process Identifier */
    uint32_t             slot;    /*!< Slot number */
    uint32_t             subslot; /*!< Subslot number */
    PN_API_IOD_Im0Data_t im0Data; /*!< IM0 data */
} PN_API_IOD_Im0ListEntry_t;
#pragma pack(pop)

/*!
 * \brief IO Device instance.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint16_t vendorId;                /*!< Vendor ID */
    uint16_t deviceId;                /*!< Device ID */
    int8_t  *pDevType;                /*!< Device type */
    uint32_t maxNumOfSubslots;        /*!< Maximum number of subslots */
    uint32_t maxNumOfBytesPerSubslot; /*!< Maximum number of bytes per subslot */
} PN_API_IOD_IodInstance_t;

/*!
 * \brief Remanent submodule configuration information.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct
{
    uint32_t api;           /*!< Application Process Identifier */
    uint16_t slot;          /*!< Slot number */
    uint16_t subslot;       /*!< Subslot number. */
    uint32_t modId;         /*!< Module ID (1 to 0xFFFFFFFF) */
    uint32_t submodId;      /*!< Submodule ID (1 to 0xFFFFFFFF) */
    uint16_t inDataLen;     /*!< Input data length */
    uint16_t outDataLen;    /*!< Output data length */
} PN_API_IOD_RemaDataSubmodCfgInfo_t;
#pragma pack(pop)

/*!
 * \brief SNMP parameters.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    int8_t  *pSysName;      /*!< SNMP MIB2 object "system name" */
    uint32_t sysNameLen;    /*!< Length of system name */
    int8_t  *pSysContact;   /*!< SNMP MIB2 object "system contact" */
    uint32_t sysContactLen; /*!< Length of system contact */
    int8_t  *pSysLoc;       /*!< SNMP MIB2 object "system location" */
    uint32_t sysLocLen;     /*!< Length of system location */
    int8_t  *pSysDesc;      /*!< SNMP MIB2 object "system description" */
    uint32_t sysDescLen;    /*!< Length of system description */
    int8_t  *pIfDesc;       /*!< SNMP MIB2 object "interface description" */
    uint32_t ifDescLen;     /*!< Length of interface description */
    int8_t  *pPortName1;    /*!< Name of port 1 */
    uint32_t portNameLen1;  /*!< Length of port 1 name */
#if(PN_API_IOD_PDEV_NUM_OF_PORTS >= 2)
    int8_t *pPortName2;     /*!< Name of port 2 */
    uint32_t portNameLen2;  /*!< Length of port 2 name */
#endif
#if(PN_API_IOD_PDEV_NUM_OF_PORTS >= 3)
    int8_t *pPortName3;     /*!< Name of port 3 */
    uint32_t portNameLen3;  /*!< Length of port 3 name */
#endif
#if(PN_API_IOD_PDEV_NUM_OF_PORTS >= 4)
    int8_t *pPortName4;     /*!< Name of port 4 */
    uint32_t portNameLen4;  /*!< Length of port 4 name */
#endif
} PN_API_IOD_SnmpLldpParams_t;

/*!
 * \details
 * -# The first four parameters represent the PNIO Status.
 *   For details refer to IEC 61158-6 chapter 3.2.10.6 and 3.2.11.66.<br>
 * -# The last two parameters correspond to AdditionalValue1 and AdditionalValue2,
 *   see IEC 61158-6 chapter 3.2.11.50.<br>
 *   The values shall contain additional user information within negative responses.<br>
 *   The value zero indicates no further information.<br>
 *   For positive read responses, the value 1 of the field AdditionalValue1 indicates
 *   that the Record Data Object contains more data than have been read.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct
{
    uint8_t  errCode;   /*!< ErrorCode: Most significant word, most significant byte of PNIO Status */
    uint8_t  errDecode; /*!< ErrorDecode: Most significant word, least significant byte of PNIO Status */
    uint8_t  errCode1;  /*!< ErrorDecode: Least significant word, most significant byte of PNIO Status */
    uint8_t  errCode2;  /*!< ErrorCode2: Least significant word, least significant byte of PNIO Status */
    uint16_t addValue1; /*!< AdditionalValue1: additional information for user */
    uint16_t addValue2; /*!< AdditionalValue2: additional information for user */
} PN_API_IOD_ErrState_t;
#pragma pack(pop)

/*!
 * \brief Alarm data.
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint16_t arNum;                /*!< AR number */
    uint16_t sessionKey;           /*!< Session key */
    uint32_t api;                  /*!< Application Process Identifier */
    uint16_t slot;                 /*!< Slot number (for device substitute: slot = 0) */
    uint16_t subslot;              /*!< Subslot number (for module substitute: subslot = 0) */
    uint16_t alarmPriority;        /*!< PN_API_IOD_CM_ALARM_PRIORITY_LOW or PN_API_IOD_CM_ALARM_PRIORITY_HIGH */
    uint16_t alarmType;            /*!< See PN_API_IOD_CM_ALARM_TYPE_*. */
    uint16_t alarmSequence;        /*!< See PNIO specifications: "AlarmSpecifier", bits 0-10 */
    bool     diagChannelAvailable; /*!< See PNIO specifications: "AlarmSpecifier", bit 11 */
    bool     diagGenericAvailable; /*!< See PNIO specifications: "AlarmSpecifier", bit 12 */
    bool     diagSubmodAvailable;  /*!< See PNIO specifications: "AlarmSpecifier", bit 13 */
    uint8_t  reserved;             /*!< See PNIO specifications: "AlarmSpecifier", bit 14 */
    bool     arDiagnState;         /*!< See PNIO specifications: "AlarmSpecifier", bit 15 */
    uint32_t modId;                /*!< Module identifier (1 to 0xFFFFFFFF) */
    uint32_t submodId;             /*!< Submodule identifier (1 to 0xFFFFFFFF) */
    uint16_t userStructId;         /*!< User structure tag for alarm data */
    uint16_t userAlarmDataLen;     /*!< Length of alarm data */
    uint8_t *userAlarmData;        /*!< User alarm data */
    uint32_t cmPnioErr;            /*!< Alarm ack only */
} PN_API_IOD_DevAlarmData_t;

/*!
 * \brief IP configuration.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct
{
    uint32_t ipAddr;     /*!< IP address in network format (big endian) */
    uint32_t subnetMask; /*!< Network mask in network format (big endian) */
    uint32_t defaultGw;  /*!< Default gateway router in network format (big endian) */
} PN_API_IOD_IpSuite_t;
#pragma pack(pop)

/*!
 * \brief Profinet Block header.
 *
 * More details see Profinet specifications.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
#pragma pack(push, 1)
typedef struct
{
    uint16_t type;                                    /*!< Block type (big endian) */
    uint16_t len;                                     /*!< Block length */
    uint16_t version;                                 /*!< Block version (big endian) */
} PN_API_IOD_ioBlockHeader_t;
#pragma pack(pop)

typedef enum
{
    PN_API_IOD_NotPlugged  = 0,
    PN_API_IOD_Plugged      = 1
} PN_API_IOD_PlugState_t;

/*!
 * \brief Submodule information.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t                    api;              /*!< Application Process Identifier */
    uint32_t                    slot;             /*!< Slot number */
    uint32_t                    subslot;          /*!< Subslot number */
    uint32_t                    modId;            /*!< Module identifier (1 to 0xFFFFFFFF) */
    uint32_t                    submodId;         /*!< Submodule identifier (1 to 0xFFFFFFFF) */
    uint32_t                    dataDirection;    /*!< Data direction, see PN_API_IOD_SubmodProp_t */
    uint32_t                    index;            /*!< Real index of the entity */
    PN_API_IOD_PlugState_t      isPlugged;        /*!< Submodule is plugged (or not) */
    PN_API_IOD_RealSubmodStatus_t submodStatus;     /*!< Status of the submodule */
#if PN_API_IOD_INCLUDE_IM0_4
    uint8_t                     im0Support;       /*!< Type of IM0 support */
    PN_API_IOD_Im0Data_t        im0Data;          /*!< IM0 data */
#endif
    uint8_t                     inIops;           /*!< IOPS of input data (if submodule has input data) */
} PN_API_IOD_RealSubmod_t;

/*!
 * \brief Data buffer (DBAI).
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t devHndl;               /*!< [in]  Device handle */
    uint32_t arNum;                 /*!< [in]  AR number (1....NumOfAR), obtained in PNIO_cbf_ar_ownership_ind */
    PN_API_IOD_IoCrType_t ioDir;    /*!< IO direction (PNIO_IOCR_TYPE_INPUT, PNIO_IOCR_TYPE_OUTPUT) */
    uint8_t *pBuf;                  /*!< [out] Pointer to the IOCR buffer inside PNIO stack */
    uint32_t apduStatus;            /*!< [out] Cycle counter der Input IOCR */
} PN_API_IOD_BufferLock_t;

/*!
 * \brief Profinet Stack version.
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint8_t itemHH;     /*!< Highest item */
    uint8_t itemH;      /*!< High item */
    uint8_t itemL;      /*!< Low item */
    uint8_t itemLL;     /*!< Lowest item */
} PN_API_IOD_DkVersion_t;


#if(defined __cplusplus)
}
#endif

#endif /* PN_API_IOD_TYPES_H */
