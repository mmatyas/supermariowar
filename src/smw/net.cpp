#include "global.h"
#include <string.h>

#include "net.h"

#ifdef _WIN32
	#pragma comment(lib, "SDL_net.lib")
#endif

Networking netplay;


bool sendTCPMessage(TCPsocket& target, void* data, int dataLength) // int a Send miatt
{
	if (!data || !dataLength || !target)
		return false;

	if (SDLNet_TCP_Send(target, data, dataLength) < dataLength) {
		fprintf(stderr, "[Warning] Sending message failed. %s\n", SDLNet_GetError());
		return false;
	}

	return true;
}

bool receiveTCPMessage(TCPsocket& source, void* buffer, int bufferMaxSize) // int a Recv miatt
{
	if (!buffer || bufferMaxSize <= 0 || !source)
		return false;

	if (SDLNet_TCP_Recv(source, buffer, bufferMaxSize) <= 0) {
        fprintf(stderr, "[Warning] Receiving message failed. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

union {
	float f;
	unsigned char b[4];
} dFloatAsBytes;

union {
	int i;
	unsigned char b[4];
} iIntAsBytes;

union {
	short s;
	unsigned char b[2];
} sShortAsBytes;

union {
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
    if (SDLNet_Init() < 0) {
        fprintf(stderr, "[Error] SDLNet_Init: %s\n", SDLNet_GetError());
        return false;
    }

    atexit(SDLNet_Quit);

    ServerAddress localhost;
    localhost.hostname = "localhost";
    localhost.port = 2000;
    netplay.savedServers.push_back(localhost);

    printf("Network system initialized.\n");
}

void net_close()
{
	SDLNet_Quit();
}

/********************************************************************
 * NetClient
 ********************************************************************/
NetClient::NetClient()
{}

NetClient::~NetClient()
{
	endSession();
}

bool NetClient::connect(const char* hostname, const uint16_t port)
{
    /* Resolve server address */
    printf("Connecting to %s:%d...\n", hostname, port);
    if (SDLNet_ResolveHost(&serverIP, hostname, port) < 0) {
        if (serverIP.host == INADDR_NONE)
            fprintf(stderr, "[Error] Couldn't resolve hostname\n");
        else
            fprintf(stderr, "[Error] SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return false;
    }

    /* Open TCP socket */
    tcpSocket = SDLNet_TCP_Open(&serverIP);
    if (!tcpSocket) {
        fprintf(stderr, "[Error] SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return false;
    }
    SDLNet_TCP_AddSocket(sockets, tcpSocket);

    return true;
}

void NetClient::update()
{
	SDLNet_CheckSockets(sockets, 0);

    // TCP műveletek
    if (SDLNet_SocketReady(tcpSocket))
    {
        printf("READY\n");
        uint8_t response[128];
        if (!receiveTCPMessage(tcpSocket, response, 128))
            SDLNet_TCP_Close(tcpSocket);
        else
        {
            switch (response[0])
            {
                case NET_RESPONSE_SERVERINFO:
                    ServerInfoPackage serverInfo;
                    memcpy(&serverInfo, response, sizeof(ServerInfoPackage));


                    printf("NET_RESPONSE_SERVERINFO [%lu byte]\n", sizeof(serverInfo));
                    printf("{\n  name: %s\n  desc: %s\n  currentPlayers: %d\n  maxPlayers: %d\n}\n",
                        serverInfo.name, serverInfo.description, serverInfo.currentPlayers, serverInfo.maxPlayers);

                    SDLNet_TCP_Close(tcpSocket);
                    break;

                default:
                    SDLNet_TCP_Close(tcpSocket);
                    break;
            }
        }
    }

    // UDP műveletek
    /*if (SDLNet_SocketReady(client.udpSocket)) {
        printf("UDP-t kaptam!\n");
    }*/

}

void NetClient::cleanup()
{
	if (tcpSocket) {
        SDLNet_TCP_Close(tcpSocket);
        tcpSocket = NULL;
    }
    if (udpSocket) {
        SDLNet_UDP_Close(udpSocket);
        udpSocket = NULL;
    }

    if (sockets) {
        SDLNet_FreeSocketSet(sockets);
        sockets = NULL;
    }
}

bool NetClient::startSession()
{
	// Finish previous network session
	endSession();

	netplay.active = true;
    sockets = SDLNet_AllocSocketSet(2);
    if (!sockets) {
        fprintf(stderr, "[Error] SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

void NetClient::endSession()
{
	netplay.active = false;
	cleanup();
}
