/*!
 *  \file app_discrete_io_device.h
 *
 *  \brief
 *  Necessary declarations of Device Profile: General Purpose of Discrete IO.
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

#ifndef APP_DISCRETE_IO_DEVICE_H
#define APP_DISCRETE_IO_DEVICE_H

typedef enum EI_APP_DIO_DEVICE_ConnectionState
{
    EI_APP_DIO_DEVICE_ConnectionNotEstablished = 0,
    EI_APP_DIO_DEVICE_ConnectionEstablished,
    EI_APP_DIO_DEVICE_ConnectionClosed,
    EI_APP_DIO_DEVICE_ConnectionTimeOut
}EI_APP_DIO_DEVICE_ConnectionState_t;

#ifdef __cplusplus
extern "C" {
#endif

EI_API_ADP_SEipStatus_t EI_APP_DIO_DEVICE_cmgrCb(uint32_t serviceCode, EI_API_ADP_UCmgrInfo_u cmgrInfo);
EI_API_ADP_SEipStatus_t EI_APP_DIO_DEVICE_cobjTimeOutCb(
        uint8_t *producedConnectionPath,
        uint16_t producedConnectionPathLen,
        uint8_t *consumedConnectionPath,
        uint16_t consumedConnectionPathLen);

#ifdef  __cplusplus
}
#endif

#endif // APP_DISCRETE_IO_DEVICE_H
