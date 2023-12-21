/*!
 *  \file app_discrete_io_device_dop.h
 *
 *  \brief
 *  Necessary declarations of Discrete Output Point (DOP) Object.
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

#ifndef APP_DISCRETE_IO_DEVICE_DOP_H
#define APP_DISCRETE_IO_DEVICE_DOP_H

#define EI_APP_DOP_LED_ON  0x01 /*!< LED ON Status for DIP & DOP Objects */
#define EI_APP_DOP_LED_OFF 0x00 /*!< LED OFF Status for DIP & DOP Objects */

/**
 * @brief
 * A structure to hold Discrete Output Point's (DOP)
 * class data.
 */
typedef struct EI_APP_DOP_ClassData
{
    uint16_t revision;
}EI_APP_DOP_ClassData_t;

/**
 * @brief
 * EI_APP_DISCRETE_IO_DEVICE_DOP_SmStates_t enum is used for
 * states of DOP state machine.
 */
typedef enum EI_APP_DOP_SmStates
{
    EI_APP_DOP_SM_NONEXISTENT = 0,
    EI_APP_DOP_SM_AVAILABLE,
    EI_APP_DOP_SM_IDLE,
    EI_APP_DOP_SM_READY,
    EI_APP_DOP_SM_RUN,
    EI_APP_DOP_SM_RECOVERABLEFAULT,
    EI_APP_DOP_SM_UNRECOVERABLEFAULT,
    EI_APP_DOP_SM_MAX_STATES
}EI_APP_DOP_SmStates_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __cplusplus
}
#endif

#endif // APP_DISCRETE_IO_DEVICE_DOP_H
