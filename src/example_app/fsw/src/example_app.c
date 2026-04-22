/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * \file
 *   This file contains the source code for the Sample App.
 */

/*
** Include Files:
*/
#include "example_app.h"
#include "example_app_cmds.h"
#include "example_app_utils.h"
#include "example_app_eventids.h"
#include "example_app_dispatch.h"
#include "example_app_tbl.h"
#include "example_app_version.h"

/*
** global data
*/
EXAMPLE_APP_Data_t EXAMPLE_APP_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void EXAMPLE_APP_Main(void)
{
    CFE_Status_t     status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(EXAMPLE_APP_PERF_ID);

    /*
    ** Perform application-specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = EXAMPLE_APP_Init();
    if (status != CFE_SUCCESS)
    {
        EXAMPLE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Sample App Runloop
    */
    while (CFE_ES_RunLoop(&EXAMPLE_APP_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(EXAMPLE_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, EXAMPLE_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(EXAMPLE_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            EXAMPLE_APP_TaskPipe(SBBufPtr);
            printf("------------------Cryptography sample------------------\n");
        }
        else
        {
            CFE_EVS_SendEvent(EXAMPLE_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE_APP: SB Pipe Read Error, App Will Exit");

            EXAMPLE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(EXAMPLE_APP_PERF_ID);

    CFE_ES_ExitApp(EXAMPLE_APP_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t EXAMPLE_APP_Init(void)
{
    CFE_Status_t status;
    char         VersionString[EXAMPLE_APP_CFG_MAX_VERSION_STR_LEN];

    /* Zero out the global data structure */
    memset(&EXAMPLE_APP_Data, 0, sizeof(EXAMPLE_APP_Data));

    EXAMPLE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Example App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }
    else
    {
        /*
         ** Initialize housekeeping packet (clear user data area).
         */
        CFE_MSG_Init(CFE_MSG_PTR(EXAMPLE_APP_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(EXAMPLE_APP_HK_TLM_MID),
                     sizeof(EXAMPLE_APP_Data.HkTlm));

        /*
         ** Create Software Bus message pipe.
         */
        printf("DEBUG: Pipe Depth: %d\n", EXAMPLE_APP_PLATFORM_PIPE_DEPTH);
        printf("DEBUG: Pipe Name: %s\n", EXAMPLE_APP_PLATFORM_PIPE_NAME);
        printf("DEBUG: Calling CFE_SB_CreatePipe...\n");
        fflush(stdout);
        status = CFE_SB_CreatePipe(&EXAMPLE_APP_Data.CommandPipe, EXAMPLE_APP_PLATFORM_PIPE_DEPTH,
                                   EXAMPLE_APP_PLATFORM_PIPE_NAME);
        printf("DEBUG: CFE_SB_CreatePipe returned 0x%08lX\n", (unsigned long)status);
        fflush(stdout);
        if (status != CFE_SUCCESS)
        {
            printf("DEBUG: Pipe creation FAILED\n");
            fflush(stdout);
            CFE_EVS_SendEvent(EXAMPLE_APP_CR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Example App: Error creating SB Command Pipe, RC = 0x%08lX", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to Housekeeping request commands
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(EXAMPLE_APP_SEND_HK_MID), EXAMPLE_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(EXAMPLE_APP_SUB_HK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Example App: Error Subscribing to HK request, RC = 0x%08lX", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to ground command packets
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(EXAMPLE_APP_CMD_MID), EXAMPLE_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(EXAMPLE_APP_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Example App: Error Subscribing to Commands, RC = 0x%08lX", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Register Example Table(s)
        */
        status = CFE_TBL_Register(&EXAMPLE_APP_Data.TblHandles[0], "ExampleTable", sizeof(EXAMPLE_APP_ExampleTable_t),
                                  CFE_TBL_OPT_DEFAULT, EXAMPLE_APP_TblValidationFunc);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(EXAMPLE_APP_TABLE_REG_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Example App: Error Registering Example Table, RC = 0x%08lX", (unsigned long)status);
        }
        else
        {
            status = CFE_TBL_Load(EXAMPLE_APP_Data.TblHandles[0], CFE_TBL_SRC_FILE, EXAMPLE_APP_PLATFORM_TABLE_FILE);
        }

        CFE_Config_GetVersionString(VersionString, EXAMPLE_APP_CFG_MAX_VERSION_STR_LEN, "Example App", EXAMPLE_APP_VERSION,
                                    EXAMPLE_APP_BUILD_CODENAME, EXAMPLE_APP_LAST_OFFICIAL);

        CFE_EVS_SendEvent(EXAMPLE_APP_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "Example App Initialized.%s",
                          VersionString);
    }

    return status;
}
