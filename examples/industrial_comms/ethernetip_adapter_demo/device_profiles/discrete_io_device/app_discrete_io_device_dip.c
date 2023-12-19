/*!
 *  \file app_discrete_io_device_dip.c
 *
 *  \brief
 *  EtherNet/IP&trade; Discrete Input Point Object.
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
#include "device_profiles/discrete_io_device/app_discrete_io_device_dip.h"

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

void EI_APP_DIP_init (EI_API_CIP_NODE_T* pCipNode);
void EI_APP_DIP_run  (EI_API_CIP_NODE_T* pCipNode);

static EI_APP_DIP_ClassData_t dipClassData_s = {.revision = EI_APP_DIO_DEVICE_DIP_REVISION_NUMBER
                                               };

/*!
 *
 * \brief
 * Add an attribute for the class instance
 *
 * \details
 * This function adds an attribute for the class instance.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  attrId                                      Attribute identifier.
 * \param[in]  pvValue                                     Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 * \retval     #EI_API_CIP_eERR_CLASS_BLACKLIST            Operation on the CIP object with specified identifier (class ID) is not allowed.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID          Attribute is invalid, possibly NULL pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_ALREADY_EXIST    Attribute already exists in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_NOT_IMPLEMENTED  Attribute function not implemented.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_CDT_SIZE         Passed constructed data type size for e.g. farray not within valid range.
 * \retval     #EI_API_CIP_eERR_MEMALLOC                   Memory allocation error.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_EDT_SIZE         Passed elementary data type size for e.g. short string not within valid range.
 * \retval     #EI_API_CIP_eERR_INSTANCE_UNKNOWN_STATE     Unknown state during class operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint16_t revision = 0x0002; // Create one of the class level attribute
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * errCode = EI_APP_DIP_addClassAttribute(pEI_API_CIP_NODE, 0x0003, &revision);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DIP
 *
 */
static uint32_t EI_APP_DIP_addClassAttribute(EI_API_CIP_NODE_T* pCipNode, uint16_t attrId, void* pvValue)
{
    uint32_t errCode;
    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));

    attr.id = attrId;
    attr.edt = EI_API_CIP_eEDT_UINT;
    attr.accessRule = EI_API_CIP_eAR_GET;
    attr.pvValue = pvValue;
    errCode = EI_API_CIP_addClassAttr(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, &attr);
    if(errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }
    errCode = EI_API_CIP_setClassAttr(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

laError:
    return errCode;
}

/*!
 *
 * \brief
 * Add an attribute for the instance
 *
 * \details
 * This function adds an attribute with the data type, the access rule, callback
 * functions for set and get attribute service and the initial value.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  attrId                                      Attribute identifier.
 * \param[in]  edt                                         Data type. (as enum)
 * \param[in]  accessRule                                  Available attribute access rules. (as enum)
 * \param[in]  getCb                                       Function prototype for CIP get attribute callback function.
 * \param[in]  setCb                                       Function prototype for CIP set attribute callback function.
 * \param[in]  len                                         Data type length.
 * \param[in]  pvValue                                     Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 * \retval     #EI_API_CIP_eERR_CLASS_BLACKLIST            Operation on the CIP object with specified identifier (class ID) is not allowed.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID          Attribute is invalid, possibly NULL pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_ALREADY_EXIST    Attribute already exists in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_NOT_IMPLEMENTED  Attribute function not implemented.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_CDT_SIZE         Passed constructed data type size for e.g. farray not within valid range.
 * \retval     #EI_API_CIP_eERR_MEMALLOC                   Memory allocation error.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_EDT_SIZE         Passed elementary data type size for e.g. short string not within valid range.
 * \retval     #EI_API_CIP_eERR_INSTANCE_UNKNOWN_STATE     Unknown state during class operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * // Callback function for the get service
 * uint32_t EI_APP_CLASS_GetCb(...)
 * {
 *     ...
 * }
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add attribute 3 for instance 1
 * errCode = EI_APP_DIP_addInstanceAttribute(
 *                  pEI_API_CIP_NODE
 *                  0x0001,
 *                  0x0003,
 *                  EI_API_CIP_eEDT_BOOL,         // Elementary data type
 *                  EI_API_CIP_eAR_GET_AND_SET,   // Available attribute access rule
 *                  EI_APP_CLASS_GetCb,           // Get callback
 *                  NULL,                         // Not set callback
 *                  sizeof(ei_api_cip_edt_bool),
 *                  &instanceValue);
 *
 * \endcode
 *
 * \see EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DIP
 *
 */
static uint32_t EI_APP_DIP_addInstanceAttribute (EI_API_CIP_NODE_T* pCipNode,
                                                                    uint16_t             instanceId,
                                                                    uint16_t             attrId,
                                                                    EI_API_CIP_EEdt_t    edt,
                                                                    EI_API_CIP_EAr_t     accessRule,
                                                                    EI_API_CIP_CBGetAttr getCb,
                                                                    EI_API_CIP_CBSetAttr setCb,
                                                                    uint16_t             len,
                                                                    void*                pvValue)
{
    uint32_t errCode;

    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));
    attr.id = attrId;
    attr.edt = edt;
    attr.edtSize = len;
    attr.accessRule = accessRule;
    attr.get_callback = getCb;
    attr.set_callback = setCb;
    attr.pvValue = pvValue;

    // add attribute
    errCode = EI_API_CIP_addInstanceAttr(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set initial value
    errCode = EI_API_CIP_setInstanceAttr(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set callback function
    if (attr.get_callback != NULL || attr.set_callback != NULL)
    {
        errCode = EI_API_CIP_setInstanceAttrFunc(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, instanceId, &attr);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }
    }

laError:
    return errCode;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute value of DIP object.
 *
 * \details
 * Function for the get service of the value. All instances are connected industrial
 * LEDs controlled by TPIC2810.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     value as boolean.
 *
 * \retval     0         LED is turned off.
 * \retval     1         LED is turned on.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * error = EI_APP_DIP_getValue(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \see EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DIP
 *
 */
bool EI_APP_DIP_getValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t value = 0;
    // Using DOP object's LED output as input for the DIP object
    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, &value);

    return value;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DIP object
 *
 * \param[in]   pCipNode                       Pointer to the CIP node.
 * \param[in]   classId                        Class identifier.
 * \param[in]   instanceId                     Instance identifier.
 * \param[in]   attrId                         Attribute identifier.
 * \param[out]  len                            Pointer to the data type length.
 * \param[out]  pvValue                        Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR        Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use get callback
 * errCode = EI_APP_DIP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                     // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,               // Available attribute access rule
 *                              EI_APP_DIP_getValueCb, // Get callback
 *                              NULL,                                     // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DIP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DIP
 *
 */
uint32_t EI_APP_DIP_getValueCb(
                              EI_API_CIP_NODE_T* pCipNode,
                              uint16_t classId,
                              uint16_t instanceId,
                              uint16_t attrId,
                              uint16_t* len,
                              void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *len = sizeof(bool);
    *(bool*)pvValue = EI_APP_DIP_getValue(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Initialize class data
 *
 * \details
 * This function creates the class with class instance and instances.
 * The class instance contains attribute for the revision.
 * The instances contain attribute for the value.
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
 * EI_APP_DIP_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \see EI_API_ADP_new  EI_API_CIP_NODE_new  EI_API_CIP_createClass  EI_API_CIP_createInstance  EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DIP
 *
 */
void EI_APP_DIP_init(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    EI_API_CIP_SService_t service;

    errCode = EI_API_CIP_createClass(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID);

    // Example how to evaluate error codes returned by API functions.
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set class instance
    OSAL_MEMORY_memset(&service, 0, sizeof(service));
    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addClassService(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    errCode = EI_APP_DIP_addClassAttribute(pCipNode, 1, &dipClassData_s.revision);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }


    for (uint16_t i = 1; i <= EI_APP_DIO_DEVICE_DIP_NUM_OF_INST; i++)
    {
        // Dummy value for each instance
        ei_api_cip_edt_bool instanceValue = 0;
        // Create instances
        errCode = EI_API_CIP_createInstance(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, i);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add get service for instances
        service.code = EI_API_CIP_eSC_GETATTRSINGLE;
        errCode = EI_API_CIP_addInstanceService(pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, i, &service);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add attribute 3 Value (required) for instance
        errCode = EI_APP_DIP_addInstanceAttribute(pCipNode,
                                                  i,
                                                  EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  EI_APP_DIP_getValueCb,
                                                  NULL,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }
    }

laError:
    return;
}

/*!
 *
 * \brief
 * Run function for class DIP.
 *
 * \details
 * It updates the value.
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
 * EI_APP_DIP_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DIP
 *
 */
void EI_APP_DIP_run(EI_API_CIP_NODE_T* pCipNode)
{
    uint8_t buffer[EI_APP_DIO_DEVICE_DIP_NUM_OF_INST] = { 0 };

    // Mirror I/O data
    // Using DOP object's LED output as input for the DIP object
    for(uint8_t instanceId = 0; instanceId < EI_APP_DIO_DEVICE_DIP_NUM_OF_INST; instanceId++)
    {
        EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId + 1, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, &buffer[instanceId]);
    }

    EI_API_CIP_setAssemblyData(pCipNode, EI_APP_DIO_DEVICE_ASSEMBLY_PRODUCING, buffer, EI_APP_DIO_DEVICE_DIP_NUM_OF_INST);
}
