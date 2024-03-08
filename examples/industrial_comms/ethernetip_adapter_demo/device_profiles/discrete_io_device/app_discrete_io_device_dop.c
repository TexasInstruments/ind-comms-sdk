/*!
 *  \file app_discrete_io_device_dop.c
 *
 *  \brief
 *  EtherNet/IP&trade; Discrete Output Point Object.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br><br>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "EI_API.h"
#include "EI_API_def.h"

#include "appNV.h"

#include <osal.h>
#include <osal_error.h>
#include <hwal.h>
#include <board.h>

#include <CMN_board.h>
#include <CMN_os.h>
#include <CMN_app.h>
#include <CMN_mem.h>
#include <CMN_CPU_api.h>

#include <drivers/CUST_drivers.h>

#include "appWebServer.h"

#include "app.h"
#include "appTask.h"
#include "appCipCodes.h"
#include "appCfg.h"
#include "device_profiles/app_device_profile.h"
#include "device_profiles/app_device_profile_intern.h"
#include "device_profiles/discrete_io_device/app_discrete_io_device_cfg.h"
#include "device_profiles/discrete_io_device/app_discrete_io_device.h"
#include "device_profiles/discrete_io_device/app_discrete_io_device_dop.h"

#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#include <device_profiles/discrete_io_device/app_discrete_io_device_sm.h>

extern PRUICSS_Handle prusshandle;

void EI_APP_DOP_init (EI_API_CIP_NODE_T* pCipNode);
void EI_APP_DOP_run  (EI_API_CIP_NODE_T* pCipNode);

void EI_APP_DOP_SM_RUN_entryAction(void);
void EI_APP_DOP_SM_RUN_doAction(void);
void EI_APP_DOP_SM_RUN_exitAction(void);

void EI_APP_DOP_SM_RECOVERABLEFAULT_entryAction(void);
void EI_APP_DOP_SM_RECOVERABLEFAULT_doAction(void);
void EI_APP_DOP_SM_RECOVERABLEFAULT_exitAction(void);

void EI_APP_DOP_SM_UNRECOVERABLEFAULT_entryAction(void);
void EI_APP_DOP_SM_UNRECOVERABLEFAULT_doAction(void);
void EI_APP_DOP_SM_UNRECOVERABLEFAULT_exitAction(void);

void EI_APP_DOP_SM_READY_entryAction(void);
void EI_APP_DOP_SM_READY_doAction(void);
void EI_APP_DOP_SM_READY_exitAction(void);

void EI_APP_DOP_SM_IDLE_entryAction(void);
void EI_APP_DOP_SM_IDLE_doAction(void);
void EI_APP_DOP_SM_IDLE_exitAction(void);

void EI_APP_DOP_SM_AVAILABLE_entryAction(void);
void EI_APP_DOP_SM_AVAILABLE_doAction(void);
void EI_APP_DOP_SM_AVAILABLE_exitAction(void);

void EI_APP_DOP_SM_NONEXISTENT_entryAction(void);
void EI_APP_DOP_SM_NONEXISTENT_doAction(void);
void EI_APP_DOP_SM_NONEXISTENT_exitAction(void);

static uint32_t EI_APP_DIO_DEVICE_connectionState_s;
static uint32_t EI_APP_DIO_DEVICE_receiveData_s;

static EI_APP_DOP_ClassData_t dopClassData_s = {.revision = EI_APP_DIO_DEVICE_DOP_REVISION_NUMBER
                                               };

static uint32_t EI_APP_DOP_ledStatus_s   = 0;

/* An event that is internally generated and product-specific. The network does not know if a Fault has occurred. */
static uint8_t EI_APP_DOP_receiveFault       = 0;
/* If the fault is solved. */
static uint8_t EI_APP_DOP_faultCleared       = 0;
/* The setting of the Run_Idle Command attribute to the value 0. */
static uint8_t EI_APP_DOP_receiveIdle        = 0;
/* The setting of the Run_Idle Command attribute to the value 1.*/
static uint8_t EI_APP_DOP_receiveReadyToRun  = 1;
/* Fault is not recoverable. */
static uint8_t EI_APP_DOP_unrecoverableFault = 0;

typedef EI_APP_DIO_DEVICE_SM_StateMachine_t EI_APP_DOP_stateMachine_t;
static  EI_APP_DOP_stateMachine_t dopStateMachine_s = { 0 };

static const EI_APP_DIO_DEVICE_SM_State_t EI_APP_DOP_SM_StateFuncTable_s[EI_APP_DOP_SM_MAX_STATES] = {
    {
        .stateId     = EI_APP_DOP_SM_NONEXISTENT,
        .entryAction = EI_APP_DOP_SM_NONEXISTENT_entryAction,
        .doAction    = EI_APP_DOP_SM_NONEXISTENT_doAction,
        .exitAction  = EI_APP_DOP_SM_NONEXISTENT_exitAction
    },

    {
        .stateId     = EI_APP_DOP_SM_AVAILABLE,
        .entryAction = EI_APP_DOP_SM_AVAILABLE_entryAction,
        .doAction    = EI_APP_DOP_SM_AVAILABLE_doAction,
        .exitAction  = EI_APP_DOP_SM_AVAILABLE_exitAction
    },

    {
        .stateId     = EI_APP_DOP_SM_IDLE,
        .entryAction = EI_APP_DOP_SM_IDLE_entryAction,
        .doAction    = EI_APP_DOP_SM_IDLE_doAction,
        .exitAction  = EI_APP_DOP_SM_IDLE_exitAction
    },

    {
        .stateId     = EI_APP_DOP_SM_READY,
        .entryAction = EI_APP_DOP_SM_READY_entryAction,
        .doAction    = EI_APP_DOP_SM_READY_doAction,
        .exitAction  = EI_APP_DOP_SM_READY_exitAction
    },

    {
        .stateId     = EI_APP_DOP_SM_RUN,
        .entryAction = EI_APP_DOP_SM_RUN_entryAction,
        .doAction    = EI_APP_DOP_SM_RUN_doAction,
        .exitAction  = EI_APP_DOP_SM_RUN_exitAction
    },

    {
        .stateId     = EI_APP_DOP_SM_RECOVERABLEFAULT,
        .entryAction = EI_APP_DOP_SM_RECOVERABLEFAULT_entryAction,
        .doAction    = EI_APP_DOP_SM_RECOVERABLEFAULT_doAction,
        .exitAction  = EI_APP_DOP_SM_RECOVERABLEFAULT_exitAction
    },

    {
        .stateId     = EI_APP_DOP_SM_UNRECOVERABLEFAULT,
        .entryAction = EI_APP_DOP_SM_UNRECOVERABLEFAULT_entryAction,
        .doAction    = EI_APP_DOP_SM_UNRECOVERABLEFAULT_doAction,
        .exitAction  = EI_APP_DOP_SM_UNRECOVERABLEFAULT_exitAction
    },
};

/*!
 *  \brief
 *  EI_APP_DIP_SM_getStateFuncs return the state functions according to
 *  state
 *
 *  \details
 *  EI_APP_DOP_SM_getStateFuncs return the state functions according to
 *  state
 *
 *
 *  \param[in]  state                           state to get related state functions
 *
 *  \return     #SM_State_t*                    pointer of state functions
 *
 *  \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
static const EI_APP_DIO_DEVICE_SM_State_t *EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SmStates_t state)
{
    const EI_APP_DIO_DEVICE_SM_State_t *pStateFunc = NULL;

    if((state >= EI_APP_DOP_SM_NONEXISTENT) && (state < EI_APP_DOP_SM_MAX_STATES))
    {
        pStateFunc = &EI_APP_DOP_SM_StateFuncTable_s[state];
    }

    return pStateFunc;
}

void EI_APP_DOP_SM_NONEXISTENT_doAction(void)
{
    const EI_APP_DIO_DEVICE_SM_State_t *nextState = NULL;

    nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_AVAILABLE);

    if(nextState != NULL)
    {
        EI_APP_DIO_DEVICE_SM_nextState(&dopStateMachine_s, nextState);
    }
}

void EI_APP_DOP_SM_NONEXISTENT_entryAction(void)
{
    EI_API_CIP_NODE_T* pCipNode = NULL;
    pCipNode = EI_API_CIP_NODE_new();

    EI_APP_DOP_init(pCipNode);

    OSAL_printf("Power Up & LED Off\n\r");
}

void EI_APP_DOP_SM_NONEXISTENT_exitAction(void)
{
    // Do nothing.
}

void EI_APP_DOP_SM_AVAILABLE_doAction(void)
{
    const EI_APP_DIO_DEVICE_SM_State_t *nextState = NULL;
    uint8_t unrecoverableFault  = 1;

    EI_APP_DIO_DEVICE_getConnectionInfo(&EI_APP_DIO_DEVICE_connectionState_s, &EI_APP_DIO_DEVICE_receiveData_s);

    if(unrecoverableFault == EI_APP_DOP_unrecoverableFault)
    {
        nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else
    {
        if(EI_APP_DIO_DEVICE_ConnectionEstablished == EI_APP_DIO_DEVICE_connectionState_s)
        {
            nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_READY);
        }
    }

    if(nextState != NULL)
    {
        EI_APP_DIO_DEVICE_SM_nextState(&dopStateMachine_s, nextState);
    }
}
void EI_APP_DOP_SM_AVAILABLE_entryAction(void)
{
    // Connection deleted (from any state)
    OSAL_printf("Outputs Off & LED Off\n\r");
}

void EI_APP_DOP_SM_AVAILABLE_exitAction(void)
{
    // Do nothing.
}

void EI_APP_DOP_SM_IDLE_doAction(void)
{

    const EI_APP_DIO_DEVICE_SM_State_t *nextState = NULL;
    EI_API_CIP_NODE_T* pCipNode = NULL;
    uint8_t faultReceived       = 1;
    uint8_t unrecoverableFault  = 1;
    uint8_t command             = 0;
    uint8_t instanceId          = 0x01;
    // uint8_t value           = 0;

    pCipNode = EI_API_CIP_NODE_new();

    EI_APP_DIO_DEVICE_getConnectionInfo(&EI_APP_DIO_DEVICE_connectionState_s, &EI_APP_DIO_DEVICE_receiveData_s);

    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06, &command);

    if(unrecoverableFault == EI_APP_DOP_unrecoverableFault)
    {
        nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else
    {
        if(EI_APP_DIO_DEVICE_ConnectionTimeOut == EI_APP_DIO_DEVICE_connectionState_s)
        {
            // If the DOP
            // enters the Recoverable_Fault state from the Idle state in response to the I/O connection
            // transitioning to Timed Out, the DOP’s value should go unchanged.
            nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_RECOVERABLEFAULT);
        }

        if(faultReceived == EI_APP_DOP_receiveFault)
        {
            nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_RECOVERABLEFAULT);
        }
        else
        {
            if (command == EI_APP_DOP_receiveReadyToRun)
            {
                nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_READY);
            }

            if(EI_APP_DIO_DEVICE_ConnectionClosed == EI_APP_DIO_DEVICE_connectionState_s)
            {
                nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_AVAILABLE);
            }
      }
    }

    if(nextState != NULL)
    {
        EI_APP_DIO_DEVICE_SM_nextState(&dopStateMachine_s, nextState);
    }
}
void EI_APP_DOP_SM_IDLE_entryAction(void)
{
    EI_API_CIP_NODE_T* pCipNode = NULL;
    pCipNode = EI_API_CIP_NODE_new();

    uint8_t idleValue = 0;
    uint8_t idleAction = 0;
    uint8_t instanceId = 0x01;
    uint32_t errCode = EI_API_CIP_eERR_GENERAL;

    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_09, &idleAction);

    if(0 == idleAction)
    {
        errCode = EI_API_CIP_getAttr_bool(
                                         pCipNode,
                                         EI_APP_DIO_DEVICE_DOG_CLASS_ID,
                                         instanceId,
                                         EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_10,
                                         &idleValue);

        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        }

        for (uint16_t i = 1; i <= EI_APP_DIO_DEVICE_DOP_NUM_OF_INST; i++)
        {
            errCode = EI_API_CIP_setAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, i, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, idleValue);

            if (errCode != EI_API_CIP_eERR_OK)
            {
                OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            }
        }
    }

    OSAL_printf("Outputs Idle & LED Flash Green\n\r");
}

void EI_APP_DOP_SM_IDLE_exitAction(void)
{
    // Do nothing
}

void EI_APP_DOP_SM_READY_doAction(void)
{
    const EI_APP_DIO_DEVICE_SM_State_t *nextState = NULL;
    EI_API_CIP_NODE_T* pCipNode = NULL;
    uint8_t dataReceived        = 1;
    uint8_t faultReceived       = 1;
    uint8_t unrecoverableFault  = 1;
    uint8_t command             = 0;
    uint8_t instanceId          = 0x01;

    pCipNode = EI_API_CIP_NODE_new();

    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06, &command);

    EI_APP_DIO_DEVICE_getConnectionInfo(&EI_APP_DIO_DEVICE_connectionState_s, &EI_APP_DIO_DEVICE_receiveData_s);

    if(unrecoverableFault == EI_APP_DOP_unrecoverableFault)
    {
        nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else
    {
        if(EI_APP_DIO_DEVICE_ConnectionTimeOut == EI_APP_DIO_DEVICE_connectionState_s
                        || faultReceived == EI_APP_DOP_receiveFault)
        {
            nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_RECOVERABLEFAULT);
        }
        else
        {
            if (command == EI_APP_DOP_receiveIdle)
            {
                nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_IDLE);
            }
            if (dataReceived == EI_APP_DIO_DEVICE_receiveData_s && command == EI_APP_DOP_receiveReadyToRun)
            {
                nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_RUN);
            }

            if(EI_APP_DIO_DEVICE_ConnectionClosed == EI_APP_DIO_DEVICE_connectionState_s)
            {
                nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_AVAILABLE);
            }
        }
    }

    if(nextState != NULL)
    {
        EI_APP_DIO_DEVICE_SM_nextState(&dopStateMachine_s, nextState);
    }
}
void EI_APP_DOP_SM_READY_entryAction(void)
{
    // Connection transitions to established
    // OSAL_printf("Outputs Unchanged & LEDs Unchanged\n\r");
    // OSAL_printf("LEDs Flash Green\n\r");
    OSAL_printf("Ready\n\r");
}

void EI_APP_DOP_SM_READY_exitAction(void)
{
    // Do nothing.
}

void EI_APP_DOP_SM_RUN_doAction(void)
{
    const EI_APP_DIO_DEVICE_SM_State_t *nextState = NULL;
    EI_API_CIP_NODE_T* pCipNode = NULL;
    uint8_t faultReceived       = 1;
    uint8_t unrecoverableFault  = 1;
    uint8_t command             = 0;
    uint8_t instanceId          = 0x01;

    pCipNode = EI_API_CIP_NODE_new();

    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06, &command);

    EI_APP_DIO_DEVICE_getConnectionInfo(&EI_APP_DIO_DEVICE_connectionState_s, &EI_APP_DIO_DEVICE_receiveData_s);

    if(unrecoverableFault == EI_APP_DOP_unrecoverableFault)
    {
        nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else
    {
        if (EI_APP_DIO_DEVICE_ConnectionTimeOut == EI_APP_DIO_DEVICE_connectionState_s
                        || faultReceived == EI_APP_DOP_receiveFault)
        {
            nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_RECOVERABLEFAULT);
        }
        else
        {
            // - the IO connection object receives an I/O message containing no application data
            if (command == EI_APP_DOP_receiveIdle)
            {
                nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_IDLE);
            }

            EI_APP_DOP_run(pCipNode);

            if(EI_APP_DIO_DEVICE_ConnectionClosed == EI_APP_DIO_DEVICE_connectionState_s)
            {
                nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_AVAILABLE);
            }
            /*
            // An event that signals the reception of I/O data and causes the object to transition to the Run state
            if (EI_APP_DIO_DEVICE_DATA_RECEIVED == EI_APP_DIO_DEVICE_receiveData_s)
            {
                nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_RUN);
            }
            */
        }
    }

    if(nextState != NULL)
    {
        EI_APP_DIO_DEVICE_SM_nextState(&dopStateMachine_s, nextState);
    }
}
void EI_APP_DOP_SM_RUN_entryAction(void)
{
    OSAL_printf("Outputs Active & LED Solid Green\n\r");
}

void EI_APP_DOP_SM_RUN_exitAction(void)
{
    // Do nothing.
}

void EI_APP_DOP_SM_RECOVERABLEFAULT_doAction(void)
{
    const EI_APP_DIO_DEVICE_SM_State_t *nextState = NULL;
    uint8_t unrecoverableFault  = 1;
    uint8_t faultCleared        = 1;

    EI_APP_DIO_DEVICE_getConnectionInfo(&EI_APP_DIO_DEVICE_connectionState_s, &EI_APP_DIO_DEVICE_receiveData_s);

    if(unrecoverableFault == EI_APP_DOP_unrecoverableFault)
    {
        nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_UNRECOVERABLEFAULT);
    }
    else
    {
        if (faultCleared == EI_APP_DOP_faultCleared
                    || EI_APP_DIO_DEVICE_ConnectionEstablished == EI_APP_DIO_DEVICE_connectionState_s)
        {
            nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_READY);
        }

        if(EI_APP_DIO_DEVICE_ConnectionClosed == EI_APP_DIO_DEVICE_connectionState_s)
        {
            nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_AVAILABLE);
        }
    }

    if(nextState != NULL)
    {
        EI_APP_DIO_DEVICE_SM_nextState(&dopStateMachine_s, nextState);
    }
}
void EI_APP_DOP_SM_RECOVERABLEFAULT_entryAction(void)
{

    EI_API_CIP_NODE_T* pCipNode = NULL;
    uint32_t errCode    = EI_API_CIP_eERR_GENERAL;
    uint8_t instanceId  = 0x01;
    uint8_t faultValue  = 0;
    uint8_t faultAction = 0;

    pCipNode = EI_API_CIP_NODE_new();

    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_07, &faultAction);

    if(0 == faultAction)
    {
        errCode = EI_API_CIP_getAttr_bool(
                                         pCipNode,
                                         EI_APP_DIO_DEVICE_DOG_CLASS_ID,
                                         instanceId,
                                         EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_08,
                                         &faultValue);

        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        }

        for (uint16_t i = 1; i <= EI_APP_DIO_DEVICE_DOP_NUM_OF_INST; i++)
        {
            errCode = EI_API_CIP_setAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, i, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, faultValue);

            if (errCode != EI_API_CIP_eERR_OK)
            {
                OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            }
        }
    }

    OSAL_printf("Recoverable Fault\n\r");
//    OSAL_printf("Outputs Fault & LED Flash Red\n\r");
}

void EI_APP_DOP_SM_RECOVERABLEFAULT_exitAction(void)
{
    // Do nothing.
}

void EI_APP_DOP_SM_UNRECOVERABLEFAULT_doAction(void)
{
    // Do nothing.
}
void EI_APP_DOP_SM_UNRECOVERABLEFAULT_entryAction(void)
{
    OSAL_printf("Outputs Fault & LED Solid Red\n\r");
}

void EI_APP_DOP_SM_UNRECOVERABLEFAULT_exitAction(void)
{
    // Do nothing.
}

/*!
 *  \brief
 *  EI_APP_DOP_SM_run executes the states in a endless loop
 *
 *  \details
 *  EI_APP_DOP_SM_run executes the states in a endless loop
 *
 *
 *  \return     void
 *
 *  \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
void EI_APP_DOP_SM_run(void)
{
    if(dopStateMachine_s.state.doAction != NULL)
    {
        dopStateMachine_s.state.doAction();
    }
}

/*!
 *  \brief
 *  EI_APP_DOP_SM_init initializes the DOP state machines
 *
 *  \details
 *  EI_APP_DOP_SM_init initializes the DOP state machines
 *
 *
 *  \return     void
 *
 *  \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
void EI_APP_DOP_SM_init(void)
{
    const EI_APP_DIO_DEVICE_SM_State_t *nextState = EI_APP_DOP_SM_getStateFuncs(EI_APP_DOP_SM_NONEXISTENT);
    OSAL_MEMORY_memset(&dopStateMachine_s, 0, sizeof(EI_APP_DOP_stateMachine_t));

    if (nextState != NULL)
    {
        EI_APP_DIO_DEVICE_SM_nextState(&dopStateMachine_s, nextState);
    }
}

/*!
 * \brief
 * Add an attribute for the class instance.
 *
 * \details
 * This function adds an attribute for the class instance.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  attrId                                      Attribute identifier.
 * \param[in]  pvValue                                     Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 * \retval     #EI_API_CIP_eERR_CLASS_BLACKLIST            Operation on the CIP object with specified identifier (class ID) is not allowed.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID          Attribute is invalid, possibly NULL pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_ALREADY_EXIST    Attribute already exists in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_NOT_IMPLEMENTED  Attribute function not implemented.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_CDT_SIZE         Passed constructed data type size for e.g. farray not within valid range.
 * \retval     #EI_API_CIP_eERR_MEMALLOC                   Memory allocation error.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_EDT_SIZE         Passed elementary data type size for e.g. short string not within valid range.
 * \retval     #EI_API_CIP_eERR_INSTANCE_UNKNOWN_STATE     Unknown state during class operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint16_t revision = 0x0002; // Create one of the class level attribute
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * errCode = EI_APP_DOP_addClassAttribute(pEI_API_CIP_NODE, 0x0001, &revision);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
static uint32_t EI_APP_DOP_addClassAttribute(EI_API_CIP_NODE_T* pCipNode, uint16_t attrId, void* pvValue)
{
    uint32_t errCode;
    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));

    attr.id         = attrId;
    attr.edt        = EI_API_CIP_eEDT_UINT;
    attr.accessRule = EI_API_CIP_eAR_GET;
    attr.pvValue    = pvValue;

    errCode = EI_API_CIP_addClassAttr(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, &attr);
    if(errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    errCode = EI_API_CIP_setClassAttr(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, &attr);
    if(errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

laError:
    return errCode;
}

/*!
 *
 * \brief
 * Add an attribute for the instance
 *
 * \details
 * This function adds an attribute with the data type, the access rule, callback
 * functions for set and get attribute service and the initial value.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  attrId                                      Attribute identifier.
 * \param[in]  edt                                         Data type. (as enum)
 * \param[in]  accessRule                                  Available attribute access rules. (as enum)
 * \param[in]  getCb                                       Function prototype for CIP get attribute callback function.
 * \param[in]  setCb                                       Function prototype for CIP set attribute callback function.
 * \param[in]  len                                         Data type length.
 * \param[in]  pvValue                                     Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 * \retval     #EI_API_CIP_eERR_CLASS_BLACKLIST            Operation on the CIP object with specified identifier (class ID) is not allowed.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID          Attribute is invalid, possibly NULL pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_ALREADY_EXIST    Attribute already exists in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_NOT_IMPLEMENTED  Attribute function not implemented.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_CDT_SIZE         Passed constructed data type size for e.g. farray not within valid range.
 * \retval     #EI_API_CIP_eERR_MEMALLOC                   Memory allocation error.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_EDT_SIZE         Passed elementary data type size for e.g. short string not within valid range.
 * \retval     #EI_API_CIP_eERR_INSTANCE_UNKNOWN_STATE     Unknown state during class operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * // Callback function for the set service
 * uint32_t EI_APP_CLASS_SetCb(...)
 * {
 *     ...
 * }
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add attribute 3 for instance 1
 * errCode = EI_APP_DOP_addInstanceAttribute(
 *                  pEI_API_CIP_NODE
 *                  0x0001,
 *                  0x0003,
 *                  EI_API_CIP_eEDT_BOOL,         // Elementary data type
 *                  EI_API_CIP_eAR_GET_AND_SET,   // Available attribute access rule
 *                  NULL,                         // No get callback
 *                  EI_APP_CLASS_SetCb,           // Set callback
 *                  sizeof(ei_api_cip_edt_bool),
 *                  &instanceValue);
 *
 * \endcode
 *
 * \see EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
static uint32_t EI_APP_DOP_addInstanceAttribute(
                                                EI_API_CIP_NODE_T*   pCipNode,
                                                uint16_t             instanceId,
                                                uint16_t             attrId,
                                                EI_API_CIP_EEdt_t    edt,
                                                EI_API_CIP_EAr_t     accessRule,
                                                EI_API_CIP_CBGetAttr getCb,
                                                EI_API_CIP_CBSetAttr setCb,
                                                uint16_t             len,
                                                void*                pvValue)
{
    uint32_t errCode;

    EI_API_CIP_SAttr_t attr;
    OSAL_MEMORY_memset(&attr, 0, sizeof(attr));
    attr.id = attrId;
    attr.edt = edt;
    attr.edtSize = len;
    attr.accessRule = accessRule;
    attr.get_callback = getCb;
    attr.set_callback = setCb;
    attr.pvValue = pvValue;

    // add attribute
    errCode = EI_API_CIP_addInstanceAttr(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set initial value
    errCode = EI_API_CIP_setInstanceAttr(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
    if (errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set callback function
    if (attr.get_callback != NULL || attr.set_callback != NULL)
    {
        errCode = EI_API_CIP_setInstanceAttrFunc(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId, &attr);
        if (errCode != EI_API_CIP_eERR_OK)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }
    }

laError:
    return errCode;
}

/*!
 *
 * \brief
 * Function provides set access to the attribute value of DOP object.
 *
 * \details
 * Function for the set service of the value. All instances are connected industrial
 * LEDs controlled by TPIC2810.
 *
 * \param[in]  pCipNode                                    Pointer to the CIP node.
 * \param[in]  instanceId                                  Instance identifier.
 * \param[in]  value                                       Value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_OK                         Success.
 * \retval     #EI_API_CIP_eERR_NODE_INVALID               CIP node is invalid, possibly EI_API_CIP_NODE_new() was not called.
 * \retval     #EI_API_CIP_eERR_CLASS_DOES_NOT_EXIST       Class does not exists in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_INSTANCE_DOES_NOT_EXIST    Instance does not exist in CIP node dictionary.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_DOES_NOT_EXIST   Attribute does not exist in class or instance.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_VALUE    Invalid data pointer.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_INVALID_TYPE     Attribute type is invalid.
 * \retval     #EI_API_CIP_eERR_ATTRIBUTE_UNKNOWN_STATE    Unknown state during attribute operation reached.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * uint8_t value = 1;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * errCode = EI_APP_DOP_setValue(pEI_API_CIP_NODE, 0x0001, value);
 *
 * \endcode
 *
 * \see EI_API_CIP_EError_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
uint32_t EI_APP_DOP_setValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId, uint8_t value)
{
    uint32_t error = EI_API_CIP_eERR_GENERAL;
    uint8_t instanceIndex = instanceId - 1;

    if (EI_APP_DOP_LED_ON == value)
    {
        EI_APP_DOP_ledStatus_s |= EI_APP_DOP_LED_ON << instanceIndex;
        CUST_DRIVERS_LED_setIndustrialLeds(EI_APP_DOP_ledStatus_s);
    }
    else
    {
        EI_APP_DOP_ledStatus_s &= ~(EI_APP_DOP_LED_ON << instanceIndex);
        CUST_DRIVERS_LED_setIndustrialLeds(EI_APP_DOP_ledStatus_s);
    }

    error = EI_API_CIP_setAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, value);

    return error;
}

/*!
 *
 * \brief
 * Set attribute single service callback of DOP object.
 *
 * \param[in]  pCipNode                        Pointer to the CIP node.
 * \param[in]  classId                         Class identifier.
 * \param[in]  instanceId                      Instance identifier.
 * \param[in]  attrId                          Attribute identifier.
 * \param[in]  len                             Data type length.
 * \param[in]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_CB_ERR_CODE_t as uint32_t.
 *
 * \retval     #EI_API_CIP_eERR_GENERAL        General error.
 * \retval     #EI_API_eERR_CB_VAL_TOO_HIGH    Value is too high.
 * \retval     #EI_API_eERR_CB_VAL_TOO_LOW     Value is too low.
 * \retval     #EI_API_eERR_CB_INVALID_VALUE   Value is not valid.
 * \retval     #EI_API_eERR_CB_NOT_ENOUGH_DATA Not enough data.
 * \retval     #EI_API_eERR_CB_TOO_MUCH_DATA   Too much data.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use set callback
 * errCode = EI_APP_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                     // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,               // Available attribute access rule
 *                              NULL,                                     // No get callback
 *                              EI_APP_DOP_setValueCb, // Set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
uint32_t EI_APP_DOP_setValueCb(
                              EI_API_CIP_NODE_T* pCipNode,
                              uint16_t classId,
                              uint16_t instanceId,
                              uint16_t attrId,
                              uint16_t len,
                              void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    uint32_t error = EI_API_CIP_eERR_GENERAL;
    uint8_t value = *(uint8_t*)pvValue;

    if (sizeof(ei_api_cip_edt_bool) != len)
    {
        // Not necessary, already validated by the object dictionary.
        error = len < sizeof(ei_api_cip_edt_bool) ? EI_API_eERR_CB_NOT_ENOUGH_DATA : EI_API_eERR_CB_TOO_MUCH_DATA;
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }
    error = value <= 1 ? EI_APP_DOP_setValue(pCipNode, instanceId, value) : EI_API_eERR_CB_INVALID_VALUE;

laError:
    return error;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute value of DOP object.
 *
 * \details
 * Function for the get service of the value. All instances are connected industrial
 * LEDs controlled by TPIC2810.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     value as boolean.
 *
 * \retval     0         LED is turned off.
 * \retval     1         LED is turned on.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * error = EI_APP_DOP_getValue(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
bool EI_APP_DOP_getValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t value = 0;

    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03, &value);

    return value;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOP object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  len                             Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use get callback
 * errCode = EI_APP_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                     // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,               // Available attribute access rule
 *                              EI_APP_DOP_getValueCb, // Get callback
 *                              NULL,                                     // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
uint32_t EI_APP_DOP_getValueCb(
                              EI_API_CIP_NODE_T* pCipNode,
                              uint16_t classId,
                              uint16_t instanceId,
                              uint16_t attrId,
                              uint16_t* len,
                              void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *len = sizeof(bool);
    *(bool*)pvValue = EI_APP_DOP_getValue(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute Fault Action of DOP object. If Discrete Output Group (DOG) Object is implemented,
 * this value will be getting from DOG's Fault Action attribute.
 *
 * \details
 * Function for the get service of action taken on output’s value in Recoverable Fault state.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     Fault Action as boolean.
 *
 * \retval     0         Fault Value attribute.
 * \retval     1         Hold last state.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * error = EI_APP_DOP_getFaultAction(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
bool EI_APP_DOP_getFaultAction(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t faultAction = 0;
    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_07, &faultAction);

    return faultAction;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOP object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  len                             Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use get callback
 * errCode = EI_APP_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                           // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                     // Available attribute access rule
 *                              EI_APP_DOP_getFaultActionCb, // Get callback
 *                              NULL,                                           // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
uint32_t EI_APP_DOP_getFaultActionCb(
                                    EI_API_CIP_NODE_T* pCipNode,
                                    uint16_t classId,
                                    uint16_t instanceId,
                                    uint16_t attrId,
                                    uint16_t* len,
                                    void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *len = sizeof(bool);
    *(bool*)pvValue = EI_APP_DOP_getFaultAction(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute Fault Action of DOP object. If Discrete Output Group (DOG) Object is implemented,
 * this value will be getting from DOG's Fault value attribute.
 *
 * \details
 * Function for the get service of User–defined value for use with Fault Action attribute.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     Fault Value as boolean.
 *
 * \retval     0         Off.
 * \retval     1         On.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * error = EI_APP_DOP_getFaultValue(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE
 *
 */
bool EI_APP_DOP_getFaultValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t faultValue = 0;
    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_08, &faultValue);

    return faultValue;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOP object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  len                             Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use get callback
 * errCode = EI_APP_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                          // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                    // Available attribute access rule
 *                              EI_APP_DOP_getFaultValueCb, // Get callback
 *                              NULL,                                          // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
uint32_t EI_APP_DOP_getFaultValueCb(
                                   EI_API_CIP_NODE_T* pCipNode,
                                   uint16_t classId,
                                   uint16_t instanceId,
                                   uint16_t attrId,
                                   uint16_t* len,
                                   void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *len = sizeof(bool);
    *(bool*)pvValue = EI_APP_DOP_getFaultValue(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute Idle Action of DOP object. If Discrete Output Group (DOG) Object is implemented,
 * this value will be getting from DOG's Idle Action attribute.
 *
 * \details
 * Function for the get service of action taken on output’s value in Idle state.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     Idle Action as boolean.
 *
 * \retval     0         Idle Value attribute.
 * \retval     1         Hold last state.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * error = EI_APP_DOP_getIdleAction(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
bool EI_APP_DOP_getIdleAction(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t idleAction = 0;
    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_09, &idleAction);

    return idleAction;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOP object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  len                             Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use get callback
 * errCode = EI_APP_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                          // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,                    // Available attribute access rule
 *                              EI_APP_DOP_getIdleActionCb, // Get callback
 *                              NULL,                                          // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
uint32_t EI_APP_DOP_getIdleActionCb(
                                   EI_API_CIP_NODE_T* pCipNode,
                                   uint16_t classId,
                                   uint16_t instanceId,
                                   uint16_t attrId,
                                   uint16_t* len,
                                   void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *len = sizeof(bool);
    *(bool*)pvValue = EI_APP_DOP_getIdleAction(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute Idle Value of DOP object. If Discrete Output Group (DOG) Object is implemented,
 * this value will be getting from DOG's Idle Value attribute.
 *
 * \details
 * Function for the get service of User–defined value for use with Idle Action attribute.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     Idle Value as boolean.
 *
 * \retval     0         Off.
 * \retval     1         On.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * error = EI_APP_DOP_getIdleValue(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
bool EI_APP_DOP_getIdleValue(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t idleValue = 0;
    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_10, &idleValue);

    return idleValue;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOP object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  len                             Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use get callback
 * errCode = EI_APP_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                     // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,               // Available attribute access rule
 *                              EI_APP_DOP_getFaultValueCb, // Get callback
 *                              NULL,                                     // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
uint32_t EI_APP_DOP_getIdleValueCb(
                                  EI_API_CIP_NODE_T* pCipNode,
                                  uint16_t classId,
                                  uint16_t instanceId,
                                  uint16_t attrId,
                                  uint16_t* len,
                                  void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *len = sizeof(bool);
    *(bool*)pvValue = EI_APP_DOP_getIdleValue(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Function provides get access to the attribute Run_Idle_Command of DOP object. If Discrete Output Group (DOG) Object is implemented,
 * this value will be getting from DOG's Command attribute.
 *
 * \details
 * Function for the get service of generating the Receive_Idle or Receive_ Ready_to_Run event.
 *
 * \param[in]  pCipNode   Pointer to the CIP node.
 * \param[in]  instanceId Instance identifier.
 *
 * \return     runIdleCommand as boolean.
 *
 * \retval     0         Receive_Idle.
 * \retval     1         Receive_Ready_to_Run.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t error;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * error = EI_APP_DOP_getRunIdleCommand(pEI_API_CIP_NODE, 0x0001);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
bool EI_APP_DOP_getRunIdleCommand(EI_API_CIP_NODE_T* pCipNode, uint16_t instanceId)
{
    uint8_t runIdleCommand = 0;
    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06, &runIdleCommand);

    return runIdleCommand;
}

/*!
 *
 * \brief
 * Get attribute single service callback of DOP object.
 *
 * \param[in]   pCipNode                        Pointer to the CIP node.
 * \param[in]   classId                         Class identifier.
 * \param[in]   instanceId                      Instance identifier.
 * \param[in]   attrId                          Attribute identifier.
 * \param[out]  len                             Data type length.
 * \param[out]  pvValue                         Pointer to the value.
 *
 * \return     #EI_API_CIP_EError_t as uint32_t.
 *
 * \retval     #EI_API_eERR_CB_NO_ERROR         Success.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 * uint32_t errCode = EI_API_CIP_eERR_GENERAL;
 * ei_api_cip_edt_bool instanceValue = 0; // Dummy value for each instance
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * // Add instance attribute & use get callback
 * errCode = EI_APP_DOP_addInstanceAttribute(
 *                              pEI_API_CIP_NODE,
 *                              0x0001,
 *                              0x0003,
 *                              EI_API_CIP_eEDT_BOOL,                     // Elementary data type
 *                              EI_API_CIP_eAR_GET_AND_SET,               // Available attribute access rule
 *                              EI_APP_DOP_getRunIdleCommandCb, // Get callback
 *                              NULL,                                     // No set callback
 *                              sizeof(ei_api_cip_edt_bool),
 *                              &instanceValue);
 *
 * \endcode
 *
 * \see EI_APP_DOP_addInstanceAttribute  EI_API_CIP_EEdt_t  EI_API_CIP_EAr_t  EI_API_CIP_CB_ERR_CODE_t
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
uint32_t EI_APP_DOP_getRunIdleCommandCb(
                                       EI_API_CIP_NODE_T* pCipNode,
                                       uint16_t classId,
                                       uint16_t instanceId,
                                       uint16_t attrId,
                                       uint16_t* len,
                                       void* pvValue)
{
    OSALUNREF_PARM(classId);
    OSALUNREF_PARM(attrId);

    *len = sizeof(bool);
    *(bool*)pvValue = EI_APP_DOP_getRunIdleCommand(pCipNode, instanceId);

    return EI_API_eERR_CB_NO_ERROR;
}

/*!
 *
 * \brief
 * Initialize class data.
 *
 * \details
 * This function creates the class with class instance and instances.
 * The class instance contains attribute for the revision.
 * The instances contain attribute for the value.
 *
 * \param[in]  pCipNode Pointer to the CIP node.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * EI_APP_DOP_init(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
void EI_APP_DOP_init(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    EI_API_CIP_SService_t service;

    errCode = EI_API_CIP_createClass(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID);

    // Example how to evaluate error codes returned by API functions.
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    // set class instance
    OSAL_MEMORY_memset(&service, 0, sizeof(service));
    service.code = EI_API_CIP_eSC_GETATTRSINGLE;
    errCode = EI_API_CIP_addClassService(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, &service);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }
    errCode = EI_APP_DOP_addClassAttribute(pCipNode, 1, &dopClassData_s.revision);
    if (EI_API_CIP_eERR_OK != errCode)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    for (uint16_t i = 1; i <= EI_APP_DIO_DEVICE_DOP_NUM_OF_INST; i++)
    {
        // Dummy value for each instance
        ei_api_cip_edt_bool instanceValue = 0;
        // Create instances
        errCode = EI_API_CIP_createInstance(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, i);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add set & get service for instances
        service.code = EI_API_CIP_eSC_SETATTRSINGLE;
        errCode = EI_API_CIP_addInstanceService(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, i, &service);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        service.code = EI_API_CIP_eSC_GETATTRSINGLE;
        errCode = EI_API_CIP_addInstanceService(pCipNode, EI_APP_DIO_DEVICE_DOP_CLASS_ID, i, &service);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add attribute 3 Value (required) for instance
        errCode = EI_APP_DOP_addInstanceAttribute(
                                                  pCipNode,
                                                  i,
                                                  EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_03,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET_AND_SET,
                                                  EI_APP_DOP_getValueCb,
                                                  EI_APP_DOP_setValueCb,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add attribute 5 Fault Action (optional) for instance
        errCode = EI_APP_DOP_addInstanceAttribute(
                                                  pCipNode,
                                                  i,
                                                  EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_05,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET,
                                                  EI_APP_DOP_getFaultActionCb,
                                                  NULL,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add attribute 6 Fault Value (optional) for instance
        errCode = EI_APP_DOP_addInstanceAttribute(
                                                  pCipNode,
                                                  i,
                                                  EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET,
                                                  EI_APP_DOP_getFaultValueCb,
                                                  NULL,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add attribute 7 Idle Action (optional) for instance
        errCode = EI_APP_DOP_addInstanceAttribute(
                                                  pCipNode,
                                                  i,
                                                  EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_07,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET,
                                                  EI_APP_DOP_getIdleActionCb,
                                                  NULL,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add attribute 8 Idle Value (optional) for instance
        errCode = EI_APP_DOP_addInstanceAttribute(
                                                  pCipNode,
                                                  i,
                                                  EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_08,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET,
                                                  EI_APP_DOP_getIdleValueCb,
                                                  NULL,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }

        // Add attribute 9 Run_Idle_Command (optional) for instance
        errCode = EI_APP_DOP_addInstanceAttribute(
                                                  pCipNode,
                                                  i,
                                                  EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_09,
                                                  EI_API_CIP_eEDT_BOOL,
                                                  EI_API_CIP_eAR_GET,
                                                  EI_APP_DOP_getRunIdleCommandCb,
                                                  NULL,
                                                  sizeof(ei_api_cip_edt_bool),
                                                  &instanceValue);
        if (EI_API_CIP_eERR_OK != errCode)
        {
            OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            goto laError;
        }
    }

laError:
    return;
}

/*!
 *
 * \brief
 * Run function for class DOP.
 *
 * \details
 * This function must be called cyclically. It updates the value and turns on & off the industrial
 * LEDs controlled by TPIC2810.
 *
 * \param[in]  pCipNode Pointer to the CIP node.
 *
 * \par Example
 * \code{.c}
 * #include <discreteIoDevice/app_discrete_io_device.h>
 *
 * EI_API_CIP_NODE_T* pEI_API_CIP_NODE = NULL;
 *
 * // Create a CIP node
 * pEI_API_CIP_NODE = EI_API_CIP_NODE_new();
 *
 * EI_APP_DOP_run(pEI_API_CIP_NODE);
 *
 * \endcode
 *
 * \ingroup EI_APP_DISCRETE_IO_DEVICE_DOP
 *
 */
void EI_APP_DOP_run(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode = EI_API_CIP_eERR_OK;
    uint8_t buffer[EI_APP_DIO_DEVICE_DOP_NUM_OF_INST] = { 0 };

    errCode = EI_API_CIP_getAssemblyData(pCipNode, EI_APP_DIO_DEVICE_ASSEMBLY_CONSUMING, buffer, EI_APP_DIO_DEVICE_DOP_NUM_OF_INST);
    if(errCode != EI_API_CIP_eERR_OK)
    {
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        goto laError;
    }

    uint8_t command = 0;
    uint8_t instanceId = 0x01;
    EI_API_CIP_getAttr_bool(pCipNode, EI_APP_DIO_DEVICE_DOG_CLASS_ID, instanceId, EI_APP_CIP_INSTANCE_ATTRIBUTE_ID_06, &command);

    // Mirror I/O data
    for(uint8_t instanceIndex = 0; instanceIndex < EI_APP_DIO_DEVICE_DOP_NUM_OF_INST; instanceIndex++)
    {
        if(command == EI_APP_DOP_receiveReadyToRun)
        {
            if (EI_APP_DOP_LED_OFF == buffer[instanceIndex])
            {
                EI_APP_DOP_ledStatus_s &= ~(EI_APP_DOP_LED_ON << instanceIndex);
                CUST_DRIVERS_LED_setIndustrialLeds(EI_APP_DOP_ledStatus_s);
            }
            else if (EI_APP_DOP_LED_ON == buffer[instanceIndex])
            {
                EI_APP_DOP_ledStatus_s |= (EI_APP_DOP_LED_ON << instanceIndex);
                CUST_DRIVERS_LED_setIndustrialLeds(EI_APP_DOP_ledStatus_s);
            }
        }
    }

laError:
    return;
}
