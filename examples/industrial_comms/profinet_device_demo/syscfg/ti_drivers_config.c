/*
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
/*
 * Auto generated file
 */

#include "ti_drivers_config.h"
#include <drivers/sciclient.h>

/*
 * I2C
 */
/* I2C atrributes */
static I2C_HwAttrs gI2cHwAttrs[CONFIG_I2C_NUM_INSTANCES] =
{
    {
        .baseAddr       = CSL_I2C1_CFG_BASE,
        .intNum         = 194,
        .eventId        = 0,
        .funcClk        = 96000000U,
        .enableIntr     = 1,
        .ownTargetAddr   =
        {
            0x1C,
            0x1C,
            0x1C,
            0x1C,
        },
    },
    {
        .baseAddr       = CSL_I2C0_CFG_BASE,
        .intNum         = 193,
        .eventId        = 0,
        .funcClk        = 96000000U,
        .enableIntr     = 1,
        .ownTargetAddr   =
        {
            0x1C,
            0x1C,
            0x1C,
            0x1C,
        },
    },
};
/* I2C objects - initialized by the driver */
static I2C_Object gI2cObjects[CONFIG_I2C_NUM_INSTANCES];
/* I2C driver configuration */
I2C_Config gI2cConfig[CONFIG_I2C_NUM_INSTANCES] =
{
    {
        .object = &gI2cObjects[CONFIG_I2C1_STATUS_LEDS],
        .hwAttrs = &gI2cHwAttrs[CONFIG_I2C1_STATUS_LEDS]
    },
    {
        .object = &gI2cObjects[CONFIG_I2C0_EEPROM],
        .hwAttrs = &gI2cHwAttrs[CONFIG_I2C0_EEPROM]
    },
};

uint32_t gI2cConfigNum = CONFIG_I2C_NUM_INSTANCES;

/*
 * OSPI
 */


/* Regions restricted for DMA. We should use CPU memcpy in these cases */
static OSPI_AddrRegion gOspiDmaRestrictRegions[] = 
{
    {
        .regionStartAddr = CSL_R5FSS0_ATCM_BASE,
        .regionSize      = CSL_R5FSS0_ATCM_SIZE,
    },  
    {
        .regionStartAddr = CSL_MCU_M4FSS0_IRAM_BASE,
        .regionSize      = CSL_MCU_M4FSS0_IRAM_SIZE,
    },  
    {
        .regionStartAddr = CSL_MCU_M4FSS0_DRAM_BASE,
        .regionSize      = CSL_MCU_M4FSS0_DRAM_SIZE,
    },  
    {
        .regionStartAddr = 0xFFFFFFFFU,
        .regionSize      = 0U,
    }
};

/* OSPI attributes */
static OSPI_Attrs gOspiAttrs[CONFIG_OSPI_NUM_INSTANCES] =
{
    {
        .baseAddr             = CSL_FSS0_OSPI0_CTRL_BASE,
        .dataBaseAddr         = CSL_FSS0_DAT_REG1_BASE,
        .inputClkFreq         = 166666666U,
        .intrNum              = 171U,
        .intrEnable           = FALSE,
        .intrPriority         = 4U,
        .dmaEnable            = FALSE,
        .phyEnable            = FALSE,
        .dacEnable            = FALSE,
        .chipSelect           = OSPI_CS0,
        .frmFmt               = OSPI_FF_POL0_PHA0,
        .decChipSelect        = OSPI_DECODER_SELECT4,
        .baudRateDiv          = 4,
        .dmaRestrictedRegions = gOspiDmaRestrictRegions,
    },
};
/* OSPI objects - initialized by the driver */
static OSPI_Object gOspiObjects[CONFIG_OSPI_NUM_INSTANCES];
/* OSPI driver configuration */
OSPI_Config gOspiConfig[CONFIG_OSPI_NUM_INSTANCES] =
{
    {
        &gOspiAttrs[CONFIG_OSPI0],
        &gOspiObjects[CONFIG_OSPI0],
    },
};

uint32_t gOspiConfigNum = CONFIG_OSPI_NUM_INSTANCES;

#include <drivers/ospi/v0/dma/ospi_dma.h>
OSPI_DmaConfig gOspiDmaConfig[CONFIG_OSPI_NUM_DMA_INSTANCES] =
{
};

uint32_t gOspiDmaConfigNum = CONFIG_OSPI_NUM_DMA_INSTANCES;

/*
 * GPIO
 */

/* ----------- GPIO Direction, Trigger, Interrupt initialization ----------- */

void GPIO_init()
{
    uint32_t    baseAddr;

    /* Instance 0 */
    /* Get address after translation translate */
    baseAddr = (uint32_t) AddrTranslateP_getLocalAddr(CONFIG_GPIO0_DCP_SIGNAL_LED_BASE_ADDR);
    GPIO_pinWriteLow(baseAddr, CONFIG_GPIO0_DCP_SIGNAL_LED_PIN);

    GPIO_setDirMode(baseAddr, CONFIG_GPIO0_DCP_SIGNAL_LED_PIN, CONFIG_GPIO0_DCP_SIGNAL_LED_DIR);
}


/* ----------- GPIO Interrupt de-initialization ----------- */
void GPIO_deinit()
{

}

/*
 * PRUICSS
 */
/* PRUICSS HW attributes - provided by the driver */
extern PRUICSS_HwAttrs gPruIcssHwAttrs_ICSSG1;

/* PRUICSS objects - initialized by the driver */
static PRUICSS_Object gPruIcssObjects[CONFIG_PRUICSS_NUM_INSTANCES];
/* PRUICSS driver configuration */
PRUICSS_Config gPruIcssConfig[CONFIG_PRUICSS_NUM_INSTANCES] =
{
    {
        .object = &gPruIcssObjects[CONFIG_PRU_ICSS1],
        .hwAttrs = &gPruIcssHwAttrs_ICSSG1
    },
};

uint32_t gPruIcssConfigNum = CONFIG_PRUICSS_NUM_INSTANCES;


/*
 * ICSS EMAC
 */

/* ICSS EMAC Packet Buffers */
#define ICSS_EMAC_PKT_BUF_1_MEM_SIZE (65536)
uint8_t gIcssEmacPktBufMem1[ICSS_EMAC_PKT_BUF_1_MEM_SIZE] __attribute__((aligned(128), section(".bss.icss_emac_pktbuf_mem")));

/* ICSS EMAC atrributes */
static ICSS_EMAC_Attrs gIcssEmacAttrs[CONFIG_ICSS_EMAC_INSTANCES] =
{
    {
        .emacMode = (ICSS_EMAC_MODE_SWITCH | 0),
        .phyAddr = {15, 3},
        .phyToMacInterfaceMode = ICSS_EMAC_MII_MODE,
        .halfDuplexEnable = 0,
        .enableIntrPacing = ICSS_EMAC_DISABLE_PACING,
        .intrPacingMode = ICSS_EMAC_INTR_PACING_MODE1,
        .pacingThreshold = 100,
        .ethPrioQueue = ICSS_EMAC_QUEUE3,
        .learningEnable = ICSS_EMAC_LEARNING_ENABLE,
        .portMask = ICSS_EMAC_MODE_SWITCH,
        .txInterruptEnable = 0,
        .linkIntNum = 254,
        .rxIntNum = 248,
        .txIntNum = 250,
        .l3OcmcBaseAddr = (uint32_t)&gIcssEmacPktBufMem1[0],
        .l3OcmcSize = ICSS_EMAC_PKT_BUF_1_MEM_SIZE,
        .linkTaskPriority = 28,
        .rxTaskPriority = 28,
        .txTaskPriority = 28,
        .splitQueue = 0,
    },
};
/* ICSS EMAC objects - initialized by the driver */
static ICSS_EMAC_InternalObject gIcssEmacObjects[CONFIG_ICSS_EMAC_INSTANCES];
/* ICSS EMAC driver configuration */
ICSS_EMAC_Config gIcssEmacConfig[CONFIG_ICSS_EMAC_INSTANCES] =
{
    {
        &gIcssEmacObjects[CONFIG_ICSS_EMAC0],
        &gIcssEmacAttrs[CONFIG_ICSS_EMAC0],
    },
};
uint32_t gIcssEmacConfigNum = CONFIG_ICSS_EMAC_INSTANCES;


/*
 * UART
 */

/* UART atrributes */
static UART_Attrs gUartAttrs[CONFIG_UART_NUM_INSTANCES] =
{
        {
            .baseAddr           = CSL_UART0_BASE,
            .inputClkFreq       = 48000000U,
        },
};
/* UART objects - initialized by the driver */
static UART_Object gUartObjects[CONFIG_UART_NUM_INSTANCES];
/* UART driver configuration */
UART_Config gUartConfig[CONFIG_UART_NUM_INSTANCES] =
{
        {
            &gUartAttrs[CONFIG_UART_CONSOLE],
            &gUartObjects[CONFIG_UART_CONSOLE],
        },
};

uint32_t gUartConfigNum = CONFIG_UART_NUM_INSTANCES;

#include <drivers/uart/v0/lld/dma/uart_dma.h>
#include <drivers/udma.h>
UART_DmaHandle gUartDmaHandle[] =
{

};
Udma_DrvObject gUdmaDrvObj[] =
{

};

uint32_t gUartDmaConfigNum = CONFIG_UART_NUM_DMA_INSTANCES;


void Drivers_uartInit(void)
{
    UART_init();
}


void Pinmux_init(void);
void PowerClock_init(void);
void PowerClock_deinit(void);

/*
 * Common Functions
 */
void System_init(void)
{
    /* DPL init sets up address transalation unit, on some CPUs this is needed
     * to access SCICLIENT services, hence this needs to happen first
     */
    Dpl_init();
    /* We should do sciclient init before we enable power and clock to the peripherals */
    /* SCICLIENT init */
    {
        int32_t retVal = SystemP_SUCCESS;

        retVal = Sciclient_init(CSL_CORE_ID_R5FSS0_0);
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);
    }

    
    /* initialize PMU */
    CycleCounterP_init(SOC_getSelfCpuClk());


    PowerClock_init();
    /* Now we can do pinmux */
    Pinmux_init();
    /* finally we initialize all peripheral drivers */
    I2C_init();
    OSPI_init();
    GPIO_init();
    PRUICSS_init();
    /* PRU IEP Enable SYNC MODE */
    CSL_REG32_WR(CSL_PRU_ICSSG1_PR1_CFG_SLV_BASE + CSL_ICSSCFG_IEPCLK, 1);
    CSL_REG32_WR(CSL_TIMESYNC_EVENT_INTROUTER0_CFG_BASE + 0x64, 0x0001001D);
    ICSS_EMAC_init();
    Drivers_uartInit();
}

void System_deinit(void)
{
    I2C_deinit();
    OSPI_deinit();
    GPIO_deinit();
    PRUICSS_deinit();
    ICSS_EMAC_deinit();
    UART_deinit();
    PowerClock_deinit();
    /* SCICLIENT deinit */
    {
        int32_t         retVal = SystemP_SUCCESS;

        retVal = Sciclient_deinit();
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);
    }
    Dpl_deinit();
}