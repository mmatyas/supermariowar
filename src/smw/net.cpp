#include "global.h"
#include <string.h>

#include "net.h"

#ifdef _WIN32
    #pragma comment(lib, "SDL_net.lib")
#endif

extern gv game_values;
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
    netplay.joinSuccessful = false;
    netplay.gameRunning = false;

    /*ServerAddress none;
    none.hostname = "(none)";
    netplay.savedServers.push_back(none);*/

    ServerAddress localhost;
    localhost.hostname = "localhost";
    netplay.savedServers.push_back(localhost);

    /*ServerAddress rpi;
    rpi.hostname = "192.168.1.103";
    netplay.savedServers.push_back(rpi);*/

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
    if (fp) {
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

void NetClient::setRoomListUIControl(MI_NetworkListScroll* control)
{
    uiRoomList = control;
}

/****************************
    Building up
****************************/

bool NetClient::startSession()
{
    printf("Session start.\n");
    endSession(); // Finish previous network session if active

    netplay.active = true;
    netplay.connectSuccessful = false;

    // backup singleplayer settings
    for (uint8_t p; p < 4; p++)
        backup_playercontrol[p] = game_values.playercontrol[p];

    return true;
}

bool NetClient::openSocket(const char* hostname, const uint16_t port)
{
    /* Resolve server address */
    netplay.connectSuccessful = false;
    printf("Resolving %s:%d\n", hostname, port);
    if (SDLNet_ResolveHost(&serverIP, hostname, port) < 0) {
        if (serverIP.host == INADDR_NONE)
            fprintf(stderr, "[Error] Couldn't resolve hostname\n");
        else
            fprintf(stderr, "[Error] SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return false;
    }

    /* Open UDP socket */
    if (!udpSocket) {
        udpSocket = SDLNet_UDP_Open(0);
        if (!udpSocket) {
            fprintf(stderr, "[Error] SDLNet_UDP_Open: %s\n", SDLNet_GetError());
            return false;
        }
        printf("[] UDP open.\n");
    }

    return true;
}

/****************************
    Outgoing messages
****************************/

void NetClient::requestRoomList()
{
    MessageHeader msg;
    msg.protocolVersion = NET_PROTOCOL_VERSION;
    msg.packageType = NET_REQUEST_ROOM_LIST;

    sendUDPMessage(&msg, sizeof(MessageHeader));

    if (uiRoomList)
        uiRoomList->Clear();
}

bool NetClient::sendConnectRequestToSelectedServer()
{
    ServerAddress* selectedServer = &netplay.savedServers[netplay.selectedServerIndex];
    if ( openSocket(selectedServer->hostname.c_str()) )
    {
        ClientConnectionPackage message;
        message.protocolVersion = NET_PROTOCOL_VERSION;
        message.packageType = NET_REQUEST_CONNECT;
        memcpy(message.playerName, netplay.playername, NET_MAX_PLAYER_NAME_LENGTH);
        message.playerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

        sendUDPMessage(&message, sizeof(ClientConnectionPackage));
        netplay.operationInProgress = true;
        return true;
    }
    return false;
}

void NetClient::sendCreateRoomMessage()
{
    NewRoomPackage message;
    message.protocolVersion = NET_PROTOCOL_VERSION;
    message.packageType = NET_REQUEST_CREATE_ROOM;

    memcpy(message.name, netplay.newroom_name, NET_MAX_ROOM_NAME_LENGTH);
    message.name[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

    memcpy(message.password, netplay.newroom_password, NET_MAX_ROOM_PASSWORD_LENGTH);
    message.password[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

    sendUDPMessage(&message, sizeof(NewRoomPackage));
    netplay.operationInProgress = true;
}

void NetClient::sendJoinRoomMessage()
{
    if (netplay.selectedRoomIndex >= netplay.currentRooms.size())
        return;

    JoinRoomPackage message;
    message.protocolVersion = NET_PROTOCOL_VERSION;
    message.packageType = NET_REQUEST_JOIN_ROOM;

    message.roomID = netplay.currentRooms.at(netplay.selectedRoomIndex).roomID;
    message.password[0] = '\0'; // TODO: implement

    sendUDPMessage(&message, sizeof(JoinRoomPackage));
    netplay.operationInProgress = true;
}

void NetClient::sendLeaveRoomMessage()
{
    MessageHeader message;
    message.protocolVersion = NET_PROTOCOL_VERSION;
    message.packageType = NET_REQUEST_LEAVE_ROOM;

    sendUDPMessage(&message, sizeof(MessageHeader));
}

void NetClient::sendStartRoomMessage()
{
    MessageHeader message;
    message.protocolVersion = NET_PROTOCOL_VERSION;
    message.packageType = NET_REQUEST_START_GAME;

    sendUDPMessage(&message, sizeof(MessageHeader));
}

void NetClient::sendSynchOKMessage()
{
    MessageHeader message;
    message.protocolVersion = NET_PROTOCOL_VERSION;
    message.packageType = NET_NOTICE_GAME_SYNCH_OK;

    sendUDPMessage(&message, sizeof(MessageHeader));
}

void NetClient::sendLocalInput()
{
    LocalKeysPackage pkg;
    pkg.protocolVersion = NET_PROTOCOL_VERSION;
    pkg.packageType = NET_NOTICE_LOCAL_KEYS;

    memcpy(&pkg.keys, &game_values.playerInput.outputControls[0].keys, 8 * sizeof(CKeyState));

    sendUDPMessage(&pkg, sizeof(LocalKeysPackage));
}

/****************************
    Incoming messages
****************************/

void NetClient::handleServerinfoAndClose()
{
    ServerInfoPackage serverInfo;
    memcpy(&serverInfo, udpIncomingPacket->data, sizeof(ServerInfoPackage));

    printf("NET_RESPONSE_SERVERINFO [%lu byte]\n", sizeof(serverInfo));
    printf("Sending:\n  protocolVersion: %d\n  packageType: %d\n  name: %s\n  players/max: %d / %d\n",
        serverInfo.protocolVersion, serverInfo.packageType, serverInfo.name, serverInfo.currentPlayerCount, serverInfo.maxPlayerCount);

    closeUDPsocket();
}

void NetClient::handleNewRoomListEntry()
{
    RoomInfoPackage roomInfo;
    memcpy(&roomInfo, udpIncomingPacket->data, sizeof(RoomInfoPackage));
    printf("  Incoming room entry: [%u] %s (%d/4)\n", roomInfo.roomID, roomInfo.name, roomInfo.currentPlayerCount);

    RoomListEntry newRoom;
    newRoom.roomID = roomInfo.roomID;
    newRoom.name = roomInfo.name;
    newRoom.playerCount = roomInfo.currentPlayerCount;
    netplay.currentRooms.push_back(newRoom);

    if (uiRoomList) {
        char playerCountString[4] = {'0' + roomInfo.currentPlayerCount, '/', '4', '\0'};
        uiRoomList->Add(newRoom.name, playerCountString);
    }
}

void NetClient::handleRoomCreatedMessage()
{
    NewRoomCreatedPackage pkg;
    memcpy(&pkg, udpIncomingPacket->data, sizeof(NewRoomCreatedPackage));

    netplay.currentRoom.roomID = pkg.roomID;
    memcpy(netplay.currentRoom.name, netplay.newroom_name, NET_MAX_ROOM_NAME_LENGTH);
    memcpy(netplay.currentRoom.playerNames[0], netplay.playername, NET_MAX_PLAYER_NAME_LENGTH);
    memcpy(netplay.currentRoom.playerNames[1], "(empty)", NET_MAX_PLAYER_NAME_LENGTH);
    memcpy(netplay.currentRoom.playerNames[2], "(empty)", NET_MAX_PLAYER_NAME_LENGTH);
    memcpy(netplay.currentRoom.playerNames[3], "(empty)", NET_MAX_PLAYER_NAME_LENGTH);

    printf("Room created, ID: %u, %d\n", pkg.roomID, pkg.roomID);
    netplay.currentMenuChanged = true;
    netplay.joinSuccessful = true;

    game_values.playercontrol[0] = 1;
    game_values.playercontrol[1] = 0;
    game_values.playercontrol[2] = 0;
    game_values.playercontrol[3] = 0;
}

void NetClient::handleRoomChangedMessage()
{
    CurrentRoomPackage pkg;
    memcpy(&pkg, udpIncomingPacket->data, sizeof(CurrentRoomPackage));

    netplay.currentRoom.roomID = pkg.roomID;
    memcpy(netplay.currentRoom.name, pkg.name, NET_MAX_ROOM_NAME_LENGTH);

    printf("Room %u (%s) changed:\n", pkg.roomID, pkg.name);
    for (uint8_t p = 0; p < 4; p++) {
        memcpy(netplay.currentRoom.playerNames[p], pkg.playerName[p], NET_MAX_PLAYER_NAME_LENGTH);
        printf("  player %d: %s\n", p+1, netplay.currentRoom.playerNames[p]);


        if (strcmp(netplay.currentRoom.playerNames[p], "(empty)") == 0)
            game_values.playercontrol[p] = 0;
        //else if (strcmp(netplay.currentRoom.playerNames[p], "(bot)") == 0)
        //    game_values.playercontrol[p] = 2;
        //    game_values.playercontrol[p] = 1;
        else
            game_values.playercontrol[p] = 1; // valid player
    }
    remotePlayerNumber = pkg.remotePlayerNumber;

    netplay.currentMenuChanged = true;
}

void NetClient::handleRemoteInput()
{
    RemoteKeysPackage pkg;
    memcpy(&pkg, udpIncomingPacket->data, sizeof(RemoteKeysPackage));

    COutputControl* playerControl;
    if (pkg.playerNumber != 0) {// local player is at 0
        playerControl = &game_values.playerInput.outputControls[pkg.playerNumber];
        printf("normal input\n");
    }
    else
        playerControl = &game_values.playerInput.outputControls[remotePlayerNumber];

    memcpy(playerControl->keys, pkg.keys, 8 * sizeof(CKeyState));
    printf("most?\n");
}

void NetClient::listen()
{
    if (receiveUDPMessage()) {
        uint8_t protocollVersion = udpIncomingPacket->data[0];
        uint8_t responseCode = udpIncomingPacket->data[1];
        if (protocollVersion == NET_PROTOCOL_VERSION) {
            switch (responseCode)
            {
                //
                // Query
                //
                case NET_RESPONSE_BADPROTOCOL:
                    printf("Not implemented: NET_RESPONSE_BADPROTOCOL\n");
                    break;

                case NET_RESPONSE_SERVERINFO:
                    handleServerinfoAndClose();
                    break;

                case NET_RESPONSE_SERVER_MOTD:
                    printf("Not implemented: NET_RESPONSE_SERVER_MOTD\n");
                    break;

                //
                // Connect
                //
                case NET_RESPONSE_CONNECT_OK:
                    printf("Connection attempt successful.\n");
                    netplay.connectSuccessful = true;
                    break;

                case NET_RESPONSE_CONNECT_DENIED:
                    printf("NET_RESPONSE_CONNECT_DENIED\n");
                    netplay.connectSuccessful = false;
                    break;

                case NET_RESPONSE_CONNECT_SERVERFULL:
                    printf("NET_RESPONSE_CONNECT_SERVERFULL\n");
                    netplay.connectSuccessful = false;
                    break;

                case NET_RESPONSE_CONNECT_NAMETAKEN:
                    printf("NET_RESPONSE_CONNECT_NAMETAKEN\n");
                    netplay.connectSuccessful = false;
                    break;

                //
                // Rooms
                //
                case NET_RESPONSE_ROOM_LIST_ENTRY:
                    handleNewRoomListEntry();
                    break;

                case NET_RESPONSE_NO_ROOMS:
                    printf("There are no rooms currently on the server.\n");
                    break;

                //
                // Join
                //
                case NET_RESPONSE_JOIN_OK:
                    printf("NET_RESPONSE_JOIN_OK\n");
                    netplay.joinSuccessful = true;
                    break;

                case NET_RESPONSE_ROOM_FULL:
                    printf("NET_RESPONSE_ROOMFULL\n");
                    netplay.joinSuccessful = false;
                    break;

                case NET_NOTICE_ROOM_CHANGED:
                    handleRoomChangedMessage();
                    break;

                //
                // Create
                //
                case NET_RESPONSE_CREATE_OK:
                    handleRoomCreatedMessage();
                    break;

                case NET_RESPONSE_CREATE_ERROR:
                    printf("Not implemented: NET_RESPONSE_CREATE_ERROR\n");
                    break;

                //
                // Game
                //

                case NET_NOTICE_GAME_SYNCH:
                    printf("NET_NOTICE_GAME_SYNCH\n");
                    sendSynchOKMessage();
                    break;

                case NET_NOTICE_GAME_STARTED:
                    printf("NET_NOTICE_GAME_STARTED\n");
                    netplay.gameRunning = true;
                    break;

                case NET_NOTICE_REMOTE_KEYS:
                    printf("NET_NOTICE_REMOTE_KEYS\n");
                    handleRemoteInput();
                    break;

                //
                // Default
                //

                default:
                    printf("Unknown: ");
                    for (int a = 0; a < udpIncomingPacket->len; a++)
                        printf("%3d ", udpIncomingPacket->data[a]);
                    printf("\n");
                    break;
            }

            netplay.lastReceivedMessage.packageType = responseCode;
            netplay.lastReceivedMessage.timestamp = SDL_GetTicks();
        }
    }
}

/****************************
    Building down
****************************/

void NetClient::endSession()
{
    if (netplay.active) {
        printf("Session end.\n");
        if (netplay.connectSuccessful)
            sendGoodbye();

        closeUDPsocket();
        netplay.active = false;
        netplay.connectSuccessful = false;

        // restore singleplayer settings
        for (uint8_t p; p < 4; p++)
            game_values.playercontrol[p] = backup_playercontrol[p];
    }
}

void NetClient::sendGoodbye()
{
    MessageHeader msg;
    msg.protocolVersion = NET_PROTOCOL_VERSION;
    msg.packageType = NET_REQUEST_LEAVE_SERVER;

    printf("sendGoodbye\n");
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
