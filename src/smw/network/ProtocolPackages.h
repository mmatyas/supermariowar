#ifndef NETWORK_PROTOCOL_PACKAGES_H
#define NETWORK_PROTOCOL_PACKAGES_H

#include "ProtocolDefinitions.h"

#include <stdint.h>
#include <cassert>
#include <cstring>


namespace NetPkgs {

struct MessageHeader {
    uint8_t     protocolMajorVersion;
    uint8_t     protocolMinorVersion;
    uint8_t     packageType;
    //uint32_t       packageNumber;

    MessageHeader(uint8_t packageType = 0) {
        protocolMajorVersion = NET_PROTOCOL_VERSION_MAJOR;
        protocolMinorVersion = NET_PROTOCOL_VERSION_MINOR;
        this->packageType = packageType;
    }
};


/*

    Connection packages

*/


struct ServerInfo : MessageHeader {
    char        name[32];
    uint32_t    currentPlayerCount;
    uint32_t    maxPlayerCount;

    // Response package
};

struct ClientConnection : MessageHeader {
    char    playerName[NET_MAX_PLAYER_NAME_LENGTH];

    ClientConnection(const char* playerName)
        : MessageHeader(NET_REQUEST_CONNECT)
    {
        memcpy(this->playerName, playerName, NET_MAX_PLAYER_NAME_LENGTH);
        this->playerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';
    }
};

struct ClientDisconnection : MessageHeader {
    ClientDisconnection() : MessageHeader(NET_REQUEST_LEAVE_SERVER) {}
};


/*

    Room packages

*/


struct RoomList : MessageHeader {
    RoomList() : MessageHeader(NET_REQUEST_ROOM_LIST) {}
};

struct RoomInfo : MessageHeader {
    uint32_t    roomID;
    char        name[NET_MAX_ROOM_NAME_LENGTH];
    uint8_t     currentPlayerCount;
    bool        passwordRequired;
    uint8_t     gamemodeID;

    // Response package
};

struct NewRoom : MessageHeader {
    char        name[NET_MAX_ROOM_NAME_LENGTH];
    char        password[NET_MAX_ROOM_PASSWORD_LENGTH];
    uint8_t     gamemodeID; // 0 - GAMEMODE_LAST
    uint16_t    gamemodeGoal;

    NewRoom(const char* name, const char* password)
        : MessageHeader(NET_REQUEST_CREATE_ROOM)
        , gamemodeID(0)
        , gamemodeGoal(10)
    {
        memcpy(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
        this->name[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

        memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
};

struct NewRoomCreated : MessageHeader {
    uint32_t    roomID;

    // Response package
};

struct JoinRoom : MessageHeader {
    uint32_t    roomID;
    char        password[NET_MAX_ROOM_PASSWORD_LENGTH];

    JoinRoom(uint32_t roomID, const char* password)
        : MessageHeader(NET_REQUEST_JOIN_ROOM)
    {
        this->roomID = roomID;
        memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
};

struct CurrentRoom : MessageHeader {
    uint32_t    roomID;
    char        name[NET_MAX_ROOM_NAME_LENGTH];
    char        playerName[4][NET_MAX_PLAYER_NAME_LENGTH];
    uint8_t     hostPlayerNumber; //  1-4
    uint8_t     remotePlayerNumber; // of the receiving client
    uint8_t     gamemodeID; // 0 - GAMEMODE_LAST
    uint16_t    gamemodeGoal;

    // Response package
};

struct RoomChatMsg : MessageHeader {
    uint8_t     senderNum;
    char        message[NET_MAX_CHAT_MSG_LENGTH];

    RoomChatMsg() : MessageHeader()
    {
        memset(message, 0, NET_MAX_CHAT_MSG_LENGTH);
    }

    RoomChatMsg(uint8_t playerNum, const char* msg)
        : MessageHeader(NET_NOTICE_ROOM_CHAT_MSG)
    {
        senderNum = playerNum;
        memset(message, 0, NET_MAX_CHAT_MSG_LENGTH);
        memcpy(message, msg, NET_MAX_CHAT_MSG_LENGTH);
        message[NET_MAX_CHAT_MSG_LENGTH - 1] = '\0';
    }
};

struct LeaveRoom : MessageHeader {
    LeaveRoom() : MessageHeader(NET_REQUEST_LEAVE_ROOM) {}
};

struct StartRoom : MessageHeader {
    StartRoom() : MessageHeader(NET_G2L_START_ROOM) {}
};


/*

    Pre-game packages

*/

struct GameHostInfo : MessageHeader {
    uint32_t host;

    GameHostInfo(uint32_t gh_host)
        : MessageHeader(NET_L2P_GAMEHOST_INFO)
        , host(gh_host)
    { }
};

struct PlayerInfo : MessageHeader {
    uint32_t host[3]; // 3 clients for a game host
    uint16_t port[3];

    PlayerInfo()
        : MessageHeader(NET_L2G_CLIENTS_INFO)
    {
        memset(host, 0, sizeof(uint32_t) * 3);
        memset(port, 0, sizeof(uint16_t) * 3);
    }

    void setPlayer(uint8_t playerNum, uint32_t p_host, uint16_t p_port)
    {
        assert(playerNum < 3);

        host[playerNum] = p_host;
        port[playerNum] = p_port;
    }
};

struct StartSync : MessageHeader {
    uint32_t    commonRandomSeed;

    StartSync(uint32_t seed)
        : MessageHeader(NET_G2P_SYNC)
        , commonRandomSeed(seed)
    { }
};

struct MapSyncOK : MessageHeader {
    MapSyncOK() : MessageHeader(NET_P2G_MAP_OK) {}
};

struct SyncOK : MessageHeader {
    SyncOK() : MessageHeader(NET_P2G_SYNC_OK) {}
};

struct StartGame : MessageHeader {
    StartGame() : MessageHeader(NET_G2E_GAME_START) {}
};


/*

    Gameplay packages

*/

struct LeaveGame : MessageHeader {
    LeaveGame() : MessageHeader(NET_P2G_LEAVE_GAME) {}
};

// 2 byte bit field instead of 8*2, but you can't use arrays :(
struct RawInput {
    uint16_t    flags;

    RawInput() {
        flags = 0;
    }

    void setPlayerKey(uint8_t keyNum, bool down, bool pressed) {
        assert(keyNum < 8);
        flags |= (down ? 1 << (keyNum << 1) : 0);
        flags |= (pressed ? 1 << ((keyNum << 1) + 1) : 0);
    }

    void getPlayerKey(uint8_t keyNum, bool& down, bool& pressed) {
        assert(keyNum < 8);
        down = (flags & (1 << (keyNum << 1)));
        pressed = (flags & (1 << ((keyNum << 1) + 1)));
    }
};

struct ClientInput : MessageHeader {
    RawInput    input;

    ClientInput(const COutputControl* playerControl)
        : MessageHeader(NET_P2G_LOCAL_KEYS)
    {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.setPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }

    void readKeys(COutputControl* playerControl) {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.getPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
};

/*struct RemoteInput : MessageHeader {
    uint8_t         playerNumber;
    RawInput    input;

    // Response package

    void readKeys(COutputControl* playerControl) {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.getPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
};*/

struct GameState : MessageHeader {
    float           player_x[4];
    float           player_y[4];
    float           player_xvel[4];
    float           player_yvel[4];
    RawInput    input[4];

    GameState() : MessageHeader(NET_G2P_GAME_STATE) {}

    // SEND
    void setPlayerCoord(uint8_t playerNum, float x, float y) {
        assert(playerNum < 4);
        player_x[playerNum] = x;
        player_y[playerNum] = y;
    }

    void setPlayerVel(uint8_t playerNum, float xvel, float yvel) {
        assert(playerNum < 4);
        player_xvel[playerNum] = xvel;
        player_yvel[playerNum] = yvel;
    }

    void setPlayerKeys(uint8_t playerNum, const COutputControl* playerControl) {
        assert(playerNum < 4);
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input[playerNum].setPlayerKey(k,
                playerControl->keys[k].fDown,
                playerControl->keys[k].fPressed);
    }

    // RECEIVE
    void getPlayerCoord(uint8_t playerNum, float& x, float& y) {
        assert(playerNum < 4);
        x = player_x[playerNum];
        y = player_y[playerNum];
    }

    void getPlayerVel(uint8_t playerNum, float& xvel, float& yvel) {
        assert(playerNum < 4);
        xvel = player_xvel[playerNum];
        yvel = player_yvel[playerNum];
    }

    void getPlayerKeys(uint8_t playerNum, COutputControl* playerControl) {
        assert(playerNum < 4);
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input[playerNum].getPlayerKey(k,
                playerControl->keys[k].fDown,
                playerControl->keys[k].fPressed);
    }
};

struct RequestPowerup : MessageHeader {
    RequestPowerup() : MessageHeader(NET_P2G_REQ_POWERUP) {}
};

struct StartPowerup : MessageHeader {
    uint8_t player_id;
    uint8_t powerup_id;
    uint32_t delay; // time between source player and game host

    StartPowerup(uint8_t playerID, uint8_t powerupID, uint32_t delay)
        : MessageHeader(NET_G2P_START_POWERUP)
        , player_id(playerID)
        , powerup_id(powerupID)
        , delay(delay)
    {
        assert(playerID < 4 || playerID == 0xFF /* error value */);
        assert(powerupID < 128);
    }
};

struct TriggerPowerup : MessageHeader {
    uint8_t player_id;
    uint8_t powerup_id;
    float player_x;
    float player_y;

    TriggerPowerup(uint8_t playerID, uint8_t powerupID, float playerX, float playerY)
        : MessageHeader(NET_G2P_TRIGGER_POWERUP)
        , player_id(playerID)
        , powerup_id(powerupID)
        , player_x(playerX)
        , player_y(playerY)
    {
        // TODO: reduce bit count
        assert(playerID < 4);
        assert(powerupID < 128);
    }
};

struct MapCollision : MessageHeader {
    uint8_t player_id;
    float player_x;
    float player_y;
    float player_xvel;
    float player_yvel;

    MapCollision(): MessageHeader(NET_G2P_TRIGGER_MAPCOLL) {}

    MapCollision(CPlayer& player)
        : MessageHeader(NET_G2P_TRIGGER_MAPCOLL)
        , player_id(player.getGlobalID())
        , player_x(player.fx)
        , player_y(player.fy)
        , player_xvel(player.velx)
        , player_yvel(player.vely)
    {}

    void fill(CPlayer& player) {
        player_id = player.getGlobalID();
        player_x = player.fx;
        player_y = player.fy;
        player_xvel = player.velx;
        player_yvel = player.vely;
    }
};

struct P2PCollision : MessageHeader {
    uint8_t player_id[2];
    float player_x[2];
    float player_y[2];
    float player_xvel[2];
    float player_yvel[2];
    float player_oldy[2]; // used by stomp detection

    P2PCollision(): MessageHeader(NET_G2P_TRIGGER_P2PCOLL) {}

    P2PCollision(CPlayer& p1, CPlayer& p2)
        : MessageHeader(NET_G2P_TRIGGER_P2PCOLL)
    {
        player_id[0] = p1.getGlobalID();
        player_x[0] = p1.fx;
        player_y[0] = p1.fy;
        player_xvel[0] = p1.velx;
        player_yvel[0] = p1.vely;
        player_oldy[0] = p1.fOldY;

        player_id[1] = p2.getGlobalID();
        player_x[1] = p2.fx;
        player_y[1] = p2.fy;
        player_xvel[1] = p2.velx;
        player_yvel[1] = p2.vely;
        player_oldy[1] = p2.fOldY;
    }
};

} // namespace NetPkgs

#endif // NETWORK_PROTOCOL_PACKAGES_H
