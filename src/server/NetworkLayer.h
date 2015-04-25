#ifndef NETWORK_LAYER_H
#define NETWORK_LAYER_H

#include <cstddef>      // size_t
#include <stdint.h>     // uintN_t
#include <string>

class NetClient
{
    public:
        NetClient() : playerID(0) {};
        virtual ~NetClient() {}

        virtual bool sendData(const void*, size_t) = 0;

        // get address host and port in network byte order
        virtual uint32_t addressHost() = 0;
        virtual uint16_t addressPort() = 0;

        //print address
        virtual std::string addressAsString() = 0;

        uint64_t getPlayerID() const { return playerID; }

    protected:
        uint64_t playerID;
};

class NetworkEventHandler
{
    public:
        virtual void onConnect(NetClient*) = 0;
        virtual void onReceive(NetClient&, const uint8_t*, size_t) = 0;
        virtual void onDisconnect(NetClient& client) = 0;
};

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
