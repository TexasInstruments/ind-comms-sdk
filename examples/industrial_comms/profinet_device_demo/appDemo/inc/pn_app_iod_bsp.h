/*!
 * \file pn_app_iod_bsp.h
 *
 * \brief
 * Functions and callbacks for handling the board support package like memory and LED control.
 *
 * \author
 * KUNBUS GmbH
 *
 * \copyright
 * Copyright (c) 2023, KUNBUS GmbH<br /><br />
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024 KUNBUS GmbH.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * <ol>
 * <li>Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer./<li>
 * <li>Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.</li>
 * <li>Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.</li>
 * </ol>
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef PN_APP_IOD_BSP_H
#define PN_APP_IOD_BSP_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

#define PN_APP_IOD_NV_STRUCT_VERSION   1 /*!< Version of PN_APP_IOD_Nvdata_t */

#define PN_APP_IOD_BSP_CHECKSUM_SEED    0x55555555

#if 0
// use eeprom
#define PN_APP_IOD_NVM_TYPE             NVM_TYPE_EEPROM
#define PN_APP_IOD_NVM_INSTANCE         CONFIG_EEPROM0
#define PN_APP_IOD_NVM_OFFSET           512
#else
// use flash
#define PN_APP_IOD_NVM_TYPE             NVM_TYPE_FLASH
#define PN_APP_IOD_NVM_INSTANCE         CONFIG_FLASH0
#define PN_APP_IOD_NVM_OFFSET           0x500000
#endif

#define PN_APP_IOD_LED_TASK_PRIO        OSAL_TASK_Prio_2
#define PN_APP_IOD_LED_TASK_STACK       1024

#define PN_APP_IOD_LED_TICKS            10
#define PN_APP_IOD_LED_TASK_INTERVAL_MS (1000)
#define PN_APP_IOD_LED_ON               0x000003FF
#define PN_APP_IOD_LED_BLINK            0x0000001F
#define PN_APP_IOD_LED_OFF              0x00000000

/*!
 * \brief Profinet application non volatile structure
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
typedef struct PN_APP_IOD_Nvdata
{
    uint32_t             checkSum;   /*!< Checksum over all, must be first element */
    uint32_t             version;    /*!< Version of the data */
    uint16_t             ipSuiteLen; /*!< 0xffff: Invalid, else length value */
    PN_API_IOD_IpSuite_t ipSuite;    /*!< Ip suite (ip-addr, subnet mask, default router) */
    PN_API_IOD_Im1Data_t im1[PN_API_IOD_MAX_NUM_OF_SUBSLOTS]; /*!< I&M1  data */
    PN_API_IOD_Im2Data_t im2[PN_API_IOD_MAX_NUM_OF_SUBSLOTS]; /*!< I&M2  data */
    PN_API_IOD_Im3Data_t im3[PN_API_IOD_MAX_NUM_OF_SUBSLOTS]; /*!< I&M3  data */
    PN_API_IOD_Im4Data_t im4[PN_API_IOD_MAX_NUM_OF_SUBSLOTS]; /*!< I&M4  data */

    uint16_t snmpSysNameLen;                           /*!< 0xffff: Invalid, else length value */
    uint8_t  snmpSysName[PN_API_IOD_SNMP_SYSNAME_LEN]; /*!< SNMP System Name */
    uint16_t snmpSysContLen;                           /*!< 0xffff: Invalid, else length value */
    uint8_t  snmpSysCont[PN_API_IOD_SNMP_SYSCONT_LEN]; /*!< SNMP System Contact */
    uint16_t snmpSysLocLen;                            /*!< 0xffff: Invalid, else length value */
    uint8_t  snmpSysLoc[PN_API_IOD_SNMP_SYSLOC_LEN];   /*!< SNMP System Location */
    uint16_t devNameLen;                               /*!< 0xffff: Invalid, else length value */
    uint8_t  devName[PN_API_IOD_DEVICE_NAME_LEN];      /*!< Device Name */
    uint16_t pdevLen;                                  /*!< 0xffff: Invalid, else length value */
    uint8_t  pdev[PN_API_IOD_PDEV_LEN];                /*!< PDEV content */
    uint16_t arfsuLen;                                 /*!< 0xffff: Invalid, else length value */
    uint8_t  arfsu[PN_API_IOD_ARFSU_LEN];              /*!< 0x0000: Empty */
    uint16_t subCfgLen;                                /*!< Size of Submodule config. list */
    PN_API_IOD_RemaDataSubmodCfgInfo_t subCfgList[PN_API_IOD_MAX_NUM_OF_SUBSLOTS]; /*!< List of
                                                                                Submodule Config. */
} PN_APP_IOD_Nvdata_t;

/*!
 * \brief
 * Callback function to set LED states
 *
 * \details
 * In this function, the application sets LED states.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      led                 Led type
 * \param[in]      state               Led state
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 */
uint32_t PN_APP_IOD_cbSetLed(PN_API_IOD_Handle_t *const pnHandle, const PN_API_IOD_Led_t led, const bool state);

/*!
 * \brief
 * Callback function to request the start of LED blinking.
 *
 * \details
 * In this function, the application requests an LED to blink based on the given frequency.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      portNum             Port number (currently not used).
 * \param[in]      frequency           Blinking frequency in Hz.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_startLedBlinkCbf
 */
uint32_t PN_APP_IOD_cbStartLedBlink(
        PN_API_IOD_Handle_t *const pnHandle,
        const uint32_t portNum,
        const uint32_t frequency);

/*!
 * \brief
 * Callback function to request the stop of LED blinking.
 *
 * \details
 * In this function, the application requests an LED to blink based on a zero frequency.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      portNum             Port number (currently not used).
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_stopLedBlinkCbf
 */
uint32_t PN_APP_IOD_cbStopLedBlink(PN_API_IOD_Handle_t *const pnHandle, const uint32_t portNum);

/*!
 * \brief
 * Callback function to request to clear remanent data from NV memory.
 *
 * \details
 * In this function, the application clears some profinet-related data from NV memory.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      destMem             Pointer to a buffer where the data to be cleared.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_freeRemaMemCbf
 */
uint32_t PN_APP_IOD_cbFreeRemaMem(PN_API_IOD_Handle_t *const pnHandle, uint8_t *const destMem);

/*!
 * \brief
 * Callback function to request to store remanent data in NV memory.
 *
 * \details
 * In this function, the application receives the PDev records from Profinet stack and
 * stores them in NV memory.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      type                Type of content to be stored
 * \param[in]      instance            Instance of type
 * \param[in]      memSize             Size of the PDev buffer that needs to be
 *                                     written to NV memory.
 * \param[in]      srcMem              Pointer to the buffer holding PDev data.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK                  Success.
 * \retval         #PN_API_ERR_PARAM           Invalid parameter.
 * \retval         #PN_API_ERR_FLASH_STORE     NV memory write operation failed.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_storeRemaMemCbf
 */
uint32_t PN_APP_IOD_cbStoreRemaMem(PN_API_IOD_Handle_t *const pnHandle,
                                   PN_API_IOD_RemaDataType_t type,
                                   const uint32_t             instance,
                                   const uint32_t             memSize,
                                   uint8_t *const             srcMem);

/*!
 * \brief
 * Callback function to request stored remanent data.
 *
 * \details
 * In this function, the application requests the PDev records from NV memory and
 * sends them back to Profinet stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      type                Type of content to be stored
 * \param[in]      instance            Instance of type
 * \param[out]     destMem             Pointer to the buffer holding PDev data.
 * \param[in]      memSize             Size of the PDev buffer that needs to be
 *                                     restored from NV memory.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_restoreRemaMemCbf
 */
uint32_t PN_APP_IOD_cbRestoreRemaMem(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_RemaDataType_t  type,
    const uint32_t             instance,
    uint8_t **const            destMem,
    uint32_t *const            memSize);

/*!
 * \brief
 * Reset remanent memory to factory defaults
 *
 * \details
 * This function resets the remanent storage to factory defaults
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      rtfOption           Set of parameter which needs to be reseted
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 */
uint32_t PN_APP_IOD_factoryResetRemaMem(PN_API_IOD_Handle_t         *pnHandle,
    const PN_API_IOD_RtfOption_t rtfOption);

/*!
 * \brief
 * Update App Cycle Timer
 *
 * \details
 * This function updates the app cycletimer. The timer is by default configured
 * in sysconfig.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      timeNs              timer interval in nanoseconds
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 */
uint32_t PN_APP_IOD_updateAppCycle(PN_API_IOD_Handle_t *pnHandle, uint32_t timeNs);

/*!
 * \brief
 * Initialize ramNvData instance, load data from persistent memory and initialize
 * the async write thread
 *
 * \return        result of the operation as uint32_t.
 * \retval        #PN_API_OK           Success.
 * \retval        #PN_API_NOT_OK      Something went wrong.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 */
uint32_t PN_APP_IOD_remaInit(void);



/*!
 * \brief
 * Initialize ledContext instance and start
 * led task
 *
 *
 * \return        result of the operation as uint32_t.
 * \retval        #PN_API_OK           Success.
 * \retval        #PN_API_NOT_OK       Something went wrong.
 *
 * \ingroup PN_APP_IOD_BSP_DOXY_GROUP
 *
 */
uint32_t PN_APP_IOD_ledInit(void);

#if(defined __cplusplus)
}
#endif

#endif /* PN_APP_IOD_BSP_H */
