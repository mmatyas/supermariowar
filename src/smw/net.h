#ifndef NETWORK_H
#define NETWORK_H

#include "input.h"
#include "network/NetworkInterface.h"
#include "network/ProtocolDefinitions.h"

#include <list>
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
class CPlayer;

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
    std::string    name; // NET_MAX_ROOM_NAME_LENGTH
    std::string    playerNames[4]; // NET_MAX_PLAYER_NAME_LENGTH
    uint8_t        hostPlayerNumber; // 1-4

    Room() {
        name[0] = '\0';
        for (uint8_t p = 0; p < 4; p++)
            playerNames[p] = "";
    }

    uint8_t playerCount() {
        uint8_t sum = 0;
        for (uint8_t p = 0; p < 4; p++)
            if (playerNames[p].compare("(empty)")) // != 0
                sum++;

        return sum;
    }
};


// Network communication classes

class NetGameHost : public NetworkEventHandler
{
    friend class NetClient;

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

        // P3. Game
        void sendCurrentGameState();
        void sendPowerupStart();
        void sendMapCollisionEvent(CPlayer&);
        void sendP2PCollisionEvent(CPlayer&, CPlayer&);


        LastMessage lastSentMessage;
        LastMessage lastReceivedMessage;

    private:
        bool active;

        // connected players except local client
        // TODO: support more players?
        NetPeer* clients[3];
        NetPeer* foreign_lobbyserver;

        // TODO: This class should be replaced eventually
        struct RawPlayerAddress {
            uint32_t host;
            uint16_t port;
            bool map_ok;
            bool sync_ok;

            RawPlayerAddress() { reset(); }
            void reset() { host = 0; port = 0; map_ok = false; sync_ok = false; }

            bool operator==(const RawPlayerAddress& other) const {
                if (host == other.host && port == other.port)
                    return true;
                return false;
            }

            bool operator!=(const RawPlayerAddress& other) const {
                return !(*this == other);
            }
        };

        RawPlayerAddress expected_clients[3];
        uint8_t expected_client_count;
        uint8_t next_free_client_slot;

        // P2.5. Pre-game
        void sendStartGameMessage();
        void sendMapSyncMessages();
        void handleMapOKMessage(const NetPeer&, const uint8_t*, size_t);
        void sendSyncMessages();
        void handleSyncOKMessage(const NetPeer&, const uint8_t*, size_t);
        void setExpectedPlayers(uint8_t count, uint32_t* hosts, uint16_t* ports);

        // P3. Play
        void handleRemoteInput(const NetPeer&, const uint8_t*, size_t);
        void handlePowerupRequest(const NetPeer&, const uint8_t*, size_t);

        bool sendMessageToMyPeers(const void*, size_t);

        void setAsLastSentMessage(uint8_t packageType);
        void setAsLastReceivedMessage(uint8_t packageType);
};

class NetClient : public NetworkEventHandler
{
    friend class NetGameHost;

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
        void sendChatMessage(const char*);

        // P3. Play
        void sendLeaveGameMessage();
        void sendLocalInput();
        void sendPowerupRequest();


        LastMessage lastSentMessage;
        LastMessage lastReceivedMessage;

        NetGameHost local_gamehost;

    private:
        NetPeer* foreign_lobbyserver;
        NetPeer* foreign_gamehost;

        MI_NetworkListScroll* uiRoomList;

        //uint8_t incomingData[NET_MAX_MESSAGE_SIZE];

        bool connectLobby(const char* hostname, const uint16_t port = NET_SERVER_PORT);
        bool connectGameHost(const char* hostname, const uint16_t port = NET_SERVER_PORT + 1);

        bool sendMessageToLobbyServer(const void* data, int dataLength);
        bool sendMessageToGameHost(const void* data, int dataLength);
        bool sendMessageToGameHostReliable(const void* data, int dataLength);
        bool sendTo(NetPeer*& peer, const void* data, int dataLength, bool reliable = false);

        // P1. Connect
        void handleServerinfoAndClose(const uint8_t*, size_t);

        // P2. Join room
        void handleNewRoomListEntry(const uint8_t*, size_t);
        void handleRoomCreatedMessage(const uint8_t*, size_t);
        void handleRoomChangedMessage(const uint8_t*, size_t);
        void handleRoomChatMessage(const uint8_t*, size_t);

        // P2.5. Pre-game
        void handleRoomStartMessage(const uint8_t*, size_t);
        void handleExpectedClientsMessage(const uint8_t*, size_t);
        void handleMapSyncMessage(const uint8_t*, size_t);
        void handleStartSyncMessage(const uint8_t*, size_t);
        void handleGameStartMessage();

        // P3. Game
        void handleRemoteGameState(const uint8_t*, size_t);
        void handlePowerupStart(const uint8_t*, size_t);
        void handlePowerupTrigger(const uint8_t*, size_t);
        void handleMapCollision(const uint8_t*, size_t);
        void handleP2PCollision(const uint8_t*, size_t);

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

struct Net_PlayerData {
    float          player_x[4];
    float          player_y[4];
    float          player_xvel[4];
    float          player_yvel[4];
    COutputControl player_input[4];

    void copyFromLocal();
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
    std::string mapfilepath;

    // In-game
    bool theHostIsMe;
    uint8_t remotePlayerNumber;
    CPlayerInput netPlayerInput;

    bool waitingForPowerupTrigger;
    bool allowMapCollisionEvent;

    bool gamestate_changed;
    Net_PlayerData latest_playerdata;
};

extern Networking netplay;

#endif // NETWORK_H
