#ifndef __NETWORK_H_
#define __NETWORK_H_

#include "SDL.h"
#include "SDL_net.h"

#define MAXCLIENTS      4
#define MAX_NETWORK_MESSAGE_SIZE 128

#define NET_MSG_JOIN			0
#define NET_MSG_JOIN_NLEN		1
#define NET_MSG_JOIN_NAME		NET_MSG_JOIN_NLEN + 1

#define NET_MSG_ADD				1
#define NET_MSG_ADD_SLOT		1
#define NET_MSG_ADD_NLEN		NET_MSG_ADD_SLOT + 2
#define NET_MSG_ADD_NAME		NET_MSG_ADD_NLEN + 1

#define NET_MSG_DEL				2
#define NET_MSG_DEL_SLOT		1
#define NET_MSG_DEL_LEN			NET_MSG_DEL_SLOT + 1

#define NET_MSG_REJECT			255
#define NET_MSG_REJECT_LEN		1

#define NET_MSG_CHAT			3
#define NET_MSG_CHAT_NLEN		1
#define NET_MSG_CHAT_BODY		NET_MSG_CHAT_NLEN + 1

void ReadFloatFromBuffer(float * pFloat, char * pData);
void ReadIntFromBuffer(int * pInt, char * pData);
void ReadShortFromBuffer(short * pShort, char * pData);
void ReadDoubleFromBuffer(double * pDouble, char * pData);

void WriteFloatToBuffer(char * pData, float dFloat);
void WriteIntToBuffer(char * pData, int iInt);
void WriteShortToBuffer(char * pData, short sShort);
void WriteDoubleToBuffer(char * pData, double dDouble);

//Write network handler class here
//Do similar to how gfx/sfx works with init and clean up functions
//Have all send and receive logic here
//Make sure to have message handler functions in main and menu loops

bool net_init();
void net_close();

bool net_runserver();
bool net_connectclient();

struct ServerClient
{
	int active;
	TCPsocket sock;
	IPaddress peer;
	Uint8 name[256 + 1];
};

struct ClientPeer
{
	int active;
	Uint8 name[256 + 1];
};

class NetServer
{
	public:

		NetServer();
		~NetServer();

		bool startserver();
		void update();

		void handleserver();
		void handleclient(int which);
		void sendnewclientmessage(int about, int to);
		void broadcastmessage(char * szMsg);

		void cleanup();

	private:
		
		IPaddress ip;
		TCPsocket tcpsock;
		SDLNet_SocketSet socketset;

		int numclients;
		ServerClient clients[MAXCLIENTS];
};

class NetClient
{
	public:

		NetClient();
		~NetClient();

		bool connecttoserver();
		void update();
		void handleserver();
		int handleserverdata(Uint8 *data);
		void sendjoin();
		
		void cleanup();
		
	private:

		IPaddress ip;
		TCPsocket tcpsock;
		SDLNet_SocketSet socketset;

		ClientPeer peers[MAXCLIENTS];
};

#endif //__NETWORK_H_

