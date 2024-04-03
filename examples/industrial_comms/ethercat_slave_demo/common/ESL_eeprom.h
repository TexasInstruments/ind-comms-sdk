/*!
 *  \file ESL_eeprom.h
 *
 *  \brief
 *  EEPROM load and store function.
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

#if !(defined __ESL_EEPROM_H__)
#define __ESL_EEPROM_H__		1

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#if (defined __cplusplus)
extern "C" {
#endif

extern void EC_SLV_APP_EEP_init(void*pContext);
extern void EC_SLV_APP_EEP_write(void*pContext,void*pEeprom,uint32_t length);
extern bool EC_SLV_APP_EEP_read(void*pContext,void*pEeprom,uint32_t *pLength);

#if (defined __cplusplus)
}
#endif

#endif /* __ESL_EEPROM_H__ */
