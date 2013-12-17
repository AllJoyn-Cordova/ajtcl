/*
 * @file
 */

/******************************************************************************
 * Copyright (c) 2012-2013, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "alljoyn.h"
#include "aj_debug.h"
#include "aj_crypto.h"

static const char ServiceName[] = "org.alljoyn.bus.samples.secure";
static const char InterfaceName[] = "org.alljoyn.bus.samples.secure.SecureInterface";
static const char ServicePath[] = "/SecureService";
static const uint16_t ServicePort = 42;

static const char* const secureInterface[] = {
    "$org.alljoyn.bus.samples.secure.SecureInterface",
    "?Ping inStr<s outStr>s",
    NULL
};

static const AJ_InterfaceDescription secureInterfaces[] = {
    secureInterface,
    NULL
};

/**
 * Objects implemented by the application
 */
static const AJ_Object ProxyObjects[] = {
    { ServicePath, secureInterfaces },
    { NULL }
};

#define PRX_PING   AJ_PRX_MESSAGE_ID(0, 0, 0)

/*
 * Let the application do some work
 */
static void AppDoWork()
{
}

/*
 * get a line of input from the file pointer (most likely stdin).
 * This will capture the the num-1 characters or till a newline character is
 * entered.
 *
 * @param[out] str a pointer to a character array that will hold the user input
 * @param[in]  num the size of the character array 'str'
 * @param[in]  fp the file pointer the sting will be read from. (most likely stdin)
 *
 * @return returns the length of the string received from the file.
 */
uint32_t get_line(char*str, int num, FILE*fp)
{
    uint32_t stringLength = 0;
    char*p = fgets(str, num, fp);

    // fgets will capture the '\n' character if the string entered is shorter than
    // num. Remove the '\n' from the end of the line and replace it with nul '\0'.
    if (p != NULL) {
        stringLength = (uint32_t)strlen(str) - 1;
        if (str[stringLength] == '\n') {
            str[stringLength] = '\0';
        }
    }

    return stringLength;
}

static uint32_t PasswordCallback(uint8_t* buffer, uint32_t bufLen)
{
    char inputBuffer[16];
    const int bufSize = sizeof(inputBuffer) / sizeof(inputBuffer[0]);
    uint32_t maxCopyLength;

    /* Take input from stdin and send it. */
    printf("Please enter one time password : ");
    maxCopyLength = get_line(inputBuffer, bufSize, stdin);

    if (maxCopyLength > bufLen) {
        maxCopyLength = bufLen;
    }

    memcpy(buffer, inputBuffer, maxCopyLength);
    printf("Responding with password of '%s' length %d.\n", buffer, maxCopyLength);

    return maxCopyLength;
}

#define CONNECT_TIMEOUT    (1000 * 200)
#define UNMARSHAL_TIMEOUT  (1000 * 5)
#define METHOD_TIMEOUT     (100 * 10)

static char pingString[] = "Client AllJoyn Lite says Hello AllJoyn!";

AJ_Status SendPing(AJ_BusAttachment* bus, uint32_t sessionId)
{
    AJ_Status status;
    AJ_Message msg;

    printf("Sending ping request '%s'.\n", pingString);

    status = AJ_MarshalMethodCall(bus,
                                  &msg,
                                  PRX_PING,
                                  ServiceName,
                                  sessionId,
                                  AJ_FLAG_ENCRYPTED,
                                  METHOD_TIMEOUT);
    if (AJ_OK == status) {
        status = AJ_MarshalArgs(&msg, "s", pingString);
    } else {
        printf("In SendPing() AJ_MarshalMethodCall() status = %d.\n", status);
    }

    if (AJ_OK == status) {
        status = AJ_DeliverMsg(&msg);
    } else {
        printf("In SendPing() AJ_MarshalArgs() status = %d.\n", status);
    }

    if (AJ_OK != status) {
        printf("In SendPing() AJ_DeliverMsg() status = %d.\n", status);
    }

    return status;
}

void AuthCallback(const void* context, AJ_Status status)
{
    *((AJ_Status*)context) = status;
}

static uint32_t authenticate = TRUE;

int AJ_Main(void)
{
    int done = FALSE;
    AJ_Status status = AJ_OK;
    AJ_BusAttachment bus;
    uint8_t connected = FALSE;
    uint32_t sessionId = 0;
    AJ_Status authStatus = AJ_ERR_NULL;

    /*
     * One time initialization before calling any other AllJoyn APIs
     */
    AJ_Initialize();

    AJ_PrintXML(ProxyObjects);
    AJ_RegisterObjects(NULL, ProxyObjects);

    while (!done) {
        AJ_Message msg;

        if (!connected) {
            status = AJ_StartClient(&bus, NULL, CONNECT_TIMEOUT, ServiceName, ServicePort, &sessionId, NULL);
            if (status == AJ_OK) {
                printf("StartClient returned %d, sessionId=%u\n", status, sessionId);
                connected = TRUE;
                if (authenticate) {
                    AJ_BusSetPasswordCallback(&bus, PasswordCallback);
                    authStatus = AJ_BusAuthenticatePeer(&bus, ServiceName, AuthCallback, &authStatus);
                } else {
                    authStatus = AJ_OK;
                }
            } else {
                printf("StartClient returned %d\n", status);
                break;
            }
        }

        if (authStatus != AJ_ERR_NULL) {
            if (authStatus != AJ_OK) {
                AJ_Disconnect(&bus);
                break;
            }
            authStatus = AJ_ERR_NULL;
            SendPing(&bus, sessionId);
        }

        status = AJ_UnmarshalMsg(&bus, &msg, UNMARSHAL_TIMEOUT);

        if (AJ_ERR_TIMEOUT == status) {
            AppDoWork();
            continue;
        }

        if (AJ_OK == status) {
            switch (msg.msgId) {
            case AJ_REPLY_ID(PRX_PING):
                {
                    AJ_Arg arg;

                    if (AJ_OK == AJ_UnmarshalArg(&msg, &arg)) {
                        printf("%s.Ping (path=%s) returned \"%s\".\n", InterfaceName,
                               ServicePath, arg.val.v_string);

                        if (strcmp(arg.val.v_string, pingString) == 0) {
                            printf("Ping was successful.\n");
                        } else {
                            printf("Ping returned different string.\n");
                        }
                    } else {
                        printf("Bad ping response.\n");
                    }

                    done = TRUE;
                }
                break;

            case AJ_SIGNAL_SESSION_LOST_WITH_REASON:
                /*
                 * Force a disconnect
                 */
                {
                    uint32_t id, reason;
                    AJ_UnmarshalArgs(&msg, "uu", &id, &reason);
                    printf("Session lost. ID = %u, reason = %u", id, reason);
                }
                status = AJ_ERR_READ;
                break;

            default:
                /*
                 * Pass to the built-in handlers
                 */
                status = AJ_BusHandleBusMessage(&msg);
                break;
            }
        }

        /*
         * Messages must be closed to free resources
         */
        AJ_CloseMsg(&msg);

        if (status == AJ_ERR_READ) {
            printf("AllJoyn disconnect.\n");
            AJ_Disconnect(&bus);
            exit(0);
        }
    }

    printf("SecureClient EXIT %d.\n", status);

    return status;
}

#ifdef AJ_MAIN
int main()
{
    return AJ_Main();
}
#endif
