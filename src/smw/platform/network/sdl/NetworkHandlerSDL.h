#ifndef __NETWORK_HANDLER_SDL_H_
#define __NETWORK_HANDLER_SDL_H_

#include "../NetworkHandlerBase.h"

#include <SDL/SDL_net.h>

class NetworkHandlerSDL : public NetworkHandlerBase
{
    public:
        bool init_networking();
        bool init_client();
        bool init_server();
        void cleanup();

        bool openUDPConnection(const char* hostname, const uint16_t port);
        void closeUDPConnection();

        bool sendUDPMessage(const void* data, const unsigned dataLength);
        bool receiveUDPMessage(const void* dataBuffer);

    private:
        IPaddress serverIP;

        UDPsocket udpSocket;
        UDPpacket* udpOutgoingPacket;
        UDPpacket* udpIncomingPacket;
};

bool NetworkHandlerSDL::init_networking()
{
    if (SDLNet_Init() < 0) {
        fprintf(stderr, "[Error] SDLNet_Init: %s\n", SDLNet_GetError());
        return false;
    }

    atexit(SDLNet_Quit);
    return true;
}

void NetworkHandlerSDL::cleanup()
{
    SDLNet_Quit();
}

bool NetworkHandlerSDL::init_client()
{
    udpOutgoingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    udpIncomingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    if (!udpOutgoingPacket || !udpIncomingPacket) {
        fprintf(stderr, "[Error] SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetworkHandlerSDL::init_server()
{
    fprintf(stderr, "NOT IMPLEMENTED\n");
}

bool NetworkHandlerSDL::openUDPConnection(const char* hostname, const uint16_t port)
{
    /* Resolve server address */
    printf("Resolving %s:%d\n", hostname, port);
    if (SDLNet_ResolveHost(&serverIP, hostname, port) < 0) {
        if (serverIP.host == INADDR_NONE)
            fprintf(stderr, "[Error] Couldn't resolve hostname\n");
        else
            fprintf(stderr, "[Error] SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return false;
    }

    /* Open UDP socket */
    if (!udpSocket) {
        udpSocket = SDLNet_UDP_Open(0);
        if (!udpSocket) {
            fprintf(stderr, "[Error] SDLNet_UDP_Open: %s\n", SDLNet_GetError());
            return false;
        }
        printf("[] UDP open.\n");
    }

    return true;
}

bool NetworkHandlerSDL::sendUDPMessage(const void* data, const unsigned dataLength)
{
    if (!data || dataLength < 2 || dataLength >= NET_MAX_MESSAGE_SIZE || !udpSocket || !udpOutgoingPacket) {
        printf("[Debug] Invalid call: sendUDPMessage\n");
        return false;
    }

    memcpy(udpOutgoingPacket->data, data, dataLength);
    udpOutgoingPacket->len = dataLength;
    udpOutgoingPacket->address.host = serverIP.host;
    udpOutgoingPacket->address.port = serverIP.port;

    if (!SDLNet_UDP_Send(udpSocket, -1, udpOutgoingPacket)) {
        fprintf(stderr, "[Warning] Sending UDP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetworkHandlerSDL::receiveUDPMessage(const void* dataBuffer)
{
    if (!udpSocket || !udpIncomingPacket || !dataBuffer)
        return false;

    int receivedPackages = SDLNet_UDP_Recv(udpSocket, udpIncomingPacket);
    if (receivedPackages == 0)
        return false;

    if (receivedPackages < 0) {
        fprintf(stderr, "[Warning] Receiving UDP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    // Must have version and type field.
    if (udpIncomingPacket->len < 2)
        return false;

    memcpy(dataBuffer, udpIncomingPacket->data, NET_MAX_MESSAGE_SIZE);
}

#endif // __NETWORK_HANDLER_SDL_H_