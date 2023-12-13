/*!
 *  \file TinyQueue.h
 *
 *  \brief
 *  small ring buffer queue as a replacement for OSAL_CMDQUEUE
 *
 *  \author
 *  KUNBUS GmbH
 *
 *  \copyright
 *  Copyright (c) 2023, KUNBUS GmbH<br /><br />
 *  SPDX-License-Identifier: LicenseRef-Kunbus
 *
 *  Copyright (c) 2023 KUNBUS GmbH
 *  All rights reserved.
 *
 *
 */

#if !(defined PROTECT_TINYQUEUE_H)
#define PROTECT_TINYQUEUE_H        1


#include <stdint.h>
#include <stdbool.h>
#include <osal.h>

#if (defined __cplusplus)
extern "C" {
#endif

#define TQUEUE_MAX_ENTRY          40

typedef enum tq_error_t {
    TQ_ERRCODE_OK = 0,
    TQ_ERRCODE_NOMUTEX,
    TQ_ERRCODE_NOEVENT,
    TQ_ERRCODE_EVENTWAIT,
    TQ_ERRCODE_INVALIDPARAM
} tq_error_t;


/*!
 *  <!-- Description: -->
 *
 *  \brief
 *  typedef struct of circular buffer structure
 *
 * */
typedef struct TQUEUE_ringBuffer {
    uintptr_t aQueueValues[TQUEUE_MAX_ENTRY];  /**! \brief fix type and fix length of queue entries */
    uint8_t head;                             /**! \brief write index */
    uint8_t tail;                             /**! \brief read index */
    uint8_t counter;                          /**! \brief write index */
    uint8_t maxEntries;                       /**! \brief count of open entries, for statistic */
    bool full;                                /**! \brief true = queue is full */
    OSAL_SCHED_EventHandle_t* pQueueEvt;     /**! \brief wakeup event */
    OSAL_SCHED_MutexHandle_t* pQueueMtx;     /**! \brief secure mutex */
}TQUEUE_ringBuffer_t;


// initialize queue elements
extern uint32_t TQUEUE_init(
    TQUEUE_ringBuffer_t* pQueueHandle_p);


// get a queue element, if queue empty than go to sleep
extern uint32_t TQUEUE_wait(
    TQUEUE_ringBuffer_t* pQueueHandle_p, 
    uintptr_t*           pQueueElement_p);


// put a element to queue and wake up event handler
extern void TQUEUE_put(
    TQUEUE_ringBuffer_t* pQueueHandle_p, 
    const uintptr_t      queueElement_p);


#if (defined __cplusplus)
}
#endif

#endif /* PROTECT_TINYQUEUE_H */
