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
#define AJ_MODULE NET

#include <Winsock2.h>
#include <Mswsock.h>

#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "aj_target.h"
#include "aj_bufio.h"
#include "aj_net.h"
#include "aj_util.h"
#include "aj_debug.h"

static void WinsockCheck()
{
    static uint8_t initialized = FALSE;
    if (!initialized) {
        WSADATA wsaData;
        WORD version = MAKEWORD(2, 0);
        int ret;
        AJ_InfoPrintf(("WinsockCheck\n"));

        ret = WSAStartup(version, &wsaData);
        if (ret) {
            AJ_ErrPrintf(("WSAStartup failed with error: %d\n", ret));
        } else {
            initialized = TRUE;
        }
    }
}

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
#ifndef NDEBUG
uint8_t dbgNET = 0;
#endif

/*
 * IANA assigned IPv4 multicast group for AllJoyn.
 */
static const char AJ_IPV4_MULTICAST_GROUP[] = "224.0.0.113";

/*
 * IANA assigned IPv6 multicast group for AllJoyn.
 */
static const char AJ_IPV6_MULTICAST_GROUP[] = "ff02::13a";

/*
 * IANA assigned UDP multicast port for AllJoyn
 */
#define AJ_UDP_PORT 9956

static AJ_Status AJ_Net_Send(AJ_IOBuffer* buf)
{
    DWORD ret;
    DWORD tx = AJ_IO_BUF_AVAIL(buf);

    AJ_InfoPrintf(("AJ_Net_Send(buf=0x%p)\n", buf));

    assert(buf->direction == AJ_IO_BUF_TX);

    if (tx > 0) {
        ret = send((SOCKET)buf->context, buf->readPtr, tx, 0);
        if (ret == SOCKET_ERROR) {
            AJ_ErrPrintf(("AJ_Net_Send(): send() failed. WSAGetLastError()=0x%x, status=AJ_ERR_WRITE\n", WSAGetLastError()));
            return AJ_ERR_WRITE;
        }
        buf->readPtr += ret;
    }
    if (AJ_IO_BUF_AVAIL(buf) == 0) {
        AJ_IO_BUF_RESET(buf);
    }
    AJ_InfoPrintf(("AJ_Net_Send(): status=AJ_OK\n"));
    return AJ_OK;
}

static AJ_Status AJ_Net_Recv(AJ_IOBuffer* buf, uint32_t len, uint32_t timeout)
{
    AJ_Status status = AJ_OK;
    DWORD rx = AJ_IO_BUF_SPACE(buf);
    fd_set fds;
    int rc = 0;
    const struct timeval tv = { timeout / 1000, 1000 * (timeout % 1000) };

    AJ_InfoPrintf(("AJ_Net_Recv(buf=0x%p, len=%d., timeout=%d.)\n", buf, len, timeout));

    assert(buf->direction == AJ_IO_BUF_RX);

    FD_ZERO(&fds);
    FD_SET((SOCKET)buf->context, &fds);
    rc = select(1, &fds, NULL, NULL, &tv);
    if (rc == 0) {
        return AJ_ERR_TIMEOUT;
    }

    rx = min(rx, len);
    if (rx) {
        DWORD ret = recv((SOCKET)buf->context, buf->writePtr, rx, 0);
        if ((ret == SOCKET_ERROR) || (ret == 0)) {
            AJ_ErrPrintf(("AJ_Net_Recv(): recv() failed. WSAGetLastError()=0x%x, status=AJ_ERR_READ\n", WSAGetLastError()));
            status = AJ_ERR_READ;
        } else {
            buf->writePtr += ret;
        }
    }
    AJ_InfoPrintf(("AJ_Net_Recv(): status=%s\n", AJ_StatusText(status)));
    return status;
}

/*
 * Statically sized buffers for I/O
 */
static uint8_t rxData[1024];
static uint8_t txData[1024];

AJ_Status AJ_Net_Connect(AJ_NetSocket* netSock, uint16_t port, uint8_t addrType, const uint32_t* addr)
{
    DWORD ret;
    SOCKADDR_STORAGE addrBuf;
    socklen_t addrSize;
    SOCKET sock;

    AJ_InfoPrintf(("AJ_Net_Connect(nexSock=0x%p, port=%d., addrType=%d., addr=0x%p)\n", netSock, port, addrType, addr));

    memset(&addrBuf, 0, sizeof(addrBuf));

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        AJ_ErrPrintf(("AJ_Net_Connect(): invalid socket.  status=AJ_ERR_CONNECT\n"));
        return AJ_ERR_CONNECT;
    }
    if (addrType == AJ_ADDR_IPV4) {
        struct sockaddr_in* sa = (struct sockaddr_in*)&addrBuf;
        sa->sin_family = AF_INET;
        sa->sin_port = htons(port);
        sa->sin_addr.s_addr = *addr;
        addrSize = sizeof(*sa);
        AJ_InfoPrintf(("AJ_Net_Connect(): Connect to \"%s:%u\"\n", inet_ntoa(sa->sin_addr), port));;
    } else {
        struct sockaddr_in6* sa = (struct sockaddr_in6*)&addrBuf;
        sa->sin6_family = AF_INET6;
        sa->sin6_port = htons(port);
        memcpy(sa->sin6_addr.s6_addr, addr, sizeof(sa->sin6_addr.s6_addr));
        addrSize = sizeof(*sa);
    }
    ret = connect(sock, (struct sockaddr*)&addrBuf, addrSize);
    if (ret == SOCKET_ERROR) {
        AJ_ErrPrintf(("AJ_Net_Connect(): connect() failed. WSAGetLastError()=0x%x, status=AJ_ERR_CONNECT\n", WSAGetLastError()));
        closesocket(sock);
        return AJ_ERR_CONNECT;
    } else {
        AJ_IOBufInit(&netSock->rx, rxData, sizeof(rxData), AJ_IO_BUF_RX, (void*)sock);
        netSock->rx.recv = AJ_Net_Recv;
        AJ_IOBufInit(&netSock->tx, txData, sizeof(txData), AJ_IO_BUF_TX, (void*)sock);
        netSock->tx.send = AJ_Net_Send;
        AJ_InfoPrintf(("AJ_Net_Connect(): status=AJ_OK\n"));
        return AJ_OK;
    }
}

void AJ_Net_Disconnect(AJ_NetSocket* netSock)
{
    SOCKET sock = (SOCKET)netSock->rx.context;

    AJ_InfoPrintf(("AJ_Net_Disconnect(nexSock=0x%p)\n", netSock));

    if (sock) {
        shutdown(sock, 0);
        closesocket(sock);
        memset(netSock, 0, sizeof(AJ_NetSocket));
    }
}

static SOCKET* McastSocks = NULL;
static size_t NumMcastSocks = 0;

static AJ_Status AJ_Net_SendTo(AJ_IOBuffer* buf)
{
    DWORD ret;
    DWORD tx = AJ_IO_BUF_AVAIL(buf);
    int numWrites = 0;

    AJ_InfoPrintf(("AJ_Net_SendTo(buf=0x%p)\n", buf));

    assert(buf->direction == AJ_IO_BUF_TX);
    assert(NumMcastSocks > 0);

    if (tx > 0) {
        size_t i;
        /*
         * Only multicasting over IPv4 for now
         */
        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = htons(AJ_UDP_PORT);
        sin.sin_addr.s_addr = inet_addr(AJ_IPV4_MULTICAST_GROUP);

        // our daemon (hopefully) lives on one of the networks but we don't know which one.
        // send the WhoHas packet to all of them.
        for (i = 0; i < NumMcastSocks; ++i) {
            SOCKET sock = McastSocks[i];
            ret = sendto(sock, buf->readPtr, tx, 0, (struct sockaddr*) &sin, sizeof(sin));
            if (ret == SOCKET_ERROR) {
                AJ_ErrPrintf(("AJ_Net_SendTo(): sendto() failed. WSAGetLastError()=0x%x, status=AJ_ERR_CONNECT\n", WSAGetLastError()));
            } else {
                ++numWrites;
            }
        }

        if (numWrites == 0) {
            AJ_ErrPrintf(("AJ_Net_SendTo(): Did not sendto() at least one socket.  status=AJ_ERR_WRITE\n"));
            return AJ_ERR_WRITE;
        }
        buf->readPtr += ret;
    }
    AJ_IO_BUF_RESET(buf);
    AJ_InfoPrintf(("AJ_Net_SendTo(): status=AJ_OK\n"));
    return AJ_OK;
}

static AJ_Status AJ_Net_RecvFrom(AJ_IOBuffer* buf, uint32_t len, uint32_t timeout)
{
    AJ_Status status;
    DWORD ret;
    DWORD rx = AJ_IO_BUF_SPACE(buf);
    fd_set fds;
    size_t rc = 0;
    size_t i;
    const struct timeval tv = { timeout / 1000, 1000 * (timeout % 1000) };
    SOCKET sock;

    AJ_InfoPrintf(("AJ_Net_RecvFrom(buf=0x%p, len=%d., timeout=%d.)\n", buf, len, timeout));

    assert(buf->direction == AJ_IO_BUF_RX);
    assert(NumMcastSocks > 0);

    // one per interface
    FD_ZERO(&fds);
    for (i = 0; i < NumMcastSocks; ++i) {
        SOCKET sock = McastSocks[i];
        FD_SET(sock, &fds);
    }

    rc = select(NumMcastSocks, &fds, NULL, NULL, &tv);
    if (rc == 0) {
        AJ_InfoPrintf(("AJ_Net_RecvFrom(): select() timed out. status=AJ_ERR_TIMEOUT\n"));
        return AJ_ERR_TIMEOUT;
    } else if (rc < 0) {
        AJ_ErrPrintf(("AJ_Net_RecvFrom(): select() failed. WSAGetLastError()=0x%x, status=AJ_ERR_READ\n", WSAGetLastError()));
        return AJ_ERR_READ;
    }

    // we sent the WhoHas packet out on ALL multicast WIFI interfaces
    // now we need to listen to all of them for a reply
    // ignore multiple replies; only consider the first one to arrive
    rx = min(rx, len);
    for (i = 0; i < NumMcastSocks; ++i) {
        if (FD_ISSET(McastSocks[i], &fds)) {
            sock = McastSocks[i];
            break;
        }
    }

    if (sock != INVALID_SOCKET) {
        ret = recvfrom(sock, buf->writePtr, rx, 0, NULL, 0);
        if (ret == SOCKET_ERROR) {
            AJ_ErrPrintf(("AJ_Net_RecvFrom(): recvfrom() failed. WSAGetLastError()=0x%x, status=AJ_ERR_READ\n", WSAGetLastError()));
            status = AJ_ERR_READ;
        } else {
            buf->writePtr += ret;
            status = AJ_OK;
        }
    } else {
        AJ_ErrPrintf(("AJ_Net_RecvFrom(): invalid socket.  status=AJ_ERR_READ\n"));
        status = AJ_ERR_READ;
    }

    AJ_InfoPrintf(("AJ_Net_RecvFrom(): status=%s\n", AJ_StatusText(status)));
    return status;
}

/*
 * Need enough space to receive a complete name service packet when used in UDP
 * mode.  NS expects MTU of 1500 subtracts UDP, IP and ethertype overhead.
 * 1500 - 8 -20 - 18 = 1454.  txData buffer size needs to be big enough to hold
 * a NS WHO-HAS for one name (4 + 2 + 256 = 262)
 */
static uint8_t rxDataMCast[1454];
static uint8_t txDataMCast[262];

/* The following function is very ugly.  Because of the way Windows does multicast, we need
 * to iterate through our network adaptors and bind each that supports multicast to a separate
 * socket.  Then on each socket, join the NameService mcast group.
 *
 * Note: this does not yet support IPV6.
 *
 */

AJ_Status AJ_Net_MCastUp(AJ_NetSocket* netSock)
{
    AJ_Status status;
    DWORD ret = 0;
    struct ip_mreq mreq;
    struct sockaddr_in sin;

    IP_ADAPTER_ADDRESSES info, * parray = NULL, * pinfo = NULL;
    ULONG infoLen = sizeof(info);

    WinsockCheck();

    AJ_InfoPrintf(("AJ_Net_MCastUp(nexSock=0x%p)\n", netSock));

    // find out how many Adapter addresses we have and get the list
    GetAdaptersAddresses(
        AF_INET,
        GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER,
        0, &info, &infoLen);
    parray = pinfo = (IP_ADAPTER_ADDRESSES*) malloc(infoLen);

    GetAdaptersAddresses(
        AF_INET,
        GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER,
        0, pinfo, &infoLen);

    // iterate through the adaptor addresses.
    for (; pinfo; pinfo = pinfo->Next) {
        IP_ADAPTER_UNICAST_ADDRESS* paddr;

#if USE_WIFI_ONLY
        // we only care about WIFI adaptors that support multicast
        if (pinfo->IfType != IF_TYPE_IEEE80211) {
            continue;
        }
#endif

        if (pinfo->Flags & IP_ADAPTER_NO_MULTICAST) {
            continue;
        }

        // iterate through the IP addressses
        for (paddr = pinfo->FirstUnicastAddress; paddr; paddr = paddr->Next) {
            SOCKET sock = INVALID_SOCKET;
            char buffer[NI_MAXHOST];

            // get the IP address
            memset(buffer, 0, NI_MAXHOST);
            getnameinfo(paddr->Address.lpSockaddr, paddr->Address.iSockaddrLength,
                        buffer, sizeof(buffer), NULL, 0, NI_NUMERICHOST);

            // create a socket
            sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock == INVALID_SOCKET) {
                AJ_ErrPrintf(("AJ_Net_MCastUp(): socket() failed. WSAGetLastError()=0x%x\n", WSAGetLastError()));
                continue;
            }

            // bind the socket to the address with ephemeral port
            memset((char*) &sin, 0, sizeof(SOCKADDR_IN));
            sin.sin_family = AF_INET;
            sin.sin_port = htons(0);
            sin.sin_addr.s_addr = inet_addr(buffer);

            ret = bind(sock, (struct sockaddr*) &sin, sizeof(sin));
            if (ret == SOCKET_ERROR) {
                AJ_ErrPrintf(("AJ_Net_MCastUp(): bind() failed. WSAGetLastError()=0x%x\n", WSAGetLastError()));
                closesocket(sock);
                continue;
            }

            // because routers are advertised silently, the reply will be unicast
            // however, Windows forces us to join the multicast group before we can broadcast our WhoHas packets
            memset((char*) &mreq, 0, sizeof(struct ip_mreq));
            mreq.imr_multiaddr.s_addr = inet_addr(AJ_IPV4_MULTICAST_GROUP);
            mreq.imr_interface.s_addr = inet_addr(buffer);
            ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq));
            if (ret == SOCKET_ERROR) {
                AJ_ErrPrintf(("AJ_Net_MCastUp(): setsockopt(IP_ADD_MEMBERSHIP) failed. WSAGetLastError()=0x%x\n", WSAGetLastError()));
                continue;
            }

            // add the socket to the socket vector
            ++NumMcastSocks;
            McastSocks = realloc(McastSocks, sizeof(SOCKET) * NumMcastSocks);
            McastSocks[NumMcastSocks - 1] = sock;
        }
    }

    // if we don't have at least one good socket for multicast, error
    if (NumMcastSocks == 0) {
        AJ_ErrPrintf(("AJ_Net_MCastUp(): No sockets found.  status=AJ_ERR_READ\n"));
        status = AJ_ERR_READ;
        goto ErrorExit;
    }



    AJ_IOBufInit(&netSock->rx, rxDataMCast, sizeof(rxDataMCast), AJ_IO_BUF_RX, (void*) McastSocks);
    netSock->rx.recv = AJ_Net_RecvFrom;
    AJ_IOBufInit(&netSock->tx, txDataMCast, sizeof(txDataMCast), AJ_IO_BUF_TX, (void*) McastSocks);
    netSock->tx.send = AJ_Net_SendTo;
    AJ_ErrPrintf(("AJ_Net_MCastUp(): status=AJ_OK\n"));
    return AJ_OK;

ErrorExit:
    free(parray);
    if (status != AJ_OK && McastSocks) {
        size_t i;
        for (i = 0; i < NumMcastSocks; ++i) {
            closesocket(McastSocks[i]);
        }

        NumMcastSocks = 0;
        free(McastSocks);
        McastSocks = NULL;
    }

    AJ_InfoPrintf(("AJ_Net_MCastUp(): status=%s\n", AJ_StatusText(status)));
    return status;
}

void AJ_Net_MCastDown(AJ_NetSocket* netSock)
{
    size_t i;
    struct ip_mreq mreq;

    AJ_InfoPrintf(("AJ_Net_MCastDown(nexSock=0x%p)\n", netSock));

    /*
     * Leave our multicast group
     */
    for (i = 0; i < NumMcastSocks; ++i) {
        SOCKET sock = McastSocks[i];
        mreq.imr_multiaddr.s_addr = inet_addr(AJ_IPV4_MULTICAST_GROUP);
        mreq.imr_interface.s_addr = INADDR_ANY;
        setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*) &mreq, sizeof(mreq));
        shutdown(sock, 0);
        closesocket(sock);
    }


    NumMcastSocks = 0;
    free(McastSocks);
    McastSocks = NULL;
    memset(netSock, 0, sizeof(AJ_NetSocket));
}
