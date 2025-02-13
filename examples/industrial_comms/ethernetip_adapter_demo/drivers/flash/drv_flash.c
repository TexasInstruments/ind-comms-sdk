/*!
 *  \file drv_flash.c
 *
 *  \brief
 *  FLASH application driver.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2025 Texas Instruments Incorporated
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

#include <stdbool.h>
#include <string.h>

#include "portmacro.h"
#include "FreeRTOSConfig.h"

#include "ti_board_open_close.h"

#include "board/flash.h"

#include "osal.h"

#include "drivers/flash/drv_flash.h"

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
#define DRV_FLASH_WRITE_STACK_SIZE_BYTE     1024
#define DRV_FLASH_WRITE_STACK_SIZE          (DRV_FLASH_WRITE_STACK_SIZE_BYTE/sizeof(configSTACK_DEPTH_TYPE))

static StackType_t DRV_FLASH_writeTaskStack_s[DRV_FLASH_WRITE_STACK_SIZE] __attribute__((aligned(32), section(".threadstack"))) = {0};


typedef void (*DRV_FLASH_fnWriteTask) (void* pArg);

typedef struct DRV_FLASH_writeParam
{
    void*    handle;
    uint32_t offset;
    uint8_t* pData;
    uint32_t length;
    bool     blocking;
}DRV_FLASH_writeParam_t;

typedef struct DRV_FLASH_Write
{
    struct
    {
        void*                 handle;
        DRV_FLASH_fnWriteTask func;
        OSAL_TASK_Priority_t  prio;
    }task;
    struct
    {
        uint32_t           count;
        bool               pending;
        void*              start;
        void*              finished;
    }req;
    DRV_FLASH_writeParam_t param;
}DRV_FLASH_Write_t;

typedef struct DRV_Flash
{
    bool               isActive;
    DRV_FLASH_Write_t  write;
}DRV_Flash_t;

static void DRV_FLASH_writeTask (void* pArg);

static DRV_Flash_t DRV_Flash_s = {0};
#endif

/*!
*
*  \brief
*  Provides specific FLASH handle defined by instance.
*
*  \param[in]  instance       SysConfig ID of FLASH
*
*  \return     requested FLASH handler
*
*  \retval     NULL           Failed.
*  \retval     Other          Success.
*
*/
Flash_Handle DRV_FLASH_getHandle(uint32_t instanceId)
{
    Flash_Handle handle = NULL;

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    if (CONFIG_FLASH_NUM_INSTANCES > instanceId)
    {
        handle = gFlashHandle[instanceId];
    }
#else
    OSALUNREF_PARM(instanceId);
#endif

    return handle;
}

/*!
*
*  \brief
*  Provides FLASH object initialization.
*
*  \param[in]  taskPrio       FLASH write task priority
*
*  \return     uint32_t                      Error code
*
*  \retval     OSAL_NO_ERROR                 Success.
*  \retval     OSAL_GENERAL_ERROR            Negative default value.
*  \retval     OSAL_FLASH_DRV_WRITE_START    FLASH write start signal error.
*  \retval     OSAL_FLASH_DRV_WRITE_FINISHED FLASH write finished signal error.
*  \retval     OSAL_FLASH_DRV_WRITE_TASK     FLASH write task create error.
*
*/
uint32_t DRV_FLASH_init (uint32_t taskPrio)
{
    uint32_t error  = OSAL_GENERAL_ERROR;

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)

    DRV_Flash_s.write.req.start = OSAL_createSignal("FlashWriteSignal");

    if (NULL == DRV_Flash_s.write.req.start)
    {
        error = OSAL_FLASH_DRV_WRITE_START;
        goto laError;
    }

    DRV_Flash_s.write.req.finished = OSAL_createSignal("FlashWriteFinishedSignal");

    if (NULL == DRV_Flash_s.write.req.finished)
    {
        error = OSAL_FLASH_DRV_WRITE_FINISHED;
        goto laError;
    }

    DRV_Flash_s.write.task.prio = taskPrio;
    DRV_Flash_s.write.task.func = DRV_FLASH_writeTask;

    DRV_Flash_s.write.task.handle = OSAL_SCHED_startTask(DRV_Flash_s.write.task.func,
                                                         &DRV_Flash_s.write.param,
                                                         DRV_Flash_s.write.task.prio,
                                                         (uint8_t*) DRV_FLASH_writeTaskStack_s,
                                                         DRV_FLASH_WRITE_STACK_SIZE_BYTE,
                                                         OSAL_OS_START_TASK_FLG_NONE,
                                                         "WriteFlash");
    if (NULL == DRV_Flash_s.write.task.handle)
    {
        error = OSAL_FLASH_DRV_WRITE_TASK;
        goto laError;
    }

    DRV_Flash_s.isActive = true;

    error = OSAL_NO_ERROR;

#else

    error = OSAL_NO_ERROR;
    goto laError;
#endif

laError:

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    if (OSAL_NO_ERROR != error)
    {
        if(NULL != DRV_Flash_s.write.req.start)
        {
            OSAL_deleteSignal(DRV_Flash_s.write.req.start);
            DRV_Flash_s.write.req.start = NULL;
        }

        if(NULL != DRV_Flash_s.write.req.finished)
        {
            OSAL_deleteSignal(DRV_Flash_s.write.req.finished);
            DRV_Flash_s.write.req.finished = NULL;
        }
    }
#endif

    return error;
}

/*!
*
*  \brief
*  Provides FLASH object de-initialization.
*
*  \return     uint32_t          Error code
*
*  \retval     OSAL_NO_ERROR     Success.
*
*/
uint32_t DRV_FLASH_deInit (void)
{
#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    if (false == DRV_Flash_s.isActive)
    {
        goto laError;
    }

    while(0 != DRV_Flash_s.write.req.count)
    {
        OSAL_SCHED_sleep(10);
    }

    DRV_FLASH_write((void*) 0xFFFFFFFF,
                    0xFFFFFFFF,
                    (uint8_t*) 0xFFFFFFFF,
                    0xFFFFFFFF,
                    true);

    if(NULL != DRV_Flash_s.write.req.start)
    {
        OSAL_deleteSignal(DRV_Flash_s.write.req.start);
        DRV_Flash_s.write.req.start = NULL;
    }

    if(NULL != DRV_Flash_s.write.req.finished)
    {
        OSAL_deleteSignal(DRV_Flash_s.write.req.finished);
        DRV_Flash_s.write.req.finished = NULL;
    }

laError:
#endif

    return OSAL_NO_ERROR;
}

/*!
*
* \brief
* Reads non-volatile configuration data stored in FLASH.
*
*  \param[in]  handle        FLASH device handle.
*  \param[in]  offset_p      Non-volatile data memory offset where data need to be written.
*  \param[in]  pBuf_p        Pointer to read buffer.
*  \param[in]  length_p      Length of data to be read.
*
*  \return     uint32_t                                 Error code.
*
*  \retval     OSAL_NO_ERROR                            Success.
*  \retval     OSAL_GENERAL_ERROR                       Negative default value.
*  \retval     OSAL_FLASH_DRV_HANDLE_INVALID            FLASH handle set to NULL.
*  \retval     OSAL_FLASH_DRV_READ                      Flash_read call failed.
*
*
*/
uint32_t DRV_FLASH_read (Flash_Handle  handle, uint32_t offset, const uint8_t* pBuf, uint32_t length)
{
    uint32_t err    = OSAL_GENERAL_ERROR;

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    int32_t  ret    = SystemP_FAILURE;

    if (NULL == handle)
    {
        err = OSAL_FLASH_DRV_HANDLE_INVALID;
        goto laError;
    }

    ret = Flash_read (handle, offset, (uint8_t*) pBuf, length);

    if (SystemP_SUCCESS != ret)
    {
        err = OSAL_FLASH_DRV_READ;
        goto laError;
    }

    err = OSAL_NO_ERROR;

laError:
#endif

    return err;
}

/*!
*
* \brief
* Provides FLASH non-volatile data write operation status.
*
*  \return     bool        Status of FLASH non-volatile data write operation.
*
*  \retval     false       Not pending.
*  \retval     true        Pending.
*
*/
bool DRV_FLASH_isWritePending (void)
{
    bool ret = true;
#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    if (DRV_Flash_s.write.req.count == 0)
    {
        ret = false;
    }
#else

    ret = false;
#endif

    return ret;
}

/*!
* <!-- Description: -->
*
* \brief
* Writes non-volatile configuration data.
*
*  <!-- Parameters and return values: -->
*
*  \param[in]  handle          FLASH device handle.
*  \param[in]  offset          Non-volatile data memory offset where data need to be written.
*  \param[in]  pBuf            Pointer to write buffer.
*  \param[in]  length          Length of data to be read.
*  \param[in]  blocking        Waits until operation is finished.
*
*  \return     uint32_t                             Error code.
*
*  \retval     OSAL_NO_ERROR                        Success.
*  \retval     OSAL_GENERAL_ERROR                   Failed.
*  \retval     OSAL_FLASH_DRV_HANDLE_INVALID        Flash driver handler set to NULL.
*  \retval     OSAL_FLASH_DRV_DATA_INVALID          Flash write data buffer allocation failed or input buffer set to NULL.
*  \retval     OSAL_FLASH_DRV_LENGTH_INVALID        Flash data have 0 length
*
*/
uint32_t DRV_FLASH_write (Flash_Handle   handle,
                          uint32_t       offset,
                          const uint8_t* pBuf,
                          uint32_t       length,
                          bool           blocking)
{
    uint32_t err = OSAL_GENERAL_ERROR;

    if (NULL == handle)
    {
        err = OSAL_FLASH_DRV_HANDLE_INVALID;
        goto laError;
    }

    if (0 == length)
    {
        err = OSAL_FLASH_DRV_LENGTH_INVALID;
        goto laError;
    }

    if (NULL == pBuf)
    {
        err = OSAL_FLASH_DRV_DATA_INVALID;
        goto laError;
    }

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    DRV_Flash_s.write.req.count++;

    // Check if last write to FLASH is pending and wait in case that yes
    if (true == DRV_Flash_s.write.req.pending)
    {
        OSAL_waitSignal(DRV_Flash_s.write.req.finished, OSAL_WAIT_INFINITE);

        DRV_Flash_s.write.req.pending = false;
    }

    // allocate memory for data to be written
    DRV_Flash_s.write.param.handle   = handle;
    DRV_Flash_s.write.param.offset   = offset;
    DRV_Flash_s.write.param.pData    = (uint8_t*) pBuf;
    DRV_Flash_s.write.param.length   = length;
    DRV_Flash_s.write.param.blocking = blocking;

    OSAL_postSignal(DRV_Flash_s.write.req.start);

    if (true == blocking)
    {
        OSAL_waitSignal(DRV_Flash_s.write.req.finished, OSAL_WAIT_INFINITE);

        DRV_Flash_s.write.req.pending = false;
    }

    err = OSAL_NO_ERROR;
#endif

laError:

    return err;
}

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
/*!
*
* \brief
* Task function responsible for writing of data to the FLASH.
*
*  \param[in]  pArg Pointer to task argument of CUST_DRIVERS_SPermWriteParam_t type.
*
*/
static void DRV_FLASH_writeTask (void *pArg)
{
    DRV_FLASH_writeParam_t *pParam = (DRV_FLASH_writeParam_t*) pArg;

    const Flash_Attrs *pAttr;
    int32_t            err;
    uint32_t           iOffset;
    uint32_t           block;
    uint32_t           page;

    while(1)
    {
        OSAL_waitSignal(DRV_Flash_s.write.req.start, OSAL_WAIT_INFINITE);

        DRV_Flash_s.write.req.pending = true;

        if (((void*) 0xFFFFFFFF    == pParam->handle) &&
            ((uint8_t*) 0xFFFFFFFF == pParam->pData)  &&
            (0xFFFFFFFF == pParam->offset) &&
            (0xFFFFFFFF == pParam->length))
        {
            break;
        }

        pAttr = ((Flash_Config*) pParam->handle)->attrs;

        err = SystemP_SUCCESS;

        for (iOffset = pParam->offset; iOffset <= pParam->offset + pParam->length; iOffset += pAttr->blockSize)
        {
            err = Flash_offsetToBlkPage ((Flash_Handle) pParam->handle, iOffset, &block, &page);
            if (SystemP_SUCCESS != err)
            {
                OSAL_error (__func__, __LINE__, OSAL_FLASH_DRV_OFFSETTOBLKPAGE, true, 0);
                break;
            }

            err = Flash_eraseBlk ((Flash_Handle) pParam->handle, block);
            if (SystemP_SUCCESS != err)
            {
                OSAL_error (__func__, __LINE__, OSAL_FLASH_DRV_ERASEBLK, true, 0);
                break;
            }
        }

        err = Flash_write ((Flash_Handle) pParam->handle, pParam->offset, pParam->pData, pParam->length);
        if (SystemP_SUCCESS != err)
        {
            OSAL_error (__func__, __LINE__, OSAL_FLASH_DRV_WRITE, true, 0);
            goto laError;
        }

        DRV_Flash_s.write.req.count--;

        OSAL_postSignal(DRV_Flash_s.write.req.finished);
    }

laError:

    OSAL_postSignal(DRV_Flash_s.write.req.finished);

    OSAL_SCHED_exitTask(NULL);
}
#endif
