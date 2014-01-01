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

    if (!netplay.client.init())
        return false;

    atexit(SDLNet_Quit);

    netplay.connectSuccessful = false;

    /*ServerAddress none;
    none.hostname = "(none)";
    netplay.savedServers.push_back(none);*/

    ServerAddress localhost;
    localhost.hostname = "localhost";
    netplay.savedServers.push_back(localhost);

    net_loadServerList();

    printf("Network system initialized.\n");
}

void net_close()
{
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
            ReadString(buffer, 128, fp);

            while (!feof(fp) && !ferror(fp)) {
                ServerAddress host;
                host.hostname = buffer;
                netplay.savedServers.push_back(host);

                ReadString(buffer, 128, fp);
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
    cleanup();
}

bool NetClient::init()
{
    udpOutgoingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    udpIncomingPacket = SDLNet_AllocPacket(NET_MAX_MESSAGE_SIZE);
    if (!udpOutgoingPacket || !udpIncomingPacket) {
        fprintf(stderr, "[Error] SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool NetClient::startSession()
{
    printf("Session start.\n");
    endSession(); // Finish previous network session if active

    netplay.active = true;
    netplay.connectSuccessful = false;

    return true;
}

bool NetClient::sendConnectRequestToSelectedServer()
{
    ServerAddress* selectedServer = &netplay.savedServers[netplay.selectedServerIndex];
    if ( connect(selectedServer->hostname.c_str()) )
    {
        ClientConnectionPackage message;
        message.protocolVersion = NET_PROTOCOL_VERSION;
        message.packageType = NET_REQUEST_CONNECT;
        memcpy(message.playerName, netplay.playername, NET_MAX_PLAYER_NAME_LENGTH);
        message.playerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

        sendUDPMessage(&message, sizeof(ClientConnectionPackage));
        return true;
    }
    return false;
}

bool NetClient::connect(const char* hostname, const uint16_t port)
{
    /* Resolve server address */
    netplay.connectSuccessful = false;
    printf("Connecting to %s:%d...\n", hostname, port);
    if (SDLNet_ResolveHost(&serverIP, hostname, port) < 0) {
        if (serverIP.host == INADDR_NONE)
            fprintf(stderr, "[Error] Couldn't resolve hostname\n");
        else
            fprintf(stderr, "[Error] SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return false;
    }

    /* Open UDP socket */
    udpSocket = SDLNet_UDP_Open(0);
    if (!udpSocket) {
        fprintf(stderr, "[Error] SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return false;
    }
    printf("[] UDP open.\n");

    return true;
}

void NetClient::requestRoomList()
{
    MessageHeader msg;
    msg.protocolVersion = NET_PROTOCOL_VERSION;
    msg.packageType = NET_REQUEST_ROOM_LIST;

    sendUDPMessage(&msg, sizeof(MessageHeader));
    printf("Req. room list.\n");
}

void NetClient::update()
{
    if (receiveUDPMessage()) {
        printf("READY\n");

        uint8_t protocollVersion = udpIncomingPacket->data[0];
        uint8_t responseCode = udpIncomingPacket->data[1];
        if (protocollVersion == NET_PROTOCOL_VERSION) {
            switch (responseCode)
            {
                case NET_RESPONSE_SERVERINFO:
                    ServerInfoPackage serverInfo;
                    memcpy(&serverInfo, udpIncomingPacket->data, sizeof(ServerInfoPackage));


                    printf("NET_RESPONSE_SERVERINFO [%lu byte]\n", sizeof(serverInfo));
                    printf("Sending:\n  protocolVersion: %d\n  packageType: %d\n  name: %s\n  players/max: %d / %d\n",
                        serverInfo.protocolVersion, serverInfo.packageType, serverInfo.name, serverInfo.currentPlayers, serverInfo.maxPlayers);

                    closeUDPsocket();
                    break;

                case NET_RESPONSE_CONNECT_OK:
                    printf("Connection attempt successful.\n");
                    netplay.connectSuccessful = true;
                    requestRoomList();
                    break;

                case NET_RESPONSE_ROOM_LIST_ENTRY: {
                    RoomInfoPackage roomInfo;
                    memcpy(&roomInfo, udpIncomingPacket->data, sizeof(RoomInfoPackage));
                    printf("Room entry: %s (%d/4)\n", roomInfo.name, roomInfo.playerCount);

                    RoomEntry newRoom;
                    newRoom.name = roomInfo.name;
                    newRoom.playerCount = roomInfo.playerCount;
                    netplay.currentRooms.push_back(newRoom);
                    }
                    break;

                case NET_RESPONSE_NO_ROOMS:
                    printf("There are no rooms currently on the server.\n");
                    break;

                default:
                    printf("Unknown:\n");
                    for (int a = 0; a < udpIncomingPacket->len; a++)
                        printf("%3d ", udpIncomingPacket->data[a]);
                    break;
            }

            netplay.lastReceivedMessage.packageType = responseCode;
            netplay.lastReceivedMessage.timestamp = SDL_GetTicks();
        }
    }
}

void NetClient::endSession()
{
    if (netplay.active) {
        printf("Session end.\n");
        sendGoodbye();
        closeUDPsocket();

        netplay.active = false;
        netplay.connectSuccessful = false;
    }
}

void NetClient::sendGoodbye()
{
    MessageHeader msg;
    msg.protocolVersion = NET_PROTOCOL_VERSION;
    msg.packageType = NET_REQUEST_LEAVE_SERVER;

    sendUDPMessage(&msg, sizeof(MessageHeader));
}

void NetClient::cleanup()
{
    endSession();

    if (udpIncomingPacket) {
        SDLNet_FreePacket(udpIncomingPacket);
        udpIncomingPacket = NULL;
    }

    if (udpOutgoingPacket) {
        SDLNet_FreePacket(udpOutgoingPacket);
        udpOutgoingPacket = NULL;
    }
}

void NetClient::closeUDPsocket()
{
    if (udpSocket) {
        SDLNet_UDP_Close(udpSocket);
        udpSocket = NULL;
        printf("[] UDP closed.\n");
    }
}

/****************************
    Network Communication
****************************/

bool NetClient::sendUDPMessage(const void* data, const int dataLength)
{
    if (!data || dataLength < 2 || dataLength >= NET_MAX_MESSAGE_SIZE || !udpSocket || !udpOutgoingPacket) {
        printf("[Debug] Invalid call: sendUDPMessage\n");
        return false;
    }

    memcpy(udpOutgoingPacket->data, data, dataLength);
    udpOutgoingPacket->len = dataLength;
    udpOutgoingPacket->address.host = serverIP.host;
    udpOutgoingPacket->address.port = serverIP.port;

    if (!SDLNet_UDP_Send(udpSocket, -1, udpOutgoingPacket)) {
        fprintf(stderr, "[Warning] Sending UDP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    netplay.lastSentMessage.packageType = ((uint8_t*)data)[1];
    netplay.lastSentMessage.timestamp = SDL_GetTicks();

    return true;
}

bool NetClient::receiveUDPMessage()
{
    if (!udpSocket || !udpIncomingPacket)
        return false;

    int receivedPackages = SDLNet_UDP_Recv(udpSocket, udpIncomingPacket);
    if (receivedPackages == 0)
        return false;

    if (receivedPackages < 0) {
        fprintf(stderr, "[Warning] Receiving UDP message failed. %s\n", SDLNet_GetError());
        return false;
    }

    // Must have version and type field.
    if (udpIncomingPacket->len < 2)
        return false;

    netplay.lastReceivedMessage.packageType = udpIncomingPacket->data[1];
    netplay.lastReceivedMessage.timestamp = SDL_GetTicks();

    return true;
}
