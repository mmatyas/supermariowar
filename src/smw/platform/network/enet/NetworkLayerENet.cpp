#include "NetworkLayerENet.h"

#include "ProtocolDefinitions.h"
#include "platform_enet/NetPeerENet.h"

#include <cassert>
#include <cstdio>


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
    printf("[net] ENet %u.%u.%u initialized.\n",
        ENET_VERSION_GET_MAJOR(version),
        ENET_VERSION_GET_MINOR(version),
        ENET_VERSION_GET_PATCH(version));

    return true;
}

bool NetworkLayerENet::client_restart()
{
    client_shutdown();

    // no bind
    // two connections (lobby server and game host)
    // two channels (a reliable and an unreliable)
    // no up/down speed limit
    local_client = enet_host_create(NULL, 2, 2, 0, 0);
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
    local_gamehost_address.port = NET_GAMEHOST_PORT;

    // bind address
    // max. 3 connections + 3 NAT punch
    // two channels (a reliable and an unreliable)
    // no up/down speed limit
    local_gamehost = enet_host_create(&local_gamehost_address, 6, 2, 0, 0);
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
    // local_client connects
    // to target_address
    // on two channels
    // without any initial data
    ENetPeer* target_host = enet_host_connect(local_client, &target_address, 2, 0x0);
    if (!target_host) {
        printf("[net] Could not initiate connection to this address.\n");
        return false;
    }

    return true;
}

// This is almost the same as openConnection, except it's launched from the game hosting port
// TODO: refactor maybe?
// TODO: add router hairpinning support
bool NetworkLayerENet::natPunch(uint32_t host, uint16_t port)
{
    ENetAddress target_address;
    target_address.host = host;
    target_address.port = port;

    // Initiate connection
    // local_gamehost connects
    // to target_address
    // on two channels
    // without any initial data
    ENetPeer* target_host = enet_host_connect(local_gamehost, &target_address, 2, 0x0);
    if (!target_host) {
        printf("[net] Could not initiate connection to a client.\n");
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

            default:
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
