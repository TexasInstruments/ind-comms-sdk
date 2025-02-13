/*!
 *  \file drivers.h
 *
 *  \brief
 *  Application drivers.
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

#if !(defined PROTECT_DRIVERS_H)
#define PROTECT_DRIVERS_H      1

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "drivers/common/drv_common.h"

#include "drivers/eeprom/drv_eeprom.h"
#include "drivers/flash/drv_flash.h"
#include "drivers/led/drv_led.h"
#include "drivers/pruicss/drv_pruicss.h"
#include "drivers/uart/drv_uart.h"


/*!
 *  \brief
 *  Custom driver error codes.
 */
typedef enum DRIVERS_EError
{
    DRIVERS_eERR_NOERROR               =  0,   /*!< No error, everything is fine. */
    DRIVERS_eERR_NO_NVM_STORAGE        = -20,  /*!< No storage defined for non-volatile configuration data */
    DRIVERS_eERR_EEPROM_HANDLE_INVALID = -19,  /*!< EEPROM handle is invalid */
    DRIVERS_eERR_EEPROM_DATA_INVALID   = -18,  /*!< Pointer to data for eeprom write is invalid. */
    DRIVERS_eERR_EEPROM_LENGTH_INVALID = -17,  /*!< Length of data for eeprom write is invalid. */
    DRIVERS_eERR_EEPROM_READ           = -16,  /*!< EEPROM read error. */
    DRIVERS_eERR_EEPROM_WRITE          = -15,  /*!< EEPROM write error. */
    DRIVERS_eERR_EEPROM                = -14,  /*!< EEPROM driver error. */
    DRIVERS_eERR_FLASH_HANDLE_INVALID  = -13,  /*!< FLASH handle is invalid. */
    DRIVERS_eERR_FLASH_DATA_INVALID    = -12,  /*!< Pointer to data to be flashed is invalid. */
    DRIVERS_eERR_FLASH_LENGTH_INVALID  = -11,  /*!< Length of data to be flashed is invalid. */
    DRIVERS_eERR_FLASH_READ            = -10,  /*!< FLASH read error. */
    DRIVERS_eERR_FLASH_WRITE           = -9,   /*!< FLASH write error. */
    DRIVERS_eERR_FLASH_ERASE           = -8,   /*!< FLASH erase error. */
    DRIVERS_eERR_FLASH_OFFSET          = -7,   /*!< FLASH offset error. */
    DRIVERS_eERR_FLASH                 = -6,   /*!< FLASH driver error. */
    DRIVERS_eERR_ETHPHY                = -5,   /*!< PHY driver error. */
    DRIVERS_eERR_UART                  = -4,   /*!< UART driver error. */
    DRIVERS_eERR_LED                   = -3,   /*!< LED driver error. */
    DRIVERS_eERR_PRUICSS               = -2,   /*!< PRU-ICSS driver error. */
    DRIVERS_eERR_GENERALERROR          = -1    /*!< General error */
} DRIVERS_EError_t;

/*!
 *  \brief
 *  Application drivers initialization parameters.
 */
typedef struct APP_DRIVERS_SInit
{
    DRV_EEPROM_SInit_t         eeprom;         /* EEPROM parameters. */
    DRV_FLASH_SInit_t          flash;          /* FLASH parameters. */
    DRV_LED_SInit_t            led;            /* LED parameters. */
    DRV_PRUICSS_SInit_t        pruicss;        /* PRU-ICSS parameters. */
    DRV_UART_SInit_t           uart;           /* UART parameters. */
}APP_DRIVERS_SInit_t;

/*!
 *  \brief
 *  Drivers initialization parameters.
 */
typedef struct DRIVERS_SInit
{
    APP_DRIVERS_SInit_t             app;            /* Application driver parameters */
    CUST_DRIVERS_SInit_t            custom;         /* Custom driver parameters */
}DRIVERS_SInit_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern uint32_t  APP_DRIVERS_init       (APP_DRIVERS_SInit_t* pParams_p);
extern uint32_t  APP_DRIVERS_deinit     (void);

#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_DRIVERS_H */
