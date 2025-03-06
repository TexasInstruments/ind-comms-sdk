/*!
 *  \file pn_app_iod_settings.h
 *
 *  \brief
 *  Functions and callback for handling IO device's info like name and IP address.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

#ifndef PN_APP_IOD_SETTINGS_H
#define PN_APP_IOD_SETTINGS_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif


/*!
 * \brief
 * Callback function to reset IO device to factory settings.
 *
 * \details
 * This function resets the IO device to its factory settings when
 * requested by the IO controller.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      rtfOption           Reset option (data to be reset).
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 *
 * \ingroup PN_APP_IOD_SETTINGS_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_resetToFactoryCbf
 */
uint32_t PN_APP_IOD_cbResetToFactory(
    PN_API_IOD_Handle_t   *const pnHandle,
    const PN_API_IOD_RtfOption_t rtfOption);

/*!
 * \brief
 * Callback function to print a reported IP address.
 *
 * \details
 * This function simply prints the new IP address reported by the stack.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      ipAddr              IP address.
 * \param[in]      subnetMask          Subnet mask.
 * \param[in]      defaultGw           Default gateway.
 *
 * \ingroup PN_APP_IOD_SETTINGS_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_reportNewIpAddrCbf
 */
void PN_APP_IOD_cbReportipAddr(
    PN_API_IOD_Handle_t *const pnHandle,
    const uint32_t             ipAddr,
    const uint32_t             subnetMask,
    const uint32_t             defaultGw);

#if(defined __cplusplus)
}
#endif

#endif /* PN_APP_IOD_SETTINGS_H */
