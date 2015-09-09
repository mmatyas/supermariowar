#ifndef NETWORK_LAYER_H
#define NETWORK_LAYER_H

#include "NetworkInterface.h" // from common netplay dir

#include <stdint.h>     // uintN_t

class NetworkLayer
{
    public:
        NetworkLayer() {}
        virtual ~NetworkLayer() {}

        virtual bool init() { return true; }
        virtual void cleanup() {}

        virtual bool client_restart() = 0;
        virtual void client_listen(NetworkEventHandler&) = 0;
        virtual void client_shutdown() = 0;

        virtual bool gamehost_restart() = 0;
        virtual void gamehost_listen(NetworkEventHandler&) = 0;
        virtual void gamehost_shutdown() = 0;

        virtual bool connectToLobbyServer(const char*, uint16_t) = 0;
        virtual bool connectToForeignGameHost(const char*, uint16_t) = 0;
        virtual bool natPunch(uint32_t, uint16_t) = 0;
};

#endif // NETWORK_LAYER_H
