/*!
 *  \file nvm.c
 *
 *  \brief
 *  Persistent data storage API.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2024, KUNBUS GmbH<br /><br />
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
 */

#include "nvm.h"
#include "FreeRTOS.h"
#include "nvm_drv_eeprom.h"
#include "nvm_drv_flash.h"

#define WRITE_STACK_SIZE_BYTE     1024
#define WRITE_FLASH_STACK_SIZE    (WRITE_STACK_SIZE_BYTE/sizeof(configSTACK_DEPTH_TYPE))

typedef struct NVM_APP_writeParam
{
    NVM_type_t  type;
    uint32_t    offset;
    uint32_t    id;
    uint32_t    length;
    const void* pData;
} NVM_APP_writeParam_t;

typedef struct NVM_APP_handle
{
    NVM_APP_writeCallback_t callback;
    void* task;
    void* semaphore;
    NVM_APP_writeParam_t data;
    NVM_err_t status;
}NVM_APP_handle_t;

static NVM_APP_handle_t NVM_handle = {0};

OSAL_FUNC_NORETURN static void NVM_APP_writeTask(void *pArg);

static StackType_t writeTaskStack[WRITE_FLASH_STACK_SIZE] __attribute__((aligned(32), section(".threadstack"))) = {0};

/*!
 * \brief
 * Init NVM library.
 *
 * \details
 * It initializes the asynchronous write thread and the OS semaphore.
 *
 * \param[in]     priority                  Write task priority.
 *
 * \return        NVM_err_t as uint32_t.
 * \retval        NVM_ERR_SUCCESS           Success.
 * \retval        NVM_ERR_FAIL              Failed to create OSAL signal or write task.
 *
 * \see NVM_APP_writeTask
 *
 * \ingroup NVM_APP
 *
 */
uint32_t NVM_APP_init(const OSAL_TASK_Priority_t priority)
{
    uint32_t error = NVM_ERR_FAIL;

    if(NVM_handle.semaphore == NULL)
    {
        NVM_handle.semaphore = OSAL_createSignal("NVM_APP_WriteAsyncSignal");
    }

    if(NVM_handle.semaphore != NULL && NVM_handle.task == NULL)
    {
        NVM_handle.task =  OSAL_SCHED_startTask(NVM_APP_writeTask,
                                                NULL,
                                                priority,
                                                (uint8_t*) writeTaskStack,
                                                WRITE_STACK_SIZE_BYTE,
                                                OSAL_OS_START_TASK_FLG_NONE,
                                                "NVM_APP_WriteAsyncTask");
        error = NVM_ERR_SUCCESS;
    }
    return error;
}

/*!
 * \brief
 * Close NVM library.
 *
 * \return        NVM_err_t as uint32_t.
 * \retval        NVM_ERR_SUCCESS           Success.
 *
 * \see NVM_APP_init
 *
 * \ingroup NVM_APP
 *
 */
uint32_t NVM_APP_close(void)
{
    uint32_t error = NVM_ERR_SUCCESS;

    if(NVM_handle.semaphore != NULL)
    {
        OSAL_deleteSignal(NVM_handle.semaphore);
        NVM_handle.semaphore = NULL;
    }

    if(NVM_handle.task != NULL)
    {
        OSAL_SCHED_exitTask(NVM_handle.task);
        NVM_handle.task = NULL;
    }
    return error;
}

/*!
 * \brief
 * Register write callback.
 *
 * \details
 * The registered function is called when the asynchronous write function finishes to write data
 * or when something went wrong.
 *
 * \param[in]     cb                        Write callback function.
 *
 * \return        NVM_err_t as uint32_t.
 * \retval        NVM_ERR_SUCCESS           Success.
 * \retval        NVM_ERR_INVALID           Something went wrong.
 *
 * \see NVM_APP_write
 *
 * \ingroup NVM_APP
 *
 */
uint32_t NVM_APP_registerCallback(NVM_APP_writeCallback_t cb)
{
    uint32_t error = NVM_ERR_INVALID;
    if(cb != NULL)
    {
        NVM_handle.callback = cb;
        error = NVM_ERR_SUCCESS;
    }
    return error;
}

/*!
 * \brief
 * Read data from persistent data storage.
 *
 * \details
 * Read some data from persistent storage. The function is a blocking function.
 *
 * \param[in]     type                      Type of persistent storage.
 * \param[in]     id                        Device ID from sysconfig.
 * \param[in]     offset                    Offset on storage device.
 * \param[in]     length                    Data length in bytes.
 * \param[in]     pData                     Data buffer.
 *
 * \return        NVM_err_t as uint32_t.
 * \retval        NVM_ERR_SUCCESS           Success.
 * \retval        NVM_ERR_INVALID           Something went wrong.
 *
 * \see NVM_APP_write NVM_APP_writeAsync
 *
 * \ingroup NVM_APP
 *
 */
uint32_t NVM_APP_read(
    const NVM_type_t type,
    const uint32_t id,
    const uint32_t offset,
    const uint32_t length,
    void * const pData)
{
    uint32_t error = NVM_ERR_SUCCESS;
    switch (type)
    {
        case NVM_TYPE_EEPROM:
        {
            error = NVM_DRV_EEPROM_read(id, offset, length, pData);
            break;
        }
        case NVM_TYPE_FLASH:
        {
            error = NVM_DRV_FLASH_read(id, offset, length, pData);
            break;
        }
        default:
            error = NVM_ERR_INVALID;
    }
    return error;
}

/*!
 * \brief
 * Write data to persistent data storage.
 *
 * \details
 * Write some data to persistent storage. The function is a blocking function.
 *
 * \param[in]     type                      Type of persistent storage.
 * \param[in]     id                        Device ID from sysconfig.
 * \param[in]     offset                    Offset on storage device.
 * \param[in]     length                    Data length in bytes.
 * \param[in]     pData                     Data buffer.
 *
 * \return        NVM_err_t as uint32_t.
 * \retval        NVM_ERR_SUCCESS           Success.
 * \retval        NVM_ERR_INVALID           Something went wrong.
 *
 * \see NVM_APP_read NVM_APP_writeAsync
 *
 * \ingroup NVM_APP
 *
 */
uint32_t NVM_APP_write(
    const NVM_type_t type,
    const uint32_t id,
    const uint32_t offset,
    const uint32_t length,
    const void * const pData)
{
    uint32_t error = NVM_ERR_SUCCESS;
    switch (type)
    {
        case NVM_TYPE_EEPROM:
        {
            error = NVM_DRV_EEPROM_write(id, offset, length, pData);
            break;
        }
        case NVM_TYPE_FLASH:
        {
            error = NVM_DRV_FLASH_write(id, offset, length, pData);
            break;
        }
        default:
        {
            error = NVM_ERR_INVALID;
        }
    }
    return error;
}

/*!
 * \brief
 * Write data to persistent data storage.
 *
 * \details
 * This tasks writes data to persistent storage.
 *
 * \param[in]     pArg                      Thread arguments.
 *
 * \see NVM_APP_read NVM_APP_write NVM_APP_registerCallback
 *
 * \ingroup NVM_APP
 *
 */
OSAL_FUNC_NORETURN static void NVM_APP_writeTask(void *pArg)
{
    uint32_t result = NVM_ERR_SUCCESS;
    int32_t error = OSAL_ERR_InvalidParm;

    OSALUNREF_PARM(pArg);

    while (true)
    {
        //wait for signal
        error = OSAL_waitSignal(NVM_handle.semaphore, OSAL_WAIT_INFINITE);
        //Reject more NVM_APP_writeAsync calls

        if (error == OSAL_ERR_NoError)
        {
            result = NVM_APP_write(NVM_handle.data.type,
                                   NVM_handle.data.id,
                                   NVM_handle.data.offset,
                                   NVM_handle.data.length,
                                   NVM_handle.data.pData);
            //Allow NVM_APP_writeAsync calls
            NVM_handle.status = NVM_ERR_SUCCESS;

            //call callback with write result
            if(NVM_handle.callback != NULL)
            {
                NVM_handle.callback(result);
            }
        }
        else
        {
            OSAL_SCHED_yield();
        }
    }
}

/*!
 * \brief
 * Write data to persistent data storage.
 *
 * \details
 * Write some data to persistent storage. The function is a non blocking function.
 * This function does NOT copy the data to another buffer, therefore please ensure
 * that the buffer is not destroyed after calling this function.
 *
 * Hint: Use the registered callback to free the data buffer.
 *
 * \param[in]     type                      Type of persistent storage.
 * \param[in]     id                        Device ID from sysconfig.
 * \param[in]     offset                    Offset on storage device.
 * \param[in]     length                    Data length in bytes.
 * \param[in]     pData                     Data buffer.
 *
 * \return        NVM_err_t as uint32_t.
 * \retval        NVM_ERR_SUCCESS           Success.
 * \retval        NVM_ERR_BUSY              Write task is busy.
 *
 * \see NVM_APP_read NVM_APP_write NVM_APP_registerCallback
 *
 * \ingroup NVM_APP
 *
 */
uint32_t NVM_APP_writeAsync(
    const NVM_type_t type,
    const uint32_t id,
    const uint32_t offset,
    const uint32_t length,
    const void * const pData)
{
    uint32_t error = NVM_ERR_BUSY;

    if(NVM_handle.status != NVM_ERR_BUSY)
    {
        NVM_handle.status = NVM_ERR_BUSY;

        NVM_handle.data.type = type;
        NVM_handle.data.id = id;
        NVM_handle.data.offset = offset;
        NVM_handle.data.length = length;
        NVM_handle.data.pData = pData;

        //push to queue and send signal to write thread
        OSAL_postSignal(NVM_handle.semaphore);
        error = NVM_ERR_SUCCESS;
    }

    return error;
}
