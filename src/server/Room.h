#ifndef ROOM_H
#define ROOM_H

#include "Clock.h"
#include "ProtocolDefinitions.h"
#include "Player.h"

#include <stdint.h>

struct Room {
    uint32_t        roomID;
    char            name[NET_MAX_ROOM_NAME_LENGTH];
    char            password[NET_MAX_ROOM_PASSWORD_LENGTH];
    bool            visible;

    Player*         gamehost;
    uint8_t         playerCount;

    uint8_t         gamemodeID;
    uint16_t        gamemodeGoal;

    TimePoint       createTime;
    TimePoint       lastActivityTime;

    Room();
    Room(uint32_t roomID, const char * name, const char * password, Player* host);
    ~Room();

    void setGamemode(uint8_t id, uint16_t goal);
};

#endif // ROOM_H
