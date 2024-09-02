/*!
 * \file pn_app_iod_device.c
 *
 * \brief
 * Functions and callbacks for handling the configuration of the IO device.
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

#include "pn_app_iod_device.h"

#include <stdbool.h>
#include <string.h>

#include "osal.h"

#include "pn_api_iod_callbacks.h"
#include "pn_api_iod_error.h"
#include "pn_api_iod_handle.h"
#include "pn_api_iod_startup.h"
#include "pn_api_iod_types.h"

#include "pn_app_iod_data.h"
#include "pn_app_iod_mod_cfg.h"
#include "pn_app_iod_bsp.h"

extern uint16_t demoOutDataLen[PN_API_IOD_MAX_NUM_OF_SLOTS][PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];
extern uint16_t demoInDataLen[PN_API_IOD_MAX_NUM_OF_SLOTS][PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT + 1];

uint8_t PN_APP_IOD_getIm0SupportInfo(
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    uint32_t modId,
    uint32_t submodId)
{
    uint8_t im0Support = PN_API_IOD_Im0Nothing;

    OSALUNREF_PARM(api);
    OSALUNREF_PARM(modId);
    OSALUNREF_PARM(submodId);

    if(slot > 0)
    {
        switch (subslot)
        {
            case 1U:
            {
                im0Support = (PN_API_IOD_Im0Submodule | PN_API_IOD_Im0Module);
                break;
            }
            default:
            {
                im0Support = PN_API_IOD_Im0Submodule;
                break;
            }
        }
    }
    else
    {
        switch (subslot)
        {
            case 1U:
            {
                im0Support = (PN_API_IOD_Im0Submodule | PN_API_IOD_Im0Device);
                break;
            }
            case 0x8000U:
            case 0x8001U:
            case 0x8002U:
            {
                im0Support = PN_API_IOD_Im0Submodule;
                break;
            }
            default:
            {
                // do nothing
                break;
            }
        }
    }

    return im0Support;
}

uint32_t PN_APP_IOD_buildIm0Data(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    PN_API_IOD_Im0Data_t *im0Data)
{
    uint32_t status = PN_API_ERR_PARAM;
    uint32_t subslotIndex;
    uint32_t stringSize = 0;

    OSALUNREF_PARM(api);

    subslotIndex = PN_APP_IOD_getSubslotIndex(slot, subslot);

    if (PN_API_IOD_isHandleValid(pnHandle) && (NULL != im0Data)
        && (PN_API_IOD_MAX_NUM_OF_SLOTS > slot)
        && (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT >= subslotIndex))
    {
        status = PN_API_OK;

        OSAL_MEMORY_memset(im0Data, 0, sizeof(PN_API_IOD_Im0Data_t));

        im0Data->vendorId = PN_API_IOD_VENDOR_ID;

        stringSize = PN_API_IOD_DEV_ANNOTATION_ORDER_ID_SIZE;
        if (PN_API_IOD_IM0_ORDER_ID_LEN < stringSize)
        {
            stringSize = PN_API_IOD_IM0_ORDER_ID_LEN;
        }
        OSAL_MEMORY_memset(&im0Data->orderId[0], ' ', PN_API_IOD_IM0_ORDER_ID_LEN);
        OSAL_MEMORY_memcpy(&im0Data->orderId[0], PN_API_IOD_DEV_ANNOTATION_ORDER_ID, stringSize);

        stringSize = PN_API_IOD_IM0_SERIAL_NUM_SIZE;
        if (PN_API_IOD_IM0_SERIAL_NUM_LEN < stringSize)
        {
            stringSize = PN_API_IOD_IM0_SERIAL_NUM_LEN;
        }
        OSAL_MEMORY_memset(&im0Data->serialNum[0], ' ', PN_API_IOD_IM0_SERIAL_NUM_LEN);
        OSAL_MEMORY_memcpy(&im0Data->serialNum[0], PN_API_IOD_IM0_SERIAL_NUM, stringSize);

        im0Data->hwRevision = PN_API_IOD_HW_REV;
        im0Data->swRevision.prefix = PN_API_IOD_VER_PREFIX;
        im0Data->swRevision.funcEnhancement = PN_API_IOD_VER_HH;
        im0Data->swRevision.bugFix = PN_API_IOD_VER_H;
        im0Data->swRevision.internalChange = PN_API_IOD_VER_L;
        im0Data->revisionCnt = PN_API_IOD_VER_LL;
        im0Data->profileId = 0x00;
        im0Data->versionMajor = 0x01;
        im0Data->versionMinor = 0x01;
#if PN_API_IOD_INCLUDE_IM5
        im0Data->imXSupported = PN_API_IOD_SUPPORT_IM12345;
#else
        im0Data->imXSupported = PN_API_IOD_SUPPORT_IM1234;
#endif

        if (0 < slot) /* IO module */
        {
            im0Data->profileSpecType = 0x03;

            if (1 < subslot) /* IO submodule */
            {
                im0Data->imXSupported = PN_API_IOD_SUPPORT_IM1234;
            }
        }
        else /* Interface module */
        {
            im0Data->profileSpecType = 0x05;
        }
    }

    return status;
}

 /*!
 * \brief
 * Convert remanent submodule config info into IO submodule list entry.
 *
 * \details
 * This function simply copies the values of remanent submodule config structure
 * into an IO submodule list entry and fills up the IM0 support value.
 *
 * \param[out]      ioSubmodListEntry           IO submodule list entry.
 * \param[in]       submodCfg                   Remanent submodule config info.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 *
 */
static void PN_APP_IOD_remaSubmodCfgToSubmodList(
    PN_API_IOD_SubmodListEntry_t *ioSubmodListEntry,
    PN_API_IOD_RemaDataSubmodCfgInfo_t *submodCfg)
{
    uint8_t im0Support;

    if ((ioSubmodListEntry != NULL) && (submodCfg != NULL))
    {
        ioSubmodListEntry->api   = submodCfg->api;
        ioSubmodListEntry->slot  = submodCfg->slot;
        ioSubmodListEntry->subslot = submodCfg->subslot;
        ioSubmodListEntry->modId   = submodCfg->modId;
        ioSubmodListEntry->submodId = submodCfg->submodId;
        ioSubmodListEntry->inDataLen = submodCfg->inDataLen;
        ioSubmodListEntry->outDataLen = submodCfg->outDataLen;

        im0Support = PN_APP_IOD_getIm0SupportInfo(
            submodCfg->api,
            submodCfg->slot,
            submodCfg->subslot,
            submodCfg->modId,
            submodCfg->submodId);

        ioSubmodListEntry->im0Support = im0Support;
    }
}

static uint32_t PN_APP_IOD_validateModLocation(
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    uint32_t modId,
    uint32_t submodId)
{
    PN_APP_IOD_ModPropEntry_t modProp;
    uint32_t status = PN_API_ERR_PARAM;

    OSALUNREF_PARM(api);
    OSALUNREF_PARM(submodId);

    OSAL_MEMORY_memset(&modProp, 0, sizeof(modProp));

    status = PN_APP_IOD_getModProp(modId, &modProp);

    /* Evaluate module/slot compatibility */
    if (status == PN_API_OK)
    {
        uint32_t cnt = 0;
        status = PN_API_ERR_INVALID_SLOT;

        while ((status != PN_API_OK) && (cnt < modProp.slotList.cnt))
        {
            if (slot == modProp.slotList.slots[cnt])
            {
                status = PN_API_OK;
            }

            cnt++;
        }
    }

    /* Evaluate module/subslot compatibility */
    if (status == PN_API_OK)
    {
        uint32_t cnt = 0;
        status = PN_API_ERR_INVALID_SUBSLOT;

        while ((status != PN_API_OK) && (cnt < modProp.subslotList.cnt))
        {
            if (subslot == modProp.subslotList.subslots[cnt])
            {
                status = PN_API_OK;
            }

            cnt++;
        }
    }

    return status;
}

static uint32_t PN_APP_IOD_validateSubmodLocation(
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    uint32_t modId,
    uint32_t submodId)
{
    uint32_t tmpSubmodId = 0U;
    uint32_t status = PN_API_OK;

    OSALUNREF_PARM(api);
    OSALUNREF_PARM(modId);

    if (slot > 0U)
    {
        switch (submodId)
        {
            case PN_CFG_IDS_IN_IF_ID:
            case PN_CFG_IDS_OUT_IF_ID:
            case PN_CFG_IDS_IO_IF_ID:
            {
                if (subslot != 1U)
                {
                    status = PN_API_ERR_INVALID_SUBSLOT;
                }
                break;
            }
            default:
            {
                if (subslot == 1U)
                {
                    status = PN_API_ERR_INVALID_SUBSLOT;
                }
                break;
            }
        }
    }
    else
    {
        switch (subslot)
        {
            case 1:
            {
                tmpSubmodId = PN_CFG_IDD_DAP_SUB_ID;
                break;
            }
            case 0x8000:
            {
                tmpSubmodId = PN_CFG_IDD_DAP_IF_ID;
                break;
            }
            case 0x8001:
            {
                tmpSubmodId = PN_CFG_IDD_DAP_P1_ID;
                break;
            }
#if (PN_API_IOD_PDEV_NUM_OF_PORTS >= 2)
            case 0x8002:
            {
                tmpSubmodId = PN_CFG_IDD_DAP_P2_ID;
                break;
            }
#endif
            default:
            {
                // do nothing
                break;
            }
        }

        if (tmpSubmodId != submodId)
        {
            status = PN_API_ERR_INVALID_SUBSLOT;
        }
    }

    return status;
}

static uint32_t PN_APP_IOD_validateSubmodCompatibility(
    uint32_t api,
    uint32_t modId,
    uint32_t submodId)
{
    PN_APP_IOD_SubmodCfgEntry_t submodCfg;
    uint32_t status = PN_API_ERR_PARAM;

    OSAL_MEMORY_memset(&submodCfg, 0, sizeof(submodCfg));

    status = PN_APP_IOD_getSubmodCfg(api, submodId, &submodCfg);

    /* Evaluate module/submodule compatibility */
    if (status == PN_API_OK)
    {
        uint32_t cnt = 0;
        status = PN_API_ERR_INVALID_SUBMODULE;

        while ((status != PN_API_OK) && (cnt < submodCfg.modList.cnt)
               && (cnt < PN_API_IOD_MAX_NUM_OF_SLOTS))
        {
            /* Does the user submodule config contain the given module in its module list? */
            if (modId == submodCfg.modList.modIds[cnt])
            {
                status = PN_API_OK;
            }

            cnt++;
        }
    }

    return status;
}

uint32_t PN_APP_IOD_validateSubmod(
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    uint32_t modId,
    uint32_t submodId)
{
    uint32_t status = PN_API_ERR_PARAM;

    status = PN_APP_IOD_validateModLocation(api, slot, subslot, modId, submodId);

    if(status == PN_API_OK)
    {
        status = PN_APP_IOD_validateSubmodCompatibility(api, modId, submodId);
    }

    if (status == PN_API_OK)
    {
        status = PN_APP_IOD_validateSubmodLocation(api, slot, subslot, modId, submodId);
    }

    return status;
}

 /*!
 * \brief
 * Load remanent submodule config list into an IO submodule list.
 *
 * \details
 * This function creates an IO submodule list and fills it up with the data
 * available in the given remanent submodule config list. Before loading each
 * entry, the remanent data is vatidated against the user defined configuration.
 *
 * \param[out]      ioSubmodList                    IO submodule list.
 * \param[out]      ioSubmodListSize                Number of entries in IO submodule list.
 * \param[in]       submodCfg                       Remanent submodule config list.
 * \param[in]       submodCfgSize                   Number of entries in remanent submodule config list.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK                      Success.
 * \retval          #PN_API_NOT_OK                  Something went wrong.
 * \retval          #PN_API_ERR_PARAM               Invalid parameter.
 * \retval          #PN_API_ERR_INVALID_SLOT        Invalid slot number.
 * \retval          #PN_API_ERR_INVALID_SUBSLOT     Invalid subslot number.
 * \retval          #PN_API_ERR_INVALID_SUBMODULE   Invalid submodule ID.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 *
 */
static uint32_t PN_APP_IOD_loadRemaSubmodConfig(
    PN_API_IOD_SubmodListEntry_t **ioSubmodList,
    uint32_t *ioSubmodListSize,
    PN_API_IOD_RemaDataSubmodCfgInfo_t *submodCfg,
    uint32_t submodCfgSize)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_RemaDataSubmodCfgInfo_t *tmpSubmodCfg = NULL;
    PN_API_IOD_SubmodListEntry_t *tmpIoSubmodEntry = NULL;
    uint32_t numOfSubmods = 0U;
    uint16_t cnt = 0U;

    if ((NULL != ioSubmodList) && (NULL != ioSubmodListSize) && (NULL != submodCfg) && (0 < submodCfgSize))
    {
        status = PN_API_NOT_OK;

        numOfSubmods = (submodCfgSize / sizeof(PN_API_IOD_RemaDataSubmodCfgInfo_t));

        *ioSubmodList = (PN_API_IOD_SubmodListEntry_t*)OSAL_MEMORY_calloc(sizeof(PN_API_IOD_SubmodListEntry_t), numOfSubmods);

        if (NULL != *ioSubmodList)
        {
            *ioSubmodListSize = numOfSubmods;

            status = PN_API_OK;
        }

        while ((status == PN_API_OK) && (cnt < numOfSubmods))
        {
            tmpSubmodCfg = &submodCfg[cnt];

            if ((0U >= tmpSubmodCfg->subslot) || (0U >= tmpSubmodCfg->modId) || (0U >= tmpSubmodCfg->submodId))
            {
                status = PN_API_NOT_OK;
                continue;
            }

            status = PN_APP_IOD_validateSubmod(
                tmpSubmodCfg->api,
                tmpSubmodCfg->slot,
                tmpSubmodCfg->subslot,
                tmpSubmodCfg->modId,
                tmpSubmodCfg->submodId);

            if (status == PN_API_OK)
            {
                tmpIoSubmodEntry = &(*ioSubmodList)[cnt];
                PN_APP_IOD_remaSubmodCfgToSubmodList(tmpIoSubmodEntry, tmpSubmodCfg);
                cnt++;
            }
        }

        if (PN_API_OK != status)
        {
            if (NULL != *ioSubmodList)
            {
                OSAL_MEMORY_free(*ioSubmodList);
            }

            *ioSubmodListSize = 0;
        }
    }

    return status;
}

 /*!
 * \brief
 * Load user-defined (default) IO submodule config into an IO submodule list.
 *
 * \details
 * This function reads the global default IO submodule list defined in pn_app_iod_mod_cfg.c
 * file and loads it into the IO submodule list. Before loading each entry, the remanent
 * data is vatidated against the user-defined configuration.
 *
 * \param[out]      ioSubmodList                    IO submodule list.
 * \param[out]      ioSubmodListSize                Number of entries in IO submodule list.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK                      Success.
 * \retval          #PN_API_NOT_OK                  Something went wrong.
 * \retval          #PN_API_ERR_PARAM               Invalid parameter.
 * \retval          #PN_API_ERR_INVALID_SLOT        Invalid slot number.
 * \retval          #PN_API_ERR_INVALID_SUBSLOT     Invalid subslot number.
 * \retval          #PN_API_ERR_INVALID_SUBMODULE   Invalid submodule ID.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 *
 */
static uint32_t PN_APP_IOD_loadUserSubmodConfig(
    PN_API_IOD_SubmodListEntry_t **ioSubmodList,
    uint32_t *ioSubmodListSize)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_SubmodListEntry_t *tmpIoSubmodEntry = NULL;
    uint32_t numOfSubmods = 0U;
    uint16_t cnt = 0U;

    if ((NULL != ioSubmodList) && (NULL != ioSubmodListSize))
    {
        status = PN_API_NOT_OK;

        PN_APP_IOD_getDefaultIoSubmodList(ioSubmodList, ioSubmodListSize);

        if ((*ioSubmodList != NULL) && (*ioSubmodListSize >= (2U + PN_API_IOD_PDEV_NUM_OF_PORTS)))
        {
            numOfSubmods = *ioSubmodListSize;

            status = PN_API_OK;
        }

        while ((status == PN_API_OK) && (cnt < numOfSubmods))
        {
            tmpIoSubmodEntry = &(*ioSubmodList)[cnt++];
            if ((0U >= tmpIoSubmodEntry->subslot) ||
                (0U >= tmpIoSubmodEntry->modId) ||
                (0U >= tmpIoSubmodEntry->submodId))
            {
                status = PN_API_NOT_OK;
                continue;
            }

            status = PN_APP_IOD_validateSubmod(
                tmpIoSubmodEntry->api,
                tmpIoSubmodEntry->slot,
                tmpIoSubmodEntry->subslot,
                tmpIoSubmodEntry->modId,
                tmpIoSubmodEntry->submodId);
        }

        if (PN_API_OK != status)
        {
            *ioSubmodList = NULL;
            *ioSubmodListSize = 0;
        }
    }

    return status;
}

static uint32_t PN_APP_IOD_loadIm0Data(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    uint32_t modId,
    uint32_t submodId,
    PN_API_IOD_Im0Data_t *im0Data)
{
    uint8_t im0Support;
    uint32_t status = PN_API_ERR_PARAM;

    if (PN_API_IOD_isHandleValid(pnHandle) && (NULL != im0Data))
    {
        status = PN_API_OK;

        im0Support = PN_APP_IOD_getIm0SupportInfo(api, slot, subslot, modId, submodId);

        if (PN_API_IOD_Im0Nothing < im0Support)
        {
            status = PN_APP_IOD_buildIm0Data(pnHandle, api, slot, subslot, im0Data);
        }
    }

    return status;
}

uint32_t PN_APP_IOD_loadSubmodConfig(
    PN_API_IOD_Handle_t *pnHandle,
    PN_API_IOD_SubmodListEntry_t **ioSubmodList,
    uint32_t *ioSubmodListSize,
    PN_API_IOD_Im0ListEntry_t **im0List,
    uint32_t *im0ListSize,
    bool checkRemaData)
{
    PN_API_IOD_RemaDataSubmodCfgInfo_t *pSubmodCfg = NULL;
    PN_API_IOD_SubmodListEntry_t *tmpIoSubmodEntry = NULL;
    uint32_t submodCfgSize = 0U;
    uint32_t status = PN_API_ERR_PARAM;

    if (PN_API_IOD_isHandleValid(pnHandle) &&
        (NULL != ioSubmodList) &&
        (NULL != ioSubmodListSize) &&
        (NULL != im0List) &&
        (NULL != im0ListSize))
    {
        if (true == checkRemaData)
        {
            /* Check for remanent configuration, if available, load it */
            status = PN_APP_IOD_cbRestoreRemaMem(
                pnHandle,
                PN_API_IOD_RemaDataSubmodCfg,
                0,
                (uint8_t**)&pSubmodCfg,
                &submodCfgSize);
            if (status == PN_API_OK)
            {
                status = PN_APP_IOD_loadRemaSubmodConfig(ioSubmodList, ioSubmodListSize, pSubmodCfg, submodCfgSize);
            }

            if (pSubmodCfg != NULL)
            {
                PN_APP_IOD_cbFreeRemaMem(pnHandle, (uint8_t *)pSubmodCfg);
                pSubmodCfg = NULL;
            }
        }
        if (status != PN_API_OK)
        {
            /* If correct remanent configuration is not obtained, load the user configuration from pn_app_iod_mod_cfg.h */
            status = PN_APP_IOD_loadUserSubmodConfig(ioSubmodList, ioSubmodListSize);
        }

        /* If configuration data is successfully loaded, load the IM0 data */
        if ((status == PN_API_OK) && (*ioSubmodList != NULL))
        {
            *im0ListSize = *ioSubmodListSize;
            *im0List = (PN_API_IOD_Im0ListEntry_t*) OSAL_MEMORY_calloc(*im0ListSize, sizeof(PN_API_IOD_Im0ListEntry_t));

            for (uint32_t i = 0; i < *im0ListSize; i++)
            {
                tmpIoSubmodEntry = &(*ioSubmodList)[i];

                status = PN_APP_IOD_loadIm0Data(
                    pnHandle,
                    tmpIoSubmodEntry->api,
                    tmpIoSubmodEntry->slot,
                    tmpIoSubmodEntry->subslot,
                    tmpIoSubmodEntry->modId,
                    tmpIoSubmodEntry->submodId,
                    &(*im0List)[i].im0Data);

                (*im0List)[i].api = tmpIoSubmodEntry->api;
                (*im0List)[i].slot = tmpIoSubmodEntry->slot;
                (*im0List)[i].subslot = tmpIoSubmodEntry->subslot;
            }
        }

        if (status != PN_API_OK)
        {
            /* Cleanup allocated memory space or pointers */
            *ioSubmodList = NULL;
            *im0List = NULL;
        }
    }

    return status;
}

void PN_APP_IOD_cbNewModPlug(
    PN_API_IOD_Handle_t *const pnHandle,
    const PN_API_IOD_DevAddr_t *const addr,
    const uint32_t inDataLen,
    const uint32_t outDataLen)
{
    OSALUNREF_PARM(pnHandle);

    uint32_t slotIndex = 0;
    uint32_t subslotIndex = 0;

    if(NULL != addr)
    {
        slotIndex = addr->address.geo.slot;
        subslotIndex = addr->address.geo.subslot;

        subslotIndex = PN_APP_IOD_getSubslotIndex(slotIndex, subslotIndex);

        if ((PN_API_IOD_MAX_NUM_OF_SLOTS > slotIndex) &&
            (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT >= subslotIndex))
        {
            demoInDataLen[slotIndex][subslotIndex] = inDataLen;
            demoOutDataLen[slotIndex][subslotIndex] = outDataLen;
        }

        //OSAL_printf("Module/submodule successfully plugged into slot %d, subslot %d\r\n", slotIndex, subslotIndex);
    }
}

void PN_APP_IOD_cbNewModPull(PN_API_IOD_Handle_t *const pnHandle, const PN_API_IOD_DevAddr_t *const addr)
{
    OSALUNREF_PARM(pnHandle);

    uint32_t slotIndex = 0;
    uint32_t subslotIndex = 0;

    if(NULL != addr)
    {
        slotIndex = addr->address.geo.slot;
        subslotIndex = addr->address.geo.subslot;

        subslotIndex = PN_APP_IOD_getSubslotIndex(slotIndex, subslotIndex);

        if ((PN_API_IOD_MAX_NUM_OF_SLOTS > slotIndex) &&
            (PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT >= subslotIndex))
        {
            demoInDataLen[slotIndex][subslotIndex] = 0;
            demoOutDataLen[slotIndex][subslotIndex] = 0;
        }

        //OSAL_printf("Module/submodule successfully pulled from slot %d, subslot %d\r\n", slotIndex, subslotIndex);
    }
}
