#ifndef NETWORK_LAYER_H
#define NETWORK_LAYER_H

#include "NetworkInterface.h"

#include <stdint.h>     // uintN_t

class NetworkLayer
{
    public:
        NetworkLayer() {}
        virtual ~NetworkLayer() {}

        virtual bool init(uint64_t max_players) = 0;
        virtual void cleanup() = 0;
        virtual void listen(NetworkEventHandler&) = 0;
};

#endif // NETWORK_LAYER_H
