/*!
 *  \file discrete_io_device.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application for Device Profile: General Purpose of Discrete IO.
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

#if (!(defined FBTLPROVIDER) || (0 == FBTLPROVIDER)) && (!(defined FBTL_REMOTE) || (0 == FBTL_REMOTE))

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#include "osal.h"
#include "osal_error.h"
#include "hwal.h"

#include "EI_API.h"
#include "EI_API_def.h"

#include "cfg_example.h"

#include "cust_drivers.h"

#include "drivers/drivers.h"

#include "cmn_board.h"
#include "cmn_os.h"
#include "cmn_app.h"
#include "cmn_mem.h"
#include "cmn_cpu_api.h"

#include "device_profiles/common/device_profile_cip_codes.h"
#include "device_profiles/common/device_profile_nvm.h"
#include "device_profiles/common/device_profile_cfg.h"
#include "web_server.h"
#include "app_task.h"
#include "app.h"

#include "device_profiles/device_profile_intf.h"
#include "device_profiles/device_profile_intf_intern.h"
#include "device_profiles/discrete_io_device/discrete_io_device_cfg.h"
#include "device_profiles/discrete_io_device/discrete_io_device.h"
#include "device_profiles/discrete_io_device/discrete_io_device_dip.h"
#include "device_profiles/discrete_io_device/discrete_io_device_dog.h"
#include "device_profiles/discrete_io_device/discrete_io_device_dop.h"
#include "device_profiles/discrete_io_device/discrete_io_device_asm.h"

static bool DIO_DEVICE_init     (EI_API_ADP_T      *pAdapter, EI_API_CIP_NODE_T *pCipNode);
static void DIO_DEVICE_run      (EI_API_CIP_NODE_T *pCipNode);

static bool DIO_DEVICE_cipSetup (EI_API_CIP_NODE_T *pCipNode);

static DIO_DEVICE_ConnectionState_t DIO_DEVICE_connectionState_s = DIO_DEVICE_ConnectionNotEstablished;
static EI_API_ADP_SModNetStatus_t DIO_DEVICE_modnetStatus_s    = {0};
static volatile uint32_t DIO_DEVICE_dopLedStatus_s = 0;


static uint16_t DIO_DEVICE_extendedStatus_s[255] = {0};

static uint8_t DIO_DEVICE_producedConnectionPath[EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH] =
{
        0x20, 0x04, 0x24,
        CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONFIGURATION, 0x2C,
        CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONSUMING, 0x2C,
        CFG_PROFILE_DIO_DEVICE_ASSEMBLY_PRODUCING
};

static uint8_t DIO_DEVICE_consumedConnectionPath[EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH] =
{
        0x20, 0x04, 0x24,
        CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONFIGURATION, 0x2C,
        CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONSUMING, 0x2C,
        CFG_PROFILE_DIO_DEVICE_ASSEMBLY_PRODUCING
};

void DIO_DEVICE_ModNetStatusCb(EI_API_ADP_T* pAdp_p, EI_API_ADP_SModNetStatus_t status_p)
{
    OSAL_MEMORY_memcpy(&DIO_DEVICE_modnetStatus_s, &status_p, sizeof(EI_API_ADP_SModNetStatus_t));
}
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
EI_API_ADP_SEipStatus_t DIO_DEVICE_cmgrCb(uint32_t serviceCode, EI_API_ADP_UCmgrInfo_u cmgrInfo)
{
    EI_API_ADP_SEipStatus_t ret_val= {.gen_status=0,
                                      .extended_status_size=0,
                                      .extended_status_arr=DIO_DEVICE_extendedStatus_s};
    uint8_t i;

    switch(serviceCode)
        {
        case 0x54:
            if(EI_APP_DIO_DEVICE_IO_CONNECTION == cmgrInfo.forwardOpenInfo.typeTrigger
                                                    && CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONSUMING == cmgrInfo.forwardOpenInfo.conPath[5])
            {
                DIO_DEVICE_connectionState_s = DIO_DEVICE_ConnectionEstablished;
                for(i = 0; i<CFG_PROFILE_DIO_DEVICE_DOP_NUM_OF_INST; i++)
                {
                    DIO_DEVICE_DOP_receiveConnectionEvent(i+1, DIO_DEVICE_DOP_CONNECTION_ESTABLISHED);
                }
            }
            break;
        case 0x5b:
            break;
        case 0x4e:
            if(CFG_PROFILE_DIO_DEVICE_ASSEMBLY_PRODUCING == cmgrInfo.forwardCloseInfo.conPath[7])
            {
                DIO_DEVICE_connectionState_s = DIO_DEVICE_ConnectionClosed;
                for(i = 0; i<CFG_PROFILE_DIO_DEVICE_DOP_NUM_OF_INST; i++)
                {
                    DIO_DEVICE_DOP_receiveConnectionEvent(i+1, DIO_DEVICE_DOP_CONNECTION_DELETED);
                }
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
void DIO_DEVICE_cobjTimeOutCb(
        uint8_t *producedConnectionPath,
        uint16_t producedConnectionPathLen,
        uint8_t *consumedConnectionPath,
        uint16_t consumedConnectionPathLen)
{

    uint8_t i;

    /* Check if the Connection Paths are correct */
    if((0 == OSAL_MEMORY_memcmp(producedConnectionPath, DIO_DEVICE_producedConnectionPath, EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH))
            || (0 == OSAL_MEMORY_memcmp(consumedConnectionPath, DIO_DEVICE_consumedConnectionPath, EI_APP_DIO_DEVICE_CONNECTION_PATH_LENGTH)))
    {
        for(i = 0; i<CFG_PROFILE_DIO_DEVICE_DOP_NUM_OF_INST; i++)
        {
            DIO_DEVICE_DOP_receiveConnectionEvent(i+1, DIO_DEVICE_DOP_CONNECTION_TIMEDOUT);
        }
    }

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
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * uint32_t connectionState = 0;
 * uint32_t receiveData     = 0;
 *
 * DIO_DEVICE_getConnectionInfo(&connectionState, &receiveData)
 *
 * \endcode
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
void DIO_DEVICE_getConnectionInfo(DIO_DEVICE_ConnectionState_t *pConnectionState, EI_API_ADP_SModNetStatus_t *pModNetStatus)
{
    *pConnectionState = DIO_DEVICE_connectionState_s;
    pModNetStatus->mod = DIO_DEVICE_modnetStatus_s.mod;
    pModNetStatus->net = DIO_DEVICE_modnetStatus_s.net;
}

/*!
 * \brief
 * Registers discrete IO device profile interface functions.
 *
 * \details
 * Called by #DEVICE_PROFILE_INTF_init function of device profile interface.
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
 * #include "device_profiles/discrete_io_device/discrete_io_device_cfg.h"
 *
 * uint32_t DEVICE_PROFILE_INTF_init (void)
 * {
 *     return DEVICE_PROFILE_register(&EI_APP_DEVICE_PROFILE_intf_s);
 * }
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_INTF_init
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
uint32_t DEVICE_PROFILE_register (DEVICE_PROFILE_INTF_Interface_t *pIntf)
{
    uint32_t error = DEVICE_PROFILE_INTF_ERR_General;

    if (NULL == pIntf)
    {
        error = DEVICE_PROFILE_INTF_ERR_PtrInvalid;
        goto laError;
    }

    pIntf->task.init = DIO_DEVICE_init;
    pIntf->task.run  = DIO_DEVICE_run;

    pIntf->cfg.init                  = DIO_DEVICE_CFG_init;
    pIntf->cfg.isValid               = DIO_DEVICE_CFG_isValid;
    pIntf->cfg.setHeader             = DIO_DEVICE_CFG_setHeader;
    pIntf->cfg.apply                 = DIO_DEVICE_CFG_apply;
    pIntf->cfg.setDefaultWithoutComm = DIO_DEVICE_CFG_setDefaultWithoutComm;
    pIntf->cfg.callback              = DIO_DEVICE_CFG_callback;
    pIntf->cfg.getRuntimeData        = DIO_DEVICE_CFG_getRuntimeData;
    pIntf->cfg.getFactoryResetData   = DIO_DEVICE_CFG_getFactoryResetData;
    pIntf->cfg.getLength             = DIO_DEVICE_CFG_getLength;

    error = DEVICE_PROFILE_INTF_ERR_OK;

laError:
    return error;
}


/**
 * \brief user function to read out Discrete Input Point value
 * \param[in]  instanceID instance-ID of the DIP object
 * \param[out] pValue pointer to value, to be set by user
 */
static void  DIO_DEVICE_dipGetValue (uint16_t  instanceID, uint8_t *pValue)
{

  uint16_t instanceIndex = instanceID - 1;
  //Mirror Output data into input
  if(DIO_DEVICE_dopLedStatus_s & (1<<instanceIndex))
  {
    *pValue = 1;
  }
  else
  {
    *pValue = 0;
  }
}

/**
 * \brief user function to set Discrete Output Point value
 * \param[in] instanceID Instance-ID of the DOP object
 * \param[in] value the value to be set on the output for the given instance
 */
static void DIO_DEVICE_dopGetValue (uint16_t instanceID, ei_api_cip_edt_bool value)
{
    uint16_t instanceIndex = instanceID - 1;
    if (0 != value)
    {
        DIO_DEVICE_dopLedStatus_s |= (1 << instanceIndex);
    }
    else
    {
        DIO_DEVICE_dopLedStatus_s &= ~(1 << instanceIndex);
    }

    DRV_LED_industrialSet(DIO_DEVICE_dopLedStatus_s);
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
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * bool result = false;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * result = DIO_DEVICE_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \see EI_API_ADP_new  EI_API_CIP_NODE_new  EI_API_CIP_EError_t
 *
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
bool DIO_DEVICE_init(EI_API_ADP_T *pAdapter, EI_API_CIP_NODE_T *pCipNode)
{
    bool result = false;
    uint8_t i;
    DIO_DEVICE_DIP_ObjectCfg_t dipCfg = {0};
    DIO_DEVICE_DOP_ObjectCfg_t dopCfg = {0};
    DIO_DEVICE_DOG_ObjectCfg_t dogCfg = {0};
    // list of Discrete Output Point Object instances to be bound into a Discrete Output Group
    static uint16_t dog_bind_array[] = {1, 2, 3, 4};
    // Init Discrete IO Device specific objects.
    DIO_DEVICE_DIP_init(pCipNode);
    DIO_DEVICE_DOP_init(pCipNode);
    DIO_DEVICE_DOG_init(pCipNode);

    for(i = 0; i<CFG_PROFILE_DIO_DEVICE_DIP_NUM_OF_INST; i++)
    {
        dipCfg.fuGetInput = DIO_DEVICE_dipGetValue;
        dipCfg.fuGetStatus = NULL;
        dipCfg.fuGetEvent = DIO_DEVICE_getConnectionInfo;
        result = DIO_DEVICE_DIP_addObject((i + 1), &dipCfg);
        if(false == result)
        {
            OSAL_printf("%s:%d Failed to create DIP object\r\n", __func__, __LINE__);
            OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        }
    }

    for(i = 0; i<CFG_PROFILE_DIO_DEVICE_DOP_NUM_OF_INST; i++)
    {
        dopCfg.fuSetOutput = DIO_DEVICE_dopGetValue;
        dopCfg.fuGetEvent = DIO_DEVICE_getConnectionInfo;
        dopCfg.FaultAction = 0;
        dopCfg.FaultValue = 0;
        dopCfg.IdleAction = 0;
        dopCfg.IdleValue = 0;
        dopCfg.isBindedToGroup = 1;
        result = DIO_DEVICE_DOP_addObject((i + 1), &dopCfg);
        if(false == result)
        {
            OSAL_printf("%s:%d Failed to create DOP object\r\n", __func__, __LINE__);
            OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        }
    }

    dogCfg.dop_instance_binding_list = dog_bind_array;
    dogCfg.list_len = 4;
    result = DIO_DEVICE_DOG_addObject(1, &dogCfg);
    if(false == result)
    {
        OSAL_printf("%s:%d Failed to create DOG object\r\n", __func__, __LINE__);
        OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    // Create assemlies
    result = DIO_DEVICE_cipSetup(pCipNode);

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
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * DIO_DEVICE_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
void DIO_DEVICE_run(EI_API_CIP_NODE_T* pCipNode)
{
    DIO_DEVICE_DIP_run();
    DIO_DEVICE_DOP_run();
}

/*!
 *
 * \brief
 * Setup the application assemblies.
 *
 * \details
 * Setup the application assemblies.<br>
 * Use General Purpose Discrete I/O Device Profile assemblies.
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
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * bool result = false;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * result = DIO_DEVICE_cipSetup(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DIO_DEVICE
 *
 */
static bool DIO_DEVICE_cipSetup(EI_API_CIP_NODE_T* pCipNode)
{

    bool retVal = true;

    if(EI_API_CIP_eERR_OK != EI_API_CIP_createAssembly(pCipNode, 0xFE, EI_API_CIP_eAR_GET)) // Input-only.
    {
        OSAL_printf("Failed to create assembly Input-Only");
        retVal = false;
    }
    if(EI_API_CIP_eERR_OK != EI_API_CIP_createAssembly(pCipNode, 0xFF, EI_API_CIP_eAR_GET)) // Listen-only.
    {
        OSAL_printf("Failed to create assembly Listen-Only");
        retVal = false;
    }

    if(DIO_DEVICE_ASM_Ok != DIO_DEVICE_ASM_initInstance(pCipNode, CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONFIGURATION, 0))
    {
        OSAL_printf("Failed to create assembly configuration for DOG");
        retVal = false;
    }

    if(DIO_DEVICE_ASM_Ok != DIO_DEVICE_ASM_initInstance(pCipNode, CFG_PROFILE_DIO_DEVICE_ASSEMBLY_PRODUCING, 0))
    {
        OSAL_printf("Failed to create assembly instance for Input points");
        retVal = false;
    }
    if(DIO_DEVICE_ASM_Ok != DIO_DEVICE_ASM_initInstance(pCipNode, CFG_PROFILE_DIO_DEVICE_ASSEMBLY_CONSUMING, 0))
    {
        OSAL_printf("Failed to create assembly instance for Output points");
        retVal = false;
    }

    return retVal;
}

#endif  // (!(defined FBTLPROVIDER) || (0 == FBTLPROVIDER)) && (!(defined FBTL_REMOTE) || (0 == FBTL_REMOTE))

//*************************************************************************************************
