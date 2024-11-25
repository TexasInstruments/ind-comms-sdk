/*!
 *  \file IOLM_Sitara_Version.h
 *
 *  \brief
 *  Get the version string of the IO-Link master library.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
\brief Get the version string of the IO-Link master library

This function returns the built in version string of the IO-Link master library.
It is set at the creating of the binary library.

\return const char*

*/
const char* IOLM_LIB_getVersion();
