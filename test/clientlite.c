/*
 * clientlite.c
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

#define AJ_MODULE CLIENTLITE
#define SECURE_INTERFACE

#include <aj_target.h>
#include <alljoyn.h>
#include <aj_cert.h>
#include <aj_peer.h>
#include <aj_creds.h>
#include <aj_auth_listener.h>
#include <aj_keyexchange.h>
#include <aj_keyauthentication.h>



uint8_t dbgCLIENTLITE = 0;
static const char ServiceName[] = "org.alljoyn.svclite";
static const uint16_t ServicePort = 24;

/*
 * Default key expiration
 */
static const uint32_t keyexpiration = 0xFFFFFFFF;

/*
 * The app should authenticate the peer if one or more interfaces are secure
 * To define a secure interface, prepend '$' before the interface name, eg., "$org.alljoyn.alljoyn_test"
 */
#ifdef SECURE_INTERFACE
#ifdef NO_AUTH_PIN_KEYX
#error "You are defining a secure interface but not using authentication\n"
#endif
static const char testInterfaceName[] = "$org.alljoyn.alljoyn_test";
static const char testValuesInterfaceName[] = "$org.alljoyn.alljoyn_test.values";
#else
static const char testInterfaceName[] = "org.alljoyn.alljoyn_test";
static const char testValuesInterfaceName[] = "org.alljoyn.alljoyn_test.values";
#endif

static const char* const testInterface[] = {
    testInterfaceName,
    "?my_ping inStr<s outStr>s",
    NULL
};


static const char* const testValuesInterface[] = {
    testValuesInterfaceName,
    "@int_val=i",
    NULL
};

static const AJ_InterfaceDescription testInterfaces[] = {
    AJ_PropertiesIface,
    testInterface,
    testValuesInterface,
    NULL
};

static const char testObj[] = "/org/alljoyn/alljoyn_test";

/**
 * Objects implemented by the application
 */
static AJ_Object ProxyObjects[] = {
    { NULL, testInterfaces },    /* Object path will be specified later */
    { NULL }
};

#define PRX_GET_PROP  AJ_PRX_MESSAGE_ID(0, 0, AJ_PROP_GET)
#define PRX_SET_PROP  AJ_PRX_MESSAGE_ID(0, 0, AJ_PROP_SET)
#define PRX_MY_PING   AJ_PRX_MESSAGE_ID(0, 1, 0)
#define PRX_GET_INT   AJ_PRX_PROPERTY_ID(0, 2, 0)
#define PRX_SET_INT   AJ_PRX_PROPERTY_ID(0, 2, 0)

/*
 * Let the application do some work
 */
static AJ_Status SendPing(AJ_BusAttachment* bus, uint32_t sessionId, unsigned int num);
static int32_t g_iterCount = 0;
static void AppDoWork(AJ_BusAttachment* bus, uint32_t sessionId)
{
    AJ_Printf("AppDoWork\n");
    /*
     * This function is called if there are no messages to unmarshal
     */
    g_iterCount = g_iterCount + 1;
    SendPing(bus, sessionId, g_iterCount);
}

static const char PWD[] = "123456";

#ifdef SECURE_INTERFACE
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
static const char ecc_pub_b64[] = "JmZC779f7YjYPa3rU0xdifnW0qyiCmmUXcN1XExC334AAAAA1j95MCfIAFa6Fpa5vJ+2tUMfYVmhny04itEwJPnfDqAAAAAAAAAAAA==";
static const char ecc_prv_b64[] = "koWEteat13YRYrv/olCqEmMg7YufcTsjSQNbIL1ue+wAAAAA";
static const char owner_cert1_b64[] = "\
AAAAAUQn+YoXptNlRV7yGvBFSdQiXCZ7N3pdCB/h+dbNO63NAAAAAJ+iiYn1Dx9a\
EZjy6MgRxgoO/zU/JFuuxg/JEI2Yf1PwAAAAAAAAAAAmZkLvv1/tiNg9retTTF2J\
+dbSrKIKaZRdw3VcTELffgAAAADWP3kwJ8gAVroWlrm8n7a1Qx9hWaGfLTiK0TAk\
+d8OoAAAAAAAAAAAAAAAAAAAAAAAAAAA/////wBOnWRZjvJdd9adaDleMIDQJOJC\
OuSepUTdfamDakEy/rQbXYuqvmUj1ZiGGpPYBfh7aNkFE4rng9TixhKXJ15XAAAA\
AN6X04g62BUVvnCbFuBiw2r783HQeBdGUdUrsnVoHUKkAAAAAA==";
static const char owner_cert2_b64[] = "\
AAAAAkQn+YoXptNlRV7yGvBFSdQiXCZ7N3pdCB/h+dbNO63NAAAAAJ+iiYn1Dx9a\
EZjy6MgRxgoO/zU/JFuuxg/JEI2Yf1PwAAAAAAAAAAAmZkLvv1/tiNg9retTTF2J\
+dbSrKIKaZRdw3VcTELffgAAAADWP3kwJ8gAVroWlrm8n7a1Qx9hWaGfLTiK0TAk\
+d8OoAAAAAAAAAAAAAAAAAAAAAAAAAAA/////wD5/PM2YlgaDcbxM2GD2BntTp1k\
WY7yXXfWnWg5XjCA0CTiQjrknqVE3X2pg2pBMv4X9K7ntr5Z4AQzJnz9DaHh0clG\
WYk3iayjtM2IUTldlgAAAAALQdeFHaHyScnOSPXzaHV/tLCTPKogvpv4gWOfQAsy\
2AAAAAA=";
static ecc_publickey ecc_pub;
static ecc_privatekey ecc_prv;
static AJ_Certificate root_cert;

static AJ_Status AuthListenerCallback(uint32_t authmechanism, uint32_t command, AJ_Credential* cred)
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
#endif

#define CONNECT_TIMEOUT    (1000 * 200)
#define UNMARSHAL_TIMEOUT  (1000 * 5)
#define METHOD_TIMEOUT     (100 * 10)

static const char PingString[] = "Ping String";

AJ_Status SendPing(AJ_BusAttachment* bus, uint32_t sessionId, unsigned int num)
{
    AJ_Status status;
    AJ_Message msg;

    /*
     * Since the object path on the proxy object entry was not set in the proxy object table above
     * it must be set before marshalling the method call.
     */
    status = AJ_SetProxyObjectPath(ProxyObjects, PRX_MY_PING, testObj);
    if (status == AJ_OK) {
        status = AJ_MarshalMethodCall(bus, &msg, PRX_MY_PING, ServiceName, sessionId, 0, METHOD_TIMEOUT);
    }
    if (status == AJ_OK) {
        status = AJ_MarshalArgs(&msg, "s", PingString);
    }
    if (status == AJ_OK) {
        status = AJ_DeliverMsg(&msg);
    }
    return status;
}

AJ_Status SendGetProp(AJ_BusAttachment* bus, uint32_t sessionId)
{
    AJ_Status status;
    AJ_Message msg;

    status = AJ_MarshalMethodCall(bus, &msg, PRX_GET_PROP, ServiceName, sessionId, 0, METHOD_TIMEOUT);
    if (status == AJ_OK) {
        AJ_MarshalPropertyArgs(&msg, PRX_GET_INT);
        status = AJ_DeliverMsg(&msg);
    }
    return status;
}

AJ_Status SendSetProp(AJ_BusAttachment* bus, uint32_t sessionId, int val)
{
    AJ_Status status;
    AJ_Message msg;

    status = AJ_MarshalMethodCall(bus, &msg, PRX_SET_PROP, ServiceName, sessionId, 0, METHOD_TIMEOUT);
    if (status == AJ_OK) {
        status = AJ_MarshalPropertyArgs(&msg, PRX_SET_INT);

        if (status == AJ_OK) {
            status = AJ_MarshalArgs(&msg, "i", val);
        } else {
            AJ_Printf(">>>>>>>>In SendSetProp() AJ_MarshalPropertyArgs() returned status = 0x%04x\n", status);
        }

        if (status == AJ_OK) {
            status = AJ_DeliverMsg(&msg);
        } else {
            AJ_Printf(">>>>>>>>In SendSetProp() AJ_MarshalArgs() returned status = 0x%04x\n", status);
        }
    }

    return status;
}

void AuthCallback(const void* context, AJ_Status status)
{
    *((AJ_Status*)context) = status;
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

#ifdef MAIN_ALLOWS_ARGS
int AJ_Main(int ac, char** av)
#else
int AJ_Main()
#endif
{
    AJ_Status status = AJ_OK;
    AJ_BusAttachment bus;
    uint8_t connected = FALSE;
    uint32_t sessionId = 0;
    AJ_Status authStatus = AJ_ERR_NULL;
    uint32_t suites[16];
    size_t numsuites = 0;
    uint8_t clearkeys = FALSE;
    uint8_t enablepwd = FALSE;

#ifdef MAIN_ALLOWS_ARGS
#ifdef SECURE_INTERFACE
    ac--;
    av++;
    /*
     * Enable authentication mechanism by command line
     */
    if (ac) {
        if (0 == strncmp(*av, "-ek", 3)) {
            clearkeys = TRUE;
            ac--;
            av++;
        } else if (0 == strncmp(*av, "-e", 2)) {
            ac--;
            av++;
        }
        if (!ac) {
            AJ_Printf("-e(k) requires an auth mechanism.\n");
            return 1;
        }
        while (ac) {
            if (0 == strncmp(*av, "ECDHE_ECDSA", 11)) {
                suites[numsuites++] = AUTH_SUITE_ECDHE_ECDSA;
            } else if (0 == strncmp(*av, "ECDHE_PSK", 9)) {
                suites[numsuites++] = AUTH_SUITE_ECDHE_PSK;
            } else if (0 == strncmp(*av, "ECDHE_NULL", 10)) {
                suites[numsuites++] = AUTH_SUITE_ECDHE_NULL;
            } else if (0 == strncmp(*av, "PIN", 3)) {
                enablepwd = TRUE;
            }
            ac--;
            av++;
        }
    }
#endif
#endif

    /*
     * One time initialization before calling any other AllJoyn APIs
     */
    AJ_Initialize();

    AJ_PrintXML(ProxyObjects);
    AJ_RegisterObjects(NULL, ProxyObjects);

    while (TRUE) {
        AJ_Message msg;

        if (!connected) {
            status = AJ_StartClient(&bus, NULL, CONNECT_TIMEOUT, FALSE, ServiceName, ServicePort, &sessionId, NULL);
            if (status == AJ_OK) {
                AJ_Printf("StartClient returned %d, sessionId=%u\n", status, sessionId);
                AJ_Printf("Connected to Daemon:%s\n", AJ_GetUniqueName(&bus));
                connected = TRUE;
#ifdef SECURE_INTERFACE
                if (enablepwd) {
                    AJ_BusSetPasswordCallback(&bus, PasswordCallback);
                }
                AJ_BusEnableSecurity(&bus, suites, numsuites);
                AJ_BusSetAuthListenerCallback(&bus, AuthListenerCallback);
                if (clearkeys) {
                    status = AJ_ClearCredentials();
                    AJ_ASSERT(AJ_OK == status);
                }
                StoreIssuer();
                status = AJ_BusAuthenticatePeer(&bus, ServiceName, AuthCallback, &authStatus);
                if (status != AJ_OK) {
                    AJ_Printf("AJ_BusAuthenticatePeer returned %d\n", status);
                }
#else
                authStatus = AJ_OK;
#endif
            } else {
                AJ_Printf("StartClient returned %d\n", status);
                break;
            }
        }

        if (authStatus != AJ_ERR_NULL) {
            if (authStatus != AJ_OK) {
                AJ_Disconnect(&bus);
                break;
            }
            authStatus = AJ_ERR_NULL;
            AJ_BusSetLinkTimeout(&bus, sessionId, 10 * 1000);
        }

        AJ_Printf("Auth status %d and AllJoyn status %d\n", authStatus, status);

        status = AJ_UnmarshalMsg(&bus, &msg, UNMARSHAL_TIMEOUT);
        if (status != AJ_OK) {
            if (status == AJ_ERR_TIMEOUT) {
                AppDoWork(&bus, sessionId);
                continue;
            }
        } else {
            switch (msg.msgId) {

            case AJ_REPLY_ID(AJ_METHOD_SET_LINK_TIMEOUT):
                {
                    uint32_t disposition;
                    uint32_t timeout;
                    status = AJ_UnmarshalArgs(&msg, "uu", &disposition, &timeout);
                    if (disposition == AJ_SETLINKTIMEOUT_SUCCESS) {
                        AJ_Printf("Link timeout set to %d\n", timeout);
                    } else {
                        AJ_Printf("SetLinkTimeout failed %d\n", disposition);
                    }
                    SendPing(&bus, sessionId, 1);
                }
                break;

            case AJ_REPLY_ID(PRX_MY_PING):
                {
                    AJ_Arg arg;
                    AJ_UnmarshalArg(&msg, &arg);
                    status = SendGetProp(&bus, sessionId);
                }
                break;

            case AJ_REPLY_ID(PRX_GET_PROP):
                {
                    const char* sig;
                    status = AJ_UnmarshalVariant(&msg, &sig);
                    if (status == AJ_OK) {
                        status = AJ_UnmarshalArgs(&msg, sig, &g_iterCount);
                        AJ_Printf("Get prop reply %d\n", g_iterCount);

                        if (status == AJ_OK) {
                            g_iterCount = g_iterCount + 1;
                            status = SendSetProp(&bus, sessionId, g_iterCount);
                        }
                    }
                }
                break;

            case AJ_REPLY_ID(PRX_SET_PROP):
                AJ_Printf("Set prop reply\n");
                break;

            case AJ_SIGNAL_SESSION_LOST_WITH_REASON:
                /*
                 * Force a disconnect
                 */
                {
                    uint32_t id, reason;
                    AJ_UnmarshalArgs(&msg, "uu", &id, &reason);
                    AJ_Printf("Session lost. ID = %u, reason = %u\n", id, reason);
                }
                status = AJ_ERR_SESSION_LOST;
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

        if ((status == AJ_ERR_SESSION_LOST) || (status == AJ_ERR_READ) || (status == AJ_ERR_LINK_DEAD)) {
            AJ_Printf("AllJoyn disconnect\n");
            AJ_Printf("Disconnected from Daemon:%s\n", AJ_GetUniqueName(&bus));
            AJ_Disconnect(&bus);
            return status;
        }
    }
    AJ_Printf("clientlite EXIT %d\n", status);

    return status;
}

#ifdef AJ_MAIN
#ifdef MAIN_ALLOWS_ARGS
int main(int ac, char** av)
{
    return AJ_Main(ac, av);
}
#else
int main(int ac, char** av)
{
    return AJ_Main();
}
#endif
#endif
