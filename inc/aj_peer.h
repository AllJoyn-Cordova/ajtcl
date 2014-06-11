#ifndef _AJ_PEER_H
#define _AJ_PEER_H
/**
 * @file aj_peer.h
 * @defgroup aj_peer Implementation of org.alljoyn.Bus.Peer Object
 * @{
 */
/******************************************************************************
 * Copyright (c) 2012-2014 AllSeen Alliance. All rights reserved.
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

#include "aj_target.h"
#include "aj_msg.h"

/**
 * Handle an exchange guids message
 *
 * @param msg    The exchange guids message
 * @param reply  The guids reply message
 *
 * @return        Return AJ_Status
 *          - AJ_OK if successfully handled
 *          - AJ_ERR_RESOURCES if the arguments are too big to marshal into the message buffer
 *          - AJ_ERR_WRITE if there was a write failure
 *          - AJ_ERR_UNEXPECTED if any of the argument types in the signature is not a basic type
 */
AJ_Status AJ_PeerHandleExchangeGUIDs(AJ_Message* msg, AJ_Message* reply);

/**
 * Handle an exchange guids reply message
 *
 * @param msg    The exchange guids reply
 *
 * @return        Return AJ_Status
 *          - AJ_OK if successfully handled
 *          - AJ_ERR_UNMARSHAL if the message was badly formed
 *          - AJ_ERR_RESOURCES if the message header is too big to unmarshal into the attached buffer
 *          - AJ_ERR_TIMEOUT if there was no message to unmarshal within the timeout period
 *          - AJ_ERR_READ if there was a read failure
 */
AJ_Status AJ_PeerHandleExchangeGUIDsReply(AJ_Message* msg);

/**
 * Handle a gen session key message
 *
 * @param msg    The gen session key message
 * @param reply  The gen session key reply message
 *
 * @return        Return AJ_Status
 */
AJ_Status AJ_PeerHandleGenSessionKey(AJ_Message* msg, AJ_Message* reply);

/**
 * Handle a gen session key reply message
 *
 * @param msg  The gen session key reply message
 *
 * @return        Return AJ_Status
 */
AJ_Status AJ_PeerHandleGenSessionKeyReply(AJ_Message* msg);

/**
 * Handle an exchange group keys message
 *
 * @param msg    The exchange group keys message
 * @param reply  The group keys reply message
 *
 * @return        Return AJ_Status
 */
AJ_Status AJ_PeerHandleExchangeGroupKeys(AJ_Message* msg, AJ_Message* reply);

/**
 * Handle an exchange group keys reply message
 *
 * @param msg  The exchange group keys reply message
 *
 * @return        Return AJ_Status
 */
AJ_Status AJ_PeerHandleExchangeGroupKeysReply(AJ_Message* msg);

/**
 * Callback function prototype for the function called when an authentication completes or fails.
 *
 * @param context   The context provided when AJ_PeerAuthenticate() was called.
 * @param status    A status code indicating if the authentication was succesful
 *                  - AJ_OK indicates the authentication succeeded
 *                  - AJ_ERR_SECURITY indicates the authentication failed
 *                  - AJ_ERR_TIMEOUT indciates the authentication timed-out
 */
typedef void (*AJ_PeerAuthenticateCallback)(const void* context, AJ_Status status);

/**
 * Initiate a secure connection to a remote peer authenticating if necessary.
 *
 * @param bus        The bus attachment
 * @param peerName   The bus name of the remove peer to secure.
 * @param callback   A function to be called when the authentication completes
 * @param cbContext  A caller provided context to pass to the callback function
 *
 * @return   Return AJ_Status
 *         - AJ_OK if the request was sent
 *         - An error status otherwise
 */
AJ_Status AJ_PeerAuthenticate(AJ_BusAttachment* bus, const char* peerName, AJ_PeerAuthenticateCallback callback, void* cbContext);

/**
 * Handle an exchange suites message
 *
 * @param msg    The exchange suites message
 * @param reply  The exchange suites reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_PeerHandleExchangeSuites(AJ_Message* msg, AJ_Message* reply);

/**
 * Handle an exchange suites reply message
 *
 * @param msg    The exchange suites reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_PeerHandleExchangeSuitesReply(AJ_Message* msg);

/**
 * Handle a key exchange message
 *
 * @param msg    The key exchange message
 * @param reply  The key exchange reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_PeerHandleKeyExchange(AJ_Message* msg, AJ_Message* reply);

/**
 * Handle a key exchange reply message
 *
 * @param msg    The key exchange reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_PeerHandleKeyExchangeReply(AJ_Message* msg);

/**
 * Handle a key authentication message
 *
 * @param msg    The key authentication message
 * @param reply  The key authentication reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_PeerHandleKeyAuthentication(AJ_Message* msg, AJ_Message* reply);

/**
 * Handle a key authentication reply message
 *
 * @param msg    The key authentication reply message
 *
 * @return   Return AJ_Status
 *         - AJ_OK if successful
 *         - AJ_ERR_RESOURCES if resource error or authentication in progress
 *         - AJ_ERR_SECURITY if generic security violation
 */
AJ_Status AJ_PeerHandleKeyAuthenticationReply(AJ_Message* msg);

/**
 * Clear the authentication handshake context
 */
void AJ_ClearAuthContext();

/* the key exchange is in the 16 MSB */
#define AUTH_KEYX_ANONYMOUS     0x00010000
#define AUTH_KEYX_EXTERNAL      0x00020000
#define AUTH_KEYX_PIN           0x00040000
#define AUTH_KEYX_SRP           0x00080000
#define AUTH_KEYX_SRP_LOGON     0x00100000
#define AUTH_KEYX_RSA           0x00200000
#define AUTH_KEYX_ECDHE         0x00400000

/*the key authentication suite is in the 16 LSB */

#define AUTH_SUITE_ANONYMOUS    AUTH_KEYX_ANONYMOUS
#define AUTH_SUITE_EXTERNAL     AUTH_KEYX_EXTERNAL
#define AUTH_SUITE_PIN_KEYX     AUTH_KEYX_PIN
#define AUTH_SUITE_SRP_KEYX     AUTH_KEYX_SRP
#define AUTH_SUITE_SRP_LOGON    AUTH_KEYX_SRP_LOGON
#define AUTH_SUITE_RSA_KEYX     AUTH_KEYX_RSA

#define AUTH_SUITE_ECDHE_NULL   (AUTH_KEYX_ECDHE | 0x0001)
#define AUTH_SUITE_ECDHE_PSK    (AUTH_KEYX_ECDHE | 0x0002)
#define AUTH_SUITE_ECDHE_ECDSA  (AUTH_KEYX_ECDHE | 0x0004)

/**
 * @}
 */
#endif
