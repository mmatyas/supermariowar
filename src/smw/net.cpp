#include "global.h"
#include <string.h>

#include "net.h"

#ifdef _WIN32
    #pragma comment(lib, "SDL_net.lib")
#endif

extern int g_iVersion[];
extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

Networking netplay;


bool net_init()
{
    if (SDLNet_Init() < 0) {
        fprintf(stderr, "[Error] SDLNet_Init: %s\n", SDLNet_GetError());
        return false;
    }

    atexit(SDLNet_Quit);

    ServerAddress none;
    none.hostname = "(none)";
    netplay.savedServers.push_back(none);

    /*ServerAddress localhost;
    localhost.hostname = "localhost";
    netplay.savedServers.push_back(localhost);*/

    net_loadServerList();

    printf("Network system initialized.\n");
}

void net_close()
{
    netplay.client.endSession();
    net_saveServerList();
    SDLNet_Quit();
}

void net_saveServerList()
{
    FILE * fp = OpenFile("servers.bin", "wb");
    if(fp) {
        fwrite(g_iVersion, sizeof(int), 4, fp);

        // Don't save "(none)"
        for (unsigned iServer = 1; iServer < netplay.savedServers.size(); iServer++) {
            ServerAddress* host = &netplay.savedServers[iServer];
            WriteString(host->hostname.c_str(), fp);
        }
        fclose(fp);
    }
}

void net_loadServerList()
{
    FILE * fp = OpenFile("servers.bin", "rb");
    if(fp) {
        int version[4];
        fread(version, sizeof(int), 4, fp);

        if(VersionIsEqual(g_iVersion, version[0], version[1], version[2], version[3])) {
            char buffer[128];

            while (!feof(fp) && !ferror(fp)) {
                ReadString(buffer, 128, fp);

                ServerAddress host;
                host.hostname = buffer;
                netplay.savedServers.push_back(host);
            }
        }
        fclose(fp);
    }
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

void NetClient::update()
{
    readySockets = SDLNet_CheckSockets(sockets, 0);

    // TCP műveletek
    if (readySockets && SDLNet_SocketReady(tcpSocket))
    {
        printf("READY %d\n", readySockets);
        if (!receiveTCPMessage())
            closeTCPsocket();
        else {
            uint8_t protocollVersion = tcpResponseBuffer[0];
            uint8_t responseCode = tcpResponseBuffer[1];
            switch (responseCode)
            {
                case NET_RESPONSE_SERVERINFO:
                    ServerInfoPackage serverInfo;
                    memcpy(&serverInfo, tcpResponseBuffer, sizeof(ServerInfoPackage));


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
    /*if (readySockets && SDLNet_SocketReady(udpSocket)) {
        if (!receiveUDPMessage())
            closeUDPsocket();
        else {
            printf("  chan %d\n", udpIncomingPacket->channel);
            printf("  len %d\n", udpIncomingPacket->len);
            printf("  maxlen %d\n", udpIncomingPacket->maxlen);
            printf("  status %d\n", udpIncomingPacket->status);

            uint8_t* addr = (Uint8*)&udpIncomingPacket->address;
            printf("  source %d.%d.%d.%d::%d\n", addr[0], addr[1], addr[2], addr[3], udpIncomingPacket->address.port);

            for (int i = 0; i < udpIncomingPacket->len; i++)
                printf("%3d ", udpIncomingPacket->data[i]);

            printf("\n");
            closeUDPsocket();
        }
    }*/

}

bool NetClient::startSession()
{
    printf("Session start.\n");
    endSession(); // Finish previous network session if active

    /*udpIncomingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    if(!udpIncomingPacket) {
        fprintf(stderr, "[Error] SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return false;
    }

    udpOutgoingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    if(!udpOutgoingPacket) {
        fprintf(stderr, "[Error] SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return false;
    }*/

    netplay.active = true;
    sockets = SDLNet_AllocSocketSet(2);
    if (!sockets) {
        fprintf(stderr, "[Error] SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return false;
    }

    if ( connect(netplay.savedServers[0].hostname.c_str()) ) {
        MessageHeader message;
        message.protocolVersion = NET_PROTOCOL_VERSION;
        message.packageType = NET_REQUEST_SERVERINFO;
        sendTCPMessage(&message, sizeof(MessageHeader));
    }

    return true;
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

    /* Open UDP socket */
    /*udpSocket = SDLNet_UDP_Open(port);
    if (!udpSocket) {
        fprintf(stderr, "[Error] SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return false;
    }
    udpChannel = SDLNet_UDP_Bind(udpSocket, -1, &serverIP);
    if (udpChannel < 0) {
        fprintf(stderr, "[Error] SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
        return false;
    }
    SDLNet_UDP_AddSocket(sockets, udpSocket);*/

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


        if (udpIncomingPacket) {
            SDLNet_FreePacket(udpIncomingPacket);
            udpIncomingPacket = NULL;
        }

        if (udpOutgoingPacket) {
            SDLNet_FreePacket(udpOutgoingPacket); // CRASH
            udpOutgoingPacket = NULL;
        }

        if (udpChannel)
            SDLNet_UDP_Unbind(udpSocket, udpChannel);
        SDLNet_UDP_Close(udpSocket);
        udpSocket = NULL;
    }
}

bool NetClient::sendTCPMessage(void* data, int dataLength) // int a Send miatt
{
    if (!data || dataLength <= 0 || dataLength >= NET_MAX_MESSAGE_SIZE || !tcpSocket)
        return false;

    if (SDLNet_TCP_Send(tcpSocket, data, dataLength) < dataLength) {
        fprintf(stderr, "[Warning] Sending TCP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetClient::receiveTCPMessage()
{
    if (!tcpSocket)
        return false;

    if (SDLNet_TCP_Recv(tcpSocket, tcpResponseBuffer, NET_MAX_MESSAGE_SIZE) <= 0) {
        fprintf(stderr, "[Warning] Receiving TCP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetClient::sendUDPMessage(void* data, int dataLength)
{
    if (!data || dataLength <= 0 || dataLength >= NET_MAX_MESSAGE_SIZE || !udpSocket || !udpOutgoingPacket)
        return false;

    udpOutgoingPacket->data = (Uint8*)data;
    udpOutgoingPacket->len = dataLength;

    if (!SDLNet_UDP_Send(udpSocket, udpChannel, udpOutgoingPacket)) {
        fprintf(stderr, "[Warning] Sending UDP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetClient::receiveUDPMessage()
{
    if (!udpSocket || !udpIncomingPacket)
        return false;

    if (SDLNet_UDP_Recv(udpSocket, udpIncomingPacket) <= 0) {
        fprintf(stderr, "[Warning] Receiving UDP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}
