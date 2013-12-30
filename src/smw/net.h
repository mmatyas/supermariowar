#ifndef __NETWORK_H_
#define __NETWORK_H_

#include "SDL.h"
#include "SDL_net.h"

#define NET_PROTOCOL_VERSION                1
#define NET_MAX_MESSAGE_SIZE                128
#define NET_DEFAULT_PORT                    12521

#define NET_REQUEST_SERVERINFO              1
#define NET_RESPONSE_SERVERINFO             2

#define NET_REQUEST_CONNECT                 10
#define NET_RESPONSE_CONNECT_OK             11
#define NET_RESPONSE_CONNECT_DENIED         12
#define NET_RESPONSE_CONNECT_NAMETAKEN      13

#define NET_RESPONSE_ROOMFULL


//Write network handler class here
//Do similar to how gfx/sfx works with init and clean up functions
//Have all send and receive logic here
//Make sure to have message handler functions in main and menu loops

bool net_init();
void net_close();
void net_saveServerList();
void net_loadServerList();

struct MessageHeader {
    uint8_t        protocolVersion;
    uint8_t        packageType;
};

struct ServerAddress {
    std::string    hostname;
};

struct ServerInfoPackage : MessageHeader {
    char           name[32];
    uint32_t       currentPlayers;
    uint32_t       maxPlayers;
};

struct ClientConnectionPackage: MessageHeader {
    char           playerName[32];
};

class NetClient
{
	public:

		NetClient();
		~NetClient();

		bool connect(const char* hostname, const uint16_t port = NET_DEFAULT_PORT);
		void update();

		bool startSession();
		void endSession();
		
	private:

		IPaddress serverIP;

		TCPsocket tcpSocket;
        uint8_t tcpResponseBuffer[NET_MAX_MESSAGE_SIZE];

		UDPsocket udpSocket;
        int udpChannel;
        UDPpacket* udpOutgoingPacket;
        UDPpacket* udpIncomingPacket;

		SDLNet_SocketSet sockets;
		int readySockets;

		void cleanup();
		void closeTCPsocket();
		void closeUDPsocket();

        bool sendTCPMessage(void* data, int dataLength);
        bool receiveTCPMessage();

        bool sendUDPMessage(void* data, int dataLength);
        bool receiveUDPMessage();
};

#endif //__NETWORK_H_

