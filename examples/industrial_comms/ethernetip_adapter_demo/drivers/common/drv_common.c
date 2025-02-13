/*!
 *  \file drv_common.c
 *
 *  \brief
 *  Common support for application drivers.
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

#include "kernel/dpl/SemaphoreP.h"

#include "drivers/common/drv_common.h"

typedef struct DRV_COMMON_Mutex_container
{
    SemaphoreP_Object mutex_handle;
    uint8_t           isInitialized;
}DRV_COMMON_Mutex_container_t;

static DRV_COMMON_Mutex_container_t shared_resources_container[DRV_COMMON_MUTEX_TOTALCOUNT] = {0};


/**
 * \brief
 * Initializes shared resources Mutex handles
 * \return
 * returns DRV_COMMON_MUTEX_eERR_NOERROR when successful, otherwise error code.
 */
DRV_COMMON_Mutex_EError_t DRV_COMMON_Mutex_init(void)
{
    DRV_COMMON_Mutex_EError_t retVal = DRV_COMMON_MUTEX_eERR_NOERROR;

    for(uint16_t i=0; i<DRV_COMMON_MUTEX_TOTALCOUNT; i++)
    {
        if(SystemP_FAILURE == SemaphoreP_constructMutex(&shared_resources_container[i].mutex_handle))
        {
            retVal = DRV_COMMON_MUTEX_eERR_INIT ;
            shared_resources_container[i].isInitialized = 0;
            break;
        }
        else
        {
            shared_resources_container[i].isInitialized = 1;
        }
    }

    return retVal;
}

/**
 * \brief
 * Lock the given shared resource
 * \param[in] mutexType the type of the shared resource to lock
 * \param[in] timeout timeout to wait for the shared resource to become available
 * \return
 * returns DRV_COMMON_MUTEX_eERR_NOERROR when successful, otherwise error code.
 */
DRV_COMMON_Mutex_EError_t DRV_COMMON_Mutex_Lock(DRV_COMMON_Mutex_Type_t mutexType, uint32_t timeout)
{
    DRV_COMMON_Mutex_EError_t retVal = DRV_COMMON_MUTEX_eERR_NOERROR;
    int32_t SemaphoreRetVal;

    if(mutexType >= DRV_COMMON_MUTEX_TOTALCOUNT)
    {
        retVal = DRV_COMMON_MUTEX_eERR_INVALID_ARG ;
    }
    else if(0 == shared_resources_container[mutexType].isInitialized)
    {
        retVal = DRV_COMMON_MUTEX_eERR_NOT_INITIALIZED ;
    }
    else
    {
        SemaphoreRetVal = SemaphoreP_pend(&shared_resources_container[mutexType].mutex_handle, timeout);
        if(SystemP_FAILURE == SemaphoreRetVal )
        {
            retVal = DRV_COMMON_MUTEX_eERR_LOCK_FAILED ;
        }
        else if(SystemP_TIMEOUT == SemaphoreRetVal )
        {
            retVal = DRV_COMMON_MUTEX_eERR_LOCK_TIMEOUT ;
        }

    }

    return retVal;
}


/**
 * \brief
 * Unlocks the shared resource
 * \param[in] mutexType the type of the shared resource to unlock
 * \return
 * returns DRV_COMMON_MUTEX_eERR_NOERROR when successful, otherwise error code.
 */
DRV_COMMON_Mutex_EError_t DRV_COMMON_Mutex_Unlock(DRV_COMMON_Mutex_Type_t mutexType)
{
    DRV_COMMON_Mutex_EError_t retVal = DRV_COMMON_MUTEX_eERR_NOERROR;
    if(mutexType >= DRV_COMMON_MUTEX_TOTALCOUNT)
    {
        retVal = DRV_COMMON_MUTEX_eERR_INVALID_ARG;
    }
    else if(0 == shared_resources_container[mutexType].isInitialized)
    {
        retVal = DRV_COMMON_MUTEX_eERR_NOT_INITIALIZED ;
    }
    else
    {
        SemaphoreP_post(&shared_resources_container[mutexType].mutex_handle);
    }

    return retVal;
}
