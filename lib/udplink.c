/**
 * Copyright (c) 2017 cs8425
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license.
 */

#include <string.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/socket.h>

#include "udplink.h"

int udpInit(udpLink_t* link, const char* addr, int port, bool isServer) {
    int one = 1;

    if ((link->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        return -2;
    }

    setsockopt(link->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)); // can multi-bind
    fcntl(link->fd, F_SETFL, fcntl(link->fd, F_GETFL, 0) | O_NONBLOCK); // nonblock

    link->isServer = isServer;
    memset(&link->si, 0, sizeof(link->si));
    link->si.sin_family = AF_INET;
    link->si.sin_port = htons(port);
    link->port = port;

    if (addr == NULL) {
        link->si.sin_addr.s_addr = htonl(INADDR_ANY);
    }else{
        link->si.sin_addr.s_addr = inet_addr(addr);
    }

    if (isServer) {
        if (bind(link->fd, (const struct sockaddr *)&link->si, sizeof(link->si)) == -1) {
            return -1;
        }
    }
    return 0;
}

int udpSend(udpLink_t* link, const void* data, size_t size) {
    return sendto(link->fd, data, size, 0, (struct sockaddr *)&link->si, sizeof(link->si));
}

int udpRecv(udpLink_t* link, void* data, size_t size, uint32_t timeout_ms) {
    fd_set fds;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(link->fd, &fds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000UL;

    if (select(link->fd+1, &fds, NULL, NULL, &tv) != 1) {
        return -1;
    }

    socklen_t len;
    int ret;
    //ret = recvfrom(link->fd, data, size, 0, (struct sockaddr *)&link->recv, &len);
    ret = recv(link->fd, data, size, 0);
    //printf("rec: %d\n", ret);
    return ret;
}

void* udpRecvWorker(void *ptr)
{
    udpLink_t* link = (udpLink_t*) ptr;
    while(true) 
    {
        int ret = udpRecv(link, link->buffer, link->buffer_size, link->timeout_ms);
        if(ret > 0) 
        {
            link->onData(link->buffer, (uint32_t)ret);
        }
    }
}


void udpInitRecvThread(udpLink_t* link, const char* addr, int port, OnUdpData callback, uint32_t timeout_ms, char* buffer, uint32_t buffer_size)
{
    link->timeout_ms = timeout_ms;
    link->onData = callback;
    link->buffer = buffer;
    link->buffer_size = buffer_size;
    udpInit(link, addr, port, true);
    pthread_create( &link->thread, NULL, udpRecvWorker, (void*) link);
}