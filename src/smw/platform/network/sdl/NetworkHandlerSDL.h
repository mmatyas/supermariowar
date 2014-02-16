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
        bool receiveUDPMessage(void* dataBuffer);

    private:
        IPaddress serverIP;

        UDPsocket udpSocket;
        UDPpacket* udpOutgoingPacket;
        UDPpacket* udpIncomingPacket;
};

#endif // __NETWORK_HANDLER_SDL_H_
