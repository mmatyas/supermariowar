#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "NetworkLayer.h"
#include "Clock.h"

struct PlayerSkin {
    uint8_t* data;
    size_t size;

    PlayerSkin() : data(0), size(0) {}
    void setPlayerID(uint8_t id);
};

struct Player {
    std::string     name;
    NetPeer*        network_client;

    uint32_t        currentRoomID;
    bool            isPlaying;
    uint8_t         playerNumberInRoom;
    bool            synchOK;

    PlayerSkin      skinPackage;

    TimePoint       joinTime;
    TimePoint       lastActivityTime;

    Player();
    ~Player();

    void setClient(NetPeer* client);
    void setName(std::string& name);
    void setSkin(const void*, size_t);

    bool sendData(const void*, size_t);
    bool sendCode(uint8_t);
    bool sendConnectOK();

    std::string toString();
    std::string address_to_string();
};

#endif // PLAYER_H
