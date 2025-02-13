/*!
 *  \file drv_eeprom.h
 *
 *  \brief
 *  Declarations related to EEPROM application driver.
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

#ifndef DRV_EEPROM_H
#define DRV_EEPROM_H

// definition of missing OSAL codes - remove on next OSAL release
#define OSAL_GENERAL_ERROR         (0x80000000u)  //!< Negative default value

#define OSAL_EE_DRV_HANDLE_INVALID (0x00020021u)  //!< EEPROM handle set to NULL
#define OSAL_EE_DRV_PARAMS_INVALID (0x00020022u)  //!< EEPROM parameters set to NULL
#define OSAL_EE_DRV_DATA_INVALID   (0x00020023u)  //!< EEPROM read/write buffer set to NULL
#define OSAL_EE_DRV_LENGTH_INVALID (0x00020024u)  //!< EEPROM data have 0 length
#define OSAL_EE_DRV_GETATTR        (0x00020025u)  //!< EEPROM_getAttrs call returns NULL
#define OSAL_EE_DRV_OPEN           (0x00020026u)  //!< EEPROM_open call failed
#define OSAL_EE_DRV_CLOSE          (0x00020027u)  //!< EEPROM_close call failed
#define OSAL_EE_DRV_READ           (0x00020028u)  //!< EEPROM_read call failed
#define OSAL_EE_DRV_WRITE          (0x00020029u)  //!< EEPROM_write call failed
#define OSAL_EE_DRV_WRITE_TASK     (0x0002002Au)  //!< EEPROM write task create error
#define OSAL_EE_DRV_WRITE_START    (0x0002002Bu)  //!< EEPROM write start signal error
#define OSAL_EE_DRV_WRITE_FINISHED (0x0002002Cu)  //!< EEPROM write finished signal error

typedef struct DRV_EEPROM_SInit
{
    uint32_t taskPrio;
}DRV_EEPROM_SInit_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t      DRV_EEPROM_init           (uint32_t taskPrio);
extern uint32_t      DRV_EEPROM_deInit         (void);
extern uint32_t      DRV_EEPROM_read           (EEPROM_Handle handle, uint32_t offset, const uint8_t* pBuf, uint32_t length);
extern uint32_t      DRV_EEPROM_write          (EEPROM_Handle handle, uint32_t offset, const uint8_t* pBuf, uint32_t length, bool blocking);
extern EEPROM_Handle DRV_EEPROM_getHandle      (uint32_t instanceId);
extern bool          DRV_EEPROM_isWritePending (void);

#ifdef  __cplusplus
}
#endif

#endif // DRV_EEPROM_H
