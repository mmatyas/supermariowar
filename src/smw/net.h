#ifndef NETWORK_H
#define NETWORK_H

#include "input.h"

#include <cassert>

#include "network/NetworkProtocolCodes.h"


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

        void sendSyncOKMessage();
        void sendGoodbye();
};


struct Networking {
    // Network status
    bool active;            // True if netplay code is currently running
    //bool networkErrorHappened;
    bool connectSuccessful;
    bool joinSuccessful;
    bool gameRunning;
    bool currentMenuChanged; // eg. room players changed
    bool operationInProgress; // the waiting dialogs watch this variable

    //
    NetClient client;
    LastMessage lastSentMessage;
    LastMessage lastReceivedMessage;
    char myPlayerName[NET_MAX_PLAYER_NAME_LENGTH];    // last byte will be \0

    // Server list
    unsigned short selectedServerIndex;
    std::vector<ServerAddress> savedServers;

    // Room list
    char roomFilter[NET_MAX_ROOM_NAME_LENGTH];
    unsigned short selectedRoomIndex;
    std::vector<RoomListEntry> currentRooms;
    Room currentRoom;

    // Current room
    char newroom_name[NET_MAX_ROOM_NAME_LENGTH];
    char newroom_password[NET_MAX_ROOM_PASSWORD_LENGTH];
    char mychatmessage[NET_MAX_CHAT_MSG_LENGTH];

    // In-game
    bool theHostIsMe;
    CPlayerInput netPlayerInput;
};

#endif // NETWORK_H
