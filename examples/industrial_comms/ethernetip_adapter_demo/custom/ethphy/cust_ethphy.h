/*!
 *  \file cust_ethphy.h
 *
 *  \brief
 *  Custom ETHPHY interface.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

#if !(defined _CUST_ETHPHY_H_)
#define _CUST_ETHPHY_H_     1

#define CUST_ETHPHY_MAX_NUM  2

typedef enum CUST_ETHPHY_EError
{
    CUST_ETHPHY_eERR_NOERROR           = 0,    /*!< No error, everything is fine. */
    CUST_ETHPHY_eERR_GENERALERROR      = -1    /*!< General error */
} CUST_ETHPHY_EError_t;

typedef struct CUST_ETHPHY_SInit
{
    uint32_t                instance_0;               /* Instance of ETHPHY0 connected to PRU-ICSS block used by stack (as defined by SysConfig) */
    uint32_t                instance_1;               /* Instance of ETHPHY1 connected to PRU-ICSS block used by stack (as defined by SysConfig) */
    OSAL_TASK_Priority_t    taskPrioPhyMdixTask;      /* Task priority for PHY MDIX task */
}CUST_ETHPHY_SInit_t;

typedef struct CUST_ETHPHY_SParams
{
    uint32_t pruIcssSysConfigId;    /* SysConfig ID of PRU-ICSS block used by stack */
    uint32_t ethPhySysConfigId_0;   /* SysConfig ID of ETHPHY0 connected to PRU-ICSS block used by stack */
    uint32_t ethPhySysConfigId_1;   /* SysConfig ID of ETHPHY1 connected to PRU-ICSS block used by stack) */
}CUST_ETHPHY_SParams_t;

#if (defined __cplusplus)
extern "C" {
#endif

extern uint32_t       CUST_ETHPHY_init                          (CUST_ETHPHY_SParams_t* pParams);
extern uint32_t       CUST_ETHPHY_deInit                        (void);
extern ETHPHY_Handle  CUST_ETHPHY_getHandle                     (uint32_t instance);
extern ETHPHY_Config* CUST_ETHPHY_getConfig                     (uint32_t instance);
extern bool           CUST_ETHPHY_MDIOMANUALMODE_isEnabled      (void);
extern bool           CUST_ETHPHY_MDIOMANUALMODE_isMlinkBased   (void);
extern uint32_t       CUST_ETHPHY_MDIOMANUALMODE_getBaseAddress (void);

#if (defined __cplusplus)
}
#endif

#endif /* _CUST_ETHPHY_H_ */
