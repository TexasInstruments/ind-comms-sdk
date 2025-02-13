/*!
 *  \file device_profile_intf.c
 *
 *  \brief
 *  Device profile interface implementation.
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "EI_API.h"
#include "EI_API_def.h"

#include "device_profiles/common/device_profile_nvm.h"
#include "device_profiles/common/device_profile_cfg.h"
#include "device_profiles/device_profile_intf.h"
#include "device_profiles/device_profile_intf_intern.h"

static DEVICE_PROFILE_INTF_Interface_t DEVICE_PROFILE_INTF_intf_s = {NULL};

/*!
 * \brief
 * Initialization of device profile interface.
 *
 * \details
 * Calls register function to initialize interface functions.
 *
 * \remarks
 * The name of register function is fixed to 'DEVICE_PROFILE_register'.
 * Each device profile needs to provide this register function.
 * In build time just one device profile can be active.
 *
 * \return        #DEVICE_PROFILE_INTF_Err_t as uint32_t.
 * \retval        #DEVICE_PROFILE_INTF_ERR_OK            Success.
 * \retval        #DEVICE_PROFILE_INTF_ERR_General       General error, initial value.
 * \retval        #DEVICE_PROFILE_INTF_ERR_PtrInvalid    Invalid pointer error.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/device_profile_intf_intern.h"
 *
 * uint32_t err = DEVICE_PROFILE_INTF_ERR_General;
 *
 * err = DEVICE_PROFILE_INTF_init();
 *
 * if (DEVICE_PROFILE_INTF_ERR_OK != err)
 * {
 *     OSAL_printf("Fatal error: Device profile initialization failed\n");
 *     return;
 * }
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_register
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
uint32_t DEVICE_PROFILE_INTF_init (void)
{
    return DEVICE_PROFILE_register(&DEVICE_PROFILE_INTF_intf_s);
}

/*!
 * \brief
 * Device profile interface function to provide initialization necessary for main task.
 *
 * \param         pAdapter    Pointer to EtherNet/IP adapter object.
 * \param         pCipNode    Pointer to the CIP node.
 *
 * \return        value as bool.
 * \retval        true            Success.
 * \retval        false           Initialization of CIP objects failed.
  *
 * \par Example
 * \code{.c}
 * #include "device_profiles/device_profile_intf.h"
 *
 * bool               ret      = false;
 * EI_API_ADP_T*      pAdapter = EI_API_ADP_new(1);
 *
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * EI_API_CIP_NODE_T* pCipNode = EI_API_CIP_NODE_new(&initParams);
 *
 * if (false = DEVICE_PROFILE_INTF_taskInit(pAdapter, pCipNode))
 * {
 *     OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
 * }
 *
 * \endcode
 *
 * \see EI_APP_TASK_init
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
bool DEVICE_PROFILE_INTF_taskInit (EI_API_ADP_T *pAdapter, EI_API_CIP_NODE_T *pCipNode)
{
    bool ret = false;

    if (NULL != DEVICE_PROFILE_INTF_intf_s.task.init)
    {
        ret = DEVICE_PROFILE_INTF_intf_s.task.init(pAdapter, pCipNode);
    }
    else
    {
        OSAL_printf("Device profile task interface doesn't provide \"init\" function.\n");
    }

    return ret;
}

/*!
 * \brief
 * Device profile interface function to execute run. Called cyclically.
 *
 * \param      pCipNode    Pointer to the CIP node.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/device_profile_intf.h"
 *
 *
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * EI_API_CIP_NODE_T* pCipNode = EI_API_CIP_NODE_new(&initParams);
 *
 * for (;;)
 * {
 *     EI_API_ADP_run();
 *
 *     DEVICE_PROFILE_INTF_taskRun(pCipNode);
 *
 *     OSAL_SCHED_yield();
 * }
 *
 * \endcode
 *
 * \see EI_APP_TASK_run
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void DEVICE_PROFILE_INTF_taskRun (EI_API_CIP_NODE_T *pCipNode)
{
    if (NULL != DEVICE_PROFILE_INTF_intf_s.task.run)
    {
        DEVICE_PROFILE_INTF_intf_s.task.run(pCipNode);
    }
    else
    {
        OSAL_printf("Device profile task interface doesn't provide \"run\" function.\n");
    }
}

/*!
 * \brief
 * Device profile interface function to initialize configuration.
 *
 * \param      pAdapter    Pointer to EtherNet/IP adapter object.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/device_profile_intf.h"
 *
 * EI_API_ADP_T* pAdapter = EI_API_ADP_new(1);
 *
 * if (false = DEVICE_PROFILE_INTF_cfgInit(pAdapter))
 * {
 *     OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
 * }
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_init
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
bool DEVICE_PROFILE_INTF_cfgInit (EI_API_ADP_T *pAdapter)
{
    bool ret = false;

    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.init)
    {
        ret = DEVICE_PROFILE_INTF_intf_s.cfg.init(pAdapter);
    }
    else
    {
        OSAL_printf("Device profile non volatile interface doesn't provide \"init\" function.\n");
    }

    return ret;
}

/*!
 * \brief
 * Device profile interface function to set default values except communication parameters.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/device_profile_intf.h"
 *
 * DEVICE_PROFILE_INTF_cfgSetDefaultWithoutComm();
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_setDefaultWithoutComm
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void DEVICE_PROFILE_INTF_cfgSetDefaultWithoutComm (void)
{
    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.setDefaultWithoutComm)
    {
        DEVICE_PROFILE_INTF_intf_s.cfg.setDefaultWithoutComm();
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"setDefaultWithoutComm\" function.\n");
    }

    return;
}

/*!
 * \brief
 * Device profile interface function to set header value for non volatile configuration parameters.
 *
 * \remark
 * The header is used for validation of configuration data stored in non volatile memory.
 *
 * \par Example
 * \code{.c}
 *
 * #include "device_profiles/common/device_profile_cfg.h"
 * #include "device_profiles/device_profile_intf.h"
 *
 * DEVICE_PROFILE_CFG_Data_t *pCfgData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_CFG_getRuntimeData();
 *
 * DEVICE_PROFILE_INTF_cfgSetHeader(&pCfgData->header);
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_setHeader
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void DEVICE_PROFILE_INTF_cfgSetHeader (DEVICE_PROFILE_CFG_Header_t *pHeader)
{
    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.setHeader)
    {
        DEVICE_PROFILE_INTF_intf_s.cfg.setHeader(pHeader);
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"setHeader\" function.\n");
    }
}

/*!
 * \brief
 * Device profile interface function to provide result of validation of non volatile data.
 *
 * \details
 * Validation check is done on runtime values of non volatile configuration data.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/device_profile_intf.h"
 *
 * // fill runtime configuration with data from non volatile memory
 * DEVICE_PROFILE_NVM_cfgRead();
 *
 * if (false == DEVICE_PROFILE_INTF_cfgIsValid())
 * {
 *     // non volatile data are corrupted
 * }
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_isValid
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
bool DEVICE_PROFILE_INTF_cfgIsValid (void)
{
    bool ret = false;

    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.isValid)
    {
        ret = DEVICE_PROFILE_INTF_intf_s.cfg.isValid();
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"isValid\" function.\n");
    }

    return ret;
}

/*!
 * \brief
 * Device profile interface function to apply non volatile configuration data on stack.
 *
 * \details
 * Use this function to setup the stack with device profile specific configuration values.
 *
 * \remark
 * The EtherNet/IP Adapter configuration is applied on other place.
 *
 * \param      pAdapter    Pointer to EtherNet/IP adapter object.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/device_profile_intf.h"
 *
 * EI_API_ADP_T* pAdapter = EI_API_ADP_new(1);
 *
 * DEVICE_PROFILE_INTF_cfgApply(pAdapter);
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_apply
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void DEVICE_PROFILE_INTF_cfgApply (EI_API_ADP_T *pAdapter)
{
    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.apply)
    {
        DEVICE_PROFILE_INTF_intf_s.cfg.apply(pAdapter);
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"apply\" function.\n");
    }

    return;
}

/*!
 * \brief
 * Device profile interface function used as callback for device profile specific configuration.
 *
 * \param[in]     pCipNode    Pointer to CIP object.
 * \param[in]     classId     Class identifier.
 * \param[in]     instanceId  Instance identifier.
 * \param[in]     attrId      Attribute identifier.
 * \param[in]     serviceCode Service code.
 * \param[in]     serviceFlag Service flag.
 *
 * \return        value as bool.
 * \retval        true     Non volatile discrete IO device profile configuration part was changed.
 * \retval        false    No change.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/device_profile_intf.h"
 *
 * void DEVICE_PROFILE_CFG_callback ( EI_API_CIP_NODE_T *pCipNode,
 *                            uint16_t           classId,
 *                            uint16_t           instanceId,
 *                            uint16_t           attrId,
 *                            EI_API_CIP_ESc_t   serviceCode,
 *                            int16_t            serviceFlag)
 * {
 *     bool isChnaged = false;
 *     ...
 *
 *     isChanged = DEVICE_PROFILE_INTF_cfgCallback(pCipNode, classId, instanceId, attrId, serviceCode, serviceFlag);
 *
 *     ...
 *
 * }
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_callback
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
bool DEVICE_PROFILE_INTF_cfgCallback (EI_API_CIP_NODE_T *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag)
{
    bool ret = false;

    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.callback)
    {
        ret = DEVICE_PROFILE_INTF_intf_s.cfg.callback(pCipNode, classId, instanceId, attrId, serviceCode, serviceFlag);
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"callback\" function.\n");
    }

    return ret;
}

/*!
 * \brief
 * Device profile interface function to provide pointer to runtime configuration data.
 *
 * \return        pointer to runtime configuration non volatile data.
 *
 * \par Example
 * \code{.c}
 *
 * #include "device_profiles/common/device_profile_cfg.h"
 * #include "device_profiles/device_profile_intf.h"
 *
 * // reset runtime non volatile configuration data with factory reset values
 * DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();
 * DEVICE_PROFILE_CFG_Data_t *pResetData   = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetFactoryResetData();
 * uint32_t           length       = DEVICE_PROFILE_INTF_cfgGetLength();
 *
 * OSAL_MEMORY_memcpy(pRuntimeData, pResetData, length);
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_getRuntimeData
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void* DEVICE_PROFILE_INTF_cfgGetRuntimeData (void)
{
    void* pRet = NULL;

    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.getRuntimeData)
    {
        pRet = DEVICE_PROFILE_INTF_intf_s.cfg.getRuntimeData();
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"getRuntimeData\" function.\n");
    }

    return pRet;
}

/*!
 * \brief
 * Device profile interface function to provide pointer to factory reset configuration data.
 *
 * \return        pointer to factory reset configuration non volatile data.
 *
 * \par Example
 * \code{.c}
 *
 * #include "device_profiles/common/device_profile_cfg.h"
 * #include "device_profiles/device_profile_intf.h"
 *
 * // reset runtime non volatile configuration data with factory reset values
 * DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();
 * DEVICE_PROFILE_CFG_Data_t *pResetData   = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetFactoryResetData();
 * uint32_t           length       = DEVICE_PROFILE_INTF_cfgGetLength();
 *
 * OSAL_MEMORY_memcpy(pRuntimeData, pResetData, length);
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_getFactoryResetData
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void* DEVICE_PROFILE_INTF_cfgGetFactoryResetData (void)
{
    void* pRet = NULL;

    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.getFactoryResetData)
    {
        pRet = DEVICE_PROFILE_INTF_intf_s.cfg.getFactoryResetData();
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"getFactoryResetData\" function.\n");
    }

    return pRet;
}

/*!
 * \brief
 * Device profile interface function to provide length of configuration data stored in non volatile memory.
 *
 * \return       value as uint32_t
 * \retval       Size of configuration non volatile data.
 *
 * \par Example
 * \code{.c}
 *
 * #include "device_profiles/common/device_profile_cfg.h"
 * #include "device_profiles/device_profile_intf.h"
 *
 * // reset runtime non volatile configuration data with factory reset values
 * DEVICE_PROFILE_CFG_Data_t *pRuntimeData = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetRuntimeData();
 * DEVICE_PROFILE_CFG_Data_t *pResetData   = (DEVICE_PROFILE_CFG_Data_t*) DEVICE_PROFILE_INTF_cfgGetFactoryResetData();
 * uint32_t           length       = DEVICE_PROFILE_INTF_cfgGetLength();
 *
 * OSAL_MEMORY_memcpy(pRuntimeData, pResetData, length);
 *
 * \endcode
 *
 * \see DEVICE_PROFILE_CFG_getFactoryResetData
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
uint32_t DEVICE_PROFILE_INTF_cfgGetLength (void)
{
    uint32_t ret = 0;

    if (NULL != DEVICE_PROFILE_INTF_intf_s.cfg.getLength)
    {
        ret = DEVICE_PROFILE_INTF_intf_s.cfg.getLength();
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"getLength\" function.\n");
    }

    return ret;
}
