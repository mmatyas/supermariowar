#ifndef NETWORK_LAYER_ENET_H
#define NETWORK_LAYER_ENET_H

#include "NetworkLayer.h"

#include <enet/enet.h>

#include <stdint.h>     // uintN_t

class NetworkLayerENet : public NetworkLayer
{
    public:
        NetworkLayerENet();
        ~NetworkLayerENet() override;

        bool init(uint64_t max_players) override;
        void cleanup() override;
        void listen(NetworkEventHandler&) override;

    private:
        ENetHost* server_host;
        ENetEvent last_event;
};

#endif // NETWORK_LAYER_ENET_H
