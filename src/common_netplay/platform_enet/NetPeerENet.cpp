#include "NetPeerENet.h"

#include <cassert>
#include <sstream>

#define CONNECTION_TIMEOUT_MS 5000
#define UNRELIABLE_CHANNEL 0
#define RELIABLE_CHANNEL 1


NetPeerENet::NetPeerENet(ENetPeer* peer)
    : NetPeer()
    , foreign_peer(peer)
{
    // Create a player ID based on address
    player_id = foreign_peer->address.host;
    player_id <<= sizeof(foreign_peer->address.port) * 8; // no. of bytes * bits
    player_id += foreign_peer->address.port;
}

NetPeerENet::~NetPeerENet()
{
    // Note: foreign_peer points to an element in ENet's internal structure.
    // When a player reconnects, the pointer gets the same value
    // (points to the same computer), so we shouldn't reset/delete it manually.
    // That will be handled by enet_deinitialize() at exit.
}

bool NetPeerENet::send(const void* data, size_t length)
{
    assert(foreign_peer);
    if (!foreign_peer || !data || length <= 0)
        return false;

    ENetPacket* packet = enet_packet_create(data, length, 0x0);
    if (!packet)
        return false;

    if (enet_peer_send(foreign_peer, UNRELIABLE_CHANNEL, packet) < 0)
        return false;

    return true;
}

bool NetPeerENet::sendReliable(const void* data, size_t length)
{
    assert(foreign_peer);
    if (!foreign_peer || !data || length <= 0)
        return false;

    ENetPacket* packet = enet_packet_create(data, length, ENET_PACKET_FLAG_RELIABLE);
    if (!packet)
        return false;

    if (enet_peer_send(foreign_peer, RELIABLE_CHANNEL, packet) < 0)
        return false;

    return true;
}

void NetPeerENet::disconnect()
{
    if (foreign_peer)
        enet_peer_disconnect_now(foreign_peer, 0x0);

    // Note: do NOT null out foreign_peer!
}

uint32_t NetPeerENet::addressHost()
{
    // ENet stores host in network byte order
    if (foreign_peer)
        return foreign_peer->address.host;

    return 0;
}

uint16_t NetPeerENet::addressPort()
{
    // ENet stores port in host byte order -> convert it
    if (foreign_peer)
        return foreign_peer->address.port; // TODO/FIXME: check on other platforms

    return 0;
}

std::string NetPeerENet::addressAsString()
{
    uint8_t* addr = (uint8_t*)&foreign_peer->address.host;

    std::ostringstream ss;
    ss << (unsigned short)addr[0] <<"."<< (unsigned short)addr[1] <<"."<<
          (unsigned short)addr[2] <<"."<< (unsigned short)addr[3] <<":"<< foreign_peer->address.port;

    return ss.str();
}

uint32_t NetPeerENet::averageRTT() const
{
    return foreign_peer->roundTripTime;
}

bool NetPeerENet::operator==(const NetPeer* peer) const
{
    if (!peer)
        return false;

    if (this->foreign_peer == static_cast<const NetPeerENet*>(peer)->foreign_peer)
        return true;

    return false;
}

bool NetPeerENet::operator==(const NetPeer& peer) const
{
    if (this->foreign_peer == static_cast<const NetPeerENet&>(peer).foreign_peer)
        return true;

    return false;
}
