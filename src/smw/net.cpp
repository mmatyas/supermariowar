#include "global.h"
#include <string.h>

#include "net.h"
#include "GSMenu.h"
#include "FileIO.h"

#include "network/NetworkProtocolPackages.h"

// define platform guards here
#ifndef __EMSCRIPTEN__
    #define NetworkHandler NetworkHandlerSDL
    #include "platform/network/sdl/NetworkHandlerSDL.h"
#else
    #define NetworkHandler NetworkHandlerNULL
    #include "platform/network/null/NetworkHandlerNULL.h"
#endif

NetworkHandler networkHandler;
Networking netplay;

extern gv game_values;
extern int g_iVersion[];
extern CPlayer *list_players[4];
extern short list_players_cnt;
extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);


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
    return true;
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
    if (fp) {
        int version[4];
        if (4 != fread(version, sizeof(int), 4, fp)) {
            fclose(fp);
            return; // read error
        }

        if (VersionIsEqual(g_iVersion, version[0], version[1], version[2], version[3])) {
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

void NetClient::cleanup()
{
    endSession();
    networkHandler.cleanup();
}

void NetClient::setRoomListUIControl(MI_NetworkListScroll* control)
{
    uiRoomList = control;
}

/****************************
    Session
****************************/

bool NetClient::startSession()
{
    printf("Session start.\n");
    endSession(); // Finish previous network session if active

    netplay.active = true;
    netplay.connectSuccessful = false;

    // backup singleplayer settings
    for (uint8_t p = 0; p < 4; p++)
        backup_playercontrol[p] = game_values.playercontrol[p];

    return true;
}

void NetClient::endSession()
{
    if (netplay.active) {
        printf("Session end.\n");
        if (netplay.connectSuccessful)
            sendGoodbye();

        closeConnection();
        netplay.active = false;
        netplay.connectSuccessful = false;

        // restore singleplayer settings
        for (uint8_t p = 0; p < 4; p++)
            game_values.playercontrol[p] = backup_playercontrol[p];
    }
}

void NetClient::sendGoodbye()
{
    //printf("sendGoodbye\n");
    ClientDisconnectionPackage msg;
    sendMessage(&msg, sizeof(ClientDisconnectionPackage));
}

/****************************
    Outgoing messages
****************************/

void NetClient::requestRoomList()
{
    RoomListPackage msg;
    sendMessage(&msg, sizeof(RoomListPackage));

    if (uiRoomList)
        uiRoomList->Clear();
}

bool NetClient::sendConnectRequestToSelectedServer()
{
    ServerAddress* selectedServer = &netplay.savedServers[netplay.selectedServerIndex];
    if (openConnection(selectedServer->hostname.c_str()))
    {
        ClientConnectionPackage message(netplay.myPlayerName);
        sendMessage(&message, sizeof(ClientConnectionPackage));
        netplay.operationInProgress = true;
        return true;
    }
    return false;
}

void NetClient::sendCreateRoomMessage()
{
    NewRoomPackage msg(netplay.newroom_name, netplay.newroom_password);

    sendMessage(&msg, sizeof(NewRoomPackage));
    netplay.operationInProgress = true;
}

void NetClient::sendJoinRoomMessage()
{
    if (netplay.selectedRoomIndex >= netplay.currentRooms.size())
        return;

    // TODO: implement password
    JoinRoomPackage msg(netplay.currentRooms.at(netplay.selectedRoomIndex).roomID, "");
    sendMessage(&msg, sizeof(JoinRoomPackage));
    netplay.operationInProgress = true;
}

void NetClient::sendLeaveRoomMessage()
{
    LeaveRoomPackage msg;
    sendMessage(&msg, sizeof(LeaveRoomPackage));
}

void NetClient::sendStartRoomMessage()
{
    StartRoomPackage msg;
    sendMessage(&msg, sizeof(StartRoomPackage));
}

void NetClient::sendSyncOKMessage()
{
    SyncOKPackage msg;
    sendMessage(&msg, sizeof(SyncOKPackage));
}

void NetClient::sendLocalInput()
{
    LocalInputPackage pkg(&netplay.netPlayerInput.outputControls[0]);
    sendMessage(&pkg, sizeof(LocalInputPackage));
}

void NetClient::sendCurrentGameState()
{
    if (!netplay.theHostIsMe)
        return;

    GameStatePackage pkg;

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.setPlayerCoord(p, list_players[p]->fx, list_players[p]->fy);
        pkg.setPlayerVel(p, list_players[p]->velx, list_players[p]->vely);
        pkg.setPlayerKeys(p, &netplay.netPlayerInput.outputControls[p]);
    }

    sendMessage(&pkg, sizeof(GameStatePackage));
}

/****************************
    Incoming messages
****************************/

void NetClient::handleServerinfoAndClose()
{
    ServerInfoPackage serverInfo;
    memcpy(&serverInfo, incomingData, sizeof(ServerInfoPackage));

    printf("NET_RESPONSE_SERVERINFO [%lu byte]\n", sizeof(serverInfo));
    printf("Sending:\n  protocolVersion: %d\n  packageType: %d\n  name: %s\n  players/max: %d / %d\n",
        serverInfo.protocolVersion, serverInfo.packageType, serverInfo.name, serverInfo.currentPlayerCount, serverInfo.maxPlayerCount);

    closeConnection();
}

void NetClient::handleNewRoomListEntry()
{
    RoomInfoPackage roomInfo;
    memcpy(&roomInfo, incomingData, sizeof(RoomInfoPackage));
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
    memcpy(&pkg, incomingData, sizeof(NewRoomCreatedPackage));

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
    memcpy(&pkg, incomingData, sizeof(CurrentRoomPackage));

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
    memcpy(&pkg, incomingData, sizeof(RemoteInputPackage));

    pkg.readKeys(&netplay.netPlayerInput.outputControls[pkg.playerNumber]);
}

void NetClient::handleRemoteGameState() // for other clients
{
    assert(!netplay.theHostIsMe);

    GameStatePackage pkg;
    memcpy(&pkg, incomingData, sizeof(GameStatePackage));

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.getPlayerCoord(p, list_players[p]->fx, list_players[p]->fy);
        pkg.getPlayerVel(p, list_players[p]->velx, list_players[p]->vely);
        pkg.getPlayerKeys(p, &netplay.netPlayerInput.outputControls[p]);
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

                case NET_NOTICE_GAME_SYNC:
                    printf("NET_NOTICE_GAME_SYNC\n");
                    {
                        StartSyncPackage pkg;
                        memcpy(&pkg, incomingData, sizeof(StartSyncPackage));

                        //printf("reseed: %d\n", pkg.commonRandomSeed);
                        smw->rng->ReSeed(pkg.commonRandomSeed);
                    }
                    sendSyncOKMessage();
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
                    /*for (int a = 0; a < udpIncomingPacket->len; a++)
                        printf("%3d ", incomingData[a]);*/
                    printf("\n");
                    break;
            }
        }
    }
}

/****************************
    Network Communication
****************************/

bool NetClient::openConnection(const char* hostname, const uint16_t port)
{
    netplay.connectSuccessful = false;
    if (!networkHandler.openUDPConnection(hostname, port))
        return false;

    return true;
    // now we wait for CONNECT_OK
    // connectSuccessful will be set to 'true' there
}

void NetClient::closeConnection()
{
    networkHandler.closeUDPConnection();
}

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
