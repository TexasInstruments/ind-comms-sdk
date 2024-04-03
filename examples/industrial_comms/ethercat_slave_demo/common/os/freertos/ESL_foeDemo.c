/*!
 *  \file ESL_foeDemo.c
 *
 *  \brief
 *  EtherCAT<sup>&reg;</sup> File Transfer over EtherCAT Example
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
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

#include <osal.h>
#include <ESL_foeDemo.h>
#include <string.h>
#include "ti_board_config.h"
#if !(defined FBTLPROVIDER) || (FBTLPROVIDER==0)
#include "nvm.h"
#endif

// Offset at which file is written for FoE. 
#define APP_OSPI_FLASH_OFFSET_BASE  (0x200000U)

#define FOE_FRAGMENT_MAX_SIZE       1012

/*---------------------------------------------
-    Error Codes
-----------------------------------------------*/
#define    ECAPP_ERROR_CLASS(x)                 (0x38020000|((x)&0xffff))

#define    ECAT_FOE_ERRCODE_NOTDEFINED          ECAPP_ERROR_CLASS(0x8000)   /**< \brief Not defined*/
#define    ECAT_FOE_ERRCODE_NOTFOUND            ECAPP_ERROR_CLASS(0x8001)   /**< \brief The file requested by an FoE upload service could not be found on the server*/
#define    ECAT_FOE_ERRCODE_ACCESS              ECAPP_ERROR_CLASS(0x8002)   /**< \brief Read or write access to this file not allowed (e.g. due to local control).*/
#define    ECAT_FOE_ERRCODE_DISKFULL            ECAPP_ERROR_CLASS(0x8003)   /**< \brief Disk to store file is full or memory allocation exceeded*/
#define    ECAT_FOE_ERRCODE_ILLEGAL             ECAPP_ERROR_CLASS(0x8004)   /**< \brief Illegal FoE operation, e.g. service identifier invalid*/
#define    ECAT_FOE_ERRCODE_PACKENO             ECAPP_ERROR_CLASS(0x8005)   /**< \brief FoE packet number invalid*/
#define    ECAT_FOE_ERRCODE_EXISTS              ECAPP_ERROR_CLASS(0x8006)   /**< \brief The file which is requested to be downloaded does already exist*/
#define    ECAT_FOE_ERRCODE_NOUSER              ECAPP_ERROR_CLASS(0x8007)   /**< \brief No User*/
#define    ECAT_FOE_ERRCODE_BOOTSTRAPONLY       ECAPP_ERROR_CLASS(0x8008)   /**< \brief FoE only supported in Bootstrap*/
#define    ECAT_FOE_ERRCODE_NOTINBOOTSTRAP      ECAPP_ERROR_CLASS(0x8009)   /**< \brief This file may not be accessed in BOOTSTRAP state*/
#define    ECAT_FOE_ERRCODE_NORIGHTS            ECAPP_ERROR_CLASS(0x800A)   /**< \brief Password invalid*/
#define    ECAT_FOE_ERRCODE_PROGERROR           ECAPP_ERROR_CLASS(0x800B)   /**< \brief Generic programming error. Should only be returned if  error reason cannot be distinguished*/
#define    ECAT_FOE_ERRCODE_INVALID_CHECKSUM    ECAPP_ERROR_CLASS(0x800C)   /**< \brief checksum included in the file is invalid*/
#define    ECAT_FOE_ERRCODE_INVALID_FIRMWARE    ECAPP_ERROR_CLASS(0x800D)   /**< \brief The hardware does not support the downloaded firmware*/
#define    ECAT_FOE_ERRCODE_NO_FILE             ECAPP_ERROR_CLASS(0x800F)   /**< \brief Do not use (identical with 0x8001)*/
#define    ECAT_FOE_ERRCODE_NO_FILE_HEADER      ECAPP_ERROR_CLASS(0x8010)   /**< \brief Missing file header of error in file header*/
#define    ECAT_FOE_ERRCODE_FLASH_ERROR         ECAPP_ERROR_CLASS(0x8011)   /**< \brief Flash cannot be accessed*/

// FoE header stored along with the actual file data.
typedef struct ESL_FOE_header
{
    uint32_t    password;
    uint32_t    dataSize;
} ESL_FOE_header_t;

// buffer used for async write
static void * foeBufferPtr = NULL;

// Used when FoE file size is larger.
static void * foeBufferPtr1 = NULL;
static void * foeBufferPtr2 = NULL;

// stores the accumulated file size during a larger FoE file write 
static uint32_t foeFileWriteChunkSize = 0;

// stores the FoE file access password for authentication check during file read/write process.
static uint32_t fileAccessPassword = 0;

/*! <!-- Description: -->
 *
 *  \brief
 *  Callback function registered for FoE file write in async mode.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  status          NVM async write status
 *  \return     void            
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  uint32_t    status;
 *
 *  // the Call
 *  EC_SLV_APP_FoE_fileWriteCb(status);
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup EC_SLV_APP
 *
 * */
#if !(defined FBTLPROVIDER) || (FBTLPROVIDER==0)
void EC_SLV_APP_FoE_fileWriteCb(uint32_t status)
{
    if (status == NVM_ERR_SUCCESS)
    {
#if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
        OSAL_printf("FoE - File download to flash : ok!\n\r");
#endif
    }

    foeFileWriteChunkSize = 0;

    if(foeBufferPtr != NULL)
    {
        OSAL_MEMORY_free(foeBufferPtr);
        foeBufferPtr = NULL;
    }
    if(foeBufferPtr1 != NULL)
    {
        OSAL_MEMORY_free(foeBufferPtr1);
        foeBufferPtr1 = NULL;
    }
    if(foeBufferPtr2 != NULL)
    {
        OSAL_MEMORY_free(foeBufferPtr2);
        foeBufferPtr2 = NULL;
    }
}
#endif
 
/*! <!-- Description: -->
 *
 *  \brief
 *  Read file from device.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext      context handle
 *  \param[in]  pData         data buffer.
 *  \param[in]  size          read length parameter.
 *  \param[in]  fileOffset    File offset.
 *  \return     ErrorCode     FoE error code.
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  uint16_t*   pData;
 *  uint16_t    length;
 *  uint32_t    fileOffset;
 *
 *  // the Call
 *  retVal = EC_SLV_APP_FoE_fileRead(NULL, pData, length, fileOffset);
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup EC_SLV_APP
 *
 * */
uint32_t EC_SLV_APP_FoE_fileRead(void* pContext, uint16_t* pData, uint16_t size, uint32_t fileOffset)
{
    uint32_t            retVal;
    static uint32_t     foeFileReadDoneSize = 0;
    uint32_t            foeFileChunkReadSize = 0;
    ESL_FOE_header_t    foeHeader = {0};

    retVal = ECAT_FOE_ERRCODE_ILLEGAL;

#if !(defined FBTLPROVIDER) || (FBTLPROVIDER==0)
    NVM_err_t error;
    error = NVM_APP_read(NVM_TYPE_FLASH,
                         CONFIG_FLASH0,
                         APP_OSPI_FLASH_OFFSET_BASE,
                         (uint32_t)sizeof(ESL_FOE_header_t),
                         (uint8_t*)&foeHeader);

    if (error != NVM_ERR_SUCCESS)
    {
        retVal = ECAT_FOE_ERRCODE_FLASH_ERROR;
        return retVal;
    }

    if (foeHeader.password != fileAccessPassword)
    {
        #if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
            OSAL_printf("FoE - File access password incorrect!\n\r");
        #endif
        retVal = ECAT_FOE_ERRCODE_ACCESS;
        return retVal;
    }

    if (foeHeader.dataSize == 0)
    {
        #if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
            OSAL_printf("FoE - File not found or empty!\n\r");
        #endif
        retVal = ECAT_FOE_ERRCODE_NOTFOUND;
        return retVal;
    }

    if ((foeHeader.dataSize - foeFileReadDoneSize) >= FOE_FRAGMENT_MAX_SIZE)
    {
        fileOffset = foeFileReadDoneSize;
        foeFileReadDoneSize += size;
        foeFileChunkReadSize = size;
    }
    else
    {
        fileOffset = foeFileReadDoneSize;
        foeFileChunkReadSize = foeHeader.dataSize - foeFileReadDoneSize;
        foeFileReadDoneSize = 0;
    }

    error = NVM_APP_read(NVM_TYPE_FLASH,
                         CONFIG_FLASH0,
                         APP_OSPI_FLASH_OFFSET_BASE + sizeof(ESL_FOE_header_t) + fileOffset,
                         (uint32_t)foeFileChunkReadSize,
                         (uint8_t*)pData);

    if (error != NVM_ERR_SUCCESS)
    {
        #if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
            OSAL_printf("FoE - File upload NOT successful!\n\r");
        #endif
        retVal = ECAT_FOE_ERRCODE_FLASH_ERROR;
        return retVal;
    }

    retVal = foeFileChunkReadSize;
#endif

    return retVal;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Write file from device.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext        context
 *  \param[in]  pData   	Data Buffer.
 *  \param[in]  size    	write length parameter.
 *  \return     ErrorCode       FoE error code.
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  // required variables
 *  uint16_t* pData;
 *  uint16_t  length;
 *
 *  // the Call
 *  retVal = EC_SLV_APP_FoE_fileWrite(pData, length);
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup EC_SLV_APP
 *
 * */
uint32_t EC_SLV_APP_FoE_fileWrite(void *pContext, uint16_t* pData, uint16_t size)
{
    uint32_t    retVal;

    retVal = ECAT_FOE_ERRCODE_DISKFULL;

#if !(defined FBTLPROVIDER) || (FBTLPROVIDER==0)
    NVM_err_t error;
    
    if (size >= FOE_FRAGMENT_MAX_SIZE)
    {
        foeFileWriteChunkSize += size;
        if (foeBufferPtr1 == NULL)
        {
            if(foeBufferPtr2 == NULL)
            {
                foeBufferPtr1 = OSAL_MEMORY_calloc(foeFileWriteChunkSize,sizeof(uint8_t));
                OSAL_MEMORY_memcpy(foeBufferPtr1,pData,foeFileWriteChunkSize);
            }
            else
            {
                foeBufferPtr1 = OSAL_MEMORY_calloc(foeFileWriteChunkSize,sizeof(uint8_t));
                OSAL_MEMORY_memcpy(foeBufferPtr1,foeBufferPtr2,(foeFileWriteChunkSize-size));
                OSAL_MEMORY_memcpy((foeBufferPtr1 + (foeFileWriteChunkSize-size)),pData,size);
                OSAL_MEMORY_free(foeBufferPtr2);
                foeBufferPtr2 = NULL;
            }
        }
        else if (foeBufferPtr2 == NULL)
        {
            if(foeBufferPtr1 == NULL)
            {
                foeBufferPtr2 = OSAL_MEMORY_calloc(foeFileWriteChunkSize,sizeof(uint8_t));
                OSAL_MEMORY_memcpy(foeBufferPtr2,pData,foeFileWriteChunkSize);
            }
            else
            {
                foeBufferPtr2 = OSAL_MEMORY_calloc(foeFileWriteChunkSize,sizeof(uint8_t));
                OSAL_MEMORY_memcpy(foeBufferPtr2,foeBufferPtr1,(foeFileWriteChunkSize-size));
                OSAL_MEMORY_memcpy((foeBufferPtr2 + (foeFileWriteChunkSize-size)),pData,size);
                OSAL_MEMORY_free(foeBufferPtr1);
                foeBufferPtr1 = NULL;
            }
        }
        retVal = size;
        return retVal; 
    }

    foeFileWriteChunkSize += size;

    ESL_FOE_header_t foeHeader = {  .dataSize = foeFileWriteChunkSize,
                                    .password = fileAccessPassword
                                 };

    foeBufferPtr = OSAL_MEMORY_calloc((sizeof(ESL_FOE_header_t) + foeFileWriteChunkSize),sizeof(uint8_t));
    OSAL_MEMORY_memcpy(foeBufferPtr,&foeHeader,sizeof(ESL_FOE_header_t));
    if(foeBufferPtr1 != NULL)
    {
        OSAL_MEMORY_memcpy((foeBufferPtr + sizeof(ESL_FOE_header_t)),foeBufferPtr1,(foeFileWriteChunkSize - size));
        if (size != 0)
        {
            OSAL_MEMORY_memcpy((foeBufferPtr + sizeof(ESL_FOE_header_t) + (foeFileWriteChunkSize - size)),pData,size);
        }
    }
    else if(foeBufferPtr2 != NULL)
    {
        OSAL_MEMORY_memcpy((foeBufferPtr + sizeof(ESL_FOE_header_t)),foeBufferPtr2,(foeFileWriteChunkSize - size));
        if (size != 0)
        {
            OSAL_MEMORY_memcpy((foeBufferPtr + sizeof(ESL_FOE_header_t) + (foeFileWriteChunkSize - size)),pData,size);
        }
    }
    else
    {
        OSAL_MEMORY_memcpy(foeBufferPtr + sizeof(ESL_FOE_header_t),pData,foeFileWriteChunkSize);
    }
    
    #if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
        OSAL_printf("FoE - Downloaded file size : %d\n\r",foeFileWriteChunkSize);
    #endif

    error = NVM_APP_writeAsync( NVM_TYPE_FLASH,
                                CONFIG_FLASH0,
                                APP_OSPI_FLASH_OFFSET_BASE,
                                (uint32_t)(sizeof(ESL_FOE_header_t) + foeFileWriteChunkSize),
                                (uint8_t*)foeBufferPtr);
    
    if (error != NVM_ERR_SUCCESS)
    {
        retVal = ECAT_FOE_ERRCODE_ACCESS;
        return retVal;
    }

    retVal = size;
#endif
    return retVal;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Close file.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext  context
 *  \param[in]  errorCode Error code handed by the Stack.
 *  \return     ErrorCode   FoE error code.
 *
 *  <!-- Example: -->
 *  // required variables
 *  uint32_t errorCode;
 *  void*    pCtxt;
 *
 *  EC_SLV_APP_FoE_fileClose(pCtxt, errorCode);
 *
 *  <!-- Group: -->
 *
 *  \ingroup EC_SLV_APP
 *
 * */
uint32_t EC_SLV_APP_FoE_fileClose(void* pContext, uint32_t errorCode)
{
    uint32_t    retVal  = 0;
    retVal = EC_API_eERR_NONE;
    return retVal;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Open file from device.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext          context
 *  \param[in]  pName             Filename.
 *  \param[in]  nameLen           Filename length.
 *  \param[in]  isRead            Access rigths.
 *  \param[in]  password          Access code.
 *  \return     ErrorCode         FoE Error code.
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  // required variables
 *  uin16t_t    length;
 *  const char* pName;
 *  bool        access;
 *  uint32_t    password;
 *
 *  // the Call
 *  EC_SLV_APP_FoeFileOpen(fileName, length, access, password)
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup EC_SLV_APP
 *
 * */
uint32_t EC_SLV_APP_FoE_fileOpen(void* pContext, const char* pName, uint16_t nameLen, bool isRead, uint32_t password)
{
    uint32_t    retVal;
    retVal = EC_API_eERR_NONE;
#if !(defined FBTLPROVIDER) || (FBTLPROVIDER==0)
    NVM_err_t error;
    error = NVM_APP_registerCallback(EC_SLV_APP_FoE_fileWriteCb);

    if (error != NVM_ERR_SUCCESS)
    {
        retVal = ECAT_FOE_ERRCODE_NOTDEFINED;
        return retVal;
    }
#endif
    fileAccessPassword = password;
    return retVal;
}

//*************************************************************************************************
