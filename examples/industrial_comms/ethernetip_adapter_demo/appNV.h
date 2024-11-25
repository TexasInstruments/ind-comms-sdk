/*!
 *  \file appNV.h
 *
 *  \brief
 *  Declarations related to access to non volatile data storage.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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


#ifndef APPNV_H
#define APPNV_H

#include <stdbool.h>
#include <stdint.h>

// application error code definition
#define EI_APP_NV_NO_MEMORY_DEFINED (0x000000001u) //!< No storage defined for non-volatile data

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

/*!
 *  \brief
 *  Non-volatile memory data storage types.
 */
typedef enum EI_APP_NV_EType
{
    EI_APP_NV_eTYPE_UNDEFINED,   /*!< No non-volatile memory data storage defined.  */
    EI_APP_NV_eTYPE_FLASH,       /*!< FLASH defined as non-volatile memory data storage. */
    EI_APP_NV_eTYPE_EEPROM       /*!< EEPROM defined as non-volatile memory data storage. */
}EI_APP_NV_EType_t;

/*!
 *  \brief
 *  Non-Volatile memory initialization parameters.
 */
typedef struct EI_APP_NV_SInit
{
    OSAL_TASK_Priority_t   taskPrio;         /* Non-volatile data write task priority */
}EI_APP_NV_SInit_t;

/*!
 *  \brief
 *  Non-Volatile memory write parameters.
 */
typedef struct EI_APP_NV_SWriteParam
{
    void*        handle;
    uint32_t     offset;
    uint8_t*     pData;
    uint32_t     length;
}EI_APP_NV_SWriteParam_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t EI_APP_NV_init  (EI_API_ADP_T *pAdapter, EI_APP_NV_SInit_t *pParams);
extern uint32_t EI_APP_NV_deinit(void);
extern uint32_t EI_APP_NV_read  (uint32_t type, uint32_t instance, uint32_t offset, const uint8_t* pBuf, uint32_t length);
extern uint32_t EI_APP_NV_write (uint32_t type, uint32_t instance, uint32_t offset, const uint8_t* pBuf, uint32_t length, bool blocking);

extern void*    EI_APP_NV_getHandle      (uint32_t type, uint32_t instance);
extern bool     EI_APP_NV_isWritePending (void);

#ifdef  __cplusplus
}
#endif

#endif // APPNV_H
