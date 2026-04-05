#include "Player.h"

#include "ProtocolPackages.h"

#include <chrono>
#include <cstdio>

#define SKINPKG_SIZE_LIMIT 20000 /* bytes in worst case */

void PlayerSkin::setPlayerID(uint8_t id)
{
    assert(data);
    assert(size > 3);
    if (!data || size < 4)
        return;

    data[3] = id;
}

Player::Player()
{
    currentRoomID = 0;
    isPlaying = false;
    playerNumberInRoom = 0;
    synchOK = false;

    name = "Anonymous";
    network_client = NULL;

    joinTime = time_now();
    lastActivityTime = joinTime;
}

Player::~Player()
{
    if (network_client)
        delete network_client;
}

void Player::setClient(NetPeer* client)
{
    assert(network_client == NULL);
    network_client = client;

    lastActivityTime = time_now();
}

void Player::setName(const std::string& name)
{
    this->name = name;

    lastActivityTime = time_now();
}

void Player::setSkin(const void* data, size_t data_length)
{
    // Some basic package validation
    if (data_length <= sizeof(NetPkgs::MessageHeader) + 5 /* id 1B + un-/compressed size 2*2B */
        || data_length > SKINPKG_SIZE_LIMIT) {
        printf("[error] Corrupt skin arrived from %s\n", toString().c_str());
        return;
    }

    skinPackage.replace_with(data, data_length);
    lastActivityTime = time_now();
}

// Printing stuff
std::string Player::toString()
{
    std::string out("[");
    out += name;
    out += "@";
    out += address_to_string();
    out += "]";
    return out;
}

std::string Player::address_to_string()
{
    if (!network_client)
        return "(null)";

    return network_client->addressAsString();
}

// Network stuff
bool Player::sendData(const void* data, size_t dataLength)
{
    assert(network_client);
    if (!network_client || !data || dataLength < 3)
        return false;

    lastActivityTime = time_now();
    return network_client->sendReliable(data, dataLength);
}

bool Player::sendCode(uint8_t code)
{
    NetPkgs::MessageHeader msg(code);
    return sendData(&msg, sizeof(NetPkgs::MessageHeader));
}

bool Player::sendConnectOK()
{
    return sendCode(NET_RESPONSE_CONNECT_OK);
}
