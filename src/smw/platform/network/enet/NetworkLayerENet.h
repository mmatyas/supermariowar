#ifndef NETWORK_LAYER_ENET_H
#define NETWORK_LAYER_ENET_H

#include "network/NetworkInterface.h"

#include <enet/enet.h>

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

        bool operator==(const NetPeer&) const;
        bool operator==(const NetPeer*&) const;

    private:
        ENetPeer* foreign_peer;
};

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

    private:
        ENetHost* local_client;
        ENetEvent last_client_event;

        ENetHost* local_gamehost;
        ENetEvent last_gamehost_event;

        bool openConnection(const char* hostname, const uint16_t port);
        void listen(ENetHost*, ENetEvent&, NetworkEventHandler&);
};

#endif // NETWORK_LAYER_ENET_H
