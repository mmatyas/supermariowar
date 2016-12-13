#include "net.h"

#include "GlobalConstants.h"
#include "GameMode.h"
#include "GameModeSettings.h"
#include "GameValues.h"
#include "FileList.h"
#include "path.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "ui/NetworkListScroll.h"

#include "network/FileCompressor.h"
#include "network/ProtocolGamePackages.h"
#include "network/NetConfigManager.h"

// define platform guards here
#ifdef NETWORK_DISABLED
    #include "platform/network/null/NetworkLayerNULL.h"
    #define NetworkHandler NetworkLayerNULL
#else
    #include "platform/network/enet/NetworkLayerENet.h"
    #define NetworkHandler NetworkLayerENet
#endif

#include <algorithm>
#include <cassert>
#include <cstring>
#include <ctime>
#include <string>

NetworkHandler networkHandler;
Networking netplay;

extern CGameValues game_values;
extern CPlayer* list_players[4];
extern short list_players_cnt;

// used for setting netplay player skins
extern SkinList *skinlist;
extern CResourceManager* rm;

extern short currentgamemode;
extern CGameMode * gamemodes[GAMEMODE_LAST];

short backup_playercontrol[4];

Net_PlayerData::Net_PlayerData()
    : x(0), y(0), xvel(0), yvel(0)
{}

Net_IndexedPlayerData::Net_IndexedPlayerData(uint8_t id)
    : input_id(id)
{}

union GameModeSettingsUnion {
    ClassicGameModeSettings classic;
    FragGameModeSettings frag;
    TimeGameModeSettings time;
    JailGameModeSettings jail;
    CoinGameModeSettings coins;
    StompGameModeSettings stomp;
    EggGameModeSettings egg;
    FlagGameModeSettings flag;
    ChickenGameModeSettings chicken;
    TagGameModeSettings tag;
    StarGameModeSettings star;
    DominationGameModeSettings domination;
    KingOfTheHillModeSettings kingofthehill;
    RaceGameModeSettings race;
    FrenzyGameModeSettings frenzy;
    SurvivalGameModeSettings survival;
    GreedGameModeSettings greed;
    HealthGameModeSettings health;
    CollectionGameModeSettings collection;
    ChaseGameModeSettings chase;
    ShyGuyTagGameModeSettings shyguytag;

    GameModeSettingsUnion() { memset(this, 0, sizeof(GameModeSettingsUnion)); }

    void set_global(GameModeType mode) {
        switch (mode) {
        case game_mode_classic:     game_values.gamemodesettings.classic = classic; break;
        case game_mode_frag:        game_values.gamemodesettings.frag = frag; break;
        case game_mode_timelimit:   game_values.gamemodesettings.time = time; break;
        case game_mode_jail:        game_values.gamemodesettings.jail = jail; break;
        case game_mode_coins:       game_values.gamemodesettings.coins = coins; break;
        case game_mode_stomp:       game_values.gamemodesettings.stomp = stomp; break;
        case game_mode_eggs:        game_values.gamemodesettings.egg = egg; break;
        case game_mode_ctf:         game_values.gamemodesettings.flag = flag; break;
        case game_mode_chicken:     game_values.gamemodesettings.chicken = chicken; break;
        case game_mode_tag:         game_values.gamemodesettings.tag = tag; break;
        case game_mode_star:        game_values.gamemodesettings.star = star; break;
        case game_mode_domination:  game_values.gamemodesettings.domination = domination; break;
        case game_mode_koth:        game_values.gamemodesettings.kingofthehill = kingofthehill; break;
        case game_mode_race:        game_values.gamemodesettings.race = race; break;
        case game_mode_frenzy:      game_values.gamemodesettings.frenzy = frenzy; break;
        case game_mode_survival:    game_values.gamemodesettings.survival = survival; break;
        case game_mode_greed:       game_values.gamemodesettings.greed = greed; break;
        case game_mode_health:      game_values.gamemodesettings.health = health; break;
        case game_mode_collection:  game_values.gamemodesettings.collection = collection; break;
        case game_mode_chase:       game_values.gamemodesettings.chase = chase; break;
        case game_mode_shyguytag:   game_values.gamemodesettings.shyguytag = shyguytag; break;
        default:
            assert(false);
        }
    }

    void read_global(GameModeType mode) {
        switch (mode) {
        case game_mode_classic:     classic = game_values.gamemodesettings.classic; break;
        case game_mode_frag:        frag = game_values.gamemodesettings.frag; break;
        case game_mode_timelimit:   time = game_values.gamemodesettings.time; break;
        case game_mode_jail:        jail = game_values.gamemodesettings.jail; break;
        case game_mode_coins:       coins = game_values.gamemodesettings.coins; break;
        case game_mode_stomp:       stomp = game_values.gamemodesettings.stomp; break;
        case game_mode_eggs:        egg = game_values.gamemodesettings.egg; break;
        case game_mode_ctf:         flag = game_values.gamemodesettings.flag; break;
        case game_mode_chicken:     chicken = game_values.gamemodesettings.chicken; break;
        case game_mode_tag:         tag = game_values.gamemodesettings.tag; break;
        case game_mode_star:        star = game_values.gamemodesettings.star; break;
        case game_mode_domination:  domination = game_values.gamemodesettings.domination; break;
        case game_mode_koth:        kingofthehill = game_values.gamemodesettings.kingofthehill; break;
        case game_mode_race:        race = game_values.gamemodesettings.race; break;
        case game_mode_frenzy:      frenzy = game_values.gamemodesettings.frenzy; break;
        case game_mode_survival:    survival = game_values.gamemodesettings.survival; break;
        case game_mode_greed:       greed = game_values.gamemodesettings.greed; break;
        case game_mode_health:      health = game_values.gamemodesettings.health; break;
        case game_mode_collection:  collection = game_values.gamemodesettings.collection; break;
        case game_mode_chase:       chase = game_values.gamemodesettings.chase; break;
        case game_mode_shyguytag:   shyguytag = game_values.gamemodesettings.shyguytag; break;
        default:
            assert(false);
        }
    }
};

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
    netplay.allowMapCollisionEvent = false;

    if (!networkHandler.init())
        return false;

    if (!netplay.client.init())
        return false;

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
    netplay.gamestate_changed = false;
    netplay.frames_since_last_gamestate = 0;
    std::fill_n(netplay.player_disconnected, 4, false);

    COutputControl nullinput = COutputControl {0}; // set every field to false
    for (uint8_t p = 0; p < 4; p++) {
        // backup singleplayer settings
        backup_playercontrol[p] = game_values.playercontrol[p];

        netplay.latest_playerdata.player_input[p].clear();
        netplay.latest_playerdata.player_input[p].push_back(nullinput);

        netplay.local_input_buffer.clear();
        netplay.remote_input_buffer[p].clear();
    }

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
    : foreign_lobbyserver(NULL)
    , foreign_gamehost(NULL)
    , uiRoomList(NULL)
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
    NetPkgs::ClientDisconnection msg;
    sendMessageToLobbyServer(&msg, sizeof(NetPkgs::ClientDisconnection));
}

void NetClient::handleServerinfoAndClose(const uint8_t* data, size_t dataLength)
{
    // TODO: Remove copies.

    NetPkgs::ServerInfo serverInfo;
    memcpy(&serverInfo, data, sizeof(NetPkgs::ServerInfo));

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
    NetPkgs::RoomList msg;
    sendMessageToLobbyServer(&msg, sizeof(NetPkgs::RoomList));

    netplay.currentRooms.clear();
    if (uiRoomList)
        uiRoomList->Clear();
}

void NetClient::handleNewRoomListEntry(const uint8_t* data, size_t dataLength)
{
    NetPkgs::RoomInfo roomInfo;
    memcpy(&roomInfo, data, sizeof(NetPkgs::RoomInfo));

    RoomListEntry newRoom;
    newRoom.roomID = roomInfo.roomID;
    newRoom.name = roomInfo.name;
    newRoom.playerCount = roomInfo.currentPlayerCount;
    netplay.currentRooms.push_back(newRoom);
    printf("  Incoming room entry: [%u] %s (%d/4)\n", newRoom.roomID, newRoom.name.c_str(), newRoom.playerCount);

    if (uiRoomList) {
        // TODO: strings would be better
        char playerCountString[4] = {'0' + roomInfo.currentPlayerCount, '/', '4', '\0'};
        uiRoomList->Add(newRoom.name, playerCountString);
    }
}

void NetClient::sendCreateRoomMessage()
{
    NetPkgs::NewRoom msg(netplay.newroom_name, netplay.newroom_password);
    msg.gamemodeID = currentgamemode;
    game_values.gamemode = gamemodes[currentgamemode];
    msg.gamemodeGoal = game_values.gamemode->goal;

    sendMessageToLobbyServer(&msg, sizeof(NetPkgs::NewRoom));
    netplay.operationInProgress = true;
}

void NetClient::handleRoomCreatedMessage(const uint8_t* data, size_t dataLength)
{
    printf("room created!\n");
    NetPkgs::NewRoomCreated pkg;
    memcpy(&pkg, data, sizeof(NetPkgs::NewRoomCreated));

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

    sendMapChangeMessage();
    sendGameModeSettingsChangeMessage();

    local_gamehost.start(foreign_lobbyserver);
}

void NetClient::sendJoinRoomMessage()
{
    assert(netplay.selectedRoomIndex < netplay.currentRooms.size());

    printf("currentRooms[%d] = {id=%d, name='%s', cnt=%d}, össz: %lu\n",
        netplay.selectedRoomIndex,
        netplay.currentRooms.at(netplay.selectedRoomIndex).roomID,
        netplay.currentRooms.at(netplay.selectedRoomIndex).name.c_str(),
        netplay.currentRooms.at(netplay.selectedRoomIndex).playerCount,
        netplay.currentRooms.size());

    // TODO: implement password
    NetPkgs::JoinRoom msg(netplay.currentRooms.at(netplay.selectedRoomIndex).roomID, "");
    sendMessageToLobbyServer(&msg, sizeof(NetPkgs::JoinRoom));
    netplay.operationInProgress = true;
}

void NetClient::sendLeaveRoomMessage()
{
    local_gamehost.stop();

    NetPkgs::LeaveRoom msg;
    sendMessageToLobbyServer(&msg, sizeof(NetPkgs::LeaveRoom));
}

void NetClient::handleRoomChangedMessage(const uint8_t* data, size_t dataLength)
{
    //printf("ROOM CHANGED\n");
    NetPkgs::CurrentRoom pkg;
    memcpy(&pkg, data, sizeof(NetPkgs::CurrentRoom));

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

    // set the game mode
    assert(pkg.gamemodeID < GAMEMODE_LAST);
    currentgamemode = pkg.gamemodeID;
    game_values.gamemode = gamemodes[currentgamemode];
    game_values.gamemode->goal = pkg.gamemodeGoal;

    printf("  Game mode #%d: %s with %s: %d\n",
        currentgamemode, gamemodes[currentgamemode]->GetModeName(),
        gamemodes[currentgamemode]->GetGoalName(), game_values.gamemode->goal);

    netplay.theHostIsMe = false;
    netplay.currentRoom.hostPlayerNumber = pkg.hostPlayerNumber;
    netplay.remotePlayerNumber = pkg.remotePlayerNumber;
    netplay.hostPlayerNumber = pkg.hostPlayerNumber;
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

    NetPkgs::RoomChatMsg pkg(netplay.remotePlayerNumber, message);
    sendMessageToLobbyServer(&pkg, sizeof(NetPkgs::RoomChatMsg));
}

void NetClient::sendMapChangeMessage()
{
    printf("[net] Sending map: %s\n", netplay.mapfilepath.c_str());
    assert(netplay.mapfilepath.length() > 4);

    // NOTE: This is just an alert
    // Make sure you didn't broke something by changing the package headers!
    // You can safely update this line then.
    static_assert(sizeof(NetPkgs::MessageHeader) == 3, "The size of Net_MessageHeader should be 3");

    CompressedData compressed = FileCompressor::compress(netplay.mapfilepath, sizeof(NetPkgs::MessageHeader));
    assert(compressed.data);
    assert(compressed.size > 4 + sizeof(NetPkgs::MessageHeader));
    if (!compressed.is_valid())
        return;
    if (compressed.size <= 4 + sizeof(NetPkgs::MessageHeader))
        return;

    NetPkgs::MessageHeader header(NET_NOTICE_MAP_CHANGE);
    memcpy(compressed.data, &header, sizeof(NetPkgs::MessageHeader));

    sendMessageToLobbyServer(compressed.data, compressed.size);
}

void NetClient::handleMapChangeMessage(const uint8_t* data, size_t dataLength)
{
    if (dataLength <= sizeof(NetPkgs::MessageHeader) + 4 /* un-/compressed size 2*2B */
        || dataLength > 20000
        || !data) {
        printf("[error] Corrupt map arrived\n");
        return;
    }

    // read
    NetPkgs::MessageHeader pkg(0);
    memcpy(&pkg, data, sizeof(NetPkgs::MessageHeader));

    uint16_t full_size, compressed_size;
    memcpy(&full_size, data + sizeof(NetPkgs::MessageHeader), 2);
    memcpy(&compressed_size, data + sizeof(NetPkgs::MessageHeader) + 2, 2);

    printf("[net] Map arrived (C:%d unC:%d)\n", compressed_size, full_size);

    netplay.mapfilepath = GetHomeDirectory() + "net_last.map";
    if (!FileCompressor::decompress(data + sizeof(NetPkgs::MessageHeader), netplay.mapfilepath))
        return;
}

void NetClient::sendGameModeSettingsChangeMessage()
{
    size_t data_size = sizeof(NetPkgs::MessageHeader) + sizeof(GameModeSettingsUnion);
    CompressedData blob((uint8_t*) calloc(data_size, 1), data_size);

    NetPkgs::MessageHeader header(NET_NOTICE_GAMEMODESETTINGS);
    memcpy(blob.data, &header, sizeof(NetPkgs::MessageHeader));

    GameModeSettingsUnion modesettings;
    modesettings.read_global((GameModeType) currentgamemode);
    memcpy(blob.data + sizeof(NetPkgs::MessageHeader), &modesettings, sizeof(GameModeSettingsUnion));

    sendMessageToLobbyServer(blob.data, blob.size);
}

void NetClient::handleGameModeSettingsChangeMessage(const uint8_t* data, size_t dataLength)
{
    if (dataLength != sizeof(NetPkgs::MessageHeader) + sizeof(GameModeSettingsUnion)
        || !data) {
        printf("[error] Corrupt game mode settings arrived\n");
        return;
    }

    // read
    NetPkgs::MessageHeader pkg(0);
    memcpy(&pkg, data, sizeof(NetPkgs::MessageHeader));

    GameModeSettingsUnion modesettings;
    memcpy(&modesettings, data + sizeof(NetPkgs::MessageHeader), sizeof(GameModeSettingsUnion));
    modesettings.set_global((GameModeType) currentgamemode);

    printf("[net] Game mode settings changed\n");
}

void NetClient::handleRoomChatMessage(const uint8_t* data, size_t dataLength)
{
    NetPkgs::RoomChatMsg pkg;
    memcpy(&pkg, data, sizeof(NetPkgs::RoomChatMsg));

    assert(pkg.senderNum < 4);
    printf("Not implemented: [net] %s: %s\n",
        netplay.currentRoom.playerNames[pkg.senderNum].c_str(), pkg.message);
}

void NetClient::sendSkinChange()
{
    assert(strlen(skinlist->GetIndex(game_values.skinids[netplay.remotePlayerNumber])) > 4);
    std::string skinpath(skinlist->GetIndex(game_values.skinids[netplay.remotePlayerNumber]));
    printf("[net] Sending skin: %s\n", skinpath.c_str());

    // NOTE: see sendMapChangeMessage() for similar code
    static_assert(sizeof(NetPkgs::MessageHeader) == 3, "The size of Net_MessageHeader should be 3");

    CompressedData compressed = FileCompressor::compress(skinpath, sizeof(NetPkgs::MessageHeader) + 1);
    assert(compressed.data);
    assert(compressed.size > 1 + sizeof(NetPkgs::MessageHeader));
    if (!compressed.is_valid())
        return;
    if (compressed.size <= 1 + sizeof(NetPkgs::MessageHeader))
        return;

    NetPkgs::MessageHeader header(NET_NOTICE_SKIN_CHANGE);
    memcpy(compressed.data, &header, sizeof(NetPkgs::MessageHeader));
    compressed.data[sizeof(NetPkgs::MessageHeader)] = 0xFF; // the player's id in a room (0-3), or 0xFF

    sendMessageToLobbyServer(compressed.data, compressed.size);
}

void NetClient::handleSkinChangeMessage(const uint8_t* data, size_t dataLength)
{
    if (dataLength <= sizeof(NetPkgs::MessageHeader) + 5 /* player num 1B + un-/compressed size 2*2B */
        || dataLength > 20000
        || !data) {
        printf("[error] Corrupt skin arrived\n");
        return;
    }

    // read
    NetPkgs::MessageHeader pkg(0);
    memcpy(&pkg, data, sizeof(NetPkgs::MessageHeader));

    int playerID = data[3];
    if (playerID > 3) {
        printf("[error] Corrupt skin arrived: bad player id\n");
        return;
    }

    uint16_t full_size, compressed_size;
    memcpy(&full_size, data + sizeof(NetPkgs::MessageHeader) + 1, 2);
    memcpy(&compressed_size, data + sizeof(NetPkgs::MessageHeader) + 3, 2);

    printf("[net] Skin arrived (from: %d, C:%d unC:%d)\n", playerID, compressed_size, full_size);

    std::ostringstream path;
    path << GetHomeDirectory() << "net_skin" << playerID << ".bmp";
    if (!FileCompressor::decompress(data + sizeof(NetPkgs::MessageHeader) + 1, path.str()))
        return;

    if (!rm->LoadMenuSkin(playerID, path.str(), playerID, false)) {
        printf("[warning] Could not load netplay skin of player %d, using default\n", playerID);
        rm->LoadMenuSkin(playerID, game_values.skinids[playerID], playerID, false);
    }
}

/****************************
    Phase 2.5: Pre-game
****************************/

// This package goes to normal players
void NetClient::handleRoomStartMessage(NetPeer& client, const uint8_t* data, size_t dataLength)
{
    NetPkgs::GameHostInfo pkg(0);
    memcpy(&pkg, data, sizeof(NetPkgs::GameHostInfo));

    // If the server and a player is on the same machine,
    // then the player's address will be 127.0.0.1.
    // Use the server's address in this case.
    if (pkg.host == 0x100007F)
        pkg.host = client.addressHost();

    // pretty print
    uint8_t* host_bytes = (uint8_t*) &pkg.host;
    char host_str[17];
    sprintf(host_str, "%d.%d.%d.%d",
        host_bytes[0], host_bytes[1], host_bytes[2], host_bytes[3]);

    printf("[net] Connecting to game host... [%s:%d]\n", host_str, NET_GAMEHOST_PORT);
    if (!connectGameHost(host_str)) {
        printf("[net][error] Could not connect to game host.\n");
        return;
    }

    netplay.operationInProgress = true;
}

// This package goes to the game host player
void NetClient::handleExpectedClientsMessage(NetPeer& client, const uint8_t* data, size_t dataLength)
{
    NetPkgs::PlayerInfo pkg;
    memcpy(&pkg, data, sizeof(NetPkgs::PlayerInfo));

    uint8_t playerCount = 0;
    uint32_t hosts[3];
    uint16_t ports[3];

    printf("[net] Expecting the following clients:\n");
    for (uint8_t p = 0; p < 3; p++) {
        if (pkg.host[p]) { // this is 0 for empty player slots
            hosts[playerCount] = pkg.host[p];
            ports[playerCount] = pkg.port[p];

            // If the server and a player is on the same machine,
            // then the player's address will be 127.0.0.1.
            // Use the server's address in this case.
            if (hosts[playerCount] == 0x100007F)
                hosts[playerCount] = client.addressHost();

            uint8_t* host_bytes = (uint8_t*) &pkg.host[p];
            char host_str[17];
            sprintf(host_str, "%d.%d.%d.%d",
                host_bytes[0], host_bytes[1], host_bytes[2], host_bytes[3]);

            printf("  Client %d is %s:%u\n", p, host_str, pkg.port[p]);
            playerCount++;
        }
    }

    local_gamehost.setExpectedPlayers(playerCount, hosts, ports);
}

void NetClient::handleStartSyncMessage(const uint8_t* data, size_t dataLength)
{
    // read
    NetPkgs::StartSync pkg(0);
    memcpy(&pkg, data, sizeof(NetPkgs::StartSync));

    // apply
    printf("reseed: %d\n", pkg.commonRandomSeed);
    RandomNumberGenerator::generator().reseed(pkg.commonRandomSeed);

    std::fill_n(netplay.player_disconnected, 4, false);
    netplay.last_confirmed_input = 0xFF;
    netplay.current_input_counter = 0;
    std::fill_n(netplay.local_playerdata_store_time, 256, std::chrono::system_clock::now());

    // respond
    if (netplay.theHostIsMe)
        setAsLastSentMessage(NET_P2G_SYNC_OK);
    else {
        NetPkgs::SyncOK respond_pkg;
        sendMessageToGameHostReliable(&respond_pkg, sizeof(NetPkgs::SyncOK));
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

    NetPkgs::LeaveGame pkg;
    if (netplay.theHostIsMe) {
        // disconnect all players
        local_gamehost.sendMessageToMyPeers(&pkg, sizeof(NetPkgs::LeaveGame));
        local_gamehost.update();
        // stop game host
        local_gamehost.stop();

        //apply to local client
        setAsLastReceivedMessage(pkg.packageType);
    } else {
        // diconnect from foreign game host
        sendMessageToGameHostReliable(&pkg, sizeof(NetPkgs::LeaveGame));
        assert(!local_gamehost.active);
    }

    //state change
    //...
    netplay.gameRunning = false;
}

void NetClient::storeLocalInput()
{
    if (netplay.theHostIsMe)
        netplay.local_input_buffer.push_back(game_values.playerInput.outputControls[0]);
}

void NetClient::sendLocalInput()
{
    if (netplay.theHostIsMe) {
        local_gamehost.sendLocalInput();
    }
    else {
        NetPkgs::ClientInput pkg(&game_values.playerInput.outputControls[0]);
        pkg.input_id = netplay.current_input_counter;
        sendMessageToGameHost(&pkg, sizeof(NetPkgs::ClientInput));
    }

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

    NetPkgs::RequestPowerup pkg;
    sendMessageToGameHostReliable(&pkg, sizeof(NetPkgs::RequestPowerup));
}

void NetClient::handlePowerupStart(const uint8_t* data, size_t dataLength)
{
    NetPkgs::StartPowerup pkg(0xFF, 0, 0);
    memcpy(&pkg, data, sizeof(NetPkgs::StartPowerup));

    assert(pkg.player_id < 4);
    if (pkg.player_id > 3)
        return;

    assert(list_players[pkg.player_id]);
    if (!list_players[pkg.player_id])
        return;

    unsigned missed_frames = pkg.delay;
    if (!netplay.theHostIsMe)
        missed_frames += foreign_gamehost->averageRTT() / 2;
    missed_frames /= WAITTIME; // ms to frames

    // TODO: make this nicer
    // TODO: this might be overriden by a late packet, in tryReleasingPowerup()
    list_players[pkg.player_id]->powerupused = pkg.powerup_id;
    list_players[pkg.player_id]->powerupradius = 100.0f;
    for (; missed_frames > 0; missed_frames--) {
        list_players[pkg.player_id]->powerupradius -= (float)game_values.storedpowerupdelay / 2.0f;
    }

    // TODO: replace this eventually with the powerup trigger package
    list_players[pkg.player_id]->net_waitingForPowerupTrigger = false;
    printf("[net] P%d used powerup %d\n", pkg.player_id, pkg.powerup_id);
}

/*void NetClient::handlePowerupTrigger(const uint8_t* data, size_t dataLength)
{
    NetPkgs::TriggerPowerup pkg(0xFF, 0, 0, 0);
    memcpy(&pkg, data, sizeof(NetPkgs::TriggerPowerup));

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
    NetPkgs::MapCollision pkg;
    memcpy(&pkg, data, sizeof(NetPkgs::MapCollision));

    assert(pkg.player_id < 4);
    if (pkg.player_id > 3)
        return;

    assert(list_players[pkg.player_id]);
    if (!list_players[pkg.player_id])
        return;

    //printf("[net] Map collision triggered for P%d.\n", pkg.player_id);
    float temp_px = list_players[pkg.player_id]->fx;
    float temp_py = list_players[pkg.player_id]->fy;
    float temp_velx = list_players[pkg.player_id]->velx;
    float temp_vely = list_players[pkg.player_id]->vely;
    list_players[pkg.player_id]->setXf(pkg.player_x);
    list_players[pkg.player_id]->setYf(pkg.player_y);
    list_players[pkg.player_id]->velx = pkg.player_xvel;
    list_players[pkg.player_id]->vely = pkg.player_yvel;

    netplay.allowMapCollisionEvent = true;
    list_players[pkg.player_id]->collision_detection_map();
    netplay.allowMapCollisionEvent = false;

    if (list_players[pkg.player_id]->isdead())
        return;

    list_players[pkg.player_id]->setXf(temp_px);
    list_players[pkg.player_id]->setYf(temp_py);
    list_players[pkg.player_id]->velx = temp_velx;
    list_players[pkg.player_id]->vely = temp_vely;

    printf("[net] Map block collision!\n");
}

void NetClient::handleP2PCollision(const uint8_t * data, size_t dataLength)
{
    NetPkgs::P2PCollision pkg;
    memcpy(&pkg, data, sizeof(NetPkgs::P2PCollision));

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

void NetClient::handleRemoteInput(const uint8_t* data, size_t dataLength)
{
    NetPkgs::RemoteInput* pkg = (NetPkgs::RemoteInput*) data;
    COutputControl keys;
    pkg->readKeys(&keys);
    netplay.remote_input_buffer[pkg->playerNumber].push_back(std::make_pair(0, keys));
}

void NetClient::handleRemoteGameState(const uint8_t* data, size_t dataLength) // for other clients
{
    NetPkgs::GameState pkg;
    memcpy(&pkg, data, sizeof(NetPkgs::GameState));

    netplay.previous_playerdata = netplay.latest_playerdata;

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.getPlayerCoord(p, netplay.latest_playerdata.player[p].x, netplay.latest_playerdata.player[p].y);
        pkg.getPlayerVel(p, netplay.latest_playerdata.player[p].xvel, netplay.latest_playerdata.player[p].yvel);
    }

    netplay.gamestate_changed = true;
    netplay.frames_since_last_gamestate = 0;
    netplay.last_confirmed_input = pkg.last_confirmed_local_input_id;
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

        NetPkgs::ClientConnection message(netplay.myPlayerName);
        sendMessageToLobbyServer(&message, sizeof(NetPkgs::ClientConnection));
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
    // Beware of the null pointers!

    if (foreign_gamehost) {
        if (client == *foreign_gamehost) {
            printf("[net] Disconnected from game host.\n");
            netplay.player_disconnected[netplay.remotePlayerNumber] = true;
            return;
        }
    }

    if (foreign_lobbyserver) {
        if (client == *foreign_lobbyserver) {
            printf("[net] Disconnected from lobby server.\n");
            return;
        }
    }

    printf("[net] Client %s disconnected\n", client.addressAsString().c_str());
}

void NetClient::onReceive(NetPeer& client, const uint8_t* data, size_t dataLength)
{
    assert(data);
    assert(dataLength >= 3);

    uint8_t protocolMajor = data[0];
    uint8_t protocolMinor = data[1];
    uint8_t packageType = data[2];
    if (protocolMajor != NET_PROTOCOL_VERSION_MAJOR
        || protocolMinor != NET_PROTOCOL_VERSION_MINOR) {
        printf("Not implemented: bad protocol version\n");
        return;
    }

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
            sendSkinChange();
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

        case NET_NOTICE_ROOM_CHANGE:
            handleRoomChangedMessage(data, dataLength);
            break;

        case NET_NOTICE_MAP_CHANGE:
            handleMapChangeMessage(data, dataLength);
            break;

        case NET_NOTICE_SKIN_CHANGE:
            handleSkinChangeMessage(data, dataLength);
            break;

        case NET_NOTICE_GAMEMODESETTINGS:
            handleGameModeSettingsChangeMessage(data, dataLength);
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
            handleRoomStartMessage(client, data, dataLength);
            break;

        case NET_L2G_CLIENTS_INFO:
            handleExpectedClientsMessage(client, data, dataLength);
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

        case NET_G2P_REMOTE_KEYS:
            handleRemoteInput(data, dataLength);
            break;

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
    if (packageType == NET_RESPONSE_CREATE_OK)
        printf("last: %d, %d\n", lastSentMessage.packageType, lastReceivedMessage.packageType);
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





NetGameHost::NetGameHost()
    : active(false)
    , current_server_tick(0)
    , foreign_lobbyserver(NULL)
    , expected_client_count(0)
    , next_free_client_slot(0)
{
    //printf("NetGameHost::ctor\n");
    for (short p = 0; p < 3; p++) {
        clients[p] = NULL;
        last_processed_input_id[p] = 0xFF;
    }

    preparedMapCollPkg = new NetPkgs::MapCollision();
}

NetGameHost::~NetGameHost()
{
    //printf("NetGameHost::dtor\n");
    cleanup();

    delete preparedMapCollPkg;
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
        last_processed_input_id[p] = 0xFF;
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
        printf("[error] More than 3 players want to join.\n");
        return;
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
        sendSyncMessages();
    }
}

void NetGameHost::onDisconnect(NetPeer& player)
{
    printf("[net] Client %s disconnected\n", player.addressAsString().c_str());
    for (short c = 0; c < expected_client_count; c++) {
        if (clients[c] && *clients[c] == player) {
            netplay.player_disconnected[c + 1] = true;
            return;
        }
    }
}

void NetGameHost::onReceive(NetPeer& player, const uint8_t* data, size_t dataLength)
{
    assert(data);
    assert(dataLength >= 3);

    uint8_t protocolMajor = data[0];
    uint8_t protocolMinor = data[1];
    uint8_t packageType = data[2];
    if (protocolMajor != NET_PROTOCOL_VERSION_MAJOR
        || protocolMinor != NET_PROTOCOL_VERSION_MINOR) {
        printf("Not implemented: bad protocol version\n");
        return;
    }

    switch (packageType)
    {
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

    NetPkgs::StartRoom pkg;
    foreign_lobbyserver->sendReliable(&pkg, sizeof(NetPkgs::StartRoom));
    setAsLastSentMessage(pkg.packageType);
}

void NetGameHost::sendSyncMessages()
{
    printf("[net] Prepare launching the game...\n");
    assert(clients[0] || clients[1] || clients[2]);

    // send syncronization package
    RandomNumberGenerator::generator().reseed(time(0));
    NetPkgs::StartSync pkg(RANDOM_INT(32767 /* RAND_MAX */));
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
            expected_clients[c].sync_ok = true;
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

    printf("[net] Game starts soon, waiting for %d players.\n", expected_client_count);

    for (unsigned short c = 0; c < expected_client_count; c++)
        networkHandler.natPunch(expected_clients[c].host, expected_clients[c].port);
}

void NetGameHost::sendStartGameMessage()
{
    assert(foreign_lobbyserver);
    assert(clients[0] || clients[1] || clients[2]);

    NetPkgs::StartGame pkg;
    foreign_lobbyserver->sendReliable(&pkg, sizeof(NetPkgs::StartGame));
    sendMessageToMyPeers(&pkg, sizeof(NetPkgs::StartGame));

    netplay.client.handleGameStartMessage();
    netplay.client.setAsLastReceivedMessage(pkg.packageType);
}

void NetGameHost::sendLocalInput()
{
    if (netplay.local_input_buffer.empty())
        return;

    NetPkgs::RemoteInput pkg(0);
    pkg.writeKeys(netplay.local_input_buffer.front());
    netplay.local_input_buffer.pop_front();

    for (int c = 0; c < 3; c++) {
        if (clients[c]) {
            clients[c]->send(&pkg, sizeof(NetPkgs::RemoteInput));
        }
    }

    netplay.client.handleRemoteInput((uint8_t*)&pkg, sizeof(pkg));
    netplay.client.setAsLastReceivedMessage(pkg.packageType);
}

void NetGameHost::sendCurrentGameStateIfNeeded()
{
    if (current_server_tick % NET_GAMESTATE_FRAMES_TO_SEND == 0)
        sendCurrentGameStateNow();

    current_server_tick++;
}

void NetGameHost::sendCurrentGameStateNow()
{
    NetPkgs::GameState pkg;

    for (uint8_t p = 0; p < list_players_cnt; p++) {
        pkg.setPlayerCoord(p, list_players[p]->fx, list_players[p]->fy);
        pkg.setPlayerVel(p, list_players[p]->velx, list_players[p]->vely);
    }

    for (unsigned short c = 0; c < expected_client_count; c++) {
        if (clients[c]) {
            pkg.last_confirmed_local_input_id = last_processed_input_id[c];
            clients[c]->send(&pkg, sizeof(NetPkgs::GameState));
        }
    }

    netplay.client.handleRemoteGameState((uint8_t*)&pkg, sizeof(pkg));
    netplay.client.setAsLastReceivedMessage(pkg.packageType);
}

void NetGameHost::confirmCurrentInputs()
{
    for (unsigned short c = 0; c < expected_client_count; c++) {
        assert(clients[c]);
        if (netplay.remote_input_buffer[c + 1].empty())
            continue;

        // Careful, 255 < 0 !
        uint8_t input_id = netplay.remote_input_buffer[c + 1].front().first;
        last_processed_input_id[c] = (std::max)(last_processed_input_id[c], input_id);
        if (last_processed_input_id[c] > 200 && input_id < 50)
            last_processed_input_id[c] = input_id;
    }
}

void NetGameHost::handleRemoteInput(const NetPeer& player, const uint8_t* data, size_t dataLength) // only for room host
{
    assert(player == *clients[0] || player == *clients[1] || player == *clients[2]);

    NetPkgs::ClientInput* pkg = (NetPkgs::ClientInput*) data; // TODO: check size!

    for (unsigned short c = 0; c < expected_client_count; c++) {
        assert(clients[c]);

        // compare by content
        if (player == *clients[c]) {
            //printf("yey\n");

            // TODO: does this work if GH leaves?

            // TODO: Do not accept inputs from the past
            //if (last_processed_input[c] < pkg->counter) {
            //    last_processed_input[c] = pkg->counter;
                COutputControl keys;
                pkg->readKeys(&keys);
                netplay.remote_input_buffer[c + 1].push_back(std::make_pair(pkg->input_id, keys));
            //}

            NetPkgs::RemoteInput pkg_out(c + 1, pkg->input);
            for (unsigned short c_out = 0; c_out < expected_client_count; c_out++) {
                if (clients[c_out] && c != c_out) {
                    clients[c_out]->send(&pkg_out, sizeof(NetPkgs::RemoteInput));
                }
            }

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
void NetGameHost::sendPowerupStartByGH()
{
    assert(netplay.theHostIsMe);
    assert(list_players[netplay.remotePlayerNumber]->powerupused >= 0);

    NetPkgs::StartPowerup pkg(netplay.remotePlayerNumber,
        list_players[netplay.remotePlayerNumber]->powerupused, 0);
    sendMessageToMyPeers(&pkg, sizeof(NetPkgs::StartPowerup));

    list_players[netplay.remotePlayerNumber]->net_waitingForPowerupTrigger = false;
    printf("[net] P%d (host) used powerup %d\n", pkg.player_id, pkg.powerup_id);
}

// A player wants to use a stored powerup
void NetGameHost::handlePowerupRequest(const NetPeer& player, const uint8_t* data, size_t dataLength)
{
    assert(player == *clients[0] || player == *clients[1] || player == *clients[2]);

    // NetPkgs::RequestPowerup* in_pkg = (NetPkgs::RequestPowerup*) data; // TODO: check size!

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
    if (playerID == 0xFF)
        return;
    if (!list_players[playerID])
        return;

    // TODO: this was copied from player.cpp
    int powerupused = game_values.gamepowerups[playerID];
    assert(powerupused > 0);
    if (powerupused <= 0)
        return; // invalid request or de-sync

    game_values.gamepowerups[playerID] = -1;

    NetPkgs::StartPowerup pkg(playerID, powerupused, player.averageRTT() / 2);
    for (unsigned short c = 0; c < expected_client_count; c++) {
        if (clients[c]) {
            clients[c]->sendReliable(&pkg, sizeof(NetPkgs::StartPowerup));
        }
    }
    netplay.client.handlePowerupStart((uint8_t*)&pkg, sizeof(pkg));
    netplay.client.setAsLastReceivedMessage(pkg.packageType);
}

/*void NetGameHost::sendPowerupTriggerIfReady()
{
    assert(netplay.theHostIsMe);
    assert(list_players[netplay.remotePlayerNumber]->powerupused >= 0);
}*/

void NetGameHost::prepareMapCollisionEvent(CPlayer& player)
{
    assert(netplay.theHostIsMe);
    assert(preparedMapCollPkg);
    preparedMapCollPkg->fill(player);
}

void NetGameHost::sendMapCollisionEvent()
{
    assert(netplay.theHostIsMe);
    assert(preparedMapCollPkg);

    sendMessageToMyPeers(preparedMapCollPkg, sizeof(NetPkgs::MapCollision));

    printf("[net] P%d collided with a map block\n", preparedMapCollPkg->player_id);
}

void NetGameHost::sendP2PCollisionEvent(CPlayer& p1, CPlayer& p2)
{
    assert(netplay.theHostIsMe);
    assert(!p1.isdead());
    assert(!p2.isdead());

    NetPkgs::P2PCollision pkg(p1, p2);
    sendMessageToMyPeers(&pkg, sizeof(NetPkgs::P2PCollision));

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
