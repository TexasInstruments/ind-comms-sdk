/*!
 *  \file CUST_drivers.h
 *
 *  \brief
 *  Custom drivers support.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
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

#if !(defined PROTECT_CUST_DRIVERS_H)
#define PROTECT_CUST_DRIVERS_H		1

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "kernel/dpl/TaskP.h"

#include <board.h>
#include <board/flash.h>
#include <board/eeprom.h>
#include <board/ethphy.h>

#include <drivers/pruicss/g_v0/pruicss.h>

#include <drivers/led/CUST_led.h>
#include <drivers/uart/CUST_uart.h>
#include <drivers/flash/CUST_flash.h>
#include <drivers/eeprom/CUST_eeprom.h>
#include <drivers/ethphy/CUST_ethPhy.h>
#include <drivers/pru_icss/CUST_pruIcss.h>

/*!
 *  \brief
 *  Custom driver permanent data storage types.
 */
typedef enum CUST_DRIVERS_PRM_EType
{
    CUST_DRIVERS_PRM_eTYPE_UNDEFINED,   /*!< No permanent data memory defined.  */
    CUST_DRIVERS_PRM_eTYPE_FLASH,       /*!< FLASH permanent data memory type. */
    CUST_DRIVERS_PRM_eTYPE_EEPROM       /*!< EEPROM permanent data memory type. */
}CUST_DRIVERS_PRM_EType_t;

/*!
 *  \brief
 *  Custom driver error codes.
 */
typedef enum CUST_DRIVERS_EError
{
    CUST_DRIVERS_eERR_NOERROR               =  0,   /*!< No error, everything is fine. */
    CUST_DRIVERS_eERR_NO_PERMANENT_STORAGE  = -20,   /*!< No permanent data storage defined */
    CUST_DRIVERS_eERR_EEPROM_HANDLE_INVALID = -19,  /*!< EEPROM handle is invalid */
    CUST_DRIVERS_eERR_EEPROM_DATA_INVALID   = -18,  /*!< Pointer to data for eeprom write is invalid. */
    CUST_DRIVERS_eERR_EEPROM_LENGTH_INVALID = -17,  /*!< Length of data for eeprom write is invalid. */
    CUST_DRIVERS_eERR_EEPROM_READ           = -16,  /*!< EEPROM read error. */
    CUST_DRIVERS_eERR_EEPROM_WRITE          = -15,  /*!< EEPROM write error. */
    CUST_DRIVERS_eERR_EEPROM                = -14,  /*!< EEPROM driver error. */
    CUST_DRIVERS_eERR_FLASH_HANDLE_INVALID  = -13,  /*!< FLASH handle is invalid. */
    CUST_DRIVERS_eERR_FLASH_DATA_INVALID    = -12,  /*!< Pointer to data to be flashed is invalid. */
    CUST_DRIVERS_eERR_FLASH_LENGTH_INVALID  = -11,  /*!< Length of data to be flashed is invalid. */
    CUST_DRIVERS_eERR_FLASH_READ            = -10,  /*!< FLASH read error. */
    CUST_DRIVERS_eERR_FLASH_WRITE           = -9,   /*!< FLASH write error. */
    CUST_DRIVERS_eERR_FLASH_ERASE           = -8,   /*!< FLASH erase error. */
    CUST_DRIVERS_eERR_FLASH_OFFSET          = -7,   /*!< FLASH offset error. */
    CUST_DRIVERS_eERR_FLASH                 = -6,   /*!< FLASH driver error. */
    CUST_DRIVERS_eERR_ETHPHY                = -5,   /*!< PHY driver error. */
    CUST_DRIVERS_eERR_UART                  = -4,   /*!< UART driver error. */
    CUST_DRIVERS_eERR_LED                   = -3,   /*!< LED driver error. */
    CUST_DRIVERS_eERR_PRUICSS               = -2,   /*!< PRU-ICSS driver error. */
    CUST_DRIVERS_eERR_GENERALERROR          = -1    /*!< General error */
} CUST_DRIVERS_EError_t;

/*!
 *  \brief
 *  PRU-ICSS configuration parameters.
 */
typedef struct CUST_DRIVERS_SInitPruIcss
{
    uint32_t                        instance;       /* Instance of PRU-ICSS block used by stack (as defined by SysConfig) */
    CUST_ETHPHY_SInit_t             ethPhy;         /* PRU-ICSS PHY parameters */
}CUST_DRIVERS_SInitPruIcss_t;

/*!
 *  \brief
 *  EEPROM configuration parameters.
 */
typedef struct CUST_DRIVERS_SEepromParams
{
    OSAL_TASK_Priority_t   taskPrio;         /* EEPROM write task priority */
}CUST_DRIVERS_SEepromParams_t;

/*!
 *  \brief
 *  FLASH configuration parameters.
 */
typedef struct CUST_DRIVERS_SFlashParams
{
    OSAL_TASK_Priority_t   taskPrio;         /* EEPROM write task priority */
}CUST_DRIVERS_SFlashParams_t;

/*!
 *  \brief
 *  Custom drivers initialization parameters.
 */
typedef struct CUST_DRIVERS_SInit
{
    CUST_DRIVERS_SInitPruIcss_t     pruIcss;        /* PRUICSS init parameters */
    CUST_DRIVERS_SEepromParams_t    eeprom;         /* EEPROM init paprameters */
    CUST_DRIVERS_SFlashParams_t     flash;          /* FLASH init parameters */
}CUST_DRIVERS_SInit_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern uint32_t        CUST_DRIVERS_init                            (CUST_DRIVERS_SInit_t* pParams_p);
extern uint32_t        CUST_DRIVERS_deinit                          (void);

extern void            CUST_DRIVERS_UART_printf           (void* pContext_p, const char* __restrict pFormat_p, va_list argptr_p);
extern void            CUST_DRIVERS_LOG_printf            (void* pContext_p, const char* __restrict pFormat_p, va_list argptr_p);

extern void*           CUST_DRIVERS_PRM_getHandle         (uint32_t type_p, uint32_t instance_p);
extern uint32_t        CUST_DRIVERS_PRM_read              (void* handler_p, uint32_t type_p, uint32_t offset_p, uint8_t* pBuf_p, uint32_t length_p);
extern uint32_t        CUST_DRIVERS_PRM_write             (void* handler_p, uint32_t type_p, uint32_t offset_p, uint8_t* pBuf_p, uint32_t length_p, bool blocking_p);
extern bool            CUST_DRIVERS_PRM_isWritePending    (void);

extern void            CUST_DRIVERS_LED_setIndustrialLeds (uint32_t value);

#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_CUST_DRIVERS_H */
