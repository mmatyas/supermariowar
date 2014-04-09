#ifndef __NETWORK_HANDLER_H_
#define __NETWORK_HANDLER_H_

#include <stdint.h>

#include "../../network/NetworkProtocolCodes.h"

class NetworkHandlerBase
{
	public:
		virtual bool init_networking() { return true; };
		virtual bool init_client() { return true; };
		virtual bool init_server() { return true; };
		virtual void cleanup() {};

		// UDP
		virtual bool openUDPConnection(const char* hostname, const uint16_t port) = 0;
		virtual void closeUDPConnection() = 0;

		virtual bool sendUDPMessage(const void* data, const unsigned dataLength) = 0;
		virtual bool receiveUDPMessage(void* dataBuffer) = 0;

		// TCP
		virtual bool openTCPConnection(const char* hostname, const uint16_t port) = 0;
		virtual void closeTCPConnection() = 0;

		virtual bool sendTCPMessage(const void* data, const unsigned dataLength) = 0;
		virtual bool receiveTCPMessage(void* dataBuffer) = 0;
};

#endif // __NETWORK_HANDLER_H_
