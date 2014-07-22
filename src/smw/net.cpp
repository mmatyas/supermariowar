#include "net.h"

#include "FileIO.h"
#include "GameValues.h"
#include "GSMenu.h"
#include "RandomNumberGenerator.h"
#include "player.h"

#include "network/ProtocolPackages.h"

// define platform guards here
#ifndef __EMSCRIPTEN__
    #include "platform/network/enet/NetworkLayerENet.h"
    #define NetworkHandler NetworkLayerENet
#else
    #include "platform/network/null/NetworkLayerNULL.h"
    #define NetworkHandler NetworkLayerNULL
#endif

#include <cassert>
#include <cstring>

NetworkHandler networkHandler;
Networking netplay;

extern CGameValues game_values;
extern int g_iVersion[];
extern CPlayer* list_players[4];
extern short list_players_cnt;
extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

short backup_playercontrol[4];

bool net_init()
{
    if (!networkHandler.init())
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

    printf("[net] Network system initialized.\n");
    return true;
}

void net_close()
{
    net_saveServerList();

    net_endSession();
    netplay.client.cleanup();

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

/****************************
    Session
****************************/

bool net_startSession()
{
    net_endSession(); // Finish previous network session if active

    printf("[net] Session start.\n");
    netplay.active = true;
    netplay.connectSuccessful = false;

    // backup singleplayer settings
    for (uint8_t p = 0; p < 4; p++)
        backup_playercontrol[p] = game_values.playercontrol[p];

    return netplay.client.start();
}

void net_endSession()
{
    if (netplay.active) {
        printf("[net] Session end.\n");

        netplay.client.stop();

        netplay.active = false;
        netplay.connectSuccessful = false;

        // restore singleplayer settings
        for (uint8_t p = 0; p < 4; p++)
            game_values.playercontrol[p] = backup_playercontrol[p];
    }
}

/********************************************************************
 * NetClient
 ********************************************************************/

NetClient::NetClient()
{
    //printf("NetClient::ctor\n");
}

NetClient::~NetClient()
{
    //printf("NetClient::dtor\n");
    cleanup();
}

bool NetClient::init()
{
    //printf("NetClient::init\n");
    // init client stuff first
    // ...

    // then finally gamehost
    if (!local_gamehost.init())
        return false;

    return true;
}

bool NetClient::start() // startSession
{
    //printf("NetClient::start\n");
    if (!networkHandler.client_restart())
        return false;

    return true;
}

void NetClient::update()
{
    local_gamehost.update();
    networkHandler.client_listen(*this);
}

void NetClient::stop() // endSession
{
    //printf("NetClient::stop\n");
    local_gamehost.stop();

    if (netplay.connectSuccessful)
        sendGoodbye();

    if (foreign_lobbyserver) {
        foreign_lobbyserver->disconnect();
        foreign_lobbyserver = NULL;
    }
    if (foreign_gamehost) {
        foreign_gamehost->disconnect();
        foreign_gamehost = NULL;
    }

    networkHandler.client_shutdown();
}

void NetClient::cleanup()
{
    //printf("NetClient::cleanup\n");
    stop();

    // ...
}

void NetClient::setRoomListUIControl(MI_NetworkListScroll* control)
{
    uiRoomList = control;
}

/****************************
    Phase 1: Connect
****************************/

bool NetClient::sendConnectRequestToSelectedServer()
{
    ServerAddress* selectedServer = &netplay.savedServers[netplay.selectedServerIndex];
    if (!openConnection(selectedServer->hostname.c_str()))
        return false;
    
    netplay.operationInProgress = true;
    return true;
}

void NetClient::sendGoodbye()
{
    //printf("sendGoodbye\n");
    Net_ClientDisconnectionPackage msg;
    sendMessageToLobbyServer(&msg, sizeof(Net_ClientDisconnectionPackage));
}

void NetClient::handleServerinfoAndClose(const uint8_t* data, size_t dataLength)
{
    // TODO: Remove copies.

    Net_ServerInfoPackage serverInfo;
    memcpy(&serverInfo, data, sizeof(Net_ServerInfoPackage));

    printf("[net] Server information: Name: %s, Protocol version: %u.%u  Players/Max: %d / %d\n",
        serverInfo.name, serverInfo.protocolMajorVersion, serverInfo.protocolMinorVersion,
        serverInfo.currentPlayerCount, serverInfo.maxPlayerCount);

    foreign_lobbyserver->disconnect();
    foreign_lobbyserver = NULL;
}

/****************************
    Phase 2: Join room
****************************/

void NetClient::requestRoomList()
{
    Net_RoomListPackage msg;
    sendMessageToLobbyServer(&msg, sizeof(Net_RoomListPackage));

    if (uiRoomList)
        uiRoomList->Clear();
}

void NetClient::handleNewRoomListEntry(const uint8_t* data, size_t dataLength)
{
    Net_RoomInfoPackage roomInfo;
    memcpy(&roomInfo, data, sizeof(Net_RoomInfoPackage));
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

void NetClient::sendCreateRoomMessage()
{
    Net_NewRoomPackage msg(netplay.newroom_name, netplay.newroom_password);

    sendMessageToLobbyServer(&msg, sizeof(Net_NewRoomPackage));
    netplay.operationInProgress = true;
}

void NetClient::handleRoomCreatedMessage(const uint8_t* data, size_t dataLength)
{
    Net_NewRoomCreatedPackage pkg;
    memcpy(&pkg, data, sizeof(Net_NewRoomCreatedPackage));

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

    local_gamehost.start(foreign_lobbyserver);
}

void NetClient::sendJoinRoomMessage()
{
    if (netplay.selectedRoomIndex >= netplay.currentRooms.size())
        return;

    // TODO: implement password
    Net_JoinRoomPackage msg(netplay.currentRooms.at(netplay.selectedRoomIndex).roomID, "");
    sendMessageToLobbyServer(&msg, sizeof(Net_JoinRoomPackage));
    netplay.operationInProgress = true;
}

void NetClient::sendLeaveRoomMessage()
{
    local_gamehost.stop();

    Net_LeaveRoomPackage msg;
    sendMessageToLobbyServer(&msg, sizeof(Net_LeaveRoomPackage));
}

void NetClient::handleRoomChangedMessage(const uint8_t* data, size_t dataLength)
{
    //printf("ROOM CHANGED\n");
    Net_CurrentRoomPackage pkg;
    memcpy(&pkg, data, sizeof(Net_CurrentRoomPackage));

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
    if (remotePlayerNumber == pkg.hostPlayerNumber) {
        netplay.theHostIsMe = true;
        local_gamehost.start(foreign_lobbyserver);
    }

    netplay.currentMenuChanged = true;
}

/****************************
    Phase 3: Play
****************************/

void NetClient::sendSyncOKMessage()
{
    Net_SyncOKPackage msg;
    sendMessageToGameHost(&msg, sizeof(Net_SyncOKPackage));
}

void NetClient::sendLocalInput()
{
    Net_LocalInputPackage pkg(&netplay.netPlayerInput.outputControls[0]);
    sendMessageToGameHost(&pkg, sizeof(Net_LocalInputPackage));
}

void NetClient::handleRemoteGameState(const uint8_t* data, size_t dataLength) // for other clients
{
    Net_GameStatePackage pkg;
    memcpy(&pkg, data, sizeof(Net_GameStatePackage));

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.getPlayerCoord(p, list_players[p]->fx, list_players[p]->fy);
        pkg.getPlayerVel(p, list_players[p]->velx, list_players[p]->vely);
        pkg.getPlayerKeys(p, &netplay.netPlayerInput.outputControls[p]);
    }
}

/****************
    Listening
****************/

// This client sucesfully connected to a host
void NetClient::onConnect(NetPeer* newServer)
{
    assert(newServer);
    assert(foreign_lobbyserver == NULL);
    foreign_lobbyserver = newServer;

    Net_ClientConnectionPackage message(netplay.myPlayerName);
    sendMessageToLobbyServer(&message, sizeof(Net_ClientConnectionPackage));
}

void NetClient::onDisconnect(NetPeer& client)
{
    // ...
}

void NetClient::onReceive(NetPeer& client, const uint8_t* data, size_t dataLength)
{
    assert(data);
    assert(dataLength >= 3);

    uint8_t protocolMajor = data[0];
    uint8_t protocolMinor = data[1];
    uint8_t packageType = data[2];
    if (protocolMajor != NET_PROTOCOL_VERSION_MAJOR)
        return;

    switch (packageType)
    {
        //
        // Query
        //
        case NET_RESPONSE_BADPROTOCOL:
            printf("Not implemented: NET_RESPONSE_BADPROTOCOL\n");
            break;

        case NET_RESPONSE_SERVERINFO:
            handleServerinfoAndClose(data, dataLength);
            break;

        case NET_RESPONSE_SERVER_MOTD:
            printf("Not implemented: NET_RESPONSE_SERVER_MOTD\n");
            break;

        //
        // Connect
        //
        case NET_RESPONSE_CONNECT_OK:
            //printf("[net] Connection attempt successful.\n");
            netplay.connectSuccessful = true;
            break;

        case NET_RESPONSE_CONNECT_DENIED:
            printf("Not implemented: NET_RESPONSE_CONNECT_DENIED\n");
            netplay.connectSuccessful = false;
            break;

        case NET_RESPONSE_CONNECT_SERVERFULL:
            printf("Not implemented: NET_RESPONSE_CONNECT_SERVERFULL\n");
            netplay.connectSuccessful = false;
            break;

        case NET_RESPONSE_CONNECT_NAMETAKEN:
            printf("Not implemented: NET_RESPONSE_CONNECT_NAMETAKEN\n");
            netplay.connectSuccessful = false;
            break;

        //
        // Rooms
        //
        case NET_RESPONSE_ROOM_LIST_ENTRY:
            handleNewRoomListEntry(data, dataLength);
            break;

        case NET_RESPONSE_NO_ROOMS:
            printf("Not implemented: [net] There are no rooms currently on the server.\n");
            break;

        //
        // Join
        //
        case NET_RESPONSE_JOIN_OK:
            printf("Not implemented: [net] Joined to room.\n");
            netplay.joinSuccessful = true;
            netplay.theHostIsMe = false;
            break;

        case NET_RESPONSE_ROOM_FULL:
            printf("Not implemented: [net] The room is full\n");
            netplay.joinSuccessful = false;
            break;

        case NET_NOTICE_ROOM_CHANGED:
            handleRoomChangedMessage(data, dataLength);
            break;

        //
        // Create
        //
        case NET_RESPONSE_CREATE_OK:
            handleRoomCreatedMessage(data, dataLength);
            break;

        case NET_RESPONSE_CREATE_ERROR:
            printf("Not implemented: NET_RESPONSE_CREATE_ERROR\n");
            break;

        //
        // Game
        //

        case NET_NOTICE_GAMEHOST_INFO:
            printf("Not fully implemented: NET_RECV_GAMEHOST_INFO\n");
            {
                Net_StartSyncPackage pkg;
                memcpy(&pkg, data, sizeof(Net_StartSyncPackage));

                //printf("reseed: %d\n", pkg.commonRandomSeed);
                RandomNumberGenerator::generator().reseed(pkg.commonRandomSeed);
            }
            sendSyncOKMessage();
            break;

        case NET_G2P_GAME_START:
            printf("NET_RECV_GAMEHOST_GAME_STARTED\n");
            netplay.gameRunning = true;
            break;

        case NET_G2P_GAME_STATE:
            handleRemoteGameState(data, dataLength);
            break;

        //
        // Default
        //

        default:
            printf("Unknown: ");
            for (int a = 0; a < dataLength; a++)
                printf("%3d ", data[a]);
            printf("\n");
            return; // do not set as last message
    }

    setAsLastReceivedMessage(packageType);
}

/****************************
    Network Communication
****************************/

bool NetClient::openConnection(const char* hostname, const uint16_t port)
{
    netplay.connectSuccessful = false;
    if (!networkHandler.connectToLobbyServer(hostname, port))
        return false;

    return true;
    // now we wait for CONNECT_OK
    // connectSuccessful will be set to 'true' there
}

bool NetClient::sendTo(NetPeer*& peer, const void* data, int dataLength)
{
    assert(peer);
    assert(data);
    assert(dataLength >= 3);

    if (!peer->send(data, dataLength))
        return false;

    setAsLastSentMessage(((uint8_t*)data)[2]);
    return true;
}

bool NetClient::sendMessageToLobbyServer(const void* data, int dataLength)
{
    return sendTo(foreign_lobbyserver, data, dataLength);
}

bool NetClient::sendMessageToGameHost(const void* data, int dataLength)
{
    return sendTo(foreign_gamehost, data, dataLength);
}

void NetClient::setAsLastSentMessage(uint8_t packageType)
{
    printf("setAsLastSentMessage: %d.\n", packageType);
    lastSentMessage.packageType = packageType;
    lastSentMessage.timestamp = SDL_GetTicks();
}

void NetClient::setAsLastReceivedMessage(uint8_t packageType)
{
    printf("setAsLastReceivedMessage: %d.\n", packageType);
    lastReceivedMessage.packageType = packageType;
    lastReceivedMessage.timestamp = SDL_GetTicks();
}





NetGameHost::NetGameHost()
    : active(false)
    , foreign_lobbyserver(NULL)
{
    //printf("NetGameHost::ctor\n");
}

NetGameHost::~NetGameHost()
{
    //printf("NetGameHost::dtor\n");
    cleanup();
}

bool NetGameHost::init()
{
    //printf("NetGameHost::init\n");
    return true;
}

bool NetGameHost::start(NetPeer*& lobbyserver)
{
    assert(lobbyserver);

    if (active) {
        assert(lobbyserver == foreign_lobbyserver);
        return true;
    }

    assert(!foreign_lobbyserver);

    //printf("NetGameHost::start\n");
    foreign_lobbyserver = lobbyserver;

    if (!networkHandler.gamehost_restart())
        return false;

    active = true;
    return true;
}

void NetGameHost::update()
{
    if (active)
        networkHandler.gamehost_listen(*this);
}

void NetGameHost::stop()
{
    if (!active)
        return;

    //printf("NetGameHost::stop\n");

    foreign_lobbyserver = NULL; // client may be still connected
    for (short p = 0; p < 3; p++) {
        if (clients[p]) {
            clients[p]->disconnect();
            clients[p] = NULL;
        }
    }

    networkHandler.gamehost_shutdown();
    active = false;
}

void NetGameHost::cleanup()
{
    //printf("NetGameHost::cleanup\n");
    stop();
}

// A client connected to this host
void NetGameHost::onConnect(NetPeer* new_player)
{

}

void NetGameHost::onDisconnect(NetPeer& player)
{

}

void NetGameHost::onReceive(NetPeer& player, const uint8_t* data, size_t dataLength)
{
    assert(data);
    assert(dataLength >= 3);

    uint8_t protocolMajor = data[0];
    uint8_t protocolMinor = data[1];
    uint8_t packageType = data[2];
    if (protocolMajor != NET_PROTOCOL_VERSION_MAJOR)
        return;

    switch (packageType)
    {

        case NET_P2G_LOCAL_KEYS:
            //printf("NET_NOTICE_REMOTE_KEYS\n");
            handleRemoteInput(data, dataLength);
            break;
    }
}

void NetGameHost::sendStartRoomMessage()
{
    assert(foreign_lobbyserver);

    Net_StartRoomPackage msg;
    foreign_lobbyserver->send(&msg, sizeof(Net_StartRoomPackage));
}

void NetGameHost::sendCurrentGameState()
{
    Net_GameStatePackage pkg;

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.setPlayerCoord(p, list_players[p]->fx, list_players[p]->fy);
        pkg.setPlayerVel(p, list_players[p]->velx, list_players[p]->vely);
        pkg.setPlayerKeys(p, &netplay.netPlayerInput.outputControls[p]);
    }

    for (int c = 0; c < 3; c++) {
        if (clients[c])
            clients[c]->send(&pkg, sizeof(Net_GameStatePackage));
    }
}

bool NetGameHost::sendMessageToMyPeers(const void* data, size_t dataLength)
{
    assert(data);
    assert(dataLength >= 3);

    for (int c = 0; c < 3; c++) {
        if (clients[c])
            clients[c]->send(data, dataLength);
    }

    setAsLastSentMessage(((uint8_t*)data)[2]);
    return true;
}

void NetGameHost::handleRemoteInput(const uint8_t* data, size_t dataLength) // only for room host
{
    Net_RemoteInputPackage pkg;
    memcpy(&pkg, data, sizeof(Net_RemoteInputPackage));

    // TODO: Remove/check player number field.
    pkg.readKeys(&netplay.netPlayerInput.outputControls[pkg.playerNumber]);
}

void NetGameHost::setAsLastSentMessage(uint8_t packageType)
{
    lastSentMessage.packageType = packageType;
    lastSentMessage.timestamp = SDL_GetTicks();
}

void NetGameHost::setAsLastReceivedMessage(uint8_t packageType)
{
    lastReceivedMessage.packageType = packageType;
    lastReceivedMessage.timestamp = SDL_GetTicks();
}
