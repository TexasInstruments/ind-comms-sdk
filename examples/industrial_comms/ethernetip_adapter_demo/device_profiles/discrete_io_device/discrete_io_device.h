/*!
 *  \file discrete_io_device.h
 *
 *  \brief
 *  Necessary declarations of Device Profile: General Purpose of Discrete IO.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2025 Texas Instruments Incorporated
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

#ifndef DISCRETE_IO_DEVICE_H
#define DISCRETE_IO_DEVICE_H

typedef enum DIO_DEVICE_ConnectionState
{
    DIO_DEVICE_ConnectionNotEstablished = 0,
    DIO_DEVICE_ConnectionEstablished,
    DIO_DEVICE_ConnectionClosed,
    DIO_DEVICE_ConnectionTimeOut
}DIO_DEVICE_ConnectionState_t;

typedef void (*DIO_DEVICE_getConnectionEventInfo)(DIO_DEVICE_ConnectionState_t *, EI_API_ADP_SModNetStatus_t *);

#ifdef __cplusplus
extern "C" {
#endif

EI_API_ADP_SEipStatus_t DIO_DEVICE_cmgrCb(uint32_t serviceCode, EI_API_ADP_UCmgrInfo_u cmgrInfo);
void DIO_DEVICE_cobjTimeOutCb(
        uint8_t *producedConnectionPath,
        uint16_t producedConnectionPathLen,
        uint8_t *consumedConnectionPath,
        uint16_t consumedConnectionPathLen);
void DIO_DEVICE_ModNetStatusCb(EI_API_ADP_T* pAdp_p, EI_API_ADP_SModNetStatus_t status_p);
void DIO_DEVICE_getConnectionInfo(DIO_DEVICE_ConnectionState_t *pConnectionState, EI_API_ADP_SModNetStatus_t *pModNetStatus);

#ifdef  __cplusplus
}
#endif

#endif // DISCRETE_IO_DEVICE_H
