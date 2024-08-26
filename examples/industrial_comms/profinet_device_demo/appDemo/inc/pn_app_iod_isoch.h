/*!
 * \file pn_app_iod_isoch.h
 *
 * \brief
 * Defines, enums and structs needed for isochronous mode.
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

#ifndef PN_APP_IOD_ISOCH_H
#define PN_APP_IOD_ISOCH_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

#define GSD_T_DC_BASE   1U          /*!< Time base of T_DC (data exchange cycle time), GSD_T_DC_MIN and
                                         GSD_T_DC_MAX in units of 31,25 us. */
#define GSD_T_DC_MIN    8U          /*!< Minimum of T_DC (data exchange cycle time) in units of GSD_T_DC_BASE.
                                         Allowed values: 1 to 1024. GSD_T_DC_MIN shall be less than or equal to
                                         GSD_T_DC_MAX. GSD_T_DC_MIN multiplied by GSD_T_DC_BASE shall be less than
                                         or equal to 1024. */
#define GSD_T_DC_MAX    1024U       /*!< Maximum of T_DC (data exchange cycle time) in units of GSD_T_DC_BASE.
                                         Allowed values: 1 to 1024. GSD_T_DC_MAX shall be greater than or equal to
                                         GSD_T_DC_MIN. GSD_T_DC_MAX multiplied by GSD_T_DC_BASE shall be less than
                                         or equal to 1024.*/
#define GSD_T_IO_BASE   125000U     /*!< Time base of T_IO_Input, T_IO_Output, GSD_T_IO_IN_MIN, GSD_T_IO_OUT_MIN in
                                         nanoseconds. T_IO_Input is the point in time when the input values are
                                         collected and T_IO_Output is the point in time when the output values are
                                         taken over. The smallest possible value shall be declared.
                                         Allowed values: 1 to 32000000 */
#define GSD_T_IO_IN_MIN     1U      /*!< The minimum time that is necessary to get and update the input values of an
                                         individual input submodule, in units of GSD_T_IO_BASE. GSD_T_IO_IN_MIN
                                         multiplied by GSD_T_IO_BASE shall be less than or equal to 32000000.
                                         GSD_T_IO_IN_MIN * GSD_T_IO_BASE shall be less or equal than
                                         GSD_T_DC_MAX * GSD_T_DC_BASE * 31250. If the submodule does not have input
                                         data, a value of 1 shall be entered and engineering tools shall ignore this
                                         attribute.
                                         Allowed values: >= 1 */
#define GSD_T_IO_OUT_MIN    1U      /*!< The minimum time that is necessary at the end of the data exchange time to get
                                         and update the output values of an individual output submodule, in units of
                                         GSD_T_IO_BASE. GSD_T_IO_OUT_MIN multiplied by GSD_T_IO_BASE shall be less than
                                         or equal to 32000000. GSD_T_IO_OUT_MIN * GSD_T_IO_BASE shall be less or equal
                                         than GSD_T_DC_MAX * GSD_T_DC_BASE * 31250. If the submodule does not have output
                                         data, a value of 1 shall be entered and engineering tools shall ignore this
                                         attribute.
                                         Allowed values: >= 1 */

#define ISO_T_DC_BASE                    (GSD_T_DC_BASE * 31250U)
#define ISO_T_DC_MIN                     (GSD_T_DC_MIN * ISO_T_DC_BASE)
#define ISO_T_DC_MAX                     (GSD_T_DC_MAX * ISO_T_DC_BASE)
#define ISO_T_IO_BASE                    (GSD_T_IO_BASE)
#define ISO_T_IO_IN_MIN                  (GSD_T_IO_IN_MIN * ISO_T_IO_BASE)
#define ISO_T_IO_OUT_MIN                 (GSD_T_IO_OUT_MIN * ISO_T_IO_BASE)

#pragma pack(push)
#pragma pack(1)

/*!
 * \brief Isochronous data block.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    PN_API_IOD_ioBlockHeader_t blockHeader;  /*!< see Profinet specifications, BlockHeader */
    uint8_t padding[2];                      /*!< Padding bytes */
    uint16_t slot;                           /*!< Slot number */
    uint16_t subslot;                        /*!< Subslot number */
    uint16_t cycleFactor;                    /*!< Cycle factor */
    uint16_t dataCycleTime;                  /*!< Data cycle time */
    uint32_t ioInputTime;                    /*!< Value in nsec */
    uint32_t ioOutputTime;                   /*!< Value in nsec */
    uint32_t ioInputValidTime;               /*!< Value in nsec */
    uint32_t ioOutputValidTime;              /*!< Value in nsec */
} PN_APP_IOD_isoDataBlock_t;

/*!
 * \brief Isochronous time info.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t tDcBase;   /*!< Time base of T_DC, GSD_T_DC_MIN and GSD_T_DC_MAX in units of 31,25 us */
    uint32_t tDcMin;    /*!< Minimum of T_DC (data exchange cycle time) */
    uint32_t tDcMax;    /*!< Maximum of T_DC (data exchange cycle time) */
    uint32_t tIoBase;   /*!< Time base of T_IO_Input, T_IO_Output, GSD_T_IO_IN_MIN, GSD_T_IO_OUT_MIN in nanoseconds */
    uint32_t tIoInMin;  /*!< Minimum time necessary to get and update input values of an individual input submodule */
    uint32_t tIoOutMin; /*!< Minimum time necessary to get and update output values of an individual output submodule */
} PN_APP_IOD_isoTime_t;

#pragma pack(pop)

#if(defined __cplusplus)
}
#endif

#endif /* PN_APP_IOD_ISOCH_H */
