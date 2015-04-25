#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "NetworkLayer.h"
#include "Clock.h"


struct Player {
    std::string     name;
    NetClient*      network_client;

    uint32_t        currentRoomID;
    bool            isPlaying;
    uint8_t         playerNumberInRoom;
    bool            synchOK;

    TimePoint       joinTime;
    TimePoint       lastActivityTime;

    Player();
    ~Player();

    void setClient(NetClient* client);
    void setName(std::string& name);

    bool sendData(const void*, size_t);
    bool sendCode(uint8_t);
    bool sendConnectOK();

    std::string toString();
    std::string address_to_string();
};

#endif // PLAYER_H
