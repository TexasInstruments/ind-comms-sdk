/*!
 *  \example EtherCAT_Slave_Simple.c
 *
 *  \brief
 *  EtherCAT<sup>&reg;</sup> Slave Example Application.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2021, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 *  Copyright (c) 2023 KUNBUS GmbH.
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

#define TIESC_HW	0
#define PRU_200MHZ  1

/* this one shall the only one to be using it ! */
#include "project.h"
#include "ecSlvSimple.h"

#if (defined DPRAM_REMOTE) && (DPRAM_REMOTE==1)
 #include <DPR_Api.h>
 /* DPR implementation */
 #if (defined DPRIMPL_SHAREDRAM) && (DPRIMPL_SHAREDRAM==1)
  /* Shared RAM not TriPort */
  #include <dprLib_sharedMemory.h>
 #endif
#elif (defined FBTL_REMOTE) && (FBTL_REMOTE==1)
#if (defined FBTLIMPL_LINEUART) && (1==FBTLIMPL_LINEUART)
 #include "sysLib_lineUart.h"
#else
 #include <sysLib_sharedMemory.h>
#endif
#endif

/* OS */
#include <osal.h>

/* ESL */
#include <ESL_os.h>
#include <ESL_BOARD_config.h>
#include <ESL_version.h>

/* stack */
#include <ecSlvApi.h>

/* @cppcheck_justify{misra-c2012-8.9} we prefer moduleGlobal over threadSafety error */
/* cppcheck-suppress misra-c2012-8.9 */
static OSAL_PJumpBuf_t  ECTT_farJumpBuf;

static uint32_t EC_SLV_APP_CTT_remoteInit(EC_SLV_APP_CTT_Application_t *applicationInstance);

/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  Application Loop Task
 *
 *  <!-- Parameters and return values: -->
 *
 *  \param[in]  pArg_p      Task Parm.
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *
 *  // required variables
 *  void* pvVariable = NULL;
 *
 *  // the Call
 *  EC_SLV_APP_CTT_loopTask(pvVariable);
 *  \endcode
 *
 *  <!-- Group: -->
 *
 *  \ingroup EC_SLV_APP
 *
 * */
static void EC_SLV_APP_CTT_loopTask(void* pArg_p)
{
    /* @cppcheck_justify{misra-c2012-11.5} thread void api requires cast */
    /* cppcheck-suppress misra-c2012-11.5 */
    EC_SLV_APP_CTT_Application_t*   pApplicationInstance    = (EC_SLV_APP_CTT_Application_t*)pArg_p;
    uint32_t                        error                   = EC_API_eERR_NONE;

    if (!pApplicationInstance)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    EC_SLV_APP_CTT_initBoardFunctions(pApplicationInstance);
    EC_SLV_APP_CTT_registerStacklessBoardFunctions(pApplicationInstance);

    error = EC_API_SLV_stackInit(); // EtherCAT stack init
    if (error != EC_API_eERR_NONE)
    {
        OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, error);
        OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    EC_SLV_APP_CTT_applicationInit(pApplicationInstance);

    const uint32_t vendorId = EC_API_SLV_getVendorId(pApplicationInstance->ptEcSlvApi);
    char * const pProductName = EC_API_SLV_getProductName(pApplicationInstance->ptEcSlvApi);
    OSAL_printf("%s - %xh\r\n", pProductName, vendorId);

    /* spit out versions */
    ESL_dumpVersions();

    for(;;)
    {
        EC_API_SLV_mainLoopCyclic();
        if (EC_API_SLV_eESM_init < EC_API_SLV_getState())
        {
            OSAL_SCHED_yield();
        }
        else
        {
            /* for carve up give some air */
            OSAL_SCHED_sleep(10);
        }
    }
    // not reachable
    //EC_SLV_APP_applicationDeInit();
Exit:
    ESL_OS_taskLeave();

    return;
}

static void EC_SLV_APP_CTT_mainTask(void* pArg_p)
{
    /* @cppcheck_justify{misra-c2012-11.5} thread api signature requires cast */
    /* cppcheck-suppress misra-c2012-11.5 */
    EC_SLV_APP_CTT_Application_t*   applicationInstance = (EC_SLV_APP_CTT_Application_t*)pArg_p;
    uint32_t                        retVal              = OSAL_ERR_NoError;

    if (!applicationInstance)
    {
        OSAL_error(
            __func__,
            __LINE__,
            OSAL_ERR_InvalidParm,
            true,
            1,
            "Application instance missing!!!\r\n");
    }

    retVal = ESL_OS_boardInit(applicationInstance->selectedPruInstance);
    if(OSAL_ERR_NoError != retVal)
    {
        OSAL_error(__func__, __LINE__, retVal, true, 1, "OS Board init error\r\n");
    }

    OSAL_registerPrintOut(NULL, ESL_OS_printf);

    retVal = EC_SLV_APP_CTT_remoteInit(applicationInstance);

    if(!((OSAL_CONTAINER_LOCALIMPLEMENTATION == retVal) || (OSAL_ERR_NoError == retVal)))
    {
        OSAL_error(__func__, __LINE__, OSAL_ERR_InvalidParm, true, 1, "Fatal error remote API init!!!\r\n");
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

    retVal = EC_API_SLV_load(&ECTT_farJumpBuf, NULL /* &applErrHandler*/, applicationInstance->selectedPruInstance);

    if(0u == retVal)
    {
        EC_API_SLV_prepareTasks(KBECSLV_PRIO_PDI, KBECSLV_PRIO_LED, KBECSLV_PRIO_SYNC0, KBECSLV_PRIO_SYNC1, KBECSLV_PRIO_EOE);

        applicationInstance->loopThreadHandle = OSAL_SCHED_startTask(EC_SLV_APP_CTT_loopTask
                                                                    ,applicationInstance
#if (!(defined FBTL_REMOTE) || (0==FBTL_REMOTE))
                                                                    ,OSAL_TASK_Prio_Normal
#else
                                                                    ,OSAL_TASK_Prio_Idle
#endif
                                                                    ,(uint8_t*)EC_SLV_APP_applLoopTaskStack_g
                                                                    ,APPLLOOP_TASK_SIZE_BYTE
                                                                    ,0
                                                                    ,"Appl_LoopTask");
        if ( NULL == applicationInstance->loopThreadHandle)
        {
            OSAL_printf("Error return start Loop Task\r\n");
            OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
        }

        OSAL_SCHED_joinTask(applicationInstance->loopThreadHandle);

        /* No default tasks (run forever) here, so introduce a wait cycle */
        for (;;)
        {
            OSAL_SCHED_sleep(1000);
        }
    }
    else
    {
        for (;;)
        {

            for (;;)
            {
                OSAL_printf("long jmp restart\r\n");
                OSAL_SCHED_sleep(1000);
            }
        }
    }


    // not reachable

    //ESL_OS_boardDeinit();
    //EC_API_SLV_unLoad();

Exit:
    return;
}

/*!
 *  \brief
 *  Main entry point.
 *
 *  \details
 *  Simple EtherCAT Slave example demonstrating the configuration the hardware,
 *  creation of the base slave information as well as the Object Dictionary
 *  and the Process Data configuration. Furthermore, the FoE Protocol and the
 *  EEPROM handling are covered on this example.
 *
 *  \param[in]  argc                            Number of command line arguments.
 *  \param[in]  *argv                           Pointer to an array of command line arguments.
 *
 *  \return     uint32_t of type #NAMESPACE_EError_t.
 *  \retval     0                               Success.
 *  \retval     -1                              Board initialization failed.
 *
 */
int main(int argc, char *argv[])
{
    static
    /* @cppcheck_justify{threadsafety-threadsafety} main is singleton, so accept threadsafety */
    /* cppcheck-suppress threadsafety-threadsafety */
    uint32_t                        selectedPruInstance     = UINT32_MAX;
    static
    /* @cppcheck_justify{threadsafety-threadsafety} main is singleton, so accept threadsafety */
    /* cppcheck-suppress threadsafety-threadsafety */
    EC_SLV_APP_CTT_Application_t    applicationInstance     = {0};
    uint32_t                        error                   = OSAL_ERR_NoError;

    ESL_OS_init();

    selectedPruInstance = ESL_DEFAULT_PRUICSS;

    if (1 < argc)
    {
        char* inst = argv[1];
        selectedPruInstance = strtoul(inst, NULL, 0);
    }

    /* Init osal */
    OSAL_init();

    applicationInstance.selectedPruInstance = selectedPruInstance;

    TaskP_Params_init(&applicationInstance.mainThreadParam);

    applicationInstance.mainThreadParam.name        = "mainThread";
    applicationInstance.mainThreadParam.stackSize   = MAIN_TASK_SIZE_BYTE; /* FreeRTOS wants Stacksize in Units, when called direct, using TaskP it's bytes */
    applicationInstance.mainThreadParam.stack       = (uint8_t*)EC_SLV_APP_mainTaskStack_g;
    applicationInstance.mainThreadParam.priority    = (TaskP_PRIORITY_HIGHEST-1);
    applicationInstance.mainThreadParam.taskMain    = (TaskP_FxnMain)EC_SLV_APP_CTT_mainTask;
    applicationInstance.mainThreadParam.args        = (void*)&applicationInstance;

    error = TaskP_construct(&applicationInstance.mainThreadHandle
                           ,&applicationInstance.mainThreadParam);
    if (SystemP_SUCCESS != error)
    {
        OSAL_printf("Error setting create thread of %s (%ld)\r\n", applicationInstance.mainThreadParam.name, error);
        OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
    }

    OSAL_startOs();

    OSAL_error(__func__, __LINE__, OSAL_ERR_InvalidParm, true, 1, "Not reachable by design!!!\r\n");

    // not reachable
    //EC_API_SLV_unLoad();

    return error;
}

static uint32_t EC_SLV_APP_CTT_remoteInit(EC_SLV_APP_CTT_Application_t *applicationInstance)
{
    uint32_t retVal = OSAL_CONTAINER_LOCALIMPLEMENTATION;

    if (!applicationInstance)
    {
        /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
        /* cppcheck-suppress misra-c2012-15.1 */
        goto Exit;
    }

#if (defined DPRAM_REMOTE) && (DPRAM_REMOTE==1)
    retVal = DPRLIB_init(applicationInstance->gpioHandle, false);
#elif (defined FBTL_REMOTE) && (FBTL_REMOTE==1)
#if (defined FBTLIMPL_LINEUART) && (FBTLIMPL_LINEUART==1)
    retVal = SYSLIB_createLibInstanceLine(  FBTL_LINE_UART_NAME,
                                            FBTL_MAX_ASYNC_LEN, FBTL_MAX_ASYNC_LEN,
                                            FBTL_MAX_PD_LEN, FBTL_MAX_PD_LEN
#if (defined FBTLTRACECALLS) && (FBTLTRACECALLS==1)
                                      ,true
#else
                                      ,false
#endif
                                      ,&(applicationInstance->remoteHandle)
    );
#else
    retVal = SYSLIB_createLibInstanceShm(FBTLSHARED_MEM_NAME, FBTLSHARED_MEM_SIZE, false,
                                      FBTL_MAX_ASYNC_LEN, FBTL_MAX_ASYNC_LEN, FBTL_MAX_PD_LEN, FBTL_MAX_PD_LEN
#if (defined FBTLTRACECALLS) && (FBTLTRACECALLS==1)
            ,true
#else
            ,false
#endif
            ,&(applicationInstance->remoteHandle)
    );
#endif
#else
    OSALUNREF_PARM(applicationInstance);
    retVal = OSAL_CONTAINER_LOCALIMPLEMENTATION;
#endif

    if(OSAL_CONTAINER_LOCALIMPLEMENTATION == retVal)
    {
        OSAL_printf("\r\nLocal Implementation\r\n");
    }
#if (defined DPRAM_REMOTE) && (DPRAM_REMOTE==1)
        else if(EC_API_eERR_NONE == retVal)
    {
        OSAL_printf("\r\nDPRAM usage\r\n");

        retVal = EC_API_SLV_DPR_configuration(dprRamBase(), dprRamSize());
        if(EC_API_eERR_NONE != retVal)
        {
            OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, retVal);
            OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
            /* cppcheck-suppress misra-c2012-15.1 */
            goto Exit;
        }
    }
#elif (defined FBTL_REMOTE) && (FBTL_REMOTE==1)
    else if(EC_API_eERR_NONE == retVal)
    {
        retVal = EC_API_SLV_FBTL_configuration(applicationInstance->remoteHandle);
        if(EC_API_eERR_NONE != retVal)
        {
            OSAL_printf("%s:%d Error code: 0x%08x\r\n", __func__, __LINE__, retVal);
            OSAL_error(__func__, __LINE__, OSAL_STACK_INIT_ERROR, true, 0);
            /* @cppcheck_justify{misra-c2012-15.1} goto is used to assure single point of exit */
            /* cppcheck-suppress misra-c2012-15.1 */
            goto Exit;
        }
    }
#endif
    else
    {
        /* nothing to do */
        ;
    }

    Exit:
    if((OSAL_ERR_NoError != retVal) && (OSAL_CONTAINER_LOCALIMPLEMENTATION != retVal))
    {
        OSAL_printf("%s:%d::> err ret = 0x%x\r\n", __func__, __LINE__, retVal);
    }
    return retVal;
}
