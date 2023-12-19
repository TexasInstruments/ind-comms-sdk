/*!
 *
 *  \file app_discrete_io_device_sm.h
 *
 *  \brief
 *  Contains the function prototype and typedef struct  of how the state machine state changes
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br><br>
 *  @KUNBUS_LICENSE@
 *
 */

#ifndef APP_DISCRETE_IO_DEVICE_SM_H
#define APP_DISCRETE_IO_DEVICE_SM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef void (*pFnEntryAction)(void);
typedef void (*pFnExitAction)(void);
typedef void (*pFnDoAction)(void);

/**
 * @brief
 * This struct is used to create a state machine state
 * */
typedef struct EI_APP_DIO_DEVICE_SM_State {
    pFnEntryAction entryAction;
    pFnExitAction exitAction;
    pFnDoAction doAction;
    uint32_t stateId;
} EI_APP_DIO_DEVICE_SM_State_t;

/**
 * @brief
 * This struct is used to create a state machine
 */
typedef struct EI_APP_DIO_DEVICE_SM_StateMachine {
    EI_APP_DIO_DEVICE_SM_State_t state;
} EI_APP_DIO_DEVICE_SM_StateMachine_t;

extern void EI_APP_DIO_DEVICE_SM_nextState(
        EI_APP_DIO_DEVICE_SM_StateMachine_t *pStateMachine,
    const EI_APP_DIO_DEVICE_SM_State_t *pNextState);
#ifdef __cplusplus
}
#endif


#endif /* APP_DISCRETE_IO_DEVICE_SM_H */
