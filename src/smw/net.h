#ifndef __NETWORK_H_
#define __NETWORK_H_

#include "input.h"

#include <SDL/SDL.h>

// define platform guards here
#define NetworkHandler NetworkHandlerSDL
#include "platform/network/sdl/NetworkHandlerSDL.h"



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

struct MessageHeader {
    uint8_t        protocolVersion;
    uint8_t        packageType;
    //uint32_t       packageNumber;
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

// 2 byte bit field instead of 8*2, but you can't use arrays :(
struct RawInput {
    bool           key0_down : 1;
    bool           key0_pressed : 1;
    bool           key1_down : 1;
    bool           key1_pressed : 1;
    bool           key2_down : 1;
    bool           key2_pressed : 1;
    bool           key3_down : 1;
    bool           key3_pressed : 1;
    bool           key4_down : 1;
    bool           key4_pressed : 1;
    bool           key5_down : 1;
    bool           key5_pressed : 1;
    bool           key6_down : 1;
    bool           key6_pressed : 1;
    bool           key7_down : 1;
    bool           key7_pressed : 1;
};

struct InputPackage : MessageHeader {
    RawInput       input;
};

struct RemoteInputPackage : MessageHeader {
    uint8_t        playerNumber;
    RawInput       input;
};

struct GameStatePackage : MessageHeader {
    float          player_x[4];
    float          player_y[4];
    float          player_xvel[4];
    float          player_yvel[4];
    RawInput       input[4];
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
        void sendCurrentGameState();

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

        NetworkHandler networkHandler;
        MI_NetworkListScroll* uiRoomList;

        uint8_t incomingData[NET_MAX_MESSAGE_SIZE];

        uint8_t remotePlayerNumber;
        short backup_playercontrol[4];


        bool openConnection(const char* hostname, const uint16_t port = NET_SERVER_PORT);
		void closeConnection();
        bool sendMessage(const void* data, const int dataLength);
        bool receiveMessage();

        void handleServerinfoAndClose();
        void handleNewRoomListEntry();
        void handleRoomCreatedMessage();
        void handleRoomChangedMessage();
        void handleRemoteInput();
        void handleRemoteGameState();

        void sendSynchOKMessage();
        void sendGoodbye();
};

#endif //__NETWORK_H_

