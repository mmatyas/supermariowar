#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include <cstddef>      // size_t
#include <stdint.h>     // uintN_t
#include <string>

class NetPeer
{
    public:
        NetPeer() {};
        virtual ~NetPeer() {}

        virtual bool send(const void*, size_t) = 0;
        virtual bool sendReliable(const void*, size_t) = 0;
        virtual void disconnect() = 0;

        // get address host and port in network byte order
        virtual uint32_t addressHost() = 0;
        virtual uint16_t addressPort() = 0;
        virtual std::string addressAsString() = 0;

        virtual uint32_t averageRTT() const = 0;

        virtual bool operator==(const NetPeer&) const = 0;
        virtual bool operator==(const NetPeer*) const = 0;
        virtual bool operator!=(const NetPeer& peer) const { return !(*this == peer); }
        virtual bool operator!=(const NetPeer* peer) const { return !(this == peer); }
};

class NetworkEventHandler
{
    public:
        virtual void onConnect(NetPeer*) = 0;
        virtual void onReceive(NetPeer&, const uint8_t*, size_t) = 0;
        virtual void onDisconnect(NetPeer& client) = 0;
};

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


#endif // NETWORK_INTERFACE_H
