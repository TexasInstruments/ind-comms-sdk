/*!
 *  \file ESL_foeDemo.c
 *
 *  \brief
 *  EtherCAT<sup>&reg;</sup> File Transfer over EtherCAT Example
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2021 Texas Instruments Incorporated
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

#include <osal.h>
#include <ESL_foeDemo.h>
#include <string.h>
#include "ti_board_config.h"
#if !(defined FBTLPROVIDER) || (FBTLPROVIDER==0)
#include "nvm.h"
#endif

// Offset at which file information and content is written for FoE.
#define APP_OSPI_FLASH_OFFSET_BASE          (0x200000U)

#if (defined SOC_AM263PX)
    // Offset at which appimage binary file is written for FoE. Applicable for AM263px LP.
    #define APP_OSPI_FLASH_OFFSET_APPIMAGE      (0x81000U)
#else
    // Offset at which appimage binary file is written for FoE. Applicable for AM243x LP/EVM,AM64x EVM and AM261x LP.
    #define APP_OSPI_FLASH_OFFSET_APPIMAGE      (0x80000U)
#endif

// OSPI flash page size in bytes
#define OSPI_FLASH_PAGE_SIZE                (0x100)

// Max number of flash pages written in one write cycle.
#define MAX_PAGES_WRITTEN_IN_CYCLE          4

// Offset at which file contents are written for FoE.
#define APP_OSPI_FLASH_OFFSET_FILE_DATA     (APP_OSPI_FLASH_OFFSET_BASE + OSPI_FLASH_PAGE_SIZE)

// FoE file name max length stored in a header
#define FOE_FILE_NAME_MAX_LEN               30

// Max file data bytes received in a mailbox message.
#define FOE_FRAGMENT_MAX_SIZE               1012

#define FOE_PADDED_BYTE                     0x00
#define FOE_PADDED_BYTE_LENGTH              0x01

// circular buffer length in bytes.
#define FOE_CIRCULAR_BUFFER_LEN             2048

#define FIRMWARE_FILE_NAME                  "ECATFW__"

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
    uint8_t     isPadded;
    uint8_t     padByteLength;
    uint8_t     fileNameLength;
    uint8_t     fileName[FOE_FILE_NAME_MAX_LEN];
} ESL_FOE_header_t;

// store the FoE downloaded file information as a file header.
static ESL_FOE_header_t foeFileHeader = { 0 };

// stores the FoE file access password for authentication check during file read/write process.
static uint32_t foeFileAccessPassword = 0;

// offset in flash at which a chunk of file content is written.
static uint32_t foeFileWriteOffset = 0;

static uint32_t flashForceErase = 0;

static uint8_t isBinaryFile = false;
static uint8_t firmwareUpgradeEnabled = false;
static uint8_t firmwareUpdateComplete = false;

// Circular buffer handling variables.
volatile static uint16_t CircBuffWriteIndex = 0;
volatile static uint16_t CircBuffReadIndex  = 0;
volatile static uint8_t  foeCircularBuffer[FOE_CIRCULAR_BUFFER_LEN];

/*! <!-- Description: -->
 *
 *  \brief
 *  Increment circular buffer index.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pBufIndex   circular buffer index
 *  \return
 *
 *  <!-- Group: -->
 *
 *  \ingroup FoE
 *
 * */
static void foe_circular_buffer_index_incr(uint16_t* pBufIndex)
{
    (*pBufIndex)++;

    if(*pBufIndex == FOE_CIRCULAR_BUFFER_LEN)
    {
        *pBufIndex = 0;
    }
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Write data to circular buffer at write index.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  dataByte    data to be written to circular buffer
 *  \return
 *
 *  <!-- Group: -->
 *
 *  \ingroup FoE
 *
 * */
static void foe_circular_buffer_write(uint8_t dataByte)
{
    foeCircularBuffer[CircBuffWriteIndex] = dataByte;
    foe_circular_buffer_index_incr((uint16_t*)&CircBuffWriteIndex);

    if(CircBuffWriteIndex == CircBuffReadIndex)
    {
        foe_circular_buffer_index_incr((uint16_t*)&CircBuffReadIndex);
    }
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Get the available byte count to be read.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \return uint16_t    available byte count
 *
 *  <!-- Group: -->
 *
 *  \ingroup FoE
 *
 * */
static uint16_t foe_circular_buffer_get_avail_bytes(void)
{
    if(CircBuffWriteIndex >= CircBuffReadIndex)
    {
        return CircBuffWriteIndex - CircBuffReadIndex;
    }
    else
    {
        return ((FOE_CIRCULAR_BUFFER_LEN - CircBuffReadIndex) + CircBuffWriteIndex);
    }
}

/*! <!-- Description: -->
 *
 *  \brief
 *  Read data from the circular buffer at read index.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pData    ptr to the mem location where read data is stored.
 *  \param[in]  size     number of bytes to be read from the circular buffer.
 *  \return     uint16_t bytes copied to mem location
 *
 *  <!-- Group: -->
 *
 *  \ingroup FoE
 *
 * */
static uint16_t foe_circular_buffer_read(uint8_t *pData, uint16_t size)
{
    uint16_t available_bytes = foe_circular_buffer_get_avail_bytes();

    if(available_bytes == 0)
    {
        return 0;
    }
    for(uint16_t idx = 0 ; (idx < available_bytes) && (idx < size) ; idx++)
    {
        pData[idx] = foeCircularBuffer[CircBuffReadIndex];
        foe_circular_buffer_index_incr((uint16_t*)&CircBuffReadIndex);
    }
    if(available_bytes < size)
    {
        return available_bytes;
    }
    return size;
}

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

    retVal = ECAT_FOE_ERRCODE_ILLEGAL;

#if !(defined FBTLPROVIDER) || (FBTLPROVIDER==0)
    NVM_err_t error;

    if ((foeFileHeader.dataSize - foeFileHeader.padByteLength - foeFileReadDoneSize) >= FOE_FRAGMENT_MAX_SIZE)
    {
        fileOffset = foeFileReadDoneSize;
        foeFileReadDoneSize += size;
        foeFileChunkReadSize = size;
    }
    else
    {
        fileOffset = foeFileReadDoneSize;
        foeFileChunkReadSize = foeFileHeader.dataSize - foeFileHeader.padByteLength - foeFileReadDoneSize;
        foeFileReadDoneSize = 0;
    }

    error = NVM_APP_read(NVM_TYPE_FLASH,
                         CONFIG_FLASH0,
                         APP_OSPI_FLASH_OFFSET_FILE_DATA + fileOffset,
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
 *  \param[in]  pData       Data Buffer.
 *  \param[in]  size        write length parameter.
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
    uint8_t *temp_ptr = (uint8_t *)pData;
    uint8_t data_buff[MAX_PAGES_WRITTEN_IN_CYCLE*OSPI_FLASH_PAGE_SIZE];

    foeFileHeader.dataSize += size;

    for(uint16_t idx=0;idx<size;idx++)
    {
        foe_circular_buffer_write(temp_ptr[idx]);
    }

    while(foe_circular_buffer_get_avail_bytes() >= OSPI_FLASH_PAGE_SIZE)
    {
        uint8_t pagesToWrite = (uint8_t)(foe_circular_buffer_get_avail_bytes()/OSPI_FLASH_PAGE_SIZE);
        if (pagesToWrite > MAX_PAGES_WRITTEN_IN_CYCLE)
        {
            pagesToWrite = MAX_PAGES_WRITTEN_IN_CYCLE;
        }
        if(foe_circular_buffer_read(data_buff, (pagesToWrite*OSPI_FLASH_PAGE_SIZE)) >= OSPI_FLASH_PAGE_SIZE)
        {
            error = NVM_APP_write(  NVM_TYPE_FLASH,
                                    CONFIG_FLASH0,
                                    foeFileWriteOffset,
                                    (uint32_t)(pagesToWrite*OSPI_FLASH_PAGE_SIZE),
                                    (uint8_t*)data_buff,
                                    flashForceErase);

            if (error != NVM_ERR_SUCCESS)
            {
                retVal = ECAT_FOE_ERRCODE_ACCESS;
                return retVal;
            }
            flashForceErase = false;
            foeFileWriteOffset += (pagesToWrite*OSPI_FLASH_PAGE_SIZE);
        }
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
    NVM_err_t error;
    uint8_t data_buff[OSPI_FLASH_PAGE_SIZE];

    while(foe_circular_buffer_get_avail_bytes() > 0)
    {
        uint16_t availbytes = foe_circular_buffer_get_avail_bytes();
        if ((availbytes < OSPI_FLASH_PAGE_SIZE) && (availbytes % 2))
        {
            foeFileHeader.isPadded = true;
            foeFileHeader.padByteLength = FOE_PADDED_BYTE_LENGTH;
            foeFileHeader.dataSize +=  FOE_PADDED_BYTE_LENGTH;
            for(uint16_t idx=0;idx<FOE_PADDED_BYTE_LENGTH;idx++)
            {
                foe_circular_buffer_write((uint8_t)FOE_PADDED_BYTE);
                availbytes ++;
            }
        }
        if(foe_circular_buffer_read(data_buff, availbytes) == availbytes)
        {
            error = NVM_APP_write(  NVM_TYPE_FLASH,
                                    CONFIG_FLASH0,
                                    foeFileWriteOffset,
                                    (uint32_t)(availbytes),
                                    (uint8_t*)data_buff,
                                    flashForceErase);
            if (error != NVM_ERR_SUCCESS)
            {
                retVal = ECAT_FOE_ERRCODE_ACCESS;
                return retVal;
            }
            foeFileWriteOffset += availbytes;
        }
    }

    if (false == firmwareUpgradeEnabled || (true == firmwareUpgradeEnabled && false == isBinaryFile))
    {
        foeFileWriteOffset = APP_OSPI_FLASH_OFFSET_BASE;
        error = NVM_APP_write(  NVM_TYPE_FLASH,
                                CONFIG_FLASH0,
                                foeFileWriteOffset,
                                (uint32_t)(sizeof(ESL_FOE_header_t)),
                                (uint8_t*)&foeFileHeader,
                                flashForceErase);
        if (error != NVM_ERR_SUCCESS)
        {
            retVal = ECAT_FOE_ERRCODE_ACCESS;
            return retVal;
        }
#if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
        OSAL_printf("Name  : %s\n\r",foeFileHeader.fileName);
        OSAL_printf("NameL : %d\n\r",foeFileHeader.fileNameLength);
        OSAL_printf("Size  : %d\n\r",foeFileHeader.dataSize);
        OSAL_printf("Passwd: %d\n\r",foeFileHeader.password);
        OSAL_printf("IsPadd: %d\n\r",foeFileHeader.isPadded);
        OSAL_printf("PadLen: %d\n\r",foeFileHeader.padByteLength);
#endif
    }
    else
    {
        #if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
            OSAL_printf("FoE - Firmware download completed!\n\r");
            OSAL_printf("Reboot or Leave Bootstrap state\n\r");
        #endif
        firmwareUpdateComplete = true;
    }
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
    foeFileWriteOffset = 0;
    foeFileAccessPassword = password;

    if (false == isRead)
    {
        foeFileHeader.fileNameLength = nameLen;
        memset(foeFileHeader.fileName,'\0',FOE_FILE_NAME_MAX_LEN);
        memcpy(foeFileHeader.fileName,pName,nameLen);
        foeFileHeader.password = foeFileAccessPassword;
        foeFileHeader.dataSize = 0;
        foeFileHeader.isPadded = false;
        foeFileHeader.padByteLength = 0;
        flashForceErase = true;
        firmwareUpdateComplete = false;

        int result = strncmp((const char*)foeFileHeader.fileName,FIRMWARE_FILE_NAME,8);
        if(result == 0)
        {
            isBinaryFile = true;
            if (true == firmwareUpgradeEnabled)
            {
                foeFileWriteOffset = APP_OSPI_FLASH_OFFSET_APPIMAGE;
            }
            else
            {
                foeFileWriteOffset = APP_OSPI_FLASH_OFFSET_FILE_DATA;
            }
        }
        else
        {
            isBinaryFile = false;
            foeFileWriteOffset = APP_OSPI_FLASH_OFFSET_FILE_DATA;
        }
    }
    else
    {
        NVM_err_t error = NVM_APP_read( NVM_TYPE_FLASH,
                                        CONFIG_FLASH0,
                                        APP_OSPI_FLASH_OFFSET_BASE,
                                        (uint32_t)sizeof(ESL_FOE_header_t),
                                        (uint8_t*)&foeFileHeader);

        if (error != NVM_ERR_SUCCESS)
        {
            retVal = ECAT_FOE_ERRCODE_FLASH_ERROR;
            return retVal;
        }

        if (foeFileHeader.password != foeFileAccessPassword)
        {
            #if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
                OSAL_printf("FoE - File access password incorrect!\n\r");
            #endif
            retVal = ECAT_FOE_ERRCODE_ACCESS;
            return retVal;
        }

        if (foeFileHeader.dataSize == 0)
        {
            #if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
                OSAL_printf("FoE - File not found or empty!\n\r");
            #endif
            retVal = ECAT_FOE_ERRCODE_NOTFOUND;
            return retVal;
        }
    }
    return retVal;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  FoE Bootloader start.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext  context
 *  \return
 *
 *  <!-- Example: -->
 *  // required variables
 *  void*    pCtxt;
 *
 *  EC_SLV_APP_FoE_startBL(pCtxt);
 *
 *  <!-- Group: -->
 *
 *  \ingroup EC_SLV_APP
 *
 * */

void EC_SLV_APP_FoE_startBL(void* pContext)
{
#if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
    OSAL_printf("SM change - Init to Bootstrap\n\r");
#endif
    firmwareUpgradeEnabled = true;
    return;
}

/*! <!-- Description: -->
 *
 *  \brief
 *  FoE Bootloader stop.
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pContext  context
 *  \return
 *
 *  <!-- Example: -->
 *  // required variables
 *  void*    pCtxt;
 *
 *  EC_SLV_APP_FoE_stopBL(pCtxt);
 *
 *  <!-- Group: -->
 *
 *  \ingroup EC_SLV_APP
 *
 * */

void EC_SLV_APP_FoE_stopBL(void* pContext)
{
#if (defined DEBUGTRACING) && (DEBUGTRACING == 1)
    OSAL_printf("SM change - Bootstrap to Init\n\r");
#endif
    if ((true == firmwareUpgradeEnabled) && (true == firmwareUpdateComplete))
    {
        firmwareUpgradeEnabled = false;
        firmwareUpdateComplete = false;
        OSAL_printf("Rebooting Now....\n\r");
#if ((defined SOC_AM263PX) || (defined SOC_AM261X))
            SOC_generateSwWarmReset();
#else
            SOC_generateSwWarmResetMcuDomain();
#endif // SOC_AM263PX
    }
    firmwareUpgradeEnabled = false;
    return;
}

//*************************************************************************************************
