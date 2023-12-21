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

#if (!(defined FBTLPROVIDER) || (0 == FBTLPROVIDER)) && (!(defined FBTL_REMOTE) || (0 == FBTL_REMOTE))

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

#if (defined BRIDGING_AND_ROUTING) && (BRIDGING_AND_ROUTING==1)
    // Enable Bridging and Routing feature
    EI_API_ADP_setBridgingAndRoutingSupported(pAdapter);
#endif

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

#if (defined BRIDGING_AND_ROUTING) && (BRIDGING_AND_ROUTING==1)
    EI_APP_GENERIC_DEVICE_cipRoutingSampleSetup(pCipNode);
    EI_APP_GENERIC_DEVICE_cipUnRoutingSampleSetup(pCipNode);
#endif  //(defined BRIDGING_AND_ROUTING) && (BRIDGING_AND_ROUTING==1)

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
    uint16_t i = 0;

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

#if (defined BRIDGING_AND_ROUTING) && (BRIDGING_AND_ROUTING==1)
/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function is triggered when get_attribute_service request is received during explicit messaging.
 *  This function must be shown to the attribute get_callback during setup.
 *
 *  \details
 *
 *
 */
uint32_t EI_APP_GENERIC_DEVICE_CLASS70_GetAttr2Inst1_Link1_cb (EI_API_CIP_NODE_T *pCipNode,
                                                               uint16_t           classId,
                                                               uint16_t           instanceId,
                                                               uint16_t           attrId,
                                                               uint16_t          *pLen,
                                                               void              *pvValue,
                                                               uint16_t           linkAddress)
{
    /*USINT val*/
    *pLen = sizeof(uint8_t);

    //value to be returned as get attr response
    *(uint8_t*)pvValue = 13;

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function is triggered when set_attribute_service request is received during explicit messaging.
 *  This function must be shown to the attribute set_callback during setup.
 *
 *  \details
 *
 *
 */
uint32_t EI_APP_GENERIC_DEVICE_CLASS70_SetAttr2Inst1_Link1_cb (EI_API_CIP_NODE_T *pCipNode,
                                                               uint16_t           classId,
                                                               uint16_t           instanceId,
                                                               uint16_t           attrId,
                                                               uint16_t           len,
                                                               void              *pvValue,
                                                               uint16_t           linkAddress)
{
    uint8_t data=*((uint8_t*)pvValue);

    OSALUNREF_PARM(data);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function is triggered when get_attribute_service request is received during explicit messaging.
 *  This function must be shown to the attribute get_callback during setup.
 *
 *  \details
 *
 *
 */
uint32_t EI_APP_GENERIC_DEVICE_CLASS70_GetAttr2Inst1_cb (EI_API_CIP_NODE_T *pCipNode,
                                                         uint16_t           classId,
                                                         uint16_t           instanceId,
                                                         uint16_t           attrId,
                                                         uint16_t          *pLen,
                                                         void              *pvValue)
{

    /*USINT val*/
    *pLen = sizeof(uint8_t);

    //value to be returned as get attr response
    *(uint8_t*)pvValue = 13;


    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function is triggered when set_attribute_service request is received during explicit messaging.
 *  This function must be shown to the attribute set_callback during setup.
 *
 *  \details
 *
 *
 */
uint32_t EI_APP_GENERIC_DEVICE_CLASS70_SetAttr2Inst1_cb (EI_API_CIP_NODE_T *pCipNode,
                                                         uint16_t           classId,
                                                         uint16_t           instanceId,
                                                         uint16_t           attrId,
                                                         uint16_t           len,
                                                         void              *pvValue)
{
    uint8_t data=*((uint8_t*)pvValue);

    OSALUNREF_PARM(data);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  In order to observe configurationAssemblies data , EI_APP_CFGASSEM_cb was created.
 *
 *  \details
 *
 *
 */
uint32_t EI_APP_GENERIC_DEVICE_CFGASSEM_cb (EI_API_CIP_NODE_T   *pCipNode,
                                            uint16_t             o2t,
                                            uint16_t             t2o,
                                            uint16_t             cfg_inst,
                                            const uint8_t* const pCfgData,
                                            uint16_t             cfgDataSize,
                                            uint16_t             linkaddr)
{
    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Call EI_APP_cipGenerateContentCipRouting function in order to create Assembly and use routable functionality.
 *
 *  \details
 *   A routableInstance has been created by cipGenerateContent. Assemblies attributes are setting in this function.

 */
void EI_APP_GENERIC_DEVICE_cipGenerateContentCipRouting (EI_API_CIP_NODE_T *pCipNode,
                                                         uint16_t           classId,
                                                         uint16_t           instanceId,
                                                         uint8_t            linkAddr)
{
    EI_API_CIP_SService_t service;
    uint16_t i = 0;
    uint16_t attribID = 0x300;
    uint32_t errCode = 0;

    errCode = EI_API_CIP_createRoutableInstance(pCipNode, classId, instanceId, linkAddr);

    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_printf("Failed to create routable instance\n");
    }

    service.getAttrAllResponseCnt = 0;
    service.callback = NULL;

    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addRoutableInstanceService(pCipNode, classId, instanceId, &service, linkAddr);

    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_printf("Failed to add routable instance service\n");
    }

    service.code = EI_API_CIP_eSC_SETATTRSINGLE;
    errCode = EI_API_CIP_addRoutableInstanceService(pCipNode, classId, instanceId, &service, linkAddr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_printf("Failed to add routable instance service\n");
    }

    // 64 USINT (uint8_t).
    for (i = 0; i < 64; i++)
    {
        EI_API_CIP_SAttr_t attr;
        memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_USINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        errCode = EI_API_CIP_addRoutableInstanceAttr(pCipNode, classId, instanceId, &attr, linkAddr);
        if (errCode != EI_API_CIP_eERR_OK)
        {
           OSAL_printf("Failed to add routable instance service\n");
        }
//      errCode = EI_API_CIP_setRoutableInstanceAttrFunc(pCipNode, classId, instanceId, &attr, linkAddr);
//
//      if (errCode != EI_API_CIP_eERR_OK)
//      {
//         OSAL_printf("Failed to set routable instance service\n");
//      }
        attribID++;
    }

    // 32 UINT (uint16_t).
    for (i = 0; i < 32; i++)
    {
        EI_API_CIP_SAttr_t attr;
        memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_UINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addRoutableInstanceAttr(pCipNode, classId, instanceId, &attr, linkAddr);
//      EI_API_CIP_setRoutableInstanceAttrFunc(cipNode_s, classId, instanceId, &attr, linkAddr);

        attribID++;
    }

    // 16 UDINT (uint32_t).
    for (i = 0; i < 16; i++)
    {
        EI_API_CIP_SAttr_t attr;
        memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_UDINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addRoutableInstanceAttr(pCipNode, classId, instanceId, &attr, linkAddr);
//      EI_API_CIP_setRoutableInstanceAttrFunc(cipNode_s, classId, instanceId, &attr, linkAddr);

        attribID++;
    }

    // 8 ULINT (uint64_t).
    for (i = 0; i < 8; i++)
    {
        EI_API_CIP_SAttr_t attr;
        memset(&attr, 0, sizeof(attr));
        attr.id = attribID;
        attr.edt = EI_API_CIP_eEDT_ULINT;
        attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
        attr.pvValue = &i;

        EI_API_CIP_addRoutableInstanceAttr(pCipNode, classId, instanceId, &attr, linkAddr);
//      EI_API_CIP_setRoutableInstanceAttrFunc(cipNode_s, classId, instanceId, &attr, linkAddr);

        attribID++;
    }
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Initialize the application with instance's, attribute's, service's to be able provide unrouting functionality.
 *  This example shows how to set explicit messaging us.
 *
 *  \details
 *  In the SampleSetup application with instances,attributes callback.
 *  In EI_APP_cipUnRoutingSampleSetup, we must first create an Instance and the Instance services we need.
 *  If you need to Class Instace, You should call EI_API_CIP_addClassAttr() and EI_API_CIP_setClassAttrFunc() as well.
 */
static bool EI_APP_GENERIC_DEVICE_cipUnRoutingSampleSetup (EI_API_CIP_NODE_T *pCipNode)
{
    uint32_t                  errCode = 0;
        uint16_t              classId = 0x70;
        uint16_t              instanceId = 6;


        EI_API_CIP_SAttr_t    attr = {0};
        const uint16_t        attr2_val = 2;
        EI_API_CIP_SService_t service = {0};

         /*creating routable instance 1 which is connected to link addr 1 in the 0x70 class*/
         errCode = EI_API_CIP_createInstance(pCipNode, classId, instanceId);

         if (errCode != EI_API_CIP_eERR_OK) {
             OSAL_printf("Failed to create Routable Instance\n");
         }

         // set class instance
         service.code = EI_API_CIP_eSC_GETATTRSINGLE;
         errCode = EI_API_CIP_addInstanceService(pCipNode, classId, instanceId, &service);

         if (errCode != EI_API_CIP_eERR_OK) {
             OSAL_printf("Failed to create Routable Instance\n");
         }

         service.code = EI_API_CIP_eSC_SETATTRSINGLE;
         errCode = EI_API_CIP_addInstanceService(pCipNode, classId, instanceId, &service);

         if (errCode != EI_API_CIP_eERR_OK) {
             OSAL_printf("Failed to create Routable Instance\n");
         }


         attr.id = 2;
         attr.edt = EI_API_CIP_eEDT_USINT;
         attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
         attr.get_callback = EI_APP_GENERIC_DEVICE_CLASS70_GetAttr2Inst1_cb;
         attr.set_callback = EI_APP_GENERIC_DEVICE_CLASS70_SetAttr2Inst1_cb;
         attr.pvValue = (void *)&attr2_val;

         errCode = EI_API_CIP_addInstanceAttr(pCipNode, classId, instanceId, &attr);
         if (errCode != EI_API_CIP_eERR_OK) {
             OSAL_printf("Failed to add Routable Instance Attr\n");
         }
         errCode = EI_API_CIP_setInstanceAttrFunc(pCipNode, classId, instanceId, &attr);
         if (errCode != EI_API_CIP_eERR_OK) {
             OSAL_printf("Failed to set Routable Instance attr func\n");
         }

         //Adding class attribute and function
         EI_API_CIP_addClassAttr        (pCipNode, classId, &attr);
         EI_API_CIP_setClassAttrFunc    (pCipNode, classId, &attr);

        return true;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Setup the application with routableinstances, routable attributes callback and routableassemblies..
 *
 *  \details
 *  In the SampleSetup application with routableinstances, routable attributes callback, and routableassemblies.
 *  In the EI_APP_cipRoutingSampleSetup, first of all,setroutinglinkadress function should be called. after setting routable ports.
 *  routable Instance or assembly Instance can be created. Don't forget it, Rotable side is using the routable callback function no longer.
 */
static bool EI_APP_GENERIC_DEVICE_cipRoutingSampleSetup (EI_API_CIP_NODE_T *pCipNode)
{
    uint32_t              errCode = 0;
    uint16_t              classId = 0x10D;
    uint16_t              instanceId = 1;
    uint8_t               linkAddress = 2;
    EI_API_CIP_SAttr_t    attr = {0};
    const uint16_t        attr2_val = 2;
    EI_API_CIP_SService_t service = {0};
    const uint8_t         maxlinkaddr = 8;
    uint8_t               linkaddr = 0;

    //Cip routing functions
     errCode = EI_API_CIP_setRoutingMaxLinkAddr(pCipNode, maxlinkaddr);
     if (errCode != EI_API_CIP_eERR_OK) {
                OSAL_printf("Failed to set Max link addr\n");
            }

     errCode= EI_API_CIP_getRoutingMaxLinkAddr(pCipNode, &linkaddr);
     if ((errCode != EI_API_CIP_eERR_OK) || (linkaddr!= maxlinkaddr)) {
                   OSAL_printf("Failed to get Max link addr\n");
               }

     const uint8_t routablePorts[] = {
             11,7,3
     };

     errCode = EI_API_CIP_setRoutingRoutablePorts(pCipNode, routablePorts, sizeof(routablePorts)/sizeof(uint8_t));

     if (errCode != EI_API_CIP_eERR_OK) {
                  OSAL_printf("Failed to set Routable Ports\n");
              }

    // Create instance of 0x70 CIP Routing Cylic IO messaging.
     EI_APP_GENERIC_DEVICE_cipGenerateContentCipRouting(pCipNode, classId, instanceId, linkAddress);

    errCode = EI_API_CIP_createCfgAssembly(pCipNode);//CFG ASSEMBLY

    errCode = EI_API_CIP_createRoutingAssembly(pCipNode, 0xfe, EI_API_CIP_eAR_GET, linkAddress); // Input-only. (O2T)
    errCode = EI_API_CIP_createRoutingAssembly(pCipNode, 0xff, EI_API_CIP_eAR_GET, linkAddress); // Listen-only.

    errCode = EI_API_CIP_createRoutingAssembly(pCipNode, 3, EI_API_CIP_eAR_GET_AND_SET, linkAddress);//T2O
    errCode = EI_API_CIP_createRoutingAssembly(pCipNode, 1, EI_API_CIP_eAR_GET_AND_SET, linkAddress);//O2T

    for (uint16_t i = 0x300; i < 0x305; i++)
    {
        errCode = EI_API_CIP_addRoutingAssemblyMember(pCipNode, 3, classId, instanceId, i, linkAddress);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x64:  Error code: 0x%08x\n", classId, instanceId, (uint16_t)i, errCode);
        }

        errCode = EI_API_CIP_addRoutingAssemblyMember(pCipNode, 1, classId, instanceId, (uint16_t)(8 + i), linkAddress);
        if (errCode != EI_API_CIP_eERR_OK) {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x65:  Error code: 0x%08x\n", classId, instanceId, (uint16_t)(8 + i), errCode);
        }
    }

     instanceId = 6;
     /*creating routable instance 1 which is connected to link addr 1 in the 0x70 class*/
     errCode = EI_API_CIP_createRoutableInstance(pCipNode, classId, instanceId, linkAddress);

     if (errCode != EI_API_CIP_eERR_OK) {
         OSAL_printf("Failed to create Routable Instance\n");
     }

     // set class instance
     service.code = EI_API_CIP_eSC_GETATTRSINGLE;
     errCode = EI_API_CIP_addRoutableInstanceService(pCipNode, classId, instanceId, &service, linkAddress);

     if (errCode != EI_API_CIP_eERR_OK) {
         OSAL_printf("Failed to create Routable Instance\n");
     }

     service.code = EI_API_CIP_eSC_SETATTRSINGLE;
     errCode = EI_API_CIP_addRoutableInstanceService(pCipNode, classId, instanceId, &service, linkAddress);

     if (errCode != EI_API_CIP_eERR_OK) {
         OSAL_printf("Failed to create Routable Instance\n");
     }


     attr.id = 2;
     attr.edt = EI_API_CIP_eEDT_USINT;
     attr.accessRule = EI_API_CIP_eAR_GET_AND_SET;
     attr.get_callback_routed = EI_APP_GENERIC_DEVICE_CLASS70_GetAttr2Inst1_Link1_cb;
     attr.set_callback_routed = EI_APP_GENERIC_DEVICE_CLASS70_SetAttr2Inst1_Link1_cb;
     attr.pvValue = (void *)&attr2_val;

     errCode = EI_API_CIP_addRoutableInstanceAttr(pCipNode, classId, instanceId, &attr, linkAddress);
     if (errCode != EI_API_CIP_eERR_OK) {
         OSAL_printf("Failed to add Routable Instance Attr\n");
     }
     errCode = EI_API_CIP_setRoutableInstanceAttrFunc(pCipNode, classId, instanceId, &attr, linkAddress);
     if (errCode != EI_API_CIP_eERR_OK) {
         OSAL_printf("Failed to set Routable Instance attr func\n");
     }

     //Adding class attribute and function
     EI_API_CIP_addClassAttr        (pCipNode, classId, &attr);
     EI_API_CIP_setClassAttrFunc    (pCipNode, classId, &attr);

    return true;
}
#endif  // (defined BRIDGING_AND_ROUTING) && (BRIDGING_AND_ROUTING==1)

#endif  // (!(defined FBTLPROVIDER) || (0 == FBTLPROVIDER)) && (!(defined FBTL_REMOTE) || (0 == FBTL_REMOTE))

//*************************************************************************************************
