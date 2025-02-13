/*!
 *  \file discrete_io_device_dog.c
 *
 *  \brief
 *  EtherNet/IP&trade; Discrete Output Group Object.
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
#include "device_profiles/discrete_io_device/discrete_io_device_dog.h"
#include "device_profiles/discrete_io_device/discrete_io_device_dop.h"

typedef struct DIO_DEVICE_DOG_object
{
    uint16_t                       DOG_instanceID;
    DIO_DEVICE_DOG_ObjectCfg_t     binding;
    struct DIO_DEVICE_DOG_object  *nextObj;
}DIO_DEVICE_DOG_object_t;

typedef struct DIO_DEVICE_DOG_container
{
    bool                      isClassInitialized;
    EI_API_CIP_NODE_T        *pCipNode;
    void                     *mutex;
    DIO_DEVICE_DOG_object_t  *head;
}DIO_DEVICE_DOG_container_t;

static DIO_DEVICE_DOG_ClassData_t dogClassData_s = {.revision = CFG_PROFILE_DIO_DEVICE_DOG_REVISION_NUMBER
                                               };

static DIO_DEVICE_DOG_container_t dogContainer_s = {0};

static DIO_DEVICE_DOG_object_t *DIO_DEVICE_DOG_findObject(uint16_t dog_instanceID);
static uint32_t DIO_DEVICE_DOG_addInstance(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceID);
/*!
 * \brief
 * Add an attribute for the class instance.
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
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint16_t revision = 0x0002; // Create one of the class level attribute
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * errCode = DIO_DEVICE_DOG_addClassAttribute(pEI_API_CIP_NODE, 0x0001, &revision);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
static uint32_t DIO_DEVICE_DOG_addClassAttribute(EI_API_CIP_NODE_T* pCipNode, uint16_t attrId, void* pvValue)
{
    uint32_t errCode;
    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));

    attr.id         = attrId;
    attr.edt        = EI_API_CIP_eEDT_UINT;
    attr.accessRule = EI_API_CIP_eAR_GET;
    attr.pvValue    = pvValue;

    errCode = EI_API_CIP_addClassAttr(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, &attr);
    if(errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    errCode = EI_API_CIP_setClassAttr(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, &attr);
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
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * // Callback function for the set service
 * uint32_t EI_APP_CLASS_SetCb(...)
 * {
 *     ...
 * }
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add attribute 3 for instance 1
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                  pEI_API_CIP_NODE
 *                  0x0001,
 *                  0x0003,
 *                  EI_API_CIP_eEDT_BOOL,         // Elementary data type
 *                  EI_API_CIP_eAR_GET_AND_SET,   // Available attribute access rule
 *                  NULL,                         // No get callback
 *                  EI_APP_CLASS_SetCb,           // Set callback
 *                  sizeof(ei_api_cip_edt_bool),
 *                  &instanceValue);
 *
 * \endcode
 *
 * \see EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
static uint32_t DIO_DEVICE_DOG_addInstanceAttribute(EI_API_CIP_NODE_T*   pCipNode,
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
    errCode = EI_API_CIP_addInstanceAttr(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set initial value
    errCode = EI_API_CIP_setInstanceAttr(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set callback function
    if (attr.get_callback != NULL || attr.set_callback != NULL)
    {
        errCode = EI_API_CIP_setInstanceAttrFunc(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, &attr);
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
 * Function provides set access to the attribute command of DOG object.
 *
 * \details
 * Changes state of all DOPs (Discrete IO Points) in group to Idle Mode or Run Mode, 0 = Idle Mode, 1 = Run Mode.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  Command                                     Command.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_TYPE     Attribute type is invalid.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint8_t command = 1;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * errCode = DIO_DEVICE_DOG_setCommand(pEI_API_CIP_NODE, 0x0001, command);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setCommand(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId, uint8_t command)
{
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    DIO_DEVICE_DOG_object_t *obj = NULL;
    int32_t osalRetval;

    error = EI_API_CIP_setAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_06, command);
    if(EI_API_CIP_eERR_OK == error)
    {
        osalRetval = OSAL_lockNamedMutex(dogContainer_s.mutex, 5UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            obj = DIO_DEVICE_DOG_findObject(instanceId);
            if(NULL != obj)
            {
                for(uint16_t i = 0; i < obj->binding.list_len; i++)
                {
                    error = DIO_DEVICE_DOP_setCommand(obj->binding.dop_instance_binding_list[i], (ei_api_cip_edt_bool)command);
                }
            }
            else
            {
                error = EI_API_CIP_eERR_GENERAL;
            }
            OSAL_unLockNamedMutex(dogContainer_s.mutex);
        }
        else
        {
            error = EI_API_CIP_eERR_GENERAL;
        }
    }

    return error;
}

/*!
 *
 * \brief
 * Set attribute single service callback of DOG object.
 *
 * \param[in]  pCipNode                        Pointer to the CIP node.
 * \param[in]  classId                         Class identifier.
 * \param[in]  instanceId                      Instance identifier.
 * \param[in]  attrId                          Attribute identifier.
 * \param[in]  len                             Data type length.
 * \param[in]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_CB_ERR_CODE_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_GENERAL        General error.
 * \retval     #EI_API_eERR_CB_VAL_TOO_HIGH    Value is too high.
 * \retval     #EI_API_eERR_CB_VAL_TOO_LOW     Value is too low.
 * \retval     #EI_API_eERR_CB_INVALID_VALUE   Value is not valid.
 * \retval     #EI_API_eERR_CB_NOT_ENOUGH_DATA Not enough data.
 * \retval     #EI_API_eERR_CB_TOO_MUCH_DATA   Too much data.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use set callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                       // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                 // Available attribute access rule
 *                              NULL,                                       // No get callback
 *                              DIO_DEVICE_DOG_setCommandCb, // Set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setCommandCb(
                                EI_API_CIP_NODE_T* pCipNode,
                                uint16_t classId,
                                uint16_t instanceId,
                                uint16_t attrId,
                                uint16_t len,
                                void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    uint32_t error = EI_API_CIP_eERR_GENERAL;
    uint8_t command = *(uint8_t*)pvValue;

    if (sizeof(ei_api_cip_edt_bool) != len)
    {
        // Not necessary, already validated by the object dictionary.
        error = len < sizeof(ei_api_cip_edt_bool) ? EI_API_eERR_CB_NOT_ENOUGH_DATA : EI_API_eERR_CB_TOO_MUCH_DATA;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    error = command <= 1 ? DIO_DEVICE_DOG_setCommand(pCipNode, instanceId, command) : EI_API_eERR_CB_INVALID_VALUE;

laError:
    return error;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute command of DOG object.
 *
 * \details
 * Changes state of all DOPs (Discrete IO Point) in group to Idle Mode or Run Mode, 0 = Idle Mode, 1 = Run Mode.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     Command as boolean.
 *
 * \retval     0         Idle Mode.
 * \retval     1         Run Mode.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * error = DIO_DEVICE_DOG_getCommand(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
bool DIO_DEVICE_DOG_getCommand(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t command = 0;
    EI_API_CIP_getAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_06, &command);

    return command;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOG object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  pLen                            Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use get callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                       // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                 // Available attribute access rule
 *                              DIO_DEVICE_DOG_getCommandCb, // Get callback
 *                              NULL,                                       // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_getCommandCb(EI_API_CIP_NODE_T *pCipNode,
                                     uint16_t           classId,
                                     uint16_t           instanceId,
                                     uint16_t           attrId,
                                     uint16_t          *pLen,
                                     void              *pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *pLen = sizeof(bool);
    *(bool*)pvValue = DIO_DEVICE_DOG_getCommand(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides set access to the attribute fault action of DOG object.
 *
 * \details
 * State of output after recoverable failure, 0 = Fault Value attribute; 1 = hold last state.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  faultValue                                  Fault value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_TYPE     Attribute type is invalid.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint8_t faultValue = 1;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * errCode = DIO_DEVICE_DOG_setFaultAction(pEI_API_CIP_NODE, 0x0001, faultValue);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setFaultAction(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId, uint8_t faultAction)
{
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    DIO_DEVICE_DOG_object_t *obj = NULL;
    int32_t osalRetval;

    error = EI_API_CIP_setAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_07, faultAction);
    if(EI_API_CIP_eERR_OK == error)
    {
        osalRetval = OSAL_lockNamedMutex(dogContainer_s.mutex, 5UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            obj = DIO_DEVICE_DOG_findObject(instanceId);
            if(NULL != obj)
            {
                for(uint16_t i = 0; i < obj->binding.list_len; i++)
                {
                    error = DIO_DEVICE_DOP_setFaultAction(obj->binding.dop_instance_binding_list[i], faultAction);
                }
            }
            else
            {
                error = EI_API_CIP_eERR_GENERAL;
            }
            OSAL_unLockNamedMutex(dogContainer_s.mutex);
        }
        else
        {
            error = EI_API_CIP_eERR_GENERAL;
        }
    }

    return error;
}

/*!
 *
 * \brief
 * Set attribute single service callback of DOG object.
 *
 * \param[in]  pCipNode                        Pointer to the CIP node.
 * \param[in]  classId                         Class identifier.
 * \param[in]  instanceId                      Instance identifier.
 * \param[in]  attrId                          Attribute identifier.
 * \param[in]  len                             Data type length.
 * \param[in]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_CB_ERR_CODE_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_GENERAL        General error.
 * \retval     #EI_API_eERR_CB_VAL_TOO_HIGH    Value is too high.
 * \retval     #EI_API_eERR_CB_VAL_TOO_LOW     Value is too low.
 * \retval     #EI_API_eERR_CB_INVALID_VALUE   Value is not valid.
 * \retval     #EI_API_eERR_CB_NOT_ENOUGH_DATA Not enough data.
 * \retval     #EI_API_eERR_CB_TOO_MUCH_DATA   Too much data.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use set callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                           // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                     // Available attribute access rule
 *                              NULL,                                           // No get callback
 *                              DIO_DEVICE_DOG_setFaultActionCb, // Set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setFaultActionCb(
                                    EI_API_CIP_NODE_T* pCipNode,
                                    uint16_t classId,
                                    uint16_t instanceId,
                                    uint16_t attrId,
                                    uint16_t len,
                                    void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    uint32_t error = EI_API_CIP_eERR_GENERAL;
    uint8_t faultAction = *(uint8_t*)pvValue;

    if (sizeof(ei_api_cip_edt_bool) != len)
    {
        // Not necessary, already validated by the object dictionary.
        error = len < sizeof(ei_api_cip_edt_bool) ? EI_API_eERR_CB_NOT_ENOUGH_DATA : EI_API_eERR_CB_TOO_MUCH_DATA;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    error = faultAction <= 1 ? DIO_DEVICE_DOG_setFaultAction(pCipNode, instanceId, faultAction) : EI_API_eERR_CB_INVALID_VALUE;

laError:
    return error;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute fault action of DOG object.
 *
 * \details
 * State of output after recoverable failure, 0 = Fault Value attribute; 1 = hold last state.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     faultAction as boolean.
 *
 * \retval     0         Fault Value attribute.
 * \retval     1         Hold last state.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * error = DIO_DEVICE_DOG_getFaultAction(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
bool DIO_DEVICE_DOG_getFaultAction(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t faultAction = 0;

    EI_API_CIP_getAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_07, &faultAction);

    return faultAction;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOG object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  pLen                            Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use get callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                           // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                     // Available attribute access rule
 *                              DIO_DEVICE_DOG_getFaultActionCb, // Get callback
 *                              NULL,                                           // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_getFaultActionCb(EI_API_CIP_NODE_T *pCipNode,
                                         uint16_t           classId,
                                         uint16_t           instanceId,
                                         uint16_t           attrId,
                                         uint16_t          *pLen,
                                         void              *pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *pLen = sizeof(bool);
    *(bool*)pvValue = DIO_DEVICE_DOG_getFaultAction(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides set access to the attribute fault value of DOG object.
 *
 * \details
 * User�defined value for use with Fault Action attribute 0 = off, 1 = on.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  faultValue                                  Fault value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_TYPE     Attribute type is invalid.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint8_t faultValue = 1;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * errCode = DIO_DEVICE_DOG_setFaultValue(pEI_API_CIP_NODE, 0x0001, faultValue);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setFaultValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId, uint8_t faultValue)
{
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    DIO_DEVICE_DOG_object_t *obj = NULL;
    int32_t osalRetval;

    error = EI_API_CIP_setAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_08, faultValue);
    if(EI_API_CIP_eERR_OK == error)
    {
        osalRetval = OSAL_lockNamedMutex(dogContainer_s.mutex, 5UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            obj = DIO_DEVICE_DOG_findObject(instanceId);
            if(NULL != obj)
            {
                for(uint16_t i = 0; i < obj->binding.list_len; i++)
                {
                    error = DIO_DEVICE_DOP_setFaultValue(obj->binding.dop_instance_binding_list[i], (ei_api_cip_edt_bool)faultValue);
                }
            }
            else
            {
                error = EI_API_CIP_eERR_GENERAL;
            }
            OSAL_unLockNamedMutex(dogContainer_s.mutex);
        }
        else
        {
            error = EI_API_CIP_eERR_GENERAL;
        }
    }

    return error;
}

/*!
 *
 * \brief
 * Set attribute single service callback of DOG object.
 *
 * \param[in]  pCipNode                        Pointer to the CIP node.
 * \param[in]  classId                         Class identifier.
 * \param[in]  instanceId                      Instance identifier.
 * \param[in]  attrId                          Attribute identifier.
 * \param[in]  len                             Data type length.
 * \param[in]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_CB_ERR_CODE_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_GENERAL        General error.
 * \retval     #EI_API_eERR_CB_VAL_TOO_HIGH    Value is too high.
 * \retval     #EI_API_eERR_CB_VAL_TOO_LOW     Value is too low.
 * \retval     #EI_API_eERR_CB_INVALID_VALUE   Value is not valid.
 * \retval     #EI_API_eERR_CB_NOT_ENOUGH_DATA Not enough data.
 * \retval     #EI_API_eERR_CB_TOO_MUCH_DATA   Too much data.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use set callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                          // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                    // Available attribute access rule
 *                              NULL,                                          // No get callback
 *                              DIO_DEVICE_DOG_setFaultValueCb, // Set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setFaultValueCb(EI_API_CIP_NODE_T *pCipNode,
                                        uint16_t           classId,
                                        uint16_t           instanceId,
                                        uint16_t           attrId,
                                        uint16_t           len,
                                        void              *pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    uint32_t error = EI_API_CIP_eERR_GENERAL;
    uint8_t faultValue = *(uint8_t*)pvValue;

    if (sizeof(ei_api_cip_edt_bool) != len)
    {
        // Not necessary, already validated by the object dictionary.
        error = len < sizeof(ei_api_cip_edt_bool) ? EI_API_eERR_CB_NOT_ENOUGH_DATA : EI_API_eERR_CB_TOO_MUCH_DATA;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    error = faultValue <= 1 ? DIO_DEVICE_DOG_setFaultValue(pCipNode, instanceId, faultValue) : EI_API_eERR_CB_INVALID_VALUE;

laError:
    return error;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute fault value of DOG object.
 *
 * \details
 * User�defined value for use with Fault Action attribute 0 = off, 1 = on.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     faultValue as boolean.
 *
 * \retval     0         LED is turned off.
 * \retval     1         LED is turned on.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * error = DIO_DEVICE_DOG_getFaultValue(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
bool DIO_DEVICE_DOG_getFaultValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t faultValue = 0;
    EI_API_CIP_getAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_08, &faultValue);

    return faultValue;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOG object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  len                             Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use get callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                          // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                    // Available attribute access rule
 *                              DIO_DEVICE_DOG_getFaultValueCb, // Get callback
 *                              NULL,                                          // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_getFaultValueCb(
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
    *(bool*)pvValue = DIO_DEVICE_DOG_getFaultValue(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides set access to the attribute idle Action of DOG object.
 *
 * \details
 * State of output during idle, 0 = Idle Value attribute; 1 = hold last state
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  idleAction                                  Idle action.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_TYPE     Attribute type is invalid.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint8_t idleAction = 1;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * errCode = DIO_DEVICE_DOG_setIdleAction(pEI_API_CIP_NODE, 0x0001, idleAction);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setIdleAction(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId, uint8_t idleAction)
{
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    DIO_DEVICE_DOG_object_t *obj = NULL;
    int32_t osalRetval;

    error = EI_API_CIP_setAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_09, idleAction);
    if(EI_API_CIP_eERR_OK == error)
    {
        osalRetval = OSAL_lockNamedMutex(dogContainer_s.mutex, 5UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            obj = DIO_DEVICE_DOG_findObject(instanceId);
            if(NULL != obj)
            {
                for(uint16_t i = 0; i < obj->binding.list_len; i++)
                {
                    error = DIO_DEVICE_DOP_setIdleAction(obj->binding.dop_instance_binding_list[i], (ei_api_cip_edt_bool)idleAction);
                }
            }
            else
            {
                error = EI_API_CIP_eERR_GENERAL;
            }
            OSAL_unLockNamedMutex(dogContainer_s.mutex);
        }
        else
        {
            error = EI_API_CIP_eERR_GENERAL;
        }
    }

    return error;
}

/*!
 *
 * \brief
 * Set attribute single service callback of DOG object.
 *
 * \param[in]  pCipNode                        Pointer to the CIP node.
 * \param[in]  classId                         Class identifier.
 * \param[in]  instanceId                      Instance identifier.
 * \param[in]  attrId                          Attribute identifier.
 * \param[in]  len                             Data type length.
 * \param[in]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_CB_ERR_CODE_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_GENERAL        General error.
 * \retval     #EI_API_eERR_CB_VAL_TOO_HIGH    Value is too high.
 * \retval     #EI_API_eERR_CB_VAL_TOO_LOW     Value is too low.
 * \retval     #EI_API_eERR_CB_INVALID_VALUE   Value is not valid.
 * \retval     #EI_API_eERR_CB_NOT_ENOUGH_DATA Not enough data.
 * \retval     #EI_API_eERR_CB_TOO_MUCH_DATA   Too much data.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use set callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                           // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                     // Available attribute access rule
 *                              NULL,                                           // No get callback
 *                              DIO_DEVICE_DOG_setIdleActionCb,  // Set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setIdleActionCb(EI_API_CIP_NODE_T *pCipNode,
                                        uint16_t           classId,
                                        uint16_t           instanceId,
                                        uint16_t           attrId,
                                        uint16_t           len,
                                        void              *pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    uint32_t error = EI_API_CIP_eERR_GENERAL;
    uint8_t idleAction = *(uint8_t*)pvValue;

    if (sizeof(ei_api_cip_edt_bool) != len)
    {
        // Not necessary, already validated by the object dictionary.
        error = len < sizeof(ei_api_cip_edt_bool) ? EI_API_eERR_CB_NOT_ENOUGH_DATA : EI_API_eERR_CB_TOO_MUCH_DATA;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    error = idleAction <= 1 ? DIO_DEVICE_DOG_setIdleAction(pCipNode, instanceId, idleAction) : EI_API_eERR_CB_INVALID_VALUE;

laError:
    return error;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute idle value of DOG object.
 *
 * \details
 * State of output during idle, 0 = Idle Value attribute; 1 = hold last state
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     value as boolean.
 *
 * \retval     0         Idle Value attribute.
 * \retval     1         Hold last state.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * error = DIO_DEVICE_DOG_getIdleAction(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
bool DIO_DEVICE_DOG_getIdleAction(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t idleAction = 0;
    EI_API_CIP_getAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_09, &idleAction);

    return idleAction;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOG object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  pLen                            Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use get callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                          // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                    // Available attribute access rule
 *                              DIO_DEVICE_DOG_getIdleActionCb, // Get callback
 *                              NULL,                                          // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_getIdleActionCb(EI_API_CIP_NODE_T *pCipNode,
                                        uint16_t           classId,
                                        uint16_t           instanceId,
                                        uint16_t           attrId,
                                        uint16_t          *pLen,
                                        void              *pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *pLen = sizeof(bool);
    *(bool*)pvValue = DIO_DEVICE_DOG_getIdleAction(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides set access to the attribute idle value of DOG object.
 *
 * \details
 * User-defined value for use with Idle Action attribute, 0 = off; 1 = on
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  idleValue                                   Idle value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_TYPE     Attribute type is invalid.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint8_t idleValue = 1;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * errCode = DIO_DEVICE_DOG_setIdleValue(pEI_API_CIP_NODE, 0x0001, idleValue);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setIdleValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId, uint8_t idleValue)
{
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    DIO_DEVICE_DOG_object_t *obj = NULL;
    int32_t osalRetval;

    error = EI_API_CIP_setAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_10, idleValue);
    if(EI_API_CIP_eERR_OK == error)
    {
        osalRetval = OSAL_lockNamedMutex(dogContainer_s.mutex, 5UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            obj = DIO_DEVICE_DOG_findObject(instanceId);
            if(NULL != obj)
            {
                for(uint16_t i = 0; i < obj->binding.list_len; i++)
                {
                    error = DIO_DEVICE_DOP_setIdleValue(obj->binding.dop_instance_binding_list[i], (ei_api_cip_edt_bool)idleValue);
                }
            }
            else
            {
                error = EI_API_CIP_eERR_GENERAL;
            }
            OSAL_unLockNamedMutex(dogContainer_s.mutex);
        }
        else
        {
            error = EI_API_CIP_eERR_GENERAL;
        }
    }

    return error;
}

/*!
 *
 * \brief
 * Set attribute single service callback of DOG object.
 *
 * \param[in]  pCipNode                        Pointer to the CIP node.
 * \param[in]  classId                         Class identifier.
 * \param[in]  instanceId                      Instance identifier.
 * \param[in]  attrId                          Attribute identifier.
 * \param[in]  len                             Data type length.
 * \param[in]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_CB_ERR_CODE_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_GENERAL        General error.
 * \retval     #EI_API_eERR_CB_VAL_TOO_HIGH    Value is too high.
 * \retval     #EI_API_eERR_CB_VAL_TOO_LOW     Value is too low.
 * \retval     #EI_API_eERR_CB_INVALID_VALUE   Value is not valid.
 * \retval     #EI_API_eERR_CB_NOT_ENOUGH_DATA Not enough data.
 * \retval     #EI_API_eERR_CB_TOO_MUCH_DATA   Too much data.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use set callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                        // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                  // Available attribute access rule
 *                              NULL,                                        // No get callback
 *                              DIO_DEVICE_DOG_setIdleValueCb, // Set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_setIdleValueCb(EI_API_CIP_NODE_T *pCipNode,
                                       uint16_t           classId,
                                       uint16_t           instanceId,
                                       uint16_t           attrId,
                                       uint16_t           len,
                                       void              *pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    uint32_t error = EI_API_CIP_eERR_GENERAL;
    uint8_t idleValue = *(uint8_t*)pvValue;

    if (sizeof(ei_api_cip_edt_bool) != len)
    {
        // Not necessary, already validated by the object dictionary.
        error = len < sizeof(ei_api_cip_edt_bool) ? EI_API_eERR_CB_NOT_ENOUGH_DATA : EI_API_eERR_CB_TOO_MUCH_DATA;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    error = idleValue <= 1 ? DIO_DEVICE_DOG_setIdleValue(pCipNode, instanceId, idleValue) : EI_API_eERR_CB_INVALID_VALUE;

laError:
    return error;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute value of DOG object.
 *
 * \details
 * User-defined value for use with Idle Value attribute 0 = off, 1 = on.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     idleValue as boolean.
 *
 * \retval     0         LED is turned off.
 * \retval     1         LED is turned on.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * error = DIO_DEVICE_DOG_getIdleValue(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
bool DIO_DEVICE_DOG_getIdleValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t idleValue = 0;
    EI_API_CIP_getAttr_bool(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceId, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_10, &idleValue);

    return idleValue;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOG object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  pLen                            Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * // Add instance attribute & use get callback
 * errCode = DIO_DEVICE_DOG_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                       // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                 // Available attribute access rule
 *                              DIO_DEVICE_DOG_getIdleValue, // Get callback
 *                              NULL,                                       // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see DIO_DEVICE_DOG_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
uint32_t DIO_DEVICE_DOG_getIdleValueCb(EI_API_CIP_NODE_T *pCipNode,
                                       uint16_t           classId,
                                       uint16_t           instanceId,
                                       uint16_t           attrId,
                                       uint16_t          *pLen,
                                       void              *pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *pLen = sizeof(bool);
    *(bool*)pvValue = DIO_DEVICE_DOG_getIdleValue(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Initialize class data.
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
 * DIO_DEVICE_DOG_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOG
 *
 */
void DIO_DEVICE_DOG_init(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    EI_API_CIP_SService_t service;

    if(false == dogContainer_s.isClassInitialized)
    {
        errCode = EI_API_CIP_createClass(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID);

        // Example how to evaluate error codes returned by API functions.
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // set class instance
        OSAL_MEMORY_memset(&service, 0, sizeof(service));
        service.code = EI_API_CIP_eSC_GETATTRSINGLE;
        errCode = EI_API_CIP_addClassService(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, &service);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        errCode = DIO_DEVICE_DOG_addClassAttribute(pCipNode, 1, &dogClassData_s.revision);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        dogContainer_s.mutex  = OSAL_createNamedMutex("DOG_Mutex");
        if(NULL == dogContainer_s.mutex)
        {
            OSAL_printf("%s:%d create DOG_Mutex failed\r\n", __func__, __LINE__);
            OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        dogContainer_s.head = NULL;
        dogContainer_s.pCipNode = pCipNode;
        dogContainer_s.isClassInitialized = true;
    }

laError:
    return;
}


/**
 *
 */
static DIO_DEVICE_DOG_object_t *DIO_DEVICE_DOG_findObject(uint16_t dog_instanceID)
{
    DIO_DEVICE_DOG_object_t  *pObj = NULL;

    if(false == dogContainer_s.isClassInitialized)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return NULL;
    }
    else
    {
        pObj  = dogContainer_s.head;
        while (NULL != pObj)
        {
            if(dog_instanceID == pObj->DOG_instanceID)
            {
                break;
            }
            pObj = pObj->nextObj;
        }
    }

    return pObj;
}
/**
 *
 */
static uint32_t DIO_DEVICE_DOG_addInstance(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceID)
{
    EI_API_CIP_SService_t service;
    ei_api_cip_edt_bool instanceValue = 0;
    uint32_t errCode;

    OSAL_MEMORY_memset(&service, 0, sizeof(service));
    // Create instances
    errCode = EI_API_CIP_createInstance(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceID);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add set & get service for instances
    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addInstanceService(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    service.code = EI_API_CIP_eSC_SETATTRSINGLE;
    errCode = EI_API_CIP_addInstanceService(pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, instanceID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 6 Command (required) for instance
    errCode = DIO_DEVICE_DOG_addInstanceAttribute(pCipNode,
                                                  instanceID,
                                                  DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_06,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  DIO_DEVICE_DOG_getCommandCb,
                                                  DIO_DEVICE_DOG_setCommandCb,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 7 Fault Action (optional) for instance
    errCode = DIO_DEVICE_DOG_addInstanceAttribute(pCipNode,
                                                  instanceID,
                                                  DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_07,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  DIO_DEVICE_DOG_getFaultActionCb,
                                                  DIO_DEVICE_DOG_setFaultActionCb,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 8 Fault Value (optional) for instance
    errCode = DIO_DEVICE_DOG_addInstanceAttribute(pCipNode,
                                                  instanceID,
                                                  DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_08,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  DIO_DEVICE_DOG_getFaultValueCb,
                                                  DIO_DEVICE_DOG_setFaultValueCb,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 9 Idle Action (optional) for instance
    errCode = DIO_DEVICE_DOG_addInstanceAttribute(pCipNode,
                                                  instanceID,
                                                  DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_09,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  DIO_DEVICE_DOG_getIdleActionCb,
                                                  DIO_DEVICE_DOG_setIdleActionCb,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 10 Idle Value (optional) for instance
    errCode = DIO_DEVICE_DOG_addInstanceAttribute(pCipNode,
                                                  instanceID,
                                                  DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_10,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  DIO_DEVICE_DOG_getIdleValueCb,
                                                  DIO_DEVICE_DOG_setIdleValueCb,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    laError:
    return errCode;
}

/**
 *
 */
static void DIO_DEVICE_DOG_insertObj(DIO_DEVICE_DOG_object_t *pDogObject)
{

    if(NULL == dogContainer_s.head) //first item?
    {
        dogContainer_s.head = pDogObject;
        dogContainer_s.head->nextObj = NULL;
    }
    else
    {
        pDogObject->nextObj = dogContainer_s.head;
        dogContainer_s.head = pDogObject;
    }
}
/**
 * \brief Create and add a DOG instance object
 * \param[in] InstanceID the instance-ID of the DOG object to be created
 * \param[in] pDopConfig configuration for this instance object
 * \return true if successful, otherwise false
*/
bool DIO_DEVICE_DOG_addObject(uint16_t instanceID, DIO_DEVICE_DOG_ObjectCfg_t *config)
{
    DIO_DEVICE_DOG_object_t *pDogObj = NULL;
    int32_t osalRetval;
    uint32_t errCode;
    bool retval = true;

    if(false == dogContainer_s.isClassInitialized)
    {
        OSAL_printf("%s:%d first call the DIO_DEVICE_DOG_init before adding objects\r\n", __func__, __LINE__);
        OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return  false;
    }

    if(NULL == config)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return false;
    }
    if((NULL == config->dop_instance_binding_list) ||
    (0 == config->list_len))
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return false;
    }

    osalRetval = OSAL_lockNamedMutex(dogContainer_s.mutex, 1000UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        pDogObj = DIO_DEVICE_DOG_findObject(instanceID);
        if(NULL == pDogObj)
        {
            pDogObj = OSAL_MEMORY_calloc(sizeof(DIO_DEVICE_DOG_object_t), 1);
            if(NULL != pDogObj)
            {
                pDogObj->binding.dop_instance_binding_list = OSAL_MEMORY_calloc((config->list_len * sizeof(uint16_t)), 1);
                if(NULL != pDogObj->binding.dop_instance_binding_list)
                {
                    OSAL_MEMORY_memcpy(pDogObj->binding.dop_instance_binding_list,
                    config->dop_instance_binding_list, (config->list_len * sizeof(uint16_t)));

                    pDogObj->binding.list_len = config->list_len;
                    pDogObj->DOG_instanceID = instanceID;
                    pDogObj->nextObj = NULL;

                    errCode = DIO_DEVICE_DOG_addInstance(dogContainer_s.pCipNode, instanceID);
                    if (EI_API_CIP_eERR_OK == errCode)
                    {
                        DIO_DEVICE_DOG_insertObj(pDogObj);
                    }
                    else
                    {
                        retval = false;
                        OSAL_MEMORY_free(pDogObj->binding.dop_instance_binding_list);
                        OSAL_MEMORY_free(pDogObj);
                        pDogObj = NULL;
                    }
                }
                else
                {
                    retval = false;
                    OSAL_MEMORY_free(pDogObj);
                    pDogObj = NULL;
                }
            }
        }
        else
        {
            OSAL_printf("DOG Object already exist! \n");
        }

        OSAL_unLockNamedMutex(dogContainer_s.mutex);
    }
    else
    {
        retval = false;
        OSAL_printf("%s:%d mutex lock error %d\r\n", __func__, __LINE__,  osalRetval);
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    if(NULL == pDogObj)
    {
        retval = false;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return retval;
}
/**
 * \brief deletes the DOG object(if exist)
 * \param[in] instanceID the instance-ID of the object to be removed
 * \return  true if successful, false if the object does not exist
*/
bool DIO_DEVICE_DOG_deleteObject(uint16_t instanceID)
{
    DIO_DEVICE_DOG_object_t *previous  = NULL;
    DIO_DEVICE_DOG_object_t *temp = NULL;
    int32_t osalRetval;
    bool retval = false;

    osalRetval = OSAL_lockNamedMutex(dogContainer_s.mutex, 1000UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        if((NULL != dogContainer_s.head) && (instanceID == dogContainer_s.head->DOG_instanceID))
        {
            temp = dogContainer_s.head->nextObj;
            OSAL_MEMORY_free(dogContainer_s.head->binding.dop_instance_binding_list);
            OSAL_MEMORY_free(dogContainer_s.head);
            dogContainer_s.head = temp;
            retval =  true;
        }
        else
        {
            temp = dogContainer_s.head;
            //find the node to be deleted
            while(NULL !=  temp)
            {
                previous = temp;
                temp  = temp->nextObj;
                if((NULL !=  temp) && (instanceID == temp->DOG_instanceID))
                {
                    break;
                }

            }

            if(NULL ==  temp)
            {
                retval = false; //the Node could not be found
            }
            else
            {
                previous->nextObj  = temp->nextObj;
                OSAL_MEMORY_free(temp->binding.dop_instance_binding_list);
                OSAL_MEMORY_free(temp);
                retval = true;
            }
        }

        OSAL_unLockNamedMutex(dogContainer_s.mutex);
    }
    else
    {
        OSAL_printf("%s:%d mutex lock error %d\r\n", __func__, __LINE__,  osalRetval);
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        retval = false;
    }

    return retval;
}

/**
 *
 */
uint32_t DIO_DEVICE_DOG_receiveAssemblyConfig(uint16_t dog_instanceID, ei_api_cip_edt_bool faultAction, ei_api_cip_edt_bool idleAction)
{
    DIO_DEVICE_DOG_object_t *obj = NULL;
    int32_t osalRetval;
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    error = EI_API_CIP_setAttr_bool(dogContainer_s.pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, dog_instanceID, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_07, faultAction);
    if(EI_API_CIP_eERR_OK == error)
    {
        error = EI_API_CIP_setAttr_bool(dogContainer_s.pCipNode, CFG_PROFILE_DIO_DEVICE_DOG_CLASS_ID, dog_instanceID, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_09, idleAction);
        if(EI_API_CIP_eERR_OK == error)
        {
            osalRetval = OSAL_lockNamedMutex(dogContainer_s.mutex, 5UL);
            if(OSAL_ERR_NoError == osalRetval)
            {
                obj = DIO_DEVICE_DOG_findObject(dog_instanceID);
                if(NULL != obj)
                {
                    for(uint16_t i = 0; i < obj->binding.list_len; i++)
                    {
                        error = DIO_DEVICE_DOP_setFaultAction(obj->binding.dop_instance_binding_list[i], faultAction);
                        error = DIO_DEVICE_DOP_setIdleAction(obj->binding.dop_instance_binding_list[i], idleAction);
                    }
                }
                else
                {
                    error = EI_API_CIP_eERR_GENERAL;
                }
                OSAL_unLockNamedMutex(dogContainer_s.mutex);
            }
        }
        else
        {
            error = EI_API_CIP_eERR_GENERAL;
        }
    }


    return error;
}
