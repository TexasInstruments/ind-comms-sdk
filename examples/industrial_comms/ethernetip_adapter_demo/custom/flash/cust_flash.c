/*!
 *  \file cust_flash.c
 *
 *  \brief
 *  Provides initialization of custom FLASH's.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2022 Texas Instruments Incorporated
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

#include "ti_board_config.h"
#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#include "board/flash.h"

#include "flash/cust_flash.h"

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
extern Flash_Handle gFlashHandle[CONFIG_FLASH_NUM_INSTANCES];

#if (defined SOC_AM261X)
int32_t IO_EXP_enableLevelTranslator (void);
#endif

#endif

/*!
* <!-- Description: -->
*
* \brief
* Custom FLASH initialization.
*
* \return     uint32_t                      Error code.
*
* \retval     CUST_FLASH_eERR_NOERROR       Success.
* \retval     CUST_FLASH_eERR_GENERALERROR  Failed.
*
*/
uint32_t CUST_FLASH_init (void)
{
    // nothing needs to be done for this board
    return (uint32_t) CUST_FLASH_eERR_NOERROR;
}

/*!
* <!-- Description: -->
*
* \brief
* Custom FLASH deinitialization.
*
* \return     uint32_t                    Error code.
*
* \retval     CUST_FLASH_eERR_NOERROR       Success.
* \retval     CUST_FLASH_eERR_GENERALERROR  Failed.
*
*/
uint32_t CUST_FLASH_deInit(void)
{
    // nothing needs to be done for this board
    return (uint32_t) CUST_FLASH_eERR_NOERROR;
}

/*!
*
*  \brief
*  Provides specific FLASH handle defined by instance.
*
*  \param[in]  instance       SysConfig ID of FLASH
*
*  \return     requested FLASH handler
*
*  \retval     NULL           Failed.
*  \retval     Other          Success.
*
*/
Flash_Handle CUST_FLASH_getHandle(uint32_t instanceId)
{
    Flash_Handle handle = NULL;

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    if (CONFIG_FLASH_NUM_INSTANCES > instanceId)
    {
        handle = gFlashHandle[instanceId];
    }
#else
    OSALUNREF_PARM(instanceId);
#endif

    return handle;
}

/*!
*
*  \brief
*  Makes HW reset of the FLASH.
*
*  \details
*  HW reset is not a part of board FLASH function interface, required for custom FLASH.
*
*/
void CUST_FLASH_reset(void)
{
#if (defined SOC_AM261X) || (defined SOC_AM263PX)

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    uint32_t    gpioBaseAddr;

    Drivers_i2cOpen();

#if (defined SOC_AM261X)
    IO_EXP_enableLevelTranslator();
#endif

    /* Get address after translation translate */
    gpioBaseAddr = (uint32_t) AddrTranslateP_getLocalAddr(GPIO_OSPI_RST_BASE_ADDR);

    GPIO_setDirMode(gpioBaseAddr, GPIO_OSPI_RST_PIN, GPIO_OSPI_RST_DIR);
    GPIO_pinWriteLow(gpioBaseAddr, GPIO_OSPI_RST_PIN);
    GPIO_pinWriteHigh(gpioBaseAddr, GPIO_OSPI_RST_PIN);

    Drivers_i2cClose();
#endif  // (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
#endif  // (defined SOC_AM261X) || (defined SOC_AM263PX)

    return;
}

#if (defined SOC_AM261X)
int32_t IO_EXP_enableLevelTranslator (void)
{
    int32_t  status = SystemP_SUCCESS;

#if (defined CONFIG_FLASH_NUM_INSTANCES) && (CONFIG_FLASH_NUM_INSTANCES > 0)
    static TCA6408_Config  gTCA6408_Config;
    TCA6408_Params      TCA6408Params;
    TCA6408_Params_init(&TCA6408Params);

    TCA6408Params.i2cInstance = CONFIG_I2C0;
    TCA6408Params.i2cAddress  = 0x20U;

    status = TCA6408_open(&gTCA6408_Config, &TCA6408Params);

    /* Configure as output  */
    status += TCA6408_config(
                    &gTCA6408_Config,
                    IO_EXP_BP_BO_MUX_EN_LINE,
                    TCA6408_MODE_OUTPUT);

    /* Configure State */
    status = TCA6408_setOutput(
                    &gTCA6408_Config,
                    IO_EXP_BP_BO_MUX_EN_LINE,
                    TCA6408_OUT_STATE_HIGH);



    if(status != SystemP_SUCCESS)
    {
        DebugP_log("Failed to enable OSPI Reset Signal\r\n");
        TCA6408_close(&gTCA6408_Config);
    }

    if(SystemP_FAILURE == status)
    {
        /* Exit gracefully */
    }
#endif

    return status;
}
#endif
