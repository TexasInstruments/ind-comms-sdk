/*!
 *  \file nvm.h
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

#ifndef NVM_H
#define NVM_H

#include <stdint.h>
#include "osal.h"

/*!
 * \brief
 * Write callback function type.
 *
 * \details
 * This function is called when the asynchronous write function finishes to write data
 * or when something went wrong.
 *
 * \remarks
 * Only for non-blocking write function.
 *
 * \param[in]     status                         result of the write task.
 *
 * \ingroup NVM_APP
 *
 */
typedef void(*NVM_APP_writeCallback_t)(uint32_t status);

typedef enum NVM_err {
    NVM_ERR_SUCCESS = 0,
    NVM_ERR_FAIL,
    NVM_ERR_BUSY,
    NVM_ERR_REJECT,
    NVM_ERR_INVALID,
    NVM_ERR_END_ENUM
} NVM_err_t;

typedef enum NVM_type {
    NVM_TYPE_EEPROM = 0,
    NVM_TYPE_FLASH,
    NVM_TYPE_END_NUM
} NVM_type_t;

/******************** Public Interface Functions **********************/

extern uint32_t NVM_APP_registerCallback(NVM_APP_writeCallback_t cb);

extern uint32_t NVM_APP_init(
    const OSAL_TASK_Priority_t priority);

extern uint32_t NVM_APP_close(void);

extern uint32_t NVM_APP_read(
    const NVM_type_t type,
    const uint32_t id,
    const uint32_t offset,
    const uint32_t length,
    void * const pData);

extern uint32_t NVM_APP_write(
    const NVM_type_t type,
    const uint32_t id,
    const uint32_t offset,
    const uint32_t length,
    const void * const pData);

extern uint32_t NVM_APP_writeAsync(
    const NVM_type_t type,
    const uint32_t id,
    const uint32_t offset,
    const uint32_t length,
    const void * const pData);

#endif // NVM_H
