#include "NetworkLayerENet.h"

#include "network/ProtocolDefinitions.h"

#include <cassert>
#include <cstdio>
#include <cstring>

#define CONNECTION_TIMEOUT_MS 5000

NetPeerENet::NetPeerENet(ENetPeer* peer)
    : NetPeer()
    , foreign_peer(peer)
{
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

    ENetPacket* packet = enet_packet_create(data, length, ENET_PACKET_FLAG_RELIABLE);
    if (!packet)
        return false;

    if (enet_peer_send(foreign_peer, 0, packet) < 0)
        return false;

    return true;
}

void NetPeerENet::disconnect()
{
    if (foreign_peer)
        enet_peer_disconnect_now(foreign_peer, 0x0);

    // Note: do NOT null out foreign_peer!
}

std::string NetPeerENet::addressAsString()
{
    char buf[23]; // 4*4 octets with dots + 1 port separator + 5 port digits + 1 null

    uint8_t* addr = (uint8_t*)&foreign_peer->address.host;
    sprintf(buf, "%d.%d.%d.%d::%d",
        addr[0], addr[1], addr[2], addr[3], foreign_peer->address.port);

    return std::string(buf);
}

bool NetPeerENet::operator==(const NetPeer*& peer) const
{
    if (this->foreign_peer == ((NetPeerENet*)peer)->foreign_peer)
        return true;

    return false;
}

bool NetPeerENet::operator!=(const NetPeer*& peer) const
{
    return !(this == peer);
}



NetworkLayerENet::NetworkLayerENet()
    : NetworkLayer()
    , local_client(NULL)
    , local_gamehost(NULL)
{
}

NetworkLayerENet::~NetworkLayerENet()
{
    cleanup();
}

/**************************************
    Init
**************************************/

bool NetworkLayerENet::init()
{
    if (enet_initialize() != 0) {
        fprintf(stderr, "[error][net] Could not initialize network system.\n");
        return false;
    }

    ENetVersion version = enet_linked_version();
    printf("[net] ENet %u.%u.%u initialized.\n", ENET_VERSION_GET_MAJOR(version),
        ENET_VERSION_GET_MINOR(version), ENET_VERSION_GET_PATCH(version));

    return true;
}

bool NetworkLayerENet::client_restart()
{
    client_shutdown();

    // no bind
    // one connection (to lobby server)
    // one channel
    // no up/down speed limit
    local_client = enet_host_create(NULL, 1, 1, 0, 0);
    if (!local_client) {
        fprintf(stderr, "[error][net] Could not open client connection port.\n");
        return false;
    }
    return true;
}

bool NetworkLayerENet::gamehost_restart()
{
    gamehost_shutdown();

    ENetAddress local_gamehost_address;
    local_gamehost_address.host = ENET_HOST_ANY;
    local_gamehost_address.port = NET_SERVER_PORT + 1;

    // bind address
    // max. 3 connections
    // one channel
    // no up/down speed limit
    local_gamehost = enet_host_create(&local_gamehost_address, 3, 1, 0, 0);
    if (!local_gamehost) {
        fprintf(stderr, "[error][net] Could not open game host connection port.\n");
        return false;
    }
    return true;
}

/**************************************
    Connect
**************************************/

bool NetworkLayerENet::connectToLobbyServer(const char* hostname, const uint16_t port)
{
    if (!openConnection(hostname, port))
        return false;

    return true;
}

bool NetworkLayerENet::connectToForeignGameHost(const char* hostname, const uint16_t port)
{
    if (!openConnection(hostname, port))
        return false;

    return true;
}

bool NetworkLayerENet::openConnection(const char* hostname, const uint16_t port)
{
    // Set target host address
    ENetAddress target_address;
    enet_address_set_host(&target_address, hostname);
    target_address.port = port;

    // Initiate connection
    // client_host connects
    // to target_address
    // on one channel
    // without any data
    ENetPeer* target_host = enet_host_connect(local_client, &target_address, 1, 0x0);
    if (!target_host) {
        printf("[net] Could not initiate connection to this address.\n");
        return false;
    }

    return true;
}

/**************************************
    Communication
**************************************/

void NetworkLayerENet::client_listen(NetworkEventHandler& client)
{
    listen(local_client, last_client_event, client);
}

void NetworkLayerENet::gamehost_listen(NetworkEventHandler& gamehost)
{
    listen(local_gamehost, last_gamehost_event, gamehost);
}

void NetworkLayerENet::listen(ENetHost* host, ENetEvent& last_host_event, NetworkEventHandler& listener)
{
    while (enet_host_service(host, &last_host_event, 0) > 0)
    {
        switch (last_host_event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                NetPeerENet* peer = new NetPeerENet(last_host_event.peer);
                listener.onConnect(peer);
            }
            break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                NetPeerENet peer(last_host_event.peer);
                listener.onReceive(peer, last_host_event.packet->data, last_host_event.packet->dataLength);
                enet_packet_destroy(last_host_event.packet);
            }
            break;

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                NetPeerENet peer(last_host_event.peer);
                listener.onDisconnect(peer);
            }
            break;
        }
    }
}

/**************************************
    Cleanup
**************************************/

void NetworkLayerENet::cleanup()
{
    gamehost_shutdown();
    client_shutdown();
    enet_deinitialize();
}

void NetworkLayerENet::client_shutdown()
{
    if (!local_client)
        return;

    // Shut down
    enet_host_destroy(local_client);
    local_client = NULL;
}

void NetworkLayerENet::gamehost_shutdown()
{
    if (!local_gamehost)
        return;

    // Shut down
    enet_host_destroy(local_gamehost);
    local_gamehost = NULL;
}
