/*!
 *  \file iolm_port_spi.h
 *
 *  \brief
 *  Interface for SPI Communication on IOLink Board.
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

#ifndef IO_LINK_IOLINK_SPI_H_
#define IO_LINK_IOLINK_SPI_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <stdbool.h>
#include "osal.h"
#include "IOL_Port_Types.h"
#include "iolm_port_ledtask.h"
#include <drivers/mcspi.h>
#include <kernel/dpl/CacheP.h>
#include "ti_drivers_config.h"

/* ========================================================================== */
/*                                Defines                                     */
/* ========================================================================== */

#define IOLM_SPI_LED_WIDTH_BIT_MASK (16U)
#define IOLM_SPI_LED_NUM_BYTES      (IOLM_SPI_LED_WIDTH_BIT_MASK / 8)

#define IOLM_SPI_LED_INSTANCE       CONFIG_MCSPI_IOL
#define IOLM_SPI_LED_CHANNEL        (1U)
#define IOLM_SPI_LED_DATA_SIZE      (16U)

#define IOLM_SPI_IQ_INSTANCE        CONFIG_MCSPI_IOL
#define IOLM_SPI_IQ_CHANNEL         (0U)
#define IOLM_SPI_IQ_DATA_SIZE       (8U)


/* ========================================================================== */
/*                             typedefs                                       */
/* ========================================================================== */

typedef struct IOLM_SPI_Mapping
{
    uint32_t spiInstance;
    uint32_t spiChannel;
    uint32_t spiDataSize;
} IOLM_SPI_mapping_t;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief  Initialize the first SPI instance
 *
 *
 * \return void
 *
 */
void IOLM_SPI_init(void);

/**
 * \brief  Close the first SPI instance
 *
 *
 * \return void
 *
 */
void IOLM_SPI_close(void);

/**
 * \brief  Transfer and Reveive data to a specific MCSPI instance/channel
 *
 * \param[in]   mcspiInstance       MCSPI instance as defined in SysConfig
 * \param[in]   mcspiChannel        MCSPI channel as defined in SysConfig
 * \param[in]   mcspiDataSize       bit width of data to send (4 to 32 bits)
 * \param[in]   pTxData             pointer to transmit buffer
 * \param[in]   pRxData             pointer to receive buffer
 * \param[in]   lengthInBytes       length of transaction in bits
 *
 * \return      error               as int32_t
 * \retval      #SystemP_SUCCESS    on Success
 * \retval      #SystemP_FAILURE    on Failure
 *
 */
int32_t IOLM_SPI_mcspiTransfer(
    uint32_t mcspiInstance,
    uint32_t mcspiChannel,
    uint32_t mcspiDataSize,
    uint8_t *pTxData,
    uint8_t *pRxData,
    uint32_t lengthInBytes);

/**
 * \brief  Transfer and Receive data to a specific MCSPI instance/channel
 *
 * WARNING: DEPRECATED!
 * This function is only used in Linux ports.
 * If this is touched for any reason, please use IOLM_SPI_mcspiTransfer instead!
 *
 * \param[in]   spiInstance         MCSPI instance as defined in SysConfig
 * \param[in]   mcspiChannel        MCSPI channel as defined in SysConfig
 * \param[in]   tx                  32 bit data to transmit
 *
 * \return      rxBuf               as uint32_t
 *
 */
uint32_t IOLM_SPI_transfer(uint32_t spiInstance, uint32_t mcspiChannel, uint32_t tx);

/**
 * \brief  Set the Status-LEDs on the IO-Link adapter board
 *
 * \param[in]   bitMask             bitmask of the target LED states, red/green alternating, 16bits,
 * staring at port 0
 *
 * \return      error               as int32_t
 * \retval      #SystemP_SUCCESS    on Success
 * \retval      #SystemP_FAILURE    on Failure
 *
 */
int32_t IOLM_SPI_setIolLeds(uint16_t bitMask);

/**
 * \brief  Get state of IQ pin on specified IO-Link port through SPI
 *
 * \param[in]   portNum      IO-Link port number
 * staring at port 0
 *
 * \return      error               as int32_t
 * \retval      #SystemP_SUCCESS    on Success
 * \retval      #SystemP_FAILURE    on Failure
 *
 */
bool IOLM_SPI_getIq(uint8_t portNum);

/**
 * \brief  Set state of LED on base board
 *
 * \param[in]   ledNumber           number of LED
 * \param[in]   ledState            target state
 *
 * \return      error               as int32_t
 * \retval      #SystemP_SUCCESS    on Success
 * \retval      #SystemP_FAILURE    on Failure
 *
 */
int32_t IOLM_SPI_baseBoardLED(uint32_t ledNumber, bool ledState);


#endif /* IO_LINK_IOLINK_SPI_H_ */
