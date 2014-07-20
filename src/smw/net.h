#ifndef NETWORK_H
#define NETWORK_H

#include "input.h"
#include "network/NetworkInterface.h"
#include "network/ProtocolDefinitions.h"

#include <string>
#include <vector>


//Write network handler class here
//Do similar to how gfx/sfx works with init and clean up functions
//Have all send and receive logic here
//Make sure to have message handler functions in main and menu loops

bool net_init();
void net_close();
void net_saveServerList();
void net_loadServerList();

// called on network session start/end
bool net_startSession();
void net_endSession();

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


// Network communication classes

class NetGameHost : public NetworkEventHandler
{
    public:
        NetGameHost();
        ~NetGameHost();

        bool init();
        bool start(NetPeer*&);
        void update();
        void stop();
        void cleanup();

        bool isActive() { return active; }

        // Listen to network events
        void onConnect(NetPeer*);
        void onReceive(NetPeer&, const uint8_t*, size_t);
        void onDisconnect(NetPeer& client);

        // P2. Join room
        void sendStartRoomMessage();


        LastMessage lastSentMessage;
        LastMessage lastReceivedMessage;

    private:
        bool active;

        // connected players except local client
        // TODO: support more players?
        NetPeer* clients[3];
        NetPeer* foreign_lobbyserver;

        // P3. Play
        void handleRemoteInput(const uint8_t* data, size_t dataLength);
        void sendCurrentGameState();

        bool sendMessageToMyPeers(const void* data, int dataLength);

        void setAsLastSentMessage(uint8_t packageType);
        void setAsLastReceivedMessage(uint8_t packageType);
};

class NetClient : public NetworkEventHandler
{
    public:
        NetClient();
        ~NetClient();

        bool init();
        bool start();
        void update();
        void stop();
        void cleanup();

        void setRoomListUIControl(MI_NetworkListScroll*);

        // Listen to network events
        void onConnect(NetPeer*);
        void onReceive(NetPeer&, const uint8_t*, size_t);
        void onDisconnect(NetPeer& client);

        // P1. Connect
        bool sendConnectRequestToSelectedServer();

        // P2. Join room
        void requestRoomList();
        void sendCreateRoomMessage();
        void sendJoinRoomMessage();
        void sendLeaveRoomMessage();

        // P3. Play
        void sendLocalInput();


        LastMessage lastSentMessage;
        LastMessage lastReceivedMessage;

        NetGameHost local_gamehost;

    private:
        NetPeer* foreign_lobbyserver;
        NetPeer* foreign_gamehost;

        MI_NetworkListScroll* uiRoomList;

        //uint8_t incomingData[NET_MAX_MESSAGE_SIZE];

        uint8_t remotePlayerNumber;


        bool openConnection(const char* hostname, const uint16_t port = NET_SERVER_PORT);
        void closeConnection();

        bool sendMessageToLobbyServer(const void* data, int dataLength);
        bool sendMessageToGameHost(const void* data, int dataLength);
        bool sendTo(NetPeer*& peer, const void* data, int dataLength);

        //void setAsLastSentMessage(const void* data, int dataLength);
        //void setAsLastReceivedMessage();

        void handleServerinfoAndClose(const uint8_t*, size_t);
        void handleNewRoomListEntry(const uint8_t*, size_t);
        void handleRoomCreatedMessage(const uint8_t*, size_t);
        void handleRoomChangedMessage(const uint8_t*, size_t);

        void handleRemoteGameState(const uint8_t*, size_t);

        void sendSyncOKMessage();
        void sendGoodbye();

        void setAsLastSentMessage(uint8_t packageType);
        void setAsLastReceivedMessage(uint8_t packageType);
};

enum NetworkState {
    INACTIVE,
    DISCONNECTED,
    CONNECTED,
    JOINED,
    PLAYING
};

struct Networking {
    // Network status
    bool active;            // True if netplay code is currently running
    //bool networkErrorHappened;

    // TODO: State Pattern
    bool connectSuccessful;
    bool joinSuccessful;
    bool gameRunning;

    bool currentMenuChanged; // eg. room players changed
    bool operationInProgress; // the waiting dialogs watch this variable

    //
    NetClient client;
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

extern Networking netplay;

#endif // NETWORK_H
