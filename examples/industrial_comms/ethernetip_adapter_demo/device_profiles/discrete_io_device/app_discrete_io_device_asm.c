
#include <stdbool.h>
#include <osal.h>
#include "EI_API.h"
#include "EI_API_def.h"
#include "EI_API_CIP_define.h"
#include "app_discrete_io_device_asm.h"
#include "app_discrete_io_device_dip.h"
#include "app_discrete_io_device_dop.h"
#include "app_discrete_io_device_dog.h"



//----------- Private typedefs ----------
typedef struct EI_APP_ASM_container
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
}EI_APP_ASM_container_t;


#define EI_APP_ASM_Assert(v) \
{   \
    if(EI_API_CIP_eERR_OK != v) \
    {   \
        OSAL_error (__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0); \
    }   \
} \


static EI_APP_ASM_container_t EI_APP_ASM_handle = {0};

//-------------------------------------------------------------------------------------------

static EI_API_CIP_EAssemb_Return_Code_t EI_APP_ASM_inputPointCustomMapCb( const EI_API_CIP_SAssemMapData_t *pAttrMapData, EI_API_CIP_STransferBuffer_t *pProduceBuffer)
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

        if(EI_API_CIP_eERR_OK == EI_APP_DIP_getObjValue( pAttrMapData->instanceId, pAttrMapData->attributeId, (void *)&value))
        {
            if(0 != value)
            {
                pProduceBuffer->p8uDataBuf[produceBufferIndex] |= (1<<memberMapPosition);
            }
            else
            {
                pProduceBuffer->p8uDataBuf[produceBufferIndex] &= ~(1<<memberMapPosition);
            }

            pProduceBuffer->u16uActData = EI_APP_ASM_handle.inputPoint.input_point_assembly_lenght;
            retValue = ASSEMB_SERVICE_RESPONSE_OK;
        }
    }

     return retValue;
}

//-----------------------------------------------------------------------------------------------------------------------------
static EI_API_CIP_EAssemb_Return_Code_t EI_APP_ASM_outputPointCustomMap_SetCb( EI_API_CIP_SAssemMapData_t *pAttrMapData, const EI_API_CIP_STransferBuffer_t *pConsumeBuffer)
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
            EI_APP_DOP_receiveIdleEvent(pAttrMapData->instanceId); //Idle Command
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
            if(EI_API_CIP_eERR_OK == EI_APP_DOP_receiveDataEvent(pAttrMapData->instanceId, value))
            {
                pAttrMapData->pAttribBuf[0] = value;
                retValue = ASSEMB_SERVICE_RESPONSE_OK;
            }
        }
    }

    return retValue;
}
//---------------------------------------------------------------------------------------------------------------
EI_API_CIP_EAssemb_Return_Code_t EI_APP_ASM_outputPointCustomMap_GetCb( const EI_API_CIP_SAssemMapData_t *pAttrMapData, EI_API_CIP_STransferBuffer_t *pProduceBuffer)
{
    EI_API_CIP_EAssemb_Return_Code_t retValue = ASSEMB_SERVICE_NO_RESPONSE;

    if((0x09 == pAttrMapData->classId))
    {
        pProduceBuffer->u16uActData = EI_APP_ASM_handle.outputPoint.output_point_assembly_lenght;
        retValue = ASSEMB_SERVICE_RESPONSE_OK;
    }

     return retValue;
}
//---------------------------------------------------------------------------------------------------------------
static EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t EI_APP_ASM_createInputPoint_Members(EI_API_CIP_NODE_T* pCipNode, EI_APP_DIO_DEVICE_Assembly_instances_t type)
{
    uint32_t errCode;
    EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t retval = EI_APP_DIO_DEVICE_Assembly_OK;
    EI_API_CIP_SAssemCustomMap_t assembCustomMapCfg = {0};
    static const uint8_t countLUT[] = {1, 2, 4, 8, 16, 32};

    if((type >= EI_APP_DIO_DEVICE_1_Input_Point_No_Status) && (type <= EI_APP_DIO_DEVICE_32_Input_Point_No_Status))
    {
        errCode = EI_API_CIP_createAssembly(pCipNode, type, EI_API_CIP_eAR_GET);
        EI_APP_ASM_Assert(errCode);

        EI_APP_ASM_handle.inputPoint.input_point_count = countLUT[type - EI_APP_DIO_DEVICE_1_Input_Point_No_Status];

        switch (type)
        {
        case EI_APP_DIO_DEVICE_1_Input_Point_No_Status ... EI_APP_DIO_DEVICE_8_Input_Point_No_Status:
            EI_APP_ASM_handle.inputPoint.input_point_assembly_lenght = 1;
            break;
        case EI_APP_DIO_DEVICE_16_Input_Point_No_Status:
            EI_APP_ASM_handle.inputPoint.input_point_assembly_lenght = 2;
            break;
        case EI_APP_DIO_DEVICE_32_Input_Point_No_Status:
            EI_APP_ASM_handle.inputPoint.input_point_assembly_lenght = 4;
            break;
        default:
            break;
        }

        for(uint16_t i=0; i<EI_APP_ASM_handle.inputPoint.input_point_count; i++)
        {

            assembCustomMapCfg.assemblyMemberLength = (0 == i) ? EI_APP_ASM_handle.inputPoint.input_point_assembly_lenght : 0;
            assembCustomMapCfg.mappedClassId = 0x08;
            assembCustomMapCfg.mappedInstanceId = i + 1;
            assembCustomMapCfg.mappedAttributeId = 0x03; //value attribute of DIP object
            assembCustomMapCfg.fuCustomGet = EI_APP_ASM_inputPointCustomMapCb;
            assembCustomMapCfg.fuCustomSet = NULL;
            errCode = EI_API_CIP_addAssemblyMemberCustomMapped(pCipNode, type, &assembCustomMapCfg);
            EI_APP_ASM_Assert(errCode);
        }

        EI_APP_ASM_handle.inputPoint.isInitialized = true;
    }
    else
    {
        retval = EI_APP_DIO_DEVICE_Assembly_NotSupported;
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
static EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t EI_APP_ASM_createOutputPoint_Members(EI_API_CIP_NODE_T* pCipNode, EI_APP_DIO_DEVICE_Assembly_instances_t type)
{
    uint32_t errCode;
    EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t retval = EI_APP_DIO_DEVICE_Assembly_OK;
    EI_API_CIP_SAssemCustomMap_t assembCustomMapCfg = {0};
    static const uint8_t countLUT[] = {1, 2, 4, 8, 16, 32};

    if((type >= EI_APP_DIO_DEVICE_1_Output_Point) && (type <= EI_APP_DIO_DEVICE_32_Output_Point))
    {
        errCode = EI_API_CIP_createAssembly(pCipNode, type, EI_API_CIP_eAR_GET_AND_SET);
        EI_APP_ASM_Assert(errCode);

        EI_APP_ASM_handle.outputPoint.output_point_count = countLUT[type - EI_APP_DIO_DEVICE_1_Output_Point];

        switch (type)
        {
        case EI_APP_DIO_DEVICE_1_Output_Point ... EI_APP_DIO_DEVICE_8_Output_Point:
            EI_APP_ASM_handle.outputPoint.output_point_assembly_lenght = 1;
            break;
        case EI_APP_DIO_DEVICE_16_Output_Point:
            EI_APP_ASM_handle.outputPoint.output_point_assembly_lenght = 2;
            break;
        case EI_APP_DIO_DEVICE_32_Output_Point:
            EI_APP_ASM_handle.outputPoint.output_point_assembly_lenght = 4;
            break;
        default:
            break;
        }

        errCode = EI_API_CIP_setAssemblyFormat(pCipNode, type, ASSEMB_FORMAT_32BITHEADER);
        EI_APP_ASM_Assert(errCode);

        for(uint16_t i=0; i<EI_APP_ASM_handle.outputPoint.output_point_count; i++)
        {
            assembCustomMapCfg.assemblyMemberLength = (0 == i) ? (EI_APP_ASM_handle.outputPoint.output_point_assembly_lenght)  : 0;
            assembCustomMapCfg.mappedClassId = 0x09;
            assembCustomMapCfg.mappedInstanceId = i + 1;
            assembCustomMapCfg.mappedAttributeId = 0x03; //value attribute of DOP object
            assembCustomMapCfg.fuCustomGet = EI_APP_ASM_outputPointCustomMap_GetCb;
            assembCustomMapCfg.fuCustomSet = EI_APP_ASM_outputPointCustomMap_SetCb;
            errCode = EI_API_CIP_addAssemblyMemberCustomMapped(pCipNode, type, &assembCustomMapCfg);
            EI_APP_ASM_Assert(errCode);
        }

        EI_APP_ASM_handle.outputPoint.isInitialized = true;
    }
    else
    {
        retval = EI_APP_DIO_DEVICE_Assembly_NotSupported;
    }

    return retval;
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t EI_APP_ASM_OutputConfigCustomMap_SetCb(
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

    EI_APP_DOG_receiveASMConfig(EI_APP_DIO_DOG_CONFIGUARTION_INSTANCE, faultAction, idleAction);

    return EI_API_eERR_CB_NO_ERROR;
}
//---------------------------------------------------------------------------------------------------------------------
static EI_API_CIP_EAssemb_Return_Code_t EI_APP_ASM_OutputConfigCustomMap_GetCb( const EI_API_CIP_SAssemMapData_t *pAttrMapData, EI_API_CIP_STransferBuffer_t *pProduceBuffer)
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
            errCode = EI_API_CIP_getAttr_bool(EI_APP_ASM_handle.outputConfiguration.pCipNode, 0x1E, pAttrMapData->instanceId, atrrIDs[i], &actionValues[i]);
            EI_APP_ASM_Assert(errCode);
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
static EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t EI_APP_ASM_createOutputConfiguration(EI_API_CIP_NODE_T* pCipNode)
{
    uint32_t errCode;
    EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t retval = EI_APP_DIO_DEVICE_Assembly_OK;
    EI_API_CIP_SAssemCustomMap_t assembCustomMapCfg = {0};
    static const uint16_t dog_attr_array[] = {7, 9};
    if(EI_API_CIP_eERR_OK == EI_API_CIP_createAssembly(pCipNode, EI_APP_DIO_OUTPUT_CONFIGURATION, EI_API_CIP_eAR_GET))
    {
        /* Create configuration assembly */
        if(EI_API_CIP_eERR_OK == EI_API_CIP_setCfgAssemblyCb(
                                    pCipNode,
                                    EI_APP_DIO_OUTPUT_CONFIGURATION,
                                    EI_APP_ASM_OutputConfigCustomMap_SetCb))
        {
            for(uint16_t i=0; i<2; i++)
            {
                assembCustomMapCfg.assemblyMemberLength = (0==i) ? 1 : 0;
                assembCustomMapCfg.fuCustomGet = EI_APP_ASM_OutputConfigCustomMap_GetCb;
                assembCustomMapCfg.fuCustomSet = NULL;
                assembCustomMapCfg.mappedInstanceId = 1;
                assembCustomMapCfg.mappedClassId = 0x1E;
                assembCustomMapCfg.mappedAttributeId = dog_attr_array[i];
                errCode = EI_API_CIP_addAssemblyMemberCustomMapped(pCipNode, EI_APP_DIO_OUTPUT_CONFIGURATION, &assembCustomMapCfg);
                EI_APP_ASM_Assert(errCode);
            }

            EI_APP_ASM_handle.outputConfiguration.isInitialized = true;
        }
        else
        {
            retval = EI_APP_DIO_DEVICE_Assembly_CIP_Error;
        }
    }
    else
    {
        retval = EI_APP_DIO_DEVICE_Assembly_CIP_Error;
    }

    return retval;
}

//----------------------------------------------------------------------------------------------------------------------
EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t EI_APP_ASM_InitInstance(EI_API_CIP_NODE_T* pCipNode, EI_APP_DIO_DEVICE_Assembly_instances_t asmType, uint16_t memberCount)
{
    EI_APP_DIO_DEVICE_Assembly_ReturnCodes_t retval = EI_APP_DIO_DEVICE_Assembly_OK;
    (void)memberCount; //the memberCount is ignored, as the variable member count is not supported

    switch (asmType)
    {
    case EI_APP_DIO_DEVICE_N_Input_Point_No_Status:
    case EI_APP_DIO_DEVICE_N_Input_Point_Single_Status:
    case EI_APP_DIO_DEVICE_N_Input_Point_Multiple_Status:
    case EI_APP_DIO_DEVICE_N_Output_Point:
    case EI_APP_DIO_DEVICE_N_Output_Status:
    case EI_APP_DIO_DEVICE_N_Input_Point_Single_Input_Status_Single_Output_Status:
        retval = EI_APP_DIO_DEVICE_Assembly_NotSupported; //not supported
        break;
    case EI_APP_DIO_DEVICE_1_Input_Point_No_Status ... EI_APP_DIO_DEVICE_32_Input_Point_No_Status:
        if(false != EI_APP_ASM_handle.inputPoint.isInitialized)
        {
            retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized; //already initialized
        }
        else
        {
            retval = EI_APP_ASM_createInputPoint_Members(pCipNode, asmType);
        }
        break;
    case EI_APP_DIO_DEVICE_1_Input_Point_Single_Status ... EI_APP_DIO_DEVICE_32_Input_Point_Single_Status:
        if(false != EI_APP_ASM_handle.inputSingleStatus.isInitialized)
        {
            retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized; //already initialized
        }
        break;
    case EI_APP_DIO_DEVICE_1_Input_Point_Multiple_Status ... EI_APP_DIO_DEVICE_32_Input_Point_Multiple_Status:
        if(false != EI_APP_ASM_handle.inputMultipleStatus.isInitialized)
        {
            retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized;
        }
        break;
    case EI_APP_DIO_DEVICE_1_Output_Point ... EI_APP_DIO_DEVICE_32_Output_Point:
        if(false != EI_APP_ASM_handle.outputPoint.isInitialized)
        {
            retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized;
        }
        else
        {
            retval = EI_APP_ASM_createOutputPoint_Members(pCipNode, asmType);
        }
        break;
    case EI_APP_DIO_DEVICE_1_Output_Status ... EI_APP_DIO_DEVICE_32_Output_Status:
        if(false != EI_APP_ASM_handle.outputSingleStatus.isInitialized)
        {
            retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized;
        }
        break;
    case EI_APP_DIO_DEVICE_2_Input_Point_Single_Input_Status_Single_Output_Status ... EI_APP_DIO_DEVICE_32_Input_Point_Single_Input_Status_Single_Output_Status:
        if((false != EI_APP_ASM_handle.outputMultipleStatus.isInitialized) ||
           (false != EI_APP_ASM_handle.inputMultipleStatus.isInitialized)  ||
           (false != EI_APP_ASM_handle.inputPoint.isInitialized))
           {
                retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized;
           }
        break;
    case EI_APP_DIO_DEVICE_2_Input_Point_Multiple_Input_Status_Multiple_Output_Status ... EI_APP_DIO_DEVICE_16_Input_Point_Multiple_Input_Status_Multiple_Output_Status:
        if((false != EI_APP_ASM_handle.outputSingleStatus.isInitialized) ||
            (false != EI_APP_ASM_handle.inputSingleStatus.isInitialized)  ||
            (false != EI_APP_ASM_handle.inputPoint.isInitialized))
            {
                retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized;
            }
        break;
    case EI_APP_DIO_DEVICE_1_Input_Point_Single_Input_Status_1_Output_Status ... EI_APP_DIO_DEVICE_16_Input_Point_Single_Input_Status_16_Output_Status:
            if((false != EI_APP_ASM_handle.outputSingleStatus.isInitialized) ||
            (false != EI_APP_ASM_handle.inputMultipleStatus.isInitialized)  ||
            (false != EI_APP_ASM_handle.inputPoint.isInitialized))
            {
                retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized;
            }
        break;
    case EI_APP_DIO_OUTPUT_CONFIGURATION:
        if(false != EI_APP_ASM_handle.outputConfiguration.isInitialized)
        {
            retval = EI_APP_DIO_DEVICE_Assembly_Already_Initialized; //already initialized
        }
        else
        {
            EI_APP_ASM_handle.outputConfiguration.pCipNode = pCipNode;
            retval = EI_APP_ASM_createOutputConfiguration(pCipNode);
        }
        break;
    default:
        break;
    }

    return retval;

}
