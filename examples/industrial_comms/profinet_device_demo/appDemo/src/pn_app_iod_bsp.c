/*!
 * \file pn_app_iod_bsp.c
 *
 * \brief
 * Functions and callbacks for handling the board support package like memory and LED control.
 *
 * \author
 * KUNBUS GmbH
 *
 * \copyright
 * Copyright (c) 2023, KUNBUS GmbH<br /><br />
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024 KUNBUS GmbH.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * <ol>
 * <li>Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer./<li>
 * <li>Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.</li>
 * <li>Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.</li>
 * </ol>
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "pn_app_iod_bsp.h"

#include <string.h>
#include <ti_board_open_close.h>
#include <nvm.h>

#include "osal.h"

#include "pn_api_iod_bsp.h"
#include "pn_api_iod_error.h"

/*!
 * \brief Profinet rema data entry
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
typedef struct PN_APP_IOD_RemaEntry
{
    uint8_t  *data;        /*!< Rema data address */
    uint16_t  maxlen;      /*!< Max data length */
    uint16_t *actuallen;   /*!< Data legnth */
} PN_APP_IOD_RemaEntry_t;

/*!
 * \brief Profinet rema data context
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
typedef struct PN_APP_IOD_RemaContext
{
    PN_API_IOD_Handle_t *handle;                    /*!< Device handle */
    uint32_t             bytesToWrite;              /*!< Bytes to write */
    bool                 retriggerAfterComplete;    /*!< Retrigger read/write; nvm status = busy */
} PN_APP_IOD_RemaContext_t;

/*!
 * \brief Profinet LED monitoring struct
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
typedef struct PN_APP_IOD_Led
{
    uint32_t setState;                             /*!< New state: ON, OFF, BLINK*/
    bool     actualState;                          /*!< Current state */
}PN_APP_IOD_Led_t;


/*!
 * \brief Profinet LED context
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
typedef struct PN_APP_IOD_LedContext
{
    void *threadHandle;                             /*!< Led Task handle */
    uint32_t state;                                 /*!< Current state */
    PN_APP_IOD_Led_t led[PN_API_IOD_LedMaxLeds];    /*!< Led list */
}PN_APP_IOD_LedContext_t;

/*!< Profinet application rema data context instance */
PN_APP_IOD_RemaContext_t remaContext;

/*!< Profinet application rema data instance */
PN_APP_IOD_Nvdata_t ramNvData;

/*!< Profinet application LED context instance */
PN_APP_IOD_LedContext_t ledContext;

/*!
 * \brief
 * Get LED instance
 *
 * \details
 * In this function, the application returns LED instance.
 *
 * \param[in]      led                 Led type
 *
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
static void PN_APP_IOD_getLed(PN_API_IOD_Led_t led, uint32_t *instance, uint32_t *index)
{
    *instance = CONFIG_LED_NUM_INSTANCES;
    *index    = 0;

    switch (led)
    {
        case PN_API_IOD_LedBlink:
            if (ledContext.led[PN_API_IOD_LedBlink].setState != PN_APP_IOD_LED_OFF)
            {
                // overwrite run led
                *instance = CONFIG_DCP_SIGNAL_LED;
                *index    = 0;
            }
            break;
        case PN_API_IOD_LedRun:
            if (ledContext.led[PN_API_IOD_LedBlink].setState == PN_APP_IOD_LED_OFF)
            {
                // ignore in case of blinking enabled
                *instance = CONFIG_DCP_SIGNAL_LED;
                *index    = 0;
            }
            break;
        case PN_API_IOD_LedMaint:
            break;
        case PN_API_IOD_LedError:
            *instance = CONFIG_SYS_FAILURE_LED;
            *index    = 0;
            break;
        case PN_API_IOD_LedUser00:
        case PN_API_IOD_LedUser01:
        case PN_API_IOD_LedUser02:
        case PN_API_IOD_LedUser03:
        case PN_API_IOD_LedUser04:
        case PN_API_IOD_LedUser05:
        case PN_API_IOD_LedUser06:
        case PN_API_IOD_LedUser07:
            *instance = CONFIG_CYCLIC_LEDS;
            *index    = led - PN_API_IOD_LedUser00;
            break;
        case PN_API_IOD_LedSync:
        default:
            // undefined led
            break;
    }
}

/*!
 * \brief
 * Update LED state.
 *
 * \details
 * In this function, the application updates LED states.
 *
 * \param[in]      led                 Led type
 *
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
static void PN_APP_IOD_updateLed(PN_API_IOD_Led_t led)
{
    uint32_t instance;
    uint32_t index;

    PN_APP_IOD_getLed(led, &instance, &index);

    if (instance != CONFIG_LED_NUM_INSTANCES)
    {
        bool ledEnable = false;

        if (ledContext.led[led].setState & ledContext.state)
        {
            // check if led is enabled in this state
            ledEnable = true;
        }

        if (ledContext.led[led].actualState != ledEnable)
        {
            // switch only if state has changed
            if (ledEnable)
            {
                LED_on(gLedHandle[instance], index);
            }
            else
            {
                LED_off(gLedHandle[instance], index);
            }
            ledContext.led[led].actualState = ledEnable;
        }
    }
}

/*!
 * \brief
 * Led task.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
void PN_APP_IOD_ledTask(void *arg)
{
    OSALUNREF_PARM(arg);

    while(1)
    {
        uint32_t led;
        OSAL_SCHED_sleep(PN_APP_IOD_LED_TASK_INTERVAL_MS/PN_APP_IOD_LED_TICKS);

        // increment current led global state
        ledContext.state <<= 1;
        if (ledContext.state >= (1<<PN_APP_IOD_LED_TICKS) || ledContext.state == 0)
        {
            ledContext.state = 1;
        }

        for (led = 0; led < PN_API_IOD_LedMaxLeds; led++)
        {
            PN_APP_IOD_updateLed(led);
        }
    }
}

uint32_t PN_APP_IOD_ledInit(void)
{
    uint32_t status = PN_API_OK;

    memset(&ledContext, 0, sizeof(ledContext));

    ledContext.threadHandle = OSAL_SCHED_startTask(
        PN_APP_IOD_ledTask,
        &ledContext,
        PN_APP_IOD_LED_TASK_PRIO,
        NULL,
        PN_APP_IOD_LED_TASK_STACK,
        0,
        "LED Task");

    if (ledContext.threadHandle == NULL)
    {
        status = PN_API_NOT_OK;
    }

    return status;
}

uint32_t PN_APP_IOD_cbSetLed(
    PN_API_IOD_Handle_t *const pnHandle,
    const PN_API_IOD_Led_t     led,
    const bool                 state)
{
    // OSAL_printf("Set Led request received on Led %u to %u\r\n", led, state);
    OSALUNREF_PARM(pnHandle);

    if (led < PN_API_IOD_LedMaxLeds)
    {
        if (state)
        {
            ledContext.led[led].setState = PN_APP_IOD_LED_ON;
        }
        else
        {
            ledContext.led[led].setState = PN_APP_IOD_LED_OFF;
        }
    }

    return PN_API_OK;
}

uint32_t PN_APP_IOD_cbStartLedBlink(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t             portNum,
    const uint32_t             frequency)
{
    OSAL_printf("DCP signal request received on Port %u\r\n", portNum);
    OSALUNREF_PARM(pnHandle);
    OSALUNREF_PARM(portNum);
    OSALUNREF_PARM(frequency);

    ledContext.led[PN_API_IOD_LedBlink].setState = PN_APP_IOD_LED_BLINK;
    ledContext.led[PN_API_IOD_LedBlink].actualState = ledContext.led[PN_API_IOD_LedRun].actualState;

    return PN_API_OK;
}

uint32_t PN_APP_IOD_cbStopLedBlink(PN_API_IOD_Handle_t *const pnHandle, const uint32_t portNum)
{
    OSAL_printf("DCP signal request stop received.\r\n");
    OSALUNREF_PARM(pnHandle);
    OSALUNREF_PARM(portNum);

    ledContext.led[PN_API_IOD_LedBlink].setState = PN_APP_IOD_LED_OFF;
    ledContext.led[PN_API_IOD_LedRun].actualState = ledContext.led[PN_API_IOD_LedBlink].actualState;

    return PN_API_OK;
}

uint32_t PN_APP_IOD_cbFreeRemaMem(PN_API_IOD_Handle_t *const pnHandle, uint8_t *const destMem)
{
    uint32_t status = PN_API_OK;

    OSALUNREF_PARM(pnHandle);
    OSALUNREF_PARM(destMem);

    return status;
}

/*!
 * \brief
 * Read device rema data entries
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK            Success.
 * \retval         #PN_API_NOT_OK        Something went wrong.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_getRemaEntry(
    PN_APP_IOD_RemaEntry_t       *entry,
    PN_API_IOD_RemaDataType_t type,
    const uint32_t            instance)
{
    uint32_t status = PN_API_OK;
    memset(entry, 0, sizeof(*entry));

    switch (type)
    {
        case PN_API_IOD_RemaDataDevName:
            entry->data      = ramNvData.devName;
            entry->maxlen    = sizeof(ramNvData.devName);
            entry->actuallen = &ramNvData.devNameLen;
            break;
        case PN_API_IOD_RemaDataIpSuite:
            entry->data      = (uint8_t *)&ramNvData.ipSuite;
            entry->maxlen    = sizeof(ramNvData.ipSuite);
            entry->actuallen = &ramNvData.ipSuiteLen;
            break;
        case PN_API_IOD_RemaDataIm1:
            if (instance < PN_API_IOD_MAX_NUM_OF_SUBSLOTS)
            {
                entry->data      = (uint8_t *)&ramNvData.im1[instance];
                entry->maxlen    = sizeof(ramNvData.im1[instance]);
                entry->actuallen = &entry->maxlen;
            }
            break;
        case PN_API_IOD_RemaDataIm2:
            if (instance < PN_API_IOD_MAX_NUM_OF_SUBSLOTS)
            {
                entry->data      = (uint8_t *)&ramNvData.im2[instance];
                entry->maxlen    = sizeof(ramNvData.im2[instance]);
                entry->actuallen = &entry->maxlen;
            }
            break;
        case PN_API_IOD_RemaDataIm3:
            if (instance < PN_API_IOD_MAX_NUM_OF_SUBSLOTS)
            {
                entry->data      = (uint8_t *)&ramNvData.im3[instance];
                entry->maxlen    = sizeof(ramNvData.im3[instance]);
                entry->actuallen = &entry->maxlen;
            }
            break;
        case PN_API_IOD_RemaDataIm4:
            if (instance < PN_API_IOD_MAX_NUM_OF_SUBSLOTS)
            {
                entry->data      = (uint8_t *)&ramNvData.im4[instance];
                entry->maxlen    = sizeof(ramNvData.im4[instance]);
                entry->actuallen = &entry->maxlen;
            }
            break;
        case PN_API_IOD_RemaDataSnmpSysName:
            entry->data      = ramNvData.snmpSysName;
            entry->maxlen    = sizeof(ramNvData.snmpSysName);
            entry->actuallen = &ramNvData.snmpSysNameLen;
            break;
        case PN_API_IOD_RemaDataSnmpSysLoc:
            entry->data      = ramNvData.snmpSysLoc;
            entry->maxlen    = sizeof(ramNvData.snmpSysLoc);
            entry->actuallen = &ramNvData.snmpSysLocLen;
            break;
        case PN_API_IOD_RemaDataSnmpSysCont:
            entry->data      = ramNvData.snmpSysCont;
            entry->maxlen    = sizeof(ramNvData.snmpSysCont);
            entry->actuallen = &ramNvData.snmpSysContLen;
            break;
        case PN_API_IOD_RemaDataPdevRecord:
            entry->data      = ramNvData.pdev;
            entry->maxlen    = sizeof(ramNvData.pdev);
            entry->actuallen = &ramNvData.pdevLen;
            break;
        case PN_API_IOD_RemaDataArFsu:
            entry->data      = ramNvData.arfsu;
            entry->maxlen    = sizeof(ramNvData.arfsu);
            entry->actuallen = &ramNvData.arfsuLen;
            break;
        case PN_API_IOD_RemaDataSubmodCfg:
            entry->data      = (uint8_t *)&ramNvData.subCfgList[instance];
            entry->maxlen    = sizeof(ramNvData.subCfgList);
            entry->actuallen = &ramNvData.subCfgLen;
            break;
        default:
            // unknown data
            break;
    }

    if (entry->data == NULL || entry->actuallen == NULL)
    {
        status = PN_API_NOT_OK;
    }
    else if (*entry->actuallen == 0xFFFF)
    {
        // memory not initialized
        *entry->actuallen = 0;
    }

    return status;
}


/*!
 * \brief
 * Generate checksum for rema data
 *
 * \return         result of the operation as uint32_t.
 * \retval         #checksum        Rema data checksum.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_remaChecksum(void)
{
    uint32_t const *data     = ((uint32_t *)&ramNvData) + 1; // ignore crc
    uint32_t        length   = sizeof(ramNvData) / sizeof(uint32_t) - 1;
    uint32_t        checksum = PN_APP_IOD_BSP_CHECKSUM_SEED;

    while (length--)
    {
        checksum ^= *data;
        checksum = (checksum << 1) | (checksum >> 31); // rotation
        data++;
    }

    return checksum;
}

/*!
 * \brief
 * Trigger storage of rema data
 *
 * \details
 * In this function the data is written in the NVram, the write is retriggered in case of
 * multiple store requests
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
static uint32_t PN_APP_IOD_remaTriggerStore(void)
{
    uint32_t status = PN_API_OK;
    uint32_t nvmstatus;

    ramNvData.checkSum = PN_APP_IOD_remaChecksum();

    nvmstatus = NVM_APP_writeAsync(
        PN_APP_IOD_NVM_TYPE,
        PN_APP_IOD_NVM_INSTANCE,
        PN_APP_IOD_NVM_OFFSET,
        sizeof(ramNvData),
        (uint8_t *)&ramNvData);

    if (nvmstatus == NVM_ERR_BUSY)
    {
        //OSAL_printf("Multiple fast store requests detected.\r\n");
        remaContext.retriggerAfterComplete = true;
    }
    else if (nvmstatus != NVM_ERR_SUCCESS)
    {
        status = PN_API_NOT_OK;
    }

    return status;
}

/*!
 * \brief
 * Callback function to store rema data
 *
 * \param[in]      status   Indicates the write task status
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
void PN_APP_IOD_remaNvmCallback(uint32_t status)
{
    OSALUNREF_PARM(status);

    if (remaContext.handle)
    {
        PN_API_IOD_dataStoreComplete(remaContext.handle, remaContext.bytesToWrite);
    }

    if (remaContext.retriggerAfterComplete)
    {
        remaContext.retriggerAfterComplete = false;
        PN_APP_IOD_remaTriggerStore();
    }
}

uint32_t PN_APP_IOD_remaInit(void)
{
    uint32_t nvmstatus;
    uint32_t status = PN_API_OK;

    memset(&remaContext, 0, sizeof(remaContext));

    nvmstatus = NVM_APP_init(OSAL_TASK_Prio_2);

    if (nvmstatus == NVM_ERR_SUCCESS)
    {
        nvmstatus = NVM_APP_read(
            PN_APP_IOD_NVM_TYPE,
            PN_APP_IOD_NVM_INSTANCE,
            PN_APP_IOD_NVM_OFFSET,
            sizeof(ramNvData),
            (uint8_t *)&ramNvData);
    }

    if (nvmstatus == NVM_ERR_SUCCESS)
    {
        nvmstatus = NVM_APP_registerCallback(PN_APP_IOD_remaNvmCallback);
    }

    if (nvmstatus != NVM_ERR_SUCCESS)
    {
        status = PN_API_NOT_OK;
    }
    else if (
        ramNvData.checkSum != PN_APP_IOD_remaChecksum()
        || ramNvData.version != PN_APP_IOD_NV_STRUCT_VERSION)
    {
        memset(&ramNvData, 0, sizeof(ramNvData));

        /* IMx data need to be initialized to white spaces not zeros */
        memset(&ramNvData.im1, ' ', (uint32_t)&ramNvData.snmpSysNameLen - (uint32_t)&ramNvData.im1);

        ramNvData.version = PN_APP_IOD_NV_STRUCT_VERSION;
    }

    return status;
}

uint32_t PN_APP_IOD_cbStoreRemaMem(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_RemaDataType_t  type,
    const uint32_t             instance,
    const uint32_t             memSize,
    uint8_t *const             srcMem)
{
    uint32_t           status = PN_API_OK;
    PN_APP_IOD_RemaEntry_t entry;

    //OSAL_printf("Save remanent memory indicated (%u, %u).\r\n", type, instance);

    PN_APP_IOD_getRemaEntry(&entry, type, instance);

    if (srcMem == NULL || entry.actuallen == NULL || entry.maxlen < memSize)
    {
        // not enough space
        status = PN_API_NOT_OK;
    }
    else
    {
        memcpy(entry.data, srcMem, memSize);
        *entry.actuallen = (uint16_t)memSize;

        remaContext.handle       = pnHandle;
        remaContext.bytesToWrite = memSize;
        status                   = PN_APP_IOD_remaTriggerStore();
    }

    return status;
}

uint32_t PN_APP_IOD_cbRestoreRemaMem(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_RemaDataType_t  type,
    const uint32_t             instance,
    uint8_t **const            destMem,
    uint32_t *const            memSize)
{
    uint32_t           status = PN_API_OK;
    PN_APP_IOD_RemaEntry_t entry;

    OSAL_printf("Restore remanent memory indicated.\r\n");

    PN_APP_IOD_getRemaEntry(&entry, type, instance);

    OSALUNREF_PARM(pnHandle);

    if (destMem == NULL || memSize == NULL || entry.actuallen == NULL || *entry.actuallen == 0)
    {
        // no data available
        status = PN_API_NOT_OK;
    }
    else
    {
        *destMem = entry.data;
        *memSize = *entry.actuallen;
    }

    return status;
}

uint32_t PN_APP_IOD_factoryResetRemaMem(
    PN_API_IOD_Handle_t         *pnHandle,
    const PN_API_IOD_RtfOption_t rtfOption)
{
    uint32_t status = PN_API_OK;
    OSAL_printf("Factory Reset memory indicated.\r\n");

    OSALUNREF_PARM(pnHandle);

    switch (rtfOption)
    {
        case PN_API_IOD_RtfResetAppParam:
        case PN_API_IOD_RtfResetEngParam:
        case PN_API_IOD_RtfResetFwUpgradeParam:
            // delegate to responsible modules
            break;
        case PN_API_IOD_RtfResetCommParam:
            // delete everything but Comm Param
            memset(&ramNvData, 0, (uint32_t)&ramNvData.im1 - (uint32_t)&ramNvData);
            memset(
                &ramNvData.snmpSysNameLen,
                0,
                (uint32_t)(&ramNvData + 1) - (uint32_t)&ramNvData.snmpSysNameLen);
            ramNvData.version = PN_APP_IOD_NV_STRUCT_VERSION;
            PN_APP_IOD_remaTriggerStore();
            break;
        case PN_API_IOD_RtfResetAll:
        default:
            memset(&ramNvData, 0, sizeof(ramNvData));

            /* IMx data need to be reset to white spaces not zeros */
            memset(&ramNvData.im1, ' ', (uint32_t)&ramNvData.snmpSysNameLen - (uint32_t)&ramNvData.im1);

            ramNvData.version = PN_APP_IOD_NV_STRUCT_VERSION;
            PN_APP_IOD_remaTriggerStore();
            break;
    }
    return status;
}

uint32_t PN_APP_IOD_updateAppCycle(PN_API_IOD_Handle_t *pnHandle, uint32_t timeNs)
{
    uint32_t      baseAddr = gTimerBaseAddr[CONFIG_TIMER1];
    TimerP_Params timerParams;

    OSALUNREF_PARM(pnHandle);

    TimerP_stop(baseAddr);

    TimerP_Params_init(&timerParams);
    timerParams.inputPreScaler    = CONFIG_TIMER1_INPUT_PRE_SCALER;
    timerParams.inputClkHz        = CONFIG_TIMER1_INPUT_CLK_HZ;
    timerParams.periodInNsec      = timeNs;
    timerParams.oneshotMode       = 0U;
    timerParams.enableOverflowInt = 1U;
    timerParams.enableDmaTrigger  = 0U;
    TimerP_setup(baseAddr, &timerParams);
    HwiP_clearInt(CONFIG_TIMER1_INT_NUM);

    TimerP_start(baseAddr);

    return PN_API_OK;
}
