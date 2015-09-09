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
        ~NetPeerENet();

        bool send(const void*, size_t);
        bool sendReliable(const void*, size_t);
        void disconnect();

        uint32_t addressHost();
        uint16_t addressPort();
        std::string addressAsString();

        uint32_t averageRTT() const;

        bool operator==(const NetPeer&) const;
        bool operator==(const NetPeer*) const;

    private:
        ENetPeer* foreign_peer;
};

#endif // NETWORK_PEER_ENET_H
