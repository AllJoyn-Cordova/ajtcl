#ifndef _AJ_VERSION_H
#define _AJ_VERSION_H
/**
 * @file aj_version.h
 * @defgroup aj_version Current AllJoyn Thin Client Version
 * @{
 */
/******************************************************************************
 * Copyright (c) 2013, AllSeen Alliance. All rights reserved.
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

#define AJ_MAJOR_VERSION   14    /**< major version */
#define AJ_MINOR_VERSION   2     /**< minor version */
#define AJ_RELEASE_VERSION 0     /**< release version */
#define AJ_RELEASE_TAG     "14.02.00"

#define AJ_VERSION ((AJ_MAJOR_VERSION) << 24) | ((AJ_MINOR_VERSION) << 16) | (AJ_RELEASE_VERSION)   /**< macro to generate the version from major, minor & release */

/**
 * @}
 */
#endif
