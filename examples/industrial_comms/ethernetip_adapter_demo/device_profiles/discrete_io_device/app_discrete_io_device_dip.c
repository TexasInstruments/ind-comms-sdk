/*!
 *  \file app_discrete_io_device_dip.c
 *
 *  \brief
 *  EtherNet/IP&trade; Discrete Input Point Object.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

#include "EI_API.h"
#include "EI_API_def.h"

#include "appUart.h"
#include "appLed.h"
#include "appNV.h"
#include "appCfg.h"

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

/**
 *
*/
typedef struct EI_APP_DIP_object
{
    uint16_t                    instanceID;
    EI_DIP_OBJECT_Cfg_t         userCallbacks;
    EI_APP_DIP_OBJECT_STATE_t   current_state;
    EI_APP_DIP_OBJECT_STATE_t   old_state;
    ei_api_cip_edt_bool         value;
    ei_api_cip_edt_bool         status;
    struct EI_APP_DIP_object    *nextObject;
}EI_APP_DIP_object_t;

typedef struct EI_APP_DIP_container
{
    bool isClassInitialized;
    EI_APP_DIP_object_t *head;
    EI_API_CIP_NODE_T *pCipNode;
    void *mutex;
}EI_APP_DIP_container_t;

static EI_APP_DIP_ClassData_t dipClassData_s = {.revision = EI_APP_DIP_CLASS_REVISION
                                               };

static EI_APP_DIP_container_t dipContainer_s = {0};

static uint32_t EI_APP_DIP_addInstanceAttribute (EI_API_CIP_NODE_T* pCipNode,
                                                                    uint16_t             instanceId,
                                                                    uint16_t             attrId,
                                                                    EI_API_CIP_EEdt_t    edt,
                                                                    EI_API_CIP_EAr_t     accessRule,
                                                                    EI_API_CIP_CBGetAttr getCb,
                                                                    EI_API_CIP_CBSetAttr setCb,
                                                                    uint16_t             len,
                                                                    void*                pvValue);
static uint32_t EI_APP_DIP_getAttrCb(
                              EI_API_CIP_NODE_T* pCipNode,
                              uint16_t classId,
                              uint16_t instanceId,
                              uint16_t attrId,
                              uint16_t* len,
                              void* pvValue);

static EI_APP_DIP_object_t*  EI_APP_DIP_findObj(uint16_t instanceID)
{
    EI_APP_DIP_object_t  *pDipObj = NULL;

    if(false == dipContainer_s.isClassInitialized)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return NULL;
    }
    else
    {
        pDipObj  = dipContainer_s.head;
        while (NULL != pDipObj)
        {
            if(instanceID == pDipObj->instanceID)
            {
                break;
            }
            pDipObj = pDipObj->nextObject;
        }
    }

    return pDipObj;
}

static uint32_t EI_APP_DIP_createInstance(EI_APP_DIP_object_t *pDipObject)
{
    ei_api_cip_edt_bool instanceValue = 0;
    EI_API_CIP_SService_t service = {0};
    uint32_t errCode;
    // Create instances
    errCode = EI_API_CIP_createInstance(dipContainer_s.pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, pDipObject->instanceID);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }
    // Add get service for instances
    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addInstanceService(dipContainer_s.pCipNode, EI_APP_DIO_DEVICE_DIP_CLASS_ID, pDipObject->instanceID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }
    // Add attribute 3 Value (required) for instance
    errCode = EI_APP_DIP_addInstanceAttribute(dipContainer_s.pCipNode,
                                                pDipObject->instanceID,
                                                EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03,
                                                EI_API_CIP_eEDT_BOOL,
                                                EI_API_CIP_eAR_GET,
                                                EI_APP_DIP_getAttrCb,
                                                NULL,
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 4 Value (optional) for instance
    if(NULL != pDipObject->userCallbacks.fuGetStatus)
    {
        errCode = EI_APP_DIP_addInstanceAttribute(dipContainer_s.pCipNode,
                                            pDipObject->instanceID,
                                            EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_04,
                                            EI_API_CIP_eEDT_BOOL,
                                            EI_API_CIP_eAR_GET,
                                            EI_APP_DIP_getAttrCb,
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
    return errCode;
}

static void EI_APP_DIP_insertObj(EI_APP_DIP_object_t *pDipObject)
{

    if(NULL == dipContainer_s.head) //first item?
    {
        dipContainer_s.head = pDipObject;
        dipContainer_s.head->nextObject = NULL;
    }
    else
    {
        pDipObject->nextObject = dipContainer_s.head;
        dipContainer_s.head = pDipObject;
    }

}

bool EI_APP_DIP_deleteObject(uint16_t instanceID)
{
    EI_APP_DIP_object_t *previous  = NULL;
    EI_APP_DIP_object_t *temp = NULL;
    int32_t osalRetval;

    osalRetval = OSAL_lockNamedMutex(dipContainer_s.mutex, 1000UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        if((NULL != dipContainer_s.head) && (instanceID == dipContainer_s.head->instanceID))
        {
            temp = dipContainer_s.head->nextObject;
            OSAL_MEMORY_free(dipContainer_s.head);
            dipContainer_s.head = temp;
            OSAL_unLockNamedMutex(dipContainer_s.mutex);
            return  true;
        }

        temp = dipContainer_s.head;
        //find the node to be deleted
        while(NULL !=  temp)
        {
            previous = temp;
            temp  = temp->nextObject;
            if((NULL !=  temp) && (instanceID == temp->instanceID))
            {
                break;
            }

        }

        if(NULL ==  temp)
        {
            OSAL_unLockNamedMutex(dipContainer_s.mutex);
            return false; //the Node could not be found
        }

        previous->nextObject  = temp->nextObject;
        OSAL_MEMORY_free(temp);
        OSAL_unLockNamedMutex(dipContainer_s.mutex);
        return  true;

    }
    else
    {
        OSAL_printf("%s:%d mutex lock error %d\r\n", __func__, __LINE__,  osalRetval);
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return false;
    }
}
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
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
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
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
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
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
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
static uint32_t EI_APP_DIP_getAttrCb(
                              EI_API_CIP_NODE_T* pCipNode,
                              uint16_t classId,
                              uint16_t instanceId,
                              uint16_t attrId,
                              uint16_t* len,
                              void* pvValue)
{
    uint32_t retVal = EI_API_eERR_CB_INVALID_VALUE;
    OSALUNREF_PARM(classId);

    if(EI_API_CIP_eERR_OK == EI_APP_DIP_getObjValue(instanceId, attrId, pvValue))
    {
        *len = sizeof(ei_api_cip_edt_bool);
        retVal = EI_API_eERR_CB_NO_ERROR;
    }
    return retVal;
}

/**
 * \brief reads the value of the requested Attribute from the Object
 * \param[in]  InstanceID the instance-ID of the DIP object
 * \param[in]  attrID the AttributeID to be readed out
 * \param[out] pValue pointer to save the result
 * \return     #EI_API_CIP_EError_t as uint32_t.
*/
uint32_t EI_APP_DIP_getObjValue(uint16_t instanceId, uint16_t attrId, void *pValue)
{
    int32_t osalRetval;
    EI_APP_DIP_object_t *pDipObj = NULL;
    uint32_t retVal = EI_API_CIP_eERR_OK;

    if(NULL != pValue)
    {
        osalRetval = OSAL_lockNamedMutex(dipContainer_s.mutex, 2UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            pDipObj = EI_APP_DIP_findObj(instanceId);
            if(NULL != pDipObj)
            {
                if(3 == attrId)
                {
                    *(ei_api_cip_edt_bool *)pValue = pDipObj->value;
                }
                else if(4 == attrId)
                {
                    *(ei_api_cip_edt_bool *)pValue = pDipObj->status;
                }
                else
                {
                    retVal = EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST;
                }
            }
            else
            {
                retVal = EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST;
            }
            OSAL_unLockNamedMutex(dipContainer_s.mutex);
        }
    }
    else
    {
       retVal = EI_API_CIP_eERR_GENERAL;
    }

    return retVal;
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
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
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

    if(false == dipContainer_s.isClassInitialized)
    {
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
        dipContainer_s.mutex  = OSAL_createNamedMutex("DIP_Mutex");
        if(NULL == dipContainer_s.mutex)
        {
            OSAL_printf("%s:%d create DIP_Mutex failed\r\n", __func__, __LINE__);
            OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        dipContainer_s.head = NULL;
        dipContainer_s.pCipNode = pCipNode;
        dipContainer_s.isClassInitialized =  true;

    }

laError:
    return;
}

/**
 * \brief Create and add a DIP instance object
 * \param[in] InstanceID the instance-ID of the DIP object to be created
 * \param[in] pDipConfig configuration for this instance object
 * \return true if successful, otherwise false
*/
bool EI_APP_DIP_addObject(uint16_t instanceID, EI_DIP_OBJECT_Cfg_t *pDipConfig)
{
    EI_APP_DIP_object_t  *pDipObj = NULL;
    int32_t osalRetval;
    uint32_t errCode;
    bool retval = true;
    if(false == dipContainer_s.isClassInitialized)
    {
        OSAL_printf("%s:%d first call the EI_APP_DIP_init before adding objects\r\n", __func__, __LINE__);
        OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return  false;
    }
    if(NULL == pDipConfig->fuGetInput) //!< get-input-value function pointer is mandatory
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return false;
    }
    if(NULL == pDipConfig->fuGetEvent)  //!< get-event status is mandatory in order to run the objects state-machine
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return false;
    }

    osalRetval = OSAL_lockNamedMutex(dipContainer_s.mutex, 1000UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        //first search if this instance is already created, when yes, then simply return the handle of it.
        pDipObj = EI_APP_DIP_findObj(instanceID);

        if(NULL ==  pDipObj)
        {
            pDipObj = OSAL_MEMORY_calloc(sizeof(EI_APP_DIP_object_t), 1);
            if(NULL != pDipObj)
            {
                pDipObj->instanceID = instanceID;
                pDipObj->current_state = EI_APP_DIP_OBJ_AVAILABLE;
                pDipObj->userCallbacks.fuGetInput = pDipConfig->fuGetInput;
                pDipObj->userCallbacks.fuGetStatus =  pDipConfig->fuGetStatus;
                pDipObj->userCallbacks.fuGetEvent = pDipConfig->fuGetEvent;
                pDipObj->nextObject = NULL;
                errCode = EI_APP_DIP_createInstance(pDipObj);
                if (EI_API_CIP_eERR_OK == errCode)
                {
                    EI_APP_DIP_insertObj(pDipObj);
                }
                else
                {
                    retval = false;
                    OSAL_MEMORY_free(pDipObj);
                    pDipObj = NULL;
                }
            }
            else
            {
                OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            }
        }

        OSAL_unLockNamedMutex(dipContainer_s.mutex);
    }
    else
    {
        retval = false;
        OSAL_printf("%s:%d mutex lock error %d\r\n", __func__, __LINE__,  osalRetval);
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    if(NULL == pDipObj)
    {
        retval = false;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return retval;
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
 * EI_API_CIP_NODE_InitParams_t initParams;
 * initParams.maxInstanceNum = 256;
 *
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new(&initParams);
 *
 * EI_APP_DIP_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DIP
 *
 */
void EI_APP_DIP_run(void)
{
    static volatile EI_API_ADP_SModNetStatus_t networkStatus = {0};
    static volatile EI_APP_DIO_DEVICE_ConnectionState_t connectionStatus = EI_APP_DIO_DEVICE_ConnectionNotEstablished ;
    static volatile EI_APP_DIP_object_t  *pDipObj = NULL;
    int32_t osalRetval;

    osalRetval = OSAL_lockNamedMutex(dipContainer_s.mutex, 2UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        if(NULL != dipContainer_s.head) //is there any object to run at all?
        {
            if(NULL == pDipObj)
            {
                pDipObj = (volatile EI_APP_DIP_object_t  *)dipContainer_s.head;
            }

            pDipObj->userCallbacks.fuGetEvent((EI_APP_DIO_DEVICE_ConnectionState_t *)&connectionStatus, (EI_API_ADP_SModNetStatus_t *)&networkStatus);
            if((EI_API_ADP_eSTATUS_LED_RED_ON == networkStatus.mod) || (EI_API_ADP_eSTATUS_LED_RED_ON == networkStatus.net))
            {
                pDipObj->current_state = EI_APP_DIP_OBJ_UNRECOVERABLE_FAULT;
            }
            else if((EI_API_ADP_eSTATUS_LED_RED_BLINK== networkStatus.mod) ||
             (EI_API_ADP_eSTATUS_LED_RED_BLINK == networkStatus.net) ||
             (EI_APP_DIO_DEVICE_ConnectionClosed == connectionStatus))
            {
                pDipObj->current_state = EI_APP_DIP_OBJ_RECOVERABLE_FAULT;
            }

            if(pDipObj->old_state != pDipObj->current_state)
            {
                pDipObj->old_state = pDipObj->current_state;
            }

            switch (pDipObj->current_state)
            {
            case EI_APP_DIP_OBJ_NON_EXISTENT: //should not happen!
                pDipObj->current_state = EI_APP_DIP_OBJ_AVAILABLE;
                break;
            case EI_APP_DIP_OBJ_AVAILABLE:
                if((EI_API_ADP_eSTATUS_LED_GREEN_ON == networkStatus.net) ||
                 (EI_APP_DIO_DEVICE_ConnectionEstablished == connectionStatus))
                {
                    pDipObj->current_state = EI_APP_DIP_OBJ_RUN;
                }
                break;
            case EI_APP_DIP_OBJ_RUN:
                pDipObj->userCallbacks.fuGetInput(pDipObj->instanceID, (uint8_t *)&pDipObj->value);
                if(NULL != pDipObj->userCallbacks.fuGetStatus)
                {
                    pDipObj->userCallbacks.fuGetStatus(pDipObj->instanceID, (uint8_t *)&pDipObj->status);
                }
                break;
            case EI_APP_DIP_OBJ_RECOVERABLE_FAULT:
                if((EI_API_ADP_eSTATUS_LED_GREEN_ON == networkStatus.net) ||
                 (EI_APP_DIO_DEVICE_ConnectionEstablished == connectionStatus))
                {
                    pDipObj->current_state = EI_APP_DIP_OBJ_RUN;
                }
                break;
            case EI_APP_DIP_OBJ_UNRECOVERABLE_FAULT:
                //no recovery...
                break;
            default:
                //invalid state
                break;
            }
            pDipObj = (volatile EI_APP_DIP_object_t  *)pDipObj->nextObject;
        }

        OSAL_unLockNamedMutex(dipContainer_s.mutex);
    }

}
