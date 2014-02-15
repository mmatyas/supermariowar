#include "global.h"
#include <string.h>

#include "net.h"

#ifdef _WIN32
    #pragma comment(lib, "SDL_net.lib")
#endif


extern gv game_values;
extern int g_iVersion[];
extern CPlayer *list_players[4];
extern short list_players_cnt;
extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

Networking netplay;


bool net_init()
{
    if (!networkHandler.init_networking())
        return false;

    if (!netplay.client.init())
        return false;


    netplay.active = false;
    netplay.connectSuccessful = false;
    netplay.joinSuccessful = false;
    netplay.gameRunning = false;

    strcpy(netplay.myPlayerName, "Player");
    netplay.currentMenuChanged = false;
    netplay.theHostIsMe = false;
    netplay.selectedRoomIndex = 0;
    netplay.selectedServerIndex = 0;
    netplay.roomFilter[0] = '\0';
    netplay.newroom_name[0] = '\0';
    netplay.newroom_password[0] = '\0';
    netplay.mychatmessage[0] = '\0';

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
    networkHandler.cleanup();
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
    if (!networkHandler.init_client())
        return false;

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

bool NetClient::openConnection(const char* hostname, const uint16_t port)
{
    netplay.connectSuccessful = false;
    if (!networkHandler.openUDPConnection())
        return false;

    return true;
    // now we wait for CONNECT_OK
    // connectSuccessful will be set to 'true' there
}

/****************************
    Outgoing messages
****************************/

void NetClient::requestRoomList()
{
    MessageHeader msg;
    msg.protocolVersion = NET_PROTOCOL_VERSION;
    msg.packageType = NET_REQUEST_ROOM_LIST;

    sendMessage(&msg, sizeof(MessageHeader));

    if (uiRoomList)
        uiRoomList->Clear();
}

bool NetClient::sendConnectRequestToSelectedServer()
{
    ServerAddress* selectedServer = &netplay.savedServers[netplay.selectedServerIndex];
    if (openConnection(selectedServer->hostname.c_str()))
    {
        ClientConnectionPackage message;
        message.protocolVersion = NET_PROTOCOL_VERSION;
        message.packageType = NET_REQUEST_CONNECT;
        memcpy(message.playerName, netplay.myPlayerName, NET_MAX_PLAYER_NAME_LENGTH);
        message.playerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';

        sendMessage(&message, sizeof(ClientConnectionPackage));
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

    sendMessage(&message, sizeof(NewRoomPackage));
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

    sendMessage(&message, sizeof(JoinRoomPackage));
    netplay.operationInProgress = true;
}

void NetClient::sendLeaveRoomMessage()
{
    MessageHeader message;
    message.protocolVersion = NET_PROTOCOL_VERSION;
    message.packageType = NET_REQUEST_LEAVE_ROOM;

    sendMessage(&message, sizeof(MessageHeader));
}

void NetClient::sendStartRoomMessage()
{
    MessageHeader message;
    message.protocolVersion = NET_PROTOCOL_VERSION;
    message.packageType = NET_REQUEST_START_GAME;

    sendMessage(&message, sizeof(MessageHeader));
}

void NetClient::sendSynchOKMessage()
{
    MessageHeader message;
    message.protocolVersion = NET_PROTOCOL_VERSION;
    message.packageType = NET_NOTICE_GAME_SYNCH_OK;

    sendMessage(&message, sizeof(MessageHeader));
}

void NetClient::sendLocalInput()
{
    InputPackage pkg;
    pkg.protocolVersion = NET_PROTOCOL_VERSION;
    pkg.packageType = NET_NOTICE_LOCAL_KEYS;

    COutputControl* playerControl = &netplay.netPlayerInput.outputControls[0];

    uint16_t bitPosition = 0;
    pkg.input = 0;
    for (uint8_t k = 0; k < 8; k++) {
        pkg.input |= ( (playerControl->keys[k].fDown ? 1 : 0) << bitPosition);
        bitPosition++;
        pkg.input |= ( (playerControl->keys[k].fPressed ? 1 : 0) << bitPosition);
        bitPosition++;
    }

    /*pkg.input.key0_down = playerControl->keys[0].fDown;
    pkg.input.key0_pressed = playerControl->keys[0].fPressed;
    pkg.input.key1_down = playerControl->keys[1].fDown;
    pkg.input.key1_pressed = playerControl->keys[1].fPressed;
    pkg.input.key2_down = playerControl->keys[2].fDown;
    pkg.input.key2_pressed = playerControl->keys[2].fPressed;
    pkg.input.key3_down = playerControl->keys[3].fDown;
    pkg.input.key3_pressed = playerControl->keys[3].fPressed;
    pkg.input.key4_down = playerControl->keys[4].fDown;
    pkg.input.key4_pressed = playerControl->keys[4].fPressed;
    pkg.input.key5_down = playerControl->keys[5].fDown;
    pkg.input.key5_pressed = playerControl->keys[5].fPressed;
    pkg.input.key6_down = playerControl->keys[6].fDown;
    pkg.input.key6_pressed = playerControl->keys[6].fPressed;
    pkg.input.key7_down = playerControl->keys[7].fDown;
    pkg.input.key7_pressed = playerControl->keys[7].fPressed;*/

    sendMessage(&pkg, sizeof(InputPackage));
}

void NetClient::sendCurrentGameState()
{
    if (!netplay.theHostIsMe)
        return;

    GameStatePackage pkg;
    pkg.protocolVersion = NET_PROTOCOL_VERSION;
    pkg.packageType = NET_NOTICE_HOST_STATE;

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.player_x[p] = list_players[p]->fx;
        pkg.player_y[p] = list_players[p]->fy;
        pkg.player_xvel[p] = list_players[p]->velx;
        pkg.player_xvel[p] = list_players[p]->vely;

        COutputControl* playerControl = &netplay.netPlayerInput.outputControls[p];

        pkg.input[p].key0_down = playerControl->keys[0].fDown;
        pkg.input[p].key0_pressed = playerControl->keys[0].fPressed;
        pkg.input[p].key1_down = playerControl->keys[1].fDown;
        pkg.input[p].key1_pressed = playerControl->keys[1].fPressed;
        pkg.input[p].key2_down = playerControl->keys[2].fDown;
        pkg.input[p].key2_pressed = playerControl->keys[2].fPressed;
        pkg.input[p].key3_down = playerControl->keys[3].fDown;
        pkg.input[p].key3_pressed = playerControl->keys[3].fPressed;
        pkg.input[p].key4_down = playerControl->keys[4].fDown;
        pkg.input[p].key4_pressed = playerControl->keys[4].fPressed;
        pkg.input[p].key5_down = playerControl->keys[5].fDown;
        pkg.input[p].key5_pressed = playerControl->keys[5].fPressed;
        pkg.input[p].key6_down = playerControl->keys[6].fDown;
        pkg.input[p].key6_pressed = playerControl->keys[6].fPressed;
        pkg.input[p].key7_down = playerControl->keys[7].fDown;
        pkg.input[p].key7_pressed = playerControl->keys[7].fPressed;
    }

    sendMessage(&pkg, sizeof(GameStatePackage));
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
    //printf("  Incoming room entry: [%u] %s (%d/4)\n", roomInfo.roomID, roomInfo.name, roomInfo.currentPlayerCount);

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
    netplay.currentRoom.hostPlayerNumber = 0;
    memcpy(netplay.currentRoom.name, netplay.newroom_name, NET_MAX_ROOM_NAME_LENGTH);
    memcpy(netplay.currentRoom.playerNames[0], netplay.myPlayerName, NET_MAX_PLAYER_NAME_LENGTH);
    memcpy(netplay.currentRoom.playerNames[1], "(empty)", NET_MAX_PLAYER_NAME_LENGTH);
    memcpy(netplay.currentRoom.playerNames[2], "(empty)", NET_MAX_PLAYER_NAME_LENGTH);
    memcpy(netplay.currentRoom.playerNames[3], "(empty)", NET_MAX_PLAYER_NAME_LENGTH);

    //printf("Room created, ID: %u, %d\n", pkg.roomID, pkg.roomID);
    netplay.currentMenuChanged = true;
    netplay.joinSuccessful = true;
    netplay.theHostIsMe = true;

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
    printf("host: %d\n", pkg.hostPlayerNumber);
    for (uint8_t p = 0; p < 4; p++) {
        memcpy(netplay.currentRoom.playerNames[p], pkg.playerName[p], NET_MAX_PLAYER_NAME_LENGTH);
        printf("  player %d: %s", p+1, netplay.currentRoom.playerNames[p]);
        if (p == pkg.hostPlayerNumber)
            printf(" (HOST)");
        printf("\n");


        if (strcmp(netplay.currentRoom.playerNames[p], "(empty)") == 0)
            game_values.playercontrol[p] = 0;
        //else if (strcmp(netplay.currentRoom.playerNames[p], "(bot)") == 0)
        //    game_values.playercontrol[p] = 2;
        //    game_values.playercontrol[p] = 1;
        else
            game_values.playercontrol[p] = 1; // valid player
    }

    netplay.theHostIsMe = false;
    netplay.currentRoom.hostPlayerNumber = pkg.hostPlayerNumber;
    remotePlayerNumber = pkg.remotePlayerNumber;
    if (remotePlayerNumber == pkg.hostPlayerNumber)
        netplay.theHostIsMe = true;

    netplay.currentMenuChanged = true;
}

void NetClient::handleRemoteInput() // only for room host
{
    assert(netplay.theHostIsMe);

    RemoteInputPackage pkg;
    memcpy(&pkg, udpIncomingPacket->data, sizeof(RemoteInputPackage));

    COutputControl* playerControl = &netplay.netPlayerInput.outputControls[pkg.playerNumber];

    uint16_t mask = 1;
    for (uint8_t k = 0; k < 8; k++) {
        playerControl->keys[k].fDown = pkg.input & mask
        mask <<= 1;
        playerControl->keys[k].fPressed = pkg.input & mask;
        mask <<= 1;
    }

    /*playerControl->keys[0].fDown = pkg.input.key0_down;
    playerControl->keys[0].fPressed = pkg.input.key0_pressed;
    playerControl->keys[1].fDown = pkg.input.key1_down;
    playerControl->keys[1].fPressed = pkg.input.key1_pressed;
    playerControl->keys[2].fDown = pkg.input.key2_down;
    playerControl->keys[2].fPressed = pkg.input.key2_pressed;
    playerControl->keys[3].fDown = pkg.input.key3_down;
    playerControl->keys[3].fPressed = pkg.input.key3_pressed;
    playerControl->keys[4].fDown = pkg.input.key4_down;
    playerControl->keys[4].fPressed = pkg.input.key4_pressed;
    playerControl->keys[5].fDown = pkg.input.key5_down;
    playerControl->keys[5].fPressed = pkg.input.key5_pressed;
    playerControl->keys[6].fDown = pkg.input.key6_down;
    playerControl->keys[6].fPressed = pkg.input.key6_pressed;
    playerControl->keys[7].fDown = pkg.input.key7_down;
    playerControl->keys[7].fPressed = pkg.input.key7_pressed;*/
}

void NetClient::handleRemoteGameState() // for other clients
{
    assert(!netplay.theHostIsMe);

    GameStatePackage pkg;
    memcpy(&pkg, udpIncomingPacket->data, sizeof(GameStatePackage));

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        list_players[p]->fx = pkg.player_x[p];
        list_players[p]->fy = pkg.player_y[p];
        list_players[p]->velx = pkg.player_xvel[p];
        list_players[p]->vely = pkg.player_yvel[p];

        COutputControl* playerControl = &netplay.netPlayerInput.outputControls[p];

        playerControl->keys[0].fDown = pkg.input[p].key0_down;
        playerControl->keys[0].fPressed = pkg.input[p].key0_pressed;
        playerControl->keys[1].fDown = pkg.input[p].key1_down;
        playerControl->keys[1].fPressed = pkg.input[p].key1_pressed;
        playerControl->keys[2].fDown = pkg.input[p].key2_down;
        playerControl->keys[2].fPressed = pkg.input[p].key2_pressed;
        playerControl->keys[3].fDown = pkg.input[p].key3_down;
        playerControl->keys[3].fPressed = pkg.input[p].key3_pressed;
        playerControl->keys[4].fDown = pkg.input[p].key4_down;
        playerControl->keys[4].fPressed = pkg.input[p].key4_pressed;
        playerControl->keys[5].fDown = pkg.input[p].key5_down;
        playerControl->keys[5].fPressed = pkg.input[p].key5_pressed;
        playerControl->keys[6].fDown = pkg.input[p].key6_down;
        playerControl->keys[6].fPressed = pkg.input[p].key6_pressed;
        playerControl->keys[7].fDown = pkg.input[p].key7_down;
        playerControl->keys[7].fPressed = pkg.input[p].key7_pressed;
    }
}

void NetClient::listen()
{
    if (receiveMessage()) {
        uint8_t protocollVersion = incomingData[0];
        uint8_t responseCode = incomingData[1];
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
                    netplay.theHostIsMe = false;
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

                    StartSynchPackage pkg;
                    memcpy(&pkg, udpIncomingPacket->data, sizeof(StartSynchPackage));

                    //printf("reseed: %d\n", pkg.commonRandomSeed);
                    smw->rng->ReSeed(pkg.commonRandomSeed);
                    sendSynchOKMessage();
                    break;

                case NET_NOTICE_GAME_STARTED:
                    printf("NET_NOTICE_GAME_STARTED\n");
                    netplay.gameRunning = true;
                    break;

                case NET_NOTICE_REMOTE_KEYS:
                    //printf("NET_NOTICE_REMOTE_KEYS\n");
                    handleRemoteInput();
                    break;

                case NET_NOTICE_HOST_STATE:
                    handleRemoteGameState();
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

    //printf("sendGoodbye\n");
    sendMessage(&msg, sizeof(MessageHeader));
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

bool NetClient::sendMessage(const void* data, const int dataLength)
{
    if (!networkHandler.sendUDPMessage(data, dataLength))
        return false;

    netplay.lastSentMessage.packageType = ((uint8_t*)data)[1];
    netplay.lastSentMessage.timestamp = SDL_GetTicks();

    return true;
}

bool NetClient::receiveMessage()
{
    if (!networkHandler.receiveUDPMessage(incomingData))
        return false;

    netplay.lastReceivedMessage.packageType = incomingData[1];
    netplay.lastReceivedMessage.timestamp = SDL_GetTicks(); /* TODO: csomagból */

    return true;
}
