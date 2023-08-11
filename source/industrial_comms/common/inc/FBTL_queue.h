/*!
 *  \file FBTL_queue.h
 *
 *  \brief
 *  FBTL queue implementation interface.
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

#if !(defined __FBTL_QUEUE_H__)
#define __FBTL_QUEUE_H__		1

#include <FBTL_api.h>

struct FBTL_QUEUE_SHeader;

typedef struct FBTL_QUEUE_SHook
{
    void*                           pOwnerElement;
    struct FBTL_QUEUE_SHeader*      pQueueHead;
    struct FBTL_QUEUE_SHook*        pNext;
    struct FBTL_QUEUE_SHook*        pPrev;
} FBTL_QUEUE_SHook_t;

typedef struct FBTL_QUEUE_SHeader
{
    char                            aName[0x40];
    void*                           pSignal;
    void*                           pQueueProtect;
    bool                            locked;

    uint32_t                        count;

    FBTL_QUEUE_SHook_t*             pFirst;
    FBTL_QUEUE_SHook_t*             pLast;
} FBTL_QUEUE_SHeader_t;

#define FBTL_QUEUE_popLocked(sysLib, queue, hook, retval) \
    FBTL_QUEUE_lock((sysLib), (queue)); \
    retVal = FBTL_QUEUE_pop((sysLib), (queue), (hook)); \
    FBTL_QUEUE_unLock((sysLib), (queue));

#define FBTL_QUEUE_pushLocked(sysLib, queue, hook, retval) \
    FBTL_QUEUE_lock((sysLib), (queue)); \
    retVal = FBTL_QUEUE_push((sysLib), (queue), (hook)); \
    FBTL_QUEUE_unLock((sysLib), (queue));

#if (defined __cplusplus)
extern "C" {
#endif

extern uint32_t FBTL_QUEUE_init     (void*                  pSysLib_p
                                    ,char*                  pName_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p);
extern uint32_t FBTL_QUEUE_deinit   (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p);

extern uint32_t FBTL_QUEUE_append   (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p
                                    ,FBTL_QUEUE_SHook_t*    pElement_p);
extern uint32_t FBTL_QUEUE_push     (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p
                                    ,FBTL_QUEUE_SHook_t*    pElement_p);
extern uint32_t FBTL_QUEUE_wait     (void*                  pSysLib
                                    ,FBTL_QUEUE_SHeader_t*  pQueue
                                    ,uint32_t               timeout);
extern uint32_t FBTL_QUEUE_pop      (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p
                                    ,FBTL_QUEUE_SHook_t**   pElement_p);
extern uint32_t FBTL_QUEUE_remove   (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p
                                    ,FBTL_QUEUE_SHook_t*    pElement_p);
extern bool     FBTL_QUEUE_isSane   (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p);
extern bool     FBTL_QUEUE_isQueued (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHook_t*    pElement_p);
extern bool     FBTL_QUEUE_isElement(void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p
                                    ,FBTL_QUEUE_SHook_t*    pElement_p);

extern uint32_t FBTL_QUEUE_count    (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p);

extern uint32_t FBTL_QUEUE_lock     (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p);

extern void     FBTL_QUEUE_unLock   (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHeader_t*  pQueue_p);
extern FBTL_QUEUE_SHook_t*
                FBTL_QUEUE_getIter  (void*                  pSysLib_p
                   ,FBTL_QUEUE_SHeader_t*  pQueue_p);
extern FBTL_QUEUE_SHook_t*
                FBTL_QUEUE_getNext  (void*                  pSysLib_p
                                    ,FBTL_QUEUE_SHook_t*    pIter_p);

#if (defined __cplusplus)
}
#endif

#endif /* __FBTL_QUEUE_H__ */
