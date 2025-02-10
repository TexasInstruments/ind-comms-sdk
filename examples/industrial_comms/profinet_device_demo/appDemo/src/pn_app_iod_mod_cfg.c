/*!
 *  \file pn_app_iod_mod_cfg.c
 *
 *  \brief
 *  Helper functions used by demo app to configure the IO device.
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

#include "pn_app_iod_mod_cfg.h"

#include <osal.h>

#include "pn_api_iod_error.h"
#include "pn_api_iod_handle.h"
#include "pn_api_iod_types.h"

#include "pn_app_iod_cfg.h"

PN_APP_IOD_OwnershipHandle_t ownershipHandle;

/* list of IO submodules (including also PDEV subslots), that shall be plugged during startup.
 *
 * list order:  DAP first, then PDEV, then IO submodules:
 * 1. DAP                              (mandatory)
 * 2. PDEV-Interface                   (mandatory)
 * 3. PDEV port 1 .... PDEV port n     (mandatory)
 * 4. IO submodules                    (optional)
 *
 * Note, that I&M0 support for the DAP is mandatory, but optional for all other submoduls.
 * In this case they have to respond to an IM0 read request with the IM0 data of the DAP, that is
 * a proxy for the device related IM0 data here.
 *
 * IO submodules can optionally be plugged or pulled later.
 * DAP and PDEV subslots can not be pulled.
 *
 * Restrictions:
 * 1. exact one submodule must be the proxy for the device (must be INTERFACE subslot 0x8000)
 * 2. IM1...4 is only supported for the proxy, all other submodules must have IM0.ImSupported = 0 !
 */

PN_API_IOD_SubmodListEntry_t defaultIoSubmodList[]
= {
   //api    slot    subslot     mod_id                  submod_id               in_data out_data    im0_support
   //                                                                           _len    _len
    {0,     0,      1,          PN_CFG_IDD_DAP_MOD_ID,  PN_CFG_IDD_DAP_SUB_ID,  0,      0,          PN_API_IOD_Im0Submodule + //DAP
                                                                                                    PN_API_IOD_Im0Device},
    {0,     0,      0x8000,     PN_CFG_IDD_DAP_MOD_ID,  PN_CFG_IDD_DAP_IF_ID,   0,      0,          PN_API_IOD_Im0Submodule}, //PDEV interface
    {0,     0,      0x8001,     PN_CFG_IDD_DAP_MOD_ID,  PN_CFG_IDD_DAP_P1_ID,   0,      0,          PN_API_IOD_Im0Submodule},
    //PDEV port1
    #if (PN_API_IOD_PDEV_NUM_OF_PORTS >= 2)
    {0,     0,      0x8002,     PN_CFG_IDD_DAP_MOD_ID,  PN_CFG_IDD_DAP_P2_ID,   0,      0,          PN_API_IOD_Im0Submodule},
    //PDEV port2
    #endif
    #if (PN_API_IOD_PDEV_NUM_OF_PORTS >= 3)
    {0,     0,      0x8003,     PN_CFG_IDD_DAP_MOD_ID, PN_CFG_IDD_DAP_P3_ID,    0,      0,          PN_API_eIm0Submodule},  //PDEV port3
    #endif
    #if (PN_API_IOD_PDEV_NUM_OF_PORTS >= 4)
    {0,     0,      0x8004,     PN_CFG_IDD_DAP_MOD_ID, PN_CFG_IDD_DAP_P4_ID,    0,      0,          PN_API_eIm0Submodule},  //PDEV port4
    #endif
    {0,     1,      1,          PN_CFG_IDM_MOD_IO_ID,   PN_CFG_IDS_IO_IF_ID,    0,      0,          (PN_API_IOD_Im0Submodule | //IO subslot interface
                                                                                                    PN_API_IOD_Im0Module)},
    {0,     1,      2,          PN_CFG_IDM_MOD_IO_ID,   PN_CFG_IDS_IO_T4_ID,    4,      4,          PN_API_IOD_Im0Submodule}, //IO subslot
    {0,     1,      3,          PN_CFG_IDM_MOD_IO_ID,   PN_CFG_IDS_IO_T2_ID,    2,      2,          PN_API_IOD_Im0Submodule}  //IO subslot
};

/* List of the user modules and on which slots/subslots they are allowed to exist */
PN_APP_IOD_ModPropEntry_t modPropList[] =
{
    {PN_CFG_IDD_DAP_MOD_ID, {1, {0, 0, 0}}, {4,  {1, 0x8000, 0x8001, 0x8002, 0, 0, 0, 0, 0, 0}}},
    {PN_CFG_IDM_MOD_IN_ID,  {3, {1, 2, 3}}, {5,  {1, 2, 3, 4, 5, 0, 0, 0, 0, 0}}},
    {PN_CFG_IDM_MOD_OUT_ID, {3, {1, 2, 3}}, {5,  {1, 2, 3, 4, 5, 0, 0, 0, 0, 0}}},
    {PN_CFG_IDM_MOD_IO_ID,  {3, {1, 2, 3}}, {10, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}}},
};

/* List of the user submodules and their allowed configuration */
PN_APP_IOD_SubmodCfgEntry_t submodCfgList[] =
{
    {0, PN_CFG_IDD_DAP_SUB_ID, 0, 0, {1, {PN_CFG_IDD_DAP_MOD_ID}}},
    {0, PN_CFG_IDD_DAP_IF_ID, 0, 0, {1, {PN_CFG_IDD_DAP_MOD_ID}}},
    {0, PN_CFG_IDD_DAP_P1_ID, 0, 0, {1, {PN_CFG_IDD_DAP_MOD_ID}}},
    {0, PN_CFG_IDD_DAP_P2_ID, 0, 0, {1, {PN_CFG_IDD_DAP_MOD_ID}}},

    {0, PN_CFG_IDS_IN_IF_ID, 0, 0, {1, {PN_CFG_IDM_MOD_IN_ID}}},
    {0, PN_CFG_IDS_OUT_IF_ID, 0, 0, {1, {PN_CFG_IDM_MOD_OUT_ID}}},
    {0, PN_CFG_IDS_IO_IF_ID, 0, 0, {1, {PN_CFG_IDM_MOD_IO_ID}}},

    {0, PN_CFG_IDS_IN_T1_ID, 1, 0, {2, {PN_CFG_IDM_MOD_IN_ID, PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_IN_T2_ID, 2, 0, {2, {PN_CFG_IDM_MOD_IN_ID, PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_IN_T4_ID, 4, 0, {2, {PN_CFG_IDM_MOD_IN_ID, PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_IN_T64_ID, 64, 0, {2, {PN_CFG_IDM_MOD_IN_ID, PN_CFG_IDM_MOD_IO_ID}}},

    {0, PN_CFG_IDS_OUT_T1_ID, 0, 1, {2, {PN_CFG_IDM_MOD_OUT_ID, PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_OUT_T2_ID, 0, 2, {2, {PN_CFG_IDM_MOD_OUT_ID, PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_OUT_T4_ID, 0, 4, {2, {PN_CFG_IDM_MOD_OUT_ID, PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_OUT_T64_ID, 0, 64, {2, {PN_CFG_IDM_MOD_OUT_ID, PN_CFG_IDM_MOD_IO_ID}}},

    {0, PN_CFG_IDS_IO_T1_ID, 1, 1, {1, {PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_IO_T2_ID, 2, 2, {1, {PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_IO_T4_ID, 4, 4, {1, {PN_CFG_IDM_MOD_IO_ID}}},
    {0, PN_CFG_IDS_IO_T64_ID, 64, 64, {1, {PN_CFG_IDM_MOD_IO_ID}}}
};

void PN_APP_IOD_resetOwnershipHandle(void)
{
    OSAL_MEMORY_memset(&ownershipHandle, 0, sizeof(ownershipHandle));
}

void PN_APP_IOD_getOwnershipHandle(PN_APP_IOD_OwnershipHandle_t **expOwnershipHandle)
{
    *expOwnershipHandle = &ownershipHandle;
}

uint32_t PN_APP_IOD_updateOwnershipHandle(
        PN_API_IOD_Handle_t *const pnHandle,
        PN_API_IOD_RealSubmod_t *realSubmodList,
        uint32_t realSubmodListSize)
{
    uint32_t status = PN_API_ERR_PARAM;
    PN_APP_IOD_OwnershipHandle_t *tmpOwnershipHandle = NULL;

    if (PN_API_IOD_isHandleValid(pnHandle) && (NULL != realSubmodList))
    {
        PN_APP_IOD_getOwnershipHandle(&tmpOwnershipHandle);

        if ((NULL != tmpOwnershipHandle) && (PN_API_IOD_MAX_NUM_OF_SUBSLOTS >= realSubmodListSize))
        {
            for (uint32_t i = 0 ; i < realSubmodListSize ; i++)
            {
                tmpOwnershipHandle->submodList[i].api = realSubmodList[i].api;
                tmpOwnershipHandle->submodList[i].slot = realSubmodList[i].slot;
                tmpOwnershipHandle->submodList[i].subslot = realSubmodList[i].subslot;
                tmpOwnershipHandle->submodList[i].modId = realSubmodList[i].modId;
                tmpOwnershipHandle->submodList[i].submodId = realSubmodList[i].submodId;

                switch(realSubmodList[i].dataDirection)
                {
                case PN_API_IOD_SubmodPropIn:
                    tmpOwnershipHandle->submodList[i].inDataLen = (realSubmodList[i].submodId & 0xFF);
                    tmpOwnershipHandle->submodList[i].outDataLen = 0;
                    break;
                case PN_API_IOD_SubmodPropOut:
                    tmpOwnershipHandle->submodList[i].inDataLen = 0;
                    tmpOwnershipHandle->submodList[i].outDataLen = (realSubmodList[i].submodId & 0xFF);
                    break;
                case PN_API_IOD_SubmodPropIO:
                    tmpOwnershipHandle->submodList[i].inDataLen = (realSubmodList[i].submodId & 0xFF);
                    tmpOwnershipHandle->submodList[i].outDataLen = (realSubmodList[i].submodId & 0xFF);
                    break;
                default:
                    tmpOwnershipHandle->submodList[i].inDataLen = 0;
                    tmpOwnershipHandle->submodList[i].outDataLen = 0;
                }
            }

            tmpOwnershipHandle->cnt = realSubmodListSize;

            status = PN_API_OK;
        }
        else
        {
            status = PN_API_NOT_OK;
        }
    }

    return status;
}

void PN_APP_IOD_getDefaultIoSubmodList(PN_API_IOD_SubmodListEntry_t **list, uint32_t *listSize)
{
    if ((list != NULL) && (listSize != NULL))
    {
        *list     = &defaultIoSubmodList[0];
        *listSize = (sizeof(defaultIoSubmodList) / sizeof(defaultIoSubmodList[0]));
    }
}

void PN_APP_IOD_getModPropList(PN_APP_IOD_ModPropEntry_t **list, uint32_t *listSize)
{
    if ((list != NULL) && (listSize != NULL))
    {
        if ((sizeof(modPropList) >= sizeof(PN_APP_IOD_ModPropEntry_t))
            && (sizeof(modPropList) % sizeof(PN_APP_IOD_ModPropEntry_t) == 0))
        {
            *list     = &modPropList[0];
            *listSize = sizeof(modPropList);
        }
    }
}

void PN_APP_IOD_getSubmodCfgList(PN_APP_IOD_SubmodCfgEntry_t **list, uint32_t *listSize)
{
    if((list != NULL) && (listSize != NULL))
    {
        if ((sizeof(submodCfgList) >= sizeof(PN_APP_IOD_SubmodCfgEntry_t))
            && (sizeof(submodCfgList) % sizeof(PN_APP_IOD_SubmodCfgEntry_t) == 0))
        {
            *list     = &submodCfgList[0];
            *listSize = sizeof(submodCfgList);
        }
    }
}

uint32_t PN_APP_IOD_getModProp(uint32_t modId, PN_APP_IOD_ModPropEntry_t *modProp)
{
    uint32_t                   status          = PN_API_ERR_NO_MODULE;
    PN_APP_IOD_ModPropEntry_t *pModPropList;
    uint32_t                   modPropListSize = 0;
    uint16_t                   numOfItems      = 0;
    int                        cnt             = 0;

    if ((modProp != NULL) && (modId > 0))
    {
        /* Get the module properties list defined by the user */
        PN_APP_IOD_getModPropList(&pModPropList, &modPropListSize);

        numOfItems = modPropListSize / sizeof(PN_APP_IOD_ModPropEntry_t);

        while ((status != PN_API_OK) && (cnt < numOfItems))
        {
            /* Does the user module properties list contain the given module? */
            if (modId == pModPropList[cnt].modId)
            {
                OSAL_MEMORY_memcpy(modProp, &pModPropList[cnt], sizeof(PN_APP_IOD_ModPropEntry_t));
                status = PN_API_OK;
            }

            cnt++;
        }
    }
    else
    {
        status = PN_API_ERR_PARAM;
    }

    return status;
}

uint32_t PN_APP_IOD_getSubmodCfg(uint32_t api, uint32_t submodId, PN_APP_IOD_SubmodCfgEntry_t *submodCfg)
{
    uint32_t                     status            = PN_API_ERR_NO_SUBMODULE;
    PN_APP_IOD_SubmodCfgEntry_t *pSubmodCfgList;
    uint32_t                     submodCfgListSize = 0;
    uint16_t                     numOfItems        = 0;
    int                          cnt               = 0;

    if((submodCfg != NULL) && (submodId > 0))
    {
        /* Get the submodule config list defined by the user */
        PN_APP_IOD_getSubmodCfgList(&pSubmodCfgList, &submodCfgListSize);

        numOfItems = submodCfgListSize / sizeof(PN_APP_IOD_SubmodCfgEntry_t);
        while((status != PN_API_OK) && (cnt < numOfItems))
        {
            /* Does the user submodule config list contain the given submodule? */
            if((api == pSubmodCfgList[cnt].api) && (submodId == pSubmodCfgList[cnt].submodId))
            {
                OSAL_MEMORY_memcpy(submodCfg, &pSubmodCfgList[cnt], sizeof(PN_APP_IOD_SubmodCfgEntry_t));
                status = PN_API_OK;
            }

            cnt++;
        }
    }
    else
    {
        status = PN_API_ERR_PARAM;
    }

    return status;
}
