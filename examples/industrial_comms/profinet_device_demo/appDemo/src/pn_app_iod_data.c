/*!
 * \file pn_app_iod_data.c
 *
 * \brief
 * Functions and callbacks for handling Profinet data.
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

#include "pn_app_iod_data.h"

#include <stdbool.h>
#include <string.h>

#include "osal.h"

#include "pn_api_iod_bsp.h"
#include "pn_api_iod_callbacks.h"
#include "pn_api_iod_data.h"
#include "pn_api_iod_error.h"
#include "pn_app_iod_bsp.h"

#define PN_APP_CYCLIC_TASK_PRIO                   29
#define PN_APP_CYCLIC_TASK_STACK_SIZE             2048

static void* PN_APP_cyclicDataHandle;
uint8_t PN_APP_cyclicDataTaskStack[PN_APP_CYCLIC_TASK_STACK_SIZE]__attribute__((aligned(32), section(".threadstack")));

extern OSAL_SCHED_EventHandle_t *dataExchangeEvent; /* DO NOT CHANGE! (used by Profinet stack) */
uint8_t inDataCounter = 0;

/*! \ingroup PN_API_IOD_DATA_DOXY_GROUP
 * @{
 */
uint8_t demoOutData[PN_API_IOD_MAX_NUM_OF_SLOTS][PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1]
                   [PN_API_IOD_NUM_OF_BYTES_PER_SUBSLOT];                /*!< Output data store for all submodules. */
uint8_t demoOutDataIocs[PN_API_IOD_MAX_NUM_OF_SLOTS]
                       [PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];    /*!< IOCS for output data of
                                                                            each submodule. */
uint8_t demoOutDataIops[PN_API_IOD_MAX_NUM_OF_SLOTS]
                       [PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];    /*!< IOPS for output data of
                                                                            each submodule. */
uint8_t demoOutDataIopsOld[PN_API_IOD_MAX_NUM_OF_SLOTS]
                          [PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1]; /*!< Old IOPS for output data
                                                                            of each submodule. */
uint16_t demoOutDataLen[PN_API_IOD_MAX_NUM_OF_SLOTS]
                       [PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];    /*!< Output data length for
                                                                            each submodule. */

uint8_t demoInData[PN_API_IOD_MAX_NUM_OF_SLOTS][PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1]
                  [PN_API_IOD_NUM_OF_BYTES_PER_SUBSLOT];                 /*!< Input data store for all submodules. */
uint8_t demoInDataIocs[PN_API_IOD_MAX_NUM_OF_SLOTS]
                      [PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];     /*!< IOCS for input data of
                                                                            each submodule. */
uint8_t demoInDataIops[PN_API_IOD_MAX_NUM_OF_SLOTS]
                      [PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];     /*!< IOPS for input data of
                                                                            each submodule. */
uint8_t demoInDataIocsOld[PN_API_IOD_MAX_NUM_OF_SLOTS]
                         [PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];  /*!< Old IOCS for input data
                                                                            of each submodule. */
uint16_t demoInDataLen[PN_API_IOD_MAX_NUM_OF_SLOTS]
                      [PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];     /*!< Input data length for each
                                                                            submodule. */
/** @}*/

#ifdef UNIT_TEST
extern int FOREVER(void);
#else
#define FOREVER()                                 1
#endif

void PN_APP_IOD_cbArInData(
    PN_API_IOD_Handle_t* const pnHandle,
    const uint16_t arNum,
    const uint16_t sessionKey)
{
    OSALUNREF_PARM(pnHandle);

    OSAL_printf("AR in-data event indication received: AR = %d, session key = %d\r\n",
        arNum, sessionKey);
}

PN_API_IOD_IOXS PN_APP_IOD_cbDataWrite(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const uint32_t bufLen,
    uint8_t *const buffer,
    const PN_API_IOD_IOXS remoteIocs)
{
    OSALUNREF_PARM(pnHandle);

    PN_API_IOD_IOXS status = PN_API_IOD_STATUS_BAD;

    if (NULL != addr)
    {
        uint32_t slotIndex    = addr->address.geo.slot;
        uint32_t subslotIndex = addr->address.geo.subslot;

        subslotIndex = PN_APP_IOD_getSubslotIndex(slotIndex, subslotIndex);

        if ((PN_API_IOD_MAX_NUM_OF_SLOTS > slotIndex) &&
            (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT >= subslotIndex) &&
            (0 < bufLen) &&
            (NULL != buffer))
        {
            OSAL_MEMORY_memcpy((void*)buffer, &demoInData[slotIndex][subslotIndex][0], bufLen);

            demoInDataIops[slotIndex][subslotIndex] = PN_API_IOD_STATUS_GOOD;

            /* Consumer status (of remote IO controller) */
            demoInDataIocs[slotIndex][subslotIndex] = remoteIocs;

            /* Verify and notify a new consumer status */
            if (demoInDataIocsOld[slotIndex][subslotIndex] != remoteIocs)
            {
                OSAL_printf("New IO controller Input Consumer Status (ICS) = 0x%x in slot %d, subslot %d\r\n",
                    remoteIocs, slotIndex, subslotIndex);
                demoInDataIocsOld[slotIndex][subslotIndex] = remoteIocs;
            }

            /* Increase input data counter to send a new value next time */
            inDataCounter++;
            demoInData[slotIndex][subslotIndex][0] = inDataCounter;
        }

        status = demoInDataIops[slotIndex][subslotIndex];
    }

    return status;
}

PN_API_IOD_IOXS PN_APP_IOD_cbDataRead(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const uint32_t bufLen,
    uint8_t *const buffer,
    const PN_API_IOD_IOXS remoteIops)
{
    PN_API_IOD_IOXS status = PN_API_IOD_STATUS_BAD;

    if (NULL != addr)
    {
        uint32_t slotIndex    = addr->address.geo.slot;
        uint32_t subslotIndex = addr->address.geo.subslot;

        subslotIndex = PN_APP_IOD_getSubslotIndex(slotIndex, subslotIndex);

        if ((PN_API_IOD_MAX_NUM_OF_SLOTS > slotIndex) &&
            (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT >= subslotIndex) &&
            (0 < bufLen) &&
            (NULL != buffer))
        {
            OSAL_MEMORY_memcpy(&demoOutData[slotIndex][subslotIndex][0], buffer, bufLen);

            /* Provider status (of remote IO controller) */
            demoOutDataIops[slotIndex][subslotIndex] = remoteIops;

            /* Verify and notify a new provider status */
            if (demoOutDataIopsOld[slotIndex][subslotIndex] != remoteIops)
            {
                OSAL_printf("New IO controller Output Provider Status (OPS) = 0x%x in slot %d, subslot %d\r\n",
                    remoteIops, slotIndex, subslotIndex);

                demoOutDataIopsOld[slotIndex][subslotIndex] = remoteIops;
            }

            demoOutDataIocs[slotIndex][subslotIndex] = PN_API_IOD_STATUS_GOOD;

            status = demoOutDataIocs[slotIndex][subslotIndex];

            /* Use the received output value to set LED States */
            for (int8_t led = 0; led < 8; led++)
            {
                bool state = (demoOutData[slotIndex][subslotIndex][0] >> led) & 0x01;
                PN_APP_IOD_cbSetLed(pnHandle, PN_API_IOD_LedUser00 + led, state);
            }
        }
    }

    return status;
}

PN_API_IOD_IOXS PN_APP_IOD_cbDataWriteIOxSOnly(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const PN_API_IOD_IOXS remoteIocs)
{
    OSALUNREF_PARM(pnHandle);

    PN_API_IOD_IOXS status = PN_API_IOD_STATUS_BAD;

    if (NULL != addr)
    {
        uint32_t slotIndex    = addr->address.geo.slot;
        uint32_t subslotIndex = addr->address.geo.subslot;

        subslotIndex = PN_APP_IOD_getSubslotIndex(slotIndex, subslotIndex);

        if ((PN_API_IOD_MAX_NUM_OF_SLOTS > slotIndex) &&
            (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT >= subslotIndex))
        {
            /* Consumer status (of remote IO controller) */
            demoInDataIocs[slotIndex][subslotIndex] = remoteIocs;

            /* Verify and notify the new provider status */
            if (demoInDataIocsOld[slotIndex][subslotIndex] != remoteIocs)
            {
                OSAL_printf("New IO controller Input Consumer Status (ICS) = 0x%x in slot %d, subslot %d\r\n",
                    remoteIocs, slotIndex, subslotIndex);
                demoInDataIocsOld[slotIndex][subslotIndex] = remoteIocs;
            }

            status = (PN_API_IOD_IOXS)demoInDataIops[slotIndex][subslotIndex];
        }
    }

    return status;
}

PN_API_IOD_IOXS PN_APP_IOD_cbDataReadIOxSOnly(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const PN_API_IOD_IOXS remoteIops)
{
    OSALUNREF_PARM(pnHandle);

    PN_API_IOD_IOXS status = PN_API_IOD_STATUS_BAD;

    if (NULL != addr)
    {
        uint32_t slotIndex    = addr->address.geo.slot;
        uint32_t subslotIndex = addr->address.geo.subslot;

        subslotIndex = PN_APP_IOD_getSubslotIndex(slotIndex, subslotIndex);

        if ((PN_API_IOD_MAX_NUM_OF_SLOTS > slotIndex) &&
            (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT >= subslotIndex))
        {
            /* Provider status (of remote IO controller) */
            demoOutDataIops[slotIndex][subslotIndex] = remoteIops;

            /* Verify and notify a new provider status */
            if (demoOutDataIopsOld[slotIndex][subslotIndex] != remoteIops)
            {
                OSAL_printf("New IO controller Output Provider Status (OPS) = 0x%x in slot %d, subslot %d\r\n",
                    remoteIops, slotIndex, subslotIndex);
                demoOutDataIopsOld[slotIndex][subslotIndex] = remoteIops;
            }

            status = (PN_API_IOD_IOXS)demoOutDataIocs[slotIndex][subslotIndex];
        }
    }

    return status;
}

PN_API_IOD_IOXS PN_APP_IOD_cbOutSubmodSubstValRead(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_DevAddr_t *const addr,
    const uint32_t bufLen,
    uint8_t *const buffer,
    uint16_t *const substMode,
    uint16_t *const substActive)
{
    OSALUNREF_PARM(pnHandle);

    uint32_t slotIndex;
    uint32_t subslotIndex;
    PN_API_IOD_IOXS status = PN_API_IOD_STATUS_BAD;

    if ((0 < bufLen) && (NULL != buffer) && (NULL != addr) && (NULL != substMode) && (NULL != substActive))
    {
        slotIndex = addr->address.geo.slot;
        subslotIndex = addr->address.geo.subslot;
        subslotIndex = PN_APP_IOD_getSubslotIndex(slotIndex, subslotIndex);

        if (PN_API_IOD_STATUS_GOOD == demoOutDataIocs[slotIndex][subslotIndex])
        {
            OSAL_MEMORY_memcpy(buffer, &demoOutData[slotIndex][subslotIndex][0], bufLen);

            /* Last output value received from controller is used */
            *substMode = PN_API_IOD_SUBST_LAST_OUT;

            /* Normal operation */
            *substActive = PN_API_IOD_SUBST_NORMAL;
        }
        else
        {
            OSAL_MEMORY_memset(buffer, 0x5a, bufLen); // E.g. substitute value

            /* Replacement value decided by the application is used */
            *substMode = PN_API_IOD_SUBST_REPLACEMENT;

            /* Substitute value active (IO-CS) not GOOD */
            *substActive = PN_API_IOD_SUBST_ACTIVE;
        }

        status = PN_API_IOD_STATUS_GOOD;
    }

    return status;
}

uint32_t PN_APP_IOD_getSubslotIndex(uint32_t slot, uint32_t subslot)
{
    uint32_t tmpSubslot;

    if (0 < slot)
    {
        tmpSubslot = subslot;
    }
    else
    {
        switch (subslot)
        {
            case 0x0001:
                tmpSubslot = 1;
                break;
            case 0x8000:
                tmpSubslot = 2;
                break;
            case 0x8001:
                tmpSubslot = 3;
                break;
#if (PN_API_IOD_PDEV_NUM_OF_PORTS >= 2)
            case 0x8002:
                tmpSubslot = 4;
                break;
#endif
#if (PN_API_IOD_PDEV_NUM_OF_PORTS >= 3)
            case 0x8003:
                tmpSubslot = 5;
                break;
#endif
#if (PN_API_IOD_PDEV_NUM_OF_PORTS >= 4)
            case 0x8004:
                tmpSubslot = 6;
                break;
#endif
            default:
                tmpSubslot = subslot;
                break;
        }
    }

    return tmpSubslot;
}

void PN_APP_IOD_setIops(uint32_t slot, uint32_t subslot, PN_API_IOD_IOXS state)
{
    uint32_t subslotIndex = 0;
    subslotIndex = PN_APP_IOD_getSubslotIndex(slot, subslot);

    if ((PN_API_IOD_MAX_NUM_OF_SLOTS > slot) &&
        (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT >= subslotIndex))
    {
        demoInDataIops[slot][subslotIndex] = state;
    }
}

void PN_APP_IOD_initAppData(void)
{
    OSAL_MEMORY_memset(demoOutData, 0, sizeof(demoOutData));
    OSAL_MEMORY_memset(demoOutDataIocs, PN_API_IOD_STATUS_GOOD, sizeof(demoOutDataIocs));
    OSAL_MEMORY_memset(demoOutDataIops, PN_API_IOD_STATUS_GOOD, sizeof(demoOutDataIops));
    OSAL_MEMORY_memset(demoOutDataIopsOld, PN_API_IOD_STATUS_GOOD, sizeof(demoOutDataIopsOld));

    OSAL_MEMORY_memset(demoInData, inDataCounter, sizeof(demoInData));
    OSAL_MEMORY_memset(demoInDataIocs, PN_API_IOD_STATUS_GOOD, sizeof(demoInDataIocs));
    OSAL_MEMORY_memset(demoInDataIops, PN_API_IOD_STATUS_GOOD, sizeof(demoInDataIops));
    OSAL_MEMORY_memset(demoInDataIocsOld, PN_API_IOD_STATUS_GOOD, sizeof(demoInDataIocsOld));
}

static void PN_APP_IOD_cyclicDataTask(void* pvTaskArg)
{
    PN_API_IOD_Handle_t *pnHandle = (PN_API_IOD_Handle_t *) pvTaskArg;

    uint32_t status = PN_API_NOT_OK;

    while (FOREVER())
    {
        uint8_t isTimeOut = false;

        /* Wait for Profinet stack to trigger dataExchangeEvent. This happens when the stack is
         * (1) done receiving a valid output data frame from IO controller, (2) done sending a valid
         * input data frame to the IO controller, (3) requested to do one-time acyclic exchange of
         * up-to-date IO data (e.g. in response to the special "IO data" record read received from
         * IO controller with index 0x8028 or 0x8029).
         * User application is also required to trigger this event once a "Ready for input update"
         * call is received from Profinet stack (see PN_APP_IOD_cbReadyForInputUpdate)
         */
        status = OSAL_EVT_wait(dataExchangeEvent, OSAL_WAIT_INFINITE, &isTimeOut);

        /* Note: In order to keep IO data always up to date, it's recommended to initiate BOTH data
         * read & write every time this event is triggered. Even when the triggering action is caused
         * by only one reason of the three reasons stated above.
         */
        if (PN_API_OK == status)
        {
            PN_API_IOD_initiateDataRead(pnHandle);

            PN_API_IOD_initiateDataWrite(pnHandle);
        }
    }
}

uint32_t PN_APP_IOD_initCyclicDataExchange(PN_API_IOD_Handle_t* const pnHandle)
{
    uint32_t status = PN_API_OK;

    if (NULL == PN_APP_cyclicDataHandle)
    {
        PN_APP_cyclicDataHandle = OSAL_SCHED_startTask(
            PN_APP_IOD_cyclicDataTask,
            (void *)pnHandle,
            PN_APP_CYCLIC_TASK_PRIO,
            PN_APP_cyclicDataTaskStack,
            sizeof(PN_APP_cyclicDataTaskStack),
            OSAL_OS_START_TASK_FLG_NONE,
            "app_cyclic");

        if (NULL != PN_APP_cyclicDataHandle)
        {
            /* Initialize the synchronization event for the IO data exchange
             * between Profinet stack and user application */
            dataExchangeEvent = OSAL_EVTCTRLBLK_alloc();
            OSAL_EVT_init(dataExchangeEvent);
        }
        else
        {
            status = PN_API_NOT_OK;
        }
    }

    return status;
}
