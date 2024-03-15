/*!
 *  \file app_generic_device.c
 *
 *  \brief
 *  Generic device profile declarations.
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

#include "osal.h"
#include "osal_error.h"

#include "drivers/CUST_drivers.h"

#include "EI_API.h"
#include "EI_API_def.h"

#include "appCfg.h"
#include <device_profiles/app_device_profile.h>
#include <device_profiles/app_device_profile_intern.h>
#include <device_profiles/generic_device/app_generic_device_cfg.h>
#include <device_profiles/generic_device/app_generic_device.h>

static uint16_t EI_APP_GENERIC_DEVICE_extendedStatus_s[255] = {0};

static bool EI_APP_GENERIC_DEVICE_init               (EI_API_ADP_T      *pAdapter, EI_API_CIP_NODE_T *pCipNode);
static void EI_APP_GENERIC_DEVICE_run                (EI_API_CIP_NODE_T *pCipNode);
static bool EI_APP_GENERIC_DEVICE_cipSetup           (EI_API_CIP_NODE_T *pCipNode);
static void EI_APP_GENERIC_DEVICE_cipGenerateContent (EI_API_CIP_NODE_T *pCipNode, uint16_t classId, uint16_t instanceId);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function for ForwardOpen, LargeForwardOpen and ForwardClose.
 *
 *  \details
 *  Callback function which is called when a request for the services
 *  ForwardOpen, LargeForwardOpen and ForwardClose was received.
 *
 */
EI_API_ADP_SEipStatus_t EI_APP_GENERIC_DEVICE_cmgrCb(uint32_t serviceCode, EI_API_ADP_UCmgrInfo_u cmgrInfo)
{
    EI_API_ADP_SEipStatus_t ret_val= {.gen_status=0,
                                      .extended_status_size=0,
                                      .extended_status_arr=EI_APP_GENERIC_DEVICE_extendedStatus_s};


    switch(serviceCode)
        {
        case 0x54:
            // OSAL_printf("Forward open Connection Serial Number: 0x%04x\r\n", cmgrInfo.forwardOpenInfo.conSerialNum);
            break;
        case 0x5b:
            // OSAL_printf("Large forward open Connection Serial Number: 0x%04x\r\n", cmgrInfo.forwardOpenInfo.conSerialNum);
            break;
        case 0x4e:
            // OSAL_printf("Forward close Connection Serial Number: 0x%04x\r\n", cmgrInfo.forwardCloseInfo.conSerialNum);
            break;
        default:
            OSAL_printf("unknown service code %x\r\n", serviceCode);
        }
    return ret_val;
}

/*!
 * \brief
 * Registers generic device profile interface functions.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_init function of device profile interface.
 * Function needs to be defined in each profile.
 *
 * \remark
 * In time of build process only one device profile can be active.
 * Rest profiles need to be excluded from the build.
 *
 * \param[in]     pIntf    Pointer to device profile interface which needs to be initialized.
 *
 * \return        value as bool.
 * \retval        true     Success.
 * \retval        false    Invalid input parameter defined as NULL.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/generic_device/app_generic_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_init (void)
 * {
 *     return EI_APP_DEVICE_PROFILE_register(&EI_APP_DEVICE_PROFILE_intf_s);
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_init
 *
 * \ingroup EI_APP_DEVICE_PROFILE_CFG
 *
 */
uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t *pIntf)
{
    uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;

    if (NULL == pIntf)
    {
        error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
        goto laError;
    }

    pIntf->task.init = EI_APP_GENERIC_DEVICE_init;
    pIntf->task.run  = EI_APP_GENERIC_DEVICE_run;

    pIntf->cfg.init                  = EI_APP_GENERIC_DEVICE_CFG_init;
    pIntf->cfg.isValid               = EI_APP_GENERIC_DEVICE_CFG_isValid;
    pIntf->cfg.setHeader             = EI_APP_GENERIC_DEVICE_CFG_setHeader;
    pIntf->cfg.apply                 = EI_APP_GENERIC_DEVICE_CFG_apply;
    pIntf->cfg.setDefaultWithoutComm = EI_APP_GENERIC_DEVICE_CFG_setDefaultWithoutComm;
    pIntf->cfg.callback              = EI_APP_GENERIC_DEVICE_CFG_callback;
    pIntf->cfg.getRuntimeData        = EI_APP_GENERIC_DEVICE_CFG_getRuntimeData;
    pIntf->cfg.getFactoryResetData   = EI_APP_GENERIC_DEVICE_CFG_getFactoryResetData;
    pIntf->cfg.getLength             = EI_APP_GENERIC_DEVICE_CFG_getLength;

    error = EI_APP_DEVICE_PROFILE_ERR_OK;

laError:
    return error;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Basic initialization function.
 *
 *  \details
 *  Creates a new EtherNet/IP&trade; Adapter.<br>
 *  Initializes data structures from non-volatile storage.<br>
 *  Registers stack error handler.<br>
 *  Initializes the Adapter.<br>
 *  Create a CIP&trade; node.<br>
 *
 */
bool EI_APP_GENERIC_DEVICE_init(EI_API_ADP_T* pAdapter, EI_API_CIP_NODE_T *pCipNode)
{
    bool result = false;

    // Create vendor specific classes.
    result = EI_APP_GENERIC_DEVICE_cipSetup(pCipNode);

    return result;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Cyclically called run function.
 *
 *  \details
 *  Cyclically called run function, handles the EtherNet/IP stack and hardware
 *  specific functionality, such as reading switches, reading inputs, setting outputs
 *  and LEDs.
 *
 */
void EI_APP_GENERIC_DEVICE_run(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode   = EI_API_CIP_eERR_OK;
    uint8_t  attrValue = 0;
    uint16_t attr;

    // Mirror I/O data
    for(attr = 0; attr < 5; attr++)
    {
        errCode = EI_API_CIP_getAttr_usint(pCipNode, 0x0070, 0x0001, attr + 0x0308, &attrValue);

        if (attr == 0 && errCode == EI_API_CIP_eERR_OK)
        {
            CUST_DRIVERS_LED_setIndustrialLeds(attrValue);
        }

        EI_API_CIP_setAttr_usint(pCipNode, 0x0070, 0x0001, attr + 0x0300, attrValue);
    }

    //EI_APP_CLASS71_run();
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Setup the application with classes, instances, attributes, and assemblies.
 *
 *  \details
 *  Setup the application with classes, instances, attributes, and assemblies.<br>
 *  For the assemblies, use instances in the Vendor Specific range of IDs.
 *
 */
static bool EI_APP_GENERIC_DEVICE_cipSetup(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode = 0;
    uint16_t i = 0;

    uint16_t classId = 0x70;
    uint16_t instanceId = 0x01;

    // Create new class 0x70 with read and write service and several attributes.
    EI_APP_GENERIC_DEVICE_cipGenerateContent(pCipNode, classId, instanceId);

    errCode = EI_API_CIP_createAssembly(pCipNode, 0xfe, EI_API_CIP_eAR_GET); // Input-only.
    errCode = EI_API_CIP_createAssembly(pCipNode, 0xff, EI_API_CIP_eAR_GET); // Listen-only.

    errCode = EI_API_CIP_createAssembly(pCipNode, 0x64, EI_API_CIP_eAR_GET_AND_SET);
    errCode = EI_API_CIP_createAssembly(pCipNode, 0x65, EI_API_CIP_eAR_GET_AND_SET);

    for (i = 0x300; i < 0x305; i++)
    {
        errCode = EI_API_CIP_addAssemblyMember(pCipNode, 0x64, classId, instanceId, i);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x64:  Error code: 0x%08x\n", classId, instanceId, (uint16_t)i, errCode);
        }

        errCode = EI_API_CIP_addAssemblyMember(pCipNode, 0x65, classId, instanceId, (uint16_t)(8 + i));
        if (errCode != EI_API_CIP_eERR_OK) {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x65:  Error code: 0x%08x\n", classId, instanceId, (uint16_t)(8 + i), errCode);
        }

    }
    return true;
}

/*!
*  <!-- Description: -->
*
*  \brief
*  Generates attributes and services for a CIP&trade;class.
*
*  \details
*  Create a CIP class with a Class IDs using the value specified in parameter classId.<br>
*  Generates attributes and services for that class.<br>
*  Adds read and write services.<br>
*  Adds 64 8-bit attributes with callback function.<br>
*  Adds 32 16-bit attributes.<br>
*  Adds 16 32-bit attributes.<br>
*  Adds 8 64-bit attributes.
*
*/
static void EI_APP_GENERIC_DEVICE_cipGenerateContent(EI_API_CIP_NODE_T* cipNode, uint16_t classId, uint16_t instanceId)
{
    EI_API_CIP_SService_t service;
    uint64_t i = 0;

    EI_API_CIP_createClass(cipNode, classId);

    service.getAttrAllResponseCnt = 0;
    service.callback = NULL;
    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    EI_API_CIP_addClassService(cipNode, classId, &service);
    service.code = EI_API_CIP_eSC_SETATTRSINGLE;
    EI_API_CIP_addClassService(cipNode, classId, &service);

    EI_API_CIP_createInstance(cipNode, classId, instanceId);

    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    EI_API_CIP_addInstanceService(cipNode, classId, instanceId, &service);
    service.code = EI_API_CIP_eSC_SETATTRSINGLE;
    EI_API_CIP_addInstanceService(cipNode, classId, instanceId, &service);

    uint16_t attribID = 0x300;

    // 64 USINT (uint8_t).
    for (i = 0; i < 64; i++)
    {
        EI_API_CIP_SAttr_t attr;
        OSAL_MEMORY_memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_USINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addInstanceAttr(cipNode, classId, instanceId, &attr);
        EI_API_CIP_setInstanceAttr(cipNode, classId, instanceId, &attr);

        attribID++;
    }

    // 32 UINT (uint16_t).
    for (i = 0; i < 32; i++)
    {
        EI_API_CIP_SAttr_t attr;
        OSAL_MEMORY_memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_UINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addInstanceAttr(cipNode, classId, instanceId, &attr);
        EI_API_CIP_setInstanceAttr(cipNode, classId, instanceId, &attr);

        attribID++;
    }

    // 16 UDINT (uint32_t).
    for (i = 0; i < 16; i++)
    {
        EI_API_CIP_SAttr_t attr;
        OSAL_MEMORY_memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_UDINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addInstanceAttr(cipNode, classId, instanceId, &attr);
        EI_API_CIP_setInstanceAttr(cipNode, classId, instanceId, &attr);

        attribID++;
    }

    // 8 ULINT (uint64_t).
    for (i = 0; i < 8; i++)
    {
        EI_API_CIP_SAttr_t attr;
        OSAL_MEMORY_memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_ULINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addInstanceAttr(cipNode, classId, instanceId, &attr);
        EI_API_CIP_setInstanceAttr(cipNode, classId, instanceId, &attr);

        attribID++;
    }
}

//*************************************************************************************************
