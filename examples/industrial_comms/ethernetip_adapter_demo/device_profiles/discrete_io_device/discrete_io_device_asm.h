/*!
 *  \file discrete_io_device_asm.h
 *
 *  \brief
 *  Discrete I/O device profile Assembly instance definitions.
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

#ifndef DISCRETE_IO_DEVICE_ASM_H
#define DISCRETE_IO_DEVICE_ASM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "EI_API.h"
#include "EI_API_def.h"

#define DIO_DOG_CONFIGUARTION_INSTANCE   1   //!< the DOG instance to be used when output configuration assembly is called

typedef enum DIO_DEVICE_ASM_instances
{
    //- Input Points Without Status Bit ------------------------------------------
    DIO_DEVICE_ASM_1_Input_Point_No_Status = 1,
    DIO_DEVICE_ASM_2_Input_Point_No_Status = 2,
    DIO_DEVICE_ASM_4_Input_Point_No_Status = 3,
    DIO_DEVICE_ASM_8_Input_Point_No_Status = 4,
    DIO_DEVICE_ASM_16_Input_Point_No_Status = 5,
    DIO_DEVICE_ASM_32_Input_Point_No_Status = 6,
    DIO_DEVICE_ASM_N_Input_Point_No_Status = 7,
    //- Input Points With Single Status Bit ---------------------------------------
    DIO_DEVICE_ASM_1_Input_Point_Single_Status = 11,
    DIO_DEVICE_ASM_2_Input_Point_Single_Status = 12,
    DIO_DEVICE_ASM_4_Input_Point_Single_Status = 13,
    DIO_DEVICE_ASM_8_Input_Point_Single_Status = 14,
    DIO_DEVICE_ASM_16_Input_Point_Single_Status = 15,
    DIO_DEVICE_ASM_32_Input_Point_Single_Status = 16,
    DIO_DEVICE_ASM_N_Input_Point_Single_Status = 17,
    //- Input Points With Multiple Status Bit --------------------------------------
    DIO_DEVICE_ASM_1_Input_Point_Multiple_Status = 21,
    DIO_DEVICE_ASM_2_Input_Point_Multiple_Status = 22,
    DIO_DEVICE_ASM_4_Input_Point_Multiple_Status = 23,
    DIO_DEVICE_ASM_8_Input_Point_Multiple_Status = 24,
    DIO_DEVICE_ASM_16_Input_Point_Multiple_Status = 25,
    DIO_DEVICE_ASM_32_Input_Point_Multiple_Status = 26,
    DIO_DEVICE_ASM_N_Input_Point_Multiple_Status = 27,
    //- Output Points ---------------------------------------------------------------
    DIO_DEVICE_ASM_1_Output_Point = 31,
    DIO_DEVICE_ASM_2_Output_Point = 32,
    DIO_DEVICE_ASM_4_Output_Point = 33,
    DIO_DEVICE_ASM_8_Output_Point = 34,
    DIO_DEVICE_ASM_16_Output_Point = 35,
    DIO_DEVICE_ASM_32_Output_Point = 36,
    DIO_DEVICE_ASM_N_Output_Point = 37,
    //- Output Configuration ---------------------------------------------------------
    DIO_DEVICE_ASM_Output_Configuration  = 40,
    //- Output Status ----------------------------------------------------------------
    DIO_DEVICE_ASM_1_Output_Status = 41,
    DIO_DEVICE_ASM_2_Output_Status = 42,
    DIO_DEVICE_ASM_4_Output_Status = 43,
    DIO_DEVICE_ASM_8_Output_Status = 44,
    DIO_DEVICE_ASM_16_Output_Status = 45,
    DIO_DEVICE_ASM_32_Output_Status = 46,
    DIO_DEVICE_ASM_N_Output_Status = 47,
    //-Input Points with Single input status and single output status -----------------
    DIO_DEVICE_ASM_2_Input_Point_Single_Input_Status_Single_Output_Status = 52,
    DIO_DEVICE_ASM_4_Input_Point_Single_Input_Status_Single_Output_Status = 53,
    DIO_DEVICE_ASM_8_Input_Point_Single_Input_Status_Single_Output_Status = 54,
    DIO_DEVICE_ASM_16_Input_Point_Single_Input_Status_Single_Output_Status = 55,
    DIO_DEVICE_ASM_32_Input_Point_Single_Input_Status_Single_Output_Status = 56,
    DIO_DEVICE_ASM_N_Input_Point_Single_Input_Status_Single_Output_Status = 57,
    //-Input Points with Multiple input status and Multiple output status --------------
    DIO_DEVICE_ASM_2_Input_Point_Multiple_Input_Status_Multiple_Output_Status = 62,
    DIO_DEVICE_ASM_4_Input_Point_Multiple_Input_Status_Multiple_Output_Status = 63,
    DIO_DEVICE_ASM_8_Input_Point_Multiple_Input_Status_Multiple_Output_Status = 64,
    DIO_DEVICE_ASM_16_Input_Point_Multiple_Input_Status_Multiple_Output_Status = 65,
    //- Inpput Points with Single input status and various Output status ---------------
    DIO_DEVICE_ASM_1_Input_Point_Single_Input_Status_1_Output_Status = 70,
    DIO_DEVICE_ASM_2_Input_Point_Single_Input_Status_1_Output_Status = 71,
    DIO_DEVICE_ASM_2_Input_Point_Single_Input_Status_2_Output_Status = 72,
    DIO_DEVICE_ASM_4_Input_Point_Single_Input_Status_2_Output_Status = 73,
    DIO_DEVICE_ASM_4_Input_Point_Single_Input_Status_4_Output_Status = 74,
    DIO_DEVICE_ASM_8_Input_Point_Single_Input_Status_4_Output_Status = 75,
    DIO_DEVICE_ASM_8_Input_Point_Single_Input_Status_8_Output_Status = 76,
    DIO_DEVICE_ASM_16_Input_Point_Single_Input_Status_8_Output_Status = 77,
    DIO_DEVICE_ASM_16_Input_Point_Single_Input_Status_16_Output_Status = 78
}DIO_DEVICE_ASM_instances_t;

typedef enum DIO_DEVICE_ASM_ReturnCodes
{
    DIO_DEVICE_ASM_Ok = 0,
    DIO_DEVICE_ASM_NotSupported,
    DIO_DEVICE_ASM_CipError,
    DIO_DEVICE_ASM_AlreadyInitialized,
}DIO_DEVICE_ASM_ReturnCodes_t;

/**
 * \brief initializes the given assembly instance with all the necessary mapping
 * \param[in] pCipNode pointer to CIP node
 * \param[in] asmType the pre-defined assembly instance (refer to specification for more details)
 * \param[in] memberCount in case of using an assembly instance with N number of members, the member count should be given here.
 * \note the assemblies with variable member count is currently not supported, the parameter @memberCount is only meant for future compatibility.
 * \return returns the status of operation via DIO_DEVICE_ASM_ReturnCodes_t
*/
DIO_DEVICE_ASM_ReturnCodes_t DIO_DEVICE_ASM_initInstance(EI_API_CIP_NODE_T* pCipNode, DIO_DEVICE_ASM_instances_t asmType, uint16_t memberCount);


#ifdef __cplusplus
}
#endif

#endif // DISCRETE_IO_DEVICE_ASM_H
