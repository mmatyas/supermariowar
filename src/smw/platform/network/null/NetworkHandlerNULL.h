#ifndef __NETWORK_HANDLER_NULL_H_
#define __NETWORK_HANDLER_NULL_H_

#include "../NetworkHandlerBase.h"

class NetworkHandlerNULL : public NetworkHandlerBase
{
    public:
        bool openUDPConnection(const char* hostname, const uint16_t port) { return false; }
        void closeUDPConnection() {}

        bool sendUDPMessage(const void* data, const unsigned dataLength) { return false; }
        bool receiveUDPMessage(void* dataBuffer) {}

        bool openTCPConnection(const char* hostname, const uint16_t port) { return false; }
        void closeTCPConnection() {}

        bool sendTCPMessage(const void* data, const unsigned dataLength) { return false; }
        bool receiveTCPMessage(void* dataBuffer) {}
};

#endif // __NETWORK_HANDLER_NULL_H_
