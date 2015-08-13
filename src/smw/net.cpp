#include "net.h"

#include "GlobalConstants.h"
#include "GameValues.h"
#include "GSMenu.h"
#include "path.h"
#include "player.h"
#include "RandomNumberGenerator.h"

#include "network/FileCompressor.h"
#include "network/ProtocolPackages.h"
#include "network/NetConfigManager.h"

// define platform guards here
#ifdef NETWORK_DISABLED
    #include "platform/network/null/NetworkLayerNULL.h"
    #define NetworkHandler NetworkLayerNULL
#else
    #include "platform/network/enet/NetworkLayerENet.h"
    #define NetworkHandler NetworkLayerENet
#endif

#include <cassert>
#include <cstring>
#include <ctime>
#include <string>

NetworkHandler networkHandler;
Networking netplay;

extern CGameValues game_values;
extern CPlayer* list_players[4];
extern short list_players_cnt;

short backup_playercontrol[4];

bool net_init()
{
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
    netplay.waitingForPowerupTrigger = false;
    netplay.allowMapCollisionEvent = false;
    netplay.gamestate_changed = false;

    if (!networkHandler.init())
        return false;

    if (!netplay.client.init())
        return false;

    ServerAddress none;
    none.hostname = "(none)";
    netplay.savedServers.push_back(none);

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
    NetConfigManager config;
    config.save();
}

void net_loadServerList()
{
    NetConfigManager config;
    config.load();
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
    if (!connectLobby(selectedServer->hostname.c_str()))
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
        // TODO: strings would be better
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

    assert(strlen(netplay.myPlayerName) <= NET_MAX_PLAYER_NAME_LENGTH);
    assert(strlen(netplay.newroom_name) <= NET_MAX_ROOM_NAME_LENGTH);

    netplay.currentRoom.roomID = pkg.roomID;
    netplay.currentRoom.hostPlayerNumber = 0;
    netplay.currentRoom.name = netplay.newroom_name;
    netplay.currentRoom.playerNames[0] = netplay.myPlayerName;
    netplay.currentRoom.playerNames[1] = "(empty)";
    netplay.currentRoom.playerNames[2] = "(empty)";
    netplay.currentRoom.playerNames[3] = "(empty)";

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
    assert(netplay.selectedRoomIndex < netplay.currentRooms.size());

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

    assert(strlen(pkg.name) <= NET_MAX_ROOM_NAME_LENGTH);
    netplay.currentRoom.roomID = pkg.roomID;
    netplay.currentRoom.name = pkg.name;

    printf("Room %u (%s) changed:\n", pkg.roomID, pkg.name);
    // printf("  host: %d\n", pkg.hostPlayerNumber);
    for (uint8_t p = 0; p < 4; p++) {
        assert(strlen(pkg.playerName[p]) <= NET_MAX_PLAYER_NAME_LENGTH);
        netplay.currentRoom.playerNames[p] = pkg.playerName[p];
        printf("  player %d: %s", p+1, netplay.currentRoom.playerNames[p].c_str());
        if (p == pkg.remotePlayerNumber)
            printf(" (me)");
        if (p == pkg.hostPlayerNumber)
            printf(" (HOST)");
        printf("\n");


        if (netplay.currentRoom.playerNames[p].compare("(empty)") == 0)
            game_values.playercontrol[p] = 0;
        //else if (strcmp(netplay.currentRoom.playerNames[p], "(bot)") == 0)
        //    game_values.playercontrol[p] = 2;
        //    game_values.playercontrol[p] = 1;
        else
            game_values.playercontrol[p] = 1; // valid player
    }

    netplay.theHostIsMe = false;
    netplay.currentRoom.hostPlayerNumber = pkg.hostPlayerNumber;
    netplay.remotePlayerNumber = pkg.remotePlayerNumber;
    if (netplay.remotePlayerNumber == pkg.hostPlayerNumber) {
        netplay.theHostIsMe = true;
        local_gamehost.start(foreign_lobbyserver);
    } else {
        local_gamehost.stop();
    }

    netplay.currentMenuChanged = true;
}

void NetClient::sendChatMessage(const char* message)
{
    assert(message);
    assert(strlen(message) > 0);
    assert(strlen(message) <= NET_MAX_CHAT_MSG_LENGTH);

    Net_RoomChatMsgPackage pkg(netplay.remotePlayerNumber, message);
    sendMessageToLobbyServer(&pkg, sizeof(Net_RoomChatMsgPackage));
}

void NetClient::handleRoomChatMessage(const uint8_t* data, size_t dataLength)
{
    Net_RoomChatMsgPackage pkg;
    memcpy(&pkg, data, sizeof(Net_RoomChatMsgPackage));

    assert(pkg.senderNum < 4);
    printf("Not implemented: [net] %s: %s\n",
        netplay.currentRoom.playerNames[pkg.senderNum].c_str(), pkg.message);
}

/****************************
    Phase 2.5: Pre-game
****************************/

// This package goes to normal players
void NetClient::handleRoomStartMessage(const uint8_t* data, size_t dataLength)
{
    Net_GameHostInfoPkg pkg(0);
    memcpy(&pkg, data, sizeof(Net_GameHostInfoPkg));

    char host_str[32];
    sprintf(host_str, "%d.%d.%d.%d",
        pkg.host & 0xFF, (pkg.host & 0xFF00) >> 8,
        (pkg.host & 0xFF0000) >> 16, (pkg.host & 0xFF000000) >> 24);

    printf("[net] Connecting to game host... [%s:%d]\n", host_str, NET_SERVER_PORT + 1);
    if (!connectGameHost(host_str)) {
        printf("[net][error] Could not connect to game host.\n");
        return;
    }

    netplay.operationInProgress = true;
}

// This package goes to the game host player
void NetClient::handleExpectedClientsMessage(const uint8_t* data, size_t dataLength)
{
    Net_PlayerInfoPkg pkg;
    memcpy(&pkg, data, sizeof(Net_PlayerInfoPkg));

    uint8_t playerCount = 0;
    uint32_t hosts[3];
    uint16_t ports[3];

    for (uint8_t p = 0; p < 3; p++) {
        if (pkg.host[p]) { // this is 0 for empty player slots
            hosts[playerCount] = pkg.host[p];
            ports[playerCount] = pkg.port[p];
            playerCount++;
            printf("[net] Client %d is %u:%u\n", p, pkg.host[p], pkg.port[p]);
        }
    }

    local_gamehost.setExpectedPlayers(playerCount, hosts, ports);
}

void NetClient::handleMapSyncMessage(const uint8_t* data, size_t dataLength)
{
    // read
    Net_MessageHeader pkg(0);
    memcpy(&pkg, data, sizeof(Net_MessageHeader));

    // FIXME: a reminder, make sure to check package sizes

    uint16_t full_size, compressed_size;
    memcpy(&full_size, data + sizeof(Net_MessageHeader), 2);
    memcpy(&compressed_size, data + sizeof(Net_MessageHeader) + 2, 2);

    printf("[net] Map arrived (C:%d unC:%d)\n", compressed_size, full_size);

    netplay.mapfilepath = GetHomeDirectory() + "net_last.map";
    if (!FileCompressor::decompress(data + sizeof(Net_MessageHeader), netplay.mapfilepath))
        return;

    // respond
    if (netplay.theHostIsMe)
        setAsLastSentMessage(NET_P2G_MAP_OK);
    else {
        Net_MapSyncOKPackage respond_pkg;
        sendMessageToGameHostReliable(&respond_pkg, sizeof(Net_MapSyncOKPackage));
    }
}

void NetClient::handleStartSyncMessage(const uint8_t* data, size_t dataLength)
{
    // read
    Net_StartSyncPackage pkg(0);
    memcpy(&pkg, data, sizeof(Net_StartSyncPackage));

    // apply
    printf("reseed: %d\n", pkg.commonRandomSeed);
    RandomNumberGenerator::generator().reseed(pkg.commonRandomSeed);

    // respond
    if (netplay.theHostIsMe)
        setAsLastSentMessage(NET_P2G_SYNC_OK);
    else {
        Net_SyncOKPackage respond_pkg;
        sendMessageToGameHostReliable(&respond_pkg, sizeof(Net_SyncOKPackage));
    }
}

void NetClient::handleGameStartMessage()
{
    printf("[net] Game start!\n");
    netplay.gameRunning = true;
}

/****************************
    Phase 3: Play
****************************/

void NetClient::sendLeaveGameMessage()
{
    // FIXME: make this function better

    Net_LeaveGamePackage pkg;
    if (netplay.theHostIsMe) {
        // disconnect all players
        local_gamehost.sendMessageToMyPeers(&pkg, sizeof(Net_LeaveGamePackage));
        local_gamehost.update();
        // stop game host
        local_gamehost.stop();

        //apply to local client
        setAsLastReceivedMessage(pkg.packageType);
    } else {
        // diconnect from foreign game host
        sendMessageToGameHostReliable(&pkg, sizeof(Net_LeaveGamePackage));
        assert(!local_gamehost.active);
    }

    //state change
    //...
    netplay.gameRunning = false;
}

void NetClient::sendLocalInput()
{
    assert(!netplay.theHostIsMe);
    /*if (netplay.theHostIsMe)
        return;*/

    Net_ClientInputPackage pkg(&game_values.playerInput.outputControls[0]);
    sendMessageToGameHost(&pkg, sizeof(Net_ClientInputPackage));

    //game_values.playerInput.outputControls[iGlobalID];
    /*printf("INPUT %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d\n",
        game_values.playerInput.outputControls[0].keys[0].fDown,
        game_values.playerInput.outputControls[0].keys[0].fPressed,
        game_values.playerInput.outputControls[0].keys[1].fDown,
        game_values.playerInput.outputControls[0].keys[1].fPressed,
        game_values.playerInput.outputControls[0].keys[2].fDown,
        game_values.playerInput.outputControls[0].keys[2].fPressed,
        game_values.playerInput.outputControls[0].keys[3].fDown,
        game_values.playerInput.outputControls[0].keys[3].fPressed,
        game_values.playerInput.outputControls[0].keys[4].fDown,
        game_values.playerInput.outputControls[0].keys[4].fPressed,
        game_values.playerInput.outputControls[0].keys[5].fDown,
        game_values.playerInput.outputControls[0].keys[5].fPressed,
        game_values.playerInput.outputControls[0].keys[6].fDown,
        game_values.playerInput.outputControls[0].keys[6].fPressed,
        game_values.playerInput.outputControls[0].keys[7].fDown,
        game_values.playerInput.outputControls[0].keys[7].fPressed);*/
}

// A client wants to use powerup, so asks the boss for validation
void NetClient::sendPowerupRequest()
{
    assert(!netplay.theHostIsMe);

    Net_RequestPowerupPackage pkg;
    sendMessageToGameHostReliable(&pkg, sizeof(Net_RequestPowerupPackage));
}

void NetClient::handlePowerupStart(const uint8_t* data, size_t dataLength)
{
    Net_StartPowerupPackage pkg(0xFF, 0, 0);
    memcpy(&pkg, data, sizeof(Net_StartPowerupPackage));

    assert(pkg.player_id < 4);
    if (pkg.player_id > 3)
        return;

    // if the source of this event was me
    if (pkg.player_id == netplay.remotePlayerNumber)
        return;

    assert(list_players[pkg.player_id]);
    if (!list_players[pkg.player_id])
        return;

    list_players[pkg.player_id]->powerupused = pkg.powerup_id;
    unsigned missed_frames = (pkg.delay + foreign_gamehost->averageRTT() / 2) / WAITTIME;
    // TODO: make this nicer
    // TODO: this might be overriden by a late packet, in tryReleasingPowerup()
    list_players[pkg.player_id]->powerupradius = 100.0f;
    for (; missed_frames > 0; missed_frames--) {
        list_players[pkg.player_id]->powerupradius -= (float)game_values.storedpowerupdelay / 2.0f;
    }

    // TODO: replace this eventually with the powerup trigger package
    netplay.waitingForPowerupTrigger = false;
    printf("[net] P%d used powerup %d\n", pkg.player_id, pkg.powerup_id);
}

/*void NetClient::handlePowerupTrigger(const uint8_t* data, size_t dataLength)
{
    Net_TriggerPowerupPackage pkg(0xFF, 0, 0, 0);
    memcpy(&pkg, data, sizeof(Net_TriggerPowerupPackage));

    assert(pkg.player_id < 4);
    if (pkg.player_id > 3)
        return;

    float temp_px = list_players[pkg.player_id]->fx;
    float temp_py = list_players[pkg.player_id]->fy;
    list_players[pkg.player_id]->powerupused = pkg.powerup_id;
    list_players[pkg.player_id]->setXf(pkg.player_x);
    list_players[pkg.player_id]->setYf(pkg.player_y);
    list_players[pkg.player_id]->triggerPowerup();
    list_players[pkg.player_id]->setXf(temp_px);
    list_players[pkg.player_id]->setYf(temp_py);
}*/

void NetClient::handleMapCollision(const uint8_t* data, size_t dataLength)
{
    Net_MapCollisionPackage pkg;
    memcpy(&pkg, data, sizeof(Net_MapCollisionPackage));

    assert(pkg.player_id < 4);
    if (pkg.player_id > 3)
        return;

    assert(list_players[pkg.player_id]);
    if (!list_players[pkg.player_id])
        return;

    //printf("[net] Map collision triggered for P%d.\n", pkg.player_id);
    float temp_px = list_players[pkg.player_id]->fx;
    float temp_py = list_players[pkg.player_id]->fy;
    list_players[pkg.player_id]->setXf(pkg.player_x);
    list_players[pkg.player_id]->setYf(pkg.player_y);

    netplay.allowMapCollisionEvent = true;
    list_players[pkg.player_id]->collision_detection_map();
    netplay.allowMapCollisionEvent = false;

    if (list_players[pkg.player_id]->isdead())
        return;

    list_players[pkg.player_id]->setXf(temp_px);
    list_players[pkg.player_id]->setYf(temp_py);

    printf("[net] Map block collision!\n");
}

void NetClient::handleP2PCollision(const uint8_t * data, size_t dataLength)
{
    Net_P2PCollisionPackage pkg;
    memcpy(&pkg, data, sizeof(Net_P2PCollisionPackage));

    assert(pkg.player_id[0] < 4);
    assert(pkg.player_id[1] < 4);
    if (pkg.player_id[0] > 3 || pkg.player_id[1] > 3)
        return;

    assert(list_players[pkg.player_id[0]]);
    assert(list_players[pkg.player_id[1]]);
    if (!list_players[pkg.player_id[0]] || !list_players[pkg.player_id[1]])
        return;

    CPlayer* player1 = list_players[pkg.player_id[0]];
    CPlayer* player2 = list_players[pkg.player_id[1]];

    // float temp_p1x = player1.fx;
    // float temp_p1y = player1.fy;
    player1->setXf(pkg.player_x[0]);
    player1->setYf(pkg.player_y[0]);
    player1->fOldY = pkg.player_oldy[0];

    // float temp_p2x = player2.fx;
    // float temp_p2y = player2.fy;
    player2->setXf(pkg.player_x[1]);
    player2->setYf(pkg.player_y[1]);
    player2->fOldY = pkg.player_oldy[1];

    player1->collidesWith(player2);

    // TODO: restore position?

    printf("P%d collided with P%d!\n", pkg.player_id[0], pkg.player_id[1]);
}

void NetClient::handleRemoteGameState(const uint8_t* data, size_t dataLength) // for other clients
{
    Net_GameStatePackage pkg;
    memcpy(&pkg, data, sizeof(Net_GameStatePackage));

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.getPlayerCoord(p, netplay.latest_playerdata.player_x[p], netplay.latest_playerdata.player_y[p]);
        pkg.getPlayerVel(p, netplay.latest_playerdata.player_xvel[p], netplay.latest_playerdata.player_yvel[p]);
        pkg.getPlayerKeys(p, &netplay.latest_playerdata.player_input[p]);
    }

    netplay.gamestate_changed = true;
}

/****************
    Listening
****************/

// This client sucesfully connected to a host
void NetClient::onConnect(NetPeer* newPeer)
{
    assert(newPeer);

    // TODO: add state checks
    if (!foreign_lobbyserver) {
        foreign_lobbyserver = newPeer;

        Net_ClientConnectionPackage message(netplay.myPlayerName);
        sendMessageToLobbyServer(&message, sizeof(Net_ClientConnectionPackage));
    }
    else if (!foreign_gamehost) {
        foreign_gamehost = newPeer;
    }
    else {
        // The client must not have more than 2 connections
        newPeer->disconnect();
        assert(0);
    }
}

void NetClient::onDisconnect(NetPeer& client)
{
    // Beware of the null pointers

    if (foreign_gamehost) {
        if (client == *foreign_gamehost) {
            printf("[net] Disconnected from game host.\n");
            return;
        }
    }

    if (foreign_lobbyserver) {
        assert(client == *foreign_lobbyserver);
        printf("[net] Disconnected from lobby server.\n");
    }
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

    // TODO: Add state checks too.
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

        case NET_NOTICE_ROOM_CHAT_MSG:
            handleRoomChatMessage(data, dataLength);
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
        // Pre-game
        //
        case NET_L2P_GAMEHOST_INFO:
            handleRoomStartMessage(data, dataLength);
            break;

        case NET_L2G_CLIENTS_INFO:
            handleExpectedClientsMessage(data, dataLength);
            break;

        case NET_G2P_MAP:
            handleMapSyncMessage(data, dataLength);
            break;

        case NET_G2P_SYNC:
            handleStartSyncMessage(data, dataLength);
            break;

        case NET_G2E_GAME_START:
            handleGameStartMessage();
            break;

        //
        // Game
        //

        case NET_G2P_GAME_STATE:
            handleRemoteGameState(data, dataLength);
            break;

        case NET_G2P_START_POWERUP:
            handlePowerupStart(data, dataLength);
            break;

        /*case NET_G2P_TRIGGER_POWERUP:
            handlePowerupTrigger(data, dataLength);
            break;*/

        case NET_G2P_TRIGGER_MAPCOLL:
            handleMapCollision(data, dataLength);
            break;

        case NET_G2P_TRIGGER_P2PCOLL:
            handleP2PCollision(data, dataLength);
            break;

        //
        // Default
        //

        default:
            printf("Unknown: ");
            for (unsigned a = 0; a < dataLength; a++)
                printf("%3d ", data[a]);
            printf("\n");
            return; // do not set as last message
    }

    setAsLastReceivedMessage(packageType);
}

/****************************
    Network Communication
****************************/

bool NetClient::connectLobby(const char* hostname, const uint16_t port)
{
    netplay.connectSuccessful = false;
    if (!networkHandler.connectToLobbyServer(hostname, port))
        return false;

    return true;
    // now we wait for CONNECT_OK
    // connectSuccessful will be set to 'true' there
}

bool NetClient::connectGameHost(const char* hostname, const uint16_t port)
{
    netplay.connectSuccessful = false;
    if (!networkHandler.connectToForeignGameHost(hostname, port))
        return false;

    return true;

    // now we wait for sync package
}

bool NetClient::sendTo(NetPeer*& peer, const void* data, int dataLength, bool reliable)
{
    assert(peer);
    assert(data);
    assert(dataLength >= 3);

    if (reliable) {
        if (!peer->send(data, dataLength))
            return false;
    }
    else {
        if (!peer->sendReliable(data, dataLength))
            return false;
    }

    setAsLastSentMessage(((uint8_t*)data)[2]);
    return true;
}

bool NetClient::sendMessageToLobbyServer(const void* data, int dataLength)
{
    return sendTo(foreign_lobbyserver, data, dataLength, true);
}

bool NetClient::sendMessageToGameHost(const void* data, int dataLength)
{
    return sendTo(foreign_gamehost, data, dataLength);
}

bool NetClient::sendMessageToGameHostReliable(const void* data, int dataLength)
{
    return sendTo(foreign_gamehost, data, dataLength, true);
}

void NetClient::setAsLastSentMessage(uint8_t packageType)
{
    //printf("setAsLastSentMessage: %d.\n", packageType);
    lastSentMessage.packageType = packageType;
    lastSentMessage.timestamp = SDL_GetTicks();
}

void NetClient::setAsLastReceivedMessage(uint8_t packageType)
{
    //printf("setAsLastReceivedMessage: %d.\n", packageType);
    lastReceivedMessage.packageType = packageType;
    lastReceivedMessage.timestamp = SDL_GetTicks();
}




void Net_PlayerData::copyFromLocal()
{
    for (uint8_t p = 0; p < list_players_cnt; p++) {
        player_x[p] = list_players[p]->fx;
        player_y[p] = list_players[p]->fy;
        player_xvel[p] = list_players[p]->velx;
        player_yvel[p] = list_players[p]->vely;
        player_input[p] = game_values.playerInput.outputControls[p];
    }
}





NetGameHost::NetGameHost()
    : active(false)
    , foreign_lobbyserver(NULL)
    , expected_client_count(0)
    , next_free_client_slot(0)
{
    //printf("NetGameHost::ctor\n");
    for (short p = 0; p < 3; p++) {
        clients[p] = NULL;
    }
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
    printf("[net] GameHost started.\n");
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

    foreign_lobbyserver = NULL; // NetClient may still be connected
    next_free_client_slot = 0;
    expected_client_count = 0;
    for (short p = 0; p < 3; p++) {
        if (clients[p]) {
            clients[p]->disconnect();
            clients[p] = NULL;
        }
        expected_clients[p].reset();
    }

    networkHandler.gamehost_shutdown();
    active = false;
    printf("[net] GameHost stopped.\n");
}

void NetGameHost::cleanup()
{
    //printf("NetGameHost::cleanup\n");
    stop();
}

// A client connected to this host
void NetGameHost::onConnect(NetPeer* new_player)
{
    // GameHost must start after setExpectedPlayers
    assert(expected_client_count > 0);
    assert(clients[0] != new_player);
    assert(clients[1] != new_player);
    assert(clients[2] != new_player);

    if (next_free_client_slot >= 3) {
        new_player->disconnect();
        printf("[warning] More than 3 players want to join.\n");
    }

    // Validation
    // Is this one of the clients we're expecting to join?
    RawPlayerAddress incoming;
    incoming.host = new_player->addressHost();
    incoming.port = new_player->addressPort();

    bool valid_address = false;
    for (unsigned short c = 0; c < expected_client_count && !valid_address; c++) {
        if (incoming == expected_clients[c])
            valid_address = true;
    }

    // Intruder alert
    if (!valid_address) {
        printf("[warning] An unexpected client wants to join the game.\n");
        new_player->disconnect();
        return;
    }

    // Succesful connection
    assert(clients[next_free_client_slot] == NULL);
    clients[next_free_client_slot] = new_player;

    next_free_client_slot++; // placing this before printf makes the log nicer
    printf("%d/%d player connected.\n", next_free_client_slot, expected_client_count);

    // When all players connected,
    // sychronize them (eg. same RNG seed)
    if (next_free_client_slot == expected_client_count) {
        sendMapSyncMessages();
    }
}

void NetGameHost::onDisconnect(NetPeer& player)
{
    printf("onDisconnect!\n");
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
        case NET_P2G_MAP_OK:
            handleMapOKMessage(player, data, dataLength);
            break;

        case NET_P2G_SYNC_OK:
            handleSyncOKMessage(player, data, dataLength);
            break;

        case NET_P2G_LOCAL_KEYS:
            handleRemoteInput(player, data, dataLength);
            break;

        case NET_P2G_REQ_POWERUP:
            handlePowerupRequest(player, data, dataLength);
            break;

        default:
            printf("GH Unknown: ");
            for (unsigned a = 0; a < dataLength; a++)
                printf("%3d ", data[a]);
            printf("\n");
            return; // do not set as last message
    }
}

void NetGameHost::sendStartRoomMessage()
{
    assert(foreign_lobbyserver);

    Net_StartRoomPackage pkg;
    foreign_lobbyserver->sendReliable(&pkg, sizeof(Net_StartRoomPackage));
    setAsLastSentMessage(pkg.packageType);
}

void NetGameHost::sendMapSyncMessages()
{
    assert(clients[0] || clients[1] || clients[2]);
    printf("[net] Sending map: %s\n", netplay.mapfilepath.c_str());
    assert(netplay.mapfilepath.length() > 4);

    uint8_t* data_buffer = NULL;
    size_t data_size = 0;

    // NOTE: This is just an alert Make sure you didn't broke something by changing the package headers!
    // You can safely update this line then.
    static_assert(sizeof(Net_MessageHeader) == 3, "The size of Net_MessageHeader should be 3");

    if (!FileCompressor::compress(netplay.mapfilepath, data_buffer, data_size, sizeof(Net_MessageHeader)))
        return;

    assert(data_buffer);
    assert(data_size > 0);

    Net_MessageHeader header(NET_G2P_MAP);
    memcpy(data_buffer, &header, sizeof(Net_MessageHeader));

    sendMessageToMyPeers(data_buffer, data_size + sizeof(Net_MessageHeader) + 4);
}

void NetGameHost::handleMapOKMessage(const NetPeer& player, const uint8_t* data, size_t dataLength)
{
    assert(player == *clients[0] || player == *clients[1] || player == *clients[2]);

    uint8_t readyPlayerCount = 0;
    for (unsigned short c = 0; c < expected_client_count; c++)
    {
        if (player == *clients[c]) {
            expected_clients[0].map_ok = true;
            printf("  Client %d/%d received the map.\n", c + 1, expected_client_count);
        }

        if (expected_clients[c].map_ok)
            readyPlayerCount++;
    }

    if (readyPlayerCount == expected_client_count) {
        printf("[net] Prepare launching the game...\n");
        sendSyncMessages();
    }
}

void NetGameHost::sendSyncMessages()
{
    assert(clients[0] || clients[1] || clients[2]);

    // send syncronization package
    RandomNumberGenerator::generator().reseed(time(0));
    Net_StartSyncPackage pkg(RANDOM_INT(32767 /* RAND_MAX */));
    sendMessageToMyPeers(&pkg, sizeof(pkg));

    // apply syncronization package on local client
    netplay.client.handleStartSyncMessage((uint8_t*)&pkg, sizeof(pkg));
    netplay.client.setAsLastReceivedMessage(pkg.packageType);
}

void NetGameHost::handleSyncOKMessage(const NetPeer& player, const uint8_t* data, size_t dataLength)
{
    assert(player == *clients[0] || player == *clients[1] || player == *clients[2]);

    uint8_t readyPlayerCount = 0;
    for (unsigned short c = 0; c < expected_client_count; c++)
    {
        if (player == *clients[c]) {
            expected_clients[0].sync_ok = true;
            printf("Client %d/%d ready.\n", c + 1, expected_client_count);
        }

        if (expected_clients[c].sync_ok)
            readyPlayerCount++;
    }

    if (readyPlayerCount == expected_client_count) {
        sendStartGameMessage();
    }
}

void NetGameHost::setExpectedPlayers(uint8_t count, uint32_t* hosts, uint16_t* ports)
{
    assert(count);
    assert(hosts);
    assert(ports);

    expected_client_count = count;
    for (unsigned short c = 0; c < count; c++) {
        assert(hosts[c]);
        assert(ports[c]);
        expected_clients[c].host = hosts[c];
        expected_clients[c].port = ports[c];
    }

    printf("Game starts soon, waiting for %d players.\n", expected_client_count);
}

void NetGameHost::sendStartGameMessage()
{
    assert(foreign_lobbyserver);
    assert(clients[0] || clients[1] || clients[2]);

    Net_StartGamePackage pkg;
    foreign_lobbyserver->sendReliable(&pkg, sizeof(Net_StartGamePackage));
    sendMessageToMyPeers(&pkg, sizeof(Net_StartGamePackage));

    netplay.client.handleGameStartMessage();
    netplay.client.setAsLastReceivedMessage(pkg.packageType);
}

void NetGameHost::sendCurrentGameState()
{
    Net_GameStatePackage pkg;

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.setPlayerCoord(p, list_players[p]->fx, list_players[p]->fy);
        pkg.setPlayerVel(p, list_players[p]->velx, list_players[p]->vely);
        pkg.setPlayerKeys(p, list_players[p]->playerKeys);
    }

    for (int c = 0; c < 3; c++) {
        if (clients[c])
            clients[c]->send(&pkg, sizeof(Net_GameStatePackage));
    }
}

void NetGameHost::handleRemoteInput(const NetPeer& player, const uint8_t* data, size_t dataLength) // only for room host
{
    assert(player == *clients[0] || player == *clients[1] || player == *clients[2]);

    Net_ClientInputPackage* pkg = (Net_ClientInputPackage*) data; // TODO: check size!

    for (unsigned short c = 0; c < expected_client_count; c++) {
        if (player == *clients[c]) {
            //printf("yey\n");

            // TODO: does this work if GH leaves?
            pkg->readKeys(&netplay.netPlayerInput.outputControls[c + 1]);


            /*printf("INPUT %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d %d:%d\n",
                netplay.netPlayerInput.outputControls[c + 1].keys[0].fDown,
                netplay.netPlayerInput.outputControls[c + 1].keys[0].fPressed,
                netplay.netPlayerInput.outputControls[c + 1].keys[1].fDown,
                netplay.netPlayerInput.outputControls[c + 1].keys[1].fPressed,
                netplay.netPlayerInput.outputControls[c + 1].keys[2].fDown,
                netplay.netPlayerInput.outputControls[c + 1].keys[2].fPressed,
                netplay.netPlayerInput.outputControls[c + 1].keys[3].fDown,
                netplay.netPlayerInput.outputControls[c + 1].keys[3].fPressed,
                netplay.netPlayerInput.outputControls[c + 1].keys[4].fDown,
                netplay.netPlayerInput.outputControls[c + 1].keys[4].fPressed,
                netplay.netPlayerInput.outputControls[c + 1].keys[5].fDown,
                netplay.netPlayerInput.outputControls[c + 1].keys[5].fPressed,
                netplay.netPlayerInput.outputControls[c + 1].keys[6].fDown,
                netplay.netPlayerInput.outputControls[c + 1].keys[6].fPressed,
                netplay.netPlayerInput.outputControls[c + 1].keys[7].fDown,
                netplay.netPlayerInput.outputControls[c + 1].keys[7].fPressed);*/

            return;
        }
    }
}

// The boss wants to use powerup, so notifies the clients
void NetGameHost::sendPowerupStart()
{
    assert(netplay.theHostIsMe);
    assert(list_players[netplay.remotePlayerNumber]->powerupused >= 0);

    Net_StartPowerupPackage pkg(netplay.remotePlayerNumber,
        list_players[netplay.remotePlayerNumber]->powerupused, 0);
    sendMessageToMyPeers(&pkg, sizeof(Net_StartPowerupPackage));

    netplay.waitingForPowerupTrigger = false;
    printf("[net] P%d (host) used powerup %d\n", pkg.player_id, pkg.powerup_id);
}

// A player wants to use a stored powerup
void NetGameHost::handlePowerupRequest(const NetPeer& player, const uint8_t* data, size_t dataLength)
{
    assert(player == *clients[0] || player == *clients[1] || player == *clients[2]);

    // Net_RequestPowerupPackage* in_pkg = (Net_RequestPowerupPackage*) data; // TODO: check size!

    uint8_t playerID = 0xFF;
    for (unsigned short c = 0; c < expected_client_count; c++) {
        if (player == *clients[c]) {
            // TODO: does this work if GH leaves?
            playerID = c + 1;
            break;
        }
    }

    assert(playerID < 4);
    assert(list_players[playerID]);
    if (list_players[playerID]->powerupused < 0)
        return;

    Net_StartPowerupPackage pkg(playerID, list_players[playerID]->powerupused, player.averageRTT() / 2);
    for (unsigned short c = 0; c < expected_client_count; c++) {
        if (clients[c] && player != *clients[c]) {
            clients[c]->sendReliable(&pkg, sizeof(Net_StartPowerupPackage));
        }
    }

    // FIXME: this is almost the same as in the regular client code
    unsigned missed_frames = pkg.delay / WAITTIME;
    list_players[pkg.player_id]->powerupradius = 100.0f;
    for (; missed_frames > 0; missed_frames--) {
        list_players[pkg.player_id]->powerupradius -= (float)game_values.storedpowerupdelay / 2.0f;
    }

    printf("[net] P%d used powerup %d\n", pkg.player_id, pkg.powerup_id);
}

/*void NetGameHost::sendPowerupTriggerIfReady()
{
    assert(netplay.theHostIsMe);
    assert(list_players[netplay.remotePlayerNumber]->powerupused >= 0);
}*/

void NetGameHost::sendMapCollisionEvent(CPlayer& player)
{
    assert(netplay.theHostIsMe);

    Net_MapCollisionPackage pkg(player);
    sendMessageToMyPeers(&pkg, sizeof(Net_MapCollisionPackage));

    printf("[net] P%d collided with a map block\n", pkg.player_id);
}

void NetGameHost::sendP2PCollisionEvent(CPlayer& p1, CPlayer& p2)
{
    assert(netplay.theHostIsMe);
    assert(!p1.isdead());
    assert(!p2.isdead());

    Net_P2PCollisionPackage pkg(p1, p2);
    sendMessageToMyPeers(&pkg, sizeof(Net_P2PCollisionPackage));

    printf("[net] P%d collided with P%d\n", p1.getGlobalID(), p2.getGlobalID());
}

bool NetGameHost::sendMessageToMyPeers(const void* data, size_t dataLength)
{
    assert(data);
    assert(dataLength >= 3);

    for (unsigned short c = 0; c < 3; c++) {
        if (clients[c])
            clients[c]->sendReliable(data, dataLength);
    }

    setAsLastSentMessage(((uint8_t*)data)[2]);
    return true;
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
