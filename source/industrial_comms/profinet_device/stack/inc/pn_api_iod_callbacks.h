/*!
 * \file pn_api_iod_callbacks.h
 *
 * \brief
 * The general structure needed to register all callback functions.
 *
 * \author
 * KUNBUS GmbH
 *
 * \copyright
 * Copyright (c) 2023, KUNBUS GmbH<br /><br />
 * SPDX-License-Identifier: LicenseRef-Kunbus
 *
 * Copyright (c) 2024 KUNBUS GmbH
 * All rights reserved.
 *
 *
 */

#ifndef PN_API_IOD_CALLBACKS_H
#define PN_API_IOD_CALLBACKS_H

#include "pn_api_iod_alarm.h"
#include "pn_api_iod_bsp.h"
#include "pn_api_iod_communication.h"
#include "pn_api_iod_data.h"
#include "pn_api_iod_dbai.h"
#include "pn_api_iod_isoch.h"
#include "pn_api_iod_device.h"
#include "pn_api_iod_record.h"
#include "pn_api_iod_settings.h"
#include "pn_api_iod_types.h"
#include "pn_api_iod_utils.h"

#if(defined __cplusplus)
extern "C" {
#endif

/**
 * \brief Structure representing a set of callbacks.
 *
 * \ingroup PN_API_IOD_TYPES_DOXY_GROUP
 */
typedef struct PN_API_IOD_SCallbacks
{
    PN_API_IOD_arConnectCbf                 arConnect;              /**< Function pointer to arConnect callback function. */
    PN_API_IOD_arDisconnCbf                 arDisconn;              /**< Function pointer to arDisconn callback function. */
    PN_API_IOD_reportNewIpAddrCbf           reportNewIpAddr;        /**< Function pointer to reportNewIpAddr callback function. */
    PN_API_IOD_resetToFactoryCbf            resetToFactory;         /**< Function pointer to resetToFactory callback function. */
    PN_API_IOD_storeRemaMemCbf              storeRemaMem;           /**< Function pointer to storeRemaMem callback function. */
    PN_API_IOD_restoreRemaMemCbf            restoreRemaMem;         /**< Function pointer to restoreRemaMem callback function. */
    PN_API_IOD_freeRemaMemCbf               freeRemaMem;            /**< Function pointer to freeRemaMem callback function. */
    PN_API_IOD_setLedCbf                    setLed;                 /**< Function pointer to setLed callback function. */
    PN_API_IOD_startLedBlinkCbf             startLedBlink;          /**< Function pointer to startLedBlink callback function. */
    PN_API_IOD_stopLedBlinkCbf              stopLedBlink;           /**< Function pointer to stopLedBlink callback function. */
    PN_API_IOD_arOwnershipCbf               arOwnership;            /**< Function pointer to arOwnership callback function. */
    PN_API_IOD_paramEndCbf                  paramEnd;               /**< Function pointer to paramEnd callback function. */
    PN_API_IOD_dataReadCbf                  readData;               /**< Function pointer to readData callback function. */
    PN_API_IOD_dataWriteCbf                 writeData;              /**< Function pointer to writeData callback function. */
    PN_API_IOD_recordReadCbf                recordRead;             /**< Function pointer to recordRead callback function. */
    PN_API_IOD_arInDataCbf                  arInData;               /**< Function pointer to arInData callback function. */
    PN_API_IOD_reportArFsuRecordCbf         reportArFsuRecord;      /**< Function pointer to reportArFsuRecord callback function. */
    PN_API_IOD_recordWriteCbf               recordWrite;            /**< Function pointer to recordWrite callback function. */
    PN_API_IOD_newModPullCbf                newModPull;             /**< Function pointer to newModPull callback function. */
    PN_API_IOD_dataReadIOxSOnlyCbf          readIOxSDataOnly;       /**< Function pointer to readIOxSDataOnly callback function. */
    PN_API_IOD_dataWriteIOxSOnlyCbf         writeIOxSDataOnly;      /**< Function pointer to writeIOxSDataOnly callback function. */
    PN_API_IOD_readyForInputUpdateCbf       readyForInputUpdate;    /**< Function pointer to readyForInputUpdate callback function. */
    PN_API_IOD_newModPlugCbf                newModPlug;             /**< Function pointer to newModPlug callback function. */
    PN_API_IOD_asyncReqDoneCbf              asyncReqDone;           /**< Function pointer to asyncReqDone callback function. */
    PN_API_IOD_devAlarmCbf                  devAlarm;               /**< Function pointer to devAlarm callback function. */
    PN_API_IOD_outSubmodSubstValReadCbf     outSubmodSubstValRead;  /**< Function pointer to outSubmodSubstValRead callback function. */
    PN_API_IOD_errorLogCbf                  errorLog;               /**< Function pointer to errorLog callback function. */
    PN_API_IOD_updateAppCycleTimerCbf       updateAppCycle;         /**< Function pointer to update app cycle callback function */
} PN_API_IOD_Callbacks_t;

#if (defined __cplusplus)
}
#endif

#endif // PN_API_IOD_CALLBACKS_H
