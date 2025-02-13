/*!
 *  \file drv_led.h
 *
 *  \brief
 *  Declarations related to LED's application driver.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2025 Texas Instruments Incorporated
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


#ifndef DRV_LED_H
#define DRV_LED_H

// definition of missing OSAL codes - remove on next OSAL release
#define OSAL_GENERAL_ERROR          (0x80000000u)  //!< Negative default value

#define OSAL_LED_DRV_HANDLE_INVALID (0x00060003u)  //!< LED handle set to NULL
#define OSAL_LED_DRV_GETATTR        (0x00060004u)  //!< LED_getAttrs call returns NULL
#define OSAL_LED_DRV_OPEN           (0x00060005u)  //!< LED_open call failed
#define OSAL_LED_DRV_CLOSE          (0x00060006u)  //!< LED_close call failed
#define OSAL_LED_DRV_ON             (0x00060007u)  //!< LED_on call failed
#define OSAL_LED_DRV_OFF            (0x00060008u)  //!< LED_off call failed
#define OSAL_LED_DRV_SETMASK        (0x00060009u)  //!< LED_setMask call failed

/*!
 *  \brief
 *  Application LED's initialization parameters.
 */
typedef struct DRV_LED_SInit
{
    uint32_t   instance;         /* Industrial LED's instance */
}DRV_LED_SInit_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void       DRV_LED_industrialSet (uint32_t value);
extern uint32_t   DRV_LED_init          (DRV_LED_SInit_t* pParams);
extern uint32_t   DRV_LED_deInit        (void);
extern LED_Handle DRV_LED_getHandle     (uint32_t instanceId);

#ifdef  __cplusplus
}
#endif

#endif // DRV_LED_H
