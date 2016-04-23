#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "Blob.h"
#include "Clock.h"
#include "NetworkLayer.h"

struct PlayerSkin: public Blob {
    PlayerSkin() : Blob() {}
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
    void setName(const std::string& name);
    void setSkin(const void*, size_t);

    bool sendData(const void*, size_t);
    bool sendCode(uint8_t);
    bool sendConnectOK();

    std::string toString();
    std::string address_to_string();
};

#endif // PLAYER_H
