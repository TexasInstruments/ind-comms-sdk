/*!
 *  \example ecSlvCiA402.c
 *
 *  \brief
 *  CiA402 Application implementation.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 KUNBUS GmbH.
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

 /*-----------------------------------------------------------------------------------------
 ------
 ------    Includes
 ------
 -----------------------------------------------------------------------------------------*/
#include <ecSlvApi.h>
#include <defines/ecSlvApiDef.h>
#include "ecSlvCiA402.h"
#include "ESL_cia402Obd.h"
#include "ESL_cia402Demo.h"
#include "project.h"

#include <ESL_os.h>
#include <ESL_BOARD_config.h>

#include <ESL_vendor.h>

#include <ESL_gpioHelper.h>
#include <ESL_foeDemo.h>
#include <ESL_soeDemo.h>
#include <ESL_eeprom.h>
#include <ESL_version.h>

#include <ecSlvApi.h>

#if !(defined MBXMEM)
#define MBXMEM
#endif

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/
#define I2C_IOEXP_ADDR 0x60                  // The I2C address for GPIO expander

#define RXPDOMAP_INDEX    (0x1600u)
#define TXPDOMAP_INDEX    (0x1A00u)

#define COE_SM_ERROR_MAX_COUNT  (0xFFFFu)

#define AXIS_ONE        0
#define AXIS_TWO        1
#define AXIS_THREE      2

#if !(defined BIT2BYTE)
#define BIT2BYTE(x)                     (((x)+7) >> 3)
#endif

static void EC_SLV_APP_CIA_applicationRun(void* appCtxt);
static EC_API_EError_t EC_SLV_APP_populateCiA402Functions(EC_SLV_APP_CIA_Application_t* pApplicationInstance_p);
static EC_API_EError_t EC_SLV_APP_populateTxPDO(EC_SLV_APP_CIA_Application_t* pApplicationInstance_p, EC_API_SLV_Pdo_t* pPdo_p, uint8_t axis_p);

#if (defined GPIO_TEST_PINS) && (1==GPIO_TEST_PINS)
#if (defined GPIO_TEST_PROFILE_SEL) && (defined GPIO_TEST_PROFILE_2) && (GPIO_TEST_PROFILE_2 == GPIO_TEST_PROFILE_SEL)
static void EC_SLV_APP_measurement(void* pMsrmtCtxt_p, uint32_t measureChannel_p, bool channelOn_p);
#endif
#endif

/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Configure slave information parameters.
 *
 *  \details
 *  Configure device identification parameters and basic EtherCAT configuration parameters.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pEcApiSlv_p     SDK instance.
 *
 *  \return     EC_API_EError_t as uint32_t.
 *
 *  <!-- Group: -->
 *
 *  \ingroup CiA402
 *
 */
static EC_API_EError_t EC_SLV_APP_CIA_populateSlaveInfo(EC_SLV_APP_CIA_Application_t* pApplicationInstance_p)
{
    EC_API_EError_t         error   = EC_API_eERR_INVALID;
    EC_API_SLV_SHandle_t*   ptSlave;

    if (!pApplicationInstance_p)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }
    ptSlave = pApplicationInstance_p->ptEcSlvApi;

    error = (EC_API_EError_t)EC_API_SLV_setDeviceType    (ptSlave, EC_API_SLV_eDT_SERVO_DRIVE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setVendorId      (ptSlave, ECAT_VENDORID);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setRevisionNumber(ptSlave, EC_REVISION);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setSerialNumber  (ptSlave, 0x00000000);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setProductCode   (ptSlave, ECAT_PRODUCTCODE_CIA402);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setProductName   (ptSlave, ECAT_PRODUCTNAME_CIA402);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setGroupType     (ptSlave, "EtherCAT Toolkit");
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setHwVersion     (ptSlave, "R01");
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = ESL_setSWVersion(ptSlave);
    if (error != EC_API_eERR_NONE)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    /* Former Project.h */
    error = (EC_API_EError_t)EC_API_SLV_setPDOSize(ptSlave, EC_MAX_PD_LEN, EC_MAX_PD_LEN);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setPDICfg(ptSlave, ESC_EE_PDI_CONTROL, ESC_EE_PDI_CONFIG);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setBootStrapMailbox(ptSlave,
                                                            EC_BOOTSTRAP_MBXOUT_START, EC_BOOTSTRAP_MBXOUT_DEF_LENGTH,
                                                            EC_BOOTSTRAP_MBXIN_START, EC_BOOTSTRAP_MBXIN_DEF_LENGTH);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setStandardMailbox(ptSlave,
                                                           EC_MBXOUT_START, EC_MBXOUT_DEF_LENGTH,
                                                           EC_MBXIN_START, EC_MBXIN_DEF_LENGTH);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setSyncManConfig(ptSlave,
                                                         0, EC_MBXOUT_START, EC_MBXOUT_DEF_LENGTH,
                                                         EC_MBXOUT_CONTROLREG, EC_MBXOUT_ENABLE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setSyncManConfig(ptSlave,
                                                         1, EC_MBXIN_START, EC_MBXIN_DEF_LENGTH,
                                                         EC_MBXIN_CONTROLREG, EC_MBXIN_ENABLE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setSyncManConfig(ptSlave,
                                                         2, EC_OUTPUT_START, EC_OUTPUT_DEF_LENGTH,
                                                         EC_OUTPUT_CONTROLREG, EC_OUTPUT_ENABLE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setSyncManConfig(ptSlave,
                                                         3, EC_INPUT_START, EC_INPUT_DEF_LENGTH,
                                                         EC_INPUT_CONTROLREG, EC_INPUT_ENABLE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_setSyncManErrLimit(ptSlave, COE_SM_ERROR_MAX_COUNT);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_API_eERR_NONE;
Exit:
    return error;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Configure CiA402 parameters.
 *
 *  \details
 *  Register and call CiA402 functions.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pApplicationInstance_p     Application instance.
 *
 *  \return     EC_API_EError_t as uint32_t.
 *
 *  <!-- Group: -->
 *
 *  \ingroup CiA402
 *
 */
static EC_API_EError_t EC_SLV_APP_populateCiA402Functions(EC_SLV_APP_CIA_Application_t* pApplicationInstance_p)
{
    EC_API_EError_t         error   = EC_API_eERR_INVALID;
    EC_API_SLV_SHandle_t*   ptSlave;

    if (!pApplicationInstance_p)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    ptSlave = pApplicationInstance_p->ptEcSlvApi;

    EC_API_SLV_CiA402_setAxisNumber         (ptSlave, AXES_NUMBER);
    EC_API_SLV_CiA402_registerSetDictValues (ptSlave, EC_SLV_APP_setObdValues, pApplicationInstance_p);
    EC_API_SLV_CiA402_registerApplication   (ptSlave, EC_SLV_APP_cia402Application, pApplicationInstance_p);
    EC_API_SLV_CiA402_registerLocalError    (ptSlave, EC_SLV_APP_cia402LocalError, pApplicationInstance_p);
    EC_API_SLV_cbRegisterStartInputHandler  (ptSlave, EC_SLV_APP_CIA_startInputHandler, pApplicationInstance_p);

    error = (EC_API_EError_t)EC_SLV_APP_cia402ObjectDictionary(pApplicationInstance_p);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d CiA402 Object Dictionary error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_API_eERR_NONE;
Exit:
    return error;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Configure RxPDO mapping.
 *
 *  \details
 *  Map operation modes CSP, CSV and CST.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pApplicationInstance_p  Applicaiton instance.
 *  \param[in]  pPdo_p                  RxPDO instance.
 *  \param[in]  axis_p                  axis number.
 *
 *  \return     EC_API_EError_t as uint32_t.
 *
 *  <!-- Group: -->
 *
 *  \ingroup CiA402
 *
 */
static EC_API_EError_t EC_SLV_APP_populateRxPDO(EC_SLV_APP_CIA_Application_t* pApplicationInstance_p, EC_API_SLV_Pdo_t* pPdo_p, uint8_t axis_p)
{
    EC_API_EError_t             error       = EC_API_eERR_INVALID;
    EC_API_SLV_SHandle_t*       ptSlave     = NULL;
    uint16_t                    rxIndex     = RXPDOMAP_INDEX + axis_p;
    EC_API_SLV_SCoE_ObjEntry_t* pObjEntry   = NULL;
    uint32_t                    curOffset   = 0;
    uint32_t                    offset      = 0;
    uint8_t                     subIndex    = 0;

    if (!pApplicationInstance_p)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    ptSlave = pApplicationInstance_p->ptEcSlvApi;

    error = (EC_API_EError_t)EC_API_SLV_PDO_create(ptSlave, "RxPDO", rxIndex, &pPdo_p);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Create PDO 0x%04x error code: 0x%08x\r\n", __func__, __LINE__, rxIndex, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].controlWordIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].controlWordIndex.pdoObject          = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].controlWordIndex.pdoObjectOffset    = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 001", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }
    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);
    subIndex++;

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].modesOfOperationIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].modesOfOperationIndex.pdoObject          = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].modesOfOperationIndex.pdoObjectOffset    = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 002", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);
    subIndex++;

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].targetPositionIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].targetPositionIndex.pdoObject       = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].targetPositionIndex.pdoObjectOffset = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 003", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);
    subIndex++;

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].targetVelocityIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].targetVelocityIndex.pdoObject       = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].targetVelocityIndex.pdoObjectOffset = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 004", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);
    subIndex++;

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].targetTorqueIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].targetTorqueIndex.pdoObject         = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].targetTorqueIndex.pdoObjectOffset   = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 005", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);

    pApplicationInstance_p->realPdoInLen = curOffset;

    error = EC_API_eERR_NONE;
Exit:
    return error;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Configure TxPDO mapping.
 *
 *  \details
 *  Map operation modes CSP, CSV and CST.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pEcApiSlv_p     SDK instance.
 *  \param[in]  pPdo_p          RxPDO instance.
 *  \param[in]  axis_p          axis number.
 *
 *  \return     EC_API_EError_t as uint32_t.
 *
 *  <!-- Group: -->
 *
 *  \ingroup CiA402
 *
 */
static EC_API_EError_t EC_SLV_APP_populateTxPDO(EC_SLV_APP_CIA_Application_t* pApplicationInstance_p, EC_API_SLV_Pdo_t* pPdo_p, uint8_t axis_p)
{
    EC_API_EError_t             error       = EC_API_eERR_INVALID;
    EC_API_SLV_SHandle_t*       ptSlave     = NULL;
    uint16_t                    txIndex     = TXPDOMAP_INDEX + axis_p;
    EC_API_SLV_SCoE_ObjEntry_t* pObjEntry   = NULL;
    uint32_t                    curOffset   = 0;
    uint32_t                    offset      = 0;
    uint8_t                     subIndex    = 0;

    if (!pApplicationInstance_p)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    ptSlave = pApplicationInstance_p->ptEcSlvApi;

    error = (EC_API_EError_t)EC_API_SLV_PDO_create(ptSlave, "TxPDO", txIndex, &pPdo_p);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create PDO for axis %d error code: 0x%08x\r\n", axis_p);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].statusWordIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].statusWordIndex.pdoObject       = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].statusWordIndex.pdoObjectOffset = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 001", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);
    subIndex++;

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].modesOfOperationDisplayIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].modesOfOperationDisplayIndex.pdoObject          = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].modesOfOperationDisplayIndex.pdoObjectOffset    = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 002", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("PDO 0x1A00 Entry 2 Error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);
    subIndex++;

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].positionActualValueIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].positionActualValueIndex.pdoObject          = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].positionActualValueIndex.pdoObjectOffset    = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 003", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("PDO 0x1A00 Entry 2 Error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);
    subIndex++;

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].velocityActualValueIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].velocityActualValueIndex.pdoObject          = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].velocityActualValueIndex.pdoObjectOffset    = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 004", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("PDO 0x1A00 Entry 2 Error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);
    subIndex++;

    error = (EC_API_EError_t)EC_API_SLV_CoE_getObjectEntry(ptSlave, pApplicationInstance_p->CiA402_axisData[axis_p].torqueActualValueIndex.objectIndex, 0, &pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Variable Error code: 0x%08x\r\n", __func__, __LINE__, error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    pApplicationInstance_p->CiA402_axisData[axis_p].torqueActualValueIndex.pdoObject        = pPdo_p;
    pApplicationInstance_p->CiA402_axisData[axis_p].torqueActualValueIndex.pdoObjectOffset  = curOffset;

    error = (EC_API_EError_t)EC_API_SLV_PDO_createEntry(ptSlave, pPdo_p, "SubIndex 005", pObjEntry);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("PDO 0x1A00 Entry 2 Error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_PDO_getEntryDataLength(ptSlave, pPdo_p, subIndex, &offset);
    curOffset += BIT2BYTE(offset);

    pApplicationInstance_p->realPdoOutLen = curOffset;

    error = EC_API_eERR_NONE;
Exit:
    return error;
}

#if (defined GPIO_TEST_PINS) && (1==GPIO_TEST_PINS)
#if (defined GPIO_TEST_PROFILE_SEL) && (defined GPIO_TEST_PROFILE_2) && (GPIO_TEST_PROFILE_2 == GPIO_TEST_PROFILE_SEL)
void EC_SLV_APP_measurement(void* pMsrmtCtxt_p, uint32_t measureChannel_p, bool channelOn_p)
{
    EC_SLV_APP_Application_t*                  pApplicationInstance    = (EC_SLV_APP_Application_t*)pMsrmtCtxt_p;

    if(NULL != pApplicationInstance)
    {
        volatile CSL_GpioBank_registersRegs *       pGpioRegBank            = NULL;
        uint32_t                                    pinMask                 = 0;

        switch (measureChannel_p)
        {
        case 0:     pGpioRegBank = ESL_TESTPIN_STATE_REG_BANK; pinMask = ESL_TESTPIN_0_MASK; break;
        case 1:     pGpioRegBank = ESL_TESTPIN_STATE_REG_BANK; pinMask = ESL_TESTPIN_1_MASK; break;
        case 2:     pGpioRegBank = ESL_TESTPIN_STATE_REG_BANK; pinMask = ESL_TESTPIN_2_MASK; break;
        default:
            break;
        }

        if (channelOn_p)
        {
            ESL_GPIO_testPins_set(pGpioRegBank, pinMask);
        }
        else
        {
            ESL_GPIO_testPins_clear(pGpioRegBank, pinMask);
        }
    }
}
#endif
#endif

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Reset board PHYs.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pCallContext_p  Board phy context.
 *  \param[in]  phyIdx_p  phy id.
 *  \param[in]  bReset_p  reset value.
 *
 */
#if !(defined DPRAM_REMOTE) && !(defined FBTL_REMOTE)
static OSAL_FUNC_UNUSED void EC_SLV_APP_CIA_boardPhyReset(void* pCtxt_p, uint8_t phyIdx_p, bool reset_p)
{
    /* @cppcheck_justify{misra-c2012-11.5} generic API requires cast */
    /* cppcheck-suppress misra-c2012-11.5 */
    EC_SLV_APP_CIA_Application_t*  pApplicationInstance    = (EC_SLV_APP_CIA_Application_t*)pCtxt_p;

    ESL_BOARD_OS_phyReset(pApplicationInstance->gpioHandle, pApplicationInstance->selectedPruInstance, phyIdx_p, reset_p);
}
#endif

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Board Status LED configuration.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pCallContext_p Board phy context.
 *  \param[in]  phyIdx_p  phy id.
 *  \param[in]  bReset_p  reset value.
 *
 */
static void EC_SLV_APP_CIA_appBoardStatusLed(void* pCallContext_p, bool runLed_p, bool errLed_p)
{
    /* @cppcheck_justify{misra-c2012-11.5} generic API requires cast */
    /* cppcheck-suppress misra-c2012-11.5 */
    EC_SLV_APP_CIA_Application_t*  pApplicationInstance    = (EC_SLV_APP_CIA_Application_t*)pCallContext_p;

    if (NULL == pApplicationInstance)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    ESL_BOARD_OS_statusLED(pApplicationInstance->gpioHandle, pApplicationInstance->selectedPruInstance, runLed_p, errLed_p);

Exit:
    return;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Register board functions.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pApplicationInstance 	Application context.
 *
 *  \return     EC_API_EError_t as uint32_t.
 *
 */
static EC_API_EError_t EC_SLV_APP_CIA_populateBoardFunctions(EC_SLV_APP_CIA_Application_t* pApplicationInstance)
{
    EC_API_EError_t         error   = EC_API_eERR_INVALID;

    if (!pApplicationInstance)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_cbRegisterBoardStatusLed(pApplicationInstance->ptEcSlvApi
                                        ,EC_SLV_APP_CIA_appBoardStatusLed
                                        ,pApplicationInstance);

    error = EC_API_eERR_NONE;
Exit:
    return error;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Init board.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppInstance_p  Application instance.
 *
 */
void EC_SLV_APP_CIA_initBoardFunctions(EC_SLV_APP_CIA_Application_t *pAppInstance_p)
{
    if (!pAppInstance_p)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    /* open gpio instance */
    pAppInstance_p->gpioHandle = ESL_GPIO_init();

#if !(defined DPRAM_REMOTE) && !(defined FBTL_REMOTE)
    /* Configure manual MDIO mode workaround if configured in SysConfig or similar
     * This is to enable the TI workaround for errata i2329. The activation is
     * detected and provisioned in this call.
     */
    ESL_OS_manualMdioConfig(pAppInstance_p->ptEcSlvApi);

    /* configure Phy Reset Pin */
    ESL_BOARD_OS_configureResets(pAppInstance_p->gpioHandle, pAppInstance_p->selectedPruInstance);
#else
    OSALUNREF_PARM(pAppInstance_p);
#endif

    /* configure LED Pin */
    ESL_BOARD_OS_initStatusLED(pAppInstance_p->gpioHandle, pAppInstance_p->selectedPruInstance);

    ESL_GPIO_apply(pAppInstance_p->gpioHandle);

#if !(defined DPRAM_REMOTE) && !(defined FBTL_REMOTE)
    /* set Phy Reset Pin */
    EC_SLV_APP_CIA_boardPhyReset(pAppInstance_p, EC_API_SLV_ePHY_IN, true);
    EC_SLV_APP_CIA_boardPhyReset(pAppInstance_p, EC_API_SLV_ePHY_OUT, true);
#endif

Exit:
    return;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Register PHYs.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppInstance_p  Application instance.
 *
 */
void EC_SLV_APP_CIA_registerStacklessBoardFunctions(EC_SLV_APP_CIA_Application_t *pAppInstance_p)
{
    if (!pAppInstance_p)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

#if !(defined DPRAM_REMOTE) && !(defined FBTL_REMOTE)
    ESL_BOARD_OS_registerPhys(pAppInstance_p->ptEcSlvApi, pAppInstance_p->selectedPruInstance);

    EC_API_SLV_cbRegisterPhyReset(pAppInstance_p->ptEcSlvApi, EC_SLV_APP_CIA_boardPhyReset, pAppInstance_p);
#endif

Exit:
    return;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Initialize slave application example.
 *
 *  \details
 *  Creates a SDK instance and sets the slave application
 *  by configuring the EtherCAT basic configuration, object dictionary
 *  PDO configuration, CiA402 application and other fieldbus settings.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppInstance_p     application instance.
 *
 *  <!-- Group: -->
 *
 *  \ingroup CiA402
 *
 */
void EC_SLV_APP_CIA_applicationInit(EC_SLV_APP_CIA_Application_t* pAppInstance_p)
{
    EC_API_EError_t error;

    if (!pAppInstance_p)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    // Initialize SDK
    pAppInstance_p->ptEcSlvApi = EC_API_SLV_new();
    if (!pAppInstance_p->ptEcSlvApi)
    {
        OSAL_error(__func__, __LINE__, OSAL_CONTAINER_NOMEMORY, true, 0);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_SLV_APP_CIA_populateBoardFunctions(pAppInstance_p);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Populate board functions Error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_SLV_APP_CIA_populateSlaveInfo(pAppInstance_p);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create Slave Info Error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_SLV_APP_populateCiA402Functions(pAppInstance_p);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create slave information error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    //RxPDO: Master to Slave COM
    error = EC_SLV_APP_populateRxPDO(pAppInstance_p, pAppInstance_p->ptRxPdo1600, AXIS_ONE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create RxPDO error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_SLV_APP_populateRxPDO(pAppInstance_p, pAppInstance_p->ptRxPdo1601, AXIS_TWO);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create RxPDO error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_SLV_APP_populateRxPDO(pAppInstance_p, pAppInstance_p->ptRxPdo1602, AXIS_THREE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create RxPDO error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    //TxPDO: Slave to Master COM
    error = EC_SLV_APP_populateTxPDO(pAppInstance_p, pAppInstance_p->ptTxPdo1A00, AXIS_ONE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create TxPDO error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_SLV_APP_populateTxPDO(pAppInstance_p, pAppInstance_p->ptTxPdo1A01, AXIS_TWO);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create TxPDO error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    error = EC_SLV_APP_populateTxPDO(pAppInstance_p, pAppInstance_p->ptTxPdo1A02, AXIS_THREE);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Create TxPDO error code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

#if !(defined DPRAM_REMOTE) && !(defined FBTL_REMOTE)
    EC_API_SLV_cbRegisterFlashInit              (pAppInstance_p->ptEcSlvApi, EC_SLV_APP_EEP_initFlash, pAppInstance_p->ptEcSlvApi);
    /* @cppcheck_justify{misra-c2012-11.6} generic API requires cast */
    /* cppcheck-suppress misra-c2012-11.6 */
    EC_API_SLV_EEPROM_cbRegisterWrite           (pAppInstance_p->ptEcSlvApi, EC_SLV_APP_EEP_writeEeprom, OSPIFLASH_APP_STARTMAGIC);
    /* @cppcheck_justify{misra-c2012-11.6} generic API requires cast */
    /* cppcheck-suppress misra-c2012-11.6 */
    EC_API_SLV_EEPROM_cbRegisterLoad            (pAppInstance_p->ptEcSlvApi, EC_SLV_APP_EEP_loadEeprom, OSPIFLASH_APP_STARTMAGIC);

    EC_API_SLV_cbRegisterUserApplicationRun     (pAppInstance_p->ptEcSlvApi, EC_SLV_APP_CIA_applicationRun, pAppInstance_p);
#endif

#if (defined GPIO_TEST_PINS) && (1==GPIO_TEST_PINS)
#if (defined GPIO_TEST_PROFILE_SEL) && (defined GPIO_TEST_PROFILE_2) && (GPIO_TEST_PROFILE_2 == GPIO_TEST_PROFILE_SEL)
    EC_API_SLV_cbRegisterMeasurement(pAppInstance_p->ptEcSlvApi, EC_SLV_APP_measurement, pAppInstance_p);
#endif
#endif

    error = (EC_API_EError_t)EC_API_SLV_init(pAppInstance_p->ptEcSlvApi);
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("Slave Init Error Code: 0x%08x\r\n", error);
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_API_SLV_run(pAppInstance_p->ptEcSlvApi);

Exit:
    return;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Main application of the slave.
 *
 *  \details
 *  This functions is called cyclically by the Beckhoff stack.
 *  CiA402 application is not located here.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pAppRunCtxt_p     Application context.
 *
 *  <!-- Group: -->
 *
 *  \ingroup CiA402
 *
 */
void EC_SLV_APP_CIA_applicationRun(void* appCtxt)
{
    OSALUNREF_PARM(appCtxt);
#if (defined GPIO_TEST_PINS) && (1==GPIO_TEST_PINS)
#if (defined GPIO_TEST_PROFILE_SEL) && (defined GPIO_TEST_PROFILE_1) && (defined GPIO_TEST_PROFILE_2) && ((GPIO_TEST_PROFILE_1 == GPIO_TEST_PROFILE_SEL) || (GPIO_TEST_PROFILE_2 == GPIO_TEST_PROFILE_SEL))
    /* @cppcheck_justify{threadsafety-threadsafety} not reentrant called */
    /* cppcheck-suppress threadsafety-threadsafety */
    static bool bToggle  = false;

    if (bToggle)
    {
        ESL_GPIO_testPins_set(ESL_TESTPIN_STATE_REG_BANK, ESL_TESTPIN_3_MASK);
    }
    else
    {
        ESL_GPIO_testPins_clear(ESL_TESTPIN_STATE_REG_BANK, ESL_TESTPIN_3_MASK);
    }
    bToggle = !bToggle;
#endif
#endif
}
