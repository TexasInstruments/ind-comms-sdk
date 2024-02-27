//Control Path place holder.c

/*
 *  Copyright (C) 2021-2024 Texas Instruments Incorporated
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

/*!
 * \file  app_control.c
 *
 * \brief This file contains the implementation of the Enet Remote Connect control path for remote core.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include <enet.h>
#include <enet_apputils.h>

#include "ti_enet_config.h"
//#include "app_netif.h"
#include "app_control.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define MAX_ETH_PKT_LEN    (1540U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

Icve_ShrMemInfo gShmInfoTbl[4U]=
{
    {
        .total_shm_size       = 0x00400000U,
        .num_pkt_bufs         = 2048U,
        .buff_slot_size       = MAX_ETH_PKT_LEN + 8U, /*! 8Bytes for Magic number and Pkt Len info */
        .base_addr            = 0xA0800000U,
    },
    {
        .total_shm_size       = 0x00400000U,
        .num_pkt_bufs         = 2048U,
        .buff_slot_size       = MAX_ETH_PKT_LEN + 8U, /*! 8Bytes for Magic number and Pkt Len info */
        .base_addr            = 0xA0C00000U,
    },
    {
        .total_shm_size       = 0x00400000U,
        .num_pkt_bufs         = 2048U,
        .buff_slot_size       = MAX_ETH_PKT_LEN + 8U, /*! 8Bytes for Magic number and Pkt Len info */
        .base_addr            = 0xA1800000U,
    },
    {
        .total_shm_size       = 0x00400000U,
        .num_pkt_bufs         = 2048U,
        .buff_slot_size       = MAX_ETH_PKT_LEN + 8U, /*! 8Bytes for Magic number and Pkt Len info */
        .base_addr            = 0xA1C00000U,
    },
};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static int32_t AppCtrl_addMacAddr2fbd(void* args);

static int32_t App_handleReq(Icve_reqMsg reqMsg);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static int32_t AppCtrl_addMacAddr2fbd(void* args)
{
    int32_t status = ICVE_OK;
    bool macInvalid = false;
    Enet_Type enetType;
    uint32_t instId, i;

    Icve_macAddr* mac = (Icve_macAddr*)args;

    for (i = 0U; i<6U; i++)
    {
        macInvalid &= ((mac->macAddr[i] == 0U) ? true : false);
    }

    if(macInvalid)
    {
        status = ICVE_BADARGS;
    }

    if(status == ICVE_OK)
    {
        EnetApp_getEnetInstInfo(CONFIG_ENET_ICSS0, &enetType, &instId);
        status = App_addMacFdbEntry(enetType, instId, mac->macAddr);
        EnetAppUtils_print("Adding new to FDB  \r\n");
    }

    return status;
}

static int32_t AppCtrl_getShrMemRegion()
{
    int32_t status = ICVE_OK;
    Icve_respMsg respMsg;
    Icve_message msg;

    respMsg.shm_info[0].total_shm_size  = gShmInfoTbl[0].total_shm_size;
    respMsg.shm_info[0].num_pkt_bufs    = gShmInfoTbl[0].num_pkt_bufs;
    respMsg.shm_info[0].buff_slot_size  = gShmInfoTbl[0].buff_slot_size;
    respMsg.shm_info[0].base_addr       = gShmInfoTbl[0].base_addr;

    respMsg.shm_info[1].total_shm_size  = gShmInfoTbl[1].total_shm_size;
    respMsg.shm_info[1].num_pkt_bufs    = gShmInfoTbl[1].num_pkt_bufs;
    respMsg.shm_info[1].buff_slot_size  = gShmInfoTbl[1].buff_slot_size;
    respMsg.shm_info[1].base_addr       = gShmInfoTbl[1].base_addr;

    msg.resp_msg = respMsg;


    return status;
}

void App_handleControlMsg(void* pMsg)
{
//    uint32_t i,idx;
    Icve_message* msgBuf = (Icve_message*) pMsg;

    switch(msgBuf->msg_hdr.msg_type)
    {
        case ICVE_REQUEST_MSG:
            App_handleReq(msgBuf->req_msg);

        case ICVE_NOTIFY_MSG:
            App_handleNotify(&msgBuf->notify_msg);

        case ICVE_RESPONSE_MSG:
            App_handleResponse(&msgBuf->resp_msg);
    }
}

static int32_t App_handleReq(Icve_reqMsg reqMsg)
{
    int32_t status = ICVE_OK;
    if(reqMsg.params == NULL)
    {
        DebugP_log("Request Params are Invalid\r\n");
    }
    else
    {
        switch(reqMsg.type)
        {
            case ICVE_REQ_SHM_INFO:
                status = AppCtrl_memInfoResp();

            case ICVE_REQ_ADD_MAC_ADDR:
                status = AppCtrl_addMacAddr2fbd(reqMsg.params);
        }
    }
    DebugP_assert(reqMsg.params != NULL);

    return status;
}
