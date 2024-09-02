/*!
 * \file pn_app_iod_mod_cfg.h
 *
 * \brief
 * Helper functions to provide a default configuration and module compatibility verification for the IO device.
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

#ifndef PN_APP_IOD_MOD_CFG_H
#define PN_APP_IOD_MOD_CFG_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif

/*! \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 * @{
 */

/* IDD: ID-DAP module */
#define PN_CFG_IDD_DAP_MOD_ID     0x64020000    /*!< DAP module ID */
#define PN_CFG_IDD_DAP_SUB_ID     0x64020001    /*!< DAP submodule ID */
#define PN_CFG_IDD_DAP_IF_ID      0x64028000    /*!< DAP interface ID */
#define PN_CFG_IDD_DAP_P1_ID      0x64028001    /*!< DAP port1 ID */
#define PN_CFG_IDD_DAP_P2_ID      0x64028002    /*!< DAP port2 ID */

/* IDM: ID-Module */
#define PN_CFG_IDM_MOD_IN_ID      0x03010100    /*!< input module ID */
#define PN_CFG_IDM_MOD_OUT_ID     0x03010200    /*!< output module ID */
#define PN_CFG_IDM_MOD_IO_ID      0x03010300    /*!< input/output module ID */

/* IDS: ID-Submodule */
#define PN_CFG_IDS_IN_IF_ID       0x03010101    /*!< ID of input module virtual submodule interface
                                                     for record data/diagnosis (acyclically) */
#define PN_CFG_IDS_IN_T1_ID       0x31010001    /*!< ID of input module telegram 1: 1-byte input data */
#define PN_CFG_IDS_IN_T2_ID       0x31010002    /*!< ID of input module telegram 2: 2-byte input data */
#define PN_CFG_IDS_IN_T4_ID       0x31010004    /*!< ID of input module telegram 4: 4-byte input data */
#define PN_CFG_IDS_IN_T64_ID      0x31010064    /*!< ID of input module telegram 64: 64-byte input data */

#define PN_CFG_IDS_OUT_IF_ID      0x03010201    /*!< ID of output module virtual submodule interface for
                                                     record data/diagnosis (acyclically) */
#define PN_CFG_IDS_OUT_T1_ID      0x31020001    /*!< ID of output module telegram 1: 1-byte output data */
#define PN_CFG_IDS_OUT_T2_ID      0x31020002    /*!< ID of output module telegram 2: 2-byte output data */
#define PN_CFG_IDS_OUT_T4_ID      0x31020004    /*!< ID of output module telegram 4: 4-byte output data */
#define PN_CFG_IDS_OUT_T64_ID     0x31020064    /*!< ID of output module telegram 64: 64-byte output data */

#define PN_CFG_IDS_IO_IF_ID       0x03010301    /*!< ID of input output module virtual submodule interface
                                                     for record data/diagnosis (acyclically) */
#define PN_CFG_IDS_IO_T1_ID       0x31030001    /*!< ID of IO module telegram 1: 1-byte I/O data */
#define PN_CFG_IDS_IO_T2_ID       0x31030002    /*!< ID of IO module telegram 2: 2-byte I/O data */
#define PN_CFG_IDS_IO_T4_ID       0x31030004    /*!< ID of IO module telegram 4: 4-byte I/O data */
#define PN_CFG_IDS_IO_T64_ID      0x31030064    /*!< ID of IO module telegram 64: 64-byte I/O data */
/*! @}*/

/*!
 * \brief List of slots.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t cnt;                                /*!< Number of slots */
    uint16_t slots[PN_API_IOD_MAX_NUM_OF_SLOTS]; /*!< Slots inside the module */
} PN_APP_IOD_SlotList_t;

/*!
 * \brief List of subslots.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t cnt;                                                   /*!< Number of subslots */
    uint16_t subslots[PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT];     /*!< Subslots inside the slot */
} PN_APP_IOD_SubslotList_t;

/*!
 * \brief List of modules.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t cnt;                                   /*!< Number of modules */
    uint32_t modIds[PN_API_IOD_MAX_NUM_OF_SLOTS];   /*!< Module identifiers of the list */
} PN_APP_IOD_ModList_t;

/*!
 * \brief Module properties.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t                 modId;                 /*!< Module identifier */
    PN_APP_IOD_SlotList_t    slotList;              /*!< Slot list of the module */
    PN_APP_IOD_SubslotList_t subslotList;           /*!< Subslot list of the module */
} PN_APP_IOD_ModPropEntry_t;

/*!
 * \brief Submodule configuration.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t             api;                       /*!< Application Process Identifier */
    uint32_t             submodId;                  /*!< Submodule identifier */
    uint16_t             inDataLen;                 /*!< Length of input data */
    uint16_t             outDataLen;                /*!< Length of output data */
    PN_APP_IOD_ModList_t modList;                   /*!< Module list of the submodule */
} PN_APP_IOD_SubmodCfgEntry_t;

/*!
 * \brief Ownership handle.
 *
 * \ingroup PN_APP_IOD_TYPES_DOXY_GROUP
 */
typedef struct
{
    uint32_t cnt;                                                                    /*!< Number of submodules */
    PN_API_IOD_RemaDataSubmodCfgInfo_t submodList[PN_API_IOD_MAX_NUM_OF_SUBSLOTS];   /*!< Remanent submodule configuration information */
} PN_APP_IOD_OwnershipHandle_t;

/*!
 * \brief
 * Reset the global ownership handle.
 *
 * \details
 * This function resets the content of the global ownership handle (ownershipHandle) to zero.
 *
 * \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 */
void PN_APP_IOD_resetOwnershipHandle(void);

/*!
 * \brief
 * Get the global ownership handle.
 *
 * \details
 * This function returns a pointer to the global ownership handle (ownershipHandle).
 *
 * \param[out]      expOwnershipHandle      Pointer to the returned ownership handle.
 *
 * \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 */
void PN_APP_IOD_getOwnershipHandle(PN_APP_IOD_OwnershipHandle_t **expOwnershipHandle);

/*!
 * \brief
 * Update the global ownership handle.
 *
 * \details
 * This function adds a new element to the global ownership handle (ownershipHandle).
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      expSubmod           Expected submodule.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK                      Operation succeeded.
 * \retval         #PN_API_NOT_OK                  Something went wrong.
 * \retval         #PN_API_ERR_PARAM               Invalid parameter.
 * \retval         #PN_API_ERR_IOD_NO_RESOURCES    No available resources.
 *
 * \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 */
uint32_t PN_APP_IOD_updateOwnershipHandle(
    PN_API_IOD_Handle_t *const pnHandle,
    PN_API_IOD_ExpSubmod_t *const expSubmod);

/*!
 * \brief
 * Get ioSubmodList defined in pn_app_iod_mod_cfg.c.
 *
 * \details
 * A getter function for the list of IO submodules (including also PDEV subslots)
 * pre-defined by the user app as ioSubmodList.
 *
 * \param[out]      list                Pointer to the returned list.
 * \param[out]      listSize            Pointer to the list size.
 *
 * \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 */
void PN_APP_IOD_getDefaultIoSubmodList(PN_API_IOD_SubmodListEntry_t **list, uint32_t *listSize);

/*!
 * \brief
 * Get modPropList defined in pn_app_iod_mod_cfg.c.
 *
 * \details
 * A getter function for the list of List of the user modules and on
 * which slots/subslots they are allowed to exist.
 *
 * \param[out]      list                Pointer to the returned list.
 * \param[out]      listSize            Pointer to the list size.
 *
 * \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 */
void PN_APP_IOD_getModPropList(PN_APP_IOD_ModPropEntry_t **list, uint32_t *listSize);

/*!
 * \brief
 * Get submodCfgList defined in pn_app_iod_mod_cfg.c.
 *
 * \details
 * A getter function for the list of the user submodules and their
 * allowed configuration.
 *
 * \param[out]      list                Pointer to the returned list.
 * \param[out]      listSize            Pointer to the list size.
 *
 * \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 */
void PN_APP_IOD_getSubmodCfgList(PN_APP_IOD_SubmodCfgEntry_t **list, uint32_t *listSize);

/*!
 * \brief
 * Get the properties of a specific module as defined in pn_app_iod_mod_cfg.c.
 *
 * \details
 * A getter function for a modPropList element that corresponds to the module
 * specified by modId.
 *
 * \param[in]       modId               Module ID.
 * \param[out]      modProp             Pointer to module properties item.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK              Operation succeeded.
 * \retval         #PN_API_ERR_NO_MODULE   Module not found.
 * \retval         #PN_API_ERR_PARAM       Invalid parameter.
 *
 * \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 */
uint32_t PN_APP_IOD_getModProp(uint32_t modId, PN_APP_IOD_ModPropEntry_t *modProp);

/*!
 * \brief
 * Get the allowed configuration of a specific submodule as defined in pn_app_iod_mod_cfg.c.
 *
 * \details
 * A getter function for a submodCfgList element that corresponds to the submodule
 * specified by api and submodId.
 *
 * \param[in]       api                 Application Process Identifier.
 * \param[in]       submodId            Submodule ID.
 * \param[out]      submodCfg           Pointer to submodule config item.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK                  Operation succeeded.
 * \retval         #PN_API_ERR_NO_SUBMODULE    Submodule not found.
 * \retval         #PN_API_ERR_PARAM           Invalid parameter.
 *
 * \ingroup PN_APP_IOD_MOD_CFG_DOXY_GROUP
 */
uint32_t PN_APP_IOD_getSubmodCfg(uint32_t api, uint32_t submodId, PN_APP_IOD_SubmodCfgEntry_t *submodCfg);

#if (defined __cplusplus)
}
#endif

#endif /* PN_APP_IOD_MOD_CFG_H */
