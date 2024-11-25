/*!
 *  \file app_discrete_io_device_dip.h
 *
 *  \brief
 *  Necessary declarations of Discrete Input Point (DIP) Object.
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

#ifndef APP_DISCRETE_IO_DEVICE_DIP_H
#define APP_DISCRETE_IO_DEVICE_DIP_H

/**
 * @brief
 * Revision of current implementation of Discrete Input Point Object
*/
#define EI_APP_DIP_CLASS_REVISION 2
/**
 * @brief
 * A structure to hold Discrete Input Point's (DIP)
 * class data.
 */
typedef struct EI_APP_DIP_ClassData
{
    uint16_t revision;
}EI_APP_DIP_ClassData_t;


#ifdef __cplusplus
extern "C" {
#endif

#include "device_profiles/discrete_io_device/app_discrete_io_device.h"

typedef enum EI_APP_DIP_OBJECT_STATE
{
    EI_APP_DIP_OBJ_NON_EXISTENT         =   0,
    EI_APP_DIP_OBJ_AVAILABLE            =   1,
    EI_APP_DIP_OBJ_RUN                  =   2,
    EI_APP_DIP_OBJ_UNRECOVERABLE_FAULT  =   3,
    EI_APP_DIP_OBJ_RECOVERABLE_FAULT    =   4,
}EI_APP_DIP_OBJECT_STATE_t;

/**
 * \brief function pointer prototype for reading input value
 * \param[in] instanceID instance-ID of the object
 * \param[out] pValue pointer to the value
*/
typedef void  (*dip_get_input_value)    (uint16_t  instanceID, uint8_t *pValue);
/**
 * \brief function pointer prototype for reading input status
 * \param[in] instanceID instance-ID of the object
 * \param[out] pStatus pointer to the status
*/
typedef void  (*dip_get_input_status)   (uint16_t  instanceID, uint8_t  *pStatus);

typedef struct EI_DIP_OBJECT_Cfg
{
    dip_get_input_value             fuGetInput;
    dip_get_input_status            fuGetStatus;
    dio_get_connection_event_info   fuGetEvent;
}EI_DIP_OBJECT_Cfg_t;

/**
 * \brief Initializes DIP Class and its initial state
 * \param[in]  pCipNode CIP Node pointer
*/
void EI_APP_DIP_init (EI_API_CIP_NODE_T* pCipNode);
/**
 * \brief DIP processing funcion.
*/
void EI_APP_DIP_run  (void);
/**
 * \brief Create and add a DIP instance object
 * \param[in] InstanceID the instance-ID of the DIP object to be created
 * \param[in] pDipConfig configuration for this instance object
 * \return true if successful, otherwise false
*/
bool EI_APP_DIP_addObject(uint16_t instanceID, EI_DIP_OBJECT_Cfg_t *pDipConfig);
/**
 * \brief deletes the DIP object(if exist)
 * \param[in] instanceID the instance-ID of the object to be removed
 * \return  true if successful, false if the object does not exist
*/
bool EI_APP_DIP_deleteObject(uint16_t instanceID);
/**
 * \brief reads the value of the requested Attribute from the Object
 * \param[in]  InstanceID the instance-ID of the DIP object
 * \param[in]  attrID the AttributeID to be readed out
 * \param[out] pValue pointer to save the result
 * \return     #EI_API_CIP_EError_t as uint32_t.
*/
uint32_t EI_APP_DIP_getObjValue(uint16_t instanceID, uint16_t attrID, void *pValue);
#ifdef  __cplusplus
}
#endif

#endif // APP_DISCRETE_IO_DEVICE_DIP_H
