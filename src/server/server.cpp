#include "server.h"

#include <fstream>

#include "Clock.h"
#include "Log.h"
#include "ProtocolDefinitions.h"
#include "ProtocolPackages.h"

#include "NetworkLayerENet.h"
NetworkLayerENet netLayer;


SMWServer::SMWServer()
    : serverInfo("SMW Server", 0, maxPlayerCount)
    , maxPlayerCount(20)
    , roomCreateID(1)
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
                    strcpy(serverInfo.name, serverName.c_str());
                }
                if (keyword.compare("maxplayers") == 0)
                {
                    uint16_t num = (uint16_t) strtol(value.c_str(), NULL, 10);
                    if (num) {
                        maxPlayerCount = num;
                        serverInfo.maxPlayerCount = num;
                    }
                }

                // TODO: jelszó implemetáció
            }
        }
    }

    configFile.close();
}

void SMWServer::update(bool& running)
{
    netLayer.listen(*this);
}

void SMWServer::onConnect(NetClient* new_client)
{
    printf("Connect event [%lu].\n", new_client->getPlayerID());

    // Is there a player already with this address+port?
    if (players.count(new_client->getPlayerID()))
        players.erase(new_client->getPlayerID());

    removeInactivePlayers();

    if (maxPlayerCount <= players.size()) {
        sendCode(new_client, NET_RESPONSE_CONNECT_SERVERFULL);
        log("Server full...\n");
        // TODO: disconnect
    }
    else {
        players[new_client->getPlayerID()].setClient(new_client);
        log("New connection from %s\n", new_client->addressAsString().c_str());
    }
}

void SMWServer::onDisconnect(NetClient& client)
{
    printf("Disconnect event [%lu].\n", client.getPlayerID());
    uint64_t playerID = client.getPlayerID();
    if (players.count(playerID)) {
        playerLeavesRoom(playerID);
        log("%s disconnected.", players[playerID].name.c_str());
        players.erase(playerID);
    }
    else
        printf("No such player!\n");
}

void SMWServer::onReceive(NetClient& client, const uint8_t* data, size_t dataLength)
{
    //printf("Receive event.\n");
    if (!data || dataLength < 3)
        return;

    uint8_t versionMajor = data[0];
    uint8_t versionMinor = data[1];
    uint8_t messageType = data[2];

    uint64_t playerID = client.getPlayerID();
    //printf("    from %lu: %u.%u:%u\n", playerID, versionMajor, versionMinor, messageType);

    if (versionMajor == NET_PROTOCOL_VERSION_MAJOR) {
        switch (messageType) {
            //
            // Server-related
            //
            case NET_REQUEST_SERVERINFO:
                sendServerInfo(client);
                break;

            case NET_REQUEST_CONNECT:
                playerConnectsServer(playerID, data);
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

            case NET_REQUEST_CREATE_ROOM:
                playerCreatesRoom(playerID, data);
                break;

            case NET_REQUEST_JOIN_ROOM:
                playerJoinsRoom(playerID, data);
                break;

            case NET_REQUEST_LEAVE_ROOM:
                playerLeavesRoom(playerID);
                break;

            case NET_NOTICE_ROOM_CHAT_MSG:
                playerSendsChatMsg(playerID, data);
                break;

            //
            // Room start
            //
            case NET_G2L_START_ROOM:
                log("NET_G2L_START_ROOM!");
                playerStartsRoom(playerID);
                break;

            /*case NET_NOTICE_GAME_SYNC_OK:
                log("NET_NOTICE_GAME_SYNC_OK!");
                startRoomIfEveryoneReady(playerID);
                break;*/

            //
            // Gameplay-related
            //
            /*case NET_NOTICE_LOCAL_KEYS:
                //printf("local keys\n");
                sendInputToHost(playerID);
                break;

            case NET_NOTICE_HOST_STATE:
                //printf("local keys\n");
                sendHostStateToOtherPlayers(playerID);
                break;*/

            default:
                printf("Unknown:");
                /*for (int a = 0; a < netLayer.lastIncomingDataLength(); a++)
                    printf(" %3d", netLayer.lastIncomingData()[a]);*/
                printf("\n");
                break;
        } // end of switch
    } // end of if ()
    else
        sendCode(client, NET_RESPONSE_BADPROTOCOL);
}

/*

    MESSAGES

*/

void SMWServer::sendServerInfo(NetClient& client)
{
    client.sendData(&serverInfo, sizeof(ServerInfoPackage));
}

void SMWServer::sendCode(NetClient& client, uint8_t code)
{
    MessageHeader msg(code);
    client.sendData(&msg, sizeof(MessageHeader));
}

void SMWServer::sendCode(NetClient* client, uint8_t code)
{
    MessageHeader msg(code);
    client->sendData(&msg, sizeof(MessageHeader));
}

void SMWServer::sendCode(uint64_t playerID, uint8_t code)
{
    if (!players.count(playerID))
        return;

    players[playerID].sendCode(code);
}

void SMWServer::playerConnectsServer(uint64_t playerID, const void* data)
{
    if (!players.count(playerID))
        return;

    ClientConnectionPackage package;
    memcpy(&package, data, sizeof(ClientConnectionPackage));

    players[playerID].name = package.playerName;
    players[playerID].sendConnectOK();

    log("%s : <%s> connected.\n",
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

void SMWServer::sendVisibleRoomEntries(NetClient& client)
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
            RoomInfoPackage roomInfo;
            {
                roomInfo.protocolMajorVersion = NET_PROTOCOL_VERSION_MAJOR;
                roomInfo.protocolMinorVersion = NET_PROTOCOL_VERSION_MINOR;
                roomInfo.packageType = NET_RESPONSE_ROOM_LIST_ENTRY;

                roomInfo.roomID = room->roomID;

                memcpy(roomInfo.name, room->name, NET_MAX_ROOM_NAME_LENGTH);
                roomInfo.name[NET_MAX_ROOM_NAME_LENGTH - 1] = '\0';

                roomInfo.passwordRequired = false;
                if (room->password[0] != '\0')
                    roomInfo.passwordRequired = true;

                roomInfo.currentPlayerCount = 0;
                for (long unsigned p = 0; p < 4; p++)
                    if (room->players[p])
                        roomInfo.currentPlayerCount++;

                printf("  room %d: {%s; pass?: %d; players: %d}\n",
                    roomInfo.roomID, roomInfo.name, roomInfo.passwordRequired, roomInfo.currentPlayerCount);
            }

            client.sendData(&roomInfo, sizeof(RoomInfoPackage));
        }

        ++it;
    }
}

void SMWServer::playerCreatesRoom(uint64_t playerID, const void* data)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (player->currentRoomID || player->isPlaying)
        return;

    // input
    NewRoomPackage pkg;
    memcpy(&pkg, data, sizeof(NewRoomPackage));

    // create
    Room room(roomCreateID, pkg.name, pkg.password, player);
    rooms[roomCreateID] = room;

    player->currentRoomID = roomCreateID;
    player->isPlaying = false;

    // output
    NewRoomCreatedPackage package(roomCreateID);
    player->sendData(&package, sizeof(NewRoomCreatedPackage));

    log("New room by %s : {id: %d; name: %s; pw: %s}",
        player->toString().c_str(), room.roomID, room.name, room.password);

    roomCreateID++; // increase global ID
}

void SMWServer::playerJoinsRoom(uint64_t playerID, const void* data)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (player->currentRoomID || player->isPlaying)
        return; // TODO: warning

    JoinRoomPackage pkg;
    memcpy(&pkg, data, sizeof(JoinRoomPackage));

    printf("%s wants to join room %u\n", player->name.c_str(), pkg.roomID);

    if (!rooms.count(pkg.roomID)) {
        printf("No such room! (%u)\n", pkg.roomID);
        return;
    }

    // Find first empty slot
    Room* room = &rooms[pkg.roomID];    // TODO: Validate
    room->tryAddingPlayer(player);

    if (player->currentRoomID) {
        player->sendCode(NET_RESPONSE_JOIN_OK);
        room->sendRoomUpdate();
    }
    else
        player->sendCode(NET_RESPONSE_ROOM_FULL);

    player->isPlaying = false;
}

void SMWServer::playerLeavesRoom(uint64_t playerID)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (!player->currentRoomID || player->isPlaying)
        return;

    if (!rooms.count(player->currentRoomID))
        return;

    // Search for the room in which the player is
    Room* room = &rooms[player->currentRoomID];
    room->removePlayer(player);

    // If the room is now empty, delete it.
    if (room->playerCount == 0) {
        rooms.erase(player->currentRoomID);
        log("Room %u erased.", player->currentRoomID);
    }
    else
        room->sendRoomUpdate();

    player->currentRoomID = 0;
    player->isPlaying = false;
    player->lastActivityTime = TIME_NOW();
}

void SMWServer::playerSendsChatMsg(uint64_t playerID, const void* data)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    uint32_t roomID = player->currentRoomID;
    if (!roomID || player->isPlaying)
        return;

    if (!rooms.count(roomID))
        return;

    Net_RoomChatMsgPackage pkg;
    memcpy(&pkg, data, sizeof(Net_RoomChatMsgPackage));

    rooms[roomID].sendChatMessage(player, pkg.message);
    player->lastActivityTime = TIME_NOW();
}


void SMWServer::playerStartsRoom(uint64_t playerID)
{
    if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    uint32_t roomID = player->currentRoomID;
    if (!roomID || player->isPlaying)
        return;

    if (!rooms.count(roomID))
        return;

    Room* room = &rooms[roomID];
    assert(room->hostPlayerNumber < 4);

    // only host can start
    if (room->players[room->hostPlayerNumber] != player)
        return;

    rooms[roomID].sendStartSignal();
    player->lastActivityTime = TIME_NOW();
}

void SMWServer::startRoomIfEveryoneReady(uint64_t playerID)
{
    /*if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (!player->currentRoomID || player->isPlaying)
        return;

    if (!rooms.count(player->currentRoomID))
        return;

    player->synchOK = true;
    player->lastActivityTime = TIME_NOW();

    Room* room = &rooms[player->currentRoomID];
    room->startGameIfEverybodyReady();*/
}

void SMWServer::sendInputToHost(uint64_t playerID)
{
    /*if (!players.count(playerID))
        return;

    Player* player = &players[playerID];
    if (!player->currentRoomID || !player->isPlaying)
        return;

    if (!rooms.count(player->currentRoomID))
        return;

    Room* room = &rooms[player->currentRoomID];

    LocalInputPackage pkg_in;
    memcpy(&pkg_in, udpIncomingPacket->data, sizeof(LocalInputPackage));

    RemoteInputPackage pkg_out;
    pkg_out.protocolVersion = NET_PROTOCOL_VERSION;
    pkg_out.packageType = NET_NOTICE_REMOTE_KEYS;
    pkg_out.playerNumber = 127;

    // get player's number in the room
    for (uint8_t p = 0; p < 4 && pkg_out.playerNumber == 127; p++) {
        if (room->players[p] == player)
            pkg_out.playerNumber = p;
    }
    if (pkg_out.playerNumber == 127) // TODO: Hiba!!
        return;

    // copy input
    memcpy(&pkg_out.input, &pkg_in.input, sizeof(RawInput));

    sendPackage(room->players[room->hostPlayerNumber]->address, &pkg_out, sizeof(RemoteInputPackage));
    */
}

void SMWServer::sendHostStateToOtherPlayers(uint64_t playerID)
{
    /*if (!players.count(playerID)) // Player doesn't exists
        return;

    Player* player = &players[playerID];
    if (!player->currentRoomID || !player->isPlaying) // Player is not in the state to send such message
        return;

    if (!rooms.count(player->currentRoomID)) // Room doesn't exists
        return;

    Room* room = &rooms[player->currentRoomID];
    if (room->players[room->hostPlayerNumber] != player) // This isn't the host player
        return;

    GameStatePackage pkg;
    memcpy(&pkg, udpIncomingPacket->data, sizeof(GameStatePackage));

    for (uint8_t p = 0; p < 4; p++) {
        if (room->players[p]) {
            if (p != room->hostPlayerNumber) {
                sendPackage(room->players[p]->address, &pkg, sizeof(GameStatePackage));
            }
        }
    }*/
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
