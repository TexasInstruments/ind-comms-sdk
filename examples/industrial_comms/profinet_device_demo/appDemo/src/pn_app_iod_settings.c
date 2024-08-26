/*!
 * \file pn_app_iod_settings.c
 *
 * \brief
 * Functions and callback for handling IO device's info like name and IP address.
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

#include "pn_app_iod_settings.h"

#include <stdbool.h>
#include <string.h>

#include "osal.h"

#include "pn_api_iod_error.h"
#include "pn_api_iod_types.h"
#include "pn_app_iod_bsp.h"


uint32_t PN_APP_IOD_cbResetToFactory(PN_API_IOD_Handle_t *const pnHandle, const PN_API_IOD_RtfOption_t rtfOption)
{
    PN_APP_IOD_factoryResetRemaMem(pnHandle, rtfOption);

    // must be PN_API_OK, other's not possible
    return (PN_API_OK);
}

void PN_APP_IOD_cbReportipAddr(PN_API_IOD_Handle_t *const pnHandle,
                               const uint32_t             ipAddr,
                               const uint32_t             subnetMask,
                               const uint32_t             defaultGw)
{
    OSALUNREF_PARM(pnHandle);

    OSAL_printf(
        "IP address = %d.%d.%d.%d, Subnet mask = %d.%d.%d.%d, Default router = %d.%d.%d.%d\r\n",
        (uint8_t) ipAddr & 0xFF,
        (uint8_t) (ipAddr >> 8) & 0xFF,
        (uint8_t) (ipAddr >> 16) & 0xFF,
        (uint8_t) (ipAddr >> 24) & 0xFF,
        (uint8_t) subnetMask & 0xFF,
        (uint8_t) (subnetMask >> 8) & 0xFF,
        (uint8_t) (subnetMask >> 16) & 0xFF,
        (uint8_t) (subnetMask >> 24) & 0xFF,
        (uint8_t) defaultGw & 0xFF,
        (uint8_t) (defaultGw >> 8) & 0xFF,
        (uint8_t) (defaultGw >> 16) & 0xFF,
        (uint8_t) (defaultGw >> 24) & 0xFF);
}
