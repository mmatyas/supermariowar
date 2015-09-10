#ifndef NETWORK_PROTOCOL_PACKAGES_H
#define NETWORK_PROTOCOL_PACKAGES_H

#include "ProtocolDefinitions.h"

#include <cassert>
#include <cstring>
#include <stdint.h>


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

struct ServerInfo : MessageHeader {
    char           name[32];
    uint32_t       currentPlayerCount;
    uint32_t       maxPlayerCount;

    // Response package
    ServerInfo(const char* name, uint32_t players_current, uint32_t players_max)
        : MessageHeader(NET_RESPONSE_SERVERINFO)
    {
        currentPlayerCount = players_current;
        maxPlayerCount = players_max;
        strncpy(this->name, name, 32);
        this->name[31] = '\0';
    }
};

struct ClientConnection : MessageHeader {
    char           playerName[NET_MAX_PLAYER_NAME_LENGTH];
#if 0
    ClientConnection(const char* playerName)
        : MessageHeader(NET_REQUEST_CONNECT)
    {
        memcpy(this->playerName, playerName, NET_MAX_PLAYER_NAME_LENGTH);
        this->playerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';
    }
#endif
};

struct ClientDisconnection : MessageHeader {
    ClientDisconnection() : MessageHeader(NET_REQUEST_LEAVE_SERVER) {}
};

struct RoomList : MessageHeader {
    RoomList() : MessageHeader(NET_REQUEST_ROOM_LIST) {}
};

struct RoomInfo : MessageHeader {
    uint32_t       roomID;
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    uint8_t        currentPlayerCount;
    bool           passwordRequired;
    uint8_t        gamemodeID;

    // Response package
    RoomInfo() : MessageHeader(NET_RESPONSE_ROOM_LIST_ENTRY) {
        memset(name, '\0', NET_MAX_ROOM_NAME_LENGTH);
    }
};

struct NewRoom : MessageHeader {
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    char           password[NET_MAX_ROOM_PASSWORD_LENGTH];
    uint8_t        gamemodeID; // 0 - GAMEMODE_LAST
    uint16_t       gamemodeGoal;
#if 0
    NewRoom(const char* name, const char* password)
        : MessageHeader(NET_REQUEST_CREATE_ROOM)
    {
        memcpy(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
        this->name[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

        memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
#endif
};

struct NewRoomCreated : MessageHeader {
    uint32_t       roomID;

    // Response package
    NewRoomCreated(uint32_t roomID) : MessageHeader(NET_RESPONSE_CREATE_OK)
    {
        this->roomID = roomID;
    }
};

struct JoinRoom : MessageHeader {
    uint32_t       roomID;
    char           password[NET_MAX_ROOM_PASSWORD_LENGTH];
#if 0
    JoinRoom(uint32_t roomID, const char* password)
        : MessageHeader(NET_REQUEST_JOIN_ROOM)
    {
        this->roomID = roomID;
        memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
#endif
};

struct LeaveRoom : MessageHeader {
    LeaveRoom() : MessageHeader(NET_REQUEST_LEAVE_ROOM) {}
};

struct CurrentRoom : MessageHeader {
    uint32_t       roomID;
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    char           playerName[4][NET_MAX_PLAYER_NAME_LENGTH];
    uint8_t        hostPlayerNumber; //  1-4
    uint8_t        remotePlayerNumber; // of the receiving client
    uint8_t        gamemodeID; // 0 - GAMEMODE_LAST
    uint16_t       gamemodeGoal;

    // Response package
    CurrentRoom() : MessageHeader(NET_NOTICE_ROOM_CHANGED)
    {
        memset(playerName, 0, 4 * NET_MAX_PLAYER_NAME_LENGTH);
    }
};

struct RoomChatMsg : MessageHeader {
    uint8_t     playerNum;
    char        message[NET_MAX_CHAT_MSG_LENGTH];

    RoomChatMsg() : MessageHeader()
    {
        memset(message, 0, NET_MAX_CHAT_MSG_LENGTH);
    }

    RoomChatMsg(uint8_t playerNum, const char* msg)
        : MessageHeader(NET_NOTICE_ROOM_CHAT_MSG)
    {
        this->playerNum = playerNum;
        strncpy(message, msg, NET_MAX_CHAT_MSG_LENGTH);
        message[NET_MAX_CHAT_MSG_LENGTH - 1] = '\0';
    }
};


struct StartRoom : MessageHeader {
    StartRoom() : MessageHeader(NET_G2L_START_ROOM) {}
};

struct GameHostInfo : MessageHeader {
    uint32_t host;
    // port is SERVER_PORT + 1

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

struct SyncOK : MessageHeader {
    SyncOK() : MessageHeader(NET_P2G_SYNC_OK) {}
};

} // namespace NetPkgs

#endif // NETWORK_PROTOCOL_PACKAGES_H
