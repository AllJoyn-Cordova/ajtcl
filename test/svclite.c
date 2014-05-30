/**
 * @file
 */
/******************************************************************************
 * Copyright (c) 2012-2014, AllSeen Alliance. All rights reserved.
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

/**
 * Per-module definition of the current module for debug logging.  Must be defined
 * prior to first inclusion of aj_debug.h
 */
#define AJ_MODULE SVCLITE

#define SECURE_INTERFACE

#include <aj_target.h>
#include <aj_link_timeout.h>
#include <aj_debug.h>
#include <alljoyn.h>
#include <aj_cert.h>
#include <aj_creds.h>
#include <aj_peer.h>
#include <aj_auth_listener.h>
#include <aj_keyexchange.h>
#include <aj_keyauthentication.h>

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
uint8_t dbgSVCLITE = 0;

/*
 * Modify these variables to change the service's behavior
 */
static const char ServiceName[] = "org.alljoyn.svclite";
static const uint16_t ServicePort = 24;
static const uint8_t CancelAdvertiseName = FALSE;
static const uint8_t ReflectSignal = FALSE;

/*
 * An application property to SET or GET
 */
static int32_t propVal = 123456;

/*
 * Default key expiration
 */
static const uint32_t keyexpiration = 0xFFFFFFFF;

/*
 * To define a secure interface, prepend '$' before the interface name, eg., "$org.alljoyn.alljoyn_test"
 */
static const char* const testInterface[] = {
#ifdef SECURE_INTERFACE
#ifdef NO_AUTH_PIN_KEYX
#error "You are defining a secure interface but not using authentication\n"
#endif
    "$org.alljoyn.alljoyn_test",
#else
    "org.alljoyn.alljoyn_test",
#endif
    "?my_ping inStr<s outStr>s",
    "?delayed_ping inStr<s delay<u outStr>s",
    "?time_ping <u <q >u >q",
    "!my_signal >a{ys}",
    NULL
};

static const char* const testValuesInterface[] = {
#ifdef SECURE_INTERFACE
    "$org.alljoyn.alljoyn_test.values",
#else
    "org.alljoyn.alljoyn_test.values",
#endif
    "@int_val=i",
    "@str_val=s",
    "@ro_val>s",
    NULL
};

static const AJ_InterfaceDescription testInterfaces[] = {
    AJ_PropertiesIface,
    testInterface,
    testValuesInterface,
    NULL
};

static const AJ_Object AppObjects[] = {
    { "/org/alljoyn/alljoyn_test", testInterfaces, AJ_OBJ_FLAG_ANNOUNCED },
    { NULL }
};

/*
 * Message identifiers for the method calls this application implements
 */
#define APP_GET_PROP        AJ_APP_MESSAGE_ID(0, 0, AJ_PROP_GET)
#define APP_SET_PROP        AJ_APP_MESSAGE_ID(0, 0, AJ_PROP_SET)
#define APP_MY_PING         AJ_APP_MESSAGE_ID(0, 1, 0)
#define APP_DELAYED_PING    AJ_APP_MESSAGE_ID(0, 1, 1)
#define APP_TIME_PING       AJ_APP_MESSAGE_ID(0, 1, 2)
#define APP_MY_SIGNAL       AJ_APP_MESSAGE_ID(0, 1, 3)

/*
 * Property identifiers for the properies this application implements
 */
#define APP_INT_VAL_PROP AJ_APP_PROPERTY_ID(0, 2, 0)

/*
 * Let the application do some work
 */
static void AppDoWork()
{
    /*
     * This function is called if there are no messages to unmarshal
     */
    AJ_InfoPrintf(("do work\n"));
}

static const char PWD[] = "123456";

static uint32_t PasswordCallback(uint8_t* buffer, uint32_t bufLen)
{
    memcpy(buffer, PWD, sizeof(PWD));
    return sizeof(PWD) - 1;
}

//static const char psk_b64[] = "EBESExQVFhcYGRobHB0eHw==";
//static uint8_t psk[16];
static const char psk_hint[] = "bob";
static const char psk_char[] = "123456";
static const char owner_pub_b64[] = "RCf5ihem02VFXvIa8EVJ1CJcJns3el0IH+H51s07rc0AAAAAn6KJifUPH1oRmPLoyBHGCg7/NT8kW67GD8kQjZh/U/AAAAAAAAAAAA==";
static const char ecc_pub_b64[] = "C/KGAyLE5jyVqHEipZBhPb7Ahj/MBdNLtpDvT9OJ0LYAAAAAn8QabXetJcPD7OWmEB6uXGXh+ftJOLlCTJhAHjTJsDkAAAAAAAAAAA==";
static const char ecc_prv_b64[] = "wxieVOfgCMgys3m+V82eV/B/p0WlIMu8fizZiqMQnYsAAAAA";
static const char owner_cert1_b64[] = "\
AAAAAUQn+YoXptNlRV7yGvBFSdQiXCZ7N3pdCB/h+dbNO63NAAAAAJ+iiYn1Dx9a\
EZjy6MgRxgoO/zU/JFuuxg/JEI2Yf1PwAAAAAAAAAAAL8oYDIsTmPJWocSKlkGE9\
vsCGP8wF00u2kO9P04nQtgAAAACfxBptd60lw8Ps5aYQHq5cZeH5+0k4uUJMmEAe\
NMmwOQAAAAAAAAAAAAAAAAAAAAAAAAAA/////wBOnWRZjvJdd9adaDleMIDQJOJC\
OuSepUTdfamDakEy/s6dN/ePP+iDV96kBT0XkQfNKiyfGbPf+ux6a2mx48/rAAAA\
AGfrER3HqAGYic+k8B/iIWUyJy414G+4+tTklxFAatmmAAAAAA==";
static const char owner_cert2_b64[] = "\
AAAAAkQn+YoXptNlRV7yGvBFSdQiXCZ7N3pdCB/h+dbNO63NAAAAAJ+iiYn1Dx9a\
EZjy6MgRxgoO/zU/JFuuxg/JEI2Yf1PwAAAAAAAAAAAL8oYDIsTmPJWocSKlkGE9\
vsCGP8wF00u2kO9P04nQtgAAAACfxBptd60lw8Ps5aYQHq5cZeH5+0k4uUJMmEAe\
NMmwOQAAAAAAAAAAAAAAAAAAAAAAAAAA/////wD5/PM2YlgaDcbxM2GD2BntTp1k\
WY7yXXfWnWg5XjCA0CTiQjrknqVE3X2pg2pBMv7ZCwVue216z7QXomTSt4nPyFum\
tj2XcycgTidW60XeVAAAAADCAWDa119gVqq2GOiteOKBaM7huRPUOl+ytTMQQpCj\
WAAAAAA=";
static ecc_publickey ecc_pub;
static ecc_privatekey ecc_prv;
//static ecc_publickey  ecc_pub;
static AJ_Certificate root_cert;

static AJ_Status AuthListenerCallback(uint32_t authmechanism, uint32_t command, AJ_Credential*cred)
{
    AJ_Status status = AJ_ERR_INVALID;
    uint8_t b8[sizeof (AJ_Certificate)];
    AJ_Printf("AuthListenerCallback authmechanism %d command %d\n", authmechanism, command);

    switch (authmechanism) {
    case AUTH_SUITE_PIN_KEYX:
        cred->mask = AJ_CRED_PRV_KEY;
        cred->data = (uint8_t*) PWD;
        cred->len = sizeof (PWD) - 1;
        cred->expiration = keyexpiration;
        status = AJ_OK;
        break;

    case AUTH_SUITE_ECDHE_NULL:
        cred->expiration = keyexpiration;
        status = AJ_OK;
        break;

    case AUTH_SUITE_ECDHE_PSK:
        switch (command) {
        case AJ_CRED_PUB_KEY:
            break; // Don't use username - use anon
            cred->mask = AJ_CRED_PUB_KEY;
            cred->data = (uint8_t*) psk_hint;
            cred->len = strlen(psk_hint);
            status = AJ_OK;
            break;

        case AJ_CRED_PRV_KEY:
            if (AJ_CRED_PUB_KEY == cred->mask) {
                AJ_Printf("Request Credentials for PSK ID: %s\n", cred->data);
            }
            cred->mask = AJ_CRED_PRV_KEY;
            cred->data = (uint8_t*) psk_char;
            cred->len = strlen(psk_char);
            cred->expiration = keyexpiration;
            status = AJ_OK;
            break;
        }
        break;

    case AUTH_SUITE_ECDHE_ECDSA:
        switch (command) {
        case AJ_CRED_PUB_KEY:
            status = AJ_B64ToRaw(ecc_pub_b64, strlen(ecc_pub_b64), b8, sizeof (b8));
            AJ_ASSERT(AJ_OK == status);
            status = AJ_DecodePublicKey(&ecc_pub, b8);
            AJ_ASSERT(AJ_OK == status);
            cred->mask = AJ_CRED_PUB_KEY;
            cred->data = (uint8_t*) &ecc_pub;
            cred->len = sizeof (ecc_pub);
            cred->expiration = keyexpiration;
            break;

        case AJ_CRED_PRV_KEY:
            status = AJ_B64ToRaw(ecc_prv_b64, strlen(ecc_prv_b64), b8, sizeof (b8));
            AJ_ASSERT(AJ_OK == status);
            status = AJ_DecodePrivateKey(&ecc_prv, b8);
            AJ_ASSERT(AJ_OK == status);
            cred->mask = AJ_CRED_PRV_KEY;
            cred->data = (uint8_t*) &ecc_prv;
            cred->len = sizeof (ecc_prv);
            cred->expiration = keyexpiration;
            break;

        case AJ_CRED_CERT_CHAIN:
            status = AJ_B64ToRaw(owner_cert1_b64, strlen(owner_cert1_b64), b8, sizeof (b8));
            AJ_ASSERT(AJ_OK == status);
            status = AJ_DecodeCertificate(&root_cert, b8, sizeof (b8));
            AJ_ASSERT(AJ_OK == status);
            cred->mask = AJ_CRED_CERT_CHAIN;
            cred->data = (uint8_t*) &root_cert;
            cred->len = sizeof (root_cert);
            break;
        }
        break;

    default:
        break;
    }
    return status;
}

static AJ_Status AppHandlePing(AJ_Message* msg)
{
    AJ_Message reply;
    AJ_Arg arg;

    AJ_UnmarshalArg(msg, &arg);
    AJ_MarshalReplyMsg(msg, &reply);
    /*
     * Just return the arg we received
     */
    AJ_MarshalArg(&reply, &arg);
    return AJ_DeliverMsg(&reply);
}

/*
 * Handles a property GET request so marshals the property value to return
 */
static AJ_Status PropGetHandler(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    if (propId == APP_INT_VAL_PROP) {
        return AJ_MarshalArgs(replyMsg, "i", propVal);
    } else {
        return AJ_ERR_UNEXPECTED;
    }
}

/*
 * Handles a property SET request so unmarshals the property value to apply.
 */
static AJ_Status PropSetHandler(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    if (propId == APP_INT_VAL_PROP) {
        return AJ_UnmarshalArgs(replyMsg, "i", &propVal);
    } else {
        return AJ_ERR_UNEXPECTED;
    }
}

uint32_t MyBusAuthPwdCB(uint8_t* buf, uint32_t bufLen)
{
    const char* myPwd = "1234";
    strncpy((char*)buf, myPwd, bufLen);
    return (uint32_t)strlen(myPwd);
}

static AJ_Status StoreIssuer()
{
    AJ_Status status;
    AJ_PeerCred cred;
    uint8_t b8[sizeof (ecc_publickey)];
    uint8_t digest[SHA256_DIGEST_LENGTH];
    AJ_SHA256_Context ctx;

    status = AJ_B64ToRaw(owner_pub_b64, strlen(owner_pub_b64), b8, sizeof (b8));
    AJ_ASSERT(AJ_OK == status);

    AJ_SHA256_Init(&ctx);
    AJ_SHA256_Update(&ctx, b8, sizeof (b8));
    AJ_SHA256_Final(&ctx, digest);

    cred.type = AJ_CRED_TYPE_DSA_PUBLIC;
    cred.idLen = sizeof (digest);
    cred.id = digest;
    cred.associationLen = 0;
    cred.data = b8;
    cred.dataLen = sizeof (b8);
    cred.expiration = keyexpiration;

    status = AJ_StoreCredential(&cred);
    AJ_ASSERT(AJ_OK == status);

    return AJ_OK;
}

static const uint32_t suites[3] = { AUTH_SUITE_ECDHE_ECDSA, AUTH_SUITE_ECDHE_PSK, AUTH_SUITE_ECDHE_NULL };
static const size_t numsuites = 3;

#define CONNECT_TIMEOUT    (1000 * 1000)
#define UNMARSHAL_TIMEOUT  (1000 * 5)

int AJ_Main()
{
    AJ_Status status = AJ_OK;
    AJ_BusAttachment bus;
    uint8_t connected = FALSE;
    uint32_t sessionId = 0;

    /*
     * One time initialization before calling any other AllJoyn APIs
     */
    AJ_Initialize();

    AJ_PrintXML(AppObjects);
    AJ_RegisterObjects(AppObjects, NULL);

    StoreIssuer();
    SetBusAuthPwdCallback(MyBusAuthPwdCB);
    while (TRUE) {
        AJ_Message msg;

        if (!connected) {
            status = AJ_StartService(&bus, NULL, CONNECT_TIMEOUT, FALSE, ServicePort, ServiceName, AJ_NAME_REQ_DO_NOT_QUEUE, NULL);
            if (status != AJ_OK) {
                continue;
            }
            AJ_InfoPrintf(("StartService returned AJ_OK\n"));
            AJ_InfoPrintf(("Connected to Daemon:%s\n", AJ_GetUniqueName(&bus)));

            connected = TRUE;

            /* Register a callback for providing bus authentication password */
            AJ_BusSetPasswordCallback(&bus, PasswordCallback);
            AJ_BusEnableSecurity(&bus, suites, numsuites);
            AJ_BusSetAuthListenerCallback(&bus, AuthListenerCallback);

            /* Configure timeout for the link to the daemon bus */
            AJ_SetBusLinkTimeout(&bus, 60); // 60 seconds
        }

        status = AJ_UnmarshalMsg(&bus, &msg, UNMARSHAL_TIMEOUT);
        if (AJ_ERR_TIMEOUT == status && AJ_ERR_LINK_TIMEOUT == AJ_BusLinkStateProc(&bus)) {
            status = AJ_ERR_READ;
        }
        if (status != AJ_OK) {
            if (status == AJ_ERR_TIMEOUT) {
                AppDoWork();
                continue;
            }
        }

        if (status == AJ_OK) {
            switch (msg.msgId) {

            case AJ_REPLY_ID(AJ_METHOD_ADD_MATCH):
                if (msg.hdr->msgType == AJ_MSG_ERROR) {
                    AJ_InfoPrintf(("Failed to add match\n"));
                    status = AJ_ERR_FAILURE;
                } else {
                    status = AJ_OK;
                }
                break;

            case AJ_METHOD_ACCEPT_SESSION:
                {
                    uint16_t port;
                    char* joiner;
                    AJ_UnmarshalArgs(&msg, "qus", &port, &sessionId, &joiner);
                    if (port == ServicePort) {
                        status = AJ_BusReplyAcceptSession(&msg, TRUE);
                        AJ_InfoPrintf(("Accepted session session_id=%u joiner=%s\n", sessionId, joiner));
                    } else {
                        status = AJ_BusReplyAcceptSession(&msg, FALSE);
                        AJ_InfoPrintf(("Accepted rejected session_id=%u joiner=%s\n", sessionId, joiner));
                    }
                }
                break;

            case APP_MY_PING:
                status = AppHandlePing(&msg);
                break;

            case APP_GET_PROP:
                status = AJ_BusPropGet(&msg, PropGetHandler, NULL);
                break;

            case APP_SET_PROP:
                status = AJ_BusPropSet(&msg, PropSetHandler, NULL);
                if (status == AJ_OK) {
                    AJ_InfoPrintf(("Property successfully set to %d.\n", propVal));
                } else {
                    AJ_InfoPrintf(("Property set attempt unsuccessful. Status = 0x%04x.\n", status));
                }
                break;

            case AJ_SIGNAL_SESSION_LOST_WITH_REASON:
                {
                    uint32_t id, reason;
                    AJ_UnmarshalArgs(&msg, "uu", &id, &reason);
                    AJ_InfoPrintf(("Session lost. ID = %u, reason = %u", id, reason));
                    if (CancelAdvertiseName) {
                        status = AJ_BusAdvertiseName(&bus, ServiceName, AJ_TRANSPORT_ANY, AJ_BUS_START_ADVERTISING, 0);
                    }
                    status = AJ_ERR_SESSION_LOST;
                }
                break;

            case AJ_SIGNAL_SESSION_JOINED:
                if (CancelAdvertiseName) {
                    status = AJ_BusAdvertiseName(&bus, ServiceName, AJ_TRANSPORT_ANY, AJ_BUS_STOP_ADVERTISING, 0);
                }
                break;

            case AJ_REPLY_ID(AJ_METHOD_CANCEL_ADVERTISE):
            case AJ_REPLY_ID(AJ_METHOD_ADVERTISE_NAME):
                if (msg.hdr->msgType == AJ_MSG_ERROR) {
                    status = AJ_ERR_FAILURE;
                }
                break;

            case APP_MY_SIGNAL:
                AJ_InfoPrintf(("Received my_signal\n"));
                status = AJ_OK;

                if (ReflectSignal) {
                    AJ_Message out;
                    AJ_MarshalSignal(&bus, &out, APP_MY_SIGNAL, msg.destination, msg.sessionId, 0, 0);
                    AJ_MarshalArgs(&out, "a{ys}", 0, NULL);
                    AJ_DeliverMsg(&out);
                    AJ_CloseMsg(&out);
                }
                break;

            default:
                /*
                 * Pass to the built-in bus message handlers
                 */
                status = AJ_BusHandleBusMessage(&msg);
                break;
            }

            // Any received packets indicates the link is active, so call to reinforce the bus link state
            AJ_NotifyLinkActive();
        }
        /*
         * Unarshaled messages must be closed to free resources
         */
        AJ_CloseMsg(&msg);

        if ((status == AJ_ERR_READ) || (status == AJ_ERR_LINK_DEAD)) {
            AJ_InfoPrintf(("AllJoyn disconnect\n"));
            AJ_InfoPrintf(("Disconnected from Daemon:%s\n", AJ_GetUniqueName(&bus)));
            AJ_Disconnect(&bus);
            connected = FALSE;
            /*
             * Sleep a little while before trying to reconnect
             */
            AJ_Sleep(10 * 1000);
        }
    }
    AJ_WarnPrintf(("svclite EXIT %d\n", status));

    return status;
}


#ifdef AJ_MAIN
int main()
{
    return AJ_Main();
}
#endif
