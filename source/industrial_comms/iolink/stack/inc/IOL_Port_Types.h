/*!
 *  \file IOL_Port_Types.h
 *
 *  \brief
 *  IO-Link Port Types.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *  SPDX-License-Identifier: LicenseRef-Texas Instruments Incorporated
 *  All rights reserved.
 */

#ifndef INC_PROT__IOL_PORT_TYPES_H__
#define INC_PROT__IOL_PORT_TYPES_H__

/**
\addtogroup group_iol_port_types IO-Link port types
\{
*/

#include <stdint.h>


#ifndef NULL
#define NULL  ((void*)0)
#endif

#ifndef FALSE
#define FALSE ((TBOOL)0)
#endif

#ifndef TRUE
#define TRUE  ((TBOOL)1)
#endif

typedef char CHAR8; /**< \brief 8 bit character data type */

typedef uint8_t TBOOL; /**< \brief Boolean data type (at least 1 bit) */

typedef int8_t INT8S; /**< \brief 8 bit signed integer */

typedef int16_t INT16S; /**< \brief 16 bit signed integer */

typedef int32_t INT32S; /**< \brief 32 bit signed integer */

typedef uint8_t INT8U; /**< \brief 8 bit unsigned integer */

typedef uint16_t INT16U; /**< \brief 16 bit unsigned integer */

typedef uint32_t INT32U; /**< \brief 32 bit unsigned integer */

typedef float FLOAT32; /**< \brief 32 bit float */

typedef double FLOAT64; /**< \brief 64 bit float */

/** \} */

#endif
