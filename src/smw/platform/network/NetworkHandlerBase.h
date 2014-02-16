#ifndef __NETWORK_HANDLER_H_
#define __NETWORK_HANDLER_H_

#include <stdint.h>

#include "../../network/NetworkProtocol.h"

class NetworkHandlerBase
{
	public:
		virtual bool init_networking() = 0;
		virtual bool init_client() = 0;
		virtual bool init_server() = 0;
		virtual void cleanup() = 0;

		virtual bool openUDPConnection(const char* hostname, const uint16_t port) = 0;
		virtual void closeUDPConnection() = 0;

		virtual bool sendUDPMessage(const void* data, const unsigned dataLength) = 0;
		virtual bool receiveUDPMessage(void* dataBuffer) = 0;
};

#endif // __NETWORK_HANDLER_H_
