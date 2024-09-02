/*!
 * \file pn_api_iod_dbai.h
 *
 * \brief
 * DBAI   (Direct Buffer Access Interface)
 * The DBAI provides a direct buffer access to the IOCR-data.
 * This provides a performance benefit compared to the Standard
 * Interface (SI) in the following use case:
 *   - device has a lot of submodules with few bytes in
 *     every submodule
 *
 * \author
 * KUNBUS GmbH
 *
 * \copyright
 * Copyright (c) 2023, KUNBUS GmbH<br /><br />
 * SPDX-License-Identifier: LicenseRef-Kunbus
 *
 * Copyright (c) 2024 KUNBUS GmbH
 * All rights reserved.
 *
 *
 */

#ifndef PN_API_IOD_DBAI_H
#define PN_API_IOD_DBAI_H

#include "pn_api_iod_types.h"

#if(defined __cplusplus)
extern "C" {
#endif


/*!
 * \brief
 * Acquires the semaphore of the IOD thread, ensuring that application access to shared data
 * is safeguarded against callback functions. This precautionary measure is taken to prevent
 * the inadvertent deletion of this data in case the AR unexpectedly terminates while the
 * application is still utilizing it.
 *
 * \details
 * This function must be called before PN_API_IOD_lockDbaiBuf().
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_dbai.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_enterDbai(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DBAI_DOXY_GROUP
 */
uint32_t PN_API_IOD_enterDbai(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Releases the semaphore of the IOD thread, thereby removing the protection of
 * application access to shared data against callback functions.
 *
 * \details
 * This function should be called only after releasing the buffer with PN_API_IOD_unlockDbaiBuf().
 *
 * \param[in]      pnHandle            Profinet API Handle.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_dbai.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * status = PN_API_IOD_exitDbai(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DBAI_DOXY_GROUP
 */
uint32_t PN_API_IOD_exitDbai(PN_API_IOD_Handle_t *const pnHandle);

/*!
 * \brief
 * Locks a data buffer for a designated output IOCR to enable direct access to IO data.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      lock                Pointer to structure that contains specific buffer control.
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_dbai.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * PN_API_IOD_BufferLock_t lock;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * PN_API_IOD_enterDbai();
 * //Fill up lock
 *
 * status = PN_API_IOD_lockDbaiBuf(pnHandle, lock);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DBAI_DOXY_GROUP
 */
uint32_t PN_API_IOD_lockDbaiBuf(PN_API_IOD_Handle_t *const pnHandle, PN_API_IOD_BufferLock_t *const lock);

/*!
 * \brief
 * Unlocks a data buffer associated with a specified output IOCR used for direct IO data access.
 *
 * \details
 * A buffer that was requested using PN_API_IOD_lockDbaiBuf() is returned to the stack after processing.
 *
 * \param[in]      pnHandle            Profinet API Handle.
 * \param[in]      lock                Pointer to the same data of PN_API_IOD_lockDbaiBuf().
 *
 * \return         result of the operation as uint32_t.
 * \retval         #PN_API_OK          Success.
 * \retval         #PN_API_NOT_OK      Something went wrong.
 * \retval         #PN_API_ERR_PARAM   Invalid parameter.
 *
 * \par Example
 * \code{.c}
 * #include "pn_api_iod_dbai.h"
 *
 * PN_API_IOD_Handle_t* pnHandle = NULL;
 * PN_API_IOD_BufferLock_t lock;
 * uint32_t status;
 *
 * pnHandle = PN_API_IOD_new();
 *
 * PN_API_IOD_enterDbai();
 * //Fill up lock
 *
 * status = PN_API_IOD_lockDbaiBuf(pnHandle, lock);
 *
 * //do work
 *
 * status = PN_API_IOD_unlockDbaiBuf(pnHandle, lock);
 * status = PN_API_IOD_exitDbai(pnHandle);
 *
 * \endcode
 *
 * \ingroup PN_API_IOD_DBAI_DOXY_GROUP
 */
uint32_t PN_API_IOD_unlockDbaiBuf(PN_API_IOD_Handle_t *const pnHandle, PN_API_IOD_BufferLock_t *const lock);

#if(defined __cplusplus)
}
#endif

#endif // PN_API_IOD_DBAI_H
