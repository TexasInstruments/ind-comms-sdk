/*!
 *  \file drv_common.h
 *
 *  \brief
 *  Declarations related to common support of application drivers.
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


#ifndef DRV_COMMON_H
#define DRV_COMMON_H

#include <stdint.h>

/*!
 *  \brief
 *  Mutex status codes.
 */
typedef enum DRV_COMMON_Mutex_EError
{
    DRV_COMMON_MUTEX_eERR_NOERROR         =  0,  /*!< No error, everything is fine. */
    DRV_COMMON_MUTEX_eERR_LOCK_TIMEOUT    = -1,  /*!< Mutex lock timed out. */
    DRV_COMMON_MUTEX_eERR_LOCK_FAILED     = -2,  /*!< Mutex lock failure. */
    DRV_COMMON_MUTEX_eERR_INIT            = -3,  /*!< Mutex init for shared resources failed. */
    DRV_COMMON_MUTEX_eERR_INVALID_ARG     = -4,  /*!< Invalid Argument.*/
    DRV_COMMON_MUTEX_eERR_NOT_INITIALIZED = -5   /*!< Init is not called, the Mutex is not ready*/

}DRV_COMMON_Mutex_EError_t;

/**
 * \brief
 * Shared resources type
 */
typedef enum DRV_COMMON_Mutex_Type
{
    DRV_COMMON_MUTEX_I2C = 0,
    DRV_COMMON_MUTEX_UART,
    DRV_COMMON_MUTEX_TOTALCOUNT
}DRV_COMMON_Mutex_Type_t;

#ifdef __cplusplus
extern "C" {
#endif

DRV_COMMON_Mutex_EError_t DRV_COMMON_Mutex_init(void);
DRV_COMMON_Mutex_EError_t DRV_COMMON_Mutex_Lock(DRV_COMMON_Mutex_Type_t mutexType, uint32_t timeout);
DRV_COMMON_Mutex_EError_t DRV_COMMON_Mutex_Unlock(DRV_COMMON_Mutex_Type_t mutexType);


#ifdef  __cplusplus
}
#endif

#endif // DRV_COMMON_H
