/*!
 *
 *  \file app_discrete_io_device_sm.c
 *
 *  \brief
 *  Contains the function definition of how the state machine state changes
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br><br>
 *  @KUNBUS_LICENSE@
 *
 */

#include "app_discrete_io_device_sm.h"


/*!
 *  \brief
 *  EI_APP_DIO_DEVICE_SM_nextState is used to switch next state.
 *
 *  \details
 *  The exit action of current state is performed
 *  then entry action of next state is performed.
 *
 *
 *  \param[in]  pStateMachine                 State machine pointer
 *  \param[in]  pNextState                    Next state to switch
 *
 *  \return     void
 *
 *  \ingroup state_machine
 *
 */

void EI_APP_DIO_DEVICE_SM_nextState(EI_APP_DIO_DEVICE_SM_StateMachine_t *pStateMachine,
                                                const EI_APP_DIO_DEVICE_SM_State_t *pNextState)
{
    if ((pStateMachine != NULL) && (pNextState != NULL) && (pNextState->doAction != NULL))
    {
        if (pStateMachine->state.exitAction)
        {
            pStateMachine->state.exitAction();
        }

        pStateMachine->state = *pNextState;

        if (pStateMachine->state.entryAction)
        {
            pStateMachine->state.entryAction();
        }
    }
}
