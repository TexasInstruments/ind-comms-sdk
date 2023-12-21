/*!
 *  \file app_device_profile.c
 *
 *  \brief
 *  Device profile interface implementation.
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "EI_API.h"
#include "EI_API_def.h"

#include "appCfg.h"

#include "device_profiles/app_device_profile.h"
#include "device_profiles/app_device_profile_intern.h"

static EI_APP_DEVICE_PROFILE_Interface_t EI_APP_DEVICE_PROFILE_intf_s = {NULL};

/*!
 * \brief
 * Initialization of device profile interface.
 *
 * \details
 * Calls register function to initialize interface functions.
 *
 * \remarks
 * The name of register function is fixed to 'EI_APP_DEVICE_PROFILE_register'.
 * Each device profile needs to provide this register function.
 * In build time just one device profile can be active.
 *
 * \return        #EI_APP_DEVICE_PROFILE_Err_t as uint32_t.
 * \retval        #EI_APP_DEVICE_PROFILE_ERR_OK            Success.
 * \retval        #EI_APP_DEVICE_PROFILE_ERR_General       General error, initial value.
 * \retval        #EI_APP_DEVICE_PROFILE_ERR_PtrInvalid    Invalid pointer error.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/app_device_profile_intern.h"
 *
 * uint32_t err = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 * err = EI_APP_DEVICE_PROFILE_init();
 *
 * if (EI_APP_DEVICE_PROFILE_ERR_OK != err)
 * {
 *     OSAL_printf("Fatal error: Device profile initialization failed\n");
 *     return;
 * }
 *
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
uint32_t EI_APP_DEVICE_PROFILE_init (void)
{
    return EI_APP_DEVICE_PROFILE_register(&EI_APP_DEVICE_PROFILE_intf_s);
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
 * #include "device_profiles/app_device_profile.h"
 *
 * bool               ret      = false;
 * EI_API_ADP_T*      pAdapter = EI_API_ADP_new(1);
 * EI_API_CIP_NODE_T* pCipNode = EI_API_CIP_NODE_new();
 *
 * if (false = EI_APP_DEVICE_PROFILE_TASK_init(pAdapter, pCipNode))
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
bool EI_APP_DEVICE_PROFILE_TASK_init (EI_API_ADP_T *pAdapter, EI_API_CIP_NODE_T *pCipNode)
{
    bool ret = false;

    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.task.init)
    {
        ret = EI_APP_DEVICE_PROFILE_intf_s.task.init(pAdapter, pCipNode);
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
 * #include "device_profiles/app_device_profile.h"
 *
 * EI_API_CIP_NODE_T* pCipNode = EI_API_CIP_NODE_new();
 *
 * for (;;)
 * {
 *     EI_API_ADP_run();
 *
 *     EI_APP_DEVICE_PROFILE_TASK_run(pCipNode);
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
void EI_APP_DEVICE_PROFILE_TASK_run (EI_API_CIP_NODE_T *pCipNode)
{
    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.task.run)
    {
        EI_APP_DEVICE_PROFILE_intf_s.task.run(pCipNode);
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
 * #include "device_profiles/app_device_profile.h"
 *
 * EI_API_ADP_T* pAdapter = EI_API_ADP_new(1);
 *
 * if (false = EI_APP_DEVICE_PROFILE_CFG_init(pAdapter))
 * {
 *     OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
 * }
 *
 * \endcode
 *
 * \see EI_APP_CFG_init
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
bool EI_APP_DEVICE_PROFILE_CFG_init (EI_API_ADP_T *pAdapter)
{
    bool ret = false;

    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.init)
    {
        ret = EI_APP_DEVICE_PROFILE_intf_s.cfg.init(pAdapter);
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
 * #include "device_profiles/app_device_profile.h"
 *
 * EI_APP_DEVICE_PROFILE_CFG_setDefaultWithoutComm();
 *
 * \endcode
 *
 * \see EI_APP_CFG_setDefaultWithoutComm
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void EI_APP_DEVICE_PROFILE_CFG_setDefaultWithoutComm (void)
{
    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.setDefaultWithoutComm)
    {
        EI_APP_DEVICE_PROFILE_intf_s.cfg.setDefaultWithoutComm();
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
 * #include "appCfg.h"
 * #include "device_profiles/app_device_profile.h"
 *
 * EI_APP_CFG_Data_t *pCfgData = (EI_APP_CFG_Data_t*) EI_APP_CFG_getRuntimeData();
 *
 * EI_APP_DEVICE_PROFILE_CFG_setHeader(&pCfgData->header);
 *
 * \endcode
 *
 * \see EI_APP_CFG_setHeader
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void EI_APP_DEVICE_PROFILE_CFG_setHeader (EI_APP_CFG_Header_t *pHeader)
{
    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.setHeader)
    {
        EI_APP_DEVICE_PROFILE_intf_s.cfg.setHeader(pHeader);
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
 * #include "device_profiles/app_device_profile.h"
 *
 * // fill runtime configuration with data from non volatile memory
 * EI_APP_NV_read();
 *
 * if (false == EI_APP_DEVICE_PROFILE_CFG_isValid())
 * {
 *     // non volatile data are corrupted
 * }
 *
 * \endcode
 *
 * \see EI_APP_CFG_isValid
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
bool EI_APP_DEVICE_PROFILE_CFG_isValid (void)
{
    bool ret = false;

    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.isValid)
    {
        ret = EI_APP_DEVICE_PROFILE_intf_s.cfg.isValid();
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
 * #include "device_profiles/app_device_profile.h"
 *
 * EI_API_ADP_T* pAdapter = EI_API_ADP_new(1);
 *
 * EI_APP_DEVICE_PROFILE_CFG_apply(pAdapter);
 *
 * \endcode
 *
 * \see EI_APP_CFG_apply
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void EI_APP_DEVICE_PROFILE_CFG_apply (EI_API_ADP_T *pAdapter)
{
    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.apply)
    {
        EI_APP_DEVICE_PROFILE_intf_s.cfg.apply(pAdapter);
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
 * #include "device_profiles/app_device_profile.h"
 *
 * void EI_APP_CFG_callback ( EI_API_CIP_NODE_T *pCipNode,
 *                            uint16_t           classId,
 *                            uint16_t           instanceId,
 *                            uint16_t           attrId,
 *                            EI_API_CIP_ESc_t   serviceCode,
 *                            int16_t            serviceFlag)
 * {
 *     bool isChnaged = false;
 *     ...
 *
 *     isChanged = EI_APP_DEVICE_PROFILE_CFG_callback(pCipNode, classId, instanceId, attrId, serviceCode, serviceFlag);
 *
 *     ...
 *
 * }
 *
 * \endcode
 *
 * \see EI_APP_CFG_callback
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
bool EI_APP_DEVICE_PROFILE_CFG_callback (EI_API_CIP_NODE_T *pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, EI_API_CIP_ESc_t serviceCode, int16_t serviceFlag)
{
    bool ret = false;

    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.callback)
    {
        ret = EI_APP_DEVICE_PROFILE_intf_s.cfg.callback(pCipNode, classId, instanceId, attrId, serviceCode, serviceFlag);
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
 * #include "appCfg.h"
 * #include "device_profiles/app_device_profile.h"
 *
 * // reset runtime non volatile configuration data with factory reset values
 * EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();
 * EI_APP_CFG_Data_t *pResetData   = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData();
 * uint32_t           length       = EI_APP_DEVICE_PROFILE_CFG_getLength();
 *
 * OSAL_MEMORY_memcpy(pRuntimeData, pResetData, length);
 *
 * \endcode
 *
 * \see EI_APP_CFG_getRuntimeData
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void* EI_APP_DEVICE_PROFILE_CFG_getRuntimeData (void)
{
    void* pRet = NULL;

    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.getRuntimeData)
    {
        pRet = EI_APP_DEVICE_PROFILE_intf_s.cfg.getRuntimeData();
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
 * #include "appCfg.h"
 * #include "device_profiles/app_device_profile.h"
 *
 * // reset runtime non volatile configuration data with factory reset values
 * EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();
 * EI_APP_CFG_Data_t *pResetData   = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData();
 * uint32_t           length       = EI_APP_DEVICE_PROFILE_CFG_getLength();
 *
 * OSAL_MEMORY_memcpy(pRuntimeData, pResetData, length);
 *
 * \endcode
 *
 * \see EI_APP_CFG_getFactoryResetData
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
void* EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData (void)
{
    void* pRet = NULL;

    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.getFactoryResetData)
    {
        pRet = EI_APP_DEVICE_PROFILE_intf_s.cfg.getFactoryResetData();
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
 * #include "appCfg.h"
 * #include "device_profiles/app_device_profile.h"
 *
 * // reset runtime non volatile configuration data with factory reset values
 * EI_APP_CFG_Data_t *pRuntimeData = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getRuntimeData();
 * EI_APP_CFG_Data_t *pResetData   = (EI_APP_CFG_Data_t*) EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData();
 * uint32_t           length       = EI_APP_DEVICE_PROFILE_CFG_getLength();
 *
 * OSAL_MEMORY_memcpy(pRuntimeData, pResetData, length);
 *
 * \endcode
 *
 * \see EI_APP_CFG_getFactoryResetData
 *
 * \ingroup EI_APP_DEVICE_PROFILE
 *
 */
uint32_t EI_APP_DEVICE_PROFILE_CFG_getLength (void)
{
    uint32_t ret = 0;

    if (NULL != EI_APP_DEVICE_PROFILE_intf_s.cfg.getLength)
    {
        ret = EI_APP_DEVICE_PROFILE_intf_s.cfg.getLength();
    }
    else
    {
        OSAL_printf("Device profile configuration interface doesn't provide \"getLength\" function.\n");
    }

    return ret;
}
