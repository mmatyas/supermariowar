#ifndef NETWORK_LAYER_ENET_H
#define NETWORK_LAYER_ENET_H

#include "network/NetworkLayer.h"

#include <enet/enet.h>

#include <stdint.h>     // uintN_t


class NetworkLayerENet : public NetworkLayer
{
    public:
        NetworkLayerENet();
        ~NetworkLayerENet();

        bool init();

        bool client_restart();
        void client_listen(NetworkEventHandler&);
        void client_shutdown();

        bool gamehost_restart();
        void gamehost_listen(NetworkEventHandler&);
        void gamehost_shutdown();

        void cleanup();

        bool connectToLobbyServer(const char* hostname, const uint16_t port);
        bool connectToForeignGameHost(const char* hostname, const uint16_t port);
        bool natPunch(uint32_t host, uint16_t port);

    private:
        ENetHost* local_client;
        ENetEvent last_client_event;

        ENetHost* local_gamehost;
        ENetEvent last_gamehost_event;

        bool openConnection(const char* hostname, const uint16_t port);
        void listen(ENetHost*, ENetEvent&, NetworkEventHandler&);
};

#endif // NETWORK_LAYER_ENET_H
