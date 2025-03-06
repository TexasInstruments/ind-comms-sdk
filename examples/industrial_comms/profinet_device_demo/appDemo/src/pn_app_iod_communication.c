/*!
 *  \file pn_app_iod_communication.c
 *
 *  \brief
 *  Functions and callbacks for the handling of ARs (Application Relations).
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

#include "pn_app_iod_communication.h"

#include <stdbool.h>
#include <string.h>

#include "osal.h"

#include "pn_api_iod_callbacks.h"
#include "pn_api_iod_data.h"
#include "pn_api_iod_device.h"
#include "pn_api_iod_error.h"
#include "pn_api_iod_handle.h"
#include "pn_api_iod_types.h"

#include "pn_app_iod_data.h"
#include "pn_app_iod_device.h"
#include "pn_app_iod_mod_cfg.h"
#include "pn_app_iod_record.h"
#include "pn_app_iod_bsp.h"

#define SEND_CLOCK(x) ((x * 31250) / 1000000) // sendClock = sendClockFactor * 31,25 us
#define AR_INFO_LIST_LEN PN_API_IOD_NUM_OF_IO_AR + PN_API_IOD_NUM_OF_DEV_ACCESS_AR

extern OSAL_SCHED_EventHandle_t *dataExchangeEvent;

PN_API_IOD_ArInfo_t arInfoList[AR_INFO_LIST_LEN];

uint16_t numOfRtc3Ars;
uint16_t numOfRtcxArs;
uint32_t numOfRunningArs;

void PN_APP_IOD_initArInfo(void)
{
    OSAL_MEMORY_memset(arInfoList, 0, sizeof(arInfoList));
    numOfRtc3Ars = 0U;
    numOfRtcxArs = 0U;
    numOfRunningArs = 0U;
}

void PN_APP_IOD_cbReportArFSURecord(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint8_t arFsuEnabled,
    const uint8_t arFsuChanged)
{
    OSALUNREF_PARM(pnHandle);
    OSALUNREF_PARM(arFsuEnabled);

    if (PN_API_IOD_ARFSU_NOT_CHANGED == arFsuChanged)
    {
        OSAL_printf("New ARFSU UUID -> parameterization of device was changed\r\n");
    }
}

PN_API_IOD_SubmodStatus_t PN_APP_IOD_cbParamEnd(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint16_t arNum,
    const uint16_t sessionKey,
    const uint32_t api,
    const uint16_t slot,
    const uint16_t subslot,
    const bool moreFollows)
{
    uint32_t status = PN_API_NOT_OK;
    PN_API_IOD_SubmodStatus_t submodStatus = PN_API_IOD_SubmodStatusUnknown;
    PN_API_IOD_RealSubmod_t *realSubmod;

    OSALUNREF_PARM(moreFollows);

    status = PN_API_IOD_getRealSubmod(pnHandle, &realSubmod, api, slot, subslot);

    if (PN_API_OK == status)
    {
        status = PN_API_IOD_setRealSubmodStatus(pnHandle, realSubmod, PN_API_IOD_RealSubmodStatusRun);
    }

    if (PN_API_OK == status)
    {
        submodStatus = PN_API_IOD_SubmodStatusRun;

        if (0 == subslot)
        {
            OSAL_printf("Param_end for all submodules: AR = %d, session key = %d\r\n", arNum, sessionKey);
        }
        else
        {
            OSAL_printf(
                "Param_end for: API = %d, slot = %d, subslot = %d, AR = %d, session key = %d\r\n",
                api,
                slot,
                subslot,
                arNum,
                sessionKey);
        }
    }

    /* System automatically generates "application ready" telegram */
    return submodStatus;
}

void PN_APP_IOD_cbArConnectInd(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_ArType_t arType,
    uint32_t arNum,
    uint16_t arSessionKey,
    uint16_t sendClock,
    uint16_t redRatioIoCrIn,
    uint16_t redRatioIoCrOut,
    uint32_t hostIpAddr)
{
    OSALUNREF_PARM(pnHandle);

    if ((0 < arNum) && (AR_INFO_LIST_LEN >= arNum))
    {
        switch (arType)
        {
            case PN_API_IOD_ArTypeSingle:     // = 0x0001, /* IOCARSingle */
            case PN_API_IOD_ArTypeSupervisor: // = 0x0006, /* IOSAR, The supervisor AR is a special
                                              // form of the IOCARSingle */
            case PN_API_IOD_ArTypeSingleRtc3: // = 0x0010, /* IOCARSingle using RT_CLASS_3 */
            case PN_API_IOD_ArTypeSingleSysRedundancy: // = 0x0020, /* IOCARSR, The SR AR is a special
                                                       // form of the IOCARSingle indicating system
                                                       // redundancy or configure in run usage */
            {
                PN_API_IOD_ArInfo_t *pArInfo = &arInfoList[arNum - 1];

                if ((0U == pArInfo->sendClockFactor) || (sendClock < pArInfo->sendClockFactor))
                {
                    pArInfo->cycleFactor = 0U;
                    pArInfo->arType = arType;
                    pArInfo->arNum = arNum;
                    pArInfo->arSessionKey = arSessionKey;
                    pArInfo->sendClockFactor = sendClock;
                    pArInfo->redRatioIoCrIn = redRatioIoCrIn;
                    pArInfo->redRatioIoCrOut = redRatioIoCrOut;
                    pArInfo->hostIp = hostIpAddr;
                    pArInfo->cycleFactor = SEND_CLOCK(pArInfo->sendClockFactor);
                }

                if (PN_API_IOD_ArTypeSingleRtc3 == arType)
                {
                    numOfRtc3Ars++;
                }
                else
                {
                    if ((PN_API_IOD_ArTypeSupervisor == arType) ||
                        (PN_API_IOD_ArTypeSingleSysRedundancy == arType))
                    {
                        numOfRtcxArs++;
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }

        numOfRunningArs++;

        OSAL_printf(
            "Connect_ind: AR = %d, AR type = %d, send clock factor = %d, ",
            arNum,
            arType,
            sendClock);
        OSAL_printf(
            "Reduction ratio (in) = %d, reduction ratio (out) = %d, session key = %d, ",
            redRatioIoCrIn,
            redRatioIoCrOut,
            arSessionKey);
        OSAL_printf(
            "host IP = %d.%d.%d.%d\r\n",
            (uint8_t)hostIpAddr & 0xFF,
            (uint8_t)(hostIpAddr >> 8) & 0xFF,
            (uint8_t)(hostIpAddr >> 16) & 0xFF,
            (uint8_t)(hostIpAddr >> 24) & 0xFF);
    }
}

void PN_APP_IOD_cbArDisconn(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint16_t arNum,
    const uint16_t arSessionKey,
    const PN_API_IOD_ArReason_t reasonCode)
{
    PN_API_IOD_ArType_t arType = PN_API_IOD_arGetType(pnHandle, arNum);

    OSAL_printf(
        "AR disconnected! (AR = %d, session key = %d, reason = %d)\r\n",
        arNum,
        arSessionKey,
        reasonCode);

    if (PN_API_IOD_ArTypeSingleRtc3 == arType)
    {
        if (0U < numOfRtc3Ars)
        {
            numOfRtc3Ars--;
        }
    }
    else
    {
        if ((PN_API_IOD_ArTypeSupervisor == arType) ||
            (PN_API_IOD_ArTypeSingleSysRedundancy == arType))
        {
            if (0U < numOfRtcxArs)
            {
                numOfRtcxArs--;
            }
        }
    }

    if (0U < numOfRunningArs)
    {
        numOfRunningArs--;
    }

    if ((0 < arNum) && (AR_INFO_LIST_LEN >= arNum))
    {
        OSAL_MEMORY_memset(&arInfoList[arNum - 1], 0, sizeof(PN_API_IOD_ArInfo_t));
    }

    if (numOfRunningArs == 0)
    {
        PN_API_IOD_deactivateCyclicDataExchange(pnHandle);
    }
}

void PN_APP_IOD_cbReadyForInputUpdate(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint16_t arNum,
    const PN_API_IOD_InputUpdateState_t inputUpdateState)
{
    if (PN_API_IOD_ArStartup == inputUpdateState)
    {
        OSAL_printf("Ready for input update during startup: AR = %d\r\n", arNum);
    }
    else
    {
        OSAL_printf("Ready for input update during submodule update: AR = %d\r\n", arNum);
    }

    OSAL_EVT_set(dataExchangeEvent);

    PN_API_IOD_activateCyclicDataExchange(pnHandle);
}

/*!
 * \brief
 * Verify input/output length and API of a given submodule.
 *
 * \details
 * This function verifies submodule input/output data length and API against the given
 * submodule configuration (or GSDML).
 *
 * \param[in]       pnHandle                          Profinet API Handle.
 * \param[in]       expSubmod                         Expected submodule.
 * \param[in]       submodCfg                         Submodule config item to be compared with.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK                        Success.
 * \retval          #PN_API_ERR_IOD_INVALID_BUFLEN    Invalid buffer length.
 * \retval          #PN_API_ERR_IOD_INVALID_API       Invalid API.
 * \retval          #PN_API_ERR_PARAM                 Invalid parameter.
 *
 */
static uint32_t PN_APP_IOD_verifySubmodProps(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_ExpSubmod_t *const expSubmod,
    PN_APP_IOD_SubmodCfgEntry_t *const submodCfg)
{
    uint32_t status = PN_API_ERR_PARAM;

    if (PN_API_IOD_isHandleValid(pnHandle) && (NULL != expSubmod) && (NULL != submodCfg))
    {
        status = PN_API_OK;

        /* Verify input/output data lengths */
        switch (expSubmod->ioProp)
        {
            case PN_API_IOD_SubmodPropIn: // Input submodule should have no output data
                if ((expSubmod->in.dataLen > submodCfg->inDataLen) || (expSubmod->out.dataLen > 0))
                {
                    status = PN_API_ERR_IOD_INVALID_BUFLEN;
                }
                break;
            case PN_API_IOD_SubmodPropOut: // Output submodule should have no input data
                if ((expSubmod->out.dataLen > submodCfg->outDataLen) || (expSubmod->in.dataLen > 0))
                {
                    status = PN_API_ERR_IOD_INVALID_BUFLEN;
                }
                break;
            case PN_API_IOD_SubmodPropIO:
                if ((expSubmod->in.dataLen > submodCfg->inDataLen) ||
                    (expSubmod->out.dataLen > submodCfg->outDataLen))
                {
                    status = PN_API_ERR_IOD_INVALID_BUFLEN;
                }
                break;
            default:
                break;
        }

        if (expSubmod->api != submodCfg->api)
        {
            status = PN_API_ERR_IOD_INVALID_API;
        }
    }

    return status;
}

/*!
 * \brief
 * Verify whether a given submodule is allowed to exist on the device.
 *
 * \details
 * This function verifies submodule compatibility, input/output data length and API against
 * the given submodule configuration (or GSDML).
 *
 * \param[in]       pnHandle                          Profinet API Handle.
 * \param[in]       expSubmod                         Expected submodule.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK                        Success.
 * \retval          #PN_API_NOT_OK                    Something went wrong.
 * \retval          #PN_API_ERR_INVALID_SLOT          Invalid slot number.
 * \retval          #PN_API_ERR_INVALID_SUBSLOT       Invalid subslot number.
 * \retval          #PN_API_ERR_INVALID_SUBMODULE     Invalid submodule ID.
 * \retval          #PN_API_ERR_IOD_INVALID_BUFLEN    Invalid buffer length.
 * \retval          #PN_API_ERR_IOD_INVALID_API       Invalid API.
 * \retval          #PN_API_ERR_PARAM                 Invalid parameter.
 *
 */
static uint32_t PN_APP_IOD_verifySubmodCompatibility(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_ExpSubmod_t *const expSubmod)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_APP_IOD_SubmodCfgEntry_t submodCfg;

    if (PN_API_IOD_isHandleValid(pnHandle) && (NULL != expSubmod))
    {
        /* Check if the submodule is allowed according to the given
         * submodule configuration (or GSDML) */
        status = PN_APP_IOD_validateSubmod(
            expSubmod->api,
            expSubmod->slot,
            expSubmod->subslot,
            expSubmod->modId,
            expSubmod->submodId);

        if (PN_API_OK == status)
        {
            /* Get the submodule configuration */
            OSAL_MEMORY_memset(&submodCfg, 0, sizeof(submodCfg));
            status = PN_APP_IOD_getSubmodCfg(expSubmod->api, expSubmod->submodId, &submodCfg);
        }

        if (PN_API_OK == status)
        {
            status = PN_APP_IOD_verifySubmodProps(pnHandle, expSubmod, &submodCfg);
        }
    }

    return status;
}

/*!
 * \brief
 * Pull all submodules of an API from a specific slot.
 *
 * \details
 * This function pulls all submodule of an API starting from a given slot number and
 * clears the corresponding elements in the global record list.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       api                 Application Process Identifier.
 * \param[in]       slot                Slot number.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK          Success.
 * \retval          #PN_API_NOT_OK      Something went wrong.
 * \retval          #PN_API_ERR_PARAM   Invalid parameter.
 *
 */
static uint32_t PN_APP_IOD_pullAllSlotSubmods(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint32_t slot)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_RealSubmod_t *tmpRealSubmod;
    PN_API_IOD_DevAddr_t addr;

    if (PN_API_IOD_isHandleValid(pnHandle))
    {
        for (uint32_t subslotIndex = 0; subslotIndex < PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT;
             subslotIndex++)
        {
            OSAL_MEMORY_memset(&tmpRealSubmod, 0, sizeof(PN_API_IOD_RealSubmod_t));
            status = PN_API_IOD_getRealSubmod(pnHandle, &tmpRealSubmod, api, slot, subslotIndex);

            if (PN_API_OK == status)
            {
                addr.address.geo.slot    = tmpRealSubmod->slot;
                addr.address.geo.subslot = tmpRealSubmod->subslot;

                /* Clear the record handle */
                PN_APP_IOD_clearRecordListElement(tmpRealSubmod->index);

                /* Pull submodule */
                PN_API_IOD_pullSubmod(pnHandle, tmpRealSubmod->api, &addr);
            }
        }

        status = PN_API_OK;
    }

    return status;
}

/*!
 * \brief
 * Verify/update submodule location on the device.
 *
 * \details
 * This function checks whether a given expected submodule already exists on the device and plugged
 * into the correct location. If not, it tries to update it's location based on the following rules:
 * a). if submodule is plugged on the correct slot/subslot, do nothing.
 * b). if the wrong module is plugged, pull all submodules on the slot and go to d).
 * c). if the correct module is plugged but the submodule is wrong, pull the submodule, clear
*      the corresponding element of the global record list and go to d).
 * d). if the slot/subslot is empty, build the IM0 data of the expected submodule and plug it.
 *
 * \param[in]       pnHandle                          Profinet API Handle.
 * \param[in]       expSubmod                         Expected submodule.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK                        Success.
 * \retval          #PN_API_NOT_OK                    Something went wrong.
 * \retval          #PN_API_ERR_INVALID_SUBSLOT       Invalid subslot number.
 * \retval          #PN_API_ERR_PARAM                 Invalid parameter.
 *
 */
static uint32_t PN_APP_IOD_updateSubmodLocation(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_ExpSubmod_t *const expSubmod)
{
    uint32_t status = PN_API_ERR_PARAM;
    uint32_t submodStatus = PN_API_ERR_PARAM;
    PN_API_IOD_RealSubmod_t *realSubmod;
    PN_API_IOD_DevAddr_t addr;
    PN_API_IOD_Im0Data_t im0Data;
    uint8_t im0Support;

    if (PN_API_IOD_isHandleValid(pnHandle) && (NULL != expSubmod))
    {
        status = PN_API_OK;
        submodStatus = PN_API_IOD_getRealSubmod(
            pnHandle,
            &realSubmod,
            expSubmod->api,
            expSubmod->slot,
            expSubmod->subslot);
    }

    if ((PN_API_OK == status) && (PN_API_OK == submodStatus)) /* A submodule is plugged. Verify if
                                                                 it's the expected one */
    {
        addr.address.geo.slot = realSubmod->slot;
        addr.address.geo.subslot = realSubmod->subslot;

        if (realSubmod->modId == expSubmod->modId) /* Module ID is as expected */
        {
            if (realSubmod->submodId != expSubmod->submodId) /* Wrong submodule! */
            {
                /* Clear the record handle */
                PN_APP_IOD_clearRecordListElement(realSubmod->index);

                /* Pull submodule */
                PN_API_IOD_pullSubmod(pnHandle, realSubmod->api, &addr);
            }
        }
        else if ((1 <= expSubmod->slot) && (1 == expSubmod->subslot)) /* Module ID is not as
                                                                         expected and it's an IO
                                                                         module access point */
        {
            /* Remove all plugged submodules on this slot for this API */
            PN_APP_IOD_pullAllSlotSubmods(pnHandle, expSubmod->api, expSubmod->slot);
        }
        else
        {
            status = PN_API_ERR_INVALID_SUBSLOT;
        }

        if (PN_API_OK == status)
        {
            /* At this point, if a submodule still exists, then it's the correct one.
             * Otherwise, subslot is free */
            submodStatus = PN_API_IOD_getRealSubmod(
                pnHandle,
                &realSubmod,
                expSubmod->api,
                expSubmod->slot,
                expSubmod->subslot);
        }
    }

    if ((PN_API_OK == status) && (PN_API_NOT_OK == submodStatus)) /* Subslot is free. No plugged
                                                                     submodule */
    {
        addr.address.geo.slot = expSubmod->slot;
        addr.address.geo.subslot = expSubmod->subslot;

        im0Support = PN_APP_IOD_getIm0SupportInfo(
            expSubmod->api,
            expSubmod->slot,
            expSubmod->subslot,
            expSubmod->modId,
            expSubmod->submodId);

        if (PN_API_IOD_Im0Nothing < im0Support)
        {
            status = PN_APP_IOD_buildIm0Data(
                pnHandle,
                expSubmod->api,
                expSubmod->slot,
                expSubmod->subslot,
                &im0Data);
        }

        if (PN_API_OK == status)
        {
            status = PN_API_IOD_plugSubmod(
                pnHandle,
                expSubmod->api,
                &addr,
                expSubmod->modId,
                expSubmod->submodId,
                expSubmod->in.dataLen,
                expSubmod->out.dataLen,
                im0Support,
                &im0Data,
                PN_API_IOD_STATUS_GOOD);
        }
    }
    else if (PN_API_ERR_INVALID_SUBSLOT != status)
    {
        status = submodStatus;
    }

    return status;
}

#if (0 == PN_API_IOD_INCLUDE_S2_REDUNDANCY)
/*!
 * \brief
 * Pull remaining submodules of an API starting from a specific slot/subslot.
 *
 * \details
 * This function pulls all submodule of an API starting from a given slot/subslot number and
 * clears the corresponding elements in the global record list.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       api                 Application Process Identifier.
 * \param[in]       slot                Slot number.
 * \param[in]       subslot             Subslot number.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK          Success.
 * \retval          #PN_API_NOT_OK      Something went wrong.
 * \retval          #PN_API_ERR_PARAM   Invalid parameter.
 *
 */
static uint32_t PN_APP_IOD_pullRemainingSubmods(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint32_t slot,
    uint32_t subslot)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_RealSubmod_t *tmpRealSubmod;
    PN_API_IOD_DevAddr_t addr;

    if (PN_API_IOD_isHandleValid(pnHandle))
    {
        for (uint32_t slotIndex = slot; slotIndex < PN_API_IOD_MAX_NUM_OF_SLOTS; slotIndex++)
        {
            for (uint32_t subslotIndex = subslot + 1;
                 subslotIndex < PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT;
                 subslotIndex++)
            {
                status = PN_API_IOD_getRealSubmod(pnHandle, &tmpRealSubmod, api, slotIndex, subslotIndex);

                if (PN_API_OK == status)
                {
                    addr.address.geo.slot = tmpRealSubmod->slot;
                    addr.address.geo.subslot = tmpRealSubmod->subslot;

                    /* Clear the record handle */
                    PN_APP_IOD_clearRecordListElement(tmpRealSubmod->index);

                    /* Pull submodule */
                    PN_API_IOD_pullSubmod(pnHandle, tmpRealSubmod->api, &addr);
                }
            }
        }

        status = PN_API_OK;
    }

    return status;
}
#endif

void PN_APP_IOD_cbArOwnershipInd(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t arNum,
    PN_API_IOD_Exp_t *expSubmodsCfg)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_API_IOD_ExpSubmod_t *expSubmod = NULL;
    PN_API_IOD_RealSubmod_t *realSubmodList = NULL;
    PN_APP_IOD_OwnershipHandle_t *tmpOwnershipHandle = NULL;
    uint32_t realSubmodListSize = 0;

    if (PN_API_IOD_isHandleValid(pnHandle) && (NULL != expSubmodsCfg) && (0 < arNum))
    {
        status = PN_API_OK;
    }

    if (PN_API_OK == status)
    {
        PN_APP_IOD_resetOwnershipHandle();

        for (uint32_t i = 0; i < expSubmodsCfg->numOfPluggedSubmods; i++)
        {
            expSubmod = &expSubmodsCfg->submod[i];

            if (PN_API_IOD_MAX_NUM_OF_SUBSLOTS < (i + 1))
            {
                expSubmod->isWrongSubmod = true;
                expSubmod->ownSessionKey = 0;
                continue;
            }

            status = PN_APP_IOD_verifySubmodCompatibility(pnHandle, expSubmod);

            if (PN_API_OK != status)
            {
                expSubmod->isWrongSubmod = true;
                expSubmod->ownSessionKey = 0;
                continue;
            }

            status = PN_APP_IOD_updateSubmodLocation(pnHandle, expSubmod);

            if (PN_API_OK != status)
            {
                expSubmod->isWrongSubmod = true;
                expSubmod->ownSessionKey = 0;

                PN_APP_IOD_setIops(expSubmod->slot, expSubmod->subslot, PN_API_IOD_STATUS_BAD);
                PN_API_IOD_setIops(
                    pnHandle,
                    expSubmod->api,
                    expSubmod->slot,
                    expSubmod->subslot,
                    PN_API_IOD_STATUS_BAD);
            }
            else
            {
                expSubmod->isWrongSubmod = false;

                PN_APP_IOD_setIops(expSubmod->slot, expSubmod->subslot, PN_API_IOD_STATUS_GOOD);
                PN_API_IOD_setIops(
                    pnHandle,
                    expSubmod->api,
                    expSubmod->slot,
                    expSubmod->subslot,
                    PN_API_IOD_STATUS_GOOD);
            }
        }
#if (0 == PN_API_IOD_INCLUDE_S2_REDUNDANCY)
        if (NULL != expSubmod)
        {
            /* Pull the already existing submodules that are not expected by the controller (if any) */
            status = PN_APP_IOD_pullRemainingSubmods(
                pnHandle,
                expSubmod->api,
                expSubmod->slot,
                expSubmod->subslot);
        }
#endif

        /* Store real submodule configuration as remanent data */
        status = PN_API_IOD_getRealSubmodList(pnHandle, &realSubmodList, &realSubmodListSize);

        if(PN_API_OK == status)
        {
            status = PN_APP_IOD_updateOwnershipHandle(pnHandle, realSubmodList, realSubmodListSize);
        }

        if(PN_API_OK == status)
        {
            PN_APP_IOD_getOwnershipHandle(&tmpOwnershipHandle);
        }

        if ((PN_API_OK == status) && (NULL != tmpOwnershipHandle) && (PN_API_IOD_MAX_NUM_OF_SUBSLOTS
            >= tmpOwnershipHandle->cnt) && ((2U + PN_API_IOD_PDEV_NUM_OF_PORTS) <= tmpOwnershipHandle->cnt))
        {
            uint32_t memSize = tmpOwnershipHandle->cnt * sizeof(PN_API_IOD_RemaDataSubmodCfgInfo_t);
            PN_APP_IOD_cbStoreRemaMem(
                pnHandle,
                PN_API_IOD_RemaDataSubmodCfg,
                0,
                memSize,
                (void *)tmpOwnershipHandle->submodList);
        }
    }
}
