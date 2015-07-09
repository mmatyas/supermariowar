#include "NetworkLayerENet.h"

#include "Log.h"
#include "ProtocolDefinitions.h"

#include <cassert>
#include <string>


NetClientENet::NetClientENet(ENetPeer* peer)
    : NetClient()
    , foreign_peer(peer)
{
    // Create a player ID based on address
    playerID = foreign_peer->address.host;
    playerID <<= sizeof(foreign_peer->address.port) * 8; // no. of bytes * bits
    playerID += foreign_peer->address.port;
}

NetClientENet::~NetClientENet()
{
    // Note: foreign_peer points to an element in ENet's internal
    // structure. When a player reconnects, the pointer gets the same value
    // (points to the same computer), so we shouldn't reset/delete it manually.
    // That will be handled by enet_deinitialize() at exit.
}

bool NetClientENet::sendData(const void* data, size_t length)
{
    assert(foreign_peer);
    if (!foreign_peer || !data || length <= 0)
        return false;

    ENetPacket* packet = enet_packet_create(data, length, ENET_PACKET_FLAG_RELIABLE);
    if (!packet)
        return false;

    if (enet_peer_send(foreign_peer, 1, packet) < 0)
        return false;

    return true;
}

uint32_t NetClientENet::addressHost()
{
    // ENet stores host in network byte order
    if (foreign_peer)
        return foreign_peer->address.host;

    return 0;
}

uint16_t NetClientENet::addressPort()
{
    // ENet stores port in host byte order -> convert it
    if (foreign_peer)
        return foreign_peer->address.port; // TODO/FIXME: check on other platforms

    return 0;
}

std::string NetClientENet::addressAsString()
{
    char buf[23]; // 4*4 octets with dots + 1 port separator + 5 port digits + 1 null

    uint8_t* addr = (uint8_t*)&foreign_peer->address.host;
    sprintf(buf, "%d.%d.%d.%d:%d",
        addr[0], addr[1], addr[2], addr[3], foreign_peer->address.port);

    return std::string(buf);
}

NetworkLayerENet::NetworkLayerENet()
    : NetworkLayer()
    , server_host(NULL)
{
}

NetworkLayerENet::~NetworkLayerENet()
{
    cleanup();
}

bool NetworkLayerENet::init(uint64_t max_players)
{
    if (enet_initialize() != 0) {
        log("[error][net] Could not initialize network system.\n");
        return false;
    }

    ENetVersion version = enet_linked_version();
    log("[net] ENet %u.%u.%u initialized.\n", ENET_VERSION_GET_MAJOR(version),
        ENET_VERSION_GET_MINOR(version), ENET_VERSION_GET_PATCH(version));


    ENetAddress server_address;
    server_address.host = ENET_HOST_ANY;
    server_address.port = NET_SERVER_PORT;

    // bind address
    // multiple incoming connections
    // two channels
    // no up/down speed limit
    server_host = enet_host_create(&server_address, max_players, 2, 0, 0);
    if (!server_host) {
        log("[error][net] Could not open connection port.\n");
        return false;
    }

    enet_host_compress(server_host, NULL);
    return true;
}

void NetworkLayerENet::cleanup()
{
    // if network code is still active
    if (server_host) {
        enet_host_destroy(server_host);
        server_host = NULL;

        enet_deinitialize();
    }
}

void NetworkLayerENet::listen(NetworkEventHandler& server)
{
    while (enet_host_service(server_host, &last_event, 0) > 0)
    {
        switch (last_event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                NetClientENet* newClient = new NetClientENet(last_event.peer);
                server.onConnect(newClient);
            }
            break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                NetClientENet client(last_event.peer);
                server.onReceive(client, last_event.packet->data, last_event.packet->dataLength);
                enet_packet_destroy(last_event.packet);
            }
            break;

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                NetClientENet client(last_event.peer);
                server.onDisconnect(client);
            }
            break;

            case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}
