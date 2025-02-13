/*!
 *  \file discrete_io_device_dop.c
 *
 *  \brief
 *  EtherNet/IP&trade; Discrete Output Point Object.
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
#include "device_profiles/discrete_io_device/discrete_io_device_dop.h"

typedef enum DIO_DEVICE_DOP_events
{
    DIO_DEVICE_DOP_EVENT_RECEIVE_DATA = 0,
    DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_COMMAND,
    DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_INVALID_DATA,
    DIO_DEVICE_DOP_EVENT_RECEIVE_FAULT,
    DIO_DEVICE_DOP_EVENT_RECEIVE_RUN_COMMAND,
    DIO_DEVICE_DOP_EVENT_UNRECOVERABLE_FAULT,
    DIO_DEVICE_DOP_EVENT_CONNECTION_DELETED,
    DIO_DEVICE_DOP_EVENT_CONNECTION_ESTABLISHED,
    DIO_DEVICE_DOP_EVENT_CONNECTION_TIMEDOUT,
    DIO_DEVICE_DOP_EVENT_NOEVENT
}DIO_DEVICE_DOP_events_t;

typedef enum DIO_DEVICE_DOP_dataContext
{
    DIO_DEVICE_DOP_DATA_EXPLICIT = 0,
    DIO_DEVICE_DOP_DATA_IMPLICIT
}DIO_DEVICE_DOP_dataContext_t;

typedef struct DIO_DEVICE_DOP_Value
{
    volatile ei_api_cip_edt_usint         value;
    volatile DIO_DEVICE_DOP_dataContext_t context;
}DIO_DEVICE_DOP_Value_t;

/**
 *
*/
typedef struct DIO_DEVICE_DOP_object
{
    uint16_t                      instanceID;
    DIO_DEVICE_DOP_ObjectCfg_t    userCfg;
    volatile bool                 faultSettingChanged;
    volatile bool                 idleSettingChanged;
    volatile bool                 runIdleValueChanged;
    volatile bool                 receiveDataEvent;
    volatile bool                 receiveIdleEvent;
    DIO_DEVICE_DOP_Value_t        valueContainer;
    volatile ei_api_cip_edt_bool    run_idle_command;
    volatile DIO_DEVICE_DOP_SmStates_t  current_state;
    volatile DIO_DEVICE_DOP_SmStates_t  old_state;
    void (*processesFnc)(struct DIO_DEVICE_DOP_object *, DIO_DEVICE_DOP_events_t);
    struct DIO_DEVICE_DOP_object        *nextObject;
}DIO_DEVICE_DOP_object_t;

typedef struct DIO_DEVICE_DOP_container
{
    bool                        isClassInitialized;
    DIO_DEVICE_DOP_object_t    *head;
    EI_API_CIP_NODE_T          *pCipNode;
    void                       *mutex;
}DIO_DEVICE_DOP_container_t;

typedef void (*dop_state_proccess_t)(DIO_DEVICE_DOP_object_t *, DIO_DEVICE_DOP_events_t);
//--------------------------------------------------------------------
static void DIO_DEVICE_DOP_procNoneExistent(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event);
static void DIO_DEVICE_DOP_procAvailable(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event);
static void DIO_DEVICE_DOP_procIdle(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event);
static void DIO_DEVICE_DOP_procReady(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event);
static void DIO_DEVICE_DOP_procRun(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event);
static void DIO_DEVICE_DOP_procRecoverableFault(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event);
static void DIO_DEVICE_DOP_procUnrecoverableFault(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event);
static inline DIO_DEVICE_DOP_events_t DIO_DEVICE_DOP_getOtherEvents(DIO_DEVICE_DOP_object_t *);
static uint32_t DIO_DEVICE_DOP_setObjValue(uint16_t instanceID, uint16_t attrID, void *pValue, DIO_DEVICE_DOP_dataContext_t context);


static DIO_DEVICE_DOP_container_t dopContainer_s = {0};

static DIO_DEVICE_DOP_ClassData_t dopClassData_s = {.revision = CFG_PROFILE_DIO_DEVICE_DOP_REVISION_NUMBER
                                               };

static DIO_DEVICE_DOP_object_t*  DIO_DEVICE_DOP_findObject(uint16_t instanceID)
{
    DIO_DEVICE_DOP_object_t  *pObj = NULL;

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

static void DIO_DEVICE_DOP_insertObject(DIO_DEVICE_DOP_object_t *pDopObject)
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
 * errCode = DIO_DEVICE_DOP_addClassAttribute(pEI_API_CIP_NODE, 0x0001, &revision);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
static uint32_t DIO_DEVICE_DOP_addClassAttribute(EI_API_CIP_NODE_T* pCipNode, uint16_t attrId, void* pvValue)
{
    uint32_t errCode;
    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));

    attr.id         = attrId;
    attr.edt        = EI_API_CIP_eEDT_UINT;
    attr.accessRule = EI_API_CIP_eAR_GET;
    attr.pvValue    = pvValue;

    errCode = EI_API_CIP_addClassAttr(pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, &attr);
    if(errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    errCode = EI_API_CIP_setClassAttr(pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, &attr);
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
static inline void DIO_DEVICE_DOP_OutputFault(DIO_DEVICE_DOP_object_t  *pObj, uint8_t updateOutput)
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
static inline void DIO_DEVICE_DOP_OutputIdle(DIO_DEVICE_DOP_object_t  *pObj, uint8_t updateOutput)
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
static void DIO_DEVICE_DOP_ChangeToState(DIO_DEVICE_DOP_object_t  *pObj, DIO_DEVICE_DOP_SmStates_t state)
{
    pObj->old_state = pObj->current_state;
    pObj->current_state = state;
    switch (pObj->current_state)
    {
    case DIO_DEVICE_DOP_SM_NONEXISTENT:
        pObj->processesFnc = DIO_DEVICE_DOP_procNoneExistent;
        break;
    case DIO_DEVICE_DOP_SM_AVAILABLE:
        pObj->processesFnc = DIO_DEVICE_DOP_procAvailable;
        break;
    case DIO_DEVICE_DOP_SM_IDLE:
        pObj->processesFnc = DIO_DEVICE_DOP_procIdle;
        break;
    case DIO_DEVICE_DOP_SM_READY:
        pObj->processesFnc = DIO_DEVICE_DOP_procReady;
        break;
    case  DIO_DEVICE_DOP_SM_RUN:
        pObj->processesFnc = DIO_DEVICE_DOP_procRun;
        break;
    case DIO_DEVICE_DOP_SM_RECOVERABLEFAULT:
        pObj->processesFnc = DIO_DEVICE_DOP_procRecoverableFault;
        break;
    case DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT:
        pObj->processesFnc = DIO_DEVICE_DOP_procUnrecoverableFault;
        break;
    default:
        break;
    }

}

/**
 *
*/
static uint32_t DIO_DEVICE_DOP_setObjValue(uint16_t instanceID, uint16_t attrID, void *pValue, DIO_DEVICE_DOP_dataContext_t context)
{
    int32_t osalRetval;
    DIO_DEVICE_DOP_object_t *pDopObj = NULL;
    uint32_t retVal = EI_API_eERR_CB_NO_ERROR;
    DIO_DEVICE_DOP_events_t event = DIO_DEVICE_DOP_EVENT_NOEVENT;

    if(NULL != pValue)
    {
        osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 5UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            pDopObj = DIO_DEVICE_DOP_findObject(instanceID);
            if(NULL != pDopObj)
            {
                if(DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT != pDopObj->current_state)
                {
                    switch (attrID)
                    {
                    case 3: //!< value attribute
                        if(*(ei_api_cip_edt_usint *)pValue <= 1)
                        {
                            pDopObj->valueContainer.value = *(ei_api_cip_edt_usint *)pValue;
                            pDopObj->valueContainer.context = context;
                            if(DIO_DEVICE_DOP_DATA_IMPLICIT == context)
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
                        if( (DIO_DEVICE_DOP_SM_IDLE == pDopObj->current_state) ||
                            (DIO_DEVICE_DOP_SM_READY == pDopObj->current_state) ||
                            (DIO_DEVICE_DOP_SM_RUN == pDopObj->current_state))
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

                    event = DIO_DEVICE_DOP_getOtherEvents(pDopObj);
                    if((NULL != pDopObj->processesFnc) && (DIO_DEVICE_DOP_EVENT_NOEVENT != event))
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
uint32_t DIO_DEVICE_DOP_getObjValue(uint16_t instanceId, uint16_t attrID, void *pValue)
{
    int32_t osalRetval;
    DIO_DEVICE_DOP_object_t *pDopObj = NULL;
    uint32_t retVal = EI_API_eERR_CB_NO_ERROR;
    if(NULL != pValue)
    {
        osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 2UL);
        if(OSAL_ERR_NoError == osalRetval)
        {
            pDopObj = DIO_DEVICE_DOP_findObject(instanceId);
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
void DIO_DEVICE_DOP_receiveConnectionEvent(uint16_t instanceID, DIO_DEVICE_DOP_ConnectionEvent_t connectionEvent)
{
    int32_t osalRetval;
    DIO_DEVICE_DOP_object_t *pDopObj = NULL;
    DIO_DEVICE_DOP_events_t event = DIO_DEVICE_DOP_EVENT_NOEVENT;
    osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 5UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        pDopObj = DIO_DEVICE_DOP_findObject(instanceID);
        if(NULL != pDopObj)
        {
            if(NULL != pDopObj->processesFnc)
            {
                switch (connectionEvent)
                {
                case DIO_DEVICE_DOP_CONNECTION_DELETED:
                    event = DIO_DEVICE_DOP_EVENT_CONNECTION_DELETED;
                    break;
                case DIO_DEVICE_DOP_CONNECTION_ESTABLISHED:
                    event = DIO_DEVICE_DOP_EVENT_CONNECTION_ESTABLISHED;
                    break;
                case DIO_DEVICE_DOP_CONNECTION_TIMEDOUT:
                    event = DIO_DEVICE_DOP_EVENT_CONNECTION_TIMEDOUT;
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
void DIO_DEVICE_DOP_receiveIdleEvent(uint16_t instanceID)
{
    int32_t osalRetval;
    DIO_DEVICE_DOP_object_t *pDopObj = NULL;

    osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 2UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        pDopObj = DIO_DEVICE_DOP_findObject(instanceID);
        if(NULL != pDopObj)
        {
            if(NULL != pDopObj->processesFnc)
            {
                pDopObj->processesFnc(pDopObj, DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_INVALID_DATA);
            }
        }

        OSAL_unLockNamedMutex(dopContainer_s.mutex);
    }
}

/**
 *
 */
uint32_t DIO_DEVICE_DOP_receiveDataEvent(uint16_t instanceID, ei_api_cip_edt_bool value)
{
    return DIO_DEVICE_DOP_setObjValue(instanceID, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_03, (void *)&value, DIO_DEVICE_DOP_DATA_IMPLICIT);
}

/**
 *
 */
uint32_t DIO_DEVICE_DOP_setCommand(uint16_t instanceID, ei_api_cip_edt_bool idleRunCommand)
{
    return DIO_DEVICE_DOP_setObjValue(instanceID, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_09, (void *)&idleRunCommand, DIO_DEVICE_DOP_DATA_EXPLICIT);
}
/**
 *
 */
uint32_t DIO_DEVICE_DOP_setFaultAction(uint16_t instanceID, ei_api_cip_edt_bool faultAction)
{
    return DIO_DEVICE_DOP_setObjValue(instanceID, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_05, (void *)&faultAction, DIO_DEVICE_DOP_DATA_EXPLICIT);
}
/**
 *
 */
uint32_t DIO_DEVICE_DOP_setFaultValue(uint16_t instanceID, ei_api_cip_edt_bool faultValue)
{
    return DIO_DEVICE_DOP_setObjValue(instanceID, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_06, (void *)&faultValue, DIO_DEVICE_DOP_DATA_EXPLICIT);
}
/**
 *
 */
uint32_t DIO_DEVICE_DOP_setIdleAction(uint16_t instanceID, ei_api_cip_edt_bool idleAction)
{
    return DIO_DEVICE_DOP_setObjValue(instanceID, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_07, (void *)&idleAction, DIO_DEVICE_DOP_DATA_EXPLICIT);
}
/**
 *
 */
uint32_t DIO_DEVICE_DOP_setIdleValue(uint16_t instanceID, ei_api_cip_edt_bool idleValue)
{
    return DIO_DEVICE_DOP_setObjValue(instanceID, DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_08, (void *)&idleValue, DIO_DEVICE_DOP_DATA_EXPLICIT);
}


static uint32_t DIO_DEVICE_DOP_setValueCb(
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

    retVal = DIO_DEVICE_DOP_setObjValue(instanceId, attrId, pvValue, DIO_DEVICE_DOP_DATA_EXPLICIT);

    return retVal;
}

static uint32_t DIO_DEVICE_DOP_getAttrCb(
                              EI_API_CIP_NODE_T* pCipNode,
                              uint16_t classId,
                              uint16_t instanceId,
                              uint16_t attrId,
                              uint16_t* len,
                              void* pvValue)
{
    uint32_t retVal = EI_API_eERR_CB_INVALID_VALUE;
    OSALUNREF_PARM(classId);

    retVal = DIO_DEVICE_DOP_getObjValue(instanceId, attrId, pvValue);
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
 * errCode = DIO_DEVICE_DOP_addInstanceAttribute(
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
static uint32_t DIO_DEVICE_DOP_addInstanceAttribute(
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
    errCode = EI_API_CIP_addInstanceAttr(pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set initial value
    errCode = EI_API_CIP_setInstanceAttr(pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set callback function
    if (attr.get_callback != NULL || attr.set_callback != NULL)
    {
        errCode = EI_API_CIP_setInstanceAttrFunc(pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }
    }

laError:
    return errCode;
}

static uint32_t DIO_DEVICE_DOP_createInstance(DIO_DEVICE_DOP_object_t *pDopObject)
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
    errCode = EI_API_CIP_createInstance(dopContainer_s.pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, pDopObject->instanceID);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add set & get service for instances
    service.code = EI_API_CIP_eSC_SETATTRSINGLE;
    errCode = EI_API_CIP_addInstanceService(dopContainer_s.pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, pDopObject->instanceID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addInstanceService(dopContainer_s.pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, pDopObject->instanceID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 3 Value (required) for instance
    errCode = DIO_DEVICE_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_03,
                                                EI_API_CIP_eEDT_USINT,
                                                EI_API_CIP_eAR_GET_AND_SET,
                                                DIO_DEVICE_DOP_getAttrCb,
                                                DIO_DEVICE_DOP_setValueCb,
                                                sizeof(ei_api_cip_edt_usint),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 5 Fault Action (optional) for instance
    errCode = DIO_DEVICE_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_05,
                                                EI_API_CIP_eEDT_BOOL,
                                                cipAccess,
                                                DIO_DEVICE_DOP_getAttrCb,
                                                ((pDopObject->userCfg.isBindedToGroup) ? (NULL) : (DIO_DEVICE_DOP_setValueCb)),
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 6 Fault Value (optional) for instance
    errCode = DIO_DEVICE_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_06,
                                                EI_API_CIP_eEDT_BOOL,
                                                cipAccess,
                                                DIO_DEVICE_DOP_getAttrCb,
                                                ((pDopObject->userCfg.isBindedToGroup) ? (NULL) : (DIO_DEVICE_DOP_setValueCb)),
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 7 Idle Action (optional) for instance
    errCode = DIO_DEVICE_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_07,
                                                EI_API_CIP_eEDT_BOOL,
                                                cipAccess,
                                                DIO_DEVICE_DOP_getAttrCb,
                                                ((pDopObject->userCfg.isBindedToGroup) ? (NULL) : (DIO_DEVICE_DOP_setValueCb)),
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 8 Idle Value (optional) for instance
    errCode = DIO_DEVICE_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_08,
                                                EI_API_CIP_eEDT_BOOL,
                                                cipAccess,
                                                DIO_DEVICE_DOP_getAttrCb,
                                                ((pDopObject->userCfg.isBindedToGroup) ? (NULL) : (DIO_DEVICE_DOP_setValueCb)),
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 9 Run_Idle_Command (optional) for instance
    errCode = DIO_DEVICE_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                DEVICE_PROFILE_CIP_INSTANCE_ATTRIBUTE_ID_09,
                                                EI_API_CIP_eEDT_BOOL,
                                                EI_API_CIP_eAR_GET_AND_SET,
                                                DIO_DEVICE_DOP_getAttrCb,
                                                DIO_DEVICE_DOP_setValueCb,
                                                sizeof(ei_api_cip_edt_bool),
                                                &instanceValue);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // Add attribute 12 object state (optional) for instance
    errCode = DIO_DEVICE_DOP_addInstanceAttribute(
                                                dopContainer_s.pCipNode,
                                                pDopObject->instanceID,
                                                12,
                                                EI_API_CIP_eEDT_USINT,
                                                EI_API_CIP_eAR_GET,
                                                DIO_DEVICE_DOP_getAttrCb,
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
 * DIO_DEVICE_DOP_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
void DIO_DEVICE_DOP_init(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    EI_API_CIP_SService_t service;

    if(false == dopContainer_s.isClassInitialized)
    {
        errCode = EI_API_CIP_createClass(pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID);

        // Example how to evaluate error codes returned by API functions.
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // set class instance
        OSAL_MEMORY_memset(&service, 0, sizeof(service));
        service.code = EI_API_CIP_eSC_GETATTRSINGLE;
        errCode = EI_API_CIP_addClassService(pCipNode, CFG_PROFILE_DIO_DEVICE_DOP_CLASS_ID, &service);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }
        errCode = DIO_DEVICE_DOP_addClassAttribute(pCipNode, 1, &dopClassData_s.revision);
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
bool DIO_DEVICE_DOP_addObject(uint16_t instanceID, DIO_DEVICE_DOP_ObjectCfg_t *pDopConfig)
{
    DIO_DEVICE_DOP_object_t  *pDopObj = NULL;
    int32_t osalRetval;
    uint32_t errCode;
    bool retval = true;
    if(false == dopContainer_s.isClassInitialized)
    {
        OSAL_printf("%s:%d first call the DIO_DEVICE_DOP_init before adding objects\r\n", __func__, __LINE__);
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
        pDopObj = DIO_DEVICE_DOP_findObject(instanceID);

        if(NULL ==  pDopObj)
        {
            pDopObj = OSAL_MEMORY_calloc(sizeof(DIO_DEVICE_DOP_object_t), 1);
            if(NULL != pDopObj)
            {
                OSAL_MEMORY_memcpy(&pDopObj->userCfg, pDopConfig, sizeof(DIO_DEVICE_DOP_ObjectCfg_t));
                pDopObj->instanceID = instanceID;
                pDopObj->current_state = DIO_DEVICE_DOP_SM_AVAILABLE;
                pDopObj->old_state = DIO_DEVICE_DOP_SM_NONEXISTENT;
                pDopObj->processesFnc = DIO_DEVICE_DOP_procAvailable;
                pDopObj->nextObject = NULL;
                pDopObj->valueContainer.value = 0;
                pDopObj->valueContainer.context = 0;
                pDopObj->run_idle_command = 0;
                pDopObj->faultSettingChanged = false;
                pDopObj->idleSettingChanged = false;
                pDopObj->runIdleValueChanged = false;
                pDopObj->receiveDataEvent = false;
                pDopObj->receiveIdleEvent = false;
                errCode = DIO_DEVICE_DOP_createInstance(pDopObj);
                if (EI_API_CIP_eERR_OK == errCode)
                {
                    DIO_DEVICE_DOP_insertObject(pDopObj);
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
bool DIO_DEVICE_DOP_deleteObject(uint16_t instanceID)
{
    DIO_DEVICE_DOP_object_t *previous  = NULL;
    DIO_DEVICE_DOP_object_t *temp = NULL;
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
 * DIO_DEVICE_DOP_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
void DIO_DEVICE_DOP_run (void)
{
    static volatile EI_API_ADP_SModNetStatus_t networkStatus = {0};
    static volatile DIO_DEVICE_ConnectionState_t connectionStatus = DIO_DEVICE_ConnectionNotEstablished ;
    static volatile DIO_DEVICE_DOP_object_t  *pDopObj = NULL;
    dop_state_proccess_t currentProcess = NULL;
    volatile DIO_DEVICE_DOP_events_t event =  DIO_DEVICE_DOP_EVENT_NOEVENT;
    int32_t osalRetval;

    osalRetval = OSAL_lockNamedMutex(dopContainer_s.mutex, 2UL);
    if(OSAL_ERR_NoError == osalRetval)
    {
        if(NULL != dopContainer_s.head)
        {
            if(NULL == pDopObj)
            {
                pDopObj = (volatile DIO_DEVICE_DOP_object_t  *)dopContainer_s.head;
            }

            currentProcess = pDopObj->processesFnc;
            pDopObj->userCfg.fuGetEvent((DIO_DEVICE_ConnectionState_t *)&connectionStatus, (EI_API_ADP_SModNetStatus_t *)&networkStatus);
            if((EI_API_ADP_eSTATUS_LED_RED_ON == networkStatus.mod) || (EI_API_ADP_eSTATUS_LED_RED_ON == networkStatus.net))
            {
                event = DIO_DEVICE_DOP_EVENT_UNRECOVERABLE_FAULT;
            }

            if(NULL != currentProcess)
            {
                currentProcess((DIO_DEVICE_DOP_object_t  *)pDopObj, event);
            }

            pDopObj = (volatile DIO_DEVICE_DOP_object_t *)pDopObj->nextObject;
        }
        OSAL_unLockNamedMutex(dopContainer_s.mutex);
    }
}

//-------------------------------------------------------------------------------------------
static inline DIO_DEVICE_DOP_events_t DIO_DEVICE_DOP_getOtherEvents(DIO_DEVICE_DOP_object_t *obj)
{
    DIO_DEVICE_DOP_events_t event = DIO_DEVICE_DOP_EVENT_NOEVENT;

    if(obj->runIdleValueChanged)
    {
        obj->runIdleValueChanged = false;
        if(obj->run_idle_command)
        {
            event = DIO_DEVICE_DOP_EVENT_RECEIVE_RUN_COMMAND;
        }
        else
        {
            event = DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_COMMAND;
        }
    }
    else if(obj->receiveDataEvent)
    {
        obj->receiveDataEvent = false;
        event = DIO_DEVICE_DOP_EVENT_RECEIVE_DATA;
    }


    return event;
}
//-------------------------------------------------------------------------------------------
static void DIO_DEVICE_DOP_procNoneExistent(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event)
{
    if(DIO_DEVICE_DOP_EVENT_UNRECOVERABLE_FAULT == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT);
    }
    else
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_AVAILABLE);
    }
}
//------------------------------------------------------------------------------------------
static void DIO_DEVICE_DOP_procAvailable(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event)
{
    if(DIO_DEVICE_DOP_EVENT_UNRECOVERABLE_FAULT == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if((DIO_DEVICE_DOP_EVENT_CONNECTION_ESTABLISHED == event) ||
            (DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_INVALID_DATA == event) ||
            (DIO_DEVICE_DOP_EVENT_RECEIVE_DATA == event))
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_READY);
    }
    // else
    // {
    //     obj->valueContainer.value = 0; //output-OFF
    // }
}
//-------------------------------------------------------------------------------------------
static void DIO_DEVICE_DOP_procIdle(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event)
{
    if(DIO_DEVICE_DOP_EVENT_UNRECOVERABLE_FAULT == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if(DIO_DEVICE_DOP_EVENT_RECEIVE_FAULT == event)
    {
        //output in fault state
        DIO_DEVICE_DOP_OutputFault(obj, 1);
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_RECOVERABLEFAULT);
    }
    else if(DIO_DEVICE_DOP_EVENT_CONNECTION_TIMEDOUT == event)
    {
        //output remain unchanged
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_RECOVERABLEFAULT);
    }
    else if(DIO_DEVICE_DOP_EVENT_CONNECTION_DELETED == event)
    {
        //output Off
        obj->valueContainer.value = 0;
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_AVAILABLE);
    }
    else if(DIO_DEVICE_DOP_EVENT_RECEIVE_RUN_COMMAND == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_READY);
    }
    else if(DIO_DEVICE_DOP_EVENT_RECEIVE_DATA == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_RUN);
    }
    else if(obj->idleSettingChanged)
    {
        obj->idleSettingChanged = false;
        //update Idle-output
        DIO_DEVICE_DOP_OutputIdle(obj, 1);
    }
}
//-------------------------------------------------------------------------------------------
static void DIO_DEVICE_DOP_procReady(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event)
{
    if(DIO_DEVICE_DOP_EVENT_UNRECOVERABLE_FAULT == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if(DIO_DEVICE_DOP_EVENT_RECEIVE_FAULT == event)
    {
        //output in fault state
        DIO_DEVICE_DOP_OutputFault(obj, 1);
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_RECOVERABLEFAULT);
    }
    else if(DIO_DEVICE_DOP_EVENT_CONNECTION_TIMEDOUT == event)
    {
        /**
         * The state diagram in the specification is misleading,
         * in case of Timeout in Ready state, output should take "Fault" value.
         */
        DIO_DEVICE_DOP_OutputFault(obj, 1);
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_RECOVERABLEFAULT);
    }
    else if((DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_COMMAND == event) ||
            (DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_INVALID_DATA == event))
    {
        //output in Idle state
        DIO_DEVICE_DOP_OutputIdle(obj, 1);
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_IDLE);
    }
    else if(DIO_DEVICE_DOP_EVENT_CONNECTION_DELETED == event)
    {
        //output Off
        obj->valueContainer.value = 0;
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_AVAILABLE);
    }
    else if(DIO_DEVICE_DOP_EVENT_RECEIVE_DATA == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_RUN);
    }
}
//---------------------------------------------------------------------------------------------
static void DIO_DEVICE_DOP_procRun(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event)
{
    if(DIO_DEVICE_DOP_EVENT_UNRECOVERABLE_FAULT == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if(DIO_DEVICE_DOP_EVENT_RECEIVE_FAULT == event)
    {
        //output in fault state
        DIO_DEVICE_DOP_OutputFault(obj, 0);
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_RECOVERABLEFAULT);
    }
    else if(DIO_DEVICE_DOP_EVENT_CONNECTION_TIMEDOUT == event)
    {
        //output in fault state
        DIO_DEVICE_DOP_OutputFault(obj, 0);
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_RECOVERABLEFAULT);
    }
    else if((DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_COMMAND == event) ||
            (DIO_DEVICE_DOP_EVENT_RECEIVE_IDLE_INVALID_DATA == event))
    {
        //output in Idle state
        DIO_DEVICE_DOP_OutputIdle(obj, 0);
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_IDLE);
    }
    else if(DIO_DEVICE_DOP_EVENT_CONNECTION_DELETED == event)
    {
        //output Off
        obj->valueContainer.value = 0;
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_AVAILABLE);
    }

    obj->userCfg.fuSetOutput(obj->instanceID, obj->valueContainer.value); //update output
}
//----------------------------------------------------------------------------------------------
static void DIO_DEVICE_DOP_procRecoverableFault(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event)
{
    if(DIO_DEVICE_DOP_EVENT_UNRECOVERABLE_FAULT == event)
    {
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT);
    }
    else if(DIO_DEVICE_DOP_EVENT_CONNECTION_DELETED == event)
    {
        //output Off
        obj->valueContainer.value = 0;
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_AVAILABLE);
    }
    else if(obj->faultSettingChanged)
    {
        obj->faultSettingChanged = false;
        //update fault-output
        DIO_DEVICE_DOP_OutputFault(obj, 1);
    }
    else if((DIO_DEVICE_DOP_EVENT_CONNECTION_ESTABLISHED == event))
    {
        //output unchanged
        DIO_DEVICE_DOP_ChangeToState(obj, DIO_DEVICE_DOP_SM_READY);
    }
}
//----------------------------------------------------------------------------------------------
static void DIO_DEVICE_DOP_procUnrecoverableFault(DIO_DEVICE_DOP_object_t *obj, DIO_DEVICE_DOP_events_t event)
{
    //Nothing to do, there is no coming back!
}
//-----------------------------------------------------------------------------------------------
