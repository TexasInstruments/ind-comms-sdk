/*!
 *  \file drv_flash.h
 *
 *  \brief
 *  Declarations related to FLASH application driver.
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

#ifndef DRV_FLASH_H
#define DRV_FLASH_H

// definition of missing OSAL codes - remove on next OSAL release
#define OSAL_GENERAL_ERROR                (0x80000000u)  //!< Negative default value

#define OSAL_FLASH_DRV_HANDLE_INVALID     (0x00030006u)  //!< Flash handle set to NULL
#define OSAL_FLASH_DRV_PARAMS_INVALID     (0x00030007u)  //!< Flash parameters set to NULL
#define OSAL_FLASH_DRV_DATA_INVALID       (0x00030008u)  //!< Flash read/write buffer set to NULL
#define OSAL_FLASH_DRV_LENGTH_INVALID     (0x00030009u)  //!< Flash data have 0 length
#define OSAL_FLASH_DRV_GETATTR            (0x0003000Au)  //!< Flash_getAttrs call returns NULL
#define OSAL_FLASH_DRV_OPEN               (0x0003000Bu)  //!< Flash_open call failed
#define OSAL_FLASH_DRV_CLOSE              (0x0003000Cu)  //!< Flash_close call failed
#define OSAL_FLASH_DRV_READ               (0x0003000Du)  //!< Flash_read call failed
#define OSAL_FLASH_DRV_WRITE              (0x0003000Eu)  //!< Flash_write call failed
#define OSAL_FLASH_DRV_ERASEBLK           (0x0003000Fu)  //!< Flash_eraseBlk call failed
#define OSAL_FLASH_DRV_ERASESECTOR        (0x00030010u)  //!< Flash_eraseSector call failed
#define OSAL_FLASH_DRV_RESET              (0x00030011u)  //!< Flash_reset call failed
#define OSAL_FLASH_DRV_BLKPAGETOOFFSET    (0x00030012u)  //!< Flash_blkPageToOffset call failed
#define OSAL_FLASH_DRV_OFFSETTOBLKPAGE    (0x00030013u)  //!< Flash_offsetToBlkPage call failed
#define OSAL_FLASH_DRV_SECTORPAGETOOFFSET (0x00030014u)  //!< Flash_SectorPageToOffset call failed
#define OSAL_FLASH_DRV_OFFSETTOSECTORPAGE (0x00030015u)  //!< Flash_offsetToSectorPage call failed
#define OSAL_FLASH_DRV_WRITE_TASK         (0x00030016u)  //!< FALSH write task create error
#define OSAL_FLASH_DRV_WRITE_START        (0x00030017u)  //!< FLASH write start signal error
#define OSAL_FLASH_DRV_WRITE_FINISHED     (0x00030018u)  //!< FLASH write finished signal error

typedef struct DRV_FLASH_SInit
{
    uint32_t taskPrio;
}DRV_FLASH_SInit_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t     DRV_FLASH_init           (uint32_t taskPrio);
extern uint32_t     DRV_FLASH_deInit         (void);
extern uint32_t     DRV_FLASH_read           (Flash_Handle handle, uint32_t offset, const uint8_t* pBuf, uint32_t length);
extern uint32_t     DRV_FLASH_write          (Flash_Handle handle, uint32_t offset, const uint8_t* pBuf, uint32_t length, bool blocking);
extern Flash_Handle DRV_FLASH_getHandle      (uint32_t instanceId);
extern bool         DRV_FLASH_isWritePending (void);


#ifdef  __cplusplus
}
#endif

#endif // DRV_FLASH_H
