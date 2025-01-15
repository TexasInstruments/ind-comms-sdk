/*!
 *  \file nvm.c
 *
 *  \brief
 *  Flash driver.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2024 Texas Instruments Incorporated
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

#include "nvm.h"
#include "nvm_drv_flash.h"

#include "ti_board_open_close.h"

/*!
 * \brief
 * Read data from fLash.
 *
 * \param[in]     id                        Device ID from sysconfig.
 * \param[in]     offset                    Offset on storage device.
 * \param[in]     length                    Data length in bytes.
 * \param[in]     pData                     Data buffer.
 *
 * \return        NVM_err_t as uint32_t.
 * \retval        NVM_ERR_SUCCESS           Success.
 * \retval        NVM_ERR_FAIL              Something went wrong when reading from flash.
 * \retval        NVM_ERR_REJECT            Invalid flash ID.
 *
 * \see NVM_DRV_FLASH_write
 *
 * \ingroup NVM_APP
 *
 */
uint32_t NVM_DRV_FLASH_read(
    const uint32_t id,
    const uint32_t offset,
    const uint32_t length,
    void * const pData)
{
    uint32_t error = NVM_ERR_SUCCESS;
#if defined(CONFIG_FLASH_NUM_INSTANCES) && CONFIG_FLASH_NUM_INSTANCES > 0
    int32_t status = SystemP_SUCCESS;

    if(id < CONFIG_FLASH_NUM_INSTANCES)
    {
        status = Flash_read(gFlashHandle[id], offset, pData, length);
        if(status != SystemP_SUCCESS)
        {
            error = NVM_ERR_FAIL;
        }
    }
    else
    {
        error = NVM_ERR_REJECT;
    }
#else
    (void)id;
    (void)offset;
    (void)length;
    (void)pData;
    error = NVM_ERR_REJECT;
#endif

    return error;
}

/*!
 * \brief
 * Write data to Flash by automatically managing the flash block erase.
 * Used to write multiple pages at once.
 *
 * \param[in]     id                        Device ID from sysconfig.
 * \param[in]     offset                    Offset on storage device.
 * \param[in]     length                    Data length in bytes.
 * \param[in]     pData                     Data buffer.
 * \param[in]     forceErase                Block force erase flag. True will erase the blocks everytime this function is called.
 *
 * \return        NVM_err_t as uint32_t.
 * \retval        NVM_ERR_SUCCESS           Success.
 * \retval        NVM_ERR_FAIL              Something went wrong when writing to flash.
 * \retval        NVM_ERR_REJECT            Invalid Flash ID.
 *
 * \see NVM_DRV_FLASH_read
 *
 * \ingroup NVM_DRV
 *
 */
uint32_t NVM_DRV_FLASH_write(
    const uint32_t id,
    const uint32_t offset,
    const uint32_t length,
    const void * const pData,
    const uint32_t forceErase)
{
    uint32_t error  = NVM_ERR_SUCCESS;
#if defined(CONFIG_FLASH_NUM_INSTANCES) && CONFIG_FLASH_NUM_INSTANCES > 0
    int32_t  status = SystemP_SUCCESS;
    static int32_t  erasedBlockIdx  = -1;
    static int32_t  blockSize       = 0;
           int32_t  startBlockIdx   = 0;
           int32_t  endBlockIdx     = 0;

    if(id < CONFIG_FLASH_NUM_INSTANCES)
    {
        if(blockSize == 0)
        {
            Flash_Attrs* flashAttribute = Flash_getAttrs(id);
            blockSize = flashAttribute->blockSize;
        }
        startBlockIdx = offset/blockSize;
        endBlockIdx = (offset + length)/blockSize;
        if(true == forceErase || (((startBlockIdx != erasedBlockIdx) || (endBlockIdx != erasedBlockIdx)) && (endBlockIdx > erasedBlockIdx)))
        {
            for (int32_t blkId = startBlockIdx;blkId <= endBlockIdx;blkId++)
            {
                if (true == forceErase || blkId != erasedBlockIdx)
                {
                    status = Flash_eraseBlk(gFlashHandle[id],blkId);
                    if(status == SystemP_SUCCESS)
                    {
                        erasedBlockIdx = blkId;
                    }
                    else
                    {
                        error = NVM_ERR_FAIL;
                    }
                }
            }
        }
        if(error == NVM_ERR_SUCCESS)
        {
            status = Flash_write(gFlashHandle[id], offset, (uint8_t*)pData, length);
            if(status != SystemP_SUCCESS)
            {
                error = NVM_ERR_FAIL;
            }
        }
    }
    else
    {
        error = NVM_ERR_REJECT;
    }
#else
    (void)id;
    (void)offset;
    (void)length;
    (void)pData;
    (void)forceErase;
    error = NVM_ERR_REJECT;
#endif
    return error;
}
