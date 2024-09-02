/*!
 *  \example appHwBoardInfo.h
 *
 *  \brief
 *  Common board info related definitions
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

#ifndef APP_HW_BOARD_INFO
#define APP_HW_BOARD_INFO

#include "appHwError.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define APP_HW_BOARD_INFO_NAME_LEN               16
#define APP_HW_BOARD_INFO_DESIGN_REV_LEN         2
#define APP_HW_BOARD_INFO_PROC_NUM_LEN           4
#define APP_HW_BOARD_INFO_VARIANT_LEN            2
#define APP_HW_BOARD_INFO_PCB_REV_LEN            2
#define APP_HW_BOARD_INFO_SCH_BOM_REV_LEN        2
#define APP_HW_BOARD_INFO_SWR_REV_LEN            2
#define APP_HW_BOARD_INFO_VENDORID_LEN           2
#define APP_HW_BOARD_INFO_BUILD_WEEK_LEN         2
#define APP_HW_BOARD_INFO_BUILD_YEAR_LEN         2
#define APP_HW_BOARD_INFO_BOARD_ID_LEN           6
#define APP_HW_BOARD_INFO_SERIAL_NUM_LEN         4

#define APP_HW_BOARD_INFO_MAC_ADDR_LEN_TOTAL     192

/*!
 *  \brief Structure to store the board info header.
 */
typedef struct APP_HW_BOARD_INFO_SHeader
{
    uint32_t magicNum;
    uint8_t type;
    uint16_t payloadSize;
} __attribute__((packed)) APP_HW_BOARD_INFO_SHeader_t;

/*!
 *  \brief Structure to store the board ident data.
 */
typedef struct APP_HW_BOARD_INFO_SIdent
{
    uint8_t type;
    uint16_t length;
    char name[APP_HW_BOARD_INFO_NAME_LEN];
    char designRev[APP_HW_BOARD_INFO_DESIGN_REV_LEN];
    char procNum[APP_HW_BOARD_INFO_PROC_NUM_LEN];
    char variant[APP_HW_BOARD_INFO_VARIANT_LEN];
    char pcbRev[APP_HW_BOARD_INFO_PCB_REV_LEN];
    char schBomRev[APP_HW_BOARD_INFO_SCH_BOM_REV_LEN];
    char swrRev[APP_HW_BOARD_INFO_SWR_REV_LEN];
    char vendorId[APP_HW_BOARD_INFO_VENDORID_LEN];
    char buildWeek[APP_HW_BOARD_INFO_BUILD_WEEK_LEN];
    char buildYear[APP_HW_BOARD_INFO_BUILD_YEAR_LEN];
    char boardId[APP_HW_BOARD_INFO_BOARD_ID_LEN];
    char serialNum[APP_HW_BOARD_INFO_SERIAL_NUM_LEN];
} __attribute__((packed)) APP_HW_BOARD_INFO_SIdent_t;

/*!
 *  \brief Structure to store the board ddr ram data.
 */
typedef struct APP_HW_BOARD_INFO_SDDRRam
{
    uint8_t type;
    uint16_t length;
    uint16_t ctrl;
} __attribute__((packed)) APP_HW_BOARD_INFO_SDDRRam_t;

/*!
 *  \brief Structure to store the board mac addr data.
 */
typedef struct APP_HW_BOARD_INFO_SMAC_Addr
{
    uint8_t type;
    uint16_t length;
    uint16_t ctrl;
    uint8_t macAddr[APP_HW_BOARD_INFO_MAC_ADDR_LEN_TOTAL];
} __attribute__((packed)) APP_HW_BOARD_INFO_SMAC_Addr_t;

/*!
 *  \brief Structure to store the board data.
 */
typedef struct APP_HW_BOARD_INFO_SData
{
    APP_HW_BOARD_INFO_SHeader_t header;
    APP_HW_BOARD_INFO_SIdent_t boardIdent;
    APP_HW_BOARD_INFO_SDDRRam_t ddr;
    APP_HW_BOARD_INFO_SMAC_Addr_t mac;
    uint8_t endList;
} __attribute__((packed)) APP_HW_BOARD_INFO_SData_t;

APP_HW_EError_t APP_HW_BOARD_INFO_read(void);
APP_HW_BOARD_INFO_SMAC_Addr_t *APP_HW_BOARD_INFO_getMacInfo(void);

#ifdef  __cplusplus
}
#endif

#endif // APP_HW_BOARD_INFO
