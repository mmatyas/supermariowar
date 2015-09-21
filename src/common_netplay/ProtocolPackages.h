#ifndef NETWORK_PROTOCOL_PACKAGES_H
#define NETWORK_PROTOCOL_PACKAGES_H

#include "ProtocolDefinitions.h"

#include <cassert>
#include <cstring>
#include <stdint.h>

#ifndef IS_SERVER
#define IS_SERVER 0
#endif
#define IS_GAME !(IS_SERVER)


namespace NetPkgs {

struct MessageHeader {
    uint8_t     protocolMajorVersion;
    uint8_t     protocolMinorVersion;
    uint8_t     packageType;

    MessageHeader(uint8_t packageType = 0)
        : protocolMajorVersion(NET_PROTOCOL_VERSION_MAJOR)
        , protocolMinorVersion(NET_PROTOCOL_VERSION_MINOR)
        , packageType(packageType)
    {}
};


/*

    Connection packages

*/


struct ServerInfo : MessageHeader {
    char        name[32];
    uint32_t    currentPlayerCount;
    uint32_t    maxPlayerCount;

    ServerInfo()
        : MessageHeader(NET_RESPONSE_SERVERINFO)
        , currentPlayerCount(0)
        , maxPlayerCount(0)
    {
        memset(name, '\0', 32);
    }

#if IS_SERVER
    // Response package
    ServerInfo(const char* name, uint32_t players_current, uint32_t players_max)
        : MessageHeader(NET_RESPONSE_SERVERINFO)
        , currentPlayerCount(players_current)
        , maxPlayerCount(players_max)
    {
        assert(name);
        assert(strlen(name) > 2);
        assert(players_max > 0);

        strncpy(this->name, name, 32);
        this->name[31] = '\0';
    }
#endif
};

struct ClientConnection : MessageHeader {
    char    playerName[NET_MAX_PLAYER_NAME_LENGTH];

    ClientConnection()
        : MessageHeader(NET_REQUEST_CONNECT)
    {
        memset(playerName, '\0', NET_MAX_PLAYER_NAME_LENGTH);
    }

#if IS_GAME
    ClientConnection(const char* playerName)
        : MessageHeader(NET_REQUEST_CONNECT)
    {
        assert(playerName);
        assert(strlen(playerName) > 2);

        strncpy(this->playerName, playerName, NET_MAX_PLAYER_NAME_LENGTH);
        this->playerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';
    }
#endif
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
    uint32_t       roomID;
    char           name[NET_MAX_ROOM_NAME_LENGTH];
    uint8_t        currentPlayerCount;
    bool           passwordRequired;
    uint8_t        gamemodeID;

    RoomInfo()
        : MessageHeader(NET_RESPONSE_ROOM_LIST_ENTRY)
        , roomID(0)
        , currentPlayerCount(0)
        , passwordRequired(false)
        , gamemodeID(0)
    {
        memset(name, '\0', NET_MAX_ROOM_NAME_LENGTH);
    }
};

struct NewRoom : MessageHeader {
    char        name[NET_MAX_ROOM_NAME_LENGTH];
    char        password[NET_MAX_ROOM_PASSWORD_LENGTH];
    uint8_t     gamemodeID; // 0 - GAMEMODE_LAST
    uint16_t    gamemodeGoal;

    NewRoom()
        : MessageHeader(NET_REQUEST_CREATE_ROOM)
        , gamemodeID(0)
        , gamemodeGoal(10)
    {
        memset(name, '\0', NET_MAX_ROOM_NAME_LENGTH);
        memset(password, '\0', NET_MAX_ROOM_PASSWORD_LENGTH);
    }

#if IS_GAME
    NewRoom(const char* name, const char* password)
        : MessageHeader(NET_REQUEST_CREATE_ROOM)
        , gamemodeID(0)
        , gamemodeGoal(10)
    {
        assert(name);
        assert(password);
        assert(strlen(name) > 2);

        strncpy(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
        this->name[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

        strncpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
        this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';
    }
#endif
};

struct NewRoomCreated : MessageHeader {
    uint32_t       roomID;

    NewRoomCreated()
        : MessageHeader(NET_RESPONSE_CREATE_OK)
        , roomID(0)
    {}

#if IS_SERVER
    NewRoomCreated(uint32_t roomID)
        : MessageHeader(NET_RESPONSE_CREATE_OK)
        , roomID(roomID)
    {}
#endif
};

struct JoinRoom : MessageHeader {
    uint32_t    roomID;
    char        password[NET_MAX_ROOM_PASSWORD_LENGTH];

    JoinRoom()
        : MessageHeader(NET_REQUEST_JOIN_ROOM)
        , roomID(0)
    {
        memset(password, '\0', NET_MAX_ROOM_PASSWORD_LENGTH);
    }

#if IS_GAME
    JoinRoom(uint32_t roomID, const char* password)
        : MessageHeader(NET_REQUEST_JOIN_ROOM)
        , roomID(roomID)
    {
        assert(password);

        strncpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
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

    CurrentRoom()
        : MessageHeader(NET_NOTICE_ROOM_CHANGE)
        , roomID(0)
        , hostPlayerNumber(0)
        , remotePlayerNumber(0)
        , gamemodeID(0)
        , gamemodeGoal(10)
    {
        memset(name, '\0', NET_MAX_ROOM_NAME_LENGTH);
        memset(playerName, 0, 4 * NET_MAX_PLAYER_NAME_LENGTH);
    }
};

struct RoomChatMsg : MessageHeader {
    uint8_t     senderNum;
    char        message[NET_MAX_CHAT_MSG_LENGTH];

    RoomChatMsg()
        : MessageHeader(NET_NOTICE_ROOM_CHAT_MSG)
        , senderNum(0xFF)
    {
        memset(message, '\0', NET_MAX_CHAT_MSG_LENGTH);
    }

    RoomChatMsg(uint8_t playerNum, const char* msg)
        : MessageHeader(NET_NOTICE_ROOM_CHAT_MSG)
        , senderNum(playerNum)
    {
        assert(playerNum < 4);
        assert(msg);

        strncpy(message, msg, NET_MAX_CHAT_MSG_LENGTH);
        message[NET_MAX_CHAT_MSG_LENGTH - 1] = '\0';
    }
};

struct StartRoom : MessageHeader {
    StartRoom() : MessageHeader(NET_G2L_START_ROOM) {}
};


/*

    Pre-game packages

*/

struct GameHostInfo : MessageHeader {
    uint32_t host;
    // port is SERVER_PORT + 1

    GameHostInfo(uint32_t gh_address)
        : MessageHeader(NET_L2P_GAMEHOST_INFO)
        , host(gh_address)
    {}
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

} // namespace NetPkgs

#endif // NETWORK_PROTOCOL_PACKAGES_H
