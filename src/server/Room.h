#ifndef ROOM_H
#define ROOM_H

#include "Clock.h"
#include "ProtocolDefinitions.h"
#include "Player.h"

#include <stdint.h>

struct Map {
    uint8_t* data;
    size_t size;

    Map() : data(0), size(0) {}
};

struct Room {
    uint32_t        roomID;
    char            name[NET_MAX_ROOM_NAME_LENGTH];
    char            password[NET_MAX_ROOM_PASSWORD_LENGTH];
    bool            visible;

    Player*         players[4];
    uint8_t         hostPlayerNumber; // 0-3 index in players[]
    uint8_t         playerCount;

    Map             mapPackage;

    uint8_t         gamemodeID;
    uint16_t        gamemodeGoal;

    TimePoint       createTime;
    TimePoint       lastActivityTime;

    Room();
    Room(uint32_t roomID, const char * name, const char * password, Player * host);
    ~Room();

    void tryAddingPlayer(Player* player);
    void removePlayer(Player* player);

    void setGamemode(uint8_t id, uint16_t goal);

    void sendChatMessage(Player*, const char*);
    void sendRoomUpdate();
    void shareSkinOf(Player*);
    void changeAndSendMap(const void*, size_t);
    void sendMap();
    void sendMapTo(uint8_t);
    void sendStartSignal(); // sends start sync
    void startGameIfEverybodyReady();
};

#endif // ROOM_H
