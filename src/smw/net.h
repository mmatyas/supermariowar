#ifndef __NETWORK_H_
#define __NETWORK_H_

#include "SDL.h"
#include "SDL_net.h"

#define NET_PROTOCOL_VERSION                1
#define NET_MAX_MESSAGE_SIZE                128
#define NET_DEFAULT_PORT                    12521
#define NET_MAX_PLAYER_NAME_LENGTH          16
#define NET_MAX_ROOM_NAME_LENGTH            32

#define NET_REQUEST_SERVERINFO              1
#define NET_RESPONSE_SERVERINFO             2
#define NET_RESPONSE_BADPROTOCOL            3

#define NET_REQUEST_CONNECT                 10
#define NET_RESPONSE_CONNECT_OK             11
#define NET_RESPONSE_CONNECT_DENIED         12
#define NET_RESPONSE_CONNECT_SERVERFULL     13
#define NET_RESPONSE_CONNECT_NAMETAKEN      14

#define NET_REQUEST_ROOM_LIST               20
#define NET_RESPONSE_NO_ROOMS               21
#define NET_RESPONSE_ROOM_LIST_ENTRY        22

#define NET_RESPONSE_ROOMFULL


//Write network handler class here
//Do similar to how gfx/sfx works with init and clean up functions
//Have all send and receive logic here
//Make sure to have message handler functions in main and menu loops

bool net_init();
void net_close();
void net_saveServerList();
void net_loadServerList();

struct LastMessage {
    uint8_t        packageType;
    uint32_t       timestamp;
};

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
    char           playerName[NET_MAX_PLAYER_NAME_LENGTH];
};

class NetClient
{
	public:

		NetClient();
		~NetClient();

		void update();

        bool sendConnectRequestToSelectedServer();

        // called on network session start/end
		bool startSession();
		void endSession();

        // called on startup/shutdown
        bool init();
        void cleanup();

        //void refreshRoomList();
        void requestRoomList();

	private:

		IPaddress serverIP;

		TCPsocket tcpSocket;
        uint8_t tcpResponseBuffer[NET_MAX_MESSAGE_SIZE];

		UDPsocket udpSocket;
        UDPpacket* udpOutgoingPacket;
        UDPpacket* udpIncomingPacket;

		SDLNet_SocketSet sockets;
		int readySockets;


        bool connect(const char* hostname, const uint16_t port = NET_DEFAULT_PORT);

		void closeTCPsocket();
		void closeUDPsocket();

        bool sendTCPMessage(const void* data, const int dataLength);
        bool receiveTCPMessage();

        bool sendUDPMessage(const void* data, const int dataLength);
        bool receiveUDPMessage();
};

#endif //__NETWORK_H_

