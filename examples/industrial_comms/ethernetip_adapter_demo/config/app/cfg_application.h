/*!
 *  \file cfg_application.h
 *
 *  \brief
 *  Example application configuration.
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

#ifndef CFG_APPLICATION_H_INC
#define CFG_APPLICATION_H_INC

// Application configuration
#define CFG_APP_MAIN_TASK_PRIO              OSAL_TASK_Prio_EIP_MAIN
#define CFG_APP_MAIN_STACK_SIZE_IN_BYTES    (0x1000u)

// Application drivers configuration
#define CFG_APP_EEPROM_TASK_PRIO    OSAL_TASK_Prio_EIP_EEPROM
#define CFG_APP_FLASH_TASK_PRIO     OSAL_TASK_Prio_EIP_FLASH

// Non-volatile data configuration
#if !(defined FBTLPROVIDER)
#define CFG_APP_NVM_CONFIG_TYPE_FLASH  (0u)
#define CFG_APP_NVM_CONFIG_TYPE_EEPROM (1u)
#endif /* FBTLPROVIDER */

#if (CFG_APP_NVM_CONFIG_TYPE_FLASH)
#define CFG_APP_NVM_CONFIG_TYPE      DEVICE_PROFILE_NVM_eTYPE_FLASH
#define CFG_APP_NVM_TASK_PRIO        OSAL_TASK_Prio_EIP_FLASH
#define CFG_APP_NVM_CONFIG_INSTANCE  CFG_BOARD_FLASH_INSTANCE
#define CFG_APP_NVM_CONFIG_OFFSET    (0x200000u)
#elif (CFG_APP_NVM_CONFIG_TYPE_EEPROM)
#define CFG_APP_NVM_CONFIG_TYPE     DEVICE_PROFILE_NVM_eTYPE_EEPROM
#define CFG_APP_NVM_TASK_PRIO       OSAL_TASK_Prio_EIP_EEPROM
#define CFG_APP_NVM_CONFIG_INSTANCE CFG_BOARD_EEPROM_INSTANCE
#define CFG_APP_NVM_CONFIG_OFFSET   (0x200u)
#else
#define CFG_APP_NVM_CONFIG_TYPE      DEVICE_PROFILE_NVM_eTYPE_UNDEFINED
#define CFG_APP_NVM_CONFIG_INSTANCE  CONFIG_INSTANCE_INVALID
#define CFG_APP_NVM_CONFIG_OFFSET    (0x0u)
#endif

// Web Server configuration
#define CFG_APP_WEBSERVER_TASK_PRIO         OSAL_TASK_Prio_EIP_WEBSERVER
#define CFG_APP_WEBSERVER_CPULOAD_TASK_PRIO OSAL_TASK_Prio_EIP_CPULOAD
#define CFG_APP_WEBSERVER_OUTPUT            CMN_CPU_API_eOUT_NONE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __cplusplus
}
#endif

#endif // CFG_APPLICATION_H_INC
