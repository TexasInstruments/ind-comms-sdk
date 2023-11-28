/*!
 *  \file ESL_BOARD_config.h
 *
 *  \brief
 *  Board configuration for AM64 GP EVM.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 KUNBUS GmbH.
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

#if !(defined __ESL_BOARD_CONFIG_H__)
#define __ESL_BOARD_CONFIG_H__		1

#include <ESL_OS_os.h>
#include <ESL_BOARD_OS_config.h>
#include <ecSlvApi.h>

#define ECAT_PHYADDR_IN                     (15u)
#define ECAT_PHYADDR_OUT                    (3u)
#define ECAT_PHYPOLINVERT_IN                (true)
#define ECAT_PHYPOLINVERT_OUT               (true)
#define ECAT_PHYUSERXLINK_IN                (true)
#define ECAT_PHYUSERXLINK_OUT               (true)

#define TIESC_I2CDEVICE                     "/dev/i2c-0"

 #define ECAT_PRODUCTCODE_CTT               0x54490020      // set product code
 #define ECAT_PRODUCTCODE_CIA402            0x54490021      // set product code
 #define ECAT_PRODUCTCODE_SIMPLE            0x54490022      // set product code
 #define ECAT_PRODUCTNAME_CTT               "TI EtherCAT Toolkit Conformance for AM64X.R5F"
 #define ECAT_PRODUCTNAME_CIA402            "TI EtherCAT Toolkit CiA402 for AM64X.R5F"
 #define ECAT_PRODUCTNAME_SIMPLE            "TI EtherCAT Toolkit for AM64X.R5F"
 #define ECAT_REVISION_NO                   0x00010000

#if (defined __cplusplus)
extern "C" {
#endif

extern void ESL_BOARD_OS_initPruss          (uint32_t pruSelect
                                            ,int32_t *pBaseIrqOffset);
extern void ESL_BOARD_OS_initStatusLED      (void *gpioHandle
                                            ,uint32_t selectedPru);
extern void ESL_BOARD_OS_statusLED          (void *gpioHandle
                                            ,uint32_t selectedPru
                                            ,bool runLed
                                            ,bool errLed);
extern void ESL_BOARD_OS_configureResets    (void *pGpioHandle
                                            ,uint32_t selectedPru);
extern void ESL_BOARD_OS_phyReset           (void *pGpioHandle
                                            ,uint32_t selectedPru
                                            ,uint8_t phyIdx
                                            ,bool reset);
extern void ESL_BOARD_OS_registerPhys       (EC_API_SLV_SHandle_t *pHandle
                                            ,uint32_t selectedPru);
#if !(defined FBTLTESTAPP) || (0==FBTLTESTAPP)
extern void ESL_BOARD_OS_registerPhys       (EC_API_SLV_SHandle_t *pHandle
                                            ,uint32_t selectedPru);
#endif

#if (defined __cplusplus)
}
#endif

#endif /* __ESL_BOARD_CONFIG_H__ */
