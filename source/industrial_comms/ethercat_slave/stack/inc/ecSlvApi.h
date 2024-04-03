/*!
 *  \file ecSlvApi.h
 *
 *  \brief
 *  EtherCAT User API interface.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2024 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#if !(defined __ECSLVAPI_H__)
#define __ECSLVAPI_H__		1

#if (defined ECATSLAVE_SO) && (ECATSLAVE_SO==1) // defined if ECATSLV is compiled as a DLL
 #ifdef ECSLVAPI_EXPORTS // defined if we are building the ECATSLAVE DLL (instead of using it)
  #define ECATSLV_API OSAL_DLL_EXPORT
 #else
  #define ECATSLV_API OSAL_DLL_IMPORT
 #endif // ECSLVAPI_EXPORTS
 #define ECATSLV_LOC OSAL_DLL_LOCAL
#else // ECATSLAVE_SO is not defined: this means ECATSLAVE is a static lib.
 #define ECATSLV_API
 #define ECATSLV_LOC
#endif // ECATSLAVE_SO

#include <osal.h>
#include "profiles/ecSlvApi_AoE.h"
#include <ecSlvApi_base.h>
#include "profiles/ecSlvApi_CiA402.h"
#include "profiles/ecSlvApi_CoE.h"
#include <ecSlvApi_diagnosis.h>
#include <ecSlvApi_eeprom.h>
#include "profiles/ecSlvApi_EoE.h"
#include <ecSlvApi_esc.h>
#include <ecSlvApi_esi.h>
#include "profiles/ecSlvApi_FoE.h"
#include <ecSlvApi_leds.h>
#include <ecSlvApi_mdio.h>
#include <ecSlvApi_param.h>
#include <ecSlvApi_pdo.h>
#include <ecSlvApi_phy.h>
#include "profiles/ecSlvApi_SoE.h"
#include <ecSlvApi_ssc.h>
#include <ecSlvApi_types.h>

#if !(defined FBTL_REMOTE) && !(defined DPRAM_REMOTE)
#include <custom_phy.h>
#endif



#endif /* __ECSLVAPI_H__ */
