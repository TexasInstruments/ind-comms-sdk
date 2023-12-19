/*!
 *  \file app_discrete_io_device_cfg.c
 *
 *  \brief
 *  Configuration implementation specific to discrete IO device profile.
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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "EI_API.h"

#include "drivers/CUST_drivers.h"

#include "appCipCodes.h"
#include "appCfg.h"
#include "appRst.h"

#include "device_profiles/discrete_io_device/app_discrete_io_device.h"
#include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"

// Declaration of static local functions
static uint32_t EI_APP_DIO_DEVICE_CFG_applyIdentity (EI_API_ADP_T *pAdapter);
static uint32_t EI_APP_DIO_DEVICE_CFG_applyTimeSync (EI_API_ADP_T *pAdapter);

// Definition of static local variables
static EI_API_ADP_T                         *pAdapter_s = NULL;
static EI_APP_DIO_DEVICE_CFG_Data_t  EI_APP_DIO_DEVICE_CFG_runtimeData;
static EI_APP_DIO_DEVICE_CFG_Data_t  EI_APP_DIO_DEVICE_CFG_factoryResetData;

const EI_APP_DIO_DEVICE_CFG_ProfileData_t EI_APP_DIO_DEVICE_CFG_profileFactoryResetData =
{
    // Initialization of factory default data related to device profile of discrete IO device
};

/*!
 * \brief
 * Makes full initialization of factory reset values.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_init function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \param[in]     pAdapter Pointer to adapter object.
 *
 * \return        value as bool.
 * \retval        true     Success.
 * \retval        false    Invalid input parameter defined as NULL.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.init = EI_APP_DIO_DEVICE_CFG_init;

 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_init
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
bool EI_APP_DIO_DEVICE_CFG_init (EI_API_ADP_T *pAdapter)
{
    bool ret = false;

    if (NULL == pAdapter)
    {
        goto laError;
    }

    pAdapter_s = pAdapter;

    EI_APP_DIO_DEVICE_CFG_factoryResetData.header.magicNumber = EI_APP_DIO_DEVICE_CFG_HEADER_MAGICNUMBER;
    EI_APP_DIO_DEVICE_CFG_factoryResetData.header.version     = EI_APP_DIO_DEVICE_CFG_HEADER_VERSION;
    EI_APP_DIO_DEVICE_CFG_factoryResetData.header.checksum    = 0;

    OSAL_MEMORY_memcpy(&EI_APP_DIO_DEVICE_CFG_factoryResetData.adapter,
                       &EI_APP_RST_adapterFactoryDefaultValues,
                       sizeof(EI_APP_CFG_AdapterData_t));

    OSAL_MEMORY_memcpy(&EI_APP_DIO_DEVICE_CFG_factoryResetData.profile,
                       &EI_APP_DIO_DEVICE_CFG_profileFactoryResetData,
                       sizeof(EI_APP_DIO_DEVICE_CFG_ProfileData_t));

    EI_API_ADP_setCmgrCb(EI_APP_DIO_DEVICE_cmgrCb);
    EI_API_ADP_setCobjTimeOutCb(EI_APP_DIO_DEVICE_cobjTimeOutCb);

    ret = true;

laError:
    return ret;
}

/*!
 * \brief
 * Applies the discrete io device profile configuration data on stack.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_apply function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \remarks
 * Any error on adapter interface function can lead to hard fault and
 * to call of OSAL error handler which leads to the stop of execution.
 *
 * \param[in]     pAdapter Pointer to adapter object.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.apply = EI_APP_DIO_DEVICE_CFG_apply;
 *
 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_apply EI_APP_DIO_DEVICE_CFG_applyIdentity EI_APP_DIO_DEVICE_CFG_applyTimeSync
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
void EI_APP_DIO_DEVICE_CFG_apply (EI_API_ADP_T *pAdapter)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    if (NULL == pAdapter)
    {
        goto laError;
    }

    errCode = EI_APP_DIO_DEVICE_CFG_applyIdentity(pAdapter);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_APP_DIO_DEVICE_CFG_applyTimeSync(pAdapter);

laError:
    if (EI_API_ADP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    return;
}

/*!
 * \brief
 * Configuration callback to synchronize discrete io device profile non-volatile values
 * with actual stack attribute values. Sets the new non-volatile data, if necessary.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_callback function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \remark
 * If actual stack attribute values are same as discrete IO device profile non volatile
 * values then there is no need to write them to the non volatile memory.
 *
 * \param[in]     pCipNode    Pointer to CIP object.
 * \param[in]     classId     Class identifier.
 * \param[in]     instanceId  Instance identifier.
 * \param[in]     attrId      Attribute identifier.
 * \param[in]     serviceCode Service code.
 * \param[in]     serviceFlag Service flag.
 *
 * \return        value as bool.
 * \retval        true     Non volatile discrete IO device profile configuration part was changed.
 * \retval        false    No change.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.callback = EI_APP_DIO_DEVICE_CFG_callback;
 *
 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_callback
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
bool EI_APP_DIO_DEVICE_CFG_callback ( EI_API_CIP_NODE_T *pCipNode,
                                              uint16_t           classId,
                                              uint16_t           instanceId,
                                              uint16_t           attrId,
                                              EI_API_CIP_ESc_t   serviceCode,
                                              int16_t            serviceFlag)
{
    return false;
}

/*!
 * \brief
 * Sets discrete IO device profile factory default values to the profile part of non volatile runtime structure.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_setDefaultWithoutComm function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.setDeafaultWithoutComm = EI_APP_DIO_DEVICE_CFG_setDefaultWithoutComm;
 *
 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_setDefaultWithoutComm
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
void EI_APP_DIO_DEVICE_CFG_setDefaultWithoutComm (void)
{
    OSAL_MEMORY_memcpy(&EI_APP_DIO_DEVICE_CFG_runtimeData.profile,
                       &EI_APP_DIO_DEVICE_CFG_factoryResetData.profile,
                       sizeof(EI_APP_DIO_DEVICE_CFG_ProfileData_t));
}

/*!
 * \brief
 * Sets header for non volatile runtime structure.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_setHeader function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.setHeader = EI_APP_DIO_DEVICE_CFG_setHeader;
 *
 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_setHeader
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
void EI_APP_DIO_DEVICE_CFG_setHeader (EI_APP_CFG_Header_t *pHeader)
{
    if (NULL == pHeader)
    {
        goto laError;
    }

    pHeader->magicNumber = EI_APP_DIO_DEVICE_CFG_HEADER_MAGICNUMBER;
    pHeader->version     = EI_APP_DIO_DEVICE_CFG_HEADER_VERSION;
    pHeader->checksum    = 0;  // calculate proper checksum

laError:
    return;
}

/*!
 * \brief
 * Checks header of non volatile runtime structure against discrete IO device profile header.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_isValid function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \remark
 * Function is called directly after non volatile memory read to check data consistency.
 *
 * \return        value as bool.
 * \retval        true     Header is corresponding to header of discrete IO device profile and non volatile data are valid.
 * \retval        false    Header is not corresponding to header of discrete IO device profile or non volatile data are corrupted.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.isValid = EI_APP_DIO_DEVICE_CFG_isValid;
 *
 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_isValid
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
bool EI_APP_DIO_DEVICE_CFG_isValid (void)
{
    bool ret = false;

    uint32_t checkSum = 0;  // calculate proper checksum

    if ( (EI_APP_DIO_DEVICE_CFG_runtimeData.header.magicNumber == EI_APP_DIO_DEVICE_CFG_HEADER_MAGICNUMBER)  &&
         (EI_APP_DIO_DEVICE_CFG_runtimeData.header.version    == EI_APP_DIO_DEVICE_CFG_HEADER_VERSION)      &&
         (EI_APP_DIO_DEVICE_CFG_runtimeData.header.checksum    == checkSum)
       )
    {
        ret = true;
    }

    return ret;
}

/*!
 * \brief
 * Provides pointer to runtime configuration non volatile data.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_getRuntimeData function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \return        pointer to runtime configuration non volatile data.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.getRuntimeData = EI_APP_DIO_DEVICE_CFG_getRuntimeData;
 *
 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_getRuntimeData
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
void* EI_APP_DIO_DEVICE_CFG_getRuntimeData (void)
{
    return (void*) &EI_APP_DIO_DEVICE_CFG_runtimeData;
}

/*!
 * \brief
 * Provides pointer to factory reset non volatile data.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \return        pointer to factory reset non volatile data.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.getFactoryResetData = EI_APP_DIO_DEVICE_CFG_getFactoryResetData;
 *
 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
void* EI_APP_DIO_DEVICE_CFG_getFactoryResetData (void)
{
    return (void*) &EI_APP_DIO_DEVICE_CFG_factoryResetData;
}

/*!
 * \brief
 * Provides size of runtime configuration or factory reset data.
 *
 * \details
 * Called by #EI_APP_DEVICE_PROFILE_CFG_getLength function of device profile interface.
 * Function needs to be registered by #EI_APP_DEVICE_PROFILE_register call.
 *
 * \remark
 * Size of data needs to be same for both runtime configuration and factory reset data.
 *
 * \return        value as uint32_t.
 * \retval        Size of configuration data.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * uint32_t EI_APP_DEVICE_PROFILE_register (EI_APP_DEVICE_PROFILE_Interface_t* pIntf)
 * {
 *     uint32_t error = EI_APP_DEVICE_PROFILE_ERR_General;
 *
 *     if (NULL == pIntf)
 *     {
 *         error = EI_APP_DEVICE_PROFILE_ERR_PtrInvalid;
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     pIntf->cfg.getLength = EI_APP_DIO_DEVICE_CFG_getLength;
 *
 *     ...
 *
 *     error = EI_APP_DEVICE_PROFILE_ERR_OK;
 *
 * laError:
 *     return error;
 * }
 * \endcode
 *
 * \see EI_APP_DEVICE_PROFILE_register EI_APP_DEVICE_PROFILE_CFG_getFactoryResetData
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
uint32_t EI_APP_DIO_DEVICE_CFG_getLength (void)
{
    return sizeof(EI_APP_DIO_DEVICE_CFG_Data_t);
}

// ***************** Local function definition area ***************************** //

/*!
 * \brief
 * Applies the identity object attributes on the stack.
 *
 * \details
 * Called by #EI_APP_DIO_DEVICE_CFG_apply interface function.
 *
 * \param[in]     pAdapter Pointer to adapter object.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * void EI_APP_DIO_DEVICE_CFG_apply (EI_API_ADP_T *pAdapter)
 * {
 *     uint32_t errCode = EI_API_ADP_eERR_GENERAL;
 *
 *     if (NULL == pAdapter)
 *     {
 *         goto laError;
 *     }
 *
 *     errCode = EI_APP_DIO_DEVICE_CFG_applyIdentity(pAdapter);
 *
 *     ...
 *
 * laError:
 *     if (EI_API_ADP_eERR_OK != errCode)
 *     {
 *         OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
 *     }
 *
 *     return;
 * }
 * \endcode
 *
 * \see EI_APP_DIO_DEVICE_CFG_apply
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
static uint32_t EI_APP_DIO_DEVICE_CFG_applyIdentity (EI_API_ADP_T *pAdapter)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    const char productName[] = EI_APP_DIO_DEVICE_PRODUCT_NAME;

    uint16_t vendorId     = EI_APP_DIO_DEVICE_VENDOR_ID;
    uint16_t deviceType   = EI_APP_DIO_DEVICE_DEVICE_TYPE;
    uint16_t productCode  = EI_APP_DIO_DEVICE_PRODUCT_CODE;
    uint32_t serialNumber = EI_APP_DIO_DEVICE_SERIAL_NUMBER;

    EI_API_ADP_SRevision_t revision;

    revision.major = EI_APP_DIO_DEVICE_REVISION_MAJOR;
    revision.minor = EI_APP_DIO_DEVICE_REVISION_MINOR;

    errCode = EI_API_ADP_setVendorId(pAdapter, vendorId);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setDeviceType(pAdapter, deviceType);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setProductCode(pAdapter, productCode);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setRevision(pAdapter, revision);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setSerialNumber(pAdapter, serialNumber);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setProductName(pAdapter, productName);

laError:

    return errCode;
}

/*!
 * \brief
 * Applies the Time Sync object attributes on the stack.
 *
 * \details
 * Called by #EI_APP_DIO_DEVICE_CFG_applyTimeSync interface function.
 *
 * \param[in]     pAdapter Pointer to adapter object.
 *
 * \par Example
 * \code{.c}
 * #include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
 *
 * void EI_APP_DIO_DEVICE_CFG_apply (EI_API_ADP_T *pAdapter)
 * {
 *     uint32_t errCode = EI_API_ADP_eERR_GENERAL;
 *
 *     if (NULL == pAdapter)
 *     {
 *         goto laError;
 *     }
 *
 *     ...
 *
 *     errCode = EI_APP_DIO_DEVICE_CFG_applyTimeSync(pAdapter);
 *
 * laError:
 *     if (EI_API_ADP_eERR_OK != errCode)
 *     {
 *         OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
 *     }
 *
 *     return;
 * }
 * \endcode
 *
 * \see EI_APP_DIO_DEVICE_CFG_apply
 *
 * \ingroup EI_APP_DIO_DEVICE_CFG
 *
 */
static uint32_t EI_APP_DIO_DEVICE_CFG_applyTimeSync (EI_API_ADP_T *pAdapter)
{
    uint32_t errCode = EI_API_ADP_eERR_GENERAL;

    const char timeSyncProductDescription[] = TIMESYNC_PRODUCT_DESCRIPTION_OF_CONFIGURATION;
    const char timeSyncManufactureID[]      = TIMESYNC_MANUFACTURE_ID_OF_CONFIGURATION;
    const char timeSyncRevisionData[]       = TIMESYNC_REVISION_DATA_OF_CONFIGURATION;

    errCode = EI_API_ADP_setTimeSyncManufactureID(pAdapter, timeSyncManufactureID);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setTimeSyncRevisionData(pAdapter, timeSyncRevisionData);

    if (EI_API_ADP_eERR_OK != errCode)
    {
        goto laError;
    }

    errCode = EI_API_ADP_setTimeSyncProductDescription(pAdapter, timeSyncProductDescription);

laError:

    return errCode;
}
