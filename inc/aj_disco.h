#ifndef _AJ_DISCO_H
#define _AJ_DISCO_H
/**
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

#include "aj_target.h"
#include "aj_bufio.h"

/**
 * Information about the remote service
 */
typedef struct _AJ_Service {
    uint8_t addrTypes;         /**< address type */
    uint16_t transportMask;    /**< restricts the transports the advertisement */
    uint16_t ipv4port;         /**< port number of ipv4 */
    uint16_t ipv6port;         /**< port number of ipv6 */
    uint32_t ipv4;             /**< ipv4 address */
    uint32_t ipv6[4];          /**< ipv6 address */
} AJ_Service;

/**
 * Discover a remote service
 *
 * @param prefix    The service name prefix
 * @param service   Information about the service that was found
 * @param timeout   How long to wait to discover the service
 *
 * @return        Return AJ_Status
 */
AJ_Status AJ_Discover(const char* prefix, AJ_Service* service, uint32_t timeout);

#endif
