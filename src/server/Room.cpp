#include "Room.h"

#include "ProtocolPackages.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>

Room::Room()
    : roomID(0)
    , visible(true)
    , gamehost(NULL)
    , playerCount(0)
{}

Room::Room(uint32_t roomID, const char* name, const char* password, Player* host)
    : roomID(roomID)
    , visible(true) // TODO
    , gamehost(host)
    , playerCount(1)
    , gamemodeID(0) // Classic
    , gamemodeGoal(10) // 10 lives
{
    strncpy(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
    this->name[NET_MAX_ROOM_NAME_LENGTH - 1] = '\0';
    strncpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
    this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';

    createTime = TIME_NOW();
    lastActivityTime = createTime;
}

Room::~Room()
{
    // Game host player is freed by the server
}

void Room::changeName(const char* name)
{
    strncpy(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
    this->name[NET_MAX_ROOM_NAME_LENGTH - 1] = '\0';
}

void Room::setGamemode(uint8_t id, uint16_t goal) {
    gamemodeID = id;
    gamemodeGoal = goal;
    lastActivityTime = TIME_NOW();
}
