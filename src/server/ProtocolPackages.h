#ifndef __NETWORK_PROTOCOL_PACKAGES_H_
#define __NETWORK_PROTOCOL_PACKAGES_H_

#include "ProtocolDefinitions.h"

#include <cassert>
#include <cstring>
#include <stdint.h>

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

struct ServerInfoPackage : MessageHeader {
    char           name[32];
    uint32_t       currentPlayerCount;
    uint32_t       maxPlayerCount;

    // Response package
    ServerInfoPackage(const char* name, uint32_t players_current, uint32_t players_max)
        : MessageHeader(NET_RESPONSE_SERVERINFO)
    {
        currentPlayerCount = players_current;
        maxPlayerCount = players_max;
        strcpy(this->name, name);
    }
};

struct ClientConnectionPackage : MessageHeader {
    char           playerName[NET_MAX_PLAYER_NAME_LENGTH];
#if 0
    ClientConnectionPackage(const char* playerName)
        : MessageHeader(NET_REQUEST_CONNECT)
    {
        memcpy(this->playerName, playerName, NET_MAX_PLAYER_NAME_LENGTH);
        this->playerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';
    }
#endif
};

struct ClientDisconnectionPackage : MessageHeader {
    ClientDisconnectionPackage() : MessageHeader(NET_REQUEST_LEAVE_SERVER) {}
};

struct RoomListPackage : MessageHeader {
    RoomListPackage() : MessageHeader(NET_REQUEST_ROOM_LIST) {}
};

struct RoomInfoPackage : MessageHeader {
    uint32_t       roomID;
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    uint8_t        currentPlayerCount;
    bool           passwordRequired;

    // Response package
};

struct NewRoomPackage : MessageHeader {
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    char           password[NET_MAX_ROOM_PASSWORD_LENGTH];
#if 0
    NewRoomPackage(const char* name, const char* password)
        : MessageHeader(NET_REQUEST_CREATE_ROOM)
    {
        memcpy(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
        this->name[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

        memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
#endif
};

struct NewRoomCreatedPackage : MessageHeader {
    uint32_t       roomID;

    // Response package
    NewRoomCreatedPackage(uint32_t roomID) : MessageHeader(NET_RESPONSE_CREATE_OK)
    {
        this->roomID = roomID;
    }
};

struct JoinRoomPackage : MessageHeader {
    uint32_t       roomID;
    char           password[NET_MAX_ROOM_PASSWORD_LENGTH];
#if 0
    JoinRoomPackage(uint32_t roomID, const char* password)
        : MessageHeader(NET_REQUEST_JOIN_ROOM)
    {
        this->roomID = roomID;
        memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
#endif
};

struct LeaveRoomPackage : MessageHeader {
    LeaveRoomPackage() : MessageHeader(NET_REQUEST_LEAVE_ROOM) {}
};

struct CurrentRoomPackage : MessageHeader {
    uint32_t       roomID;
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    char           playerName[4][NET_MAX_PLAYER_NAME_LENGTH];
    uint8_t        hostPlayerNumber; //  1-4
    uint8_t        remotePlayerNumber; // of the receiving client

    // Response package
    CurrentRoomPackage() : MessageHeader(NET_NOTICE_ROOM_CHANGED)
    {
        memset(playerName, 0, 4 * NET_MAX_PLAYER_NAME_LENGTH);
    }
};

struct Net_RoomChatMsgPackage : MessageHeader {
    uint8_t     playerNum;
    char        message[NET_MAX_CHAT_MSG_LENGTH];

    Net_RoomChatMsgPackage() : MessageHeader()
    {
        memset(message, 0, NET_MAX_CHAT_MSG_LENGTH);
    }

    Net_RoomChatMsgPackage(uint8_t playerNum, const char* msg)
        : MessageHeader(NET_NOTICE_ROOM_CHAT_MSG)
    {
        this->playerNum = playerNum;
        memset(message, 0, NET_MAX_CHAT_MSG_LENGTH);
        memcpy(message, msg, NET_MAX_CHAT_MSG_LENGTH);
        message[NET_MAX_CHAT_MSG_LENGTH - 1] = '\0';
    }
};


struct Net_StartRoomPackage : MessageHeader {
    Net_StartRoomPackage() : MessageHeader(NET_G2L_START_ROOM) {}
};

struct Net_GameHostInfoPkg : MessageHeader {
    uint32_t host;
    // port is SERVER_PORT + 1

    Net_GameHostInfoPkg(uint32_t gh_host)
        : MessageHeader(NET_L2P_GAMEHOST_INFO)
        , host(gh_host)
    { }
};

struct Net_PlayerInfoPkg : MessageHeader {
    uint32_t host[3]; // 3 clients for a game host
    uint16_t port[3];

    Net_PlayerInfoPkg()
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

struct Net_StartSyncPackage : MessageHeader {
    uint32_t    commonRandomSeed;

    Net_StartSyncPackage(uint32_t seed)
        : MessageHeader(NET_G2P_SYNC)
        , commonRandomSeed(seed)
    { }
};

struct Net_SyncOKPackage : MessageHeader {
    Net_SyncOKPackage() : MessageHeader(NET_P2G_SYNC_OK) {}
};



/*

// 2 byte bit field instead of 8*2, but you can't use arrays :(
struct RawInput {
    uint16_t flags;

#if 0
    RawInput() {
        flags = 0;
    }

    void setPlayerKey(uint8_t keyNum, bool down, bool pressed) {
        assert(keyNum < 8);
        flags |= ((down ? 1 : 0) << (keyNum << 1));
        flags |= ((pressed ? 1 : 0) << ((keyNum << 1) + 1));
    }

    void getPlayerKey(uint8_t keyNum, bool& down, bool& pressed) {
        assert(keyNum < 8);
        down = (flags & (1 << (keyNum << 1)));
        pressed = (flags & (1 << (keyNum << 1)));
    }
#endif
};

struct LocalInputPackage : MessageHeader {
    RawInput       input;

#if 0
    LocalInputPackage(const COutputControl* playerControl)
        : MessageHeader(NET_NOTICE_LOCAL_KEYS)
    {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.setPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
#endif
};

struct RemoteInputPackage : MessageHeader {
    uint8_t        playerNumber;
    RawInput       input;

    // Response package
#if 0
    void readKeys(COutputControl* playerControl) {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.getPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
#endif
};

struct GameStatePackage : MessageHeader {
    float          player_x[4];
    float          player_y[4];
    float          player_xvel[4];
    float          player_yvel[4];
    RawInput       input[4];
#if 0
    GameStatePackage() : MessageHeader(NET_NOTICE_HOST_STATE) {}

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
            input[playerNum].setPlayerKey(playerNum,
                playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
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
            input[playerNum].getPlayerKey(playerNum,
                playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
#endif
};*/

#endif // __NETWORK_PROTOCOL_PACKAGES_H_
