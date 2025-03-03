/*!
 *  \file app.c
 *
 *  \brief
 *  EtherNet/IP&trade; Adapter Example Application.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2021-2025 Texas Instruments Incorporated
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "ti_drivers_config.h"
#include "ti_board_config.h"

#include "osal.h"
#include "osal_error.h"
#include "hwal.h"

#include "EI_API.h"
#include "EI_API_def.h"

#include "cfg_example.h"

#include "cust_drivers.h"

#include "drivers/drivers.h"

#include "cmn_os.h"
#include "cmn_app.h"
#include "cmn_cpu_api.h"

#include "device_profiles/common/device_profile_nvm.h"
#include "device_profiles/common/device_profile_cfg.h"

#include "web_server.h"

#include "app_task.h"
#include "app.h"

static APP_SInstance_t appInstance_s = {0};

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Main entry point.
 *
 *  \details
 *  Main entry point.<br>
 *  Initializes the general operating system abstraction layer,
 *  starts the EtherNet/IP&trade; main application task, and starts the
 *  operating system abstraction layer.
 *
 */
int main(
    int argc,
    char* argv[])
{
    uint32_t err;

    APP_SParams_t* pCfg = &appInstance_s.config;

    /* Application configuration. */
    pCfg->application.taskPrio = CFG_APP_MAIN_TASK_PRIO;

    /* HWAL configuration. */
    pCfg->hwal.taskPrioWatchDog = CFG_STACK_HWAL_WATCHDOG_TASK_PRIO;    // Value change will have no impact.
    pCfg->hwal.taskPrioLicense  = CFG_STACK_HWAL_LICENSE_TASK_PRIO;     // Value change will have no impact.

    /* LWIP configuration */
    pCfg->lwip.taskPrio = CFG_STACK_LWIP_TCPIP_TASK_PRIO;  // Value change will have no impact.

    /* CIP configuration */
    pCfg->cip.maxInstanceNum = CFG_STACK_CIP_INSTANCES_MAX; // Maximum number of instances defined as CIP object. Set not lower as 70.

    /* EtherNet/IP Adapter configuration */
    pCfg->adapter.taskPrioCyclicIo  = CFG_STACK_CYCLIC_IO_TASK_PRIO;
    pCfg->adapter.taskPrioPacket    = CFG_STACK_PACKET_TASK_PRIO;
    pCfg->adapter.taskPrioStatistic = CFG_STACK_STATISTIC_TASK_PRIO;

    pCfg->adapter.taskPrioPtpDelayRqTx   = CFG_STACK_TIMESYNC_DEL_TASK_PRIO;
    pCfg->adapter.taskPrioPtpTxTimeStamp = CFG_STACK_TIMESYNC_TS_TASK_PRIO;
    pCfg->adapter.taskPrioPtpNRT         = CFG_STACK_TIMESYNC_NRT_TASK_PRIO;
    pCfg->adapter.taskPrioPtpBackground  = CFG_STACK_TIMESYNC_BACKGROUND_TASK_PRIO;

    pCfg->adapter.taskPrioLldpReceive     = CFG_STACK_LLDP_RX_TASK_PRIO;
    pCfg->adapter.lldpMaxNeighborDevices  = CFG_STACK_LLDP_NEIGHBOR_DEVICES_MAX;

    /* Drivers configuration. */

    /* ETHPHY driver configuration */
    pCfg->drivers.custom.ethPhy.instance_0          = CFG_BOARD_PRU_ICSS_ETHPHY_0_INSTANCE;
    pCfg->drivers.custom.ethPhy.instance_1          = CFG_BOARD_PRU_ICSS_ETHPHY_1_INSTANCE;
    pCfg->drivers.custom.ethPhy.taskPrioPhyMdixTask = CFG_STACK_DRIVERS_ETHPHY_MDIX_TASK_PRIO;

    /* Application drivers configuration. */

    /* Application PRUICSS driver */
    pCfg->drivers.app.pruicss.instance           = CFG_BOARD_PRU_ICSS_BLOCK_INSTANCE;
    pCfg->drivers.app.pruicss.ethPhy0Id          = CFG_BOARD_PRU_ICSS_ETHPHY_0_INSTANCE;
    pCfg->drivers.app.pruicss.ethPhy1Id          = CFG_BOARD_PRU_ICSS_ETHPHY_1_INSTANCE;

    /* Application UART driver configuration. */
    pCfg->drivers.app.uart.instance = CFG_BOARD_UART_INSTANCE;

    /* Application LED's driver configuration */
#ifndef ENABLE_INTERCORE_TUNNELING
    pCfg->drivers.app.led.instance = CFG_BOARD_LED_INSTANCE;
#endif

    /* Application EEPROM driver configuration */
    pCfg->drivers.app.eeprom.taskPrio = CFG_APP_EEPROM_TASK_PRIO;

    /* Application FLASH driver configuration */
    pCfg->drivers.app.flash.taskPrio = CFG_APP_FLASH_TASK_PRIO;

    /* Application UART driver configuration */
    pCfg->drivers.app.uart.taskPrio = CFG_APP_UART_TASK_PRIO;

    /* Non-volatile memory */

    /* Non-volatile memory configuration data */
    pCfg->config.type     = CFG_APP_NVM_CONFIG_TYPE;
    pCfg->config.address  = CFG_APP_NVM_CONFIG_OFFSET;
    pCfg->config.instance = CFG_APP_NVM_CONFIG_INSTANCE;

    /* Adress conflict detection */
    pCfg->acd.initialDelay = CFG_STACK_ACD_INTIAL_DELAY;

#if (defined CPU_LOAD_MONITOR) && (1==CPU_LOAD_MONITOR)
    /* Web Server configuration. */
    pCfg->webServer.taskPrio = CFG_APP_WEBSERVER_TASK_PRIO;

    /* CPU load configuration. */
    pCfg->cpuLoad.taskPrio   = CFG_APP_WEBSERVER_CPULOAD_TASK_PRIO;
    pCfg->cpuLoad.output     = CFG_APP_WEBSERVER_OUTPUT;
#elif (defined UART_CPU_LOAD_MONITOR) && (1==UART_CPU_LOAD_MONITOR)
    /* CPU load configuration. */
    pCfg->cpuLoad.taskPrio   = CFG_APP_WEBSERVER_TASK_PRIO;
    pCfg->cpuLoad.output     = CFG_APP_WEBSERVER_OUTPUT;
#endif

    CMN_OS_init ();

    err = OSAL_init ();
    if (err != OSAL_NO_ERROR)
    {
        goto laError;
    }

    OSAL_registerErrorHandler (EI_APP_TASK_osErrorHandlerCb);

#if defined(EIP_QUICK_CONNECT) && (EIP_QUICK_CONNECT == 1)
    OSAL_printfSuppress(true);
#endif

    CMN_APP_mainCreate(EI_APP_TASK_main, &appInstance_s, pCfg->application.taskPrio);

    OSAL_startOs ();

    return (0);

//-------------------------------------------------------------------------------------------------
laError:

    return (-1);
}
