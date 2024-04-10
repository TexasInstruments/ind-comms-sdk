/*!
 *  \file ecSlvApiDef_SoE.h
 *
 *  \brief
 *  SoE defines.
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

#if !(defined __ECSLVAPIDEF_SOE_H__)
#define __ECSLVAPIDEF_SOE_H__		1



#if (defined __cplusplus)
extern "C" {
#endif


/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

/**
* \addtogroup SoE SoE Service and Flags
* @{
*/
/*---------------------------------------------
-    SOE services
-----------------------------------------------*/
#define    ECAT_SOE_OPCODE_RRQ      0x0001    /**< \brief SoE Read Request*/
#define    ECAT_SOE_OPCODE_RRS      0x0002    /**< \brief SoE Read Response*/
#define    ECAT_SOE_OPCODE_WRQ      0x0003    /**< \brief SoE Write Request*/
#define    ECAT_SOE_OPCODE_WRS      0x0004    /**< \brief SoE Write Response*/
#define    ECAT_SOE_OPCODE_NFC      0x0005    /**< \brief SoE Notification Request*/
#define    ECAT_SOE_OPCODE_EMCY     0x0006    /**< \brief SoE Emergency*/

/*---------------------------------------------
-    SOE flags
-----------------------------------------------*/
#define    SOEFLAGS_OPCODE          0x0007    /**< \brief SoE Flags*/
                                               /**<
                                               * 0 = unused<br>
                                               * 1 = readReq<br>
                                               * 2 = readRes<br>
                                               * 3 = writeReq<br>
                                               * 4 = writeRes<br>
                                               * 5 = notification (command changed notification)*/
#define    SOEFLAGS_INCOMPLETE      0x0008    /**< \brief more follows*/
#define    SOEFLAGS_ERROR           0x0010    /**< \brief an error word follows*/
#define    SOEFLAGS_DRIVENO         0x00E0    /**< \brief drive number*/

#define    SOEFLAGS_DATASTATE       0x0100    /**< \brief Data state follows or requested*/
#define    SOEFLAGS_NAME            0x0200    /**< \brief Name follows or requested*/
#define    SOEFLAGS_ATTRIBUTE       0x0400    /**< \brief Attribute follows or requested*/
#define    SOEFLAGS_UNIT            0x0800    /**< \brief Unit follows or requested*/
#define    SOEFLAGS_MIN             0x1000    /**< \brief Min value follows or requested*/
#define    SOEFLAGS_MAX             0x2000    /**< \brief Max value follows or requested*/
#define    SOEFLAGS_VALUE           0x4000    /**< \brief Value follows or requested*/
#define    SOEFLAGS_DEFAULT         0x8000    /**< \brief Default value*/
/** @}*/


#if (defined __cplusplus)
}
#endif

#endif /* __ECSLVAPIDEF_SOE_H__ */
