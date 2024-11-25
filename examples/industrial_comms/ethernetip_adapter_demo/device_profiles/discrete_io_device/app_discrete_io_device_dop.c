/*!
 *  \file app_discrete_io_device_dop.c
 *
 *  \brief
 *  EtherNet/IP&trade; Discrete Output Point Object.
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
#include "device_profiles/discrete_io_device/app_discrete_io_device_dop.h"

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"


typedef enum EI_APP_DOP_events
{
    EI_APP_DOP_EVENT_ReceiveData = 0,
    EI_APP_DOP_EVENT_ReceiveIdle_Command,
    EI_APP_DOP_EVENT_ReceiveIdle_InvalidData,
    EI_APP_DOP_EVENT_ReceiveFault,
    EI_APP_DOP_EVENT_ReceiveRun_Command,
    EI_APP_DOP_EVENT_UnrecoverableFault,
    EI_APP_DOP_EVENT_ConnDeleted,
    EI_APP_DOP_EVENT_ConnEstablished,
    EI_APP_DOP_EVENT_ConnTimedOut,
    EI_APP_DOP_EVENT_NoEvent
}EI_APP_DOP_events_t;

typedef enum EI_APP_DOP_dataContext
{
    EI_APP_DOP_DATA_Explicit = 0,
    EI_APP_DOP_DATA_Implicit
}EI_APP_DOP_dataContext_t;

typedef struct EI_APP_DOP_Value
{
    volatile ei_api_cip_edt_usint         value;
    volatile EI_APP_DOP_dataContext_t     context;
}EI_APP_DOP_Value_t;

/**
 *
*/
typedef struct EI_APP_DOP_object
{
    uint16_t                      instanceID;
    EI_DOP_OBJECT_Cfg_t           userCfg;
    volatile bool                 faultSettingChanged;
    volatile bool                 idleSettingChanged;
    volatile bool                 runIdleValueChanged;
    volatile bool                 receiveDataEvent;
    volatile bool                 receiveIdleEvent;
    EI_APP_DOP_Value_t            valueContainer;
    volatile ei_api_cip_edt_bool    run_idle_command;
    volatile EI_APP_DOP_SmStates_t  current_state;
    volatile EI_APP_DOP_SmStates_t  old_state;
    void (*processesFnc)(struct EI_APP_DOP_object *, EI_APP_DOP_events_t);
    struct EI_APP_DOP_object        *nextObject;
}EI_APP_DOP_object_t;

typedef struct EI_APP_DOP_container
{
    bool                        isClassInitialized;
    EI_APP_DOP_object_t         *head;
    EI_API_CIP_NODE_T           *pCipNode;
    void                        *mutex;
}EI_APP_DOP_container_t;

typedef void (*dop_state_proccess_t)(EI_APP_DOP_object_t *, EI_APP_DOP_events_t);
//--------------------------------------------------------------------
static void dop_proc_noneExistent(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event);
static void dop_proc_available(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event);
static void dop_proc_idle(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event);
static void dop_proc_ready(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event);
static void dop_proc_run(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event);
static void dop_proc_recoverableFault(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event);
static void dop_proc_unrecoverableFault(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event);
static inline EI_APP_DOP_events_t dop_get_other_evnets(EI_APP_DOP_object_t *);
static uint32_t EI_APP_DOP_setObjValue(uint16_t instanceID, uint16_t attrID, void *pValue, EI_APP_DOP_dataContext_t context);


static EI_APP_DOP_container_t dopContainer_s = {0};

static EI_APP_DOP_ClassData_t dopClassData_s = {.revision = EI_APP_DIO_DEVICE_DOP_REVISION_NUMBER
                                               };

static EI_APP_DOP_object_t*  EI_APP_DOP_findObj(uint16_t instanceID)
{
    EI_APP_DOP_object_t  *pObj = NULL;

    if(false == dopContainer_s.isClassInitialized)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return NULL;
    }
    else
    {
        pObj  = dopContainer_s.head;
        while (NULL != pObj)
        {
            if(instanceID == pObj->instanceID)
            {
                break;
            }
            pObj = pObj->nextObject;
        }
    }

    return pObj;
}

static void EI_APP_DOP_insertObj(EI_APP_DOP_object_t *pDopObject)
{

    if(NULL == dopContainer_s.head) //first item?
    {
        dopContainer_s.head = pDopObject;
        dopContainer_s.head->nextObject = NULL;
    }
    else
    {
        pDopObject->nextObject = dopContainer_s.head;
        dopContainer_s.head = pDopObject;
    }
}

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
 * errCode = EI_APP_DOP_addClassAttribute(pEI_API_CIP_NODE, 0x0001, &revision);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
static uint32_t EI_APP_DOP_addClassAttribute(EI_API_CIP_NODE_T* pCipNode, uint16_t attrId, void* pvValue)
{
    uint32_t errCode;
    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));

    attr.id         = attrId;
    attr.edt        = EI_API_CIP_eEDT_UINT;
    attr.accessRule = EI_API_CIP_eAR_GET;
    attr.pvValue    = pvValue;

    errCode = EI_API_CIP_addClassAttr(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, &attr);
    if(errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    errCode = EI_API_CIP_setClassAttr(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, &attr);
    if(errCode != EI_API_CIP_eERR_OK)
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
static inline void EI_APP_DOP_OutputFault(EI_APP_DOP_object_t  *pObj, uint8_t updateOutput)
{
    if(0 == pObj->userCfg.FaultAction)
    {
        pObj->valueContainer.value = pObj->userCfg.FaultValue;
        if(updateOutput)
        {
            pObj->userCfg.fuSetOutput(pObj->instanceID, pObj->userCfg.FaultValue);
        }
    }
    else
    {
        //nothing to do, hold the last state
    }
}
/**
 *
*/
static inline void EI_APP_DOP_OutputIdle(EI_APP_DOP_object_t  *pObj, uint8_t updateOutput)
{
    if(0 == pObj->userCfg.IdleAction)
    {
        pObj->valueContainer.value = pObj->userCfg.IdleValue;
        if(updateOutput)
        {
            pObj->userCfg.fuSetOutput(pObj->instanceID, pObj->userCfg.IdleValue);
        }
    }
    else
    {
        //Nothing to do, hold the last state
    }
}

/**
 *
*/
static void EI_APP_DOP_ChangeToState(EI_APP_DOP_object_t  *pObj, EI_APP_DOP_SmStates_t state)
{
    pObj->old_state = pObj->current_state;
    pObj->current_state = state;
    switch (pObj->current_state)
    {
    case EI_APP_DOP_SM_NONEXISTENT:
        pObj->processesFnc = dop_proc_noneExistent;
        break;
    case EI_APP_DOP_SM_AVAILABLE:
        pObj->processesFnc = dop_proc_available;
        break;
    case EI_APP_DOP_SM_IDLE:
        pObj->processesFnc = dop_proc_idle;
        break;
    case EI_APP_DOP_SM_READY:
        pObj->processesFnc = dop_proc_ready;
        break;
    case  EI_APP_DOP_SM_RUN:
        pObj->processesFnc = dop_proc_run;
        break;
    case EI_APP_DOP_SM_RECOVERABLEFAULT:
        pObj->processesFnc = dop_proc_recoverableFault;
        break;
    case EI_APP_DOP_SM_UNRECOVERABLEFAULT:
        pObj->processesFnc = dop_proc_unrecoverableFault;
        break;
    default:
        break;
    }

}

/**
 *
*/
static uint32_t EI_APP_DOP_setObjValue(uint16_t instanceID, uint16_t attrID, void *pValue, EI_APP_DOP_dataContext_t context)
{
    int32_t osalRetval;
    EI_APP_DOP_object_t *pDopObj = NULL;
    uint32_t retVal = EI_API_eERR_CB_NO_ERROR;
    EI_APP_DOP_events_t event = EI_APP_DOP_EVENT_NoEvent;

    if(NULL != pValue)
    {
        osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 5UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            pDopObj = EI_APP_DOP_findObj(instanceID);
            if(NULL != pDopObj)
            {
                if(EI_APP_DOP_SM_UNRECOVERABLEFAULT != pDopObj->current_state)
                {
                    switch (attrID)
                    {
                    case 3: //!< value attribute
                        if(*(ei_api_cip_edt_usint *)pValue <= 1)
                        {
                            pDopObj->valueContainer.value = *(ei_api_cip_edt_usint *)pValue;
                            pDopObj->valueContainer.context = context;
                            if(EI_APP_DOP_DATA_Implicit == context)
                            {
                                pDopObj->receiveDataEvent = true;
                            }
                        }
                        else
                        {
                            retVal = EI_API_eERR_CB_INVALID_VALUE;
                        }
                        break;
                    case 5: //!< Fault Action
                        pDopObj->userCfg.FaultAction = *(ei_api_cip_edt_bool *)pValue;
                        pDopObj->faultSettingChanged = true;
                        break;
                    case 6: //!< Fault Value
                        pDopObj->userCfg.FaultValue = *(ei_api_cip_edt_bool *)pValue;
                        pDopObj->faultSettingChanged = true;
                        break;
                    case 7: //!< Idle Action
                        pDopObj->idleSettingChanged = true;
                        pDopObj->userCfg.IdleAction = *(ei_api_cip_edt_bool *)pValue;
                        break;
                    case 8: //!< Idle value
                        pDopObj->idleSettingChanged = true;
                        pDopObj->userCfg.IdleValue = *(ei_api_cip_edt_bool *)pValue;
                        break;
                    case 9: //!< Run_Idle_Command
                        if( (EI_APP_DOP_SM_IDLE == pDopObj->current_state) ||
                            (EI_APP_DOP_SM_READY == pDopObj->current_state) ||
                            (EI_APP_DOP_SM_RUN == pDopObj->current_state))
                            {
                                pDopObj->runIdleValueChanged = true;
                                pDopObj->run_idle_command = *(ei_api_cip_edt_bool *)pValue;
                            }
                            else
                            {
                                retVal = EI_API_eERR_CB_CONFLICT_STATE;
                            }

                        break;
                    default:
                        retVal = EI_API_eERR_CB_INVALID_VALUE;
                        break;
                    }

                    event = dop_get_other_evnets(pDopObj);
                    if((NULL != pDopObj->processesFnc) && (EI_APP_DOP_EVENT_NoEvent != event))
                    {
                        pDopObj->processesFnc(pDopObj, event);
                    }
                }
            }
            else
            {
                retVal = EI_API_eERR_CB_INVALID_VALUE;
            }
            OSAL_unLockNamedMutex(dopContainer_s.mutex);
        }
        else
        {
            retVal = EI_API_eERR_CB_NOT_ENOUGH_DATA;
        }
    }
    else
    {
        retVal = EI_API_eERR_CB_INVALID_VALUE;
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
uint32_t EI_APP_DOP_getObjValue(uint16_t instanceId, uint16_t attrID, void *pValue)
{
    int32_t osalRetval;
    EI_APP_DOP_object_t *pDopObj = NULL;
    uint32_t retVal = EI_API_eERR_CB_NO_ERROR;
    if(NULL != pValue)
    {
        osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 2UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            pDopObj = EI_APP_DOP_findObj(instanceId);
            if(NULL != pDopObj)
            {
                switch (attrID)
                {
                case 3: //!< value attribute
                    *(ei_api_cip_edt_usint *)pValue = pDopObj->valueContainer.value;
                    break;
                case 5: //!< Fault Action
                    *(ei_api_cip_edt_bool *)pValue = pDopObj->userCfg.FaultAction;
                    break;
                case 6: //!< Fault Value
                    *(ei_api_cip_edt_bool *)pValue = pDopObj->userCfg.FaultValue;
                    break;
                case 7: //!< Idle Action
                     *(ei_api_cip_edt_bool *)pValue = pDopObj->userCfg.IdleAction;
                    break;
                case 8: //!< Idle value
                    *(ei_api_cip_edt_bool *)pValue = pDopObj->userCfg.IdleValue;
                    break;
                case 9: //!< Run_Idle_Command
                    *(ei_api_cip_edt_bool *)pValue = 0; //always return 0, as noted in specification
                    break;
                case 12: //!< object state
                    *(ei_api_cip_edt_usint *)pValue = (ei_api_cip_edt_usint)pDopObj->current_state;
                    break;
                default:
                    retVal = EI_API_eERR_CB_INVALID_VALUE;
                    break;
                }

            }
            else
            {
                retVal = EI_API_eERR_CB_INVALID_VALUE;
            }
            OSAL_unLockNamedMutex(dopContainer_s.mutex);
        }
        else
        {
            retVal = EI_API_eERR_CB_NOT_ENOUGH_DATA;
        }
    }
    else
    {
       retVal = EI_API_eERR_CB_INVALID_VALUE;
    }

    return retVal;
}


/**
 *
 */
void EI_APP_DOP_receiveConnectionEvent(uint16_t instanceID, EI_APP_DOP_ConnectionEvent_t connectionEvent)
{
    int32_t osalRetval;
    EI_APP_DOP_object_t *pDopObj = NULL;
    EI_APP_DOP_events_t event = EI_APP_DOP_EVENT_NoEvent;
    osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 5UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        pDopObj = EI_APP_DOP_findObj(instanceID);
        if(NULL != pDopObj)
        {
            if(NULL != pDopObj->processesFnc)
            {
                switch (connectionEvent)
                {
                case EI_APP_DOP_EV_ConnDeleted:
                    event = EI_APP_DOP_EVENT_ConnDeleted;
                    break;
                case EI_APP_DOP_EV_ConnEstablished:
                    event = EI_APP_DOP_EVENT_ConnEstablished;
                    break;
                case EI_APP_DOP_EV_ConnTimedOut:
                    event = EI_APP_DOP_EVENT_ConnTimedOut;
                    break;
                }
                pDopObj->processesFnc(pDopObj, event);
            }
        }

        OSAL_unLockNamedMutex(dopContainer_s.mutex);
    }
}
/**
 *
 */
void EI_APP_DOP_receiveIdleEvent(uint16_t instanceID)
{
    int32_t osalRetval;
    EI_APP_DOP_object_t *pDopObj = NULL;

    osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 2UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        pDopObj = EI_APP_DOP_findObj(instanceID);
        if(NULL != pDopObj)
        {
            if(NULL != pDopObj->processesFnc)
            {
                pDopObj->processesFnc(pDopObj, EI_APP_DOP_EVENT_ReceiveIdle_InvalidData);
            }
        }

        OSAL_unLockNamedMutex(dopContainer_s.mutex);
    }
}

/**
 *
 */
uint32_t EI_APP_DOP_receiveDataEvent(uint16_t instanceID, ei_api_cip_edt_bool value)
{
    return EI_APP_DOP_setObjValue(instanceID, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, (void *)&value, EI_APP_DOP_DATA_Implicit);
}

/**
 *
 */
uint32_t EI_APP_DOP_setCommand(uint16_t instanceID, ei_api_cip_edt_bool idleRunCommand)
{
    return EI_APP_DOP_setObjValue(instanceID, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_09, (void *)&idleRunCommand, EI_APP_DOP_DATA_Explicit);
}
/**
 *
 */
uint32_t EI_APP_DOP_setFaultAction(uint16_t instanceID, ei_api_cip_edt_bool faultAction)
{
    return EI_APP_DOP_setObjValue(instanceID, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_05, (void *)&faultAction, EI_APP_DOP_DATA_Explicit);
}
/**
 *
 */
uint32_t EI_APP_DOP_setFaultValue(uint16_t instanceID, ei_api_cip_edt_bool faultValue)
{
    return EI_APP_DOP_setObjValue(instanceID, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06, (void *)&faultValue, EI_APP_DOP_DATA_Explicit);
}
/**
 *
 */
uint32_t EI_APP_DOP_setIdleAction(uint16_t instanceID, ei_api_cip_edt_bool idleAction)
{
    return EI_APP_DOP_setObjValue(instanceID, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_07, (void *)&idleAction, EI_APP_DOP_DATA_Explicit);
}
/**
 *
 */
uint32_t EI_APP_DOP_setIdleValue(uint16_t instanceID, ei_api_cip_edt_bool idleValue)
{
    return EI_APP_DOP_setObjValue(instanceID, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_08, (void *)&idleValue, EI_APP_DOP_DATA_Explicit);
}


static uint32_t EI_APP_DOP_setValueCb(
                              EI_API_CIP_NODE_T* pCipNode,
                              uint16_t classId,
                              uint16_t instanceId,
                              uint16_t attrId,
                              uint16_t len,
                              void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(len);
    OSALUNREF_PARM(pCipNode);

    uint32_t retVal = EI_API_eERR_CB_INVALID_VALUE;

    retVal = EI_APP_DOP_setObjValue(instanceId, attrId, pvValue, EI_APP_DOP_DATA_Explicit);

    return retVal;
}

static uint32_t EI_APP_DOP_getAttrCb(
                              EI_API_CIP_NODE_T* pCipNode,
                              uint16_t classId,
                              uint16_t instanceId,
                              uint16_t attrId,
                              uint16_t* len,
                              void* pvValue)
{
    uint32_t retVal = EI_API_eERR_CB_INVALID_VALUE;
    OSALUNREF_PARM(classId);

    retVal = EI_APP_DOP_getObjValue(instanceId, attrId, pvValue);
    *len = sizeof(ei_api_cip_edt_usint);

    return retVal;
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
 * errCode = EI_APP_DOP_addInstanceAttribute(
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
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
static uint32_t EI_APP_DOP_addInstanceAttribute(
                                                EI_API_CIP_NODE_T*   pCipNode,
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
    errCode = EI_API_CIP_addInstanceAttr(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set initial value
    errCode = EI_API_CIP_setInstanceAttr(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set callback function
    if (attr.get_callback != NULL || attr.set_callback != NULL)
    {
        errCode = EI_API_CIP_setInstanceAttrFunc(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }
    }

laError:
    return errCode;
}

static uint32_t EI_APP_DOP_createInstance(EI_APP_DOP_object_t *pDopObject)
{
    ei_api_cip_edt_bool instanceValue = 0;
    EI_API_CIP_SService_t service = {0};
    EI_API_CIP_EAr_t      cipAccess = EI_API_CIP_eAR_GET_AND_SET;
    uint32_t errCode;

    if(pDopObject->userCfg.isBindedToGroup)
    {
        cipAccess = EI_API_CIP_eAR_GET;
    }
    // Create instances
    errCode = EI_API_CIP_createInstance(dopContainer_s.pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, pDopObject->instanceID);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add set & get service for instances
    service.code = EI_API_CIP_eSC_SETATTRSINGLE;
    errCode = EI_API_CIP_addInstanceService(dopContainer_s.pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, pDopObject->instanceID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addInstanceService(dopContainer_s.pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, pDopObject->instanceID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 3 Value (required) for instance
    errCode = EI_APP_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03,
                                                EI_API_CIP_eEDT_USINT,
                                                EI_API_CIP_eAR_GET_AND_SET,
                                                EI_APP_DOP_getAttrCb,
                                                EI_APP_DOP_setValueCb,
                                                sizeof(ei_api_cip_edt_usint),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 5 Fault Action (optional) for instance
    errCode = EI_APP_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_05,
                                                EI_API_CIP_eEDT_BOOL,
                                                cipAccess,
                                                EI_APP_DOP_getAttrCb,
                                                ((pDopObject->userCfg.isBindedToGroup) ? (NULL) : (EI_APP_DOP_setValueCb)),
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 6 Fault Value (optional) for instance
    errCode = EI_APP_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06,
                                                EI_API_CIP_eEDT_BOOL,
                                                cipAccess,
                                                EI_APP_DOP_getAttrCb,
                                                ((pDopObject->userCfg.isBindedToGroup) ? (NULL) : (EI_APP_DOP_setValueCb)),
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 7 Idle Action (optional) for instance
    errCode = EI_APP_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_07,
                                                EI_API_CIP_eEDT_BOOL,
                                                cipAccess,
                                                EI_APP_DOP_getAttrCb,
                                                ((pDopObject->userCfg.isBindedToGroup) ? (NULL) : (EI_APP_DOP_setValueCb)),
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 8 Idle Value (optional) for instance
    errCode = EI_APP_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_08,
                                                EI_API_CIP_eEDT_BOOL,
                                                cipAccess,
                                                EI_APP_DOP_getAttrCb,
                                                ((pDopObject->userCfg.isBindedToGroup) ? (NULL) : (EI_APP_DOP_setValueCb)),
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 9 Run_Idle_Command (optional) for instance
    errCode = EI_APP_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_09,
                                                EI_API_CIP_eEDT_BOOL,
                                                EI_API_CIP_eAR_GET_AND_SET,
                                                EI_APP_DOP_getAttrCb,
                                                EI_APP_DOP_setValueCb,
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 12 object state (optional) for instance
    errCode = EI_APP_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                12,
                                                EI_API_CIP_eEDT_USINT,
                                                EI_API_CIP_eAR_GET,
                                                EI_APP_DOP_getAttrCb,
                                                NULL,
                                                sizeof(ei_api_cip_edt_usint),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
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
 * EI_APP_DOP_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
void EI_APP_DOP_init(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    EI_API_CIP_SService_t service;

    if(false == dopContainer_s.isClassInitialized)
    {
        errCode = EI_API_CIP_createClass(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID);

        // Example how to evaluate error codes returned by API functions.
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // set class instance
        OSAL_MEMORY_memset(&service, 0, sizeof(service));
        service.code = EI_API_CIP_eSC_GETATTRSINGLE;
        errCode = EI_API_CIP_addClassService(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, &service);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }
        errCode = EI_APP_DOP_addClassAttribute(pCipNode, 1, &dopClassData_s.revision);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        dopContainer_s.mutex  = OSAL_createNamedMutex("DOP_Mutex");
        if(NULL == dopContainer_s.mutex)
        {
            OSAL_printf("%s:%d create DOP_Mutex failed\r\n", __func__, __LINE__);
            OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        dopContainer_s.head = NULL;
        dopContainer_s.pCipNode = pCipNode;
        dopContainer_s.isClassInitialized =  true;
    }

laError:
    return;
}

/**
 * \brief Create and add a DOP instance object
 * \param[in] InstanceID the instance-ID of the DOP object to be created
 * \param[in] pDipConfig configuration for this instance object
 * \return true if successful, otherwise false
*/
bool EI_APP_DOP_addObject(uint16_t instanceID, EI_DOP_OBJECT_Cfg_t *pDopConfig)
{
    EI_APP_DOP_object_t  *pDopObj = NULL;
    int32_t osalRetval;
    uint32_t errCode;
    bool retval = true;
    if(false == dopContainer_s.isClassInitialized)
    {
        OSAL_printf("%s:%d first call the EI_APP_DOP_init before adding objects\r\n", __func__, __LINE__);
        OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return  false;
    }
    if(NULL == pDopConfig->fuSetOutput) //!< set-output-value function pointer is mandatory
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return false;
    }
    if(NULL == pDopConfig->fuGetEvent)  //!< get-event status is mandatory in order to run the objects state-machine
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        return false;
    }

    osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 1000UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        //first search if this instance is already created, when yes, then simply return the handle of it.
        pDopObj = EI_APP_DOP_findObj(instanceID);

        if(NULL ==  pDopObj)
        {
            pDopObj = OSAL_MEMORY_calloc(sizeof(EI_APP_DOP_object_t), 1);
            if(NULL != pDopObj)
            {
                OSAL_MEMORY_memcpy(&pDopObj->userCfg, pDopConfig, sizeof(EI_DOP_OBJECT_Cfg_t));
                pDopObj->instanceID = instanceID;
                pDopObj->current_state = EI_APP_DOP_SM_AVAILABLE;
                pDopObj->old_state = EI_APP_DOP_SM_NONEXISTENT;
                pDopObj->processesFnc = dop_proc_available;
                pDopObj->nextObject = NULL;
                pDopObj->valueContainer.value = 0;
                pDopObj->valueContainer.context = 0;
                pDopObj->run_idle_command = 0;
                pDopObj->faultSettingChanged = false;
                pDopObj->idleSettingChanged = false;
                pDopObj->runIdleValueChanged = false;
                pDopObj->receiveDataEvent = false;
                pDopObj->receiveIdleEvent = false;
                errCode = EI_APP_DOP_createInstance(pDopObj);
                if (EI_API_CIP_eERR_OK == errCode)
                {
                    EI_APP_DOP_insertObj(pDopObj);
                }
                else
                {
                    retval = false;
                    OSAL_MEMORY_free(pDopObj);
                    pDopObj = NULL;
                }
            }
            else
            {
                OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            }
        }

        OSAL_unLockNamedMutex(dopContainer_s.mutex);
    }
    else
    {
        retval = false;
        OSAL_printf("%s:%d mutex lock error %d\r\n", __func__, __LINE__,  osalRetval);
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    if(NULL == pDopObj)
    {
        retval = false;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return retval;
}

/**
 * \brief deletes the DOP object(if exist)
 * \param[in] instanceID the instance-ID of the object to be removed
 * \return  true if successful, false if the object does not exist
*/
bool EI_APP_DOP_deleteObject(uint16_t instanceID)
{
    EI_APP_DOP_object_t *previous  = NULL;
    EI_APP_DOP_object_t *temp = NULL;
    int32_t osalRetval;
    bool retval = false;

    osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 1000UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        if((NULL != dopContainer_s.head) && (instanceID == dopContainer_s.head->instanceID))
        {
            temp = dopContainer_s.head->nextObject;
            OSAL_MEMORY_free(dopContainer_s.head);
            dopContainer_s.head = temp;
            retval =  true;
        }
        else
        {
            temp = dopContainer_s.head;
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
                retval = false; //the Node could not be found
            }
            else
            {
                previous->nextObject  = temp->nextObject;
                OSAL_MEMORY_free(temp);
                retval = true;
            }
        }

        OSAL_unLockNamedMutex(dopContainer_s.mutex);
    }
    else
    {
        OSAL_printf("%s:%d mutex lock error %d\r\n", __func__, __LINE__,  osalRetval);
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        retval = false;
    }

    return retval;
}

/*!
 *
 * \brief
 * Run function for class DOP.
 *
 * \details
 * This function must be called cyclically. It updates the value and turns on & off the industrial
 * LEDs controlled by TPIC2810.
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
 * EI_APP_DOP_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
void EI_APP_DOP_run (void)
{
    static volatile EI_API_ADP_SModNetStatus_t networkStatus = {0};
    static volatile EI_APP_DIO_DEVICE_ConnectionState_t connectionStatus = EI_APP_DIO_DEVICE_ConnectionNotEstablished ;
    static volatile EI_APP_DOP_object_t  *pDopObj = NULL;
    dop_state_proccess_t currentProcess = NULL;
    volatile EI_APP_DOP_events_t event =  EI_APP_DOP_EVENT_NoEvent;
    int32_t osalRetval;

    osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 2UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        if(NULL != dopContainer_s.head)
        {
            if(NULL == pDopObj)
            {
                pDopObj = (volatile EI_APP_DOP_object_t  *)dopContainer_s.head;
            }

            currentProcess = pDopObj->processesFnc;
            pDopObj->userCfg.fuGetEvent((EI_APP_DIO_DEVICE_ConnectionState_t *)&connectionStatus, (EI_API_ADP_SModNetStatus_t *)&networkStatus);
            if((EI_API_ADP_eSTATUS_LED_RED_ON == networkStatus.mod) || (EI_API_ADP_eSTATUS_LED_RED_ON == networkStatus.net))
            {
                event = EI_APP_DOP_EVENT_UnrecoverableFault;
            }

            if(NULL != currentProcess)
            {
                currentProcess((EI_APP_DOP_object_t  *)pDopObj, event);
            }

            pDopObj = (volatile EI_APP_DOP_object_t *)pDopObj->nextObject;
        }
        OSAL_unLockNamedMutex(dopContainer_s.mutex);
    }
}

//-------------------------------------------------------------------------------------------
static inline EI_APP_DOP_events_t dop_get_other_evnets(EI_APP_DOP_object_t *obj)
{
    EI_APP_DOP_events_t event = EI_APP_DOP_EVENT_NoEvent;

    if(obj->runIdleValueChanged)
    {
        obj->runIdleValueChanged = false;
        if(obj->run_idle_command)
        {
            event = EI_APP_DOP_EVENT_ReceiveRun_Command;
        }
        else
        {
            event = EI_APP_DOP_EVENT_ReceiveIdle_Command;
        }
    }
    else if(obj->receiveDataEvent)
    {
        obj->receiveDataEvent = false;
        event = EI_APP_DOP_EVENT_ReceiveData;
    }


    return event;
}
//-------------------------------------------------------------------------------------------
static void dop_proc_noneExistent(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event)
{
    if(EI_APP_DOP_EVENT_UnrecoverableFault == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_AVAILABLE);
    }
}
//------------------------------------------------------------------------------------------
static void dop_proc_available(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event)
{
    if(EI_APP_DOP_EVENT_UnrecoverableFault == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if((EI_APP_DOP_EVENT_ConnEstablished == event) ||
            (EI_APP_DOP_EVENT_ReceiveIdle_InvalidData == event) ||
            (EI_APP_DOP_EVENT_ReceiveData == event))
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_READY);
    }
    // else
    // {
    //     obj->valueContainer.value = 0; //output-OFF
    // }
}
//-------------------------------------------------------------------------------------------
static void dop_proc_idle(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event)
{
    if(EI_APP_DOP_EVENT_UnrecoverableFault == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if(EI_APP_DOP_EVENT_ReceiveFault == event)
    {
        //output in fault state
        EI_APP_DOP_OutputFault(obj, 1);
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_RECOVERABLEFAULT);
    }
    else if(EI_APP_DOP_EVENT_ConnTimedOut == event)
    {
        //output remain unchanged
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_RECOVERABLEFAULT);
    }
    else if(EI_APP_DOP_EVENT_ConnDeleted == event)
    {
        //output Off
        obj->valueContainer.value = 0;
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_AVAILABLE);
    }
    else if(EI_APP_DOP_EVENT_ReceiveRun_Command == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_READY);
    }
    else if(EI_APP_DOP_EVENT_ReceiveData == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_RUN);
    }
    else if(obj->idleSettingChanged)
    {
        obj->idleSettingChanged = false;
        //update Idle-output
        EI_APP_DOP_OutputIdle(obj, 1);
    }
}
//-------------------------------------------------------------------------------------------
static void dop_proc_ready(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event)
{
    if(EI_APP_DOP_EVENT_UnrecoverableFault == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if(EI_APP_DOP_EVENT_ReceiveFault == event)
    {
        //output in fault state
        EI_APP_DOP_OutputFault(obj, 1);
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_RECOVERABLEFAULT);
    }
    else if(EI_APP_DOP_EVENT_ConnTimedOut == event)
    {
        /**
         * The state diagram in the specification is misleading,
         * in case of Timeout in Ready state, output should take "Fault" value.
         */
        EI_APP_DOP_OutputFault(obj, 1);
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_RECOVERABLEFAULT);
    }
    else if((EI_APP_DOP_EVENT_ReceiveIdle_Command == event) ||
            (EI_APP_DOP_EVENT_ReceiveIdle_InvalidData == event))
    {
        //output in Idle state
        EI_APP_DOP_OutputIdle(obj, 1);
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_IDLE);
    }
    else if(EI_APP_DOP_EVENT_ConnDeleted == event)
    {
        //output Off
        obj->valueContainer.value = 0;
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_AVAILABLE);
    }
    else if(EI_APP_DOP_EVENT_ReceiveData == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_RUN);
    }
}
//---------------------------------------------------------------------------------------------
static void dop_proc_run(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event)
{
    if(EI_APP_DOP_EVENT_UnrecoverableFault == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if(EI_APP_DOP_EVENT_ReceiveFault == event)
    {
        //output in fault state
        EI_APP_DOP_OutputFault(obj, 0);
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_RECOVERABLEFAULT);
    }
    else if(EI_APP_DOP_EVENT_ConnTimedOut == event)
    {
        //output in fault state
        EI_APP_DOP_OutputFault(obj, 0);
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_RECOVERABLEFAULT);
    }
    else if((EI_APP_DOP_EVENT_ReceiveIdle_Command == event) ||
            (EI_APP_DOP_EVENT_ReceiveIdle_InvalidData == event))
    {
        //output in Idle state
        EI_APP_DOP_OutputIdle(obj, 0);
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_IDLE);
    }
    else if(EI_APP_DOP_EVENT_ConnDeleted == event)
    {
        //output Off
        obj->valueContainer.value = 0;
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_AVAILABLE);
    }

    obj->userCfg.fuSetOutput(obj->instanceID, obj->valueContainer.value); //update output
}
//----------------------------------------------------------------------------------------------
static void dop_proc_recoverableFault(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event)
{
    if(EI_APP_DOP_EVENT_UnrecoverableFault == event)
    {
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if(EI_APP_DOP_EVENT_ConnDeleted == event)
    {
        //output Off
        obj->valueContainer.value = 0;
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_AVAILABLE);
    }
    else if(obj->faultSettingChanged)
    {
        obj->faultSettingChanged = false;
        //update fault-output
        EI_APP_DOP_OutputFault(obj, 1);
    }
    else if((EI_APP_DOP_EVENT_ConnEstablished == event))
    {
        //output unchanged
        EI_APP_DOP_ChangeToState(obj, EI_APP_DOP_SM_READY);
    }
}
//----------------------------------------------------------------------------------------------
static void dop_proc_unrecoverableFault(EI_APP_DOP_object_t *obj, EI_APP_DOP_events_t event)
{
    //Nothing to do, there is no coming back!
}
//-----------------------------------------------------------------------------------------------
