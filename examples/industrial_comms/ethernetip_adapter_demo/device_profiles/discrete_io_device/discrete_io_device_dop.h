/*!
 *  \file discrete_io_device_dop.h
 *
 *  \brief
 *  Necessary declarations of Discrete Output Point (DOP) Object.
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

#ifndef DIO_DEVICE_DOP_H
#define DIO_DEVICE_DOP_H

#include <stdbool.h>

#include "EI_API.h"
#include "EI_API_def.h"

#define DIO_DEVICE_DOP_LED_ON  0x01 /*!< LED ON Status for DIP & DOP Objects */
#define DIO_DEVICE_DOP_LED_OFF 0x00 /*!< LED OFF Status for DIP & DOP Objects */

/**
 * @brief
 * A structure to hold Discrete Output Point's (DOP)
 * class data.
 */
typedef struct DIO_DEVICE_DOP_ClassData
{
    uint16_t revision;
}DIO_DEVICE_DOP_ClassData_t;

/**
 * @brief
 * EI_APP_DISCRETE_IO_DEVICE_DOP_SmStates_t enum is used for
 * states of DOP state machine.
 */
typedef enum DIO_DEVICE_DOP_SmStates
{
    DIO_DEVICE_DOP_SM_NONEXISTENT = 1,
    DIO_DEVICE_DOP_SM_AVAILABLE,
    DIO_DEVICE_DOP_SM_IDLE,
    DIO_DEVICE_DOP_SM_READY,
    DIO_DEVICE_DOP_SM_RUN,
    DIO_DEVICE_DOP_SM_RECOVERABLEFAULT,
    DIO_DEVICE_DOP_SM_UNRECOVERABLEFAULT,
    DIO_DEVICE_DOP_SM_RESERVED = 255
}DIO_DEVICE_DOP_SmStates_t;

typedef enum DIO_DEVICE_DOP_ConnectionEvent
{
    DIO_DEVICE_DOP_CONNECTION_DELETED = 0,
    DIO_DEVICE_DOP_CONNECTION_ESTABLISHED,
    DIO_DEVICE_DOP_CONNECTION_TIMEDOUT,
}DIO_DEVICE_DOP_ConnectionEvent_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*DIO_DEVICE_DOP_setOutputValue)(uint16_t instanceID, ei_api_cip_edt_bool value);

typedef struct DIO_DEVICE_DOP_ObjectCfg
{
    DIO_DEVICE_DOP_setOutputValue       fuSetOutput;
    DIO_DEVICE_getConnectionEventInfo   fuGetEvent;
    volatile ei_api_cip_edt_bool        FaultAction;
    volatile ei_api_cip_edt_bool        FaultValue;
    volatile ei_api_cip_edt_bool        IdleAction;
    volatile ei_api_cip_edt_bool        IdleValue;
    bool     isBindedToGroup;
}DIO_DEVICE_DOP_ObjectCfg_t;

/**
 * \brief Initializes DOP Class and its initial state
 * \param[in]  pCipNode CIP Node pointer
*/
void DIO_DEVICE_DOP_init (EI_API_CIP_NODE_T* pCipNode);
/**
 * \brief DOP processing funcion.
*/
void DIO_DEVICE_DOP_run  (void);
/**
 * \brief Create and add a DOP instance object
 * \param[in] InstanceID the instance-ID of the DOP object to be created
 * \param[in] pDopConfig configuration for this instance object
 * \return true if successful, otherwise false
*/
bool DIO_DEVICE_DOP_addObject(uint16_t instanceID, DIO_DEVICE_DOP_ObjectCfg_t *pDopConfig);
/**
 * \brief deletes the DOP object(if exist)
 * \param[in] instanceID the instance-ID of the object to be removed
 * \return  true if successful, false if the object does not exist
*/
bool DIO_DEVICE_DOP_deleteObject(uint16_t instanceID);

/**
 * \brief sets Idle-Run command for given instance
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] idleRunCommand Idle-Run command
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
uint32_t DIO_DEVICE_DOP_setCommand(uint16_t instanceID, ei_api_cip_edt_bool idleRunCommand);

/**
 * \brief sets fault-action for given instance
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] faultAction fault-action
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
uint32_t DIO_DEVICE_DOP_setFaultAction(uint16_t instanceID, ei_api_cip_edt_bool faultAction);

/**
 * \brief sets fault-value for given instance
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] faultValue fault-value
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
uint32_t DIO_DEVICE_DOP_setFaultValue(uint16_t instanceID, ei_api_cip_edt_bool faultValue);

/**
 * \brief sets idle-action for given instance
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] idleAction idle-action
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
uint32_t DIO_DEVICE_DOP_setIdleAction(uint16_t instanceID, ei_api_cip_edt_bool idleAction);

/**
 * \brief sets idle-value for given instance
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] idleValue idle-value
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
uint32_t DIO_DEVICE_DOP_setIdleValue(uint16_t instanceID, ei_api_cip_edt_bool idleValue);

/**
 * \brief reads the requested attribute from DOP object
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] attrID attribute-ID of the DOP object
 * \param[out] pvalue DOP requested attribute will be written here
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
uint32_t DIO_DEVICE_DOP_getObjValue(uint16_t instanceId, uint16_t attrID, void *pValue);

/**
 * \brief should be called to pass I/O data to the DOP object
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] value DOP value
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
uint32_t DIO_DEVICE_DOP_receiveDataEvent(uint16_t instanceID, ei_api_cip_edt_bool value);

/**
 * \brief should be called when Idle-Event from I/O connection is received
 * (Either through 32bit-run/idle header or zero lenght data to indicate Idle event)
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] idleRunCommand Idle-Run command
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
void DIO_DEVICE_DOP_receiveIdleEvent(uint16_t instanceID);

/**
 * \brief should be called in case of connection event
 * \param[in] instanceID instance-ID of the DOP object
 * \param[in] connectionEvent connection-event type
 * \return EI_API_eERR_CB_NO_ERROR if successful, otherwise error code
 */
void DIO_DEVICE_DOP_receiveConnectionEvent(uint16_t instanceID, DIO_DEVICE_DOP_ConnectionEvent_t connectionEvent);

#ifdef  __cplusplus
}
#endif

#endif // DIO_DEVICE_DOP_H
