#ifndef _AJ_CONNECT_H
#define _AJ_CONNECT_H
/**
 * @file aj_connect.h
 * @defgroup aj_connect Bus Connection Management
 * @{
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

#include "aj_target.h"
#include "aj_status.h"
#include "aj_bus.h"

/**
 * Set the minimum acceptable routing node protocol version.
 *
 * @param min       Minimum acceptable protocol version
 */
void AJ_SetMinProtoVersion(uint8_t min);

/**
 * Get the minimum acceptable routing node protocol version.
 *
 * @return          Minimum acceptable protocol version
 */
uint8_t AJ_GetMinProtoVersion();

/**
 * Get the routing nodes protocol version
 *
 * @return          The routing nodes protocol version
 *                  0 if not connected to a routing node
 */
AJ_EXPORT
uint8_t AJ_GetRoutingProtoVersion(void);

/**
 * Authenticate with the daemon
 *
 * @param bus                   The bus attachment to authenticate
 *
 * @return        Return AJ_Status
 */
AJ_Status AJ_Authenticate(AJ_BusAttachment* bus);

/**
 * Establish an AllJoyn connection.
 *
 * @param  bus          The bus attachment to connect.
 * @param  serviceName  Name of a specific service to connect to, NULL for the default name.
 * @param  timeout      How long to spend attempting to connect
 *
 * @return
 *         - AJ_OK if the connection was succesfully established
 *         - AJ_ERR_TIMEOUT if the connection attempt timed out
 */
AJ_EXPORT
AJ_Status AJ_Connect(AJ_BusAttachment* bus, const char* serviceName, uint32_t timeout);


/**
 * Find a daemon, connect to it and then authenticate.
 *
 * @param  bus          The bus attachment to connect.
 * @param  serviceName  Name of a specific service to connect to, NULL for the default name.
 * @param  timeout      How long to spend attempting to connect
 *
 * @return
 *         - AJ_OK if the connection was succesfully established
 *         - AJ_ERR_TIMEOUT if the connection attempt timed out
 */
AJ_EXPORT
AJ_Status AJ_FindBusAndConnect(AJ_BusAttachment* bus, const char* serviceName, uint32_t timeout);


/**
 * Terminate an AllJoyn connection
 *
 * @param  bus  The bus attachment to disconnect.
 */
AJ_EXPORT
void AJ_Disconnect(AJ_BusAttachment* bus);

/**
 * Bus authentication password function prototype for requesting a
 * password (to authenticate with the daemon) from the application.
 *
 * @param  buffer  The buffer to receive the password
 * @param  bufLen  The size of the buffer
 *
 * @return  Returns the length of the password. If the length is zero,
 *          this will be treated as a rejected password request.
 */
typedef uint32_t (*BusAuthPwdFunc)(uint8_t* buffer, uint32_t bufLen);

/**
 * Set the callback for the application to provide a password for authentication to the daemon bus
 *
 * @param callback  The callback provided by the application
 */
AJ_EXPORT
void SetBusAuthPwdCallback(BusAuthPwdFunc callback);

/**
 * @}
 */
#endif
