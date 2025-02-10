/*!
 *  \file pn_api_iod_error.h
 *
 *  \brief
 *  Error codes for the IO device.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#ifndef PN_API_IOD_ERROR_H
#define PN_API_IOD_ERROR_H

/*! \ingroup PN_API_IOD_ERROR_DOXY_GROUP
 * @{
 */
#define PN_API_OK                           0x00000000U /*!< No Error */
#define PN_API_NOT_OK                       0x00000001U /*!< General Fatal Error */
#define PN_API_ERR_FLASH_STORE              0x00000002U /*!< Error during flash store operation */
#define PN_API_ERR_FLASH_RESET              0x00000003U /*!< Error during flash reset operation */

#define PN_API_ERR_RESOURCE                 0x00001000U /*!< Reserved range of numbers, starting at 0x1000 */
#define PN_API_ERR_SYS_PATH                 0x00001001U /*!< Error in system adaptation */
#define PN_API_ERR_SEQUENCE                 0x00001002U /*!< Sequence error */
#define PN_API_ERR_PARAM                    0x00001003U /*!< Error in the passed parameter */
#define PN_API_ERR_LOWER_LAYER              0x00001004U /*!< Error occurred in the lower layer */
#define PN_API_ERR_MEM_ALLOCATION           0x00001005U /*!< Error during memory allocation */
#define PN_API_ERR_NOT_ACCESSIBLE           0x00001006U /*!< Could not lock buffer */
#define PN_API_ERR_ABORT                    0x00001007U /*!< Operation was aborted */
#define PN_API_ERR_SESSION                  0x00001008U /*!< Request belongs to an old session */
#define PN_API_ERR_TIMEOUT                  0x00001009U /*!< RPC failed due to a timeout */
#define PN_API_ERR_COMM                     0x0000100AU /*!< RPC failed due to a comm-error */
#define PN_API_ERR_BUSY                     0x0000100BU /*!< RPC peer signalled "busy" (should try again later) */
#define PN_API_ERR_LOWER_NARE               0x0000100CU /*!< Error occurred in the lower layer NARE */
#define PN_API_ERR_LOWER_RPC                0x0000100DU /*!< Error occurred in the lower layer CLRPC */
#define PN_API_ERR_RESERVED_CD              0x0000100EU /*!< Reserved */
#define PN_API_ERR_OWNED                    0x0000100FU /*!< Interface-submodule cannot be removed */
#define PN_API_ERR_FORMAT                   0x00001010U /*!< Wrong format of alarm data */
#define PN_API_ERR_PRM_INDEX                0x00001011U /*!< Unknown index (PrmWrite and PrmRead) */
#define PN_API_ERR_PRM_PORTID               0x00001012U /*!< Port-id does not match with index */
#define PN_API_ERR_PRM_DATA                 0x00001013U /*!< Data-length too short (PrmRead) or not consistent */
#define PN_API_ERR_PRM_BLOCK                0x00001014U /*!< Wrong block-type/version */
#define PN_API_ERR_PRM_CONSISTENCY          0x00001015U /*!< Parameterization is not consistent (PrmEnd) */
#define PN_API_ERR_PRM_OTHER                0x00001016U /*!< Internally used */
#define PN_API_ERR_NOT_YET                  0x00001017U /*!< Reserved */

#define PN_API_ERR_NO_MODULE                0x00001100U /*!< No module found */
#define PN_API_ERR_NO_SUBMODULE             0x00001101U /*!< No submodule found */
#define PN_API_ERR_IOD_SLOT_OCCUPIED        0x00001102U /*!< Plugging a module into an occupied slot */
#define PN_API_ERR_IOD_SUBSLOT_OCCUPIED     0x00001103U /*!< Plugging a submodule into an occupied subslot */
#define PN_API_ERR_IOD_INVALID_PARAM        0x00001104U /*!< Invalid parameter */
#define PN_API_ERR_IOD_NO_AR                0x00001105U /*!< No application relation available */
#define PN_API_ERR_IOD_AR_RUNNING           0x00001106U /*!< Function only allowed if no AR is running */
#define PN_API_ERR_IOD_INVALID_API          0x00001107U /*!< Invalid API number */
#define PN_API_ERR_INVALID_SLOT             0x00001108U /*!< Invalid slot number */
#define PN_API_ERR_INVALID_SUBSLOT          0x00001109U /*!< Invalid subslot number */
#define PN_API_ERR_IOD_NO_REC_RQ            0x0000110AU /*!< No record request pending for asynchronous response */
#define PN_API_ERR_IOD_MAXNUM_API           0x0000110BU /*!< Maximum numbers of APIs have been reached */
#define PN_API_ERR_IOD_MAXNUM_SLOTS         0x0000110CU /*!< Maximum numbers of Slots exceeded */
#define PN_API_ERR_IOD_MAXNUM_SUBSLOTS      0x0000110DU /*!< Maximum numbers of Subslots exceeded */
#define PN_API_ERR_IOD_INVALID_DEV_HNDL     0x0000110EU /*!< Invalid device handle */
#define PN_API_ERR_IOD_INVALID_AR_HNDL      0x0000110FU /*!< Invalid AR handle */
#define PN_API_ERR_IOD_INVALID_API_HNDL     0x00001110U /*!< Invalid API handle */
#define PN_API_ERR_IOD_INVALID_SLOT_HNDL    0x00001111U /*!< Invalid slot handle */
#define PN_API_ERR_IOD_INVALID_SUBSLOT_HNDL 0x00001112U /*!< Invalid subslot handle */
#define PN_API_ERR_IOD_INVALID_LOCKING      0x00001113U /*!< Locking mechanism not allowed in IRT TOP */
#define PN_API_ERR_IOD_INVALID_BUFLEN       0x00001114U /*!< Invalid buffer length */
#define PN_API_ERR_IOD_INVALID_DEVNAME      0x00001115U /*!< Invalid device name (station name) */
#define PN_API_ERR_IOD_INVALID_DEVTYPE      0x00001116U /*!< Invalid device type (station type) */
#define PN_API_ERR_IOD_NO_RESOURCES         0x00001117U /*!< Resource limit has been reached */
#define PN_API_ERR_IOD_ALARM_SEQUENCE       0x00001118U /*!< Alarm sequence error */
#define PN_API_ERR_TIMER_BAD_RETURN_VAL     0x00001119U /*!< Bad retval from timer handling */
#define PN_API_ERR_SYS                      0x0000111AU /*!< System error */
#define PN_API_ERR_PRM_CALLBACK             0x0000111BU /*!< Parameter cbf is illegal */
#define PN_API_ERR_INVALID_SUBMODULE        0x00001128U /*!< Submodule config doesn't contain corresponding module */
/** @}*/

#endif /* PN_API_IOD_ERROR_H */
