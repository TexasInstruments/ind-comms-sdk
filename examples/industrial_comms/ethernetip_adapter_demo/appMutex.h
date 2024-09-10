/*!
 *  \file appMutex.h
 *
 *  \brief
 *  Mutex APIs for shared resources.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 None.
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


#ifndef APPMUTEX_H
#define APPMUTEX_H

#include <stdint.h>

/*!
 *  \brief
 *  Mutex status codes.
 */
typedef enum EI_APP_MUTEX_EError
{
    EI_APP_MUTEX_eERR_NOERROR         =  0,  /*!< No error, everything is fine. */
    EI_APP_MUTEX_eERR_LOCK_TIMEOUT    = -1,  /*!< Mutex lock timed out. */
    EI_APP_MUTEX_eERR_LOCK_FAILED     = -2,  /*!< Mutex lock failure. */
    EI_APP_MUTEX_eERR_INIT            = -3,  /*!< Mutex init for shared resources failed. */
    EI_APP_MUTEX_eERR_INVALID_ARG     = -4,  /*!< Invalid Argument.*/
    EI_APP_MUTEX_eERR_NOT_INITIALIZED = -5   /*!< Init is not called, the Mutex is not ready*/

}EI_APP_MUTEX_EError_t;

/**
 * \brief
 * Shared resources type
 */
typedef enum EI_APP_Mutex_Type
{
    EI_APP_Mutex_I2C = 0,
    EI_APP_Mutex_UART,
    EI_APP_Mutex_TotalCount
}EI_APP_Mutex_Type_t;

#ifdef __cplusplus
extern "C" {
#endif

EI_APP_MUTEX_EError_t EI_APP_Mutex_init(void);
EI_APP_MUTEX_EError_t EI_APP_Mutex_Lock(EI_APP_Mutex_Type_t mutexType, uint32_t timeout);
EI_APP_MUTEX_EError_t EI_APP_Mutex_Unlock(EI_APP_Mutex_Type_t mutexType);


#ifdef  __cplusplus
}
#endif

#endif