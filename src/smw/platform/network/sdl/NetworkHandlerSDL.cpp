#include "NetworkHandlerSDL.h"

bool NetworkHandlerSDL::init_networking()
{
    SDL_version ver_compiled;
    const SDL_version * ver_net_current = SDLNet_Linked_Version(); // needs const TODO: fix
    SDL_NET_VERSION(&ver_compiled);
    printf("[net] Initializing SDL net %d.%d.%d (compiled with %d.%d.%d) ... ",
        ver_net_current->major, ver_net_current->minor, ver_net_current->patch,
        ver_compiled.major, ver_compiled.minor, ver_compiled.patch);

    if (SDLNet_Init() < 0) {
        fprintf(stderr, "[error][net] Couldn't initialize SDL_Net. %s\n", SDLNet_GetError());
        printf("error: %s\n", SDLNet_GetError());
        return false;
    } else
        printf("ok\n");

    atexit(SDLNet_Quit);

    udpOutgoingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    udpIncomingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    if (!udpOutgoingPacket || !udpIncomingPacket) {
        fprintf(stderr, "[error][net] Couldn't create UDP packets. %s\n", SDLNet_GetError());
        return false;
    }

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
    // Nothing to do here
    return true;
}

bool NetworkHandlerSDL::init_server()
{
    fprintf(stderr, "[net][debug] NOT IMPLEMENTED\n");
    return true;
}

//
//
// UDP
//
//

bool NetworkHandlerSDL::openUDPConnection(const char* hostname, const uint16_t port)
{
    /* Resolve server address */
    printf("Resolving %s:%d\n", hostname, port);
    if (SDLNet_ResolveHost(&serverIP, hostname, port) < 0) {
        if (serverIP.host == INADDR_NONE)
            fprintf(stderr, "[error][net] Couldn't resolve hostname.\n");
        else
            fprintf(stderr, "[error][net] Error when resolving host name. %s\n", SDLNet_GetError());
        return false;
    }

    /* Open UDP socket */
    if (!udpSocket) {
        udpSocket = SDLNet_UDP_Open(0);
        if (!udpSocket) {
            fprintf(stderr, "[error][net] Couldn't open UDP socket. %s\n", SDLNet_GetError());
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
    if (udpIncomingPacket->len < 3) {
        fprintf(stderr, "[net][warning] Invalid incoming package.\n");
        return false;
    }

    memcpy(dataBuffer, udpIncomingPacket->data, NET_MAX_MESSAGE_SIZE);
    return true;
}

//
//
// TCP
//
//

bool NetworkHandlerSDL::openTCPConnection(const char* hostname, const uint16_t port)
{
    /* Resolve server address */
    printf("Resolving %s:%d\n", hostname, port);
    if (SDLNet_ResolveHost(&serverIP, hostname, port) < 0) {
        if (serverIP.host == INADDR_NONE)
            fprintf(stderr, "[error][net] Couldn't resolve hostname.\n");
        else
            fprintf(stderr, "[error][net] Error when resolving host name. %s\n", SDLNet_GetError());
        return false;
    }

    /* Open UDP socket */
    if (!tcpSocket) {
        tcpSocket = SDLNet_TCP_Open(&serverIP);
        if (!tcpSocket) {
            fprintf(stderr, "[error][net] Couldn't open TCP socket. %s\n", SDLNet_GetError());
            return false;
        }
        printf("[net] TCP open.\n");
    }

    return true;
}

void NetworkHandlerSDL::closeTCPConnection()
{
    if (tcpSocket) {
        SDLNet_TCP_Close(tcpSocket);
        tcpSocket = NULL;
        printf("[net] TCP closed.\n");
    }
}

bool NetworkHandlerSDL::sendTCPMessage(const void* data, const unsigned dataLength)
{
    if (!data || dataLength < 2 || dataLength >= NET_MAX_MESSAGE_SIZE || !tcpSocket) {
        printf("[net][debug] Invalid call: sendTCPMessage.\n");
        return false;
    }

    if (SDLNet_TCP_Send(tcpSocket, data, dataLength) < dataLength) {
        fprintf(stderr, "[net][warning] Sending TCP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetworkHandlerSDL::receiveTCPMessage(void* dataBuffer)
{
    if (!tcpSocket || !dataBuffer)
        return false;

    int receivedBytes = SDLNet_TCP_Recv(tcpSocket, dataBuffer, NET_MAX_MESSAGE_SIZE);
    if (receivedBytes <= 0) {
        fprintf(stderr, "[net][warning] Receiving TCP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    // Must have version and type field.
    if (receivedBytes < 2) {
        fprintf(stderr, "[net][warning] Invalid incoming package.\n");
        return false;
    }

    return true;
}
