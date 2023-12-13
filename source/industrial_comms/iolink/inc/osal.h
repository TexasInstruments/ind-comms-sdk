/*!
 *  \file osal.h
 *
 *  \brief
 *  OS abstraction layer (generic) interface.
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  @KUNBUS_LICENSE@
 *
 */

#if !(defined PROTECT_OSAL_H)
#define PROTECT_OSAL_H  1

/* >= C99 required */
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>

#include "osal_error.h"

#define BACKWARD_COMPATIBILITY 1

#if (defined OSAL_LINUX) || (defined OSAL_TIRTOS) || \
    (defined OSAL_FREERTOS) || (defined OSAL_FREERTOS_JACINTO)
#else
#error "Unknown OS selected. Implement OS specific abstraction Layer."
#endif



#if defined _WIN32 || defined __CYGWIN__
#define OSAL_DLL_IMPORT __declspec(dllimport)
#define OSAL_DLL_EXPORT __declspec(dllexport)
#define OSAL_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define OSAL_DLL_IMPORT __attribute__ ((visibility ("default")))
#define OSAL_DLL_EXPORT __attribute__ ((visibility ("default")))
#define OSAL_DLL_LOCAL __attribute__ ((visibility ("hidden")))
#else
#define OSAL_DLL_IMPORT
#define OSAL_DLL_EXPORT
#define OSAL_DLL_LOCAL
#endif
#endif
/* defined if ECATSLV is compiled as a DLL */
#if ((defined ECATSLAVE_SO) && (ECATSLAVE_SO==1) || ((defined ETHERNETIP_SO) && \
     (ETHERNETIP_SO==1)) || ((defined PROFINETIO_SO) && (PROFINETIO_SO==1)))

/* defined if we are building the ECATSLAVE DLL (instead of using it) */
#if (defined OSALAPI_EXPORTS) || (defined ECSLVAPI_EXPORTS)
#define OSAL_API OSAL_DLL_EXPORT
#else
#define OSAL_API OSAL_DLL_IMPORT
#endif /* ECSLVAPI_EXPORTS */
#define OSAL_LOC OSAL_DLL_LOCAL
#else /* ECATSLAVE_SO is not defined: this means ECATSLAVE is a static lib. */
#define OSAL_API
#define OSAL_LOC
#endif /* ECATSLAVE_SO */


#if (defined __GNUC__) || (defined __TI_ARM__)
#define OSAL_STRUCT_PACKED __attribute__((__packed__))
#define OSAL_FUNC_NORETURN __attribute__((noreturn))
#define OSAL_FUNC_UNUSED __attribute__((unused))
#define OSAL_FUNC_NOINLINE __attribute__((noinline))

#elif defined (MSVCC)
#else
#ifndef __attribute__
    /* @cppcheck_justify{misra-c2012-21.1} some compilers do not provide the attribute macro */
    /* cppcheck-suppress misra-c2012-21.1 */
    #define __attribute__(x)
#endif
#define OSAL_STRUCT_PACKED
#define OSAL_FUNC_NORETURN __attribute__((noreturn))
#define OSAL_FUNC_UNUSED __attribute__((unused))
#define OSAL_FUNC_NOINLINE __attribute__((noinline))
#endif

#define OSAL_WAIT_INFINITE (UINT32_MAX)

#define OSALUNREF_PARM(x) \
    (void)(x)

#if (defined __GNUC__) || (defined __TI_ARM__)
#define OSAL_ASSERT_DMB() __asm("    dmb")
#define OSAL_ASSERT_DSB() __asm("    dsb")
#else
#define OSAL_ASSERT_DMB()
#define OSAL_ASSERT_DSB()
#endif

#define OSAL_TICKS_IN_MILLI_SEC 1 /* 1000us tick */

#define UINT32REGISTERGET(base, offset) \
    (((volatile uint32_t*)&(((volatile uint8_t*)(base))[(offset)]))[0]);

#define UINT32REGISTERSET(base, offset, value) \
    ((volatile uint32_t*)&(((volatile uint8_t*)(base))[(offset)]))[0] = (value);

#define OSAL_ERROR_CLASS(x) \
    (0x39000000|((x)&0xffff))
#define OSAL_OS_ERROR_CLASS(x) \
    (0x39010000|((x)&0xffff))

/*!
 *  \brief
 *  OSAL error codes.
 *  \ingroup OSALAPI
 */
typedef enum OSAL_Error
{
    OSAL_ERR_NoError = 0,                               /*!< No error, everything is fine. */
    OSAL_ERR_NoEntity = OSAL_ERROR_CLASS(2),            /*!< No such file or directory. */
    OSAL_ERR_NoDeviceOrAddr = OSAL_ERROR_CLASS(6),      /*!< No such device or address. */
    OSAL_ERR_NoMemory = OSAL_ERROR_CLASS(12),           /*!< Not enough memory. */
    OSAL_ERR_NoAccess = OSAL_ERROR_CLASS(13),           /*!< No access, no hardware license */
    OSAL_ERR_InvalidState = OSAL_ERROR_CLASS(14),       /*!< Invalid state. */
    OSAL_ERR_NoDirectory = OSAL_ERROR_CLASS(20),        /*!< No directory. */
    OSAL_ERR_InvalidParm = OSAL_ERROR_CLASS(22),        /*!< Invalid argument. */
    OSAL_ERR_NotImplemented = OSAL_ERROR_CLASS(34),     /*!< Not implemeented. */
    OSAL_ERR_Timedout = OSAL_ERROR_CLASS(110),          /*!< Operation or connection timed out. */
    OSAL_ERR_MutexInconsistent = OSAL_ERROR_CLASS(205), /*!< Mutex incionsistent */
} OSAL_Error_t;

#if (defined BACKWARD_COMPATIBILITY) && (1 == BACKWARD_COMPATIBILITY)
 #define OSAL_eERR_NOERROR OSAL_ERR_NoError
 #define OSAL_eERR_ENOENT OSAL_ERR_NoEntity
 #define OSAL_eERR_ENXIO OSAL_ERR_NoDeviceOrAddr
 #define OSAL_eERR_NOMEMORY OSAL_ERR_NoMemory
 #define OSAL_eERR_NOACCESS OSAL_ERR_NoAccess
 #define OSAL_eERR_INVALIDSTATE OSAL_ERR_InvalidState
 #define OSAL_eERR_EINVAL OSAL_ERR_InvalidParm
 #define OSAL_eERR_NOTIMPLEMENTED  OSAL_ERR_NotImplemented
 #define OSAL_eERR_TIMEDOUT OSAL_ERR_Timedout
 #define OSAL_eERR_MUTEXINCONSISTENT OSAL_ERR_MutexInconsistent
#endif

typedef enum OSAL_TASK_Priority
{
    OSAL_TASK_Prio_0 = 0, /* Lowest Priority = idle */
    OSAL_TASK_Prio_1,
    OSAL_TASK_Prio_2,
    OSAL_TASK_Prio_3,
    OSAL_TASK_Prio_4,
    OSAL_TASK_Prio_5,
    OSAL_TASK_Prio_6,
    OSAL_TASK_Prio_7,
    OSAL_TASK_Prio_8,
    OSAL_TASK_Prio_9,
    OSAL_TASK_Prio_10,
    OSAL_TASK_Prio_11,
    OSAL_TASK_Prio_12,
    OSAL_TASK_Prio_13,
    OSAL_TASK_Prio_14,
    OSAL_TASK_Prio_15,
    OSAL_TASK_Prio_16,
    OSAL_TASK_Prio_17,
    OSAL_TASK_Prio_18,
    OSAL_TASK_Prio_19,
    OSAL_TASK_Prio_20,
    OSAL_TASK_Prio_21,
    OSAL_TASK_Prio_22,
    OSAL_TASK_Prio_23,
    OSAL_TASK_Prio_24,
    OSAL_TASK_Prio_25,
    OSAL_TASK_Prio_26,
    OSAL_TASK_Prio_27,
    OSAL_TASK_Prio_28,
    OSAL_TASK_Prio_29,
    OSAL_TASK_Prio_30,
    OSAL_TASK_Prio_31, /* Highest (RT) priority */

    /// @cond INTERNAL
    OSAL_TASK_Prio_Force32Bit = (0x7fffffffu) /* 32Bit */
    /// @endcond
} OSAL_TASK_Priority_t;

/* CPPCHECK does not allow duplicates in enum list, alias definition lookalike enum */
/* Generic */                                               /* Linux    TiRTOS  FreeRTOS    */
#define OSAL_TASK_Prio_Lowest (OSAL_TASK_Prio_2)            /* 26       2       1           */
#define OSAL_TASK_Prio_Idle (OSAL_TASK_Prio_Lowest)         /* 26       2       1           */
#define OSAL_TASK_Prio_Normal (OSAL_TASK_Prio_8)            /* 38       8       4           */
#define OSAL_TASK_Prio_Highest (OSAL_TASK_Prio_30)          /* 82       30      15          */
#define OSAL_TASK_Prio_InterruptSub1 (OSAL_TASK_Prio_29)    /* 81       28      14          */
#define OSAL_TASK_Prio_Interrupt (OSAL_TASK_Prio_Highest)   /* 82       30      15          */
#define OSAL_TASK_Prio_OSALTimer (OSAL_TASK_Prio_Interrupt) /* 64       21      10          */

/* HWAL */
#define OSAL_TASK_Prio_HWAL_WATCHDOG (OSAL_TASK_Prio_Interrupt)
#define OSAL_TASK_Prio_HWAL_LICENSE (OSAL_TASK_Prio_Normal)

/* EtherCAT */
#define OSAL_TASK_Prio_ECEEPROM (OSAL_TASK_Prio_4)          /* 30       4       2           */
#define OSAL_TASK_Prio_ECRestart (OSAL_TASK_Prio_10)        /* 42       10      5           */
#define OSAL_TASK_Prio_ECReboot (OSAL_TASK_Prio_11)         /* 44       11      5           */
#define OSAL_TASK_Prio_ECLED (OSAL_TASK_Prio_8)             /* 38       8       4           */
#define OSAL_TASK_Prio_ECSync (OSAL_TASK_Prio_16)           /* 54       16      8           */
#define OSAL_TASK_Prio_ECEoE (OSAL_TASK_Prio_25)            /* 72       25      12          */
#define OSAL_TASK_Prio_ECPDI (OSAL_TASK_Prio_16)            /* 54       16      8           */
#define OSAL_TASK_Prio_ECDemoThread (OSAL_TASK_Prio_29)     /* 80       29      14          */

/* FBTL */
#define OSAL_TASK_Prio_FBTLSync (OSAL_TASK_Prio_Interrupt)
#define OSAL_TASK_Prio_FBTLAcycIST (OSAL_TASK_Prio_Interrupt)
#define OSAL_TASK_Prio_FBTLCyclic (OSAL_TASK_Prio_InterruptSub1)
#define OSAL_TASK_Prio_FBTLSendAcyc (OSAL_TASK_Prio_InterruptSub1)
#define OSAL_TASK_Prio_FBTLReceiver (OSAL_TASK_Prio_InterruptSub1)
#define OSAL_TASK_Prio_FBTLService (OSAL_TASK_Prio_Interrupt)
#define OSAL_TASK_Prio_FBTLSlowService (OSAL_TASK_Prio_Interrupt)
#define OSAL_TASK_Prio_FBTLOSISTCONT (OSAL_TASK_Prio_InterruptSub1)

/* DPR */
#define OSAL_TASK_Prio_DPRIRQ (OSAL_TASK_Prio_28)
#define OSAL_TASK_Prio_DPRRX (OSAL_TASK_Prio_26)
#define OSAL_TASK_Prio_DPRTX (OSAL_TASK_Prio_27)

/* IOL */
#define OSAL_TASK_Prio_IOL_LED (OSAL_TASK_Prio_Idle)
#define OSAL_TASK_Prio_IOL_SMI (OSAL_TASK_Prio_9)
#define OSAL_TASK_Prio_IOL_Example_Main (OSAL_TASK_Prio_Normal)
#define OSAL_TASK_Prio_IOL_Main (OSAL_TASK_Prio_25)
#define OSAL_TASK_Prio_IOL_NVRAM (OSAL_TASK_Prio_Normal)
#define OSAL_TASK_Prio_IOL_Port (OSAL_TASK_Prio_20)

/* EtherNet/IP */
#define OSAL_TASK_Prio_EIP_MAIN (OSAL_TASK_Prio_8)
#define OSAL_TASK_Prio_EIP_FLASH (OSAL_TASK_Prio_8)
#define OSAL_TASK_Prio_EIP_EEPROM (OSAL_TASK_Prio_8)
#define OSAL_TASK_Prio_EIP_CPULOAD (OSAL_TASK_Prio_2)
#define OSAL_TASK_Prio_EIP_CYCLICIO (OSAL_TASK_Prio_20)
#define OSAL_TASK_Prio_EIP_PACKET (OSAL_TASK_Prio_20)
#define OSAL_TASK_Prio_EIP_STATISTIC (OSAL_TASK_Prio_14)
#define OSAL_TASK_Prio_EIP_PHYMDIX (OSAL_TASK_Prio_12)
#define OSAL_TASK_Prio_EIP_TIMESYNC_DEL (OSAL_TASK_Prio_20)
#define OSAL_TASK_Prio_EIP_TIMESYNC_TS (OSAL_TASK_Prio_20)
#define OSAL_TASK_Prio_EIP_TIMESYNC_NRT (OSAL_TASK_Prio_16)
#define OSAL_TASK_Prio_EIP_TIMESYNC_BAC (OSAL_TASK_Prio_14)
#define OSAL_TASK_Prio_EIP_LWIP_TCPIP (OSAL_TASK_Prio_14)
#define OSAL_TASK_Prio_EIP_WEBSERVER (OSAL_TASK_Prio_2)

#if (defined BACKWARD_COMPATIBILITY) && (1 == BACKWARD_COMPATIBILITY)
 #define OSAL_TASK_ePRIO_0 OSAL_TASK_Prio_0
 #define OSAL_TASK_ePRIO_1 OSAL_TASK_Prio_1
 #define OSAL_TASK_ePRIO_2 OSAL_TASK_Prio_2
 #define OSAL_TASK_ePRIO_3 OSAL_TASK_Prio_3
 #define OSAL_TASK_ePRIO_4 OSAL_TASK_Prio_4
 #define OSAL_TASK_ePRIO_5 OSAL_TASK_Prio_5
 #define OSAL_TASK_ePRIO_6 OSAL_TASK_Prio_6
 #define OSAL_TASK_ePRIO_7 OSAL_TASK_Prio_7
 #define OSAL_TASK_ePRIO_8 OSAL_TASK_Prio_8
 #define OSAL_TASK_ePRIO_9 OSAL_TASK_Prio_9
 #define OSAL_TASK_ePRIO_10 OSAL_TASK_Prio_10
 #define OSAL_TASK_ePRIO_11 OSAL_TASK_Prio_11
 #define OSAL_TASK_ePRIO_12 OSAL_TASK_Prio_12
 #define OSAL_TASK_ePRIO_13 OSAL_TASK_Prio_13
 #define OSAL_TASK_ePRIO_14 OSAL_TASK_Prio_14
 #define OSAL_TASK_ePRIO_15 OSAL_TASK_Prio_15
 #define OSAL_TASK_ePRIO_16 OSAL_TASK_Prio_16
 #define OSAL_TASK_ePRIO_17 OSAL_TASK_Prio_17
 #define OSAL_TASK_ePRIO_18 OSAL_TASK_Prio_18
 #define OSAL_TASK_ePRIO_19 OSAL_TASK_Prio_19
 #define OSAL_TASK_ePRIO_20 OSAL_TASK_Prio_20
 #define OSAL_TASK_ePRIO_21 OSAL_TASK_Prio_21
 #define OSAL_TASK_ePRIO_22 OSAL_TASK_Prio_22
 #define OSAL_TASK_ePRIO_23 OSAL_TASK_Prio_23
 #define OSAL_TASK_ePRIO_24 OSAL_TASK_Prio_24
 #define OSAL_TASK_ePRIO_25 OSAL_TASK_Prio_25
 #define OSAL_TASK_ePRIO_26 OSAL_TASK_Prio_26
 #define OSAL_TASK_ePRIO_27 OSAL_TASK_Prio_27
 #define OSAL_TASK_ePRIO_28 OSAL_TASK_Prio_28
 #define OSAL_TASK_ePRIO_29 OSAL_TASK_Prio_29
 #define OSAL_TASK_ePRIO_30 OSAL_TASK_Prio_30
 #define OSAL_TASK_ePRIO_31 OSAL_TASK_Prio_31

 #define OSAL_TASK_ePRIO_Lowest OSAL_TASK_Prio_Lowest
 #define OSAL_TASK_ePRIO_Idle OSAL_TASK_Prio_Idle
 #define OSAL_TASK_ePRIO_Normal OSAL_TASK_Prio_Normal
 #define OSAL_TASK_ePRIO_Highest OSAL_TASK_Prio_Highest
 #define OSAL_TASK_ePRIO_InterruptSub1 OSAL_TASK_Prio_InterruptSub1
 #define OSAL_TASK_ePRIO_Interrupt OSAL_TASK_Prio_Interrupt
 #define OSAL_TASK_ePRIO_OSALTimer OSAL_TASK_Prio_OSALTimer

 /* HWAL */
 #define OSAL_TASK_ePRIO_HWAL_WATCHDOG OSAL_TASK_Prio_HWAL_WATCHDOG
 #define OSAL_TASK_ePRIO_HWAL_LICENSE OSAL_TASK_Prio_HWAL_LICENSE

 /* EtherCAT */
 #define OSAL_TASK_ePRIO_ECEEPROM OSAL_TASK_Prio_ECEEPROM
 #define OSAL_TASK_ePRIO_ECRestart OSAL_TASK_Prio_ECRestart
 #define OSAL_TASK_ePRIO_ECReboot OSAL_TASK_Prio_ECReboot
 #define OSAL_TASK_ePRIO_ECLED OSAL_TASK_Prio_ECLED
 #define OSAL_TASK_ePRIO_ECSync OSAL_TASK_Prio_ECSync
 #define OSAL_TASK_ePRIO_ECEoE OSAL_TASK_Prio_ECEoE
 #define OSAL_TASK_ePRIO_ECPDI OSAL_TASK_Prio_ECPDI
 #define OSAL_TASK_ePRIO_ECDemoThread OSAL_TASK_Prio_ECDemoThread

 /* FBTL */
 #define OSAL_TASK_ePRIO_FBTLSync OSAL_TASK_Prio_FBTLSync
 #define OSAL_TASK_ePRIO_FBTLAcycIST OSAL_TASK_Prio_FBTLAcycIST
 #define OSAL_TASK_ePRIO_FBTLCyclic OSAL_TASK_Prio_FBTLCyclic
 #define OSAL_TASK_ePRIO_FBTLSendAcyc OSAL_TASK_Prio_FBTLSendAcyc
 #define OSAL_TASK_ePRIO_FBTLReceiver OSAL_TASK_Prio_FBTLReceiver
 #define OSAL_TASK_ePRIO_FBTLService OSAL_TASK_Prio_FBTLService
 #define OSAL_TASK_ePRIO_FBTLSlowService OSAL_TASK_Prio_FBTLSlowService
 #define OSAL_TASK_ePRIO_FBTLOSISTCONT OSAL_TASK_Prio_FBTLOSISTCONT

 /* DPR */
 #define OSAL_TASK_ePRIO_DPRIRQ OSAL_TASK_Prio_DPRIRQ
 #define OSAL_TASK_ePRIO_DPRRX OSAL_TASK_Prio_DPRRX
 #define OSAL_TASK_ePRIO_DPRTX OSAL_TASK_Prio_DPRTX

 /* IOL */
 #define OSAL_TASK_ePRIO_IOL_LED OSAL_TASK_Prio_IOL_LED
 #define OSAL_TASK_ePRIO_IOL_Power OSAL_TASK_Prio_IOL_Power
 #define OSAL_TASK_ePRIO_IOL_SMI OSAL_TASK_Prio_IOL_SMI
 #define OSAL_TASK_ePRIO_IOL_Main OSAL_TASK_Prio_IOL_Main
 #define OSAL_TASK_ePRIO_IOL_Port OSAL_TASK_Prio_IOL_Port

 /* EtherNet/IP */
 #define OSAL_TASK_ePRIO_EIP_MAIN OSAL_TASK_Prio_EIP_MAIN
 #define OSAL_TASK_ePRIO_EIP_FLASH OSAL_TASK_Prio_EIP_FLASH
 #define OSAL_TASK_ePRIO_EIP_EEPROM OSAL_TASK_Prio_EIP_EEPROM
 #define OSAL_TASK_ePRIO_EIP_CPULOAD OSAL_TASK_Prio_EIP_CPULOAD
 #define OSAL_TASK_ePRIO_EIP_CYCLICIO OSAL_TASK_Prio_EIP_CYCLICIO
 #define OSAL_TASK_ePRIO_EIP_PACKET OSAL_TASK_Prio_EIP_PACKET
 #define OSAL_TASK_ePRIO_EIP_STATISTIC OSAL_TASK_Prio_EIP_STATISTIC
 #define OSAL_TASK_ePRIO_EIP_PHYMDIX OSAL_TASK_Prio_EIP_PHYMDIX
 #define OSAL_TASK_ePRIO_EIP_TIMESYNC_DEL OSAL_TASK_Prio_EIP_TIMESYNC_DEL
 #define OSAL_TASK_ePRIO_EIP_TIMESYNC_TS OSAL_TASK_Prio_EIP_TIMESYNC_TS
 #define OSAL_TASK_ePRIO_EIP_TIMESYNC_NRT OSAL_TASK_Prio_EIP_TIMESYNC_NRT
 #define OSAL_TASK_ePRIO_EIP_TIMESYNC_BAC OSAL_TASK_Prio_EIP_TIMESYNC_BAC
 #define OSAL_TASK_ePRIO_EIP_LWIP_TCPIP OSAL_TASK_Prio_EIP_LWIP_TCPIP
 #define OSAL_TASK_ePRIO_EIP_WEBSERVER OSAL_TASK_Prio_EIP_WEBSERVER
#endif

/*!
 *  \brief
 *  OSAL memory management configuration.
 *  \ingroup OSALAPI
 */
typedef enum OSAL_MemoryManager
{
    OSAL_MEMMAN_Default = 0,   //!< default mem manager = dynamic
    OSAL_MEMMAN_Dynamic = 1,   //!< memory management is done through libc calloc/free
    OSAL_MEMMAN_PreAlloc = 2,  //!< libc calloc is used to prealloc buffer, memory management is done in osal
    OSAL_MEMMAN_Static = 3,    //!< memory management on given static heap buffer, memory management is done in osal
} OSAL_MemoryManager_t;

#if (defined BACKWARD_COMPATIBILITY) && (1 == BACKWARD_COMPATIBILITY)
 #define OSAL_eMEMMAN_default OSAL_MEMMAN_Default
 #define OSAL_eMEMMAN_dynamic OSAL_MEMMAN_Dynamic
 #define OSAL_eMEMMAN_prealloc OSAL_MEMMAN_PreAlloc
 #define OSAL_eMEMMAN_static OSAL_MEMMAN_Static
#endif

typedef struct OSAL_MEMMAN_ConfigParameter
{
    size_t size;    //!< size of managed memory,  if OSAL_EMemoryManager_t != OSAL_eMEMMAN_default: size of memory available
    void *mem;      //!< pointer to given memory, if OSAL_EMemoryManager_t == OSAL_eMEMMAN_static:  pointer to managed memory
} OSAL_MEMMAN_ConfigParameter_t;

/*!
 *  \brief
 *  Interrupt Handler Callback function
 *
 *  \param[in]  hwIrq   Hardware Interrupt Number
 *  \param[in]  pCtxt   Interrupt Context
 *  \return     statusCode: 0:Interrupt worked
 *
 *  \sa OSAL_RegisterIrqHandler
 *
 *  \ingroup OSALAPI
 */
typedef uint32_t (*OSAL_IRQ_CBHandler_t)(uint32_t hwIrq, void *ctxt);

typedef void (*OSAL_ERR_CBHandler_t)(
     uint32_t errorCode     //!< [in] Error code
    ,bool fatal             //!< [in] Is Error fatal
    ,uint8_t paraCnt        //!< [in] parameter counter
    ,va_list argptr         //!< [in] Error arguments
);

typedef void (*OSAL_SCHED_CBTask_t)(void *arg);

typedef enum OSAL_LinkState
{
    OSAL_LINK_Down,
    OSAL_LINK_Up,
    /// @cond INTERNAL
    OSAL_LINK_Force32Bit = (0x7fffffffu)
    /// @endcond
} OSAL_LinkState_t;

#if (defined BACKWARD_COMPATIBILITY) && (1 == BACKWARD_COMPATIBILITY)
 #define OSAL_eLINK_DOWN OSAL_LINK_Down
 #define OSAL_eLINK_UP   OSAL_LINK_Up
#endif

typedef enum OSAL_MemoryError
{
    OSAL_MEM_Calloc,
    OSAL_MEM_Malloc,
    OSAL_MEM_Free,
    OSAL_MEM_Memset,
    OSAL_MEM_Memcpy,
    OSAL_MEM_Memcmp,
    /// @cond INTERNAL
    OSAL_MEM_Force32Bit = (0x7fffffffu)
    /// @endcond
} OSAL_MemoryError_t;

#if (defined BACKWARD_COMPATIBILITY) && (1 == BACKWARD_COMPATIBILITY)
 #define OSAL_eMEM_CALLOC OSAL_MEM_Calloc
 #define OSAL_eMEM_MALLOC OSAL_MEM_Malloc
 #define OSAL_eMEM_FREE OSAL_MEM_Free
 #define OSAL_eMEM_MEMSET OSAL_MEM_Memset
 #define OSAL_eMEM_MEMCPY OSAL_MEM_Memcpy
 #define OSAL_eMEM_MEMCMP OSAL_MEM_Memcmp
#endif

#define OSAL_OS_START_TASK_FLG_NONE (0x00000000u)
#define OSAL_OS_START_TASK_FLG_FPU (0x00000001u) /* Task uses floating point processor */

typedef void (*OSAL_CMDQUEUE_CBEvent_t)(void *a0, void *a1);

typedef struct OSAL_CMDQUEUE_Command
{
    OSAL_CMDQUEUE_CBEvent_t cbEvent;    //!< Function, called for processing the event
    void *instance;                     //!< Instance data for the Event
    void *arg;                          //!< Argument structure for the Event
} OSAL_CMDQUEUE_Command_t;

typedef struct OSAL_SCHED_EventHandle
{
    uint16_t dummy;                     //!< to prevent warnings/errors about an empty struct
} OSAL_SCHED_EventHandle_t;

struct OSAL_CMDQUEUE_Handle;

typedef struct OSAL_TIMER_Handle
{
    OSAL_CMDQUEUE_Command_t cmd;
    struct OSAL_CMDQUEUE_Handle* cmdQueue;
    uint8_t singleShot;

    /* ------------- Additional fields for OSAL only usage -------------------------- */
    char name[0x20];
} OSAL_TIMER_Handle_t;

typedef struct OSAL_SCHED_MutexHandle
{
    uint16_t dummy; /* to prevent warnings/errors about an empty struct */
} OSAL_SCHED_MutexHandle_t;

typedef struct OSAL_SCHED_SignalHandle
{
    uint16_t dummy; /* to prevent warnings/errors about an empty struct */
} OSAL_SCHED_SignalHandle_t;

typedef struct OSAL_CMDQUEUE_Handle
{
    uint16_t dummy; /* to prevent warnings/errors about an empty struct */

    /* ------------- Additional fields for OSAL only usage -------------------------- */
    char name[0x20];
} OSAL_CMDQUEUE_Handle_t;

typedef long long OSAL_PJumpBuf_t[64] __attribute__((__aligned__ (8)));

/*!
 *  \brief
 *  Callback to printout OSAL_printf occurencies
 *
 *  \param[in]  context     call context
 *  \param[in]  format      Format string.
 *  \param[in]  arg         Parameter list.
 *
 *  \ingroup OSALAPI
 * */
typedef void (*OSAL_PrintfOut_t)(void *context, const char *format, va_list arg);

/*!
 *  \brief
 *  Calloc trace, called after memory allocation to show memory usage
 *
 *  \param[in]  context     Call context.
 *  \param[in]  nmemb       number of allocated elements
 *  \param[in]  size        size of single allocated element
 *  \param[in]  ptr         pointer to allocated memory
 *
 *  \note can be provided by the application layer, and registered with \ref
 *          OSAL_MEMORY_traceCallocRegister
 *
 *  \par Example
 *  \code{.c}
 *  #include <osal.h>
 *
 *  // required variables
 *  void* pvVariable = NULL;
 *
 *  // the call
 *  OSAL_MEMORY_traceCalloc(NULL, 123, 456, pvVariable);
 *  \endcode
 *
 *  \sa OSAL_SCHED_traceThreadRegister OSAL_MEMORY_traceCallocRegister OSAL_MEMORY_traceFreeRegister
 *      OSAL_MEMORY_errorRegister
 *
 *  \ingroup OSALAPI_MEMORY
 * */
typedef void (*OSAL_MEMORY_TraceCallocCB_t)(void *context, size_t nmemb, size_t size, void *ptr);

/*!
 *  \brief
 *  Calloc trace, called after memory allocation to show memory usage
 *
 *  \param[in]  context     Call context.
 *  \param[in]  nmemb       number of allocated elements
 *  \param[in]  size        size of single allocated element
 *  \param[in]  ptr         pointer to allocated memory
 *
 *  \note can be provided by the application layer, and registered with
 *          \ref OSAL_MEMORY_traceCallocRegister
 *
 *  \par Example
 *  \code{.c}
 *  #include <osal.h>
 *
 *  // required variables
 *  void* pvVariable = NULL;
 *
 *  // the call
 *  OSAL_MEMORY_traceMalloc(NULL, 456, pvVariable);
 *  \endcode
 *
 *  \sa OSAL_SCHED_traceThreadRegister OSAL_MEMORY_traceMallocRegister OSAL_MEMORY_traceFreeRegister
 *      OSAL_MEMORY_errorRegister
 *
 *  \ingroup OSALAPI_MEMORY
 * */
typedef void (*OSAL_MEMORY_TraceMallocCB_t)(void *context, size_t size, void *ptr);

/*!
 *  \brief
 *  free trace, called before memory decommission to show memory usage
 *
 *  \note can be provided by the application layer, and registered with
 *          \ref OSAL_MEMORY_traceFreeRegister
 *
 *  \param[in]  context Call context.
 *  \param[in]  ptr     pointer to memory that is decommitted
 *
 *  \par Example
 *  \code{.c}
 *  #include <osal.h>
 *
 *  // required variables
 *  void* pvVariable = NULL;
 *
 *  // the call
 *  OSAL_MEMORY_traceCalloc(NULL, 123, 456, pvVariable);
 *
 *  OSAL_MEMORY_traceFree(NULL, pvVariable);
 *
 *  \endcode
 *
 *  \sa OSAL_SCHED_traceThreadRegister OSAL_MEMORY_traceCallocRegister OSAL_MEMORY_traceFreeRegister
 *      OSAL_MEMORY_errorRegister
 *
 *  \ingroup OSALAPI_MEMORY
 * */
typedef void (*OSAL_MEMORY_TraceFreeCB_t)(void *context, void *ptr);

/*!
 *  \brief
 *  Hook to be called on memory operation error in OSAL
 *
 *  \details
 *  The error callback is called in different situations with different parameters. Not all
 *  parameters are necessarily valid at the same time.
 *
 *  \note can be provided by the application layer, and registered with
 *          \ref OSAL_MEMORY_errorRegister
 *
 *  <table>
 *  <tr><td><b>errType_p</b></td><td><b>Valid parameters</b></td><td><b>Description</b></td></tr>
 *  <tr><td>OSAL_eMEM_CALLOC</td><td>size_p - size of requested memory</td>
 *      <td>Calloc did return NULL</td></tr>
 *  <tr><td>OSAL_eMEM_FREE</td><td>pPtr1_p - pointer to buffer to be freed</td>
 *      <td>Free did hit an error</td></tr>
 *  <tr><td>OSAL_eMEM_MEMSET</td><td>pPtr1_p - destination pointer<br>
 *                                   data_p - value to be set<br>size_p - set width (in bytes)</td>
 *      <td>memset did hit a error</td></tr>
 *  <tr><td>OSAL_eMEM_MEMCPY</td><td>pPtr1_p - destination pointer<br>pPtr2_p - source pointer<br>
 *                                   size_p - copy length</td><td>Memcpy did hit an error</td></tr>
 *  <tr><td>OSAL_eMEM_MEMCMP</td><td>pPtr1_p - reference pointer<br>pPtr2_p - comparison pointer<br>
 *                                   size_p - comparison length</td>
 *      <td>Memcmp did hit an error</td></tr>
 *  </table>
 *
 *  \param[in]  context Call context.
 *  \param[in]  errType Type of error
 *  \param[in]  ptr1    pointer 1
 *  \param[in]  ptr2    pointer 2
 *  \param[in]  data    fixed value
 *  \param[in]  size    size of working range
 *
 *  \sa OSAL_SCHED_traceThreadRegister OSAL_MEMORY_traceCallocRegister OSAL_MEMORY_traceFreeRegister
 *      OSAL_MEMORY_errorRegister
 *
 *  \ingroup OSALAPI_MEMORY
 * */
typedef void (*OSAL_MEMORY_ErrorCB_t)(
    void *context,
    OSAL_MemoryError_t errType,
    const void *ptr1,
    const void *ptr2,
    uint32_t data,
    size_t size);

/*!
 *  \brief
 *  Trace thread handling
 *
 *  \param[in]  context     Call context.
 *  \param[in]  task        Thread handle
 *  \param[in]  threadOp    Thread state operation
 *  \param[in]  prio        Thread priority
 *  \param[in]  name        Thread name
 *  \param[in]  stackSize   size of stack
 *  \param[in]  stackPtr    stack base address
 *  \param[in]  flagsthread flags
 *
 *  \note can be provided by the application layer, and registered with
 *          \ref OSAL_SCHED_traceThreadRegister
 *
 *  <!-- Example: -->
 *
 *  \par Example
 *  \code{.c}
 *  #include <osal.h>
 *
 *  // required variables
 *
 *  // the call
 *  OSAL_SCHED_traceThread(NULL, "START", 0, "ImportantThread");
 *  \endcode
 *
 *  <!-- References: -->
 *
 *  \sa OSAL_SCHED_traceThreadRegister OSAL_MEMORY_traceCallocRegister
 *      OSAL_MEMORY_traceFreeRegister OSAL_MEMORY_errorRegister
 *
 *  <!-- Group: -->
 *
 *  \ingroup OSALAPI_SCHEDULER
 *
 * */
typedef void (*OSAL_SCHED_TraceThreadCB_t)(
    void *context,
    void *task,
    const char *threadOp,
    uint32_t prio,
    const char *name,
    uint32_t stackSize,
    void *stackPtr,
    uint32_t flags);

#if (defined __cplusplus)
extern "C" {
#endif

/* OSAL */
extern OSAL_API uint32_t OSAL_init(void);
extern OSAL_API void OSAL_deinit(void);

extern OSAL_API uint32_t OSAL_getVersion(void);
extern OSAL_API uint32_t OSAL_getVersionStr(uint32_t bufLen, char *buffer, uint32_t *usedLen);
extern OSAL_API uint32_t OSAL_getVersionId(uint32_t bufLen, char *buffer, uint32_t *usedLen);

extern OSAL_API void OSAL_setExceptionPoint(OSAL_PJumpBuf_t *jumpBuf);
extern OSAL_API OSAL_PJumpBuf_t *OSAL_getExceptionPoint(void);
extern OSAL_API int32_t OSAL_setJmp(OSAL_PJumpBuf_t *jumpBuf);
extern OSAL_API void OSAL_longJmp(OSAL_PJumpBuf_t *jumpBuf, int32_t value);

/* @cppcheck_justify{misra-c2012-2.7} va_list is false positive shown as unused */
/* cppcheck-suppress misra-c2012-2.7 */
extern OSAL_API void OSAL_error(
    const char *function,
    uint32_t line,
    uint32_t errCode,
    bool fatal,
    uint32_t paraCnt,
    ...);
extern OSAL_API void OSAL_registerErrorHandler(OSAL_ERR_CBHandler_t cbErrHandler);
extern OSAL_API void OSAL_closeHandles(void);
extern OSAL_API void OSAL_registerPrintOut(void *context, OSAL_PrintfOut_t cbFunc);

/* @cppcheck_justify{misra-c2012-17.1} va_list dynamic args is defined in stdarg.h */
/* cppcheck-suppress misra-c2012-17.1 */
/* @cppcheck_justify{misra-c2012-2.7} va_list is false positive shown as unused */
/* cppcheck-suppress misra-c2012-2.7 */
extern OSAL_API void OSAL_printf(const char *format, ...);
extern OSAL_API void OSAL_vprintf(
    const char *format,
    /* @cppcheck_justify{misra-c2012-17.1} dynamic printf is only possible with use of stdarg */
    /* cppcheck-suppress misra-c2012-17.1 */
    va_list arg);
extern OSAL_API void OSAL_printfSuppress(bool suppress);
extern OSAL_API uint32_t OSAL_getMsTick(void);
extern OSAL_API void OSAL_waitTimerUs(uint32_t waitTime);
extern OSAL_API uint32_t OSAL_randomEnable(uint32_t seed);
extern OSAL_API uint32_t OSAL_getRandomNumber(void);

/* OSAL_SCHED */
extern OSAL_API void OSAL_SCHED_sleep(uint32_t duration);
extern OSAL_API void OSAL_SCHED_sleepUs(uint32_t durationUs);
extern OSAL_API void OSAL_SCHED_yield(void);
extern OSAL_API void OSAL_SCHED_taskPrepare(void);
extern OSAL_API void *OSAL_SCHED_startTask(
    OSAL_SCHED_CBTask_t cbTask,
    void *arg,
    OSAL_TASK_Priority_t prio,
    uint8_t *stack,
    uint32_t stackSize,
    uint32_t flags,
    char *taskName);
extern OSAL_API uint32_t OSAL_SCHED_joinTask(void *task);
extern OSAL_API void OSAL_SCHED_killTask(void *handle);
extern OSAL_API void OSAL_SCHED_exitTask(void *task);
extern OSAL_API uint32_t OSAL_SCHED_setTaskPriority(void *task, OSAL_TASK_Priority_t prio);
extern OSAL_API volatile void *OSAL_SCHED_getOwnTcb(void);

extern OSAL_API bool OSAL_SCHED_isRunning(void);
extern OSAL_API void OSAL_SCHED_traceThreadRegister(
    OSAL_SCHED_TraceThreadCB_t cbFunc,
    void *cbFuncContext);
extern OSAL_API volatile OSAL_PJumpBuf_t *OSAL_getExceptPointFromTcb(volatile void *tcb);

extern OSAL_API void OSAL_startOs(void);

extern OSAL_API void OSAL_EVT_init(OSAL_SCHED_EventHandle_t *event);
extern OSAL_API void OSAL_EVT_set(OSAL_SCHED_EventHandle_t *event);
extern OSAL_API uint32_t OSAL_EVT_wait(
    OSAL_SCHED_EventHandle_t *event,
    uint32_t timeOut,
    uint8_t *isTimeout);

extern OSAL_API void OSAL_TIMER_init(OSAL_TIMER_Handle_t *timer, char *name);
extern OSAL_API void OSAL_TIMER_set100usecTickSupport(bool support);
extern OSAL_API void OSAL_TIMER_start(OSAL_TIMER_Handle_t *timer, uint32_t durationMs);
extern OSAL_API void OSAL_TIMER_startUs(OSAL_TIMER_Handle_t *timer, uint32_t durationUs);
extern OSAL_API void OSAL_TIMER_stop(OSAL_TIMER_Handle_t *timer);
extern OSAL_API void OSAL_TIMER_expire(OSAL_TIMER_Handle_t *timer);

extern OSAL_API void OSAL_CMDQUEUE_init(OSAL_CMDQUEUE_Handle_t *queue, char *name);
extern OSAL_API uint32_t OSAL_CMDQUEUE_put(
    OSAL_CMDQUEUE_Handle_t *queue,
    OSAL_CMDQUEUE_CBEvent_t cbEvent,
    void *instance,
    void *arg);
extern OSAL_API uint8_t OSAL_CMDQUEUE_peek(
    OSAL_CMDQUEUE_Handle_t *queue,
    OSAL_CMDQUEUE_CBEvent_t cbEvent,
    void *instance,
    void *arg);
extern OSAL_API uint32_t OSAL_CMDQUEUE_exec(
    OSAL_CMDQUEUE_Handle_t *queue,
    uint32_t timeOut,
    uint8_t *isTimeout);
extern OSAL_API uint32_t OSAL_CMDQUEUE_getFree(OSAL_CMDQUEUE_Handle_t *queue);

extern OSAL_API void OSAL_MTX_init(OSAL_SCHED_MutexHandle_t *mutex);
extern OSAL_API void OSAL_MTX_deinit(OSAL_SCHED_MutexHandle_t *mutex);
extern OSAL_API void OSAL_MTX_release(OSAL_SCHED_MutexHandle_t *mutex);
extern OSAL_API uint32_t OSAL_MTX_get(
    OSAL_SCHED_MutexHandle_t *mutex,
    uint32_t timeOut,
    uint8_t *isTimeout);

extern OSAL_API OSAL_SCHED_EventHandle_t *OSAL_EVTCTRLBLK_alloc(void);
extern OSAL_API void OSAL_EVTCTRLBLK_free(OSAL_SCHED_EventHandle_t *ctrlBlk);
extern OSAL_API OSAL_TIMER_Handle_t *OSAL_TIMCTRLBLK_alloc(void);
extern OSAL_API void OSAL_TIMCTRLBLK_free(OSAL_TIMER_Handle_t *ctrlBlk);
extern OSAL_API OSAL_SCHED_MutexHandle_t *OSAL_MTXCTRLBLK_alloc(void);
extern OSAL_API void OSAL_MTXCTRLBLK_free(OSAL_SCHED_MutexHandle_t *ctrlBlk);
extern OSAL_API OSAL_CMDQUEUE_Handle_t *OSAL_CQUEUECTRLBLK_alloc(void);
extern OSAL_API void OSAL_CQUEUECTRLBLK_free(OSAL_CMDQUEUE_Handle_t *ctrlBlk);

extern OSAL_API uint32_t OSAL_CRITSECT_enter(void);
extern OSAL_API void OSAL_CRITSECT_exit(uint32_t prevState);

/* OSAL_IRQ */
extern OSAL_API uint32_t OSAL_IRQ_registerHandler(
    uint32_t hwIrq,
    void *ctxt,
    OSAL_IRQ_CBHandler_t cbIrqHandler);
extern OSAL_API uint32_t OSAL_IRQ_unRegisterHandler(uint32_t hwIrq);
extern OSAL_API void OSAL_IRQ_enable(uint32_t hwIrq);
extern OSAL_API void OSAL_IRQ_disable(uint32_t hwIrq);
extern OSAL_API uint32_t OSAL_IRQ_disableGlobalIrq(void);
extern OSAL_API void OSAL_IRQ_enableGlobalIrq(void);
extern OSAL_API uint32_t OSAL_IRQ_disableGlobalFiq(void);
extern OSAL_API void OSAL_IRQ_enableGlobalFiq(void);
extern OSAL_API void OSAL_IRQ_restoreCPSR(uint32_t key);

/* OSAL Semaphore */
extern OSAL_API void *OSAL_createNamedMutex(const char *name);
extern OSAL_API void *OSAL_openNamedMutex(const char *name);
extern OSAL_API void *OSAL_deleteNamedMutex(void *mutex);
extern OSAL_API int32_t OSAL_lockNamedMutex(void *mutex, uint32_t msTimeout);
extern OSAL_API int32_t OSAL_unLockNamedMutex(void *mutex);

extern OSAL_API void *OSAL_CSMTX_create(void);
extern OSAL_API void *OSAL_CSMTX_delete(void *critSect);
extern OSAL_API int32_t OSAL_CSMTX_enter(void *critSect, uint32_t msTimeout);
extern OSAL_API int32_t OSAL_CSMTX_leave(void *critsect);

extern OSAL_API void *OSAL_createSignal(const char *name);
extern OSAL_API void *OSAL_openSignal(const char *name);
extern OSAL_API void *OSAL_deleteSignal(void *signal);
extern OSAL_API int32_t OSAL_waitSignal(void *signal, uint32_t msTimeout);
extern OSAL_API int32_t OSAL_postSignal(void *signal);

extern OSAL_API void *OSAL_createCountSignal(uint32_t countStart, uint32_t countMax);
extern OSAL_API void *OSAL_deleteCountSignal(void *signal);
extern OSAL_API int32_t OSAL_waitCountSignal(void *signal, uint32_t msTimeout);
extern OSAL_API int32_t OSAL_postCountSignal(void *signal);

/* OSAL Memory */
extern OSAL_API void OSAL_MEMORY_init(void);
extern OSAL_API void OSAL_MMAP_init(void);
extern OSAL_API void *OSAL_MMAP_create(uint32_t physicalAddr, uint32_t size);
extern OSAL_API void OSAL_MMAP_remove(void *virtAddr, uint32_t size);
extern OSAL_API uint32_t OSAL_MEMORY_config(
    OSAL_MemoryManager_t memAlg,
    OSAL_MEMMAN_ConfigParameter_t *config);
extern OSAL_API void *OSAL_MEMORY_calloc(size_t nmemb, size_t size);
extern OSAL_API void *OSAL_MEMORY_malloc(size_t size);
extern OSAL_API void OSAL_MEMORY_free(void *ptr);
extern OSAL_API void OSAL_MEMORY_memcpy(void *target, const void *source, size_t size);
extern OSAL_API void OSAL_MEMORY_memset(void *target, int32_t value, size_t size);
extern OSAL_API int32_t OSAL_MEMORY_memcmp(void *ptr1, void *ptr2, size_t size);
extern OSAL_API void OSAL_MEMORY_traceCallocRegister(
    OSAL_MEMORY_TraceCallocCB_t cbFunc,
    void *cbFuncContext);
extern OSAL_API void OSAL_MEMORY_traceMallocRegister(
    OSAL_MEMORY_TraceMallocCB_t cbFunc,
    void *cbFuncContext);
extern OSAL_API void OSAL_MEMORY_traceFreeRegister(
    OSAL_MEMORY_TraceFreeCB_t cbFunc,
    void *cbFuncContext);
extern OSAL_API void OSAL_MEMORY_errorRegister(OSAL_MEMORY_ErrorCB_t cbFunc, void *cbFuncContext);

extern OSAL_API uint32_t OSAL_VNET_create(char *dev);
extern OSAL_API uint32_t OSAL_VNET_remove(char *dev);
extern OSAL_API uint32_t OSAL_VNET_open(char *dev, void **handle);
extern OSAL_API uint32_t OSAL_VNET_configureMAC(void *handle, const uint8_t mac[6]);
extern OSAL_API uint32_t OSAL_VNET_configureIPv4(
    void *handle,
    const uint32_t ipAddress,
    const uint32_t mask);
extern OSAL_API uint32_t OSAL_VNET_configureIPv4Route(void *handle, const uint32_t defGateway);
extern OSAL_API uint32_t OSAL_VNET_close(void *handle);
extern OSAL_API uint32_t OSAL_VNET_rxWait(void *handle, uint32_t mSecTimeout);
extern OSAL_API uint32_t OSAL_VNET_readFrame(
    void *handle,
    uint32_t bufSize,
    void *buffer,
    uint32_t *used);
extern OSAL_API uint32_t OSAL_VNET_writeFrame(void *handle, uint32_t bufSize, void *buffer);
extern OSAL_API uint32_t OSAL_VNET_setLinkState(void *handle, OSAL_LinkState_t state);
extern OSAL_API uint32_t OSAL_VNET_setIfFlags(void *handle, int32_t flags);
extern OSAL_API uint32_t OSAL_VNET_ntohl(uint32_t net);
extern OSAL_API uint32_t OSAL_VNET_htonl(uint32_t host);

#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_OSAL_H */
