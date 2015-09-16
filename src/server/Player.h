#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "NetworkLayer.h"
#include "Clock.h"


struct Player {
    std::string     name;
    NetPeer*        network_client;

    uint32_t        currentRoomID;

    TimePoint       joinTime;
    TimePoint       lastActivityTime;

    Player();
    ~Player();

    void setClient(NetPeer* client);
    void setName(std::string& name);

    bool sendData(const void*, size_t);
    bool sendCode(uint8_t);
    bool sendConnectOK();

    bool isPlaying(); // true on currentRoomID > 0
    std::string toString();
    std::string address_to_string();
};

#endif // PLAYER_H
