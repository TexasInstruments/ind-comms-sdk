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
#include <drivers/pinmux.h>

static Pinmux_PerCfg_t gPinMuxMainDomainCfg[] = {

    {
    PIN_ECAP0_IN_APWM_OUT,
    ( PIN_MODE(1) | PIN_PULL_DISABLE ) /* PIN_MODE 1 is SYNC0_OUT */
    },
            /* I2C1 pin config */
    /* I2C1_SCL -> I2C1_SCL (C18) */
    {
        PIN_I2C1_SCL,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* I2C1 pin config */
    /* I2C1_SDA -> I2C1_SDA (B19) */
    {
        PIN_I2C1_SDA,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
            /* I2C0 pin config */
    /* I2C0_SCL -> I2C0_SCL (A18) */
    {
        PIN_I2C0_SCL,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* I2C0 pin config */
    /* I2C0_SDA -> I2C0_SDA (B18) */
    {
        PIN_I2C0_SDA,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

            /* OSPI0 pin config */
    /* OSPI0_CLK -> OSPI0_CLK (N20) */
    {
        PIN_OSPI0_CLK,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_CSn0 -> OSPI0_CSn0 (L19) */
    {
        PIN_OSPI0_CSN0,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_D0 -> OSPI0_D0 (M19) */
    {
        PIN_OSPI0_D0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_D1 -> OSPI0_D1 (M18) */
    {
        PIN_OSPI0_D1,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_D2 -> OSPI0_D2 (M20) */
    {
        PIN_OSPI0_D2,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_D3 -> OSPI0_D3 (M21) */
    {
        PIN_OSPI0_D3,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_D4 -> OSPI0_D4 (P21) */
    {
        PIN_OSPI0_D4,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_D5 -> OSPI0_D5 (P20) */
    {
        PIN_OSPI0_D5,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_D6 -> OSPI0_D6 (N18) */
    {
        PIN_OSPI0_D6,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_D7 -> OSPI0_D7 (M17) */
    {
        PIN_OSPI0_D7,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* OSPI0 pin config */
    /* OSPI0_DQS -> OSPI0_DQS (N19) */
    {
        PIN_OSPI0_DQS,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

        

            /* PRU_ICSSG1_MDIO0 pin config */
    /* PRG1_MDIO0_MDC -> PRG1_MDIO0_MDC (Y6) */
    {
        PIN_PRG1_MDIO0_MDC,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MDIO0 pin config */
    /* PRG1_MDIO0_MDIO -> PRG1_MDIO0_MDIO (AA6) */
    {
        PIN_PRG1_MDIO0_MDIO,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_RXD0 -> PRG1_PRU0_GPO0 (Y7) */
    {
        PIN_PRG1_PRU0_GPO0,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_RXD1 -> PRG1_PRU0_GPO1 (U8) */
    {
        PIN_PRG1_PRU0_GPO1,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_RXD2 -> PRG1_PRU0_GPO2 (W8) */
    {
        PIN_PRG1_PRU0_GPO2,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_RXD3 -> PRG1_PRU0_GPO3 (V8) */
    {
        PIN_PRG1_PRU0_GPO3,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_RXDV -> PRG1_PRU0_GPO4 (Y8) */
    {
        PIN_PRG1_PRU0_GPO4,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_RXER -> PRG1_PRU0_GPO5 (V13) */
    {
        PIN_PRG1_PRU0_GPO5,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_RXLINK -> PRG1_PRU0_GPO8 (W13) */
    {
        PIN_PRG1_PRU0_GPO8,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_TXD0 -> PRG1_PRU0_GPO11 (AA8) */
    {
        PIN_PRG1_PRU0_GPO11,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_TXD1 -> PRG1_PRU0_GPO12 (U9) */
    {
        PIN_PRG1_PRU0_GPO12,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_TXD2 -> PRG1_PRU0_GPO13 (W9) */
    {
        PIN_PRG1_PRU0_GPO13,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_TXD3 -> PRG1_PRU0_GPO14 (AA9) */
    {
        PIN_PRG1_PRU0_GPO14,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII0_TXEN -> PRG1_PRU0_GPO15 (Y9) */
    {
        PIN_PRG1_PRU0_GPO15,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_RXD0 -> PRG1_PRU1_GPO0 (W11) */
    {
        PIN_PRG1_PRU1_GPO0,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_RXD1 -> PRG1_PRU1_GPO1 (V11) */
    {
        PIN_PRG1_PRU1_GPO1,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_RXD2 -> PRG1_PRU1_GPO2 (AA12) */
    {
        PIN_PRG1_PRU1_GPO2,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_RXD3 -> PRG1_PRU1_GPO3 (Y12) */
    {
        PIN_PRG1_PRU1_GPO3,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_RXDV -> PRG1_PRU1_GPO4 (W12) */
    {
        PIN_PRG1_PRU1_GPO4,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_RXER -> PRG1_PRU1_GPO5 (AA13) */
    {
        PIN_PRG1_PRU1_GPO5,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_RXLINK -> PRG1_PRU1_GPO8 (U12) */
    {
        PIN_PRG1_PRU1_GPO8,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_TXD0 -> PRG1_PRU1_GPO11 (AA10) */
    {
        PIN_PRG1_PRU1_GPO11,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_TXD1 -> PRG1_PRU1_GPO12 (V10) */
    {
        PIN_PRG1_PRU1_GPO12,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_TXD2 -> PRG1_PRU1_GPO13 (U10) */
    {
        PIN_PRG1_PRU1_GPO13,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_TXD3 -> PRG1_PRU1_GPO14 (AA11) */
    {
        PIN_PRG1_PRU1_GPO14,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII1_TXEN -> PRG1_PRU1_GPO15 (Y11) */
    {
        PIN_PRG1_PRU1_GPO15,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII_MR0_CLK -> PRG1_PRU0_GPO6 (AA7) */
    {
        PIN_PRG1_PRU0_GPO6,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII_MR1_CLK -> PRG1_PRU1_GPO6 (U11) */
    {
        PIN_PRG1_PRU1_GPO6,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII_MT0_CLK -> PRG1_PRU0_GPO16 (V9) */
    {
        PIN_PRG1_PRU0_GPO16,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* PRU_ICSSG1_MII_G_RT pin config */
    /* PR1_MII_MT1_CLK -> PRG1_PRU1_GPO16 (Y10) */
    {
        PIN_PRG1_PRU1_GPO16,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

            /* USART0 pin config */
    /* UART0_RXD -> UART0_RXD (D15) */
    {
        PIN_UART0_RXD,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* USART0 pin config */
    /* UART0_TXD -> UART0_TXD (C16) */
    {
        PIN_UART0_TXD,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },

    {PINMUX_END, PINMUX_END}
};

static Pinmux_PerCfg_t gPinMuxMcuDomainCfg[] = {
                
        
            /* MCU_GPIO0 pin config */
    /* MCU_GPIO0_5 -> MCU_SPI1_CS0 (A7) */
    {
        PIN_MCU_SPI1_CS0,
        ( PIN_MODE(7) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },


        
        
    {PINMUX_END, PINMUX_END}
};

/*
 * Pinmux
 */


void Pinmux_init(void)
{
    Pinmux_config(gPinMuxMainDomainCfg, PINMUX_DOMAIN_ID_MAIN);

    
    Pinmux_config(gPinMuxMcuDomainCfg, PINMUX_DOMAIN_ID_MCU);
}

