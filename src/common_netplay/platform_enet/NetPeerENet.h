#ifndef NETWORK_PEER_ENET_H
#define NETWORK_PEER_ENET_H

#include "NetworkInterface.h"

#include <enet/enet.h>

#include <cstddef>      // size_t
#include <stdint.h>     // uintN_t
#include <string>

class NetPeerENet : public NetPeer
{
    public:
        NetPeerENet(ENetPeer*);
        ~NetPeerENet() override;

        bool send(const void*, size_t) override;
        bool sendReliable(const void*, size_t) override;
        void disconnect() override;

        uint32_t addressHost() override;
        uint16_t addressPort() override;
        std::string addressAsString() override;

        uint32_t averageRTT() const override;

        bool operator==(const NetPeer&) const override;
        bool operator==(const NetPeer*) const override;

    private:
        ENetPeer* foreign_peer;
};

#endif // NETWORK_PEER_ENET_H
