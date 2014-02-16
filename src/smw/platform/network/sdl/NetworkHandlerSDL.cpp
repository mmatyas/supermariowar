#include "NetworkHandlerSDL.h"

bool NetworkHandlerSDL::init_networking()
{
    if (SDLNet_Init() < 0) {
        fprintf(stderr, "[net][error] Couldn't initialize SDL_Net. %s\n", SDLNet_GetError());
        return false;
    }

    atexit(SDLNet_Quit);
    return true;
}

void NetworkHandlerSDL::cleanup()
{
    if (udpIncomingPacket) {
        SDLNet_FreePacket(udpIncomingPacket);
        udpIncomingPacket = NULL;
    }

    if (udpOutgoingPacket) {
        SDLNet_FreePacket(udpOutgoingPacket);
        udpOutgoingPacket = NULL;
    }

    SDLNet_Quit();
}

bool NetworkHandlerSDL::init_client()
{
    udpOutgoingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    udpIncomingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    if (!udpOutgoingPacket || !udpIncomingPacket) {
        fprintf(stderr, "[net][error] Couldn't create UDP packets. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetworkHandlerSDL::init_server()
{
    fprintf(stderr, "[net][debug] NOT IMPLEMENTED\n");
}

bool NetworkHandlerSDL::openUDPConnection(const char* hostname, const uint16_t port)
{
    /* Resolve server address */
    printf("Resolving %s:%d\n", hostname, port);
    if (SDLNet_ResolveHost(&serverIP, hostname, port) < 0) {
        if (serverIP.host == INADDR_NONE)
            fprintf(stderr, "[net][error] Couldn't resolve hostname.\n");
        else
            fprintf(stderr, "[net][error] Error when resolving host name. %s\n", SDLNet_GetError());
        return false;
    }

    /* Open UDP socket */
    if (!udpSocket) {
        udpSocket = SDLNet_UDP_Open(0);
        if (!udpSocket) {
            fprintf(stderr, "[net][error] Couldn't open UDP socket. %s\n", SDLNet_GetError());
            return false;
        }
        printf("[net] UDP open.\n");
    }

    return true;
}

void NetworkHandlerSDL::closeUDPConnection()
{
    if (udpSocket) {
        SDLNet_UDP_Close(udpSocket);
        udpSocket = NULL;
        printf("[net] UDP closed.\n");
    }
}

bool NetworkHandlerSDL::sendUDPMessage(const void* data, const unsigned dataLength)
{
    if (!data || dataLength < 2 || dataLength >= NET_MAX_MESSAGE_SIZE
        || !udpSocket || !udpOutgoingPacket) {
        printf("[net][debug] Invalid call: sendUDPMessage.\n");
        return false;
    }

    memcpy(udpOutgoingPacket->data, data, dataLength);
    udpOutgoingPacket->len = dataLength;
    udpOutgoingPacket->address.host = serverIP.host;
    udpOutgoingPacket->address.port = serverIP.port;

    if (!SDLNet_UDP_Send(udpSocket, -1, udpOutgoingPacket)) {
        fprintf(stderr, "[net][warning] Sending UDP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetworkHandlerSDL::receiveUDPMessage(void* dataBuffer)
{
    if (!udpSocket || !udpIncomingPacket || !dataBuffer)
        return false;

    int receivedPackages = SDLNet_UDP_Recv(udpSocket, udpIncomingPacket);
    if (receivedPackages == 0)
        return false;

    if (receivedPackages < 0) {
        fprintf(stderr, "[net][warning] Receiving UDP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    // Must have version and type field.
    if (udpIncomingPacket->len < 2) {
        fprintf(stderr, "[net][warning] Invalid incoming package.\n");
        return false;
    }

    memcpy(dataBuffer, udpIncomingPacket->data, NET_MAX_MESSAGE_SIZE);
}
