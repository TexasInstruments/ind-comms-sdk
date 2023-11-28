/*!
 *  \file ecSlvApi_types.h
 *
 *  \brief
 *  EtherCAT User API  types.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2023 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#if !(defined __ECSLVAPI_TYPES_H__)
#define __ECSLVAPI_TYPES_H__		1

#if (defined ECATSLAVE_SO) && (ECATSLAVE_SO==1) // defined if ECATSLV is compiled as a DLL
 #ifdef ECSLVAPI_EXPORTS // defined if we are building the ECATSLAVE DLL (instead of using it)
  #define ECATSLV_API OSAL_DLL_EXPORT
 #else
  #define ECATSLV_API OSAL_DLL_IMPORT
 #endif // ECSLVAPI_EXPORTS
 #define ECATSLV_LOC OSAL_DLL_LOCAL
#else // ECATSLAVE_SO is not defined: this means ECATSLAVE is a static lib.
 #define ECATSLV_API
 #define ECATSLV_LOC
#endif // ECATSLAVE_SO

#include <osal.h>
#include "defines/ecSlvApiDef_error.h"
#include "defines/ecSlvApiDef_diagnosis.h"

#if !(defined FBTL_REMOTE) && !(defined DPRAM_REMOTE)
#include <custom_phy.h>
#endif

/** \addtogroup EC_API_SLV_TYPES
*  @{
*/

/*!
 *  @brief    PRUICSS Instance IDs
 */
/* also not all MCUs have 3 PRUs define those as Max usage */
typedef enum EC_API_SLV_EPRUICSS_MaxInstances
{
    EC_API_SLV_ePRUICSS_INSTANCE_ONE=1,
    EC_API_SLV_ePRUICSS_INSTANCE_TWO=2,
    EC_API_SLV_ePRUICSS_INSTANCE_THREE=3,
    EC_API_SLV_ePRUICSS_INSTANCE_MAX=4
} EC_API_SLV_EPRUICSS_MaxInstances_t;

/// EC_RETCODE_T  Error codes used during EtherCAT State Machine transitions.
typedef enum EC_API_SLV_EUserRetCodeserRetCodes
{
    EC_USR_eRET_OK = 0,                     ///< no error occurred
    EC_USR_eRET_ERROR,                      ///< Unspecified error occurred
    EC_USR_eRET_NOERROR_INWORK              ///< Indication for no error but operation is pending
} EC_API_SLV_EUserRetCodes_t;

/// EC_STATE_T EtherCAT State Machine states.
typedef enum EC_API_SLV_EEsmState
{
    EC_API_SLV_eESM_uninit    = 0x00,           ///< Uninitialized State
    EC_API_SLV_eESM_init      = 0x01,           ///< Init State
    EC_API_SLV_eESM_preop     = 0x02,           ///< PreOP State
    EC_API_SLV_eESM_bootstrap = 0x03,           ///< BootStrap State
    EC_API_SLV_eESM_safeop    = 0x04,           ///< SafeOP State
    EC_API_SLV_eESM_op        = 0x08,           ///< Operational State
    EC_API_SLV_eESM_errState  = 0x10,           ///< Error Flag
    EC_API_SLV_eESM_errDevId  = 0x20,           ///< Explicit Device ID
} EC_API_SLV_EEsmState_t;

/// EC_API_SLV_EDevType_t EtherCAT Device Type. Object Dictionary Index 0x1001
typedef enum EC_API_SLV_EDevType
{
    EC_API_SLV_eDT_DEFAULT_DEVICE           = 0x00000000,   ///< The device does not support any specific profile
    EC_API_SLV_eDT_MODULAR_DEVICE           = 0x00001389,   ///< The device is a modular device.
    EC_API_SLV_eDT_SERVO_DRIVE              = 0x00020192,   ///< The device supports CiA402 for a servo drive.
    EC_API_SLV_eDT_STEPPER_DRIVE            = 0x00040192,   ///< The device supports CiA402 for a stepper drive.
    EC_API_SLV_eDT_SERVO_SAFETY_PROFILE     = 0x000A0192,   ///< The device supports CiA402 for a servo drive with safety features.
    EC_API_SLV_eDT_STEPPER_SAFETY_PROFILE   = 0x000C0192,   ///< The device supports CiA402 for a stepper drive with safety features.
    EC_API_SLV_eDT_MDP_GATEWAY_IOLINK       = 0x184c1389,   ///< The device is a gateway to IO Link master
} EC_API_SLV_EDevType_t;

/// EC_API_SLV_SDIAG_parameter_t Diagnosis type parameter
typedef struct EC_SLV_INT_SDIAG_parameter
{
    uint16_t    defType;    /**< \brief paramter type (DEFTYPE_UNSIGNED8, DEFTYPE_UNSIGNED16...) */
    uint8_t     msgType;    /**< \brief message type (DIAG_MSG_PARAM_TYPE_DATA, DIAG_MSG_PARAM_TYPE_B_ARRY...) */
    uint32_t    length;     /**< \brief param length */
    uint8_t*    pParam;     /**< \brief pointer to the corresponding parameter memory*/
} EC_API_SLV_SDIAG_parameter_t;

/// EC_DEVERROR_T Error register values. Object Dictionary Index 0x1001.
typedef enum EC_API_SLV_EErrorRegister
{
    EC_API_SLV_eERR_NO_ERROR                 = 0x00,     ///< No error
    EC_API_SLV_eERR_GENERIC_ERROR            = 0x01,     ///< Generic error
    EC_API_SLV_eERR_CURRENT_ERROR            = 0x02,     ///< Current error
    EC_API_SLV_eERR_VOLTAGE_ERROR            = 0x04,     ///< Voltage error
    EC_API_SLV_eERR_TEMPERATURE_ERROR        = 0x08,     ///< Temperature error
    EC_API_SLV_eERR_COMMUNICATION_ERROR      = 0x10,     ///< Communication error
    EC_API_SLV_eERR_DEVICE_PROFILE_ERROR     = 0x20,     ///< Profile error
    EC_API_SLV_eERR_MANUFACTURER_ERROR       = 0x80,     ///< Manufacturer error
} EC_API_SLV_EErrorRegister_t;

/*!
 *  @brief    Phy Index
 */
/* also not all MCUs have 3 PRUs define those as Max usage */
typedef enum EC_API_SLV_EPhy_Index
{
    EC_API_SLV_ePHY_IN                      = 0,        ///< Phy index for IN Phy
    EC_API_SLV_ePHY_OUT                     = 1,        ///< Phy index for OUT Phy
} EC_API_SLV_EPhy_Index_t;


/// EC_SLV_API_PDO_SEntryMap_t describes the EtherCAT Slave API
typedef struct EC_SLV_API_PDO_SEntryMap
{
    uint16_t    index;
    uint8_t     subIndex;
} OSAL_STRUCT_PACKED EC_API_SLV_PDO_SEntryMap_t;

/// TEntry describes an Entry of a PDO Mapping.
typedef struct EC_API_SLV_PdoEntry EC_API_SLV_SPdoEntry_t;

/// TPdo is used to describe the RxPDOs and TxPDOs
typedef struct EC_API_SLV_Pdo EC_API_SLV_Pdo_t;

/// TSdoEntry describes an OBD Object Entry
typedef struct EC_API_SLV_CoE_ObjEntry EC_API_SLV_SCoE_ObjEntry_t;

/// TSdo describes an Object Dictionary Object
typedef struct EC_API_SLV_CoE_Object EC_API_SLV_SCoE_Object_t;

/// EC_API_SLV_SHandle_t describes the EtherCAT Slave API
typedef struct EC_API_SLV_Handle EC_API_SLV_SHandle_t;

/** @}*/

/** \addtogroup EC_API_SLV_APPCB
*  @{
*/

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called as application MainLoop in MainLoop.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppMainLoopCtxt     CallBack Content for use in complex applications.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CBUsrApplicationMainLoop_t)(void* pAppMainLoopCtxt);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called as "application run" in MainLoop.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppRunCtxt		CallBack Content for use in complex applications.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CBUsrApplicationRun_t)(void* pAppRunCtxt);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Called on severe Stack error (OSAL_error)
 *
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppRunCtxt   CallBack Content for use in complex applications.
 *  \param[in]  errorCode		Error code from DTK Stack
 *  \param[in]  fatal         Error type
 *  \param[in]  numOfParam	number of upcoming parameters
 *  \param[in]  arg           rest of parameters
 *
 *  <!-- Reference -->
 *  \sa OSAL_error
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void(*EC_API_SLV_CBStackError_t)(void* pAppRunCtxt, uint32_t errorCode, uint8_t fatal, uint8_t numOfParam, va_list arg);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Called on severe Stack error (OSAL_error)
 *
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pMsrmtCtxt        CallBack context
 *  \param[in]  measureChannel    measure channel code
 *  \param[in]  channelOn         true: channel raise, false: channel lower
 *
 *  <!-- Reference -->
 *  \sa OSAL_error
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void(*EC_API_SLV_CBMsrmt_t)(void* pMsrmtCtxt, uint32_t measureChannel, bool channelOn);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called when mailbox handler is started.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext            Function context.
 *  \return EC_USR_CB_ERetCode Returns the Callback error code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint16_t (*EC_API_SLV_CBStartMbxHandler_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called when mailbox handler is stopped.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext            Function context.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CBStopMbxHandler_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called when input handler is started.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      Function context.
 *  \param[in]  pIntMask      Value which will be written to the 0x204 register (AL event mask) during the state transition PreOP to SafeOP.
 *  \return EC_USR_CB_ERetCode Returns the Callback error code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint16_t (*EC_API_SLV_CBStartInputHandler_t)(void* pContext, uint16_t *pIntMask);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called when input handler is stopped.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext      Function context.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CBStopInputHandler_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called when output handler is started.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext      Function context.
 *  \return EC_USR_CB_ERetCode Returns the Callback error code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint16_t (*EC_API_SLV_CBStartOutputHandler_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called when ouput handler is stopped.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext      Function context.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CBStopOutputHandler_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Register external Input Process Data Buffer.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext      Function context.
 *  \param[in]  length        length of Input PD
 *  \return void pointer to Input Process Data.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void*(*EC_API_SLV_CBPreSeqInputPD_t)(void* pContext, uint32_t length);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Register external Output Process Data Buffer.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext      Function context.
 *  \param[in]  length        length of Output PD
 *  \return void pointer to Input Process Data.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void*(*EC_API_SLV_CBPreSeqOutputPD_t)(void* pContext, uint32_t length);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Register external Release Input Process Data Buffer Function.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext      Function context.
 *  \param[in]  pData         Input PD
 *  \param[in]  length        length of Input PD
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CBPostSeqInputPD_t)(void* pContext, void* pData, uint32_t length);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Register external Release Output Process Data Buffer Function.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext      Function context.
 *  \param[in]  pData         Output PD
 *  \param[in]  length        length of Output PD
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CBPostSeqOutputPD_t)(void* pContext, void* pData, uint32_t length);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Inform about PDO assignment changes.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext          Function context.
 *  \param[in]  rx                True if PDO assignment description belongs to Rx, false otherwise.
 *  \param[in]  count             Number of PDOs assigned to the SyncManager.
 *  \param[in]  pPdoIndexArray    Array of PDO indexes.
 *  \return     Returns the API error code.
 *  \retval     EC_API_eERR_NONE    Allow the assignment changes.
 *  \retval     EC_API_eERR_ABORT   Refuse the assignment changes.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint32_t (*EC_API_SLV_PDO_CBAssignmentChanges_t)(void* pContext, bool rx, uint8_t count, uint16_t* pPdoIndexArray);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Inform about PDO mapping changes.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext          Function context.
 *  \param[in]  pdoIndex          PDO index.
 *  \param[in]  count             Number of entries mapped into the PDO.
 *  \param[in]  pPdoMap           PDO mapping configuration array.
 *  \return     Returns the API error code.
 *  \retval     EC_API_eERR_NONE    Allow PDO mapping changes.
 *  \retval     EC_API_eERR_ABORT   Refuse PDO mapping changes.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint32_t (*EC_API_SLV_PDO_CBMappingChanges_t)(void* pContext, uint16_t pdoIndex, uint8_t count, EC_API_SLV_PDO_SEntryMap_t * pPdoMap);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback trigered by a SDO Upload operation.
 *
 *  \details
 *  The function is called when the EtherCAT Master sends a object read request. If no callback function is registered then the value contained in the object is returned.
 *  If a callback function is registered it is the reponsability of the callback function to update the value pointed to by \p pData. \p pData is returned within
 *  the SDO Response datagram.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext          function context
 *  \param[in]  index             Index of the requested object.
 *  \param[in]  subindex          SubIndex of the requested object.
 *  \param[in]  length            Length of the received data buffer.
 *  \param[in]  pData             Pointer to the data buffer.
 *  \param[in]  completeAccess    Read all subIndexes of the object.
 *  \return result of the upload operation. Set 0 if operation succeeded or an abort code declared on the Beckhoff Slave Stack (ABORTIDX_ in sdoserv.h).
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint8_t(*EC_API_SLV_CBObjRead_t)(void* pContext, uint16_t index, uint8_t subindex, uint32_t length, uint16_t* pData, uint8_t completeAccess);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback trigered by a SDO Download operation.
 *
 *  \details
 *  The function is called when the EtherCAT Master sends a object write request. If no callback function is registered then the value pointed to by \p pData is
 *  directly written to the object entry. If a callback function is registered it is the reponsability of the callback function to copy the value pointed to by \p pData
 *  to object entry.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext          function context
 *  \param[in]  index             Index of the requested object.
 *  \param[in]  subindex          SubIndex of the requested object.
 *  \param[in]  length            Length of the received data buffer.
 *  \param[in]  pData             Pointer to the data buffer.
 *  \param[in]  completeAccess    Write all subIndexes of the object.
 *  \return     result of the download operation. Set 0 if operation succeeded or an abort code declared on the Beckhoff Slave Stack (ABORTIDX_ in sdoserv.h).
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint8_t(*EC_API_SLV_CBObjWrite_t)(void* pContext, uint16_t index, uint8_t subindex, uint32_t length, uint16_t* pData, uint8_t completeAccess);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined EoE receive function. Called when an EoE frame is received.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext          function context
 *  \param[in]  pData             EoE Frame Data
 *  \param[in]  length            EoE Frame Size
 *  \return true if frame is handle, false if it should be passed on.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef bool (*EC_API_SLV_EoE_CBReceiveHandler_t)(void* pContext, uint16_t* pData, uint16_t length);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  EoE Settings Indication callback proxy
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext          The pointer to the EtherCAT API instance.
 *  \param[in]  pMac              Virtual Net MAC address
 *  \param[in]  pIp               Virtual Net IP address
 *  \param[in]  pSubNet           Virtual Net Subnet
 *  \param[in]  pDefaultGateway   Virtual Net Default Gateway
 *  \param[in]  pDnsIp            Virtual Net DNS server address
 *  \return     true if settings are handled, false otherwise
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef bool (*EC_API_SLV_EoE_CBSettingIndHandler_t)(void* pContext, uint16_t *pMac, uint16_t* pIp, uint16_t* pSubNet, uint16_t* pDefaultGateway, uint16_t* pDnsIp );

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  AoE Communication Request Handler.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext          The pointer to the EtherCAT API instance.
 *  \param[in]  port              AMS port.
 *  \param[in]  cmdId             ADS Request ID: read, write or read/write.
 *  \param[in]  index             16 bit index value from IndexOffset.
 *  \param[in]  subIndex          8 bit subIndex value from IndexOffset.
 *  \param[in]  completeAccess    CoE Complete Access flag.
 *  \param[in]  dataLen           Requested data length to read or write.
 *  \return     ADS error code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint16_t (*EC_API_SLV_AoE_CBRquestHandler_t)(void* pContext, uint16_t port, uint16_t cmdId, uint16_t index, uint8_t subIndex, bool completeAccess, uint32_t dataLen);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined FoE receive function. Called when an FoE frame is received.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context
 *  \param[in]  pName         FoE file name
 *  \param[in]  nameLen       Length of file name
 *  \param[in]  isRead        true: readAccess, false: writeAccess
 *  \param[in]  password      password
 *  \return Error code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint32_t (*EC_API_SLV_FoE_CBOpenFileHandler_t)(void* pContext, const char* pName, uint16_t nameLen, bool isRead, uint32_t password);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Read FoE File callback.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext              call context
 *  \param[in]  pData                 FoE file data
 *  \param[in]  foeMaxSendBlockSize   Max SendBlock Size
 *  \param[in]  offset                FoE file data offset
 *  \return Error Code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint32_t(*EC_API_SLV_FoE_CBReadFileHandler_t)(void* pContext, uint16_t* pData, uint16_t foeMaxSendBlockSize, uint32_t offset);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Write FoE File callback.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext  call context
 *  \param[in]  pData     FoE file data
 *  \param[in]  size      Size of FoE data
 *  \return Error Code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint32_t(*EC_API_SLV_FoE_CBWriteFileHandler_t)(void* pContext, uint16_t* pData, uint16_t size);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Close FoE File callback.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context
 *  \param[in]  errorCode     FoE close error code
 *  \return Error Code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint32_t(*EC_API_SLV_FoE_CBCloseFileHandler_t)(void* pContext, uint32_t errorCode);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  FoE Handle Firmware update callback.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context
 *  \return Error Code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef EC_API_EError_t(*EC_API_SLV_FoE_CBFwUpdateHandler_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Handle System Reboot.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext      call context
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void(*EC_API_SLV_CBSystemRebootHandler_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  AoE Communication Read Request Handler.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]      pContext          The pointer to the EtherCAT API instance.
 *  \param[in]      port              AMS port.
 *  \param[in]      index             16 bit index value from IndexOffset.
 *  \param[in]      subIndex          8 bit subIndex value from IndexOffset.
 *  \param[in]      completeAccess    CoE Complete Access flag.
 *  \param[in,out]  pLength           Request data length.
 *  \param[in]      pData             Pointer to data.
 *  \return     ADS error code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint16_t (*EC_API_SLV_AoE_CBReadRequestHandler_t)(void*         pContext,
                                                          uint16_t      port,
                                                          uint16_t      index,
                                                          uint8_t       subIndex,
                                                          bool          completeAccess,
                                                          uint32_t*     pLength,
                                                          uint16_t*     pData);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  AoE Communication Write Request Handler.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]      pContext          The pointer to the EtherCAT API instance.
 *  \param[in]      port              AMS port.
 *  \param[in]      index             16 bit index value from IndexOffset.
 *  \param[in]      subIndex          8 bit subIndex value from IndexOffset.
 *  \param[in]      completeAccess    CoE Complete Access flag.
 *  \param[in,out]  pLength           Request data length.
 *  \param[in]      pData             Pointer to data.
 *  \return     ADS error code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint16_t (*EC_API_SLV_AoE_CBWriteRequestHandler_t)(void*           pContext,
                                                           uint16_t        port,
                                                           uint16_t        index,
                                                           uint8_t         subIndex,
                                                           bool            completeAccess,
                                                           uint32_t*       pLength,
                                                           uint16_t*       pData);
/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Send SoE message to master.
 *
 *  \details
 *  This function is called when a ServoDriveProfile over EtherCAT service has to be sent to the master.
 *  The DTK Stack collects the Service, Flags and data to send from the user application.
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context
 *  \param[in]  pSoEService   SoE Service code
 *  \param[in]  pSoEFlags		SoE Flags
 *  \param[in]  pData         SoE Data
 *  \param[in]  pLen          Data length
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_SoE_CBSend_t)(void* pContext, uint16_t* pSoEService, uint16_t* pSoEFlags, void** pData, uint16_t* pLen);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Recevice SoE message from Master.
 *
 *  \details
 *  This function is called when a ServoDriveProfile over EtherCAT service is received from the master.
 *  DTK Stack calls the function with the parameters received from the master. The application should response to the service.
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context
 *  \param[in]  soEService    SoE Service code
 *  \param[in]  soEFlags		SoE Flags
 *  \param[in]  pData         SoE Data
 *  \param[in]  pLen          Data length
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void(*EC_API_SLV_SoE_CBRecv_t)(void* pContext, uint16_t soEService, uint16_t soEFlags, void* pData, uint16_t *pLen);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Master requests a notification. Application should send an Emergency message.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context
 *  \return     ErrorCode       Mailbox error code.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint8_t(*EC_API_SLV_SoE_CBNofReq_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User defined function. Called as "application Cia402 StateMachine" in MainLoop.
 *
 *  \details
 *  If function is not provided, the DTK uses an internal CiA402 State Machine.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppRunCtxt   CallBack Content for use in complex applications.
 *  \return     bool            true if statemachine handled, false handle in stack
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef bool (*EC_API_SLV_CiA402_CBUsrApplSM_t)(void* pAppRunCtxt);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Configure CiA402 Object Dictionary.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext		call context.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef uint16_t (*EC_API_SLV_CiA402_CBUsrApplSetDict_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Set default values for the CiA402 Objects.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext		call context.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CiA402_CBUsrApplSetDictValues_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User application slot (control)
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext		call context.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CiA402_CBUsrApplApplication_t)(void* pContext);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  User application local error
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext		Parameter description.
 *  \param[in]  errorCode     Local Error
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void (*EC_API_SLV_CiA402_CBUsrApplLocalError_t)(void* pContext, uint16_t errorCode);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Load EEPROM from memory.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context
 *  \param[in]  pEeprom       Eeprom data loaded from memory
 *  \param[in]  pLength       Eeprom Length read from memory
 *  \return     bool            true on successfull load
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef bool(*EC_API_SLV_CBEepromLoad_t)(void* pContext, void* pEeprom, uint32_t* pLength);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Write EEPROM to memory.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context
 *  \param[in]  pEeprom		Eeprom Content.
 *  \param[in]  length        Eeprom Length.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void(*EC_API_SLV_CBEepromWrite_t)(void* pContext, void* pEeprom, uint32_t length);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Init flash memory block to load and store eeprom content.
 *
 *  <!-- Parameters and return values: -->
 *  \param[in]  pContext  call context
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void(*EC_API_SLV_CBInitFlash_t)(void* pContext);
/** @}*/

/** \addtogroup EC_API_SLV_BOARD
*  @{
*/

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Set Status LED call back
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      call context.
 *  \param[in]  errLed        state of error LED
 *  \param[in]  statusLed     state of status LED.
 *
 *  <!-- Group: -->
 *
 *  \ingroup SLVAPI
 *
 * */
typedef void(*EC_API_SLV_CBBoardStatusLed_t)(void* pContext, bool errLed, bool statusLed);

/** @}*/

#endif /* __ECSLVAPI_TYPES_H__ */
