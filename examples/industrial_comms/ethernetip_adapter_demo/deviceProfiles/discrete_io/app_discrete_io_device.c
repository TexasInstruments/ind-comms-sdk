/*!
 *  \file app_discrete_io_device.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application for Device Profile: General Purpose of Discrete IO.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
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

#include <api/EI_API.h>
#include <api/EI_API_def.h>

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
#include "deviceProfiles/app_device_profile.h"
#include "deviceProfiles/app_device_profile_intern.h"
#include "deviceProfiles/discrete_io/app_discrete_io_device_cfg.h"
#include "deviceProfiles/discrete_io/app_discrete_io_device.h"

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

extern PRUICSS_Handle prusshandle;

// Static variables and pointers used in this example.
//static void EI_APP_DISCRETE_IO_DEVICE_adpInit  (EI_API_ADP_T* pAdapter);

static bool EI_APP_DISCRETE_IO_DEVICE_init     (EI_API_ADP_T      *pAdapter, EI_API_CIP_NODE_T *pCipNode);
static void EI_APP_DISCRETE_IO_DEVICE_run      (EI_API_CIP_NODE_T *pCipNode);

static bool EI_APP_DISCRETE_IO_DEVICE_cipSetup (EI_API_CIP_NODE_T *pCipNode);
static void EI_APP_DISCRETE_IO_DEVICE_DIP_init (EI_API_CIP_NODE_T* pCipNode);
static void EI_APP_DISCRETE_IO_DEVICE_DIP_run  (EI_API_CIP_NODE_T* pCipNode);
static void EI_APP_DISCRETE_IO_DEVICE_DOP_init (EI_API_CIP_NODE_T* pCipNode);
static void EI_APP_DISCRETE_IO_DEVICE_DOP_run  (EI_API_CIP_NODE_T* pCipNode);

/* Discrete Input Point & Discrete Output Point Object definitions */
static uint8_t EI_APP_DISCRETE_IO_DEVICE_dopStatus_s[EI_APP_DISCRETE_IO_DOP_NUM_OF_INST]; /**< Set DOP instances for Configuration Assembly */
static EI_APP_DISCRETE_IO_DEVICE_DOP_classData_t dopClassData_s = {.revision = EI_APP_DISCRETE_IO_DEVICE_DOP_REVISION_NUMBER
                                                                  };
static EI_APP_DISCRETE_IO_DEVICE_DIP_classData_t dipClassData_s = {.revision = EI_APP_DISCRETE_IO_DEVICE_DIP_REVISION_NUMBER
                                                                  };
static uint32_t EI_APP_DISCRETE_IO_DEVICE_ledStatus_s  = 0;

/*!
 * \brief
 * Registers discrete IO device profile interface functions.
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
 * #include "deviceProfiles/discreteIoDevice/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_init (void)
 * {
 *     return EI_APP_DEVICE_PROFILE_register(&EI_APP_DEVICE_PROFILE_intf_s);
 * }
 *
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_init
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
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

    pIntf->task.init = EI_APP_DISCRETE_IO_DEVICE_init;
    pIntf->task.run  = EI_APP_DISCRETE_IO_DEVICE_run;

    pIntf->cfg.init                  = EI_APP_DISCRETE_IO_DEVICE_CFG_init;
    pIntf->cfg.isValid               = EI_APP_DISCRETE_IO_DEVICE_CFG_isValid;
    pIntf->cfg.setHeader             = EI_APP_DISCRETE_IO_DEVICE_CFG_setHeader;
    pIntf->cfg.apply                 = EI_APP_DISCRETE_IO_DEVICE_CFG_apply;
    pIntf->cfg.setDefaultWithoutComm = EI_APP_DISCRETE_IO_DEVICE_CFG_setDefaultWithoutComm;
    pIntf->cfg.callback              = EI_APP_DISCRETE_IO_DEVICE_CFG_callback;
    pIntf->cfg.getRuntimeData        = EI_APP_DISCRETE_IO_DEVICE_CFG_getRuntimeData;
    pIntf->cfg.getFactoryResetData   = EI_APP_DISCRETE_IO_DEVICE_CFG_getFactoryResetData;
    pIntf->cfg.getLength             = EI_APP_DISCRETE_IO_DEVICE_CFG_getLength;

    error = EI_APP_DEVICE_PROFILE_ERR_OK;

laError:
    return error;
}

/*!
 *
 * \brief
 * Basic initialization function.
 *
 * \details
 * Creates a new EtherNet/IP&trade; Adapter.<br />
 * Initializes data structures from non-volatile storage.<br />
 * Registers stack error handler.<br />
 * Initializes the Adapter.<br />
 * Create a CIP&trade; node.<br />
 *
 * \param[in]  pCipNode Pointer to the CIP node.
 *
 * \return     result as boolean.
 *
 * \retval     #true    Success.
 * \retval     #false   Error.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * bool result = false;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * result = EI_APP_DISCRETE_IO_DEVICE_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \see EI_API_ADP_new  EI_API_CIP_NODE_new  EI_API_CIP_EError_t
 *
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
bool EI_APP_DISCRETE_IO_DEVICE_init(EI_API_ADP_T *pAdapter, EI_API_CIP_NODE_T *pCipNode)
{
    bool result = false;

    // Init Discrete IO Device specific objects.
    EI_APP_DISCRETE_IO_DEVICE_DOP_init(pCipNode);
    EI_APP_DISCRETE_IO_DEVICE_DIP_init(pCipNode);

    // Create vendor specific classes.
    result = EI_APP_DISCRETE_IO_DEVICE_cipSetup(pCipNode);

    return result;
}

/*!
 *
 * \brief
 * Cyclically called run function.
 *
 * \details
 * Cyclically called run function, handles the EtherNet/IP stack and hardware
 * specific functionality, such as reading switches, reading inputs, setting outputs
 * and LEDs.
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
 * EI_APP_DISCRETE_IO_DEVICE_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
void EI_APP_DISCRETE_IO_DEVICE_run(EI_API_CIP_NODE_T* pCipNode)
{
    // Run Discrete IO Device specific objects.
    EI_APP_DISCRETE_IO_DEVICE_DOP_run(pCipNode);
    EI_APP_DISCRETE_IO_DEVICE_DIP_run(pCipNode);
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
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * errCode = EI_APP_DISCRETE_IO_DEVICE_DOP_addClassAttribute(pEI_API_CIP_NODE, 0x0001, &revision);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static uint32_t EI_APP_DISCRETE_IO_DEVICE_DOP_addClassAttribute(EI_API_CIP_NODE_T* pCipNode, uint16_t attrId, void* pvValue)
{
    uint32_t errCode;
    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));

    attr.id         = attrId;
    attr.edt        = EI_API_CIP_eEDT_UINT;
    attr.accessRule = EI_API_CIP_eAR_GET;
    attr.pvValue    = pvValue;

    errCode = EI_API_CIP_addClassAttr(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, &attr);

    if(errCode != EI_API_CIP_eERR_OK)
        return errCode;

    errCode = EI_API_CIP_setClassAttr(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, &attr);

    if (errCode != EI_API_CIP_eERR_OK)
        return errCode;

    return EI_API_CIP_eERR_OK;
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
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add attribute 3 for instance 1
 * errCode = EI_APP_DISCRETE_IO_DEVICE_DOP_addInstanceAttribute(
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
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static uint32_t EI_APP_DISCRETE_IO_DEVICE_DOP_addInstanceAttribute(
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
    errCode = EI_API_CIP_addInstanceAttr(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        return errCode;
    }

    // set initial value
    errCode = EI_API_CIP_setInstanceAttr(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        return errCode;
    }

    // set callback function
    if (attr.get_callback != NULL || attr.set_callback != NULL)
    {
        errCode = EI_API_CIP_setInstanceAttrFunc(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            return errCode;
        }
    }

    return EI_API_CIP_eERR_OK;
}

/*!
 *
 * \brief
 * Function provides set access to the attribute value of DOP object.
 *
 * \details
 * Function for the set service of the value. All instances are connected industrial
 * LEDs controlled by TPIC2810.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  value                                       Value.
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
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint8_t value = 1;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * errCode = EI_APP_DISCRETE_IO_DEVICE_DOP_setValue(pEI_API_CIP_NODE, 0x0001, value);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
uint32_t EI_APP_DISCRETE_IO_DEVICE_DOP_setValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId, uint8_t value)
{
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    uint8_t instanceIndex = instanceId - 1;

    if (EI_APP_DISCRETE_IO_DEV_LED_ON == value)
    {
        EI_APP_DISCRETE_IO_DEVICE_ledStatus_s |= EI_APP_DISCRETE_IO_DEV_LED_ON << (instanceIndex);
        CUST_DRIVERS_LED_setIndustrialLeds(EI_APP_DISCRETE_IO_DEVICE_ledStatus_s);
    }
    else
    {
       EI_APP_DISCRETE_IO_DEVICE_ledStatus_s &= ~(EI_APP_DISCRETE_IO_DEV_LED_ON << (instanceIndex));
       CUST_DRIVERS_LED_setIndustrialLeds(EI_APP_DISCRETE_IO_DEVICE_ledStatus_s);
    }

    error = EI_API_CIP_setAttr_bool(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, value);

    return error;
}

/*!
 *
 * \brief
 * Set attribute single service callback of DOP object.
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
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use set callback
 * errCode = EI_APP_DISCRETE_IO_DEVICE_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                     // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,               // Available attribute access rule
 *                              NULL,                                     // No get callback
 *                              EI_APP_DISCRETE_IO_DEVICE_DOP_setValueCb, // Set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DISCRETE_IO_DEVICE_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
uint32_t EI_APP_DISCRETE_IO_DEVICE_DOP_setValueCb(EI_API_CIP_NODE_T* pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, uint16_t len, void* pvValue)
{
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    len = sizeof(uint8_t);
    uint8_t value = *(uint8_t*)pvValue;

    if (sizeof(value) != len)
    {
        error = len < sizeof(bool) ? EI_API_eERR_CB_NOT_ENOUGH_DATA : EI_API_eERR_CB_TOO_MUCH_DATA;
        goto laError;
    }
    else
    {
        error = value <= 1 ? EI_APP_DISCRETE_IO_DEVICE_DOP_setValue(pCipNode, instanceId, value) : EI_API_eERR_CB_INVALID_VALUE;
        goto laError;
    }

laError:
    return error;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute value of DOP object.
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
 * error = EI_APP_DISCRETE_IO_DEVICE_DOP_getValue(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
bool EI_APP_DISCRETE_IO_DEVICE_DOP_getValue(EI_API_CIP_NODE_T* pCipNode, uint8_t instanceId)
{
    uint8_t value = 0;

    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, &value);

    return value;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOP object.
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
 * errCode = EI_APP_DISCRETE_IO_DEVICE_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                     // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,               // Available attribute access rule
 *                              EI_APP_DISCRETE_IO_DEVICE_DOP_getValueCb, // Get callback
 *                              NULL,                                     // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DISCRETE_IO_DEVICE_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
uint32_t EI_APP_DISCRETE_IO_DEVICE_DOP_getValueCb(EI_API_CIP_NODE_T* pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, uint16_t* pLen, void* pvValue)
{
    *pLen = sizeof(bool);
    *(bool*)pvValue = EI_APP_DISCRETE_IO_DEVICE_DOP_getValue(pCipNode, instanceId);

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
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * EI_APP_DISCRETE_IO_DEVICE_DOP_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static void EI_APP_DISCRETE_IO_DEVICE_DOP_init(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    EI_API_CIP_SService_t service;

    errCode = EI_API_CIP_createClass(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID);

    // Example how to evaluate error codes returned by API functions.
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set class instance
    OSAL_MEMORY_memset(&service, 0, sizeof(service));
    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addClassService(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, &service);
    errCode = EI_APP_DISCRETE_IO_DEVICE_DOP_addClassAttribute(pCipNode, 1, &dopClassData_s.revision);

    for (int i = 1; ((EI_API_CIP_eERR_OK == errCode) && (i <= EI_APP_DISCRETE_IO_DOP_NUM_OF_INST)); i++)
    {
        // Dummy value for each instance
        ei_api_cip_edt_bool instanceValue = 0;
        // Create instances
        errCode = EI_API_CIP_createInstance(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, i);

        // Add set service for instances
        service.code = EI_API_CIP_eSC_SETATTRSINGLE;
        errCode = EI_API_CIP_addInstanceService(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, i, &service);
        service.code = EI_API_CIP_eSC_GETATTRSINGLE;
        errCode = EI_API_CIP_addInstanceService(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, i, &service);

        // Add attribute 3 (required) for instance
        errCode = EI_APP_DISCRETE_IO_DEVICE_DOP_addInstanceAttribute(
                                                  pCipNode,
                                                  i,
                                                  EI_APP_DISCRETE_IO_DEVICE_DOP_ATTRIBUTE_3,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  EI_APP_DISCRETE_IO_DEVICE_DOP_getValueCb,
                                                  EI_APP_DISCRETE_IO_DEVICE_DOP_setValueCb,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
    }

laError:
    return;
}

/*!
 *
 * \brief
 * Configuration Assembly callback.
 *
 * \details
 * This function enables & disables the DOP I/O connection for each DOP instance.
 *
 * \param[in]  pCipNode                 Pointer to the CIP node.
 * \param[in]  o2t                      Originator to target.
 * \param[in]  t2o                      Target to originator.
 * \param[in]  configurationInstance    Configuration assembly instance.
 * \param[in]  configurationData        Configuration data.
 * \param[in]  configurationData        Configuration data size.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Create configuration assembly & use it with the callback
 * errCode = EI_API_CIP_createCfgAssemblySimple(pEI_API_CIP_NODE, 0x66, EI_APP_DISCRETE_IO_DEVICE_configurationAssemblyCb);
 *
 * \endcode
 *
 * \see EI_API_CIP_createCfgAssemblySimple  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
uint32_t EI_APP_DISCRETE_IO_DEVICE_configurationAssemblyCb(
                                    EI_API_CIP_NODE_T* pCipNode,
                                    uint16_t           o2t,
                                    uint16_t           t2o,
                                    uint16_t           configurationInstance,
                                    const uint8_t*     configurationData,
                                    uint16_t           configurationSize)
{
    for(int i = 0; i < configurationSize; i++)
    {
        if (0 == configurationData[i])
        {
            EI_APP_DISCRETE_IO_DEVICE_dopStatus_s[i] = EI_APP_DISCRETE_IO_DEVICE_DOP_DISABLED;
        }
        else
        {
            EI_APP_DISCRETE_IO_DEVICE_dopStatus_s[i] = EI_APP_DISCRETE_IO_DEVICE_DOP_ENABLED;
        }
    }

    return EI_API_eERR_CB_NO_ERROR;
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
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * EI_APP_DISCRETE_IO_DEVICE_DOP_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static void EI_APP_DISCRETE_IO_DEVICE_DOP_run(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode = EI_API_CIP_eERR_OK;
    uint8_t buffer[EI_APP_DISCRETE_IO_DOP_NUM_OF_INST] = { 0 };

    errCode = EI_API_CIP_getAssemblyData(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_CONSUMING, buffer, EI_APP_DISCRETE_IO_DOP_NUM_OF_INST);

    // Mirror I/O data
    for(uint8_t instanceIndex = 0; instanceIndex < EI_APP_DISCRETE_IO_DOP_NUM_OF_INST; instanceIndex++)
    {
        if(EI_APP_DISCRETE_IO_DEVICE_dopStatus_s[instanceIndex] == EI_APP_DISCRETE_IO_DEVICE_DOP_ENABLED)
        {
            if (EI_APP_DISCRETE_IO_DEV_LED_OFF == buffer[instanceIndex] &&  EI_API_CIP_eERR_OK == errCode)
            {
                EI_APP_DISCRETE_IO_DEVICE_ledStatus_s &= ~(EI_APP_DISCRETE_IO_DEV_LED_ON << (instanceIndex));
                CUST_DRIVERS_LED_setIndustrialLeds(EI_APP_DISCRETE_IO_DEVICE_ledStatus_s);
            }
            else if (EI_APP_DISCRETE_IO_DEV_LED_ON == buffer[instanceIndex] && EI_API_CIP_eERR_OK == errCode)
            {
                EI_APP_DISCRETE_IO_DEVICE_ledStatus_s |= (EI_APP_DISCRETE_IO_DEV_LED_ON << (instanceIndex));
                CUST_DRIVERS_LED_setIndustrialLeds(EI_APP_DISCRETE_IO_DEVICE_ledStatus_s);
            }
        }
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
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * errCode = EI_APP_DISCRETE_IO_DEVICE_DIP_addClassAttribute(pEI_API_CIP_NODE, 0x0003, &revision);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static uint32_t EI_APP_DISCRETE_IO_DEVICE_DIP_addClassAttribute(EI_API_CIP_NODE_T* pCipNode, uint16_t attrId, void* pvValue)
{
    uint32_t errCode;
    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));

    attr.id = attrId;
    attr.edt = EI_API_CIP_eEDT_UINT;
    attr.accessRule = EI_API_CIP_eAR_GET;
    attr.pvValue = pvValue;
    errCode = EI_API_CIP_addClassAttr(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, &attr);
    if(errCode != EI_API_CIP_eERR_OK)
        return errCode;
    errCode = EI_API_CIP_setClassAttr(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
        return errCode;
    return EI_API_CIP_eERR_OK;
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
 * errCode = EI_APP_DISCRETE_IO_DEVICE_DIP_addInstanceAttribute(
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
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static uint32_t EI_APP_DISCRETE_IO_DEVICE_DIP_addInstanceAttribute (EI_API_CIP_NODE_T*   pCipNode,
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
    errCode = EI_API_CIP_addInstanceAttr(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        return errCode;
    }

    // set initial value
    errCode = EI_API_CIP_setInstanceAttr(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        return errCode;
    }

    // set callback function
    if (attr.get_callback != NULL || attr.set_callback != NULL)
    {
        errCode = EI_API_CIP_setInstanceAttrFunc(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, instanceId, &attr);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            return errCode;
        }
    }

    return EI_API_CIP_eERR_OK;
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
 * error = EI_APP_DISCRETE_IO_DEVICE_DIP_getValue(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \see EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
bool EI_APP_DISCRETE_IO_DEVICE_DIP_getValue(EI_API_CIP_NODE_T* pCipNode, uint8_t instanceId)
{
    uint8_t value = 0;
    // Use DOP object's LED output as input for the DIP object
    if(EI_APP_DISCRETE_IO_DIP_NUM_OF_INST < instanceId)
    {
        return EI_API_eERR_CB_INVALID_VALUE;
    }
    else
    {
        EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, instanceId, EI_APP_DISCRETE_IO_DEVICE_DOP_ATTRIBUTE_3, &value);
    }

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
 * \param[out]  pLen                           Pointer to the data type length.
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
 * errCode = EI_APP_DISCRETE_IO_DEVICE_DIP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                     // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,               // Available attribute access rule
 *                              EI_APP_DISCRETE_IO_DEVICE_DIP_getValueCb, // Get callback
 *                              NULL,                                     // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DISCRETE_IO_DEVICE_DIP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
uint32_t EI_APP_DISCRETE_IO_DEVICE_DIP_getValueCb(EI_API_CIP_NODE_T* pCipNode, uint16_t classId, uint16_t instanceId, uint16_t attrId, uint16_t* pLen, void* pvValue)
{
    *pLen = sizeof(bool);
    *(bool*)pvValue = EI_APP_DISCRETE_IO_DEVICE_DIP_getValue(pCipNode, instanceId);

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
 * EI_APP_DISCRETE_IO_DEVICE_DIP_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \see EI_API_ADP_new  EI_API_CIP_NODE_new  EI_API_CIP_createClass  EI_API_CIP_createInstance  EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static void EI_APP_DISCRETE_IO_DEVICE_DIP_init(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    EI_API_CIP_SService_t service;

    errCode = EI_API_CIP_createClass(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID);

    // Example how to evaluate error codes returned by API functions.
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set class instance
    OSAL_MEMORY_memset(&service, 0, sizeof(service));
    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addClassService(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, &service);
    errCode = EI_APP_DISCRETE_IO_DEVICE_DIP_addClassAttribute(pCipNode, 1, &dipClassData_s.revision);

    for (int i = 1; ((EI_API_CIP_eERR_OK == errCode) && (i <= EI_APP_DISCRETE_IO_DIP_NUM_OF_INST)); i++)
    {
        // Dummy value for each instance
        ei_api_cip_edt_bool instanceValue = 0;
        // Create instances
        errCode = EI_API_CIP_createInstance(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, i);

        // Add get service for instances
        service.code = EI_API_CIP_eSC_GETATTRSINGLE;
        errCode = EI_API_CIP_addInstanceService(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, i, &service);

        // Add attribute 3 (required) for instance
        errCode = EI_APP_DISCRETE_IO_DEVICE_DIP_addInstanceAttribute(pCipNode,
                                                  i,
                                                  EI_APP_DISCRETE_IO_DEVICE_DIP_ATTRIBUTE_3,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  EI_APP_DISCRETE_IO_DEVICE_DIP_getValueCb,
                                                  NULL,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
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
 * EI_APP_DISCRETE_IO_DEVICE_DIP_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static void EI_APP_DISCRETE_IO_DEVICE_DIP_run(EI_API_CIP_NODE_T* pCipNode)
{
    uint8_t buffer[EI_APP_DISCRETE_IO_DIP_NUM_OF_INST] = { 0 };

    // Mirror I/O data
    for(uint8_t instanceId = 0; instanceId < EI_APP_DISCRETE_IO_DIP_NUM_OF_INST; instanceId++)
    {
        EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId + 1, EI_APP_DISCRETE_IO_DEVICE_DOP_ATTRIBUTE_3, &buffer[instanceId]);
    }

    EI_API_CIP_setAssemblyData(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_PRODUCING, buffer, EI_APP_DISCRETE_IO_DIP_NUM_OF_INST);
}

/*!
 *
 * \brief
 * Setup the application with classes, instances, attributes, and assemblies.
 *
 * \details
 * Setup the application with classes, instances, attributes, and assemblies.<br />
 * For the assemblies, use instances in the Vendor Specific range of IDs.
 *
 * \param[in]  pCipNode Pointer to the CIP node.
 *
 * \return     result as boolean.
 *
 * \retval     #true    Success.
 * \retval     #false   Error.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * bool result = false;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * result = EI_APP_DISCRETE_IO_DEVICE_cipSetup(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
static bool EI_APP_DISCRETE_IO_DEVICE_cipSetup(EI_API_CIP_NODE_T* pCipNode)
{

    uint32_t errCode = EI_API_CIP_eERR_OK;

    errCode = EI_API_CIP_createAssembly(pCipNode, 0xFE, EI_API_CIP_eAR_GET); // Input-only.
    errCode = EI_API_CIP_createAssembly(pCipNode, 0xFF, EI_API_CIP_eAR_GET); // Listen-only.

    /* Create assembly instance & add its assembly members */
    errCode = EI_API_CIP_createAssembly(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_PRODUCING, EI_API_CIP_eAR_GET_AND_SET);
    errCode = EI_API_CIP_createAssembly(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_CONSUMING, EI_API_CIP_eAR_GET_AND_SET);
    errCode = EI_API_CIP_createAssembly(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_CONFIGURATION, EI_API_CIP_eAR_GET_AND_SET);

    /* Create configuration assembly */
    errCode = EI_API_CIP_createCfgAssemblySimple(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_CONFIGURATION, EI_APP_DISCRETE_IO_DEVICE_configurationAssemblyCb);

    /* Add DIP intances to producing assembly */
    for(uint8_t instanceId = 1; instanceId <= EI_APP_DISCRETE_IO_DIP_NUM_OF_INST; instanceId++)
    {
        errCode = EI_API_CIP_addAssemblyMember(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_PRODUCING, EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, instanceId, EI_APP_DISCRETE_IO_DEVICE_DIP_ATTRIBUTE_3);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x64:  Error code: 0x%08x\r\n", EI_APP_DISCRETE_IO_DEVICE_DIP_CLASS_ID, instanceId, EI_APP_DISCRETE_IO_DEVICE_DIP_ATTRIBUTE_3, errCode);
        }
    }

    /* Add DOP instances to consuming and configuration assembly */
    for(uint8_t instanceId = 1; instanceId <= EI_APP_DISCRETE_IO_DOP_NUM_OF_INST; instanceId++)
    {
        errCode = EI_API_CIP_addAssemblyMember(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_CONSUMING, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, EI_APP_DISCRETE_IO_DEVICE_DOP_ATTRIBUTE_3);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x65:  Error code: 0x%08x\r\n", EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, EI_APP_DISCRETE_IO_DEVICE_DOP_ATTRIBUTE_3, errCode);
        }

        errCode = EI_API_CIP_addAssemblyMember(pCipNode, EI_APP_DISCRETE_IO_DEVICE_ASSEMBLY_CONFIGURATION, EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, EI_APP_DISCRETE_IO_DEVICE_DOP_ATTRIBUTE_3);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_printf("Failed to add Class ID %#x, Instance ID %#x, Attribute ID %#x to Assembly Instance 0x66:  Error code: 0x%08x\r\n", EI_APP_DISCRETE_IO_DEVICE_DOP_CLASS_ID, instanceId, EI_APP_DISCRETE_IO_DEVICE_DOP_ATTRIBUTE_3, errCode);
        }
    }

    return true;
}

//*************************************************************************************************
