/*!
 * \file pn_app_iod_device.h
 *
 * \brief
 * Functions and callbacks for handling the configuration of the IO device.
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

#ifndef PN_APP_IOD_DEVICE_H
#define PN_APP_IOD_DEVICE_H

#include <stdbool.h>

#include "pn_api_iod_types.h"

/*!
 * \brief
 * Validate submodule compatibility against user-defined configuration.
 *
 * \details
 * Based on the module/submodule info provided and the user app configuration
 * pre-defined in pn_app_iod_mod_cfg.c, check if the module properties allow
 * its existence on a specific slot/subslot location, and if the submodule
 * configuration corresponds to the one given to the function.
 *
 * \param[in]       api                             Application Process Identifier.
 * \param[in]       slot                            Slot number.
 * \param[in]       subslot                         Subslot number.
 * \param[in]       modId                           Module identifier.
 * \param[in]       submodId                        Submodule identifier.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK                      Operation succeeded.
 * \retval          #PN_API_ERR_INVALID_SLOT        Invalid slot number.
 * \retval          #PN_API_ERR_INVALID_SUBSLOT     Invalid subslot number.
 * \retval          #PN_API_ERR_INVALID_SUBMODULE   Invalid submodule ID.
 * \retval          #PN_API_ERR_NO_MODULE           No module was found.
 * \retval          #PN_API_ERR_NO_SUBMODULE        No submodule was found.
 * \retval          #PN_API_ERR_PARAM               Invalid parameter.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_APP_IOD_validateSubmod(
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    uint32_t modId,
    uint32_t submodId);

/*!
 * \brief
 * Load pre-defined device configuration into the given Submodule and IM0 lists.
 *
 * \details
 * First, try to read and validate device configuration (list of submodules
 * and their corresponding locations and info) from the flash memory. if failed,
 * read and validate the user app configuration pre-defined in pn_app_iod_mod_cfg.c.
 * Afterwards, build the IM0 data list.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[out]      ioSubmodList        Submodule list.
 * \param[out]      ioSubmodListSize    Number of submodule list entries.
 * \param[out]      im0List             IM0 data list.
 * \param[out]      im0ListSize         Number of IM0 data list entries.
 * \param[in]       checkRemaData       1: try to get config from remanent memory first, otherwise don't.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK          Success.
 * \retval          #PN_API_NOT_OK      Something went wrong.
 * \retval          #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_APP_IOD_loadSubmodConfig(
    PN_API_IOD_Handle_t* const     pnHandle,
    PN_API_IOD_SubmodListEntry_t** ioSubmodList,
    uint32_t* ioSubmodListSize,
    PN_API_IOD_Im0ListEntry_t** im0List,
    uint32_t* im0ListSize,
    bool checkRemaData);

 /*!
 * \brief
 * Get the IM0 support of a submodule.
 *
 * \details
 * Based on its location, the IM0 support of a submodule is returned.
 * The first submodule of slot 0 has device (DAP) info. The first submodule
 * of any other slot has module info (virtual submodule interface).
 *
 * \param[in]       api            Application Process Identifier.
 * \param[in]       slot           Slot number.
 * \param[in]       subslot        Subslot number.
 * \param[in]       modId          Module identifier.
 * \param[in]       submodId       Submodule identifier.
 *
 * \return result of the operation as uint8_t
 * \retval          PN_API_IOD_Im0Support_t     IM0 support enum.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_Im0Support_t
 *
 */
uint8_t PN_APP_IOD_getIm0SupportInfo(
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    uint32_t modId,
    uint32_t submodId);

/*!
 * \brief
 * Build IM0 data.
 *
 * \details
 * IM0 data is built based on the pre-defined configuration and the slot/subslot
 * number of the given submodule.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       api                 Application Process Identifier.
 * \param[in]       slot                Slot number.
 * \param[in]       subslot             Subslot number.
 * \param[in]       im0Data             IM0 data structure to be filled.
 *
 * \return result of the operation as uint32_t.
 * \retval          #PN_API_OK          Operation succeeded.
 * \retval          #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 */
uint32_t PN_APP_IOD_buildIm0Data(
    PN_API_IOD_Handle_t *const pnHandle,
    uint32_t api,
    uint32_t slot,
    uint32_t subslot,
    PN_API_IOD_Im0Data_t *im0Data);

/*!
 * \brief
 * Callback function to indicate the plug of a module.
 *
 * \details
 * This function updates the IO data length arrays with the info of the newly
 * plugged module/submodule and notifies the user about the plug action.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       addr                Geographical address of the module.
 * \param[in]       inDataLen           Length of input data.
 * \param[in]       outDataLen          Length of output data.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_newModPlugCbf
 */
void PN_APP_IOD_cbNewModPlug(
    PN_API_IOD_Handle_t *const        pnHandle,
    const PN_API_IOD_DevAddr_t *const addr,
    const uint32_t                    inDataLen,
    const uint32_t                    outDataLen);

/*!
 * \brief
 * Callback function to indicate the pull of a module.
 *
 * \details
 * This function updates the IO data length arrays of the newly pulled
 * module/submodule to zero and notifies the user about the pull action.
 *
 * \param[in]       pnHandle            Profinet API Handle.
 * \param[in]       addr                Geographical address of the module.
 *
 * \ingroup PN_APP_IOD_DEVICE_DOXY_GROUP
 *
 * \see \ref PN_API_IOD_newModPullCbf
 */
void PN_APP_IOD_cbNewModPull(PN_API_IOD_Handle_t *const pnHandle, const PN_API_IOD_DevAddr_t *const addr);

#endif /* PN_APP_IOD_DEVICE_H */
