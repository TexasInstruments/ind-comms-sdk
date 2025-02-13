/*!
 *  \file cmn_os.c
 *
 *  \brief
 *  Common OS support of FreeRTOS.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2021 Texas Instruments Incorporated
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

#include "ti_drivers_config.h"

#if defined (SOC_AM243X) || defined (SOC_AM64X)
#include "drivers/sciclient/include/sciclient_pm.h"
#elif defined (SOC_AM263PX) || defined(SOC_AM261X)
// ToDo: AM263PX - Use replacement for SCI Client
#else
#error "Undefined SOC"
#endif

#include "cmn_os.h"

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Initialize OS module
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  #include <CMN_os.h>
 *
 *  // the Call
 *  CMN_OS_init();
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup CMN_OS
 *
 * */
void CMN_OS_init(void)
{
    System_init();
    return;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Deinitialize OS module
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  #include <CMN_os.h>
 *
 *  // the Call
 *  CMN_OS_deinit();
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup CMN_OS
 *
 * */
void CMN_OS_deinit(void)
{
    System_deinit();
    return;
}

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Reset OS module
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  #include <CMN_os.h>
 *
 *  // the Call
 *  CMN_OS_reset();
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup CMN_OS
 *
 * */
void CMN_OS_reset (void)
{
#if (defined SOC_AM243X) || (defined SOC_AM64X)
    SOC_generateSwWarmResetMcuDomain();
#elif (defined SOC_AM263PX) || defined(SOC_AM261X)
    SOC_generateSwWarmReset();
#else
    #error Select valid MCU architecture
#endif
}

//*************************************************************************************************
