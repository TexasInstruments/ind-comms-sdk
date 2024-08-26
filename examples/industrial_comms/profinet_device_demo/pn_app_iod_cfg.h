/*!
 *  \file pn_app_iod_cfg.h
 *
 *  \brief
 *  Kunbus Common defines to configure the IO device's Profinet features and parameters.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2022, KUNBUS GmbH<br /><br />
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

#ifndef PN_APP_IOD_CFG_H
#define PN_APP_IOD_CFG_H

#include <stdint.h>

#if(defined __cplusplus)
extern "C" {
#endif

/*! \ingroup PN_API_IOD_STACK_CONFIG_DOXY_GROUP
 *  @{
 */
/**
 * @brief Number of PDEV ports
 *
 *
 */
#define PN_API_IOD_PDEV_NUM_OF_PORTS                        2

/**
 * @brief Macro switch for asset management record
 *
 * 0: No AMR functionality<br>
 * 1: With AMR functionality
 */
#define PN_API_IOD_INCLUDE_AMR                              0

/**
 * @brief Macro switch for S2 redundancy
 *
 * 0: No S2 functionality<br>
 * 1: S2 functionality enabled
 */
#define PN_API_IOD_INCLUDE_S2_REDUNDANCY                    1

/**
 *  @brief Macro switch for XHIF connection
 *         ERTEC will behave as slave only for PNIO stack with prepared memory interface
 *
 * 0: No XHIF memory interface<br>
 * 1: XHIF memory interface is functional
 */
#define PN_API_IOD_USED_WITH_XHIF_HOST                      0

/**
 *  @brief Macro switch for dynamic reconfiguration (CiR)
 *      Needs S2 redundancy activated to operate
 *
 * 0: Support for DR is not enabled<br>
 * 1: Support for DR is enabled
 */
#define PN_API_IOD_INCLUDE_DR                               1
#if ((0 == PN_API_IOD_INCLUDE_S2_REDUNDANCY) && (1 == PN_API_IOD_INCLUDE_DR))
#error "Dynamic reconfiguration requires S2 redundancy activated to operate"
#endif

/**
 *  @brief Macro switch for I&M data handling
 *
 * 1: PNIO stack handles IM0..4<br>
 * 0: Application handles all IM
 */
#define PN_API_IOD_INCLUDE_IM0_4                            1

/**
 * @brief Macro switch for IM5 functionality
 *
 * 0: No IM5 functionality<br>
 * 1: IM5 functionality
 */
#define PN_API_IOD_INCLUDE_IM5                              1

/**
 * @brief Marco switch for handing with 8028/8029 record
 *
 * 0: Application handles rec 8028/8029<br>
 * 1: Record read index 8028/8029 handling inside stack
 */
#define PN_API_IOD_INCLUDE_REC8028_8029                     1

/**
 * @brief Macro switch for MRP functionality
 *
 * 0: No MRP functionality<br>
 * 1: MRP functionality enabled
 */
#define PN_API_IOD_INCLUDE_MRP                              1

/**
 * @brief Macro switch for MRPD functionality
 *
 * 0: No MRPD functionality<br>
 * 1: MRPD functionality enabled
 */
#define PN_API_IOD_INCLUDE_MRPD                             0

/**
 * @brief Macro switch for POF functionality
 *
 * 0: No POF board<br>
 * 1: POF board
 *
 */
#define PN_API_IOD_INCLUDE_POF                              0

/**
 * @brief Macro switch for FSU functionality
 *
 * 0: Deactivate FSU capability<br>
 * 1: Activate FSU capability
 *
 */
#define PN_API_IOD_FSU_SUPPORTED                            1

/**
 * @brief Enable performance upgrade features (such as short preamble, DFP,
 *        dynamic fragmentation, absolute/relative forwarding)
 *
 * 1: Enable<br>
 * 0: Disable
 */
#define PN_API_IOD_PU_FEATURES_SUPPORT                      0

/**
 * @brief Device Vendor in SNMP
 *
 *
 */
#define PN_API_IOD_DEVKIT_VENDOR                            "Texas Instruments"

/**
 * @brief Device Product Family in SNMP
 *
 *
 */
#define PN_API_IOD_DEVKIT_PRODUCT_FAMILY                    "SITARA"

/**
 * @brief Device Product Name
 *
 *
 */
#define PN_API_IOD_DEVKIT_PRODUCT_NAME                      "Sitara PN-IO"

/**
 * @brief Vendor ID
 *
 * Must fit to GSDML file
 */
#define PN_API_IOD_VENDOR_ID                                0x01C4U

/**
 * @brief Device ID
 *
 * Must fit to GSDML file
 */
#define PN_API_IOD_DEVICE_ID                                0x6402U

/**
 * @brief Maximum input data size per AR
 *
 * @verbatim
 * Refer to GSDML: <IOConfigData MaxInputLength="720" MaxOutputLength="720" MaxDataLength="1440"/>
 * @endverbatim
 */
#define PN_API_IOD_MAX_IN_DATA_LEN   720U /* 1440 /2 = 720 */

/**
 * @brief Maximum output data size per AR
 *
 * @verbatim
 * Refer to GSDML: <IOConfigData MaxInputLength="720" MaxOutputLength="720" MaxDataLength="1440"/>
 * @endverbatim
 */
#define PN_API_IOD_MAX_OUT_DATA_LEN  720U /* 1440 /2 = 720 */

/**
 * @brief Maximum i/o data size per AR
 *
 * @verbatim
 * Refer to GSDML: <IOConfigData MaxInputLength="720" MaxOutputLength="720" MaxDataLength="1440"/>
 * @endverbatim
 */
#define PN_API_IOD_MAX_IO_DATA_LEN   (PN_API_IOD_MAX_IN_DATA_LEN + PN_API_IOD_MAX_OUT_DATA_LEN)  /* 1440 */

/**
 * @brief Maximum number of controller/supervisor ARs per device
 *
 * Refer to the data sheet for the maximum AR configuration
 *
 */
#define PN_API_IOD_NUM_OF_IO_AR                             4U

#if (PN_API_IOD_NUM_OF_IO_AR > 8U)
    #error "Wrong IO-AR Config (PN_API_IOD_NUM_OF_IO_AR > 8U)"
#endif

/**
 * @brief Maximum Provider Protocol Machines
 *
 *
 */
#define PN_API_IOD_MAX_PPM 8U

/**
 * @brief Maximum Consumer Protocol Machines
 *
 *
 */
#define PN_API_IOD_MAX_CPM 8U

/**
 * @brief Number of IOM for CRs
 *
 *
 */
#define PN_API_IOD_IOM_NUM_OF_CR                            (PN_API_IOD_MAX_PPM + PN_API_IOD_MAX_PPM)

/**
 * @brief Number of IOM for ARs
 *
 *
 */
#define PN_API_IOD_NUM_OF_AR                                (PN_API_IOD_NUM_OF_IO_AR + 1U)

/**
 * @brief Maximum number of device access ARs per device
 *
 *
 */
#define PN_API_IOD_NUM_OF_DEV_ACCESS_AR                     1U

/**
 * @brief Maximum number of slots
 *
 *
 */
#define PN_API_IOD_MAX_NUM_OF_SLOTS                         9U

/**
 * @brief Maximum number of subslots per slot
 *
 *
 */
#define PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT             10U

/**
 * @brief Maximum number of subslots
 *
 *
 */
#define PN_API_IOD_MAX_NUM_OF_SUBSLOTS                      (PN_API_IOD_MAX_NUM_OF_SLOTS * PN_API_IOD_MAX_NUM_OF_SUBSLOTS_PER_SLOT)

/**
 * @brief Number of bytes per subslot
 *
 *
 */
#define PN_API_IOD_NUM_OF_BYTES_PER_SUBSLOT                 64U

/**
 * @brief Maximum number of DAP subslots
 *
 *
 */
#define PN_API_IOD_NUM_OF_DAP_SUBSLOTS                      (1U /*Subslot-1 DAP representative*/ + 1U /*Interface: 0x8000*/ + PN_API_IOD_PDEV_NUM_OF_PORTS)   /*!< Subslot: 1, 0x8000, 08001 and 0x8002 */

/**
 * @brief Device Access Point slot
 *
 * Must fit to GSDML file
 */
#define PN_API_IOD_DAP_SLOT_NUM                             0U

/**
 * @brief Device Access Point subslot
 *
 * Must fit to GSDML file
 */
#define PN_API_IOD_DAP_SUBSLOT_NUM                          1U

/**
 * @brief Maximum data size for record read/write RQs
 *
 *
 */
#ifndef PND_EDDP_MEM_OPTIMIZATION
 #define PN_API_IOD_MAX_RECORD_LEN                           (8U * 1024U)
#else
 #define PN_API_IOD_MAX_RECORD_LEN                           (2U * 1024U)
#endif

/**
 * @brief Maximum IO net length per subslot
 *
 *
 */
#define PN_API_IOD_MAX_IO_NET_LEN_PER_SUBSLOT               256U

/**
 * @brief Maximum data size for process alarms
 *
 *
 */
#define PN_API_IOD_MAX_PROCESS_ALARM_LEN                    64U

/**
 * @brief Maximum info data size for upload/retrieval alarms
 *
 *
 */
#define PN_API_IOD_MAX_UPLOAED_RETRIEVAL_INFO_LEN           24U

/**
 * @brief Maximum data size for generic diagnosis data
 *
 *
 */
#define PN_API_IOD_MAX_GEN_DIAG_DATA_LEN                    200U

/**
 * @brief Interface name description in SNMP
 *
 */
#define PN_API_IOD_SNMP_INTERFACE_NAME                      "TI-SITARA-DEVKIT, internal, X1"

/**
 * @brief PHY port 1 description in SNMP
 *
 */
#define PN_API_IOD_SNMP_PORT1_NAME                          "TI-SITARA-DEVKIT, Ethernet Port, X1 P1"

/**
 * @brief PHY port 2 description in SNMP
 *
 */
#define PN_API_IOD_SNMP_PORT2_NAME                          "TI-SITARA-DEVKIT, Ethernet Port, X1 P2"

/**
 * @brief PHY port 3 description in SNMP
 *
 */
#define PN_API_IOD_SNMP_PORT3_NAME                          "TI-SITARA-DEVKIT, Ethernet Port, X1 P3"

/**
 * @brief PHY port 4 description in SNMP
 *
 */
#define PN_API_IOD_SNMP_PORT4_NAME                          "TI-SITARA-DEVKIT, Ethernet Port, X1 P4"

/**
 * @brief Interface description used in SNMP
 *
 */
#define PN_API_IOD_SNMP_INTERFACE_DESC                      "TI-SITARA-DEVKIT PNIO-Device"

#if (defined __cplusplus)
}
#endif
/** @}*/
#endif /* PN_APP_IOD_CFG_H */
