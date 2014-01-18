#ifndef __NETWORK_H_
#define __NETWORK_H_

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#include "input.h"

#define NET_PROTOCOL_VERSION                1
#define NET_MAX_MESSAGE_SIZE                128
#define NET_SERVER_PORT                     12521
#define NET_MAX_PLAYER_NAME_LENGTH          16
#define NET_MAX_ROOM_NAME_LENGTH            32
#define NET_MAX_ROOM_PASSWORD_LENGTH        16
#define NET_MAX_CHAT_MSG_LENGTH             100

#define NET_REQUEST_SERVERINFO              1
#define NET_RESPONSE_BADPROTOCOL            2
#define NET_RESPONSE_SERVERINFO             3
#define NET_RESPONSE_SERVER_MOTD            4 // Message of the Day
#define NET_NOTICE_SERVER_STOPPED           5

#define NET_REQUEST_CONNECT                 10
#define NET_REQUEST_LEAVE_SERVER            11
#define NET_RESPONSE_CONNECT_OK             12
#define NET_RESPONSE_CONNECT_DENIED         13
#define NET_RESPONSE_CONNECT_SERVERFULL     14
#define NET_RESPONSE_CONNECT_NAMETAKEN      15

#define NET_REQUEST_ROOM_LIST               20
#define NET_RESPONSE_NO_ROOMS               21
#define NET_RESPONSE_ROOM_LIST_ENTRY        22

#define NET_REQUEST_JOIN_ROOM               30
#define NET_REQUEST_LEAVE_ROOM              31
#define NET_RESPONSE_JOIN_OK                32
#define NET_RESPONSE_ROOM_FULL              33
#define NET_NOTICE_ROOM_CHANGED             34 // + host quit?

#define NET_REQUEST_CREATE_ROOM             40
#define NET_RESPONSE_CREATE_OK              41
#define NET_RESPONSE_CREATE_ERROR           42 // TODO: What kind of error?

#define NET_REQUEST_START_GAME              50
#define NET_NOTICE_GAME_SYNCH               51 // check if everyone is ready
#define NET_NOTICE_GAME_SYNCH_OK            52
#define NET_NOTICE_GAME_STARTED             53

#define NET_NOTICE_LOCAL_KEYS               60
#define NET_NOTICE_REMOTE_KEYS              61


//Write network handler class here
//Do similar to how gfx/sfx works with init and clean up functions
//Have all send and receive logic here
//Make sure to have message handler functions in main and menu loops

bool net_init();
void net_close();
void net_saveServerList();
void net_loadServerList();

class MI_NetworkListScroll;
//union COutputControl;


// Local structures

struct LastMessage {
    uint8_t        packageType;
    uint32_t       timestamp;
};

struct ServerAddress {
    std::string    hostname;
};

struct RoomListEntry {
    uint32_t       roomID;
    std::string    name;
    uint8_t        playerCount;
};

struct Room {
    uint32_t       roomID;
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    char           playerNames[4][NET_MAX_PLAYER_NAME_LENGTH];
    uint8_t        hostPlayerNumber; // 1-4

    Room() {
        name[0] = '\0';
        for (uint8_t p = 0; p < 4; p++)
            playerNames[p][0] = '\0';
    }
};


// Protocol structures

struct MessageHeader {      // TODO: add timestamp
    uint8_t        protocolVersion;
    uint8_t        packageType;
};

struct ServerInfoPackage : MessageHeader {
    char           name[32];
    uint32_t       currentPlayerCount;
    uint32_t       maxPlayerCount;
};

struct ClientConnectionPackage : MessageHeader {
    char           playerName[NET_MAX_PLAYER_NAME_LENGTH];
};

struct RoomInfoPackage : MessageHeader {
    uint32_t       roomID;
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    uint8_t        currentPlayerCount;
    bool           passwordRequired;
};

struct NewRoomPackage : MessageHeader {
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    char           password[NET_MAX_ROOM_PASSWORD_LENGTH];
};

struct NewRoomCreatedPackage : MessageHeader {
    uint32_t       roomID;
};

struct JoinRoomPackage : MessageHeader {
    uint32_t       roomID;
    char           password[NET_MAX_ROOM_PASSWORD_LENGTH];
};

struct CurrentRoomPackage : MessageHeader {
    uint32_t       roomID;
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    char           playerName[4][NET_MAX_PLAYER_NAME_LENGTH];
    uint8_t        hostPlayerNumber; //  1-4
    uint8_t        remotePlayerNumber; // of the receiving client
};

struct StartSynchPackage : MessageHeader {
    uint32_t       commonRandomSeed;
};

struct LocalKeysPackage : MessageHeader {
    CKeyState      keys[8];
};

struct RemoteKeysPackage : MessageHeader {
    uint8_t        playerNumber;
    CKeyState      keys[8];
};

// Network communication class

class NetClient
{
	public:

		NetClient();
		~NetClient();

		void listen();

        bool sendConnectRequestToSelectedServer();
        void sendCreateRoomMessage();
        void sendJoinRoomMessage();
        void sendLeaveRoomMessage();
        void sendStartRoomMessage();

        void sendLocalInput();

        // called on network session start/end
		bool startSession();
		void endSession();

        // called on startup/shutdown
        bool init();
        void cleanup();

        void setRoomListUIControl(MI_NetworkListScroll*);

        //void refreshRoomList();
        void requestRoomList();

	private:

		IPaddress serverIP;

		UDPsocket udpSocket;
        UDPpacket* udpOutgoingPacket;
        UDPpacket* udpIncomingPacket;

        MI_NetworkListScroll* uiRoomList;


        uint8_t remotePlayerNumber;
        short backup_playercontrol[4];


        bool openSocket(const char* hostname, const uint16_t port = NET_SERVER_PORT);
		void closeUDPsocket();
        bool sendUDPMessage(const void* data, const int dataLength);
        bool receiveUDPMessage();

        void handleServerinfoAndClose();
        void handleNewRoomListEntry();
        void handleRoomCreatedMessage();
        void handleRoomChangedMessage();
        void handleRemoteInput();

        void sendSynchOKMessage();
        void sendGoodbye();
};

#endif //__NETWORK_H_

