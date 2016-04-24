#ifndef NETWORK_H
#define NETWORK_H

#include "input.h"
#include "network/NetworkLayer.h"
#include "ProtocolDefinitions.h"

#include <chrono>
#include <list>
#include <string>
#include <vector>

typedef std::chrono::system_clock::time_point nettimepoint;

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
namespace NetPkgs { struct MapCollision; }

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
    uint8_t        hostPlayerNumber; // 1 - 4
    uint8_t        gamemodeID; // 0 - GAMEMODE_LAST
    uint16_t       gamemodeGoal;

    Room() {
        name = "";
        for (uint8_t p = 0; p < 4; p++)
            playerNames[p] = "(empty)";
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
        void sendCurrentGameStateIfNeeded();
        void sendPowerupStartByGH();
        void prepareMapCollisionEvent(CPlayer&);
        void sendMapCollisionEvent();
        void sendP2PCollisionEvent(CPlayer&, CPlayer&);
        void confirmCurrentInputs();


        LastMessage lastSentMessage;
        LastMessage lastReceivedMessage;

    private:
        bool active;

        uint32_t current_server_tick;

        // connected players except local client
        // TODO: support more players?
        NetPeer* clients[3];
        NetPeer* foreign_lobbyserver;

        // TODO: This class should be replaced eventually
        struct RawPlayerAddress {
            uint32_t host;
            uint16_t port;
            bool sync_ok;

            RawPlayerAddress() { reset(); }
            void reset() { host = 0; port = 0; sync_ok = false; }

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
        uint8_t last_processed_input_id[3];

        // Currently collision detection may change player data,
        // so it has to be saved before
        NetPkgs::MapCollision* preparedMapCollPkg;

        // P2.5. Pre-game
        void sendStartGameMessage();
        void sendSyncMessages();
        void handleSyncOKMessage(const NetPeer&, const uint8_t*, size_t);
        void setExpectedPlayers(uint8_t count, uint32_t* hosts, uint16_t* ports);

        // P3. Play
        void sendLocalInput();
        void sendCurrentGameStateNow();
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
        void onDisconnect(NetPeer&);

        // P1. Connect
        bool sendConnectRequestToSelectedServer();

        // P2. Join room
        void requestRoomList();
        void sendCreateRoomMessage();
        void sendJoinRoomMessage();
        void sendLeaveRoomMessage();
        void sendMapChangeMessage();
        void sendGameModeSettingsChangeMessage();
        void sendChatMessage(const char*);

        void sendSkinChange();

        // P3. Play
        void sendLeaveGameMessage();
        void storeLocalInput();
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

        bool connectLobby(const char* hostname, const uint16_t port = NET_LOBBYSERVER_PORT);
        bool connectGameHost(const char* hostname, const uint16_t port = NET_GAMEHOST_PORT);

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
        void handleMapChangeMessage(const uint8_t*, size_t);
        void handleGameModeSettingsChangeMessage(const uint8_t*, size_t);
        void handleSkinChangeMessage(const uint8_t*, size_t);
        void handleRoomChatMessage(const uint8_t*, size_t);

        // P2.5. Pre-game
        void handleRoomStartMessage(NetPeer&, const uint8_t*, size_t);
        void handleExpectedClientsMessage(NetPeer&, const uint8_t*, size_t);
        void handleStartSyncMessage(const uint8_t*, size_t);
        void handleGameStartMessage();

        // P3. Game
        void handleRemoteInput(const uint8_t*, size_t);
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
    float x;
    float y;
    float xvel;
    float yvel;

    Net_PlayerData();
};

struct Net_AllPlayerData {
    Net_PlayerData player[4];
    std::list<COutputControl> player_input[4];
};

struct Net_IndexedPlayerData: public Net_PlayerData {
    uint8_t input_id;

    Net_IndexedPlayerData(uint8_t id);
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
    uint8_t hostPlayerNumber;
    CPlayerInput netPlayerInput;

    bool waitingForPowerupTrigger;
    bool allowMapCollisionEvent;

    bool gamestate_changed;
    unsigned frames_since_last_gamestate;
    Net_AllPlayerData previous_playerdata;
    Net_AllPlayerData latest_playerdata;
    unsigned short last_confirmed_input;
    uint8_t current_input_counter;
    std::list<std::pair<uint8_t, COutputControl>> remote_input_buffer[4];
    bool player_disconnected[4];

    std::list<COutputControl> local_input_buffer;
    std::list<Net_IndexedPlayerData> local_playerdata_buffer;
    nettimepoint local_playerdata_store_time[256];
};

extern Networking netplay;

#endif // NETWORK_H
