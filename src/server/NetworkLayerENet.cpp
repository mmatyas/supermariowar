#include "NetworkLayerENet.h"

#include "Log.h"
#include "ProtocolDefinitions.h"
#include "platform_enet/NetPeerENet.h"

#include <cassert>


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
        log("[error] Could not initialize network system.");
        return false;
    }

    ENetVersion version = enet_linked_version();
    log_silently("[info] ENet %u.%u.%u initialized.\n", ENET_VERSION_GET_MAJOR(version),
        ENET_VERSION_GET_MINOR(version), ENET_VERSION_GET_PATCH(version));


    ENetAddress server_address;
    server_address.host = ENET_HOST_ANY;
    server_address.port = NET_LOBBYSERVER_PORT;

    // bind address
    // multiple incoming connections
    // two channels
    // no up/down speed limit
    server_host = enet_host_create(&server_address, max_players, 2, 0, 0);
    if (!server_host) {
        log("[error] Could not open connection port.");
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
                NetPeerENet* newClient = new NetPeerENet(last_event.peer);
                server.onConnect(newClient);
            }
            break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                NetPeerENet client(last_event.peer);
                server.onReceive(client, last_event.packet->data, last_event.packet->dataLength);
                enet_packet_destroy(last_event.packet);
            }
            break;

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                NetPeerENet client(last_event.peer);
                server.onDisconnect(client);
            }
            break;

            case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}
