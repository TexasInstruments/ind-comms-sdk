/*!
 *  \file app_discrete_io_device_asm.h
 *
 *  \brief
 *  Discrete I/O device profile Assembly instance definitions.
 *
 *  \author
 *  Texas Instruments Incorporated
 *
 *  \copyright
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

/*!
 *
 *  \file app_discrete_io_device_asm.h
 *
 *  \brief
 *  Contains the function prototype and typedef struct realted to Assemblies for Discrete I/O device
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br><br>
 *  @KUNBUS_LICENSE@
 *
 */

#ifndef APP_DISCRETE_IO_DEVICE_ASM_H
#define APP_DISCRETE_IO_DEVICE_ASM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "EI_API.h"
#include "EI_API_def.h"

#define EI_APP_DIO_DOG_CONFIGUARTION_INSTANCE   1   //!< the DOG instance to be used when output configuration assembly is called

typedef enum EI_APP_DIO_DEVICE_Assembly_instances
{
    //- Input Points Without Status Bit ------------------------------------------
    EI_APP_DIO_DEVICE_1_Input_Point_No_Status = 1,
    EI_APP_DIO_DEVICE_2_Input_Point_No_Status = 2,
    EI_APP_DIO_DEVICE_4_Input_Point_No_Status = 3,
    EI_APP_DIO_DEVICE_8_Input_Point_No_Status = 4,
    EI_APP_DIO_DEVICE_16_Input_Point_No_Status = 5,
    EI_APP_DIO_DEVICE_32_Input_Point_No_Status = 6,
    EI_APP_DIO_DEVICE_N_Input_Point_No_Status = 7,
    //- Input Points With Single Status Bit ---------------------------------------
    EI_APP_DIO_DEVICE_1_Input_Point_Single_Status = 11,
    EI_APP_DIO_DEVICE_2_Input_Point_Single_Status = 12,
    EI_APP_DIO_DEVICE_4_Input_Point_Single_Status = 13,
    EI_APP_DIO_DEVICE_8_Input_Point_Single_Status = 14,
    EI_APP_DIO_DEVICE_16_Input_Point_Single_Status = 15,
    EI_APP_DIO_DEVICE_32_Input_Point_Single_Status = 16,
    EI_APP_DIO_DEVICE_N_Input_Point_Single_Status = 17,
    //- Input Points With Multiple Status Bit --------------------------------------
    EI_APP_DIO_DEVICE_1_Input_Point_Multiple_Status = 21,
    EI_APP_DIO_DEVICE_2_Input_Point_Multiple_Status = 22,
    EI_APP_DIO_DEVICE_4_Input_Point_Multiple_Status = 23,
    EI_APP_DIO_DEVICE_8_Input_Point_Multiple_Status = 24,
    EI_APP_DIO_DEVICE_16_Input_Point_Multiple_Status = 25,
    EI_APP_DIO_DEVICE_32_Input_Point_Multiple_Status = 26,
    EI_APP_DIO_DEVICE_N_Input_Point_Multiple_Status = 27,
    //- Output Points ---------------------------------------------------------------
    EI_APP_DIO_DEVICE_1_Output_Point = 31,
    EI_APP_DIO_DEVICE_2_Output_Point = 32,
    EI_APP_DIO_DEVICE_4_Output_Point = 33,
    EI_APP_DIO_DEVICE_8_Output_Point = 34,
    EI_APP_DIO_DEVICE_16_Output_Point = 35,
    EI_APP_DIO_DEVICE_32_Output_Point = 36,
    EI_APP_DIO_DEVICE_N_Output_Point = 37,
    //- Output Configuration ---------------------------------------------------------
    EI_APP_DIO_OUTPUT_CONFIGURATION  = 40,
    //- Output Status ----------------------------------------------------------------
    EI_APP_DIO_DEVICE_1_Output_Status = 41,
    EI_APP_DIO_DEVICE_2_Output_Status = 42,
    EI_APP_DIO_DEVICE_4_Output_Status = 43,
    EI_APP_DIO_DEVICE_8_Output_Status = 44,
    EI_APP_DIO_DEVICE_16_Output_Status = 45,
    EI_APP_DIO_DEVICE_32_Output_Status = 46,
    EI_APP_DIO_DEVICE_N_Output_Status = 47,
    //-Input Points with Single input status and single output status -----------------
    EI_APP_DIO_DEVICE_2_Input_Point_Single_Input_Status_Single_Output_Status = 52,
    EI_APP_DIO_DEVICE_4_Input_Point_Single_Input_Status_Single_Output_Status = 53,
    EI_APP_DIO_DEVICE_8_Input_Point_Single_Input_Status_Single_Output_Status = 54,
    EI_APP_DIO_DEVICE_16_Input_Point_Single_Input_Status_Single_Output_Status = 55,
    EI_APP_DIO_DEVICE_32_Input_Point_Single_Input_Status_Single_Output_Status = 56,
    EI_APP_DIO_DEVICE_N_Input_Point_Single_Input_Status_Single_Output_Status = 57,
    //-Input Points with Multiple input status and Multiple output status --------------
    EI_APP_DIO_DEVICE_2_Input_Point_Multiple_Input_Status_Multiple_Output_Status = 62,
    EI_APP_DIO_DEVICE_4_Input_Point_Multiple_Input_Status_Multiple_Output_Status = 63,
    EI_APP_DIO_DEVICE_8_Input_Point_Multiple_Input_Status_Multiple_Output_Status = 64,
    EI_APP_DIO_DEVICE_16_Input_Point_Multiple_Input_Status_Multiple_Output_Status = 65,
    //- Inpput Points with Single input status and various Output status ---------------
    EI_APP_DIO_DEVICE_1_Input_Point_Single_Input_Status_1_Output_Status = 70,
    EI_APP_DIO_DEVICE_2_Input_Point_Single_Input_Status_1_Output_Status = 71,
    EI_APP_DIO_DEVICE_2_Input_Point_Single_Input_Status_2_Output_Status = 72,
    EI_APP_DIO_DEVICE_4_Input_Point_Single_Input_Status_2_Output_Status = 73,
    EI_APP_DIO_DEVICE_4_Input_Point_Single_Input_Status_4_Output_Status = 74,
    EI_APP_DIO_DEVICE_8_Input_Point_Single_Input_Status_4_Output_Status = 75,
    EI_APP_DIO_DEVICE_8_Input_Point_Single_Input_Status_8_Output_Status = 76,
    EI_APP_DIO_DEVICE_16_Input_Point_Single_Input_Status_8_Output_Status = 77,
    EI_APP_DIO_DEVICE_16_Input_Point_Single_Input_Status_16_Output_Status = 78
}EI_APP_DIO_DEVICE_Assembly_instances_t;

typedef enum EI_APP_DIO_DEVICE_Assembly_ReturnCodes
{
    EI_APP_DIO_DEVICE_Assembly_OK = 0,
    EI_APP_DIO_DEVICE_Assembly_NotSupported,
    EI_APP_DIO_DEVICE_Assembly_CIP_Error,
    EI_APP_DIO_DEVICE_Assembly_Already_Initialized,
}EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t;

/**
 * \brief initializes the given assembly instance with all the necessary mapping
 * \param[in] pCipNode pointer to CIP node
 * \param[in] asmType the pre-defined assembly instance (refer to specification for more details)
 * \param[in] memberCount in case of using an assembly instance with N number of members, the member count should be given here.
 * \note the assemblies with variable member count is currently not supported, the parameter @memberCount is only meant for future compatibility.
 * \return returns the status of operation via EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t
*/
EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t EI_APP_ASM_InitInstance(EI_API_CIP_NODE_T* pCipNode, EI_APP_DIO_DEVICE_Assembly_instances_t asmType, uint16_t memberCount);


#ifdef __cplusplus
}
#endif

#endif
