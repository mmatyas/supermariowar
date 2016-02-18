#ifndef NETWORK_LAYER_NULL_H
#define NETWORK_LAYER_NULL_H

#include "network/NetworkLayer.h"

class NetworkLayerNULL : public NetworkLayer
{
    public:
        bool init() { return false; }
        void cleanup() {}

        bool client_restart() { return false; }
        void client_listen(NetworkEventHandler&) {}
        void client_shutdown() {}

        bool gamehost_restart() { return false; }
        void gamehost_listen(NetworkEventHandler&) {}
        void gamehost_shutdown() {}

        bool connectToLobbyServer(const char*, uint16_t) { return false; }
        bool connectToForeignGameHost(const char*, uint16_t) { return false; }
        bool natPunch(uint32_t host, uint16_t port) { return false; };
};

#endif // NETWORK_LAYER_NULL_H
