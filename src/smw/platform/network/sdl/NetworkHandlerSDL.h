#ifndef NETWORK_HANDLER_SDL_H
#define NETWORK_HANDLER_SDL_H

#include "../NetworkHandlerBase.h"

#include "SDL_net.h"

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
        bool receiveUDPMessage(void* dataBuffer);

        bool openTCPConnection(const char* hostname, const uint16_t port);
        void closeTCPConnection();

        bool sendTCPMessage(const void* data, const unsigned dataLength);
        bool receiveTCPMessage(void* dataBuffer);

    private:
        IPaddress serverIP;

        TCPsocket tcpSocket;

        UDPsocket udpSocket;
        UDPpacket* udpOutgoingPacket;
        UDPpacket* udpIncomingPacket;
};

#endif // NETWORK_HANDLER_SDL_H
