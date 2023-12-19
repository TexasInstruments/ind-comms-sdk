/*!
 *  \file app_discrete_io_device.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application for Device Profile: General Purpose of Discrete IO.
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

#if (!(defined FBTLPROVIDER) || (0 == FBTLPROVIDER)) && (!(defined FBTL_REMOTE) || (0 == FBTL_REMOTE))

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "EI_API.h"
#include "EI_API_def.h"

#include "appNV.h"

#include <osal.h>
#include <osal_error.h>
#include <hwal.h>
#include <board.h>

#include <CMN_board.h>
#include <CMN_os.h>
#include <CMN_app.h>
#include <CMN_mem.h>
#include <CMN_CPU_api.h>

#include <drivers/CUST_drivers.h>

#include "appWebServer.h"

#include "app.h"
#include "appTask.h"
#include "appCipCodes.h"
#include "appCfg.h"
#include "device_profiles/app_device_profile.h"
#include "device_profiles/app_device_profile_intern.h"
#include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
#include "device_profiles/discrete_io_device/app_discrete_io_device.h"

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#if defined(SOC_AM64X) || defined(SOC_AM243X)
extern PRUICSS_Handle prusshandle;
#else
Board_IDInfo boardInfo;
#endif

// Static variables and pointers used in this example.
//static void EI_APP_DISCRETE_IO_DEVICE_adpInit  (EI_API_ADP_T* pAdapter);

static bool EI_APP_DIO_DEVICE_init     (EI_API_ADP_T      *pAdapter, EI_API_CIP_NODE_T *pCipNode);
static void EI_APP_DIO_DEVICE_run      (EI_API_CIP_NODE_T *pCipNode);

static bool EI_APP_DIO_DEVICE_cipSetup (EI_API_CIP_NODE_T *pCipNode);

static uint32_t EI_APP_DIO_DEVICE_connectionState_s     = 0;
static uint32_t EI_APP_DIO_DEVICE_receiveData_s         = 0;

static uint8_t EI_APP_DIO_DEVICE_idleActionConfiguration_s     = 0;
static uint8_t EI_APP_DIO_DEVICE_faultActionConfiguration_s    = 0;

static uint16_t EI_APP_DIO_DEVICE_extendedStatus_s[255] = {0};

static uint8_t EI_APP_DIO_DEVICE_producedConnectionPath[EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH] =
{
        32, 4, 36,
        EI_APP_DIO_DEVICE_ASSEMBLY_CONFIGURATION, 44,
        EI_APP_DIO_DEVICE_ASSEMBLY_CONSUMING, 44,
        EI_APP_DIO_DEVICE_ASSEMBLY_PRODUCING, 128
};

static uint8_t EI_APP_DIO_DEVICE_consumedConnectionPath[EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH] =
{
        32, 4, 36,
        EI_APP_DIO_DEVICE_ASSEMBLY_CONFIGURATION, 44,
        EI_APP_DIO_DEVICE_ASSEMBLY_CONSUMING, 44,
        EI_APP_DIO_DEVICE_ASSEMBLY_PRODUCING, 128
};

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function for ForwardOpen, LargeForwardOpen and ForwardClose.
 *
 *  \details
 *  Callback function which is called when a request for the services
 *  ForwardOpen, LargeForwardOpen and ForwardClose was received for
 *  DOP related connections and sets the Connection States for State Machine.
 *
 */
EI_API_ADP_SEipStatus_t EI_APP_DIO_DEVICE_cmgrCb(uint32_t serviceCode, EI_API_ADP_UCmgrInfo_u cmgrInfo)
{
    EI_API_ADP_SEipStatus_t ret_val= {.gen_status=0,
                                      .extended_status_size=0,
                                      .extended_status_arr=EI_APP_DIO_DEVICE_extendedStatus_s};

    switch(serviceCode)
        {
        case 0x54:
            if(EI_APP_DIO_DEVICE_IO_CONNECTION == cmgrInfo.forwardOpenInfo.typeTrigger
                                                    && EI_APP_DIO_DEVICE_ASSEMBLY_CONSUMING == cmgrInfo.forwardOpenInfo.conPath[5])
            {
                EI_APP_DIO_DEVICE_receiveData_s = EI_APP_DIO_DEVICE_DATA_RECEIVED;
                EI_APP_DIO_DEVICE_connectionState_s = EI_APP_DIO_DEVICE_ConnectionEstablished;
            }
            break;
        case 0x5b:
            break;
        case 0x4e:
            if(EI_APP_DIO_DEVICE_ASSEMBLY_PRODUCING == cmgrInfo.forwardCloseInfo.conPath[7])
            {
                EI_APP_DIO_DEVICE_connectionState_s = EI_APP_DIO_DEVICE_ConnectionClosed;
            }
            break;
        default:
            OSAL_printf("unknown service code %x\r\n", serviceCode);
        }
    return ret_val;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function for Connection Time Out.
 *
 *  \details
 *  Callback function which is called when a request for the
 *  DOP related IO Communication state of "Connection Time Out"
 *
 */
EI_API_ADP_SEipStatus_t EI_APP_DIO_DEVICE_cobjTimeOutCb(
        uint8_t *producedConnectionPath,
        uint16_t producedConnectionPathLen,
        uint8_t *consumedConnectionPath,
        uint16_t consumedConnectionPathLen)
{
    EI_API_ADP_SEipStatus_t ret_val= {.gen_status=0,
                                      .extended_status_size=0,
                                      .extended_status_arr=EI_APP_DIO_DEVICE_extendedStatus_s};

    /* Check if the Connection Paths are correct */
    if((0 == OSAL_MEMORY_memcmp(producedConnectionPath, EI_APP_DIO_DEVICE_producedConnectionPath, EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH))
            || (0 == OSAL_MEMORY_memcmp(consumedConnectionPath, EI_APP_DIO_DEVICE_consumedConnectionPath, EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH)))
    {
        EI_APP_DIO_DEVICE_connectionState_s = EI_APP_DIO_DEVICE_ConnectionTimeOut;
    }

    return ret_val;
}

/*!
 *
 * \brief
 * Gets connection states.
 *
 * \details
 * This function used to get connection states for Discrete IO Device connection.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * uint32_t connectionState = 0;
 * uint32_t receiveData     = 0;
 *
 * EI_APP_DIO_DEVICE_getConnectionInfo(&connectionState, &receiveData)
 *
 * \endcode
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
void EI_APP_DIO_DEVICE_getConnectionInfo(uint32_t *connectionState, uint32_t *receiveData)
{
    *connectionState = EI_APP_DIO_DEVICE_connectionState_s;
    *receiveData     = EI_APP_DIO_DEVICE_receiveData_s;
}

/*!
 * \brief
 * Registers discrete IO device profile interface functions.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_init function of device profile interface.
 * Function needs to be defined in each profile.
 *
 * \remark
 * In time of build process only one device profile can be active.
 * Rest profiles need to be excluded from the build.
 *
 * \param[in]     pIntf    Pointer to device profile interface which needs to be initialized.
 *
 * \return        value as bool.
 * \retval        true     Success.
 * \retval        false    Invalid input parameter defined as NULL.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_init (void)
 * {
 *     return EI_APP_DEVICE_PROFILE_register(&EI_APP_DEVICE_PROFILE_intf_s);
 * }
 *
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_init
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t *pIntf)
{
    uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;

    if (NULL == pIntf)
    {
        error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
        goto laError;
    }

    pIntf->task.init = EI_APP_DIO_DEVICE_init;
    pIntf->task.run  = EI_APP_DIO_DEVICE_run;

    pIntf->cfg.init                  = EI_APP_DIO_DEVICE_CFG_init;
    pIntf->cfg.isValid               = EI_APP_DIO_DEVICE_CFG_isValid;
    pIntf->cfg.setHeader             = EI_APP_DIO_DEVICE_CFG_setHeader;
    pIntf->cfg.apply                 = EI_APP_DIO_DEVICE_CFG_apply;
    pIntf->cfg.setDefaultWithoutComm = EI_APP_DIO_DEVICE_CFG_setDefaultWithoutComm;
    pIntf->cfg.callback              = EI_APP_DIO_DEVICE_CFG_callback;
    pIntf->cfg.getRuntimeData        = EI_APP_DIO_DEVICE_CFG_getRuntimeData;
    pIntf->cfg.getFactoryResetData   = EI_APP_DIO_DEVICE_CFG_getFactoryResetData;
    pIntf->cfg.getLength             = EI_APP_DIO_DEVICE_CFG_getLength;

    error = EI_APP_DEVICE_PROFILE_ERR_OK;

laError:
    return error;
}

/*!
 *
 * \brief
 * Basic initialization function.
 *
 * \details
 * Creates a new EtherNet/IP&trade; Adapter.<br>
 * Initializes data structures from non-volatile storage.<br>
 * Registers stack error handler.<br>
 * Initializes the Adapter.<br>
 * Create a CIP&trade; node.<br>
 *
 * \param[in]  pCipNode Pointer to the CIP node.
 *
 * \return     result as boolean.
 *
 * \retval     #true    Success.
 * \retval     #false   Error.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * bool result = false;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * result = EI_APP_DIO_DEVICE_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \see EI_API_ADP_new  EI_API_CIP_NODE_new  EI_API_CIP_EError_t
 *
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
bool EI_APP_DIO_DEVICE_init(EI_API_ADP_T *pAdapter, EI_API_CIP_NODE_T *pCipNode)
{
    bool result = false;

    // Init Discrete IO Device specific objects.
    EI_APP_DOG_init(pCipNode);

    EI_APP_DIP_init(pCipNode);

    // Init DOP State Machine
    EI_APP_DOP_SM_init();

    // Create vendor specific classes.
    result = EI_APP_DIO_DEVICE_cipSetup(pCipNode);

    return result;
}

/*!
 *
 * \brief
 * Cyclically called run function.
 *
 * \details
 * Cyclically called run function, handles the EtherNet/IP stack and hardware
 * specific functionality, such as reading switches, reading inputs, setting outputs
 * and LEDs.
 *
 * \param[in]  pCipNode Pointer to the CIP node.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * EI_APP_DIO_DEVICE_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
void EI_APP_DIO_DEVICE_run(EI_API_CIP_NODE_T* pCipNode)
{
    // Run Discrete IO Device specific objects.
    EI_APP_DOG_run(pCipNode);
    EI_APP_DIP_run(pCipNode);

    // Run DOP State Machine
    EI_APP_DOP_SM_run();
}

/*!
 *
 * \brief
 * Configuration Assembly callback.
 *
 * \details
 * This function is used to get the configuration assembly.
 *
 * \param[in]  pCipNode                 Pointer to the CIP node.
 * \param[in]  o2t                      Originator to target.
 * \param[in]  t2o                      Target to originator.
 * \param[in]  configurationInstance    Configuration assembly instance.
 * \param[in]  configurationData        Configuration data.
 * \param[in]  configurationSize        Configuration data size.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Create configuration assembly & use it with the callback
 * errCode = EI_API_CIP_createCfgAssemblySimple(pEI_API_CIP_NODE, 0x66, EI_APP_DOG_configurationAssemblyCb);
 *
 * \endcode
 *
 * \see EI_API_CIP_createCfgAssemblySimple  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
uint32_t EI_APP_DIO_DEVICE_configurationAssemblyCb(
                                    EI_API_CIP_NODE_T* pCipNode,
                                    uint16_t           o2t,
                                    uint16_t           t2o,
                                    uint16_t           configurationInstance,
                                    const uint8_t*     configurationData,
                                    uint16_t           configurationSize)
{
    for(int i = 0; i < configurationSize; i++)
    {
        /* Bit 0 = Fault Action */
        if(EI_APP_DIO_DEVICE_faultActionConfiguration_s != configurationData[EI_APP_DIO_DEVICE_FAULT_ACTION_BIT])
        {
            EI_APP_DIO_DEVICE_faultActionConfiguration_s = configurationData[EI_APP_DIO_DEVICE_FAULT_ACTION_BIT];
        }
        /* Bit 1 = Idle Action */
        if(EI_APP_DIO_DEVICE_idleActionConfiguration_s != configurationData[EI_APP_DIO_DEVICE_IDLE_ACTION_BIT])
        {
            EI_APP_DIO_DEVICE_idleActionConfiguration_s = configurationData[EI_APP_DIO_DEVICE_IDLE_ACTION_BIT];
        }
    }

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Gets configuration assembly.
 *
 * \details
 * This function used to get configuration assembly.
 *
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * uint32_t faultAction = 0;
 * uint32_t idleAction  = 0;
 *
 * EI_APP_DIO_DEVICE_getConfigurationAssemblyCb(&faultAction, &idleAction)
 *
 * \endcode
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
void EI_APP_DIO_DEVICE_getConfigurationAssemblyCb(uint8_t *faultAction, uint8_t *idleAction)
{
    *faultAction = EI_APP_DIO_DEVICE_faultActionConfiguration_s;
    *idleAction  = EI_APP_DIO_DEVICE_idleActionConfiguration_s;
}

/*!
 *
 * \brief
 * Setup the application with classes, instances, attributes, and assemblies.
 *
 * \details
 * Setup the application with classes, instances, attributes, and assemblies.<br>
 * For the assemblies, use instances in the Vendor Specific range of IDs.
 *
 * \param[in]  pCipNode Pointer to the CIP node.
 *
 * \return     result as boolean.
 *
 * \retval     #true    Success.
 * \retval     #false   Error.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * bool result = false;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * result = EI_APP_DIO_DEVICE_cipSetup(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
static bool EI_APP_DIO_DEVICE_cipSetup(EI_API_CIP_NODE_T* pCipNode)
{

    uint32_t errCode = EI_API_CIP_eERR_OK;

    errCode = EI_API_CIP_createAssembly(pCipNode, 0xFE, EI_API_CIP_eAR_GET); // Input-only.
    errCode = EI_API_CIP_createAssembly(pCipNode, 0xFF, EI_API_CIP_eAR_GET); // Listen-only.

    /* Create assembly instance & add its assembly members */
    errCode = EI_API_CIP_createAssembly(pCipNode, EI_APP_DIO_DEVICE_ASSEMBLY_PRODUCING, EI_API_CIP_eAR_GET_AND_SET);
    errCode = EI_API_CIP_createAssembly(pCipNode, EI_APP_DIO_DEVICE_ASSEMBLY_CONSUMING, EI_API_CIP_eAR_GET_AND_SET);
    errCode = EI_API_CIP_createAssembly(pCipNode, EI_APP_DIO_DEVICE_ASSEMBLY_CONFIGURATION, EI_API_CIP_eAR_GET_AND_SET);

    /* Create configuration assembly */
    errCode = EI_API_CIP_createCfgAssemblySimple(
                                                pCipNode,
                                                EI_APP_DIO_DEVICE_ASSEMBLY_CONFIGURATION,
                                                EI_APP_DIO_DEVICE_configurationAssemblyCb);

    /* Add DIP intances to producing assembly */
    for(uint8_t instanceId = 1; instanceId <= EI_APP_DIO_DEVICE_DIP_NUM_OF_INST; instanceId++)
    {
        errCode = EI_API_CIP_addAssemblyMember(
                                              pCipNode,
                                              EI_APP_DIO_DEVICE_ASSEMBLY_PRODUCING,
                                              EI_APP_DIO_DEVICE_DIP_CLASS_ID,
                                              instanceId,
                                              EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x64:  Error code: 0x%08x\r\n",
                        EI_APP_DIO_DEVICE_DIP_CLASS_ID,
                        instanceId,
                        EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03,
                        errCode);
        }
    }

    /* Add DOP instances to consuming and configuration assembly */
    for(uint8_t instanceId = 1; instanceId <= EI_APP_DIO_DEVICE_DOP_NUM_OF_INST; instanceId++)
    {
        errCode = EI_API_CIP_addAssemblyMember(
                                              pCipNode,
                                              EI_APP_DIO_DEVICE_ASSEMBLY_CONSUMING,
                                              EI_APP_DIO_DEVICE_DOP_CLASS_ID,
                                              instanceId,
                                              EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x65:  Error code: 0x%08x\r\n",
                        EI_APP_DIO_DEVICE_DOP_CLASS_ID,
                        instanceId,
                        EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03,
                        errCode);
        }
    }

    /* Add DOG instances to configuration assembly */
    errCode = EI_API_CIP_addAssemblyMember(
                                          pCipNode,
                                          EI_APP_DIO_DEVICE_ASSEMBLY_CONFIGURATION,
                                          EI_APP_DIO_DEVICE_DOG_CLASS_ID,
                                          0x01,
                                          EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_07);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x66:  Error code: 0x%08x\r\n",
                    EI_APP_DIO_DEVICE_DOG_CLASS_ID,
                    0x01,
                    EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_07,
                    errCode);
    }

    /* Add DOG instances to configuration assembly */
    errCode = EI_API_CIP_addAssemblyMember(
                                          pCipNode,
                                          EI_APP_DIO_DEVICE_ASSEMBLY_CONFIGURATION,
                                          EI_APP_DIO_DEVICE_DOG_CLASS_ID,
                                          0x01,
                                          EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_09);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x66:  Error code: 0x%08x\r\n",
                    EI_APP_DIO_DEVICE_DOG_CLASS_ID,
                    0x01,
                    EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_09,
                    errCode);
    }

    return true;
}

#endif  // (!(defined FBTLPROVIDER) || (0 == FBTLPROVIDER)) && (!(defined FBTL_REMOTE) || (0 == FBTL_REMOTE))

//*************************************************************************************************
