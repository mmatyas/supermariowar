#include "global.h"
#include <string.h>

#include "net.h"

#ifdef _WIN32
    #pragma comment(lib, "SDL_net.lib")
#endif

extern int g_iVersion[];
extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

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
    localhost.port = NET_DEFAULT_PORT;
    netplay.recentServers.push_back(localhost);

    printf("Network system initialized.\n");
}

void net_close()
{
    netplay.client.endSession();
    net_saveRecentServers();
    SDLNet_Quit();
}

void net_saveRecentServers()
{
    FILE * fp = OpenFile("servers.bin", "wb");
    if(fp) {
        fwrite(g_iVersion, sizeof(int), 4, fp);

        for (unsigned iServer = 0; iServer < netplay.recentServers.size(); iServer++)
        {
            ServerAddress* host = &netplay.recentServers[iServer];

            WriteString(host->hostname.c_str(), fp);
            fwrite(&host->port, sizeof(uint16_t), 1, fp);
        }
    }
    fclose(fp);
}

void net_loadRecentServers()
{
    FILE * fp = OpenFile("servers.bin", "rb");
    if(fp) {
        int version[4];
        fread(version, sizeof(int), 4, fp);

        if(VersionIsEqual(g_iVersion, version[0], version[1], version[2], version[3])) {
            char buffer[128];
            uint16_t port;

            while (!feof(fp) && !ferror(fp)) {
                ReadString(buffer, 128, fp);
                fread(&port, sizeof(uint16_t), 1, fp);

                ServerAddress host;
                host.hostname = buffer;
                host.port = port;
            }
        }
    }
    fclose(fp);
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
    readySockets = SDLNet_CheckSockets(sockets, 0);

    // TCP műveletek
    if (readySockets && SDLNet_SocketReady(tcpSocket))
    {
        printf("READY %d\n", readySockets);
        uint8_t response[NET_MAX_MESSAGE_SIZE];
        if (!receiveTCPMessage(tcpSocket, response, NET_MAX_MESSAGE_SIZE))
            closeTCPsocket();
        else {
            uint8_t protocollVersion = response[0];
            uint8_t responseCode = response[1];
            switch (responseCode)
            {
                case NET_RESPONSE_SERVERINFO:
                    ServerInfoPackage serverInfo;
                    memcpy(&serverInfo, response, sizeof(ServerInfoPackage));


                    printf("NET_RESPONSE_SERVERINFO [%lu byte]\n", sizeof(serverInfo));
                    printf("Sending:\n  protocolVersion: %d\n  packageType: %d\n  name: %s\n  players/max: %d / %d\n",
                        serverInfo.protocolVersion, serverInfo.packageType, serverInfo.name, serverInfo.currentPlayers, serverInfo.maxPlayers);

                    closeTCPsocket();
                    break;

                default:
                    closeTCPsocket();
                    break;
            }
        }
    }

    // UDP műveletek
    /*if (SDLNet_SocketReady(client.udpSocket)) {
        printf("UDP-t kaptam!\n");
    }*/

}

bool NetClient::startSession()
{
    printf("Session start.\n");
    // Finish previous network session
    endSession();

    netplay.active = true;
    sockets = SDLNet_AllocSocketSet(2);
    if (!sockets) {
        fprintf(stderr, "[Error] SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return false;
    }

    if (connect(netplay.recentServers[0].hostname.c_str(), netplay.recentServers[0].port)) {
        MessageHeader message;
        message.protocolVersion = NET_PROTOCOL_VERSION;
        message.packageType = NET_REQUEST_SERVERINFO;
        sendTCPMessage(tcpSocket, &message, sizeof(MessageHeader));
    }

    return true;
}

void NetClient::endSession()
{
    if (netplay.active) {
        printf("Session end.\n");
        netplay.active = false;
        cleanup();
    }
}

void NetClient::cleanup()
{
    closeTCPsocket();
    closeUDPsocket();

    if (sockets) {
        SDLNet_FreeSocketSet(sockets);
        sockets = NULL;
    }
}

void NetClient::closeTCPsocket()
{
    if (tcpSocket) {
        SDLNet_TCP_DelSocket(sockets, tcpSocket);
        SDLNet_TCP_Close(tcpSocket);
        tcpSocket = NULL;
    }
}

void NetClient::closeUDPsocket()
{
    if (udpSocket) {
        SDLNet_UDP_DelSocket(sockets, udpSocket);
        SDLNet_UDP_Close(udpSocket);
        udpSocket = NULL;
    }
}
