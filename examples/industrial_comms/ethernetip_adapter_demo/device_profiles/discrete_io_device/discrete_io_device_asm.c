/*!
 *  \file discrete_io_device_asm.c
 *
 *  \brief
 *  Discrete I/O device profile Assembly instance.
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

#include <stdbool.h>
#include <osal.h>

#include "EI_API.h"
#include "EI_API_def.h"
#include "EI_API_CIP_define.h"

#include "device_profiles/discrete_io_device/discrete_io_device_asm.h"
#include "device_profiles/discrete_io_device/discrete_io_device_dip.h"
#include "device_profiles/discrete_io_device/discrete_io_device_dop.h"
#include "device_profiles/discrete_io_device/discrete_io_device_dog.h"



//----------- Private typedefs ----------
typedef struct DIO_DEVICE_ASM_container
{
    struct
    {
        bool     isInitialized;
        uint16_t input_point_count;
        uint16_t input_point_assembly_lenght;
    }inputPoint;
    struct
    {
        bool     isInitialized;
        uint16_t output_point_count;
        uint16_t output_point_assembly_lenght;
    }outputPoint;
    struct
    {
        EI_API_CIP_NODE_T *pCipNode;
        bool isInitialized;
    }outputConfiguration;
    struct
    {
        bool     isInitialized;
        uint16_t input_single_status_count;
        uint16_t input_single_status_assembly_lenght;
    }inputSingleStatus;
    struct
    {
        bool     isInitialized;
        uint16_t output_single_status_count;
        uint16_t output_single_status_assembly_lenght;
    }outputSingleStatus;
    struct
    {
        bool     isInitialized;
        bool     hasInputMultipleStatus;
    }inputMultipleStatus;
    struct
    {
        bool     isInitialized;
        bool     hasOutputMultipleStatus;
    }outputMultipleStatus;
}DIO_DEVICE_ASM_container_t;


#define DIO_DEVICE_ASM_Assert(v) \
{   \
    if(EI_API_CIP_eERR_OK != v) \
    {   \
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0); \
    }   \
} \


static DIO_DEVICE_ASM_container_t DIO_DEVICE_ASM_handle = {0};

//-------------------------------------------------------------------------------------------

static EI_API_CIP_EAssemb_Return_Code_t DIO_DEVICE_ASM_inputPointCustomMapCb( const EI_API_CIP_SAssemMapData_t *pAttrMapData, EI_API_CIP_STransferBuffer_t *pProduceBuffer)
{
    EI_API_CIP_EAssemb_Return_Code_t retValue = ASSEMB_SERVICE_NO_RESPONSE;
    uint8_t produceBufferIndex = 0;
    uint8_t memberMapPosition = 0;
    ei_api_cip_edt_bool value = 0;

    if((0x08 == pAttrMapData->classId))
    {
        //OSAL_printf("inside inputPointMap AttrAddr %p \n", pAttrMapData->pAttribBuf);
        memberMapPosition = (pAttrMapData->instanceId - 1) % 8;
        produceBufferIndex = (pAttrMapData->instanceId - 1) / 8;

        if(EI_API_CIP_eERR_OK == DIO_DEVICE_DIP_getObjValue( pAttrMapData->instanceId, pAttrMapData->attributeId, (void *)&value))
        {
            if(0 != value)
            {
                pProduceBuffer->p8uDataBuf[produceBufferIndex] |= (1<<memberMapPosition);
            }
            else
            {
                pProduceBuffer->p8uDataBuf[produceBufferIndex] &= ~(1<<memberMapPosition);
            }

            pProduceBuffer->u16uActData = DIO_DEVICE_ASM_handle.inputPoint.input_point_assembly_lenght;
            retValue = ASSEMB_SERVICE_RESPONSE_OK;
        }
    }

     return retValue;
}

//-----------------------------------------------------------------------------------------------------------------------------
static EI_API_CIP_EAssemb_Return_Code_t DIO_DEVICE_ASM_outputPointCustomMap_SetCb( EI_API_CIP_SAssemMapData_t *pAttrMapData, const EI_API_CIP_STransferBuffer_t *pConsumeBuffer)
{
    EI_API_CIP_EAssemb_Return_Code_t retValue = ASSEMB_SERVICE_NO_RESPONSE;
    uint8_t consumeBufferIndex = 0;
    uint8_t memberMapPosition = 0;
    volatile ei_api_cip_edt_bool value = 0;

    if((0x09 == pAttrMapData->classId))
    {
        memberMapPosition = (pAttrMapData->instanceId - 1) % 8;
        consumeBufferIndex = (pAttrMapData->instanceId - 1) / 8;
        /**
         * It is assumed that the O->T connection is using 32bit idle/run header
         */
        consumeBufferIndex = consumeBufferIndex + 4;
        if(0x00 == pConsumeBuffer->p8uDataBuf[0])
        {
            DIO_DEVICE_DOP_receiveIdleEvent(pAttrMapData->instanceId); //Idle Command
            retValue = ASSEMB_SERVICE_RESPONSE_OK;
        }
        else
        {
            if(pConsumeBuffer->p8uDataBuf[consumeBufferIndex] & (1<<memberMapPosition))
            {
                value = 1;
            }
            else
            {
                value = 0;
            }
            if(EI_API_CIP_eERR_OK == DIO_DEVICE_DOP_receiveDataEvent(pAttrMapData->instanceId, value))
            {
                pAttrMapData->pAttribBuf[0] = value;
                retValue = ASSEMB_SERVICE_RESPONSE_OK;
            }
        }
    }

    return retValue;
}
//---------------------------------------------------------------------------------------------------------------
EI_API_CIP_EAssemb_Return_Code_t DIO_DEVICE_ASM_outputPointCustomMap_GetCb( const EI_API_CIP_SAssemMapData_t *pAttrMapData, EI_API_CIP_STransferBuffer_t *pProduceBuffer)
{
    EI_API_CIP_EAssemb_Return_Code_t retValue = ASSEMB_SERVICE_NO_RESPONSE;
    uint8_t produceBufferIndex = 0;
    uint8_t memberMapPosition = 0;
    uint8_t attrValue = 0;

    if((0x09 == pAttrMapData->classId))
    {
        memberMapPosition = (pAttrMapData->instanceId - 1) % 8;
        produceBufferIndex = (pAttrMapData->instanceId - 1) / 8;

        if(EI_API_CIP_eERR_OK == DIO_DEVICE_DOP_getObjValue( pAttrMapData->instanceId, 3, (void *)&attrValue))
        {
            if(0 != attrValue)
            {
                pProduceBuffer->p8uDataBuf[produceBufferIndex] |= (1<<memberMapPosition);
            }
            else
            {
                pProduceBuffer->p8uDataBuf[produceBufferIndex] &= ~(1<<memberMapPosition);
            }

            pProduceBuffer->u16uActData = DIO_DEVICE_ASM_handle.outputPoint.output_point_assembly_lenght;
            retValue = ASSEMB_SERVICE_RESPONSE_OK;
        }
    }

     return retValue;
}
//---------------------------------------------------------------------------------------------------------------
static DIO_DEVICE_ASM_ReturnCodes_t DIO_DEVICE_ASM_createInputPoint_Members(EI_API_CIP_NODE_T* pCipNode, DIO_DEVICE_ASM_instances_t type)
{
    uint32_t errCode;
    DIO_DEVICE_ASM_ReturnCodes_t retval = DIO_DEVICE_ASM_Ok;
    EI_API_CIP_SAssemCustomMap_t assembCustomMapCfg = {0};
    static const uint8_t countLUT[] = {1, 2, 4, 8, 16, 32};

    if((type >= DIO_DEVICE_ASM_1_Input_Point_No_Status) && (type <= DIO_DEVICE_ASM_32_Input_Point_No_Status))
    {
        errCode = EI_API_CIP_createAssembly(pCipNode, type, EI_API_CIP_eAR_GET);
        DIO_DEVICE_ASM_Assert(errCode);

        DIO_DEVICE_ASM_handle.inputPoint.input_point_count = countLUT[type - DIO_DEVICE_ASM_1_Input_Point_No_Status];

        switch (type)
        {
        case DIO_DEVICE_ASM_1_Input_Point_No_Status ... DIO_DEVICE_ASM_8_Input_Point_No_Status:
            DIO_DEVICE_ASM_handle.inputPoint.input_point_assembly_lenght = 1;
            break;
        case DIO_DEVICE_ASM_16_Input_Point_No_Status:
            DIO_DEVICE_ASM_handle.inputPoint.input_point_assembly_lenght = 2;
            break;
        case DIO_DEVICE_ASM_32_Input_Point_No_Status:
            DIO_DEVICE_ASM_handle.inputPoint.input_point_assembly_lenght = 4;
            break;
        default:
            break;
        }

        for(uint16_t i=0; i<DIO_DEVICE_ASM_handle.inputPoint.input_point_count; i++)
        {

            assembCustomMapCfg.assemblyMemberLength = (0 == i) ? DIO_DEVICE_ASM_handle.inputPoint.input_point_assembly_lenght : 0;
            assembCustomMapCfg.mappedClassId = 0x08;
            assembCustomMapCfg.mappedInstanceId = i + 1;
            assembCustomMapCfg.mappedAttributeId = 0x03; //value attribute of DIP object
            assembCustomMapCfg.fuCustomGet = DIO_DEVICE_ASM_inputPointCustomMapCb;
            assembCustomMapCfg.fuCustomSet = NULL;
            errCode = EI_API_CIP_addAssemblyMemberCustomMapped(pCipNode, type, &assembCustomMapCfg);
            DIO_DEVICE_ASM_Assert(errCode);
        }

        DIO_DEVICE_ASM_handle.inputPoint.isInitialized = true;
    }
    else
    {
        retval = DIO_DEVICE_ASM_NotSupported;
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
static DIO_DEVICE_ASM_ReturnCodes_t DIO_DEVICE_ASM_createOutputPoint_Members(EI_API_CIP_NODE_T* pCipNode, DIO_DEVICE_ASM_instances_t type)
{
    uint32_t errCode;
    DIO_DEVICE_ASM_ReturnCodes_t retval = DIO_DEVICE_ASM_Ok;
    EI_API_CIP_SAssemCustomMap_t assembCustomMapCfg = {0};
    static const uint8_t countLUT[] = {1, 2, 4, 8, 16, 32};

    if((type >= DIO_DEVICE_ASM_1_Output_Point) && (type <= DIO_DEVICE_ASM_32_Output_Point))
    {
        errCode = EI_API_CIP_createAssembly(pCipNode, type, EI_API_CIP_eAR_GET_AND_SET);
        DIO_DEVICE_ASM_Assert(errCode);

        DIO_DEVICE_ASM_handle.outputPoint.output_point_count = countLUT[type - DIO_DEVICE_ASM_1_Output_Point];

        switch (type)
        {
        case DIO_DEVICE_ASM_1_Output_Point ... DIO_DEVICE_ASM_8_Output_Point:
            DIO_DEVICE_ASM_handle.outputPoint.output_point_assembly_lenght = 1;
            break;
        case DIO_DEVICE_ASM_16_Output_Point:
            DIO_DEVICE_ASM_handle.outputPoint.output_point_assembly_lenght = 2;
            break;
        case DIO_DEVICE_ASM_32_Output_Point:
            DIO_DEVICE_ASM_handle.outputPoint.output_point_assembly_lenght = 4;
            break;
        default:
            break;
        }

        errCode = EI_API_CIP_setAssemblyFormat(pCipNode, type, ASSEMB_FORMAT_32BITHEADER);
        DIO_DEVICE_ASM_Assert(errCode);

        for(uint16_t i=0; i<DIO_DEVICE_ASM_handle.outputPoint.output_point_count; i++)
        {
            assembCustomMapCfg.assemblyMemberLength = (0 == i) ? (DIO_DEVICE_ASM_handle.outputPoint.output_point_assembly_lenght)  : 0;
            assembCustomMapCfg.mappedClassId = 0x09;
            assembCustomMapCfg.mappedInstanceId = i + 1;
            assembCustomMapCfg.mappedAttributeId = 0x03; //value attribute of DOP object
            assembCustomMapCfg.fuCustomGet = DIO_DEVICE_ASM_outputPointCustomMap_GetCb;
            assembCustomMapCfg.fuCustomSet = DIO_DEVICE_ASM_outputPointCustomMap_SetCb;
            errCode = EI_API_CIP_addAssemblyMemberCustomMapped(pCipNode, type, &assembCustomMapCfg);
            DIO_DEVICE_ASM_Assert(errCode);
        }

        DIO_DEVICE_ASM_handle.outputPoint.isInitialized = true;
    }
    else
    {
        retval = DIO_DEVICE_ASM_NotSupported;
    }

    return retval;
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t DIO_DEVICE_ASM_outputConfigCustomMap_SetCb(
                                    EI_API_CIP_NODE_T* pCipNode,
                                    uint16_t           o2t,
                                    uint16_t           t2o,
                                    uint16_t           configurationInstance,
                                    const uint8_t*     configurationData,
                                    uint16_t           configurationSize)
{
    ei_api_cip_edt_bool faultAction = 0;
    ei_api_cip_edt_bool idleAction = 0;

    //See Specification V1.3.37, 6-10.9 Output Configuration Assembly Data Attribute format
    if(configurationData[0] & 0x01)
    {
        faultAction = 1;
    }
    if(configurationData[0] & 0x02)
    {
        idleAction = 1;
    }

    DIO_DEVICE_DOG_receiveAssemblyConfig(DIO_DOG_CONFIGUARTION_INSTANCE, faultAction, idleAction);

    return EI_API_eERR_CB_NO_ERROR;
}
//---------------------------------------------------------------------------------------------------------------------
static EI_API_CIP_EAssemb_Return_Code_t DIO_DEVICE_ASM_outputConfigCustomMap_GetCb( const EI_API_CIP_SAssemMapData_t *pAttrMapData, EI_API_CIP_STransferBuffer_t *pProduceBuffer)
{
    EI_API_CIP_EAssemb_Return_Code_t retValue = ASSEMB_SERVICE_NO_RESPONSE;
    static const uint16_t atrrIDs[2] = {7, 9};
    ei_api_cip_edt_bool actionValues[2] = {0,0};
    uint32_t errCode;
    ei_api_cip_edt_bool value = 0;

    if((0x1E == pAttrMapData->classId))
    {
        for(uint8_t i = 0; i<2; i++)
        {
            errCode = EI_API_CIP_getAttr_bool(DIO_DEVICE_ASM_handle.outputConfiguration.pCipNode, 0x1E, pAttrMapData->instanceId, atrrIDs[i], &actionValues[i]);
            DIO_DEVICE_ASM_Assert(errCode);
            if(0 != value)
            {
                pProduceBuffer->p8uDataBuf[0] |= (1<<i);
            }
            else
            {
                pProduceBuffer->p8uDataBuf[0] &= ~(1<<i);
            }
        }
        pProduceBuffer->u16uActData = 1;
        retValue = ASSEMB_SERVICE_RESPONSE_OK;
    }

    return retValue;
}
//------------------------------------------------------------------------------------------------------------------------------
static DIO_DEVICE_ASM_ReturnCodes_t DIO_DEVICE_ASM_createOutputConfiguration(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    DIO_DEVICE_ASM_ReturnCodes_t retval = DIO_DEVICE_ASM_Ok;
    EI_API_CIP_SAssemCustomMap_t assembCustomMapCfg = {0};
    static const uint16_t dog_attr_array[] = {7, 9};
    if(EI_API_CIP_eERR_OK == EI_API_CIP_createAssembly(pCipNode, DIO_DEVICE_ASM_Output_Configuration, EI_API_CIP_eAR_GET))
    {
        /* Create configuration assembly */
        if(EI_API_CIP_eERR_OK == EI_API_CIP_setCfgAssemblyCb(
                                    pCipNode,
                                    DIO_DEVICE_ASM_Output_Configuration,
                                    DIO_DEVICE_ASM_outputConfigCustomMap_SetCb))
        {
            for(uint16_t i=0; i<2; i++)
            {
                assembCustomMapCfg.assemblyMemberLength = (0==i) ? 1 : 0;
                assembCustomMapCfg.fuCustomGet = DIO_DEVICE_ASM_outputConfigCustomMap_GetCb;
                assembCustomMapCfg.fuCustomSet = NULL;
                assembCustomMapCfg.mappedInstanceId = 1;
                assembCustomMapCfg.mappedClassId = 0x1E;
                assembCustomMapCfg.mappedAttributeId = dog_attr_array[i];
                errCode = EI_API_CIP_addAssemblyMemberCustomMapped(pCipNode, DIO_DEVICE_ASM_Output_Configuration, &assembCustomMapCfg);
                DIO_DEVICE_ASM_Assert(errCode);
            }

            DIO_DEVICE_ASM_handle.outputConfiguration.isInitialized = true;
        }
        else
        {
            retval = DIO_DEVICE_ASM_CipError;
        }
    }
    else
    {
        retval = DIO_DEVICE_ASM_CipError;
    }

    return retval;
}

//----------------------------------------------------------------------------------------------------------------------
DIO_DEVICE_ASM_ReturnCodes_t DIO_DEVICE_ASM_initInstance(EI_API_CIP_NODE_T* pCipNode, DIO_DEVICE_ASM_instances_t asmType, uint16_t memberCount)
{
    DIO_DEVICE_ASM_ReturnCodes_t retval = DIO_DEVICE_ASM_Ok;
    (void)memberCount; //the memberCount is ignored, as the variable member count is not supported

    switch (asmType)
    {
    case DIO_DEVICE_ASM_N_Input_Point_No_Status:
    case DIO_DEVICE_ASM_N_Input_Point_Single_Status:
    case DIO_DEVICE_ASM_N_Input_Point_Multiple_Status:
    case DIO_DEVICE_ASM_N_Output_Point:
    case DIO_DEVICE_ASM_N_Output_Status:
    case DIO_DEVICE_ASM_N_Input_Point_Single_Input_Status_Single_Output_Status:
        retval = DIO_DEVICE_ASM_NotSupported; //not supported
        break;
    case DIO_DEVICE_ASM_1_Input_Point_No_Status ... DIO_DEVICE_ASM_32_Input_Point_No_Status:
        if(false != DIO_DEVICE_ASM_handle.inputPoint.isInitialized)
        {
            retval = DIO_DEVICE_ASM_AlreadyInitialized; //already initialized
        }
        else
        {
            retval = DIO_DEVICE_ASM_createInputPoint_Members(pCipNode, asmType);
        }
        break;
    case DIO_DEVICE_ASM_1_Input_Point_Single_Status ... DIO_DEVICE_ASM_32_Input_Point_Single_Status:
        if(false != DIO_DEVICE_ASM_handle.inputSingleStatus.isInitialized)
        {
            retval = DIO_DEVICE_ASM_AlreadyInitialized; //already initialized
        }
        break;
    case DIO_DEVICE_ASM_1_Input_Point_Multiple_Status ... DIO_DEVICE_ASM_32_Input_Point_Multiple_Status:
        if(false != DIO_DEVICE_ASM_handle.inputMultipleStatus.isInitialized)
        {
            retval = DIO_DEVICE_ASM_AlreadyInitialized;
        }
        break;
    case DIO_DEVICE_ASM_1_Output_Point ... DIO_DEVICE_ASM_32_Output_Point:
        if(false != DIO_DEVICE_ASM_handle.outputPoint.isInitialized)
        {
            retval = DIO_DEVICE_ASM_AlreadyInitialized;
        }
        else
        {
            retval = DIO_DEVICE_ASM_createOutputPoint_Members(pCipNode, asmType);
        }
        break;
    case DIO_DEVICE_ASM_1_Output_Status ... DIO_DEVICE_ASM_32_Output_Status:
        if(false != DIO_DEVICE_ASM_handle.outputSingleStatus.isInitialized)
        {
            retval = DIO_DEVICE_ASM_AlreadyInitialized;
        }
        break;
    case DIO_DEVICE_ASM_2_Input_Point_Single_Input_Status_Single_Output_Status ... DIO_DEVICE_ASM_32_Input_Point_Single_Input_Status_Single_Output_Status:
        if((false != DIO_DEVICE_ASM_handle.outputMultipleStatus.isInitialized) ||
           (false != DIO_DEVICE_ASM_handle.inputMultipleStatus.isInitialized)  ||
           (false != DIO_DEVICE_ASM_handle.inputPoint.isInitialized))
           {
                retval = DIO_DEVICE_ASM_AlreadyInitialized;
           }
        break;
    case DIO_DEVICE_ASM_2_Input_Point_Multiple_Input_Status_Multiple_Output_Status ... DIO_DEVICE_ASM_16_Input_Point_Multiple_Input_Status_Multiple_Output_Status:
        if((false != DIO_DEVICE_ASM_handle.outputSingleStatus.isInitialized) ||
            (false != DIO_DEVICE_ASM_handle.inputSingleStatus.isInitialized)  ||
            (false != DIO_DEVICE_ASM_handle.inputPoint.isInitialized))
            {
                retval = DIO_DEVICE_ASM_AlreadyInitialized;
            }
        break;
    case DIO_DEVICE_ASM_1_Input_Point_Single_Input_Status_1_Output_Status ... DIO_DEVICE_ASM_16_Input_Point_Single_Input_Status_16_Output_Status:
            if((false != DIO_DEVICE_ASM_handle.outputSingleStatus.isInitialized) ||
            (false != DIO_DEVICE_ASM_handle.inputMultipleStatus.isInitialized)  ||
            (false != DIO_DEVICE_ASM_handle.inputPoint.isInitialized))
            {
                retval = DIO_DEVICE_ASM_AlreadyInitialized;
            }
        break;
    case DIO_DEVICE_ASM_Output_Configuration:
        if(false != DIO_DEVICE_ASM_handle.outputConfiguration.isInitialized)
        {
            retval = DIO_DEVICE_ASM_AlreadyInitialized; //already initialized
        }
        else
        {
            DIO_DEVICE_ASM_handle.outputConfiguration.pCipNode = pCipNode;
            retval = DIO_DEVICE_ASM_createOutputConfiguration(pCipNode);
        }
        break;
    default:
        break;
    }

    return retval;

}
