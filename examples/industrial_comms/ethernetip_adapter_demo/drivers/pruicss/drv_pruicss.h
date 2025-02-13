/*!
 *  \file drv_pruicss.h
 *
 *  \brief
 *  Declarations related to PRU-ICSS application driver.
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

#ifndef DRV_PRUICSS_H
#define DRV_PRUICSS_H

#define DRV_PRUICSS_PRU_MAX_NUM  2

typedef enum DRV_PRUICSS_EError
{
    DRV_PRUICSS_eERR_NOERROR       = 0,    /*!< No error, everything is fine. */
    DRV_PRUICSS_eERR_GENERALERROR  = -1    /*!< General error */
} DRV_PRUICSS_EError_t;

typedef struct DRV_PRUICSS_SInit
{
    uint32_t instance;
    uint32_t ethPhy0Id;
    uint32_t ethPhy1Id;
}DRV_PRUICSS_SInit_t;

typedef struct DRV_PRUICSS_PRU_Mdio
{
    uint32_t  isEnabled;
    uint32_t  baseAddress;
    uint32_t* pFirmware;
    uint32_t  frmLength;
    uint32_t  frmConfig;
} DRV_PRUICSS_PRU_Mdio_t;

typedef struct DRV_PRUICSS_PRU_EthPhy
{
    uint32_t       sysConfigId;
    ETHPHY_Config* pConfig;
    ETHPHY_Handle  handle;
} DRV_PRUICSS_PRU_EthPhy_t;

typedef struct DRV_PRUICSS_PRU_Config
{
    uint32_t* pFirmware;
    uint32_t  frmLength;

    DRV_PRUICSS_PRU_EthPhy_t ethPhy;
}DRV_PRUICSS_PRU_Config_t;

typedef struct DRV_PRUICSS_ConfigParams
{
    uint32_t        sysConfigId;
    PRUICSS_Config* pPruIcssCfg;

    DRV_PRUICSS_PRU_Mdio_t   manualMdioMode;
    DRV_PRUICSS_PRU_Config_t pru[DRV_PRUICSS_PRU_MAX_NUM];
}DRV_PRUICSS_ConfigParams_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t                     DRV_PRUICSS_init         (DRV_PRUICSS_SInit_t* pParams);
extern DRV_PRUICSS_ConfigParams_t*  DRV_PRUICSS_getCfgParams (void);
extern PRUICSS_Config*              DRV_PRUICSS_getConfig    (uint32_t sysConfigId);

#ifdef  __cplusplus
}
#endif

#endif // DRV_PRUICSS_H
