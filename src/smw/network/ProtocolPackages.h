#ifndef NETWORK_PROTOCOL_PACKAGES_H
#define NETWORK_PROTOCOL_PACKAGES_H

#include "ProtocolDefinitions.h"

#include <cassert>


struct Net_MessageHeader {
    uint8_t     protocolMajorVersion;
    uint8_t     protocolMinorVersion;
    uint8_t     packageType;
    //uint32_t       packageNumber;

    Net_MessageHeader(uint8_t packageType = 0) {
        protocolMajorVersion = NET_PROTOCOL_VERSION_MAJOR;
        protocolMinorVersion = NET_PROTOCOL_VERSION_MINOR;
        this->packageType = packageType;
    }
};


/*

    Connection packages

*/


struct Net_ServerInfoPackage : Net_MessageHeader {
    char        name[32];
    uint32_t    currentPlayerCount;
    uint32_t    maxPlayerCount;

    // Response package
};

struct Net_ClientConnectionPackage : Net_MessageHeader {
    char    playerName[NET_MAX_PLAYER_NAME_LENGTH];

    Net_ClientConnectionPackage(const char* playerName)
        : Net_MessageHeader(NET_REQUEST_CONNECT)
    {
        memcpy(this->playerName, playerName, NET_MAX_PLAYER_NAME_LENGTH);
        this->playerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';
    }
};

struct Net_ClientDisconnectionPackage : Net_MessageHeader {
    Net_ClientDisconnectionPackage() : Net_MessageHeader(NET_REQUEST_LEAVE_SERVER) {}
};


/*

    Room packages

*/


struct Net_RoomListPackage : Net_MessageHeader {
    Net_RoomListPackage() : Net_MessageHeader(NET_REQUEST_ROOM_LIST) {}
};

struct Net_RoomInfoPackage : Net_MessageHeader {
    uint32_t    roomID;
    char        name[NET_MAX_ROOM_NAME_LENGTH];
    uint8_t     currentPlayerCount;
    bool        passwordRequired;

    // Response package
};

struct Net_NewRoomPackage : Net_MessageHeader {
    char    name[NET_MAX_ROOM_NAME_LENGTH];
    char    password[NET_MAX_ROOM_PASSWORD_LENGTH];

    Net_NewRoomPackage(const char* name, const char* password)
        : Net_MessageHeader(NET_REQUEST_CREATE_ROOM)
    {
        memcpy(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
        this->name[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

        memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
};

struct Net_NewRoomCreatedPackage : Net_MessageHeader {
    uint32_t    roomID;

    // Response package
};

struct Net_JoinRoomPackage : Net_MessageHeader {
    uint32_t    roomID;
    char        password[NET_MAX_ROOM_PASSWORD_LENGTH];

    Net_JoinRoomPackage(uint32_t roomID, const char* password)
        : Net_MessageHeader(NET_REQUEST_JOIN_ROOM)
    {
        this->roomID = roomID;
        memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
};

struct Net_CurrentRoomPackage : Net_MessageHeader {
    uint32_t    roomID;
    char        name[NET_MAX_ROOM_NAME_LENGTH];
    char        playerName[4][NET_MAX_PLAYER_NAME_LENGTH];
    uint8_t     hostPlayerNumber; //  1-4
    uint8_t     remotePlayerNumber; // of the receiving client

    // Response package
};

struct Net_RoomChatMsgPackage : Net_MessageHeader {
    uint8_t     senderNum;
    char        message[NET_MAX_CHAT_MSG_LENGTH];

    Net_RoomChatMsgPackage() : Net_MessageHeader()
    {
        memset(message, 0, NET_MAX_CHAT_MSG_LENGTH);
    }

    Net_RoomChatMsgPackage(uint8_t playerNum, const char* msg)
        : Net_MessageHeader(NET_NOTICE_ROOM_CHAT_MSG)
    {
        senderNum = playerNum;
        memset(message, 0, NET_MAX_CHAT_MSG_LENGTH);
        memcpy(message, msg, NET_MAX_CHAT_MSG_LENGTH);
        message[NET_MAX_CHAT_MSG_LENGTH - 1] = '\0';
    }
};

struct Net_LeaveRoomPackage : Net_MessageHeader {
    Net_LeaveRoomPackage() : Net_MessageHeader(NET_REQUEST_LEAVE_ROOM) {}
};

struct Net_StartRoomPackage : Net_MessageHeader {
    Net_StartRoomPackage() : Net_MessageHeader(NET_G2L_START_ROOM) {}
};


/*

    Pre-game packages

*/

struct Net_GameHostInfoPkg : Net_MessageHeader {
    uint32_t host;

    Net_GameHostInfoPkg(uint32_t gh_host)
        : Net_MessageHeader(NET_L2P_GAMEHOST_INFO)
        , host(gh_host)
    { }
};

struct Net_PlayerInfoPkg : Net_MessageHeader {
    uint32_t host[3]; // 3 clients for a game host
    uint16_t port[3];

    Net_PlayerInfoPkg()
        : Net_MessageHeader(NET_L2G_CLIENTS_INFO)
    {
        memset(host, 0, sizeof(uint32_t) * 3);
        memset(port, 0, sizeof(uint32_t) * 3);
    }

    void setPlayer(uint8_t playerNum, uint32_t p_host, uint16_t p_port)
    {
        assert(playerNum < 3);

        host[playerNum] = p_host;
        port[playerNum] = p_port;
    }
};

struct Net_StartSyncPackage : Net_MessageHeader {
    uint32_t    commonRandomSeed;

    Net_StartSyncPackage(uint32_t seed)
        : Net_MessageHeader(NET_G2P_SYNC)
        , commonRandomSeed(seed)
    { }
};

struct Net_SyncOKPackage : Net_MessageHeader {
    Net_SyncOKPackage() : Net_MessageHeader(NET_P2G_SYNC_OK) {}
};

struct Net_StartGamePackage : Net_MessageHeader {
    Net_StartGamePackage() : Net_MessageHeader(NET_G2E_GAME_START) {}
};


/*

    Gameplay packages

*/

struct Net_LeaveGamePackage : Net_MessageHeader {
    Net_LeaveGamePackage() : Net_MessageHeader(NET_P2G_LEAVE_GAME) {}
};

// 2 byte bit field instead of 8*2, but you can't use arrays :(
struct Net_RawInput {
    uint16_t    flags;

    Net_RawInput() {
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
        pressed = (flags & (1 << ((keyNum << 1) + 1)));
    }
};

struct Net_ClientInputPackage : Net_MessageHeader {
    Net_RawInput    input;

    Net_ClientInputPackage(const COutputControl* playerControl)
        : Net_MessageHeader(NET_P2G_LOCAL_KEYS)
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

/*struct Net_RemoteInputPackage : Net_MessageHeader {
    uint8_t         playerNumber;
    Net_RawInput    input;

    // Response package

    void readKeys(COutputControl* playerControl) {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.getPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
};*/

struct Net_GameStatePackage : Net_MessageHeader {
    Net_PlayerCoords player_coords[4];
    Net_RawInput     player_input[4];

    Net_GameStatePackage() : Net_MessageHeader(NET_G2P_GAME_STATE) {}

    // SEND
    void setPlayerCoord(uint8_t playerNum, float x, float y) {
        assert(playerNum < 4);
        player_coords[playerNum].x = x;
        player_coords[playerNum].y = y;
    }

    void setPlayerVel(uint8_t playerNum, float xvel, float yvel) {
        assert(playerNum < 4);
        player_coords[playerNum].xvel = xvel;
        player_coords[playerNum].yvel = yvel;
    }

    void setPlayerKeys(uint8_t playerNum, const COutputControl* playerControl) {
        assert(playerNum < 4);
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            player_input[playerNum].setPlayerKey(playerNum,
                playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }

    // RECEIVE
    void getPlayerCoord(uint8_t playerNum, float& x, float& y) {
        assert(playerNum < 4);
        x = player_coords[playerNum].x;
        y = player_coords[playerNum].y;
    }

    void getPlayerVel(uint8_t playerNum, float& xvel, float& yvel) {
        assert(playerNum < 4);
        xvel = player_coords[playerNum].xvel;
        yvel = player_coords[playerNum].yvel;
    }

    void getPlayerKeys(uint8_t playerNum, COutputControl* playerControl) {
        assert(playerNum < 4);
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            player_input[playerNum].getPlayerKey(playerNum,
                playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
};

//struct Net_GameplayEvent
struct Net_P2PCollisionEventPackage : Net_MessageHeader {
    uint8_t player_id[2];
    Net_PlayerCoords player_coords[2]; // player position and vector at the moment of collision

    Net_P2PCollisionEventPackage(uint8_t firstID, uint8_t secondID)
        : Net_MessageHeader(NET_G2P_P2P_COLLISION_EVENT)
    {
        player_id[0] = firstID;
        player_id[1] = secondID;

        assert(player_id[0] < 4);
        assert(player_id[1] < 4);
        assert(player_id[0] != player_id[1]);

        player_coords[0].x = 0.0f;
        player_coords[0].y = 0.0f;
        player_coords[1].x = 0.0f;
        player_coords[1].y = 0.0f;
        player_coords[0].xvel = 0.0f;
        player_coords[0].yvel = 0.0f;
        player_coords[1].xvel = 0.0f;
        player_coords[1].yvel = 0.0f;
    }

    void setCoords(float p1_x, float p1_y, float p2_x, float p2_y) {
        player_coords[0].x = p1_x;
        player_coords[0].y = p1_y;
        player_coords[1].x = p2_x;
        player_coords[1].y = p2_y;
    }

    void setVelocities(float p1_xvel, float p1_yvel, float p2_xvel, float p2_yvel) {
        player_coords[0].xvel = p1_xvel;
        player_coords[0].yvel = p1_yvel;
        player_coords[1].xvel = p2_xvel;
        player_coords[1].yvel = p2_yvel;
    }
};

#endif // NETWORK_PROTOCOL_PACKAGES_H
