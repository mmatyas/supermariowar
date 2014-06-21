#ifndef NETWORK_PROTOCOL_PACKAGES_H
#define NETWORK_PROTOCOL_PACKAGES_H

#include <assert.h>

#include "NetworkProtocolCodes.h"

struct Net_MessageHeader {
    uint8_t     protocolVersion;
    uint8_t     packageType;
    //uint32_t       packageNumber;

    Net_MessageHeader(uint8_t packageType = 0) {
        protocolVersion = NET_PROTOCOL_VERSION;
        this->packageType = packageType;
    }
};

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

struct Net_LeaveRoomPackage : Net_MessageHeader {
    Net_LeaveRoomPackage() : Net_MessageHeader(NET_REQUEST_LEAVE_ROOM) {}
};

struct Net_StartRoomPackage : Net_MessageHeader {
    Net_StartRoomPackage() : Net_MessageHeader(NET_REQUEST_START_GAME) {}
};

struct Net_SyncOKPackage : Net_MessageHeader {
    Net_SyncOKPackage() : Net_MessageHeader(NET_NOTICE_GAME_SYNC_OK) {}
};

struct Net_CurrentRoomPackage : Net_MessageHeader {
    uint32_t    roomID;
    char        name[NET_MAX_ROOM_NAME_LENGTH];
    char        playerName[4][NET_MAX_PLAYER_NAME_LENGTH];
    uint8_t     hostPlayerNumber; //  1-4
    uint8_t     remotePlayerNumber; // of the receiving client

    // Response package
};

struct Net_StartSyncPackage : Net_MessageHeader {
    uint32_t    commonRandomSeed;
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
        pressed = (flags & (1 << (keyNum << 1)));
    }
};

struct Net_LocalInputPackage : Net_MessageHeader {
    Net_RawInput    input;

    Net_LocalInputPackage(const COutputControl* playerControl)
        : Net_MessageHeader(NET_NOTICE_LOCAL_KEYS)
    {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.setPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
};

struct Net_RemoteInputPackage : Net_MessageHeader {
    uint8_t         playerNumber;
    Net_RawInput    input;

    // Response package
    void readKeys(COutputControl* playerControl) {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.getPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
};

struct Net_GameStatePackage : Net_MessageHeader {
    float           player_x[4];
    float           player_y[4];
    float           player_xvel[4];
    float           player_yvel[4];
    Net_RawInput    input[4];

    Net_GameStatePackage() : Net_MessageHeader(NET_NOTICE_HOST_STATE) {}

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
};

#endif // NETWORK_PROTOCOL_PACKAGES_H
