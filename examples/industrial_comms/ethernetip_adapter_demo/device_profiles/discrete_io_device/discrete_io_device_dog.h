/*!
 *  \file discrete_io_device_dog.h
 *
 *  \brief
 *  Necessary declarations of Discrete Output Group (DOG) Object.
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

#ifndef DIO_DEVICE_DOG_H
#define DIO_DEVICE_DOG_H

/**
 * @brief
 * A structure to hold Discrete Output Group's (DOG)
 * class data.
 */
typedef struct DIO_DEVICE_DOG_ClassData
{
    uint16_t revision;
}DIO_DEVICE_DOG_ClassData_t;

typedef struct DIO_DEVICE_DOG_ObjectCfg
{
    uint16_t  *dop_instance_binding_list;
    uint16_t  list_len;
}DIO_DEVICE_DOG_ObjectCfg_t;

void DIO_DEVICE_DOG_init (EI_API_CIP_NODE_T* pCipNode);
/**
 * \brief Create and add a DOG instance object
 * \param[in] InstanceID the instance-ID of the DOG object to be created
 * \param[in] pConfig configuration for this instance object
 * \return true if successful, otherwise false
*/
bool DIO_DEVICE_DOG_addObject(uint16_t instanceID, DIO_DEVICE_DOG_ObjectCfg_t *pConfig);
/**
 * \brief deletes the DOG object(if exist)
 * \param[in] instanceID the instance-ID of the object to be removed
 * \return  true if successful, false if the object does not exist
*/
bool DIO_DEVICE_DOG_deleteObject(uint16_t instanceID);

/**
 * \brief Sets the given falut-action and idle-action when configuration assembly is received.
 * \param[in] dog_instanceID the DOG instance-ID
 * \param[in] faultAction fault-action from configuration assembly
 * \param[in] idleAction idle-action from configuration assembly
 */
uint32_t DIO_DEVICE_DOG_receiveAssemblyConfig(uint16_t dog_instanceID, ei_api_cip_edt_bool faultAction, ei_api_cip_edt_bool idleAction);


#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __cplusplus
}
#endif

#endif // DIO_DEVICE_DOG_H
