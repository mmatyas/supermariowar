#include "Player.h"

#include "ProtocolPackages.h"

#include <chrono>

Player::Player()
    : name("Anonymous")
    , network_client(NULL)
    , currentRoomID(0)
{
    joinTime = TIME_NOW();
    lastActivityTime = joinTime;
}

Player::~Player()
{
    if (network_client)
        delete network_client;
}

bool Player::isPlaying()
{
    return currentRoomID > 0;
}

void Player::setClient(NetPeer* client)
{
    assert(network_client == NULL);
    network_client = client;

    lastActivityTime = TIME_NOW();
}

void Player::setName(std::string& name)
{
    this->name = name;

    lastActivityTime = TIME_NOW();
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

    lastActivityTime = TIME_NOW();
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
