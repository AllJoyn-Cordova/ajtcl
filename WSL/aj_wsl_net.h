/**
 * @file Network functionality function declarations
 */
/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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


#ifndef AJ_WSL_NET_H_
#define AJ_WSL_NET_H_

#include "aj_target.h"
#include "aj_wsl_target.h"
#include "aj_status.h"

#pragma pack(push, 1)

typedef uint8_t AJ_WSL_SOCKNUM;

#define AJ_WSL_SCAN_LIST_SIZE 30

typedef struct _wsl_scan_item {
    char* ssid;
    uint8_t rssi;
    uint8_t bssid[6];
} wsl_scan_item;

typedef struct _wsl_scan_list {
    wsl_scan_item list[AJ_WSL_SCAN_LIST_SIZE];
    uint16_t size;
} wsl_scan_list;

typedef enum {
    WSL_NET_SCAN,
    WSL_NET_CONNECT,
    WSL_NET_DISCONNECT,
    WSL_NET_IPCONFIG,
    WSL_NET_IP6CONFIG,
    WSL_NET_OPEN,
    WSL_NET_CLOSE,
    WSL_NET_BIND,
    WSL_NET_DATA_RX,
    WSL_NET_DATA_TX,
    WSL_NET_INTERUPT,
    WSL_NET_SETSOCKOPTS
} AJ_WSL_NET_COMMAND;

typedef enum {
    WSL_NET_AUTH_NONE       = 0x01,
    WSL_NET_AUTH_WPA        = 0x02,
    WSL_NET_AUTH_WPA2       = 0x04,
    WSL_NET_AUTH_WPA_PSK    = 0x08,
    WSL_NET_AUTH_WPA2_PSK   = 0x10,
    WSL_NET_AUTH_WPA_CCKM   = 0x20,
    WSL_NET_AUTH_WPA2_CCKM  = 0x40
}WSL_NET_AUTH_MODE;

typedef enum {
    WSL_NET_CRYPTO_NONE = 0x01,
    WSL_NET_CRYPTO_WEP  = 0x02,
    WSL_NET_CRYPTO_TKIP = 0x04,
    WSL_NET_CRYPTO_AES  = 0x08
}WSL_NET_CRYPTO_TYPE;

typedef enum {
    WSL_AF_INET6        = 3,
    WSL_AF_INET         = 2
}WSL_SOCK_DOMAIN;

typedef enum {
    WSL_SOCK_STREAM     = 1,
    WSL_SOCK_DGRAM      = 2
}WSL_SOCK_TYPE;

typedef enum {
    WSL_ADD_MEMBERSHIP  = 12,
    WSL_DROP_MEMBERSHIP = 13,
    WSL_JOIN_GROUP      = 83,
    WSL_LEAVE_GROUP     = 84
}WSL_MULTICAST_OPT;

typedef enum {
    WSL_IPPROTO_IP = 0,
    WSL_IPPROTO_UDP = 17,
    WSL_IPPROTO_TCP = 6
}WSL_MCAST_LEVEL;

#define AJ_IPV4_MCAST_GROUP     0xe0000071  //0x710000e0  //224.0.0.113 - Alljoyn Multicast Group
#define AJ_INADDR_ANY           0x00000000  //0.0.0.0

/*
 * Initiate a wifi scan. This will populate a list of SSID/RSSI/BSSID's
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_scan(void);
/**
 * Connect to a wifi network
 *
 * @param SSID          The SSID of the network
 * @param passphrase    The access points password
 * @param auth          The type of authentication (NONE, WPA, WPA2 ...)
 * @param crypto        The type of cryptography used (WEP, TKIP, AES ...)
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_connect(const char* SSID, const char* passphrase, WSL_NET_AUTH_MODE auth, WSL_NET_CRYPTO_TYPE crypto, uint8_t softAP);

/**
 * Disconnect from the current wifi network
 *
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_disconnect(void);

/**
 * Used to configure IP parameters
 *
 * @param mode          What type of configuration are you doing (QUERY, STATIC, DHCP)
 * @param ip            IP address (either an uninitialized pointer (DHCP/QUERY) or
 *                      an actual address for static ip configuration
 * @param mask          subnet mask
 * @param gateway       router gateway
 * @param softAP        Flag to start up a softAP rather than connect to an access point
 *
 * @return              success code
 */
AJ_EXPORT AJ_Status AJ_WSL_ipconfig(uint32_t mode, uint32_t* ip, uint32_t* mask, uint32_t* gateway);

/**
 * Send data over a socket
 *
 * @param socket        The socket your sending over
 * @param data          Pointer to the data your sending
 * @param size          Size of the data your sending
 * @param timeout       Max time to try to send
 *
 * @return              Number of bytes sent (not done yet)
 */
AJ_EXPORT int16_t AJ_WSL_NET_socket_send(uint32_t socket, uint8_t* data, uint16_t size, uint32_t timeout);

/**
 * Send to a specific address
 *
 * @param socket        Socket your sending data over
 * @param data          Pointer to the data your sending
 * @param size          Size of the data
 * @param addr          IP address you wish to send to
 * @param port          Port to send over
 * @param timeout       How long you try and send the data
 *
 * @return              Number of bytes that was sent
 */
AJ_EXPORT int16_t AJ_WSL_NET_socket_sendto(uint32_t socket, uint8_t* data, uint16_t size, uint32_t addr, uint16_t port, uint32_t timeout);

/**
 * Open a socket
 *
 * @param domain        Domain your opening (INET, INET6)
 * @param type          Type of socket (SOCK_STREAM_TYPE, SOCK_DGRAM_TYPE)
 * @param protocol      Usually zero
 *
 * @return              Returns a socket number
 */
AJ_EXPORT int8_t AJ_WSL_NET_socket_open(uint16_t domain, uint16_t type, uint16_t protocol);

/**
 * Ping an address
 *
 * @param addr          Address to send a ping to
 * @param size          Size of the ping packet
 */
AJ_EXPORT void AJ_WSL_NET_ping(uint32_t addr, uint32_t size);

/**
 * Close a socket
 *
 * @param socketHandle  The handle of the socket to close
 * @return              AJ_OK on success
 *                      AJ_ERR_LINK_DEAD if the socket didn't exist
 *                      AJ_ERR_NULL if there was an unexpected error
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_socket_close(AJ_WSL_SOCKNUM socketHandle);

/**
 * Connect to another socket
 *
 * @param sock          The opened socket your using
 * @param addr          The address of the machine your connecting to
 * @param port          The open port at the address
 * @param family        The socket family of the connection (AF_INET, AF_INET6)
 *
 * @return              1 on connection success
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_socket_connect(AJ_WSL_SOCKNUM sock, uint32_t addr, uint16_t port, uint16_t family);

/**
 * Bind to a port
 *
 * @param sock          The socket handle your binding the port to
 * @param addr          Address to associate to that port
 * @param port          The port your binding to
 *
 * @return              1 on bind success
 * @return              AJ_OK on success
 *                      AJ_ERR_LINK_DEAD if the socket didn't exist
 *                      AJ_ERR_NULL if there was an unexpected error
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_bind(AJ_WSL_SOCKNUM sock, uint32_t addr, uint16_t port);

/**
 * Receive bytes of data over a socket
 *
 * @param sock          Socket to receive data
 * @param buffer        Buffer to put the data into
 * @param size          How much data is coming
 * @param timeout       How long to try and reveive for
 *
 * @return              Number of bytes received
 */
AJ_EXPORT int16_t AJ_WSL_NET_socket_recv(AJ_WSL_SOCKNUM sock, uint8_t* buffer, uint32_t sizeBuffer, uint32_t timeout);

/**
 * Set custom options for an opened socket
 *
 * @param socket        The socket your configuring
 * @param level         Options are: WSL_IPPROTO_IP, WSL_IPPROTO_UDP, WSL_IPPROTO_TCP
 * @param optname       What option your setting:   WSL_ADD_MEMBERSHIP,
 *                                                  WSL_DROP_MEMBERSHIP,
 *                                                  WSL_JOIN_GROUP,
 *                                                  WSL_LEAVE_GROUP
 * @param optlen        The length of the options that follow
 * @param optval        An array of option values e.g. {AJ_IPV4_MCAST_GROUP, AJ_INADDR_ANY}
 *
 * @return              success or error code
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_set_sock_options(uint32_t socket, uint32_t level, uint32_t optname, uint32_t optlen, uint8_t* optval);

/*
 * Set the host name on the QCA4002
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_set_hostname(const char* hostname);
/**
 * Used to configure IPV6 router prefix
 *
 * @param ipv6addr            IP address
 * @param gateway       router gateway
 * @param softAP        Flag to start up a softAP rather than connect to an access point
 *
 * @return              success code
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_ip6config_router_prefix(const uint8_t* ipv6addr, uint32_t prefix_length, uint32_t lifetimePrefix, uint32_t lifetimeValid);



/**
 * Used to configure DHCP pool address range and lease time
 *
 * @param startIP       first IP address to give out
 * @param endIP         last IP address to give out
 * @param leaseTime     amount of time a DHCP address is valid
 *
 * @return              success code
 */
AJ_EXPORT AJ_Status AJ_WSL_NET_ipconfig_dhcp_pool(const uint32_t* startIP, const uint32_t* endIP, uint32_t leaseTime);

#pragma pack(pop)
#endif /* AJ_WSL_NET_H_ */
