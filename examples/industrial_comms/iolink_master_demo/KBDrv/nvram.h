/*!
 *  \file nvram.h
 *
 *  \brief
 *  Access to non-volatile memory (NVRAM)
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2023 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#ifndef NVRAM_H_
#define NVRAM_H_

// this uses littleFS
#include "lfs.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define NVR_LOGLEVEL_NONE  0
#define NVR_LOGLEVEL_ERROR 1
#define NVR_LOGLEVEL_INFO  2
#define NVR_LOGLEVEL_DEBUG 3

// set logging level
#define NVR_LOGLEVEL NVR_LOGLEVEL_ERROR

#if NVR_LOGLEVEL > 2
#define NVR_LOG_DEBUG(fmt, ...) OSAL_printf("%s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#else
#define NVR_LOG_DEBUG(fmt, ...)
#endif

#if NVR_LOGLEVEL > 1
#define NVR_LOG_INFO(fmt, ...) OSAL_printf("%s: " fmt "\r\n", __func__, ##__VA_ARGS__)
#else
#define NVR_LOG_INFO(fmt, ...)
#endif

#if NVR_LOGLEVEL > 0
#define NVR_LOG_ERROR(fmt, ...) OSAL_printf("%s: ERROR: " fmt "\r\n", __func__, ##__VA_ARGS__)
#else
#define NVR_LOG_ERROR(fmt, ...)
#endif

// driver finish function type
typedef int32_t (*NVR_drvFini_t)(void);

typedef enum NVR_err {
    NVR_ERR_OK = 0,
    NVR_ERR_BUSY,
    NVR_ERR_ERROR,
    NVR_ERR_NOTAVAIL,
    NVR_ERR_IO,
    NVR_ERR_INVAL
} NVR_err_t;

typedef enum NVR_type {
    NVR_TYPE_NONE = 0,
    NVR_TYPE_GW_PARAM,
    NVR_TYPE_IOL_PORT,
} NVR_type_t;

enum {
    NVR_MODE_OVERWRITE = 1,
    NVR_MODE_APPEND = 2
};

/******************** Public Interface Functions **********************/

extern NVR_err_t NVR_init(struct lfs_config *plfscfg);
extern NVR_err_t NVR_fini(int32_t (*drv_fini)(void));
extern NVR_err_t NVR_read(const char * const name, uint32_t * const pLen, const int32_t offset, void * const pData);
extern NVR_err_t NVR_write(const char * const name, const int32_t mode, const uint32_t len, const void * const pData);
extern NVR_err_t NVR_delete(const char * const name);
extern NVR_err_t NVR_erase(void);
extern NVR_err_t NVR_list(const char * const path);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NVRAM_H_ */
