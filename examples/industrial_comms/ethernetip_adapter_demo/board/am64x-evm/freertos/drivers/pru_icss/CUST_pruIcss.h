/*!
 *  \file CUST_pruIcss.c
 *
 *  \brief
 *  PRU-ICSS block custom declarations.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
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

#ifndef CUST_PRUICSS_H
#define CUST_PRUICSS_H

#define CUST_PRUICSS_PRU_MAX_NUM  2

typedef enum CUST_PRUICSS_EError
{
    CUST_PRUICSS_eERR_NOERROR       = 0,    /*!< No error, everything is fine. */
    CUST_PRUICSS_eERR_GENERALERROR  = -1    /*!< General error */
} CUST_PRUICSS_EError_t;

typedef struct CUST_PRUICSS_InitParams
{
    uint32_t pruIcssId;
    uint32_t ethPhy0Id;
    uint32_t ethPhy1Id;
}CUST_PRUICSS_InitParams_t;

typedef struct CUST_PRUICSS_PRU_Mdio
{
    uint32_t  isEnabled;
    uint32_t  baseAddress;
    uint32_t* pFirmware;
    uint32_t  frmLength;
    uint32_t  frmConfig;
} CUST_PRUICSS_PRU_Mdio_t;

typedef struct CUST_PRUICSS_PRU_EthPhy
{
    uint32_t       sysConfigId;
    ETHPHY_Config* pConfig;
    ETHPHY_Handle  handle;
} CUST_PRUICSS_PRU_EthPhy_t;

typedef struct CUST_PRUICSS_PRU_Config
{
    uint32_t* pFirmware;
    uint32_t  frmLength;

    CUST_PRUICSS_PRU_EthPhy_t ethPhy;
}CUST_PRUICSS_PRU_Config_t;

typedef struct CUST_PRUICSS_ConfigParams
{
    uint32_t        sysConfigId;
    PRUICSS_Config* pPruIcssCfg;

    CUST_PRUICSS_PRU_Mdio_t   manualMdioMode;
    CUST_PRUICSS_PRU_Config_t pru[CUST_PRUICSS_PRU_MAX_NUM];
}CUST_PRUICSS_ConfigParams_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t                     CUST_PRUICSS_init         (CUST_PRUICSS_InitParams_t* pParams);
extern CUST_PRUICSS_ConfigParams_t* CUST_PRUICSS_getCfgParams (void);
extern PRUICSS_Config*              CUST_PRUICSS_getConfig    (uint32_t sysConfigId);

#ifdef  __cplusplus
}
#endif

#endif // CUST_PRUICSS_H
