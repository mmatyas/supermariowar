#include "Room.h"

#include "ProtocolPackages.h"
#include "Util.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>

#define REMOTE_PKG_SIZE_LIMIT 20000 /* bytes in worst case */

Room::Room()
    : roomID(0)
    , visible(true)
    , players{}
    , hostPlayerNumber(0)
    , playerCount(0)
    , gamemodeID(0) // Classic
    , gamemodeGoal(10) // 10 lives
{
}

Room::Room(uint32_t roomID, const char* name, const char* password, Player* host)
    : roomID(roomID)
    , visible(true) // TODO
    , players{}
    , hostPlayerNumber(0)
    , playerCount(1)
    , gamemodeID(0) // Classic
    , gamemodeGoal(10) // 10 lives
{
    strncpy_sec(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
    strncpy_sec(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);

    players[0] = host;
    host->skinPackage.setPlayerID(0);

    createTime = TIME_NOW();
    lastActivityTime = createTime;
}

Room::~Room()
{
    // Players are freed by the server
    for (unsigned short p = 0; p < 4; p++) {
        players[p] = NULL;
    }

    mapPackage.free();
}

void Room::tryAddingPlayer(Player* player)
{
    for (uint8_t p = 0; p < 4 && !player->currentRoomID; p++) {
        if (!players[p]) {
            players[p] = player;
            player->currentRoomID = roomID;
            playerCount++;
            assert(playerCount <= 4);

            sendRoomUpdate(); // do this first to set correct remote player IDs
            sendBlobTo(p, mapPackage);
            sendBlobTo(p, gamemodeSettingsBlob);

            // send new player's skin to others
            shareSkinOf(player);
            // send skins of others to new player
            for (uint8_t p = 0; p < 4; p++) {
                if (players[p] && players[p] != player) {
                    sendBlobTo(p, players[p]->skinPackage);
                }
            }
        }
        else
            printf("  R-%u: slot %d taken by %p\n", roomID, p, players[p]);
    }
}

void Room::removePlayer(Player* player)
{
    bool searching = true;
    for (uint8_t p = 0; p < 4 && searching; p++) {
        if (players[p] == player) {
            players[p] = NULL;
            playerCount--;
            assert(playerCount <= 4);
            searching = false;

            // if this player was the host and there are players in the room
            // TODO: check for upload/download errors
            if (hostPlayerNumber == p && playerCount > 0) {
                hostPlayerNumber = 0xFF;
                for (uint8_t pnexthost = 0; pnexthost < 4 && hostPlayerNumber > 4; pnexthost++) {
                     // set the first available player as host
                    if (players[pnexthost])
                        hostPlayerNumber = pnexthost;
                }
                assert(hostPlayerNumber < 4);
            }
        }
    }
    player->skinPackage.setPlayerID(0xFF);
}

void Room::setGamemode(uint8_t id, uint16_t goal) {
    gamemodeID = id;
    gamemodeGoal = goal;
}

void Room::sendRoomUpdate()
{
    // TODO: error check

    // Crete package
    NetPkgs::CurrentRoom package;
        package.roomID = roomID;
        package.hostPlayerNumber = hostPlayerNumber;
        strncpy_sec(package.name, name, NET_MAX_ROOM_NAME_LENGTH);

    for (uint8_t p = 0; p < 4; p++) {
        if (players[p])
            strncpy_sec(package.playerName[p], players[p]->name.c_str(), NET_MAX_PLAYER_NAME_LENGTH);
        else
            strncpy_sec(package.playerName[p], "(empty)", NET_MAX_PLAYER_NAME_LENGTH);
    }

    package.gamemodeID = gamemodeID;
    package.gamemodeGoal = gamemodeGoal;

    // Send every player information about the other players.
    for (uint8_t p = 0; p < 4; p++) {
        if (players[p]) {
            printf("Sending ROOM_CHANGED:\n");
            printf("  id: %u\n", package.roomID);
            printf("  name: %s\n", package.name);
            printf("  p1: %s\n", package.playerName[0]);
            printf("  p2: %s\n", package.playerName[1]);
            printf("  p3: %s\n", package.playerName[2]);
            printf("  p4: %s\n", package.playerName[3]);
            package.remotePlayerNumber = p;
            players[p]->sendData(&package, sizeof(NetPkgs::CurrentRoom));
        }
    }
}

void Room::sendChatMessage(const Player* sender, const char* message)
{
    if (!message)
        return;

    if (strlen(message) <= 0)
        return;

    // TODO: verify
    uint8_t senderNum = 0xFF;
    for (uint8_t p = 0; p < 4; p++) {
        if (players[p] == sender) {
            senderNum = p;
            break;
        }
    }
    assert(senderNum != 0xFF);

    NetPkgs::RoomChatMsg package(senderNum, message);

    // Send every player information about the other players.
    for (uint8_t p = 0; p < 4; p++) {
        if (players[p]) {
            players[p]->sendData(&package, sizeof(package));
        }
    }
}

void Room::changeAndSendMap(const void* data, size_t data_length)
{
    printf("chageandsendmap\n");
    assert(hostPlayerNumber < 4);
    assert(players[hostPlayerNumber]);

    // Some basic package validation
    if (data_length <= sizeof(NetPkgs::MessageHeader) + 4 /* un-/compressed size 2*2B */
        || data_length > REMOTE_PKG_SIZE_LIMIT) {
        printf("[warning] Corrupt map arrived from host in room %u\n", roomID);
        return;
    }

    mapPackage.replace_with(data, data_length);
    shareBlobExceptHost(mapPackage);
}

void Room::changeAndSendGameModeSettings(const void* data, size_t data_length)
{
    printf("chageandsendgms\n");
    assert(hostPlayerNumber < 4);
    assert(players[hostPlayerNumber]);

    // Some basic package validation
    if (data_length <= sizeof(NetPkgs::MessageHeader)
        || data_length > REMOTE_PKG_SIZE_LIMIT) {
        printf("[warning] Corrupt settings arrived from host in room %u\n", roomID);
        return;
    }

    gamemodeSettingsBlob.replace_with(data, data_length);
    shareBlobExceptHost(gamemodeSettingsBlob);
}

void Room::shareBlob(const Blob& blob)
{
    for (uint8_t p = 0; p < 4; p++)
        sendBlobTo(p, blob);
}

void Room::shareBlobExceptHost(const Blob& blob)
{
    for (uint8_t p = 0; p < 4; p++) {
        if (p == hostPlayerNumber)
            continue;

        sendBlobTo(p, blob);
    }
}

void Room::sendBlobTo(uint8_t index, const Blob& blob)
{
    printf("  sendBlobTo %d\n", index);
    assert(hostPlayerNumber < 4);
    assert(index < 4);
    if (!players[index])
        return;

    assert(!blob.empty());
    players[index]->sendData(blob.get_data(), blob.get_size());
}

void Room::shareSkinOf(Player* sender)
{
    assert(sender);

    if (sender->skinPackage.empty())
        return;

    // TODO: verify

    // find out and set player id
    for (uint8_t p = 0; p < 4; p++) {
        if (players[p] == sender) {
            sender->skinPackage.setPlayerID(p);
            break;
        }
    }

    // send the fixed package to others
    for (uint8_t p = 0; p < 4; p++) {
        if (players[p] && players[p] != sender) {
            sendBlobTo(p, sender->skinPackage);
        }
    }
}

void Room::sendStartSignal()
{
    assert(hostPlayerNumber < 4);

    Player* gamehost = players[hostPlayerNumber];
    assert(gamehost);


    printf("sendStartSignal\n");
    /*printf("  p0: %p\n", players[0]);
    printf("  p1: %p\n", players[1]);
    printf("  p2: %p\n", players[2]);
    printf("  p3: %p\n", players[3]);
    printf("  H: %d -> %p\n", hostPlayerNumber, players[hostPlayerNumber]);*/


    NetPeer* gh_netclient = gamehost->network_client;
    NetPkgs::GameHostInfo pkg_to_players(gh_netclient->addressHost());

    printf("  pkg->players: [%u:%u]\n", pkg_to_players.host, NET_GAMEHOST_PORT);


    NetPkgs::PlayerInfo pkg_to_gh;
    uint8_t playerIndex = 0;
    for (uint8_t p = 0; p < 4; p++)
    {
        if (p != hostPlayerNumber)
        {
            if (players[p])
            {
                //printf("  %d\n", p);
                NetPeer* client = players[p]->network_client;
                assert(client != gh_netclient);

                pkg_to_gh.setPlayer(playerIndex, client->addressHost(), client->addressPort());
                players[p]->sendData(&pkg_to_players, sizeof(NetPkgs::GameHostInfo));
                playerIndex++;
            }
        }
        /*else
            printf("  skip host\n");*/
    }

    gamehost->sendData(&pkg_to_gh, sizeof(NetPkgs::PlayerInfo));

    visible = false;

/*
    NetPkgs::StartSync package;
        package.protocolVersion = NET_PROTOCOL_VERSION;
        package.packageType = NET_NOTICE_GAME_SYNC;
        package.commonRandomSeed = rand();

    for (uint8_t p = 0; p < 4; p++) {
        if (players[p])
            players[p]->sendData(&package, sizeof(NetPkgs::StartSync));
    }
*/
}

void Room::startGameIfEverybodyReady()
{
/*    uint8_t readyPlayers = 0;
    for (uint8_t p = 0; p < 4; p++) {
        if (players[p]) {
            if (players[p]->synchOK)
                readyPlayers++;
        }
    }

    // If everyone is ready, start the room
    if (playerCount == readyPlayers) {
        for (uint8_t p = 0; p < 4; p++) {
            if (players[p]) {
                players[p]->sendCode(NET_NOTICE_GAME_STARTED);
                players[p]->isPlaying = true;
            }
        }
    }

    visible = false;*/
}
