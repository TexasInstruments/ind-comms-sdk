/*!
 *  \file device_profile_nvm.h
 *
 *  \brief
 *  Declarations related to device profile non volatile memory data storage.
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


#ifndef DEVICE_PROFILE_NVM_H
#define DEVICE_PROFILE_NVM_H

#include <stdbool.h>
#include <stdint.h>

// application error code definition
#define DEVICE_PROFILE_NVM_NO_MEMORY_DEFINED (0x000000001u) //!< No storage defined for non-volatile data

/*!
 *  \brief
 *  Non-volatile memory data storage types.
 */
typedef enum DEVICE_PROFILE_NVM_EType
{
    DEVICE_PROFILE_NVM_eTYPE_UNDEFINED,   /*!< No non-volatile memory data storage defined.  */
    DEVICE_PROFILE_NVM_eTYPE_FLASH,       /*!< FLASH defined as non-volatile memory data storage. */
    DEVICE_PROFILE_NVM_eTYPE_EEPROM       /*!< EEPROM defined as non-volatile memory data storage. */
}DEVICE_PROFILE_NVM_EType_t;

/*!
 *  \brief
 *  Non-Volatile memory initialization parameters.
 */
typedef struct DEVICE_PROFILE_NVM_SInit
{
     /* Place for Non-volatile memory parameters */
}DEVICE_PROFILE_NVM_SInit_t;

/*!
 *  \brief
 *  Non-Volatile memory write parameters.
 */
typedef struct DEVICE_PROFILE_NVM_SWriteParam
{
    void*        handle;
    uint32_t     offset;
    uint8_t*     pData;
    uint32_t     length;
}DEVICE_PROFILE_NVM_SWriteParam_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t DEVICE_PROFILE_NVM_init  (EI_API_ADP_T *pAdapter, DEVICE_PROFILE_NVM_SInit_t *pParams);
extern uint32_t DEVICE_PROFILE_NVM_deinit(void);
extern uint32_t DEVICE_PROFILE_NVM_read  (uint32_t type, uint32_t instance, uint32_t offset, const uint8_t* pBuf, uint32_t length);
extern uint32_t DEVICE_PROFILE_NVM_write (uint32_t type, uint32_t instance, uint32_t offset, const uint8_t* pBuf, uint32_t length, bool blocking);

extern void*    DEVICE_PROFILE_NVM_getHandle      (uint32_t type, uint32_t instance);
extern bool     DEVICE_PROFILE_NVM_isWritePending (void);

#ifdef  __cplusplus
}
#endif

#endif // DEVICE_PROFILE_NVM_H
