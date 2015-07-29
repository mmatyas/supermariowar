#include "Room.h"

#include "ProtocolPackages.h"

#include <cassert>

Room::Room()
{
    roomID = 0;
    visible = true;
    playerCount = 0;
    for (uint8_t p = 0; p < 4; p++)
        players[p] = NULL;
}

Room::Room(uint32_t roomID, const char* name, const char* password, Player* host)
{
    // TODO
    visible = true;

    memcpy(this->name, name, NET_MAX_ROOM_NAME_LENGTH);
    this->name[NET_MAX_ROOM_NAME_LENGTH - 1] = '\0';
    memcpy(this->password, password, NET_MAX_ROOM_PASSWORD_LENGTH);
    this->password[NET_MAX_ROOM_PASSWORD_LENGTH - 1] = '\0';

    playerCount = 1;
    hostPlayerNumber = 0;

    players[0] = host;
    for (uint8_t p = 1; p < 4; p++)
        players[p] = NULL;

    this->roomID = roomID;
    createTime = TIME_NOW();
    lastActivityTime = createTime;
}

Room::~Room()
{
    // Players are freed by the server
    for (unsigned short p = 0; p < 4; p++) {
        players[p] = NULL;
    }
}

void Room::tryAddingPlayer(Player* player)
{
    for (uint8_t p = 0; p < 4 && !player->currentRoomID; p++) {
        if (!players[p]) {
            players[p] = player;
            player->currentRoomID = roomID;
            playerCount++;
            assert(playerCount <= 4);
        }
        else
            printf("  R-%u: slot %d foglalt: %p\n", roomID, p, players[p]);
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
                for (uint8_t p = 0; p < 4 && hostPlayerNumber > 4; p++) {
                     // set the first available player as host
                    if (players[p])
                        hostPlayerNumber = p;
                }
                assert(hostPlayerNumber < 4);
            }
        }
    }
}

void Room::sendRoomUpdate()
{
    // TODO: error check

    // Crete package
    CurrentRoomPackage package;
        package.roomID = roomID;
        package.hostPlayerNumber = hostPlayerNumber;
        memcpy(package.name, name, NET_MAX_ROOM_NAME_LENGTH);

    for (uint8_t p = 0; p < 4; p++) {
        if (players[p])
            memcpy(package.playerName[p], players[p]->name.c_str(), NET_MAX_PLAYER_NAME_LENGTH);
        else
            memcpy(package.playerName[p], "(empty)", NET_MAX_PLAYER_NAME_LENGTH);
    }

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
            players[p]->sendData(&package, sizeof(CurrentRoomPackage));
        }
    }
}

void Room::sendChatMessage(Player* sender, const char* message)
{
    if (!message)
        return;

    if (strlen(message) <= 0)
        return;

    // TODO: verify
    uint8_t senderNum = 255;
    for (uint8_t p = 0; p < 4 && senderNum == 255; p++) {
        if (players[p] == sender) {
            senderNum = p;
        }
    }
    assert(senderNum != 255);

    Net_RoomChatMsgPackage package(senderNum, message);

    // Send every player information about the other players.
    for (uint8_t p = 0; p < 4; p++) {
        if (players[p]) {
            players[p]->sendData(&package, sizeof(Net_RoomChatMsgPackage));
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


    NetClient* gh_netclient = gamehost->network_client;
    Net_GameHostInfoPkg pkg_to_players(gh_netclient->addressHost());

    printf("  pkg->players: [%u:%u]\n", pkg_to_players.host, NET_SERVER_PORT + 1);


    Net_PlayerInfoPkg pkg_to_gh;
    uint8_t playerIndex = 0;
    for (uint8_t p = 0; p < 4; p++)
    {
        if (p != hostPlayerNumber)
        {
            if (players[p])
            {
                //printf("  %d\n", p);
                NetClient* client = players[p]->network_client;
                assert(client != gh_netclient);

                pkg_to_gh.setPlayer(playerIndex, client->addressHost(), client->addressPort());
                players[p]->sendData(&pkg_to_players, sizeof(Net_GameHostInfoPkg));
                playerIndex++;
            }
        }
        /*else
            printf("  skip host\n");*/
    }

    gamehost->sendData(&pkg_to_gh, sizeof(Net_PlayerInfoPkg));

/*
    StartSyncPackage package;
        package.protocolVersion = NET_PROTOCOL_VERSION;
        package.packageType = NET_NOTICE_GAME_SYNC;
        package.commonRandomSeed = rand();

    for (uint8_t p = 0; p < 4; p++) {
        if (players[p])
            players[p]->sendData(&package, sizeof(StartSyncPackage));
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
