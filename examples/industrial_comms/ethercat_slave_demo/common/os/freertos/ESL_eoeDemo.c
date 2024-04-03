/*!
*  \file ESL_eoeDemo.c
*
*  \brief
*  EtherCAT<sup>&reg;</sup> Ethernet over EtherCAT Example
*
*  \author
*  KUNBUS GmbH
*
*  \copyright
*  Copyright (c) 2024, KUNBUS GmbH<br /><br />
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

#include <osal.h>
#include <ESL_eoeDemo.h>

/*! \brief
*  EoE Settings Indication callback
*
*  \param[in]  pContext            The pointer to the application instance.
*  \param[in]  pMac                Virtual Net MAC address
*  \param[in]  pIp                 Virtual Net IP address
*  \param[in]  pSubNet             Virtual Net Subnet
*  \param[in]  pDefaultGateway     Virtual Net Default Gateway
*  \param[in]  pDnsIp              Virtual Net DNS server address
*  \return     true if settings are handled, false otherwise
*
*  \ingroup EC_SLV_APP
*
* */
bool EC_SLV_APP_EoE_SS_settingIndHandler(
   void *pContext,
   uint16_t *pMac,
   uint16_t *pIp,
   uint16_t *pSubNet,
   uint16_t *pDefaultGateway,
   uint16_t *pDnsIp)
{
    OSALUNREF_PARM(pContext);
    OSALUNREF_PARM(pMac);
    OSALUNREF_PARM(pIp);
    OSALUNREF_PARM(pSubNet);
    OSALUNREF_PARM(pDefaultGateway);
    OSALUNREF_PARM(pDnsIp);

    //Function not implemented
    return false;
}

/*! \brief
*  User defined EoE receive function. Called when an EoE frame is received.
*
*  \param[in]  pContext            function context
*  \param[in]  pData               EoE Frame Data
*  \param[in]  size                EoE Frame Size
*  \return true if frame is handle, false if it should be passed on.
*
*  \ingroup EC_SLV_APP
*
* */
bool EC_SLV_APP_EoE_SS_receiveHandler(void *pContext, uint16_t *pData, uint16_t size)
{
    OSALUNREF_PARM(pContext);
    OSALUNREF_PARM(pData);
    OSALUNREF_PARM(size);

    //Function not implemented
   return false;
}

/*! \brief
*  Virtual switch endpoint of EoE
*
*  \param[in]  arg		slave Handle pointer (argument)
*
*  \ingroup SLVAPI
*
* */
void EC_SLV_APP_EoE_SS_task(void *pArg)
{
    //Function not implemented
    OSALUNREF_PARM(pArg);
}
//*************************************************************************************************
