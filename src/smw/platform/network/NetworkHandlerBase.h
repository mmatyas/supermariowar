#ifndef __NETWORK_HANDLER_H_
#define __NETWORK_HANDLER_H_

#include <cstdint>

class NetworkHandlerBase
{
	public:
		virtual bool init_networking();
		virtual bool init_client();
		virtual bool init_server();
		virtual bool cleanup();

		virtual bool openUDPConnection(const char* hostname, const uint16_t port) = 0;
		virtual void closeUDPConnection() = 0;

		virtual bool sendUDPMessage(const void* data, const unsigned dataLength) = 0;
		virtual bool receiveUDPMessage(const void* dataBuffer) = 0;
};

#endif // __NETWORK_HANDLER_H_