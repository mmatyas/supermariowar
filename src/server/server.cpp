#include "server.h"

#include <fstream>

#include "Clock.h"
#include "Log.h"
#include "ProtocolDefinitions.h"
#include "ProtocolPackages.h"

#include "NetworkLayerENet.h"
NetworkLayerENet netLayer;


SMWServer::SMWServer()
    : serverInfo("SMW Server", 0, 20 /* maxPlayerCount */)
    , currentPlayerCount(0)
    , maxPlayerCount(20)
    , roomCreateID(1)
    , currentRoomCount(0)
{
    // ...
}

SMWServer::~SMWServer()
{
    cleanup();
}

bool SMWServer::init()
{
    if (!log_init())
        return false;
    log_silently("-- Server started --");

    readConfig();

    if(!netLayer.init(maxPlayerCount))
        return false;

    return true;
}

void SMWServer::readConfig()
{
    std::ifstream configFile;
    configFile.open("serverconfig", std::ifstream::in);
    if (!configFile.is_open()) {
        log("[warning] Configuration file not found, using default values.");
        return;
    }

    std::string keyword;
    std::string value;

    bool reading = true;
    while (reading)
    {
        configFile >> keyword;
        if (configFile.eof())
            reading = false;
        else
        {
            std::getline(configFile, value);
            value.erase(value.begin()); // remove separating space

            if (configFile.eof())
                reading = false;

            if (keyword[0] != '#') { // not a comment line
                if (keyword.compare("name") == 0) {
                    serverName = value;
                    serverName.resize(31);
                    strncpy(serverInfo.name, serverName.c_str(), 32);
                    serverInfo.name[31] = '\0';
                }
                if (keyword.compare("maxplayers") == 0)
                {
                    uint16_t num = (uint16_t) strtol(value.c_str(), NULL, 10);
                    if (num) {
                        maxPlayerCount = num;
                        serverInfo.maxPlayerCount = num;
                    }
                }

                // TODO: implement password
            }
        }
    }

    configFile.close();
}

void SMWServer::update(bool& running)
{
    netLayer.listen(*this);
}

void SMWServer::onConnect(NetPeer* new_client)
{
    printf("Connect event [%lu].\n", new_client->getPlayerID());

    // Is there a player already with this address+port?
    if (players.count(new_client->getPlayerID()))
        players.erase(new_client->getPlayerID());

    removeInactivePlayers();

    if (maxPlayerCount <= players.size()) {
        sendCode(new_client, NET_RESPONSE_CONNECT_SERVERFULL);
        log("[warning] Server full...");
        // TODO: disconnect
    }
    else {
        players[new_client->getPlayerID()].setClient(new_client);
        log("[info] New connection from %s", new_client->addressAsString().c_str());
    }
}

void SMWServer::onDisconnect(NetPeer& client)
{
    printf("Disconnect event [%lu].\n", client.getPlayerID());
    uint64_t playerID = client.getPlayerID();
    if (players.count(playerID)) {
        playerLeavesRoom(playerID);
        log("[info] %s@%s disconnected.",
            players[playerID].name.c_str(),
            players[playerID].network_client->addressAsString().c_str());
        players.erase(playerID);
    }
    else
        printf("No such player!\n");
}

void SMWServer::onReceive(NetPeer& client, const uint8_t* data, size_t dataLength)
{
    //printf("Receive event.\n");
    if (!data || dataLength < 3)
        return;

    uint8_t versionMajor = data[0];
    uint8_t versionMinor = data[1];
    uint8_t messageType = data[2];

    uint64_t playerID = client.getPlayerID();
    //printf("    from %lu: %u.%u:%u\n", playerID, versionMajor, versionMinor, messageType);

    if (versionMajor != NET_PROTOCOL_VERSION_MAJOR
        || versionMinor != NET_PROTOCOL_VERSION_MINOR) {
        sendCode(client, NET_RESPONSE_ERROR);
        return;
    }

    switch (messageType) {
        //
        // Server-related
        //
        case NET_REQUEST_SERVERINFO:
            sendServerInfo(client);
            break;

        case NET_REQUEST_CONNECT:
            playerConnectsServer(playerID, data, dataLength);
            break;

        case NET_REQUEST_LEAVE_SERVER:
            playerLeavesRoom(playerID);
            break;

        //
        // Room-related
        //
        case NET_REQUEST_ROOM_LIST:
            sendVisibleRoomEntries(client);
            break;

        case NET_G2L_CREATE_ROOM:
            playerCreatesRoom(playerID, data, dataLength);
            break;

        case NET_G2L_ROOM_CHANGED:
            hostUpdatesRoom(playerID, data, dataLength);
            break;

        case NET_REQUEST_JOIN_ROOM:
            playerJoinsRoom(playerID, data, dataLength);
            break;

        case NET_G2L_ROOM_STARTED:
            hostStartsRoom(playerID);
            break;

        case NET_G2L_GAME_RESULTS:
            printf("Game results arrived, not implemented yet.\n");
            break;

        default:
            printf("Unknown: %d", messageType);
            /*for (int a = 0; a < netLayer.lastIncomingDataLength(); a++)
                printf(" %3d", netLayer.lastIncomingData()[a]);*/
            printf("\n");
            break;
    } // end of switch
}

/*

    MESSAGES

*/

void SMWServer::sendServerInfo(NetPeer& client)
{
    client.sendReliable(&serverInfo, sizeof(NetPkgs::ServerInfo));
}

void SMWServer::sendCode(NetPeer& client, uint8_t code)
{
    NetPkgs::MessageHeader msg(code);
    client.sendReliable(&msg, sizeof(NetPkgs::MessageHeader));
}

void SMWServer::sendCode(NetPeer* client, uint8_t code)
{
    NetPkgs::MessageHeader msg(code);
    client->sendReliable(&msg, sizeof(NetPkgs::MessageHeader));
}

void SMWServer::sendCode(uint64_t playerID, uint8_t code)
{
    if (!players.count(playerID))
        return;

    players[playerID].sendCode(code);
}

void SMWServer::playerConnectsServer(uint64_t playerID, const void* data, size_t dataLength)
{
    if (!players.count(playerID))
        return;

    if (dataLength != sizeof(NetPkgs::ClientConnection)) {
        printf("[error] Corrupt package arrived from %lu\n", playerID);
        return;
    }

    NetPkgs::ClientConnection package;
    memcpy(&package, data, dataLength);

    players[playerID].name = package.playerName;
    players[playerID].sendConnectOK();

    log("%s : <%s> connected.",
        players[playerID].network_client->addressAsString().c_str(),
        package.playerName);
}

void SMWServer::removeInactivePlayers()
{
    if (maxPlayerCount <= players.size()) {
        /*for (unsigned p = 0; p < players.size(); ) {
            if (players[p].lastActivityTime + MAX_INACTIVE_TIME < SDL_GetTicks())
                players.remove(p);
            else
                p++;
        }*/
    }
}

void SMWServer::sendVisibleRoomEntries(NetPeer& client)
{
    if (!rooms.size()) {
        sendCode(client, NET_RESPONSE_NO_ROOMS);
        return;
    }

    printf("Sending %lu rooms:\n", rooms.size());
    auto it = rooms.begin();
    while (it != rooms.end()) {
        Room* room = &it->second;
        if (room->visible)
        {
            NetPkgs::RoomInfo roomInfo;
            {
                roomInfo.roomID = room->roomID;

                strncpy(roomInfo.name, room->name, NET_MAX_ROOM_NAME_LENGTH);
                roomInfo.name[NET_MAX_ROOM_NAME_LENGTH - 1] = '\0';

                roomInfo.passwordRequired = false;
                if (room->password[0] != '\0')
                    roomInfo.passwordRequired = true;

                roomInfo.currentPlayerCount = room->playerCount;
                roomInfo.gamemodeID = room->gamemodeID;

                printf("  room %d: {%s; pass?: %d; players: %d}\n",
                    roomInfo.roomID, roomInfo.name, roomInfo.passwordRequired, roomInfo.currentPlayerCount);
            }

            client.sendReliable(&roomInfo, sizeof(NetPkgs::RoomInfo));
        }

        ++it;
    }
}

void SMWServer::playerCreatesRoom(uint64_t playerID, const void* data, size_t dataLength)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (player->isPlaying())
        return;

    if (dataLength != sizeof(NetPkgs::NewRoom)) {
        printf("[error] Corrupt package arrived from %lu\n", playerID);
        return;
    }

    // input
    NetPkgs::NewRoom pkg;
    memcpy(&pkg, data, dataLength);

    // create
    assert(roomCreateID != 0);
    Room room(roomCreateID, pkg.name, pkg.password, player);
    room.setGamemode(pkg.gamemodeID, pkg.gamemodeGoal);
    rooms[roomCreateID] = room;

    player->currentRoomID = roomCreateID;

    // output
    NetPkgs::NewRoomCreated package(roomCreateID);
    player->sendData(&package, sizeof(NetPkgs::NewRoomCreated));

    log("New room by %s : {id: %d; name: %s; pw: %s}",
        player->toString().c_str(), room.roomID, room.name, room.password);

    roomCreateID++; // increase global ID
}

void SMWServer::playerJoinsRoom(uint64_t playerID, const void* data, size_t dataLength)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (player->isPlaying())
        return; // TODO: warning

    if (dataLength != sizeof(NetPkgs::JoinRoom)) {
        printf("[error] Corrupt package arrived from %lu\n", playerID);
        return;
    }

    NetPkgs::JoinRoom pkg;
    memcpy(&pkg, data, dataLength);

    printf("%s wants to join room %u\n", player->name.c_str(), pkg.roomID);

    if (!rooms.count(pkg.roomID)) {
        printf("No such room! (%u)\n", pkg.roomID);
        return;
    }

    Room* room = &rooms[pkg.roomID];    // TODO: Validate

    assert(room);
    assert(room->gamehost);
    assert(room->gamehost->network_client);
    assert(player->network_client);
    NetPkgs::PlayerAddress pkg_to_gh(player->network_client->addressHost());
    NetPkgs::GameHostAddress pkg_to_player(room->gamehost->network_client->addressHost());

    room->gamehost->sendData(&pkg_to_gh, sizeof(pkg_to_gh));
    player->sendData(&pkg_to_player, sizeof(pkg_to_player));
}

void SMWServer::playerLeavesRoom(uint64_t playerID)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (!player->isPlaying())
        return;

    if (!rooms.count(player->currentRoomID))
        return;

    // Search for the room in which the player is
    Room* room = &rooms[player->currentRoomID];
    assert(room->playerCount > 0);
    room->playerCount--;

    // If the room is now empty, delete it.
    if (room->playerCount == 0) {
        rooms.erase(player->currentRoomID);
        log("Room %u erased.", player->currentRoomID);
    }

    player->currentRoomID = 0;
    player->lastActivityTime = TIME_NOW();
}

void SMWServer::hostUpdatesRoom(uint64_t playerID, const void* data, size_t dataLength)
{
    // validate player
    //
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (player->isPlaying())
        return; // TODO: warning

    // validate package
    //
    if (dataLength != sizeof(NetPkgs::RoomStatus)) {
        printf("[error] Corrupt package arrived from %lu\n", playerID);
        return;
    }

    // validate room
    //
    uint32_t roomID = player->currentRoomID;
    if (!rooms.count(roomID))
        return;

    Room* room = &rooms[roomID];
    assert(room->gamehost);
    if(room->gamehost != &players[playerID])
        return;

    // do the update
    //
    NetPkgs::RoomStatus pkg;
    memcpy(&pkg, data, dataLength);

    room->changeName(pkg.name);
    room->setGamemode(pkg.gamemodeID, pkg.gamemodeGoal);

    // TODO: update players
}

void SMWServer::hostStartsRoom(uint64_t playerID)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (!player->isPlaying())
        return;

    uint32_t roomID = player->currentRoomID;
    if (!rooms.count(roomID))
        return;

    Room* room = &rooms[roomID];
    assert(room->gamehost);
    if(room->gamehost != &players[playerID])
        return;

    rooms[roomID].visible = false;
    player->lastActivityTime = TIME_NOW();
    log("[info] Room #%d starting.", roomID);
}

void SMWServer::cleanup()
{
    // TODO: Make sure it runs only once.
    rooms.clear();
    players.clear();

    netLayer.cleanup();

    log_silently("-- Server stopped --");
    log_close();
}
