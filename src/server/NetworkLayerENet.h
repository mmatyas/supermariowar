#ifndef NETWORK_LAYER_ENET_H
#define NETWORK_LAYER_ENET_H

#include "NetworkLayer.h"

#include <enet/enet.h>

#include <stdint.h>     // uintN_t

class NetworkLayerENet : public NetworkLayer
{
    public:
        NetworkLayerENet();
        ~NetworkLayerENet();

        bool init(uint64_t max_players);
        void cleanup();
        void listen(NetworkEventHandler&);

    private:
        ENetHost* server_host;
        ENetEvent last_event;
};

#endif // NETWORK_LAYER_ENET_H
