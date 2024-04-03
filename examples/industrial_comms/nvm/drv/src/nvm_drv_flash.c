/*!
 *  \file nvm.c
 *
 *  \brief
 *  Flash driver.
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

    return error;
}

/*!
 * \brief
 * Write data to Flash.
 *
 * \param[in]     id                        Device ID from sysconfig.
 * \param[in]     offset                    Offset on storage device.
 * \param[in]     length                    Data length in bytes.
 * \param[in]     pData                     Data buffer.
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
    const void * const pData)
{
    uint32_t error = NVM_ERR_SUCCESS;
    uint32_t pageCount = 0;
    uint32_t block = 0;
    uint32_t page = 0;
    int32_t status = SystemP_SUCCESS;
    Flash_Attrs* flashAttribute = NULL;

    if(id < CONFIG_FLASH_NUM_INSTANCES)
    {
        flashAttribute = Flash_getAttrs(id);
        pageCount = (length + flashAttribute->blockSize - 1) / flashAttribute->blockSize;
        for (uint32_t i = 0; i < pageCount; i++)
        {
            status = Flash_offsetToBlkPage(gFlashHandle[id],
                                          offset+(i* flashAttribute->blockSize),
                                          &block,
                                          &page);
            if(status == SystemP_SUCCESS)
            {
                status = Flash_eraseBlk(gFlashHandle[id], block);
            }
            if(status != SystemP_SUCCESS)
            {
                error = NVM_ERR_FAIL;
                break;
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
    return error;
}