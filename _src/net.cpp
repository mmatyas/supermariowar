#include "global.h"
#include <string.h>

extern char szIPString[32];

#ifdef _WIN32
	#pragma comment(lib, "SDL_net.lib")
#endif

union
{
	float f;
	unsigned char b[4];
} dFloatAsBytes;

union
{
	int i;
	unsigned char b[4];
} iIntAsBytes;

union
{
	short s;
	unsigned char b[2];
} sShortAsBytes;

union
{
	double d;
	unsigned char b[8];
} dDoubleAsBytes;

void ReadFloatFromBuffer(float * pFloat, char * pData)
{
	dFloatAsBytes.b[0] = pData[0];
	dFloatAsBytes.b[1] = pData[1];
	dFloatAsBytes.b[2] = pData[2];
	dFloatAsBytes.b[3] = pData[3];

	*pFloat = dFloatAsBytes.f;
}

void ReadIntFromBuffer(int * pInt, char * pData)
{
	iIntAsBytes.b[0] = pData[0];
	iIntAsBytes.b[1] = pData[1];
	iIntAsBytes.b[2] = pData[2];
	iIntAsBytes.b[3] = pData[3];

	*pInt = iIntAsBytes.i;
}

void ReadShortFromBuffer(short * pShort, char * pData)
{
	sShortAsBytes.b[0] = pData[0];
	sShortAsBytes.b[1] = pData[1];

	*pShort = sShortAsBytes.s;
}

void ReadDoubleFromBuffer(double * pDouble, char * pData)
{
	dDoubleAsBytes.b[0] = pData[0];
	dDoubleAsBytes.b[1] = pData[1];
	dDoubleAsBytes.b[2] = pData[2];
	dDoubleAsBytes.b[3] = pData[3];
	dDoubleAsBytes.b[4] = pData[4];
	dDoubleAsBytes.b[5] = pData[5];
	dDoubleAsBytes.b[6] = pData[6];
	dDoubleAsBytes.b[7] = pData[7];

	*pDouble = dDoubleAsBytes.d;
}

void WriteFloatToBuffer(char * pData, float dFloat)
{
	dFloatAsBytes.f = dFloat;
	
  	pData[0] = dFloatAsBytes.b[0];
	pData[1] = dFloatAsBytes.b[1];
	pData[2] = dFloatAsBytes.b[2];
	pData[3] = dFloatAsBytes.b[3];
}

void WriteIntToBuffer(char * pData, int iInt)
{
	iIntAsBytes.i = iInt;

	pData[0] = iIntAsBytes.b[0];
	pData[1] = iIntAsBytes.b[1];
	pData[2] = iIntAsBytes.b[2];
	pData[3] = iIntAsBytes.b[3];
}

void WriteShortToBuffer(char * pData, short sShort)
{
	sShortAsBytes.s = sShort;

	pData[0] = sShortAsBytes.b[0];
	pData[1] = sShortAsBytes.b[1];
	pData[2] = sShortAsBytes.b[2];
	pData[3] = sShortAsBytes.b[3];
}

void WriteDoubleToBuffer(char * pData, double dDouble)
{
	dDoubleAsBytes.d = dDouble;
	
  	pData[0] = dDoubleAsBytes.b[0];
	pData[1] = dDoubleAsBytes.b[1];
	pData[2] = dDoubleAsBytes.b[2];
	pData[3] = dDoubleAsBytes.b[3];
	pData[4] = dDoubleAsBytes.b[4];
	pData[5] = dDoubleAsBytes.b[5];
	pData[6] = dDoubleAsBytes.b[6];
	pData[7] = dDoubleAsBytes.b[7];
}

bool net_init()
{
	if ( SDLNet_Init() < 0 ) 
	{
	    printf("SDLNet_Init: %s\n", SDLNet_GetError());
	    return false;
	}
	
	atexit(SDLNet_Quit);

	IPaddress ip;
	SDLNet_ResolveHost(&ip, "happy", 12521);
	sprintf(szIPString, "%d.%d.%d.%d", ip.host & 0xff, (ip.host >> 8) & 0xff, (ip.host >> 16) & 0xff, (ip.host >> 24) & 0xff);

	return true;
}

void net_close()
{
	SDLNet_Quit();
}


/********************************************************************
 * NetServer
 ********************************************************************/

NetServer::NetServer()
{
	numclients = 0;

	for (int i = 0; i < MAXCLIENTS; ++i )
	{
		clients[i].active = 0;
		clients[i].sock = NULL;
	}

	tcpsock = NULL;
	socketset = NULL;
}

NetServer::~NetServer()
{
	cleanup();
}

bool NetServer::startserver()
{
	socketset = SDLNet_AllocSocketSet(MAXCLIENTS + 1);

	if (socketset == NULL)
	{
		printf("Couldn't create socket set: %s\n", SDLNet_GetError());
		return false;
	}

	SDLNet_ResolveHost(&ip, NULL, 12521);

	printf("Server IP: %x:%d\n", ip.host, ip.port);

	tcpsock = SDLNet_TCP_Open(&ip);
	
	if (tcpsock == NULL)
	{
		cleanup();
		printf("Couldn't create server socket: %s\n", SDLNet_GetError());
		return false;
	}

	SDLNet_TCP_AddSocket(socketset, tcpsock);

	return true;
}


void NetServer::update()
{
	SDLNet_CheckSockets(socketset, 0);

	if(SDLNet_SocketReady(tcpsock))
	{
		handleserver();
	}

	for(int i = 0; i < MAXCLIENTS; ++i) 
	{
		if(SDLNet_SocketReady(clients[i].sock)) 
		{
			handleclient(i);
		}
	}
}

void NetServer::handleserver()
{
	int which;
	unsigned char data;

	TCPsocket newsock = SDLNet_TCP_Accept(tcpsock);

	if (newsock == NULL)
		return;
	
	/* Look for unconnected person slot */
	for (which = 0; which < MAXCLIENTS; ++which)
	{
		if (!clients[which].sock)
		{
			break;
		}
	}

	if (which == MAXCLIENTS)
	{
		/* Look for inactive person slot */
		for (which = 0; which < MAXCLIENTS; ++which)
		{
			if (clients[which].sock && !clients[which].active)
			{
				/* Kick them out.. */
				data = NET_MSG_REJECT;
				SDLNet_TCP_Send(clients[which].sock, &data, 1);
				SDLNet_TCP_DelSocket(socketset, clients[which].sock);
				SDLNet_TCP_Close(clients[which].sock);
				numclients--;

#ifdef _DEBUG
				printf("Killed inactive socket %d\n", which);
#endif
				break;
			}
		}
	}

	if (which == MAXCLIENTS)
	{
		/* No more room... */
		data = NET_MSG_REJECT;
		SDLNet_TCP_Send(newsock, &data, 1);
		SDLNet_TCP_Close(newsock);

#ifdef _DEBUG
		printf("Connection refused -- server full\n");
#endif
	}
	else
	{
		/* Add socket as an inactive person */
		clients[which].sock = newsock;
		clients[which].peer = *SDLNet_TCP_GetPeerAddress(newsock);
		SDLNet_TCP_AddSocket(socketset, clients[which].sock);
		numclients++;

#ifdef _DEBUG
		printf("New inactive socket %d\n", which);
#endif
	}
}

void NetServer::handleclient(int which)
{
	char data[512];
	int i;

	/* Has the connection been closed? */
	if (SDLNet_TCP_Recv(clients[which].sock, data, 512) <= 0)
	{

#ifdef _DEBUG
		printf("Closing socket %d (was%s active)\n", which, clients[which].active ? "" : " not");
#endif
		/* Notify all active clients */
		if (clients[which].active)
		{
			clients[which].active = 0;
			data[0] = NET_MSG_DEL;
			data[NET_MSG_DEL_SLOT] = which;
			
			for (i = 0; i < MAXCLIENTS; ++i) 
			{
				if (clients[i].active) 
				{
					SDLNet_TCP_Send(clients[i].sock, data, 2);
				}
			}
		}
		
		SDLNet_TCP_DelSocket(socketset, clients[which].sock);
		SDLNet_TCP_Close(clients[which].sock);
		clients[which].sock = NULL;
	} 
	else
	{
		switch (data[0]) 
		{
			case NET_MSG_JOIN: 
			{
				/* An active connection */
				memcpy(clients[which].name, &data[NET_MSG_JOIN_NAME], 256);
				clients[which].name[256] = 0;
#ifdef _DEBUG
				printf("Activating socket %d (%s)\n", which, clients[which].name);
#endif
				/* Notify all active clients */
				for (i = 0; i < MAXCLIENTS; ++i)
				{
					if (clients[i].active)
					{
						sendnewclientmessage(which, i);
					}
				}

				/* Notify about all active clients */
				clients[which].active = 1;
				for (i = 0; i < MAXCLIENTS; ++i)
				{
					if (clients[i].active)
					{
						sendnewclientmessage(i, which);
					}
				}
			}
			break;
			default: 
			{
				/* Unknown packet type?? */;
			}
			break;
		}
	}
}

void NetServer::sendnewclientmessage(int about, int to)
{
	char data[512];

	int n = strlen((char *)clients[about].name) + 1;
	data[0] = NET_MSG_ADD;
	data[NET_MSG_ADD_SLOT] = about;
	data[NET_MSG_ADD_NLEN] = n;
	memcpy(&data[NET_MSG_ADD_NAME], clients[about].name, n);
	SDLNet_TCP_Send(clients[to].sock, data, n + NET_MSG_ADD_NAME);
}

void NetServer::broadcastmessage(char * szMsg)
{
	char data[512];
	int iLength = strlen(szMsg) + 1;

	data[0] = NET_MSG_CHAT;
	data[NET_MSG_CHAT_NLEN] = iLength;
	memcpy(&data[NET_MSG_CHAT_BODY], szMsg, iLength);
	
	for (int k = 0; k < MAXCLIENTS; ++k)
	{
		if (clients[k].active)
		{
			int iResult = SDLNet_TCP_Send(clients[k].sock, data, iLength + NET_MSG_CHAT_BODY);
			
			if(iResult < iLength)
			{
				printf("SDLNet_TCP_Send Error: %s\n", SDLNet_GetError());
				
				// It may be good to disconnect sock because it is likely invalid now.
				SDLNet_TCP_DelSocket(socketset, clients[k].sock);
				SDLNet_TCP_Close(clients[k].sock);
				numclients--;
			}
		}
	}
}

void NetServer::cleanup()
{
	if (tcpsock != NULL)
	{
		SDLNet_TCP_Close(tcpsock);
		tcpsock = NULL;
	}
	
	if (socketset != NULL)
	{
		SDLNet_FreeSocketSet(socketset);
		socketset = NULL;
	}
}

/********************************************************************
 * NetClient
 ********************************************************************/
NetClient::NetClient()
{}

NetClient::~NetClient()
{}

bool NetClient::connecttoserver()
{
	//if(SDLNet_ResolveHost(&ip, game_values.hostaddress, 12521) == -1) 
	//if(SDLNet_ResolveHost(&ip, "10.115.8.222", 12521) == -1) 
	//if(SDLNet_ResolveHost(&ip, "10.115.5.65", 12521) == -1) 
	if(SDLNet_ResolveHost(&ip, "192.168.0.2", 12521) == -1) 
	//if(SDLNet_ResolveHost(&ip, "127.0.0.1", 12521) == -1) 
	{
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		return false;
	}

	tcpsock = SDLNet_TCP_Open(&ip);
	
	if(!tcpsock)
	{
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		return false;
	}

	/* Allocate the socket set for polling the network */
	socketset = SDLNet_AllocSocketSet(1);
	if (socketset == NULL)
	{
		printf("Couldn't create socket set: %s\n", SDLNet_GetError());
		return false;
	}

	SDLNet_TCP_AddSocket(socketset, tcpsock);
	
	return true;
}

void NetClient::update()
{
	SDLNet_CheckSockets(socketset, 0);

	if (SDLNet_SocketReady(tcpsock))
	{
		handleserver();
	}

}

void NetClient::handleserver()
{
	Uint8 data[512];
	int pos, len;
	int used;

	/* Has the connection been lost with the server? */
	len = SDLNet_TCP_Recv(tcpsock, (char *)data, 512);

	if ( len <= 0 ) 
	{
		SDLNet_TCP_DelSocket(socketset, tcpsock);
		SDLNet_TCP_Close(tcpsock);
		tcpsock = NULL;

	}
	else
	{
		pos = 0;
		while ( len > 0 )
		{
			used = handleserverdata(&data[pos]);
			pos += used;
			len -= used;
			
			if ( used == 0 )
			{
				/* We might lose data here.. oh well,
				   we got a corrupt packet from server
				 */
				len = 0;
			}
		}
	}
}

int NetClient::handleserverdata(Uint8 *data)
{
	int used;

	switch (data[0])
	{
		case NET_MSG_ADD:
		{
			/* Figure out which channel we got */
			Uint8 which = data[NET_MSG_ADD_SLOT];

			if ((which >= MAXCLIENTS) || peers[which].active)
			{
				/* Invalid channel?? */
				break;
			}

			/* Copy name into channel */
			memcpy(peers[which].name, &data[NET_MSG_ADD_NAME], 256);
			peers[which].name[256] = 0;
			peers[which].active = 1;

			/* Let the user know what happened */
			printf("* New client on %d \"%s\"\n", which, peers[which].name);

			used = NET_MSG_ADD_NAME + data[NET_MSG_ADD_NLEN];
			break;
		}
		
		case NET_MSG_DEL:
		{
			Uint8 which;

			/* Figure out which channel we lost */
			which = data[NET_MSG_DEL_SLOT];
			if((which >= MAXCLIENTS) || !peers[which].active)
			{
				/* Invalid channel?? */
				break;
			}
			peers[which].active = 0;

			/* Let the user know what happened */
			printf("* Lost client on %d (%s)\n", which, peers[which].name);

			used = NET_MSG_DEL_LEN;
			break;
		}

		case NET_MSG_REJECT:
		{
			printf("* Chat server full\n");
			used = NET_MSG_REJECT_LEN;
			break;
		}

		case NET_MSG_CHAT:
		{
			/* Copy name into channel */
			char szMsg[257];
			memcpy(szMsg, &data[NET_MSG_CHAT_BODY], 256);
			szMsg[256] = 0;
			
			/* Let the user know what happened */
			printf("Chat Message: \"%s\"\n", szMsg);

			used = NET_MSG_CHAT_BODY + data[NET_MSG_CHAT_NLEN];
			break;
		}

		default: 
		{
			/* Unknown packet type?? */;
			used = 0;
			break;
		}
	}

	return(used);
}

void NetClient::cleanup()
{
	/* Close the network connections */
	if (tcpsock != NULL)
	{
		SDLNet_TCP_Close(tcpsock);
		tcpsock = NULL;
	}
	
	if ( socketset != NULL )
	{
		SDLNet_FreeSocketSet(socketset);
		socketset = NULL;
	}
}

void NetClient::sendjoin()
{
	char join[1+1+256];
	char * name = "TestClient";

	if ( tcpsock != NULL )
	{
		/* Construct the packet */
		join[0] = NET_MSG_JOIN;

		int n = strlen(name);

		join[NET_MSG_JOIN_NLEN] = n;
		strncpy(&join[NET_MSG_JOIN_NAME], name, n);
		join[NET_MSG_JOIN_NAME+n++] = 0;

		/* Send it to the server */
		SDLNet_TCP_Send(tcpsock, join, NET_MSG_JOIN_NAME+n);
	}
}

