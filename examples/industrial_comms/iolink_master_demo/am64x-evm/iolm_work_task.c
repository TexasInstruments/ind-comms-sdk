/*!
 *  \file IOLM_WorkTask.c
 *
 *  \brief
 *  IO-Link Master Example Working Task for asynchroneous jobs
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2024 KUNBUS GmbH.
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
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <osal.h>

#include "iolm_work_task.h"
#include "TinyQueue.h"
#include "nvram.h"

#define UNREACHABLE() __builtin_unreachable()
#define FOREVER()     1

// Ringbuffer Queue for asynchronous work
TQUEUE_ringBuffer_t sIOLM_AsyncWorkQueue_g;

// object for NVRAM async write
typedef struct
{
    char    *name;
    int32_t  mode;
    uint32_t len;
    void    *pData;
} nvramAsyncWrite_t;

//------------------------- Private functions prototype ------------------------
static uint32_t IOLM_doWriteNvram(void *const pLoad);
static uint32_t IOLM_cbBootcount(void *const pData);
//------------------------------------------------------------------------------

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Callback function after writing the boot count to NVRAM
 *
 *  \param[in]       pData        not used
 *
 * */
static uint32_t IOLM_cbBootcount(void *const pData)
{
    (void)pData;
    NVR_LOG_DEBUG("called");
    return 0;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  initialize Gateway structures and do the watchdog functionality
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]       pArg        not used
 *
 * */
void OSAL_FUNC_NORETURN IOLM_workTask(void *pArg)
{
    NVR_LOG_INFO("started");
    // keep boot count as simple test of NVRAM function
    static const char BCOUNTFILE[] = "bcount";
    unsigned int      bootcount    = 0;
    // read current boot count
    uint32_t len = sizeof(bootcount);
    if ((NVR_ERR_OK != NVR_read(BCOUNTFILE, &len, 0, &bootcount)) || (len != sizeof(bootcount)))
    {
        bootcount = 0;
        NVR_LOG_INFO("cannot read from %s -> reset boot count", BCOUNTFILE);
    }
    // update boot count
    bootcount += 1u;
    // print the boot count
    NVR_LOG_INFO("queuing boot count %u for write", bootcount);
    // and save it to NVRAM
    IOLM_queueWriteNvram(BCOUNTFILE, NVR_MODE_OVERWRITE, sizeof(bootcount), &bootcount, IOLM_cbBootcount);

    while (FOREVER())
    {
        // TinyQueue deals with uint32, so we put the pointer into that
        uintptr_t elemAdr = 0;
        uint32_t  error   = IOLM_WT_ERRCODE_OK;
        // for future use: do watchdog functionality
        error = TQUEUE_wait(&sIOLM_AsyncWorkQueue_g, &elemAdr);
        if (IOLM_WT_ERRCODE_OK != error)
        {
            NVR_LOG_ERROR("TQUEUE_wait failed for sIOLM_AsyncWorkQueue_g");
        }
        else
        {
            // here we got an entry from the async work queue
            NVR_LOG_DEBUG("do async job");
            asyncWorkMsg_t *pMsg = (asyncWorkMsg_t *)elemAdr;
            if (NULL != pMsg)
            {
                if (NULL != pMsg->func)
                {
                    error = pMsg->func(pMsg->pLoad);
                }
                else
                {
                    // No op is ok
                    error = IOLM_WT_ERRCODE_OK;
                }
                if (NULL != pMsg->cbDone)
                {
                    pMsg->cbDone(&error);
                }
                // delete the message and its load
                OSAL_MEMORY_free(pMsg->pLoad);
                OSAL_MEMORY_free(pMsg);
            }
        }
    }
    // assure the compiler that we never get here
    UNREACHABLE();
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Use data from queue and write to NVRAM
 *
 *  \details
 *  This is a wrapper suitable for queueing to async work tasks
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param data struct with file data
 *
 *  \return IOLM_WT_ERRCODE_OK
 *  \return IOLM_WT_ERRCODE_ERROR
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *
 *  IOLM_worktask_err_t retVal = IOLM_doWriteNvram(&userdata);
 *  \endcode
 *
 * */
static uint32_t IOLM_doWriteNvram(void *const pLoad)
{
    uint32_t errorCode = IOLM_WT_ERRCODE_ERROR;

    // extract the load and do our job
    nvramAsyncWrite_t *p = (nvramAsyncWrite_t *)pLoad;
    if (NULL != p)
    {
        if (NVR_ERR_OK == NVR_write(p->name, p->mode, p->len, p->pData))
        {
            errorCode = IOLM_WT_ERRCODE_OK;
        }
        else
        {
            NVR_LOG_ERROR("%s: ERROR: write NVRAM failed", __func__);
        }
        // delete the pData and name content only
        // the pLoad object itself is deleted by the caller
        // free(NULL) is legitimate
        OSAL_MEMORY_free(p->name);
        OSAL_MEMORY_free(p->pData);
    }
    return errorCode;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Put NVRAM write message in queue
 *
 *  \details
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in] name File name
 *  \param[in] mode File access mode enum: NVR_MODE_OVERWRITE or NVR_MODE_APPEND
 *  \param[in] len Data length in byte
 *  \param[in] pData Pointer to existing data buffer to be written to NVRAM
 *  \param[in] cbDone Callback function after writing is finished
 *
 *  \return IOLM_WT_ERRCODE_OK
 *  \return IOLM_WT_ERRCODE_ERROR
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *
 *  uint8_t pData[] = "hello world";
 *
 *  uint32_t retVal = IOLM_queueWriteNvram("hellofile.txt", NVR_MODE_OVERWRITE, sizeof(pData), pData);
 *
 *  \endcode
 *
 * */
uint32_t IOLM_queueWriteNvram(
    const char *const     name,
    const int32_t         mode,
    const uint32_t        len,
    const void *const     pData,
    asyncWorkFunc_t const cbDone)
{
    uint32_t errorCode = IOLM_WT_ERRCODE_ERROR;
    if (NULL == name)
    {
        NVR_LOG_ERROR("file name is NULL");
    }
    else if (0 == len || NULL == pData)
    {
        NVR_LOG_ERROR("no data");
    }
    else
    {
        // create the load for the called function
        nvramAsyncWrite_t *pLoad = OSAL_MEMORY_calloc(1, sizeof(nvramAsyncWrite_t));
        if (NULL == pLoad)
        {
            NVR_LOG_ERROR("calloc pLoad failed");
        }
        else
        {
            pLoad->pData = OSAL_MEMORY_calloc(1, len);
            if (NULL == pLoad->pData)
            {
                NVR_LOG_ERROR("calloc pData failed");
                OSAL_MEMORY_free(pLoad);
            }
            else
            {
                if (NULL != name)
                {
                    pLoad->name = strdup(name);
                    if (NULL == pLoad->name)
                    {
                        NVR_LOG_ERROR("strdup name failed");
                    }
                }
                else
                {
                    pLoad->name = NULL;
                }
                pLoad->mode = mode;
                pLoad->len  = len;
                OSAL_MEMORY_memcpy(pLoad->pData, pData, len);
                errorCode = IOLM_pushWorkMsg(IOLM_doWriteNvram, cbDone, pLoad);
                if (IOLM_WT_ERRCODE_OK != errorCode)
                {
                    // error pushing the message -> need to free things
                    OSAL_MEMORY_free(pLoad->pData);
                    OSAL_MEMORY_free(pLoad->name);
                    OSAL_MEMORY_free(pLoad);
                }
            } // NULL != pLoad->pData
        }     // NULL != pLoad
    }

    return errorCode;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Clear all data from NVRAM
 *
 *  \details
 *  Re-format NVRAM file system deleting all files
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param unused, only for proper function type
 *
 *  \return IOLM_WT_ERRCODE_ERROR
 *  \return IOLM_WT_ERRCODE_OK
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *
 *  IOLM_worktask_err_t retVal = IOLM_doClearNvram();
 *  \endcode
 *
 * */
uint32_t IOLM_doClearNvram(void *const unused)
{
    (void)unused;
    uint32_t errorCode = IOLM_WT_ERRCODE_ERROR;

    if (NVR_ERR_OK == NVR_erase())
    {
        errorCode = IOLM_WT_ERRCODE_OK;
        NVR_LOG_DEBUG("%s: erased NVRAM - SUCCESS", __func__);
    }
    else
    {
        NVR_LOG_ERROR("%s: ERROR: erase NVRAM failed", __func__);
    }

    return errorCode;
}
/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Queue the command to clear all data from NVRAM
 *
 *  \details
 *
 *  <!-- Parameters and return values: -->
 *
 *  \return IOLM_WT_ERRCODE_OK
 *  \return IOLM_WT_ERRCODE_ERROR
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *
 *  IOLM_worktask_err_t retVal = IOLM_queueClearNvram();
 *  \endcode
 *
 * */
uint32_t IOLM_queueClearNvram(void)
{
    return IOLM_pushWorkMsg(IOLM_doClearNvram, NULL, NULL);
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Allocate a new message and push it to the work task queue
 *
 *  \details
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param func Function pointer to be called
 *  \param cbDone Pointer to callback function to be called after execution of func
 *  \param pLoad User data for func
 *
 *  \return IOLM_WT_ERRCODE_OK
 *  \return IOLM_WT_ERRCODE_ERROR
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *
 *  IOLM_worktask_err_t retVal = IOLM_pushWorkMsg(IOLM_doClearNvram, NULL, NULL);
 *  \endcode
 *
 * */
extern uint32_t IOLM_pushWorkMsg(asyncWorkFunc_t const func, asyncWorkFunc_t const cbDone, void *const pLoad)
{
    uint32_t errorCode = IOLM_WT_ERRCODE_ERROR;

    asyncWorkMsg_t *pMsg = OSAL_MEMORY_calloc(1, sizeof(asyncWorkMsg_t));
    if (NULL != pMsg)
    {
        pMsg->func   = func;
        pMsg->cbDone = cbDone;
        pMsg->pLoad  = pLoad;
        TQUEUE_put(&sIOLM_AsyncWorkQueue_g, (const uintptr_t)pMsg);
        errorCode = IOLM_WT_ERRCODE_OK;
    }

    return errorCode;
}
