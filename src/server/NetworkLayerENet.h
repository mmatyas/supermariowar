#ifndef NETWORK_LAYER_ENET_H
#define NETWORK_LAYER_ENET_H

#include "NetworkLayer.h"

#include <enet/enet.h>

#include <cstddef>      // size_t
#include <stdint.h>     // uintN_t
#include <string>

class NetClientENet : public NetClient
{
    public:
        NetClientENet(ENetPeer*);
        ~NetClientENet();

        bool sendData(const void*, size_t);
        uint32_t addressHost();
        uint16_t addressPort();
        std::string addressAsString();

    private:
        ENetPeer* foreign_peer;
};


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
