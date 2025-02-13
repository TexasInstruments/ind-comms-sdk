/*!
 *  \file drv_eeprom.c
 *
 *  \brief
 *  EEPROM application driver.
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

#include "board/eeprom.h"

#include "osal.h"

#include "drivers/common/drv_common.h"
#include "drivers/eeprom/drv_eeprom.h"

#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
#define DRV_EEPROM_WRITE_STACK_SIZE_BYTE     1024
#define DRV_EEPROM_WRITE_STACK_SIZE          (DRV_EEPROM_WRITE_STACK_SIZE_BYTE/sizeof(configSTACK_DEPTH_TYPE))

static StackType_t DRV_EEPROM_writeTaskStack_s[DRV_EEPROM_WRITE_STACK_SIZE] __attribute__((aligned(32), section(".threadstack"))) = {0};

typedef void (*DRV_EEPROM_fnWriteTask) (void* pArg);

typedef struct DRV_EEPROM_writeParam
{
    void*    handle;
    uint32_t offset;
    uint8_t* pData;
    uint32_t length;
    bool     blocking;
}DRV_EEPROM_writeParam_t;

typedef struct DRV_EEPROM_Write
{
    struct
    {
        void*                  handle;
        DRV_EEPROM_fnWriteTask func;
        OSAL_TASK_Priority_t   prio;
    }task;
    struct
    {
        uint32_t           count;
        bool               pending;
        void*              start;
        void*              finished;
    }req;
    DRV_EEPROM_writeParam_t param;
}DRV_EEPROM_Write_t;

typedef struct DRV_Eeprom
{
    bool                isActive;
    DRV_EEPROM_Write_t  write;
}DRV_Eeprom_t;

static void DRV_EEPROM_writeTask (void* pArg);

static DRV_Eeprom_t DRV_Eeprom_s = {0};
#endif

/*!
*
*  \brief
*  Provides specific EEPROM handle defined by instance.
*
*  \param[in]  instance       SysConfig ID of EEPROM
*
*  \return     requested EEPROM handler
*
*  \retval     NULL           Failed.
*  \retval     Other          Success.
*
*/
EEPROM_Handle DRV_EEPROM_getHandle(uint32_t instanceId)
{
    EEPROM_Handle handle = NULL;

#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
    if (CONFIG_EEPROM_NUM_INSTANCES > instanceId)
    {
        handle = gEepromHandle[instanceId];
    }
#else
    OSALUNREF_PARM(instanceId);
#endif

    return handle;
}

/*!
*
*  \brief
*  Provides EEPROM object initialization.
*
*  \param[in]  taskPrio       EEPROM write task priority
*
*  \return     uint32_t                      Error code
*
*  \retval     OSAL_NO_ERROR                 Success.
*  \retval     OSAL_GENERAL_ERROR            Negative default value.
*  \retval     OSAL_EE_DRV_WRITE_START       EEPROM write start signal error.
*  \retval     OSAL_EE_DRV_WRITE_FINISHED    EEPROM write finished signal error.
*  \retval     OSAL_EE_DRV_WRITE_TASK        EEPROM write task create error.
*
*/
uint32_t DRV_EEPROM_init (uint32_t taskPrio)
{
    uint32_t error  = OSAL_GENERAL_ERROR;

#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)

    DRV_Eeprom_s.write.req.start = OSAL_createSignal("EepromWriteSignal");

    if (NULL == DRV_Eeprom_s.write.req.start)
    {
        error = OSAL_EE_DRV_WRITE_START;
        goto laError;
    }

    DRV_Eeprom_s.write.req.finished = OSAL_createSignal("EepromWriteFinishedSignal");

    if (NULL == DRV_Eeprom_s.write.req.finished)
    {
        error = OSAL_EE_DRV_WRITE_FINISHED;
        goto laError;
    }

    DRV_Eeprom_s.write.task.prio = taskPrio;
    DRV_Eeprom_s.write.task.func = DRV_EEPROM_writeTask;

    DRV_Eeprom_s.write.task.handle = OSAL_SCHED_startTask(DRV_Eeprom_s.write.task.func,
                                                          &DRV_Eeprom_s.write.param,
                                                          DRV_Eeprom_s.write.task.prio,
                                                          (uint8_t*) DRV_EEPROM_writeTaskStack_s,
                                                          DRV_EEPROM_WRITE_STACK_SIZE_BYTE,
                                                          OSAL_OS_START_TASK_FLG_NONE,
                                                          "WriteEeprom");
    if (NULL == DRV_Eeprom_s.write.task.handle)
    {
        error = OSAL_EE_DRV_WRITE_TASK;
        goto laError;
    }

    DRV_Eeprom_s.isActive = true;

    error = OSAL_NO_ERROR;

#else
    DRV_Eeprom_s.isActive = false;

    error = OSAL_NO_ERROR;
#endif

laError:
#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
    if (OSAL_NO_ERROR != error)
    {
        if(NULL != DRV_Eeprom_s.write.req.start)
        {
            OSAL_deleteSignal(DRV_Eeprom_s.write.req.start);
            DRV_Eeprom_s.write.req.start = NULL;
        }

        if(NULL != DRV_Eeprom_s.write.req.finished)
        {
            OSAL_deleteSignal(DRV_Eeprom_s.write.req.finished);
            DRV_Eeprom_s.write.req.finished = NULL;
        }
    }
#endif

    return error;
}

/*!
*
*  \brief
*  Provides EEPROM object de-initialization.
*
*  \return     uint32_t          Error code
*
*  \retval     OSAL_NO_ERROR     Success.
*
*/
uint32_t DRV_EEPROM_deInit (void)
{
#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
    if (false == DRV_Eeprom_s.isActive)
    {
        goto laError;
    }

    while(0 != DRV_Eeprom_s.write.req.count)
    {
        OSAL_SCHED_sleep(10);
    }

    DRV_EEPROM_write((void*) 0xFFFFFFFF,
                     0xFFFFFFFF,
                     (uint8_t*) 0xFFFFFFFF,
                     0xFFFFFFFF,
                     true);

    if(NULL != DRV_Eeprom_s.write.req.start)
    {
        OSAL_deleteSignal(DRV_Eeprom_s.write.req.start);
        DRV_Eeprom_s.write.req.start = NULL;
    }

    if(NULL != DRV_Eeprom_s.write.req.finished)
    {
        OSAL_deleteSignal(DRV_Eeprom_s.write.req.finished);
        DRV_Eeprom_s.write.req.finished = NULL;
    }

laError:
#endif

    return OSAL_NO_ERROR;
}

/*!
*
*  \brief
*  Reads non-volatile configuration data stored in EEPROM.
*
*  \param[in]  handle          EEPROM device handle.
*  \param[in]  offset_p        Non-volatile data memory offset where data need to be written.
*  \param[in]  pBuf_p          Pointer to read buffer.
*  \param[in]  length_p        Length of data to be read.
*
*  \return     uint32_t                                 Error code.
*
*  \retval     OSAL_NO_ERROR                            Success.
*  \retval     OSAL_GENERAL_ERROR                       Negative default value.
*  \retval     OSAL_EE_DRV_HANDLE_INVALID               EEPROM handle set to NULL.
*  \retval     OSAL_EE_DRV_READ                         EEPROM_read call failed.
*
*/
uint32_t DRV_EEPROM_read (EEPROM_Handle handle, uint32_t offset, const uint8_t* pBuf, uint32_t length)
{
    uint32_t err    = OSAL_GENERAL_ERROR;
    DRV_COMMON_Mutex_EError_t   mutexErr;

#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
    int32_t  ret    = SystemP_FAILURE;

    if (NULL == handle)
    {
        err = OSAL_EE_DRV_HANDLE_INVALID;
        goto laError;
    }

    mutexErr = DRV_COMMON_Mutex_Lock(DRV_COMMON_MUTEX_I2C, 2);
    if(DRV_COMMON_MUTEX_eERR_NOERROR == mutexErr)
    {
        ret = EEPROM_read (handle, offset, (uint8_t*) pBuf, length);

        DRV_COMMON_Mutex_Unlock(DRV_COMMON_MUTEX_I2C);

        if (SystemP_SUCCESS != ret)
        {
            err = OSAL_EE_DRV_READ;
            goto laError;
        }
    }
    else
    {
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
* Provides EEPROM non-volatile data write operation status.
*
*  \return     bool        Status of EEPROM non-volatile data write operation.
*
*  \retval     false       Not pending.
*  \retval     true        Pending.
*
*/
bool DRV_EEPROM_isWritePending (void)
{
    bool ret = true;

#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
    if (DRV_Eeprom_s.write.req.count == 0)
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
* Writes non-volatile configuration data to EEPROM specified by handle.
*
*  <!-- Parameters and return values: -->
*
*  \param[in]  handle          EEPROM device handle.
*  \param[in]  offset          Non-volatile data memory offset where data need to be written.
*  \param[in]  pBuf            Pointer to write buffer.
*  \param[in]  length          Length of data to be read.
*  \param[in]  blocking        Waits until operation is finished.
*
*  \return     uint32_t                             Error code.
*
*  \retval     OSAL_NO_ERROR                        Success.
*  \retval     OSAL_GENERAL_ERROR                   Negative default value.
*  \retval     OSAL_EE_DRV_HANDLE_INVALID           EEPROM driver handler set to NULL.
*  \retval     OSAL_EE_DRV_DATA_INVALID             EEPROM write data buffer allocation failed or input buffer set to NULL.
*  \retval     OSAL_EE_DRV_LENGTH_INVALID           EEPROM data have 0 length
*
*/
uint32_t DRV_EEPROM_write (EEPROM_Handle  handle,
                           uint32_t       offset,
                           const uint8_t* pBuf,
                           uint32_t       length,
                           bool           blocking)
{
    uint32_t err = OSAL_GENERAL_ERROR;

    if (NULL == handle)
    {
        err = OSAL_EE_DRV_HANDLE_INVALID;
        goto laError;
    }

    if (0 == length)
    {
        err = OSAL_EE_DRV_LENGTH_INVALID;
        goto laError;
    }

    if (NULL == pBuf)
    {
        err = OSAL_EE_DRV_DATA_INVALID;
        goto laError;
    }

#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
    DRV_Eeprom_s.write.req.count++;

    // Check if last write to EEPROM is pending and wait in case that yes
    if (true == DRV_Eeprom_s.write.req.pending)
    {
        OSAL_waitSignal(DRV_Eeprom_s.write.req.finished, OSAL_WAIT_INFINITE);

        DRV_Eeprom_s.write.req.pending = false;
    }

    // allocate memory for data to be written
    DRV_Eeprom_s.write.param.handle   = handle;
    DRV_Eeprom_s.write.param.offset   = offset;
    DRV_Eeprom_s.write.param.pData    = (uint8_t*) pBuf;
    DRV_Eeprom_s.write.param.length   = length;
    DRV_Eeprom_s.write.param.blocking = blocking;

    OSAL_postSignal(DRV_Eeprom_s.write.req.start);

    if (true == blocking)
    {
        OSAL_waitSignal(DRV_Eeprom_s.write.req.finished, OSAL_WAIT_INFINITE);

        DRV_Eeprom_s.write.req.pending = false;
    }

    err = OSAL_NO_ERROR;
#endif

laError:

    return err;
}

#if (defined CONFIG_EEPROM_NUM_INSTANCES) && (CONFIG_EEPROM_NUM_INSTANCES > 0)
/*!
*
*  \brief
*  Task function responsible for writing of data to the EEPROM.
*
*  \param[in]  pArg Pointer to task argument of DEVICE_PROFILE_NVM_EEPROM_writeParam_t type.
*
*/
static void DRV_EEPROM_writeTask (void *pArg)
{
    DRV_EEPROM_writeParam_t* pParam = (DRV_EEPROM_writeParam_t*) pArg;
    DRV_COMMON_Mutex_EError_t   mutexErr;
    int32_t err;

    while(1)
    {
        OSAL_waitSignal(DRV_Eeprom_s.write.req.start, OSAL_WAIT_INFINITE);

        DRV_Eeprom_s.write.req.pending = true;

        if (((void*) 0xFFFFFFFF    == pParam->handle) &&
            ((uint8_t*) 0xFFFFFFFF == pParam->pData)  &&
            (0xFFFFFFFF == pParam->offset) &&
            (0xFFFFFFFF == pParam->length))
        {
            break;
        }
        //ToDo use mutex to protect I2C access
        mutexErr = DRV_COMMON_Mutex_Lock(DRV_COMMON_MUTEX_I2C, 10);
        if(DRV_COMMON_MUTEX_eERR_NOERROR == mutexErr)
        {
            err = EEPROM_write ((EEPROM_Handle) pParam->handle, pParam->offset, pParam->pData, pParam->length);

            DRV_COMMON_Mutex_Unlock(DRV_COMMON_MUTEX_I2C);

            if (SystemP_SUCCESS != err)
            {
                OSAL_error (__func__, __LINE__, OSAL_EE_DRV_WRITE, true, 0);
                goto laError;
            }

            DRV_Eeprom_s.write.req.count--;

            OSAL_postSignal(DRV_Eeprom_s.write.req.finished);
        }
    }

laError:

    OSAL_postSignal(DRV_Eeprom_s.write.req.finished);

    OSAL_SCHED_exitTask(NULL);
}
#endif

