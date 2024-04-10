/*!
 *  \file iolm_port_smi_example.c
 *
 *  \brief
 *  Example application to show how the IO-Link Master Stack can be used via the SMI.
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

#include <stdint.h>
#include "iolm_port_version.h"
#include "iolm_port_smi_example.h"
#include "IOLinkPort/iolm_port_smi.h"
#include "nvram.h"
#include "TinyQueue.h"
#include "iolm_work_task.h"
#define IOLM_NVRAM_FILENAME     "iolm_000"
#define IOLM_NVRAM_FILENAME_LEN 8
#define IOLM_NVRAM_FILENAME_POS 7

/*** private function declarations ***/
static inline char IOLM_EXMPL_digitToCharacter(const uint8_t instance);
static void        IOLM_EXMPL_mkCfgFileName(uint8_t instance, char filename[], uint8_t pos);
static void        IOLM_EXMPL_cbLoadNvCfg(uint8_t instance, uint8_t *pData, uint32_t *pLength);
static void        IOLM_EXMPL_cbSaveNvCfg(uint8_t instance, uint8_t *pData, uint32_t length);
static void        IOLM_EXMPL_cbResetNvCfg(uint8_t instance, uint8_t *pData, uint16_t len);

IOLM_SMI_SCallbacks IOLM_EXMPL_SSmiCallbacks_g = {
    /* Generic channel for SMI over UART */
    .cbLoadMasterIdentification = IOLM_EXMPL_vLoadMasterIdentification,
    .cbChipInfo                 = IOLM_EXMPL_cbChipInfo,
    .cbGenericCnf               = IOLM_SMI_cbGenericCnf,
    .cbStdInd                   = IOLM_SMI_cbStdInd,
    /* non-volatile data storage */
    .cbLoadNvCfg     = IOLM_EXMPL_cbLoadNvCfg,
    .cbSaveNvCfg     = IOLM_EXMPL_cbSaveNvCfg,
    .cbResetDefaults = IOLM_EXMPL_cbResetNvCfg,
    /* Confirmation for port status request */
    .cbPortStatusCnf = IOLM_EXMPL_cbPortStatusCnf,
    /* Event handling(acyclic) */
    .cbPortEventInd   = IOLM_EXMPL_PortEventInd,
    .cbDeviceEventInd = IOLM_EXMPL_DeviceEventInd,
#if (IOLM_EXMPL_ENABLE_STATE_MACHINE == 1)
    /* Master Identification */
    .cbMasterIdentificationCnf = IOLM_EXMPL_cbMasterIdentificationCnf,
    /* Communication establishment */
    .cbPortConfigurationCnf = IOLM_EXMPL_cbPortConfigurationCnf,
    /* Acyclic communication */
    .cbDeviceWriteCnf = IOLM_SMI_cbDeviceWriteCnf,
    .cbDeviceReadCnf  = IOLM_EXMPL_cbDeviceReadCnf,
    /* Cyclic communication */
    .cbPDInCnf  = IOLM_EXMPL_cbPDInCnf,
    .cbPDOutCnf = IOLM_EXMPL_cbPDOutCnf,
#endif
    /* Mainloop Request - called by stack to indicate SMI mainloop run is required */
    .cbMainLoopRequest = IOLM_MAIN_cbMainLoopRequest,
};

IOLM_EXMPL_SPortDataValues_t portArray[IOLM_EXMPL_MAX_PORTS + 1];

/*** private function definitions ***/

/*!
 * \brief Convert single digit 0-9 to corresponding character.
 *
 * \param[in] digit
 *
 * \return character
 *
 */
static inline char IOLM_EXMPL_digitToCharacter(const uint8_t digit)
{
    if (digit < 10)
    {
        return ('0' + digit);
    }
    return '?';
}
/*!
 * \brief Overwrite the given string from pos to pos-2 with
 *        a three digit decimal, e.g. iolm_xxx -> iolm_001.
 *
 * \param[in]     instance Instance (e.g. port) number
 * \param[out]    pFname   File name string
 * \param[in]     pos      String position of last digit (1's)
 *
 * \return void
 *
 */
static void IOLM_EXMPL_mkCfgFileName(uint8_t instance, char filename[], uint8_t pos)
{
    // start last character position
    uint8_t first = 0;
    uint8_t instanceNumber = instance;
    uint8_t initialPos = pos;
    if (initialPos >= 2u)
    {
        first = initialPos - 2u;
    }
    while (initialPos >= first)
    {
        filename[initialPos--] = IOLM_EXMPL_digitToCharacter(instanceNumber % 10);
        instanceNumber /= 10;
    }
}
/*!
 * \brief Load non-volatile data storage
 *
 * \param[in]     instance Instance (e.g. port) number
 * \param[out]    pData    Pointer to data buffer
 * \param[in,out] pLength  Pointer to data length
 *
 * \return void
 *
 */
static void IOLM_EXMPL_cbLoadNvCfg(uint8_t instance, uint8_t *pData, uint32_t *pLength)
{
    char filename[] = IOLM_NVRAM_FILENAME;
    IOLM_EXMPL_mkCfgFileName(instance, filename, IOLM_NVRAM_FILENAME_POS);
    NVR_read(filename, pLength, 0, pData);
}
/*!
 * \brief This must be called to confirm to the stack a successful write to NVRAM.
 *
 * \param[in] pStatus Cast to uint32 as status (0 = OK, ...)
 * \return status
 *
 */
uint32_t IOLM_EXMPL_writeCallback(void *const pStatus)
{
    uint32_t status = *((uint32_t *)pStatus);
    NVR_LOG_DEBUG("status: %i", status);
    if (status == 0)
    {
        IOLM_SMI_vSaveNvFinished();
    }
    return status;
}
/*!
 * \brief Save non-volatile data storage
 *
 * \param[in]     instance Instance (e.g. port) number
 * \param[in]     pData    Pointer to data buffer
 * \param[in,out] pLength  Pointer to data length
 *
 * \return void
 *
 */
static void IOLM_EXMPL_cbSaveNvCfg(uint8_t instance, uint8_t *pData, uint32_t length)
{
    char filename[] = IOLM_NVRAM_FILENAME;
    IOLM_EXMPL_mkCfgFileName(instance, filename, IOLM_NVRAM_FILENAME_POS);
    // Put the write request into the work task queue and fire the callback after it was executed
    IOLM_queueWriteNvram(filename, NVR_MODE_OVERWRITE, length, pData, IOLM_EXMPL_writeCallback);
}
/*!
 * \brief Reset non-volatile data storage
 *
 * \param[in] instance Instance (e.g. port) number
 * \param[in] pData    Not used
 * \param[in] len      Not used
 *
 * \return void
 */
static void IOLM_EXMPL_cbResetNvCfg(uint8_t instance, uint8_t *pData, uint16_t len)
{
    (void)pData;
    (void)len;
    static const uint32_t u32CrcEmpty = 0xFFFFFFFF;
    IOLM_EXMPL_cbSaveNvCfg(instance, (uint8_t *)&u32CrcEmpty, sizeof(u32CrcEmpty));
}

/*** public function definitions ***/

/*!
 * \brief
 * Update LEDs of specific port.
 *
 * This function sends the desired target state for the red and green LED
 * on the IO-Link add-on board to the LED handler.
 * In the handler call it is possible to turn on/off LEDs or set LEDs to a slow or fast blinking
 * state. The current status of the port is retrieved from the global status sctructure portArray[] and
 * therefore needs to be updated to the current status beforehand.
 *
 * \param[in]  portNumber            Number of the target port.
 *
 * \return void
 *
 */
void IOLM_EXMPL_updateLEDs(uint8_t portNumber)
{
    static IOLM_SMI_EPortStatus previousPortStatus[IOLM_EXMPL_MAX_PORTS + 1] = {
        IOLM_SMI_ePortStatus_NOT_AVAILABLE,
    };

    IOLM_SMI_EPortStatus portStatus = portArray[portNumber].currentStackPortStatus;

    if (portStatus != previousPortStatus[portNumber])
    {
        switch (portStatus)
        {
            case IOLM_SMI_ePortStatus_PORT_DIAG:
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Green, IOLM_ELEDState_Off);
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Red, IOLM_ELEDState_Slow);
                break;
            case IOLM_SMI_ePortStatus_NO_DEVICE:
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Green, IOLM_ELEDState_Slow);
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Red, IOLM_ELEDState_Off);
                break;
            case IOLM_SMI_ePortStatus_DEACTIVATED:
            case IOLM_SMI_ePortStatus_NOT_AVAILABLE:
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Green, IOLM_ELEDState_Off);
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Red, IOLM_ELEDState_Off);
                break;
            case IOLM_SMI_ePortStatus_PORT_POWER_OFF:
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Green, IOLM_ELEDState_Off);
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Red, IOLM_ELEDState_Off);
                break;
            case IOLM_SMI_ePortStatus_RESERVED:
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Green, IOLM_ELEDState_Fast);
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Red, IOLM_ELEDState_Off);
                break;
            case IOLM_SMI_ePortStatus_OPERATE:
            case IOLM_SMI_ePortStatus_DI_CQ:
            case IOLM_SMI_ePortStatus_DO_CQ:
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Green, IOLM_ELEDState_On);
                IOLM_LED_setLedColorState(portNumber, IOLM_ELEDColor_Red, IOLM_ELEDState_Off);
                break;
            default:
                break;
        }
        previousPortStatus[portNumber] = portStatus;
    }
}

/*!
 *  \brief
 *  Initialization of example application
 *
 *  \return     void
 *
 */
void IOLM_EXMPL_init(void)
{
    uint8_t portNumber;
    uint8_t dataValueCounter;
    /* SMI Example init */
    /* Initialize state machine status array */
    /* In SMI context portNumber numbers start with 1 -> portNumber 0 will not be used */

    portArray[0].exampleState = IOLM_eExampleState_NotUsed;

    /* Set all active ports to init state */
    for (portNumber = IOLM_EXMPL_SMI_PORTS_NUMBER_START; portNumber <= IOLM_EXMPL_MAX_PORTS;
         portNumber++)
    {
        portArray[portNumber].exampleState = IOLM_eExampleState_Init;
    }

    /* Set all active ports to init portArray */
    for (portNumber = IOLM_EXMPL_SMI_PORTS_NUMBER_START; portNumber <= IOLM_EXMPL_MAX_PORTS;
         portNumber++)
    {
        portArray[portNumber].currentStackPortStatus = IOLM_SMI_ePortStatus_NO_DEVICE;
        for (dataValueCounter = 0; dataValueCounter < PD_INPUT_LENGTH; dataValueCounter++)
        {
            portArray[portNumber].aPDInCnfData[dataValueCounter] = 0;
        }
    }
    /* Initialize external SMI channel */
    IOLM_SMI_portInit();
    /* IO-Link Master stack init Example init */
    IOLM_SMI_vInit(&IOLM_EXMPL_SSmiCallbacks_g);
}

/*!
 *  \brief
 *  Initialization of example main loop
 *
 *  \return     void
 *
 */
void OSAL_FUNC_NORETURN IOLM_EXMPL_mainLoop(void)
{
    uint8_t portNumber;

    IOLM_EXMPL_printf("---------------START EXAMPLE APPLICATION------------------\n");
    IOLM_SMI_vMasterIdentificationReq(IOLM_SMI_CLIENT_APP);
    OSAL_SCHED_sleep(5); /*Wait for answer*/
    IOLM_EXMPL_printf("\n");
    while (1)
    {
        /* get port status periodically for setting the LEDs */
        for (portNumber = IOLM_EXMPL_SMI_PORTS_NUMBER_START; portNumber <= IOLM_EXMPL_MAX_PORTS;
             portNumber++)
        {
            IOLM_SMI_vPortStatusReq(IOLM_SMI_CLIENT_APP, portNumber);
        }
#if (IOLM_EXMPL_ENABLE_STATE_MACHINE == 1)
        IOLM_EXMPL_stateMachine();
#endif
        OSAL_SCHED_sleep(IOLM_EXMPL_DELAY_IN_MILISEC);
    }
}

/*!
 *  \brief
 *  Set port to desired port mode.
 *
 *  \param[in]  portNumber                port number as uint8_t.
 *  \param[in]  targetPortMode            target port mode as IOLM_SMI_EPortMode.
 */
void IOLM_EXMPL_setPortToPortMode(uint8_t portNumber, IOLM_SMI_EPortMode targetPortMode)
{
    IOLM_SMI_SPortConfigList portConfig;

    OSAL_MEMORY_memset(&portConfig, 0, sizeof(portConfig));
    portConfig.u8PortMode    = (INT8U)targetPortMode;
    portConfig.u16ArgBlockID = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_PortConfigList);
    IOLM_SMI_vPortConfigurationReq(
        IOLM_SMI_CLIENT_APP,
        portNumber,
        sizeof(portConfig),
        (INT8U *)&portConfig);
}

/*!
 *  \brief
 *  Initialization of example state machine
 *
 *  \return     void
 *
 */
void IOLM_EXMPL_stateMachine(void)
{
    uint8_t portNumber;

    for (portNumber = IOLM_EXMPL_SMI_PORTS_NUMBER_START;
         portNumber <= IOLM_EXMPL_PORTS_USED_IN_STATE_MACHINE;
         portNumber++)
    {
        /* STATE MACHINE ARCHITECTURE:
            --------------------Establishment of the Communication--------------------
                   Application                                              Stack
                   states:                                                  act as:
             1.)   IOLM_eExampleState_Init
             2.)   IOLM_eExampleState_Config                    ->          Receiver
             3.)   IOLM_eExampleState_ConfigWait                <-          Transmitter
             4.)   IOLM_eExampleState_PortStatusRequest         ->          Receiver
             5.)   IOLM_eExampleState_PortStatusWait            <-          Transmitter
             ----------------------------Communication--------------------------------
             6.)   IOLM_eExampleState_ReadVendorName            ->          Receiver
             7.)   IOLM_eExampleState_ReadVendorNameWait        <-          Transmitter
             8.)   IOLM_eExampleState_ReadProductName           ->          Receiver
             9.)   IOLM_eExampleState_ReadProductNameWait       <-          Transmitter
             10.)  IOLM_eExampleState_ReadSerialnumber          ->          Receiver
             11.)  IOLM_eExampleState_ReadSerialnumberWait      <-          Transmitter
             12.)  IOLM_eExampleState_WriteProcessDataValue     ->          Receiver
             13.)  IOLM_eExampleState_WriteProcessDataValueWait <-          Transmitter
             -------------------  Read process data Communication---------------------
             14.)  IOLM_eExampleState_PortStatusRequestPD       ->          Receiver
             15.)  IOLM_eExampleState_PortStatusWaitPD          <-          Transmitter
             16.)  IOLM_eExampleState_ReadProcessDataValue      ->          Receiver
             17.)  IOLM_eExampleState_ReadProcessDataValueWait  <-          Transmitter
             18.)  IOLM_eExampleState_CheckForOperate           ->          Receiver
             --.)  default:IOLM_eExampleState_PortStatusWait    <-          Transmitter

            Note: The state machine is not strictly executed in this specific sequence,
            see for example IOLM_EXMPL_PortErrorHandler.
        */
        switch (portArray[portNumber].exampleState)
        {
            case IOLM_eExampleState_Init: /*  wait for portArray status response */
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is waiting for port status request.\n",
                    portNumber);
                portArray[portNumber].exampleState = IOLM_eExampleState_Config;
                break;
            case IOLM_eExampleState_Config:
            {
                IOLM_SMI_SPortConfigList portConfig;
                memset(&portConfig, 0, sizeof(portConfig));
                portConfig.u8PortMode    = IOLM_SMI_ePortMode_IOL_AUTOSTART;
                portConfig.u16ArgBlockID = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_PortConfigList);
                IOLM_EXMPL_printf("IO-Link portArray %u: Port is now in \"Config\" mode\n", portNumber);
                IOLM_SMI_vPortConfigurationReq(
                    IOLM_SMI_CLIENT_APP,
                    portNumber,
                    sizeof(portConfig),
                    (INT8U *)&portConfig);
                portArray[portNumber].exampleState = IOLM_eExampleState_ConfigWait;
            }
            break;
            case IOLM_eExampleState_ConfigWait: /* wait for read response */
            {
                IOLM_EXMPL_printf("IO-Link port %u: Port is now in \"ConfigWait\" mode\n", portNumber);
            }
            break;
            case IOLM_eExampleState_PortStatusRequest: /* start check port status */
            {
                IOLM_SMI_vPortStatusReq(IOLM_SMI_CLIENT_APP, portNumber);
                IOLM_EXMPL_printf("IO-Link port %u: Port is now in \"Port request\" mode\n", portNumber);
                portArray[portNumber].exampleState = IOLM_eExampleState_PortStatusWait;
            }
            break;
            case IOLM_eExampleState_PortStatusWait: /*  wait for port status response */
            {
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is now in \"PortStatusWait\" mode\n",
                    portNumber);
            }
            break;
            case IOLM_eExampleState_ReadVendor: /* start reading vendor name */
            {
                /* Allocate ArgBlock Memory */
                uint8_t                  aMem[IOLM_SMI_ARGBLOCK_ONREQ_LEN(0)];
                IOLM_SMI_SOnRequestData *pReq = (IOLM_SMI_SOnRequestData *)aMem;

                /* Fill Request */
                pReq->u16ArgBlockID = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_OnRequestDataRead);
                pReq->u16Index      = IOLM_SMI_ENDIAN_16(IOL_eISDUIndex_VendorName); /* Index 16 =
                                                                                        VendorName     */
                pReq->u8Subindex = 0;

                /* Send Request */
                IOLM_EXMPL_printf("IO-Link port %u: Read Vendor Name\n", portNumber);
                IOLM_SMI_vDeviceReadReq(
                    IOLM_SMI_CLIENT_APP,
                    portNumber,
                    IOLM_SMI_ARGBLOCK_ONREQ_LEN(0),
                    (INT8U *)aMem); /* Data */
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadVendorWait;
            }
            break;

            case IOLM_eExampleState_ReadVendorWait: /* wait for read response */
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is now in \"ReadVendorWait\" mode\n",
                    portNumber);
                break;
            case IOLM_eExampleState_ReadProductName: /* start reading ProductName */
            {
                /* Allocate ArgBlock Memory */
                uint8_t                  aMem[IOLM_SMI_ARGBLOCK_ONREQ_LEN(0)];
                IOLM_SMI_SOnRequestData *pReq = (IOLM_SMI_SOnRequestData *)aMem;

                /* Fill Request */
                pReq->u16ArgBlockID = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_OnRequestDataRead);

                pReq->u16Index = IOLM_SMI_ENDIAN_16(IOL_eISDUIndex_ProductName); /* Index 18 =
                                                                                    ProductName */
                pReq->u8Subindex = 0;

                /* Send Request */
                IOLM_EXMPL_printf("IO-Link port %u: Read Product Name\n", portNumber);
                IOLM_SMI_vDeviceReadReq(
                    IOLM_SMI_CLIENT_APP,
                    portNumber,
                    IOLM_SMI_ARGBLOCK_ONREQ_LEN(0),
                    (INT8U *)aMem); /* Data */
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadProductNameWait;
            }
            break;
            case IOLM_eExampleState_ReadProductNameWait: /* wait for read response */
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is now in \"ReadProductNameWait\" mode\n",
                    portNumber);
                break;
            case IOLM_eExampleState_ReadSerialnumber: /* start reading Serialnumber */
            {
                /* Allocate ArgBlock Memory */
                uint8_t                  aMem[IOLM_SMI_ARGBLOCK_ONREQ_LEN(0)];
                IOLM_SMI_SOnRequestData *pReq = (IOLM_SMI_SOnRequestData *)aMem;

                /* Fill Request */
                pReq->u16ArgBlockID = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_OnRequestDataRead);
                pReq->u16Index      = IOLM_SMI_ENDIAN_16(IOL_eISDUIndex_SerialNumber); /* Index 21 =
                                                                                          Serialnumber */
                pReq->u8Subindex = 0;

                /* Send Request */
                IOLM_EXMPL_printf("IO-Link port %u: Read Serialnumber\n", portNumber);
                IOLM_SMI_vDeviceReadReq(
                    IOLM_SMI_CLIENT_APP,
                    portNumber,
                    IOLM_SMI_ARGBLOCK_ONREQ_LEN(0),
                    (INT8U *)aMem); /* Data */
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadSerialnumberWait;
            }
            break;

            case IOLM_eExampleState_ReadSerialnumberWait: /* wait for read response */
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is now in \"ReadSerialnumberWait\" mode\n",
                    portNumber);
                break;
            /*
            SPECIFIC DEVICE PROCESS DATA WRITE CALL:
            For different devices there a different process data write commands. For example: one
            will turn on a LED, an other will change a specific temperature measurement mode and so
            on, so this following comments are just for inspiration, you need to check your device
            manual first before write process data.
            */
            /*
            case IOLM_eExampleState_WriteProcessDataValue:
            {
                //Allocate ArgBlock Memory
                INT8U aMem[IOLM_SMI_ARGBLOCK_ONREQ_LEN(2)];
                IOLM_SMI_SPDOut* pReq = (IOLM_SMI_SPDOut*)aMem;

                //Fill Request
                pReq->u16ArgBlockID = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_PDOut);
                pReq->u8OE = 0x01;
                pReq->u8OutputDataLength = 0x01;
                pReq->au8Data[0]= 0x02;

                //Send PD Request
                IOLM_SMI_vPDOutReq(IOLM_SMI_CLIENT_APP, portNumber, IOLM_SMI_ARGBLOCK_ONREQ_LEN(2),
            (INT8U*)aMem); IOLM_EXMPL_printf("IO-Link port %u: Port is now in \"Write Process Data\"
            mode\n", portNumber); IOLM_EXMPL_printf("IO-Link port %u: write 0x02\n", portNumber);
                portArray[portNumber].exampleState = IOLM_eExampleState_WriteProcessDataValueWait;
            }
                break;

            case IOLM_eExampleState_WriteProcessDataValueWait:
            {
                IOLM_EXMPL_printf("IO-Link port %u: Port is now in \"Wait Write Process Data\"
            mode\n", portNumber);
            }
                break;
            */
            case IOLM_eExampleState_PortStatusRequestPD: /* start check port status */
                IOLM_SMI_vPortStatusReq(IOLM_SMI_CLIENT_APP, portNumber);
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is now in \"Process Data Exchange \" mode\n",
                    portNumber);
                portArray[portNumber].exampleState = IOLM_eExampleState_PortStatusWaitPD;
                break;
            case IOLM_eExampleState_PortStatusWaitPD: /*  wait for port status response */
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is now in \"Process Data Exchange wait \" mode\n",
                    portNumber);
                break;
            case IOLM_eExampleState_ReadProcessDataValue: /* start reading process data */
            {
                IOLM_SMI_vPDInReq(IOLM_SMI_CLIENT_APP, portNumber);
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is now in \"Port Read Process Data\" mode\n",
                    portNumber);
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadProcessDataValueWait;
            }
            break;
            case IOLM_eExampleState_ReadProcessDataValueWait:
            {
                IOLM_EXMPL_printf(
                    "IO-Link port %u: Port is now in \"Wait Read Process Data\" mode\n",
                    portNumber);
            }
            break;
            default:
                break;
        }
    }
}

/*!
 * \brief
 * Get Master identification confirmation callback.
 *
 * This function is called if a Master identification read request has finished.
 *
 * \param[in]  clientID              Client ID.
 * \param[in]  error                 Error message as #IOL_EErrorType.
 * \param[in]  argBlockLength        Length of ArgBlock.
 * \param[in]  pArgBlock               Data pointer which points to the master identification
 * (#IOLM_SMI_SMasterident).
 *
 * \return void
 *
 */
void IOLM_EXMPL_cbMasterIdentificationCnf(
    uint8_t  clientID,
    uint16_t error,
    uint16_t argBlockLength,
    uint8_t *pArgBlock)
{
    IOLM_SMI_SMasterident *psuMasterIdent = (IOLM_SMI_SMasterident *)pArgBlock;
    psuMasterIdent->u16ArgBlockID         = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_MasterIdent);

    (void)clientID;
    (void)argBlockLength;

    if (error != IOL_eErrorType_NONE)
    {
        IOLM_EXMPL_printf("NO Master Identification Possible.\n");
    }
    else
    {
        IOLM_EXMPL_printf(
            "Master Identification: \nMasterID:  %u \nMax ports: %u \n ",
            IOLM_SMI_ENDIAN_32(psuMasterIdent->u32MasterID),
            psuMasterIdent->u8MaxNumberOfPorts);
    }
}

/*!
 * \brief
 * Port event indication
 *
 * This function is called if a port event indication occurs.
 *
 * \param[in]  portNumber            Port Number.
 * \param[in]  argBlockLength        Length of ArgBlock.
 * \param[in]  pArgBlock             Data pointer which points to the port event data
 * (#IOLM_SMI_SPortEvent).
 *
 * \return void
 *
 */
void IOLM_EXMPL_PortEventInd(uint8_t portNumber, uint16_t argBlockLength, uint8_t *pArgBlock)
{
    IOLM_SMI_SPortEvent *pEvent = (IOLM_SMI_SPortEvent *)pArgBlock;

    (void)argBlockLength;
    (void)pEvent;

    /* Example Output*/
    IOLM_EXMPL_printf(
        "IO-Link port %u: Port event - qualifier: 0x%02x - code: 0x%04x\n",
        portNumber,
        pEvent->u8EventQualifier,
        IOLM_SMI_ENDIAN_16(pEvent->u16EventCode));
}

/*!
 * \brief
 * Device event indication
 *
 * This function is called if a device event indication occurs.
 *
 * \param[in]  portNumber            Port Number.
 * \param[in]  argBlockLength        Length of ArgBlock.
 * \param[in]  pArgBlock             Data pointer which points to the device event data
 * (#IOLM_SMI_SDeviceEvent).
 *
 * \return void
 *
 */
void IOLM_EXMPL_DeviceEventInd(uint8_t portNumber, uint16_t argBlockLength, uint8_t *pArgBlock)
{
    IOLM_SMI_SDeviceEvent *pEvent = (IOLM_SMI_SDeviceEvent *)pArgBlock;

    (void)portNumber;
    (void)argBlockLength;
    (void)pEvent;

    IOLM_SMI_vDeviceEventAck(IOLM_SMI_CLIENT_APP, portNumber);

    /* Example Output */
    IOLM_EXMPL_printf(
        "IO-Link port %u: Device event - qualifier: 0x%02x - code: 0x%04x\n",
        portNumber,
        pEvent->u8EventQualifier,
        IOLM_SMI_ENDIAN_16(pEvent->u16EventCode));
}

/*!
 * \brief
 * Port configuration confirmation callback.
 *
 * This function is called if a device configuration request has finished.
 *
 * \param[in] clientID         Client ID.
 * \param[in] portNumber       Port Number.
 * \param[in] error            Error message as #IOL_EErrorType.
 *
 * \return void
 *
 */
void IOLM_EXMPL_cbPortConfigurationCnf(uint8_t clientID, uint8_t portNumber, uint16_t error)
{
    if (error != IOL_eErrorType_NONE)
    {
        IOLM_EXMPL_printf("IO-Link port %u: Port is now in \"IOL_eErrorType_NONE\" mode\n", portNumber);
        portArray[portNumber].exampleState = IOLM_EXMPL_PortErrorHandler(portNumber, error);
    }
    else
    {
        portArray[portNumber].exampleState = IOLM_eExampleState_ReadVendor;
    }
}

/*!
 * \brief
 * Port status confirmation callback.
 *
 * This function is called if a device port status request has finished.
 *
 * \param[in]  clientID              Client ID.
 * \param[in]  portNumber            Port Number.
 * \param[in]  error                 Error message as #IOL_EErrorType.
 * \param[in]  argBlockLength        Length of ArgBlock.
 * \param[in]  pArgBlock             Data pointer which points to the port status list
 * (#IOLM_SMI_SPortStatusList).
 *
 * \return void
 *
 */
void IOLM_EXMPL_cbPortStatusCnf(
    uint8_t  clientID,
    uint8_t  portNumber,
    uint16_t error,
    uint16_t argBlockLength,
    uint8_t *pArgBlock)
{
    IOLM_SMI_SPortStatusList *psuPortStatus = (IOLM_SMI_SPortStatusList *)pArgBlock;

    (void)argBlockLength;

    portArray[portNumber].currentStackPortStatus = psuPortStatus->u8PortStatusInfo;

    IOLM_EXMPL_updateLEDs(portNumber);

#if (IOLM_EXMPL_ENABLE_STATE_MACHINE == 1)
    if (error != IOL_eErrorType_NONE)
    {
        IOLM_EXMPL_PortErrorHandler(portNumber, error);
    }
    else if (portArray[portNumber].currentStackPortStatus == IOLM_SMI_ePortStatus_OPERATE)
    {
        if (portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusWait)
        {
            portArray[portNumber].exampleState = IOLM_eExampleState_ReadVendor;
        }
        else if (
            portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusRequestPD
            || portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusWaitPD)
        {
            portArray[portNumber].exampleState = IOLM_eExampleState_ReadProcessDataValue;
        }
    }
#endif
}

/*!
 * \brief
 * Device write confirmation callback.
 *
 * This function is called if a device write request has finished.
 *
 * \param[in]  clientID              Client ID.
 * \param[in]  portNumber            Port Number.
 * \param[in]  error                 Error message as #IOL_EErrorType.
 *
 * \return void
 *
 */
void IOLM_SMI_cbDeviceWriteCnf(uint8_t clientID, uint8_t portNumber, uint16_t error)
{
    IOLM_EXMPL_printf("---------------Write Confirmation------------------\n");
    if (error != IOL_eErrorType_NONE)
    {
        portArray[portNumber].exampleState = IOLM_EXMPL_PortErrorHandler(portNumber, error);
    }
    else
    {
        portArray[portNumber].exampleState = IOLM_eExampleState_PortStatusRequestPD;
    }
}

/*!
 * \brief
 * Device read confirmation callback.
 *
 * This function is called if a device read request has finished.
 * This is the answer to an on-request data.
 *
 * \param[in]  clientID              Client ID.
 * \param[in]  portNumber            Port Number.
 * \param[in]  error                 Error message as #IOL_EErrorType.
 * \param[in]  argBlockLength        Length of ArgBlock.
 * \param[in]  pArgBlock             Data pointer which points to the device on-request data
 * (#IOLM_SMI_SOnRequestData).
 *
 * \return void
 *
 */
void IOLM_EXMPL_cbDeviceReadCnf(
    uint8_t  clientID,
    uint8_t  portNumber,
    uint16_t error,
    uint16_t argBlockLength,
    uint8_t *pArgBlock)
{
#if (IOLM_EXMPL_ENABLE_PRINTF == 1)
    IOLM_SMI_SOnRequestData *pReq          = (IOLM_SMI_SOnRequestData *)pArgBlock;
    uint8_t                  payloadLength = argBlockLength - IOLM_SMI_ARGBLOCK_ONREQ_LEN(0);
    uint8_t                  aDeviceData[DEVICE_READ_DATA_LENGTH] = { "" };

    OSAL_MEMORY_memcpy(aDeviceData, pReq->au8Data, payloadLength);
    aDeviceData[payloadLength] = 0;

#endif
    (void)clientID;

    if (error != IOL_eErrorType_NONE)
    {
        portArray[portNumber].exampleState = IOLM_EXMPL_PortErrorHandler(portNumber, error);
    }
    else if (portArray[portNumber].exampleState == IOLM_eExampleState_ReadVendorWait)
    {
        IOLM_EXMPL_printf("IO-Link port %u: Device vendor is \"%s\"\n", portNumber, aDeviceData);
        portArray[portNumber].exampleState = IOLM_eExampleState_ReadProductName;
    }
    else if (portArray[portNumber].exampleState == IOLM_eExampleState_ReadProductNameWait)
    {
        IOLM_EXMPL_printf("IO-Link port %u: Device vendor is \"%s\"\n", portNumber, aDeviceData);
        portArray[portNumber].exampleState = IOLM_eExampleState_ReadSerialnumber;
    }
    else if (portArray[portNumber].exampleState == IOLM_eExampleState_ReadSerialnumberWait)
    {
        IOLM_EXMPL_printf("IO-Link port %u: Serialnumber: \"%s\"\n", portNumber, aDeviceData);
        portArray[portNumber].exampleState = IOLM_eExampleState_PortStatusRequestPD;
    }
}

/*!
 * \brief
 *  Set output data confirmation callback.
 *
 * This function is called if a process data write request from the master to the device has finished.
 *
 * \param[in]  clientID              Client ID.
 * \param[in]  portNumber            Port Number.
 * \param[in]  error                 Error message as #IOL_EErrorType.
 *
 * \return void
 *
 */
void IOLM_EXMPL_cbPDOutCnf(uint8_t clientID, uint8_t portNumber, uint16_t error)
{
    (void)clientID;

    if (error != IOL_eErrorType_NONE)
    {
        portArray[portNumber].exampleState = IOLM_EXMPL_PortErrorHandler(portNumber, error);
    }
    else
    {
        IOLM_EXMPL_printf("---------------Write Process Data Success-----------------\n");
        portArray[portNumber].exampleState = IOLM_eExampleState_ReadProcessDataValue;
    }
}

/*!
 * \brief
 * Get input data request and confirmation.
 *
 * This function is called if a process data read request from the master to the device has finished.
 *
 * \param[in]  clientID              Client ID.
 * \param[in]  portNumber            Port Number.
 * \param[in]  error                 Error message as #IOL_EErrorType.
 * \param[in]  argBlockLength        Length of ArgBlock.
 * \param[in]  pArgBlock             Data pointer which points to the PDIn data (#IOLM_SMI_SPDIn).
 *
 * \return void
 *
 */
void IOLM_EXMPL_cbPDInCnf(
    uint8_t  clientID,
    uint8_t  portNumber,
    uint16_t error,
    uint16_t argBlockLength,
    uint8_t *pArgBlock)
{
    IOLM_SMI_SPDIn *psuPDIn                         = (IOLM_SMI_SPDIn *)pArgBlock;
    uint8_t         au8CurrentData[PD_INPUT_LENGTH] = {
        0,
    };
    uint8_t memoryCompareResult = 0;
    uint8_t dataElement;

    (void)clientID;
    (void)argBlockLength;

    if (error != IOL_eErrorType_NONE)
    {
        portArray[portNumber].exampleState = IOLM_EXMPL_PortErrorHandler(portNumber, error);
    }
    else
    {
        IOLM_EXMPL_printf("---------------Read Process Data Success-------------------\n");
        OSAL_MEMORY_memcpy(au8CurrentData, psuPDIn->au8Data, psuPDIn->u8InputDataLength);
        memoryCompareResult
            = OSAL_MEMORY_memcmp(au8CurrentData, portArray[portNumber].aPDInCnfData, sizeof(au8CurrentData));
        if (memoryCompareResult != 0)
        {
            OSAL_MEMORY_memcpy(portArray[portNumber].aPDInCnfData, au8CurrentData, sizeof(au8CurrentData));
            for (dataElement = 0; dataElement <= psuPDIn->u8InputDataLength; dataElement++)
            {
                if (au8CurrentData[dataElement] != 0)
                {
                    IOLM_EXMPL_printf(
                        "IO-Link port %u: Process data: 0x%02x \n",
                        portNumber,
                        au8CurrentData[dataElement]);
                }
            }
        }
        portArray[portNumber].exampleState = IOLM_eExampleState_PortStatusRequestPD;
    }
}

/**
\fn IOLM_EXMPL_CBLoadMasterIdentification
\brief Load Master configuration callback

This callback service is called by the stack and requests the Master configuration from the
application. Since this is hardware specific, it has to be implemented in the application code.

\param[in]  u16ArgBlockLength_p     Length of the ArgBlock.
\param[in]  pu8ArgBlock_p           Data pointer which points to the Master identification
(#IOLM_SMI_SMasterident).

\par Example
\code{.c}
void IOLM_SMI_vLoadMasterIdentification(INT16U *u16ArgBlockLength_p, INT8U *pu8ArgBlock_p)
{
    IOLM_SMI_SMasterident *psuMasterIdent = (IOLM_SMI_SMasterident *)pu8ArgBlock_p;

    psuMasterIdent->u16ArgBlockID = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_MasterIdent);
    ...

    // ToDo: Set up psuMasterIdent parameters
}
\endcode

\see IOLM_SMI_SCallbacks, IOLM_SMI_vInit, IOLM_SMI_EArgBlockID

\ingroup grp_smi_general

*/
void IOLM_EXMPL_vLoadMasterIdentification(uint16_t *u16ArgBlockLength_p, uint8_t *pu8ArgBlock_p)
{
    IOLM_SMI_SMasterident *psuMasterIdent = (IOLM_SMI_SMasterident *)pu8ArgBlock_p;
    psuMasterIdent->u16ArgBlockID         = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_MasterIdent);
    psuMasterIdent->u8MaxNumberOfPorts    = IOLM_EXMPL_MAX_PORTS;

    // set portType for all ports
    for (INT8U i = 0; i < IOLM_EXMPL_MAX_PORTS; i++)
    {
        pu8ArgBlock_p[sizeof(IOLM_SMI_SMasterident) + i] = IOLM_SMI_ePortTypes_ClassAWithPowerOffOn;
    }

    // write new ArgBlockLength
    *u16ArgBlockLength_p = (sizeof(IOLM_SMI_SMasterident) + IOLM_EXMPL_MAX_PORTS);
}

/*!
 * \brief
 * Get the versioning from IOL8M-Sitara-Example-Project.
 *
 * This function is used to get information about the version of the master.
 *
 * \param[in]  u8Instance_p              Client ID.
 * \param[in]  pu8Data_p                 Data(unused).
 * \param[in]  u16Length_p               Data length(unused).
 *
 * \return     void
 */
void IOLM_EXMPL_cbChipInfo(INT8U u8Instance_p, INT8U *pu8Data_p, INT16U u16Length_p)
{
    if (u8Instance_p == 0x00)
    {
        IOLM_SMI_SStackInformation suInfo;
        suInfo.u16ArgBlockID = IOLM_SMI_ENDIAN_16(IOLM_SMI_eArgBlockID_StackInformation);
        suInfo.u8Version0    = IOL8M_SITARA_VERSION_VERSION_MAJOR;
        suInfo.u8Version1    = IOL8M_SITARA_VERSION_VERSION_MINOR;
        suInfo.u8Version2    = IOL8M_SITARA_VERSION_VERSION_PATCH;
        suInfo.u8Version3    = 0;
        suInfo.u16TagVersion = IOL8M_SITARA_VERSION_COMMIT_U16;
        suInfo.u16HWRev      = 0;
        suInfo.u32Reserved   = 0;
        IOLM_SMI_vExtRsp(IOL_eErrorType_NONE, (INT8U *)&suInfo, sizeof(suInfo));
    }
    else
    {
        IOLM_SMI_vExtRsp(IOL_eErrorType_IDX_NOTAVAIL, NULL, 0);
    }
}

/*!
 * \brief
 * Call port error handler.
 *
 * This function is called from the callback, if an error occurs.
 *
 * \param[in]  portNumber            Port Number.
 * \param[in]  error                 Error message as #IOL_EErrorType.
 *
 * \return  portState_g[portNumber] as uint8_t.
 *
 */
IOLM_EXMPL_EExampleState_t IOLM_EXMPL_PortErrorHandler(uint8_t portNumber, uint16_t error)
{
    IOLM_EXMPL_printf("IO-Link port %u: Read failed with error 0x%04x", portNumber, error);
    switch (error)
    {
        case IOL_eErrorType_ARGBLOCK_NOT_SUPPORTED:
            IOLM_EXMPL_printf(" - \"ARGBLOCK_NOT_SUPPORTED\"");
            if (portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusWait)
            {
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadVendor;
            }
            else if (
                portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusRequestPD
                || portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusWaitPD)
            {
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadProcessDataValue;
            }
            else
            {
                portArray[portNumber].exampleState = IOLM_eExampleState_Init;
            }
            break;
        case IOL_eErrorType_DEVICE_NOT_IN_OPERATE:
            IOLM_EXMPL_printf(" - \"DEVICE_NOT_IN_OPERATE\"");
            if (portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusWait)
            {
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadVendor;
            }
            else if (
                portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusRequestPD
                || portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusWaitPD)
            {
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadProcessDataValue;
            }
            else
            {
                portArray[portNumber].exampleState = IOLM_eExampleState_Init;
            }
            break;
        case IOL_eErrorType_SERVICE_NOT_SUPPORTED:
            IOLM_EXMPL_printf(" - \"SERVICE_NOT_SUPPORTED\"");
            if (portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusWait)
            {
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadVendor;
            }
            else if (
                portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusRequestPD
                || portArray[portNumber].exampleState == IOLM_eExampleState_PortStatusWaitPD)
            {
                portArray[portNumber].exampleState = IOLM_eExampleState_ReadProcessDataValue;
            }
            break;
        case IOL_eErrorType_DEVICE_NOT_ACCESSIBLE:
            IOLM_EXMPL_printf(" - \"DEVICE_NOT_ACCESSIBLE\"");
            portArray[portNumber].exampleState = IOLM_eExampleState_PortStatusWait;
            break;
        case IOL_eErrorType_PORT_NUM_INVALID:
            IOLM_EXMPL_printf(" - \"PORT_NUM_INVALID\"");
            portArray[portNumber].exampleState = IOLM_eExampleState_PortStatusWait;
            break;
        case IOL_eErrorType_SERVICE_TEMP_UNAVAILABLE:
            IOLM_EXMPL_printf(" - \"SERVICE_TEMP_UNAVAILABLE\"");
            portArray[portNumber].exampleState = IOLM_eExampleState_PortStatusWait;
            break;
        case IOL_eErrorType_ARGBLOCK_INCONSISTENT:
            IOLM_EXMPL_printf(" - \"PORT_CONFIG_INCONSISTENT\"");
            portArray[portNumber].exampleState = IOLM_eExampleState_Init;
            break;
        default:
            portArray[portNumber].exampleState = IOLM_eExampleState_Init;
            break;
    }
    IOLM_EXMPL_printf("\n");
    return portArray[portNumber].exampleState;
}
