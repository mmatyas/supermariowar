#ifndef SMWSERVER_H
#define SMWSERVER_H

#include "NetworkLayer.h"
#include "ProtocolPackages.h"

#include "Player.h"
#include "Room.h"

#include <cstdio>
#include <ctime>
#include <unordered_map>
#include <string>
#include <vector>


#define MAX_INACTIVE_TIME ???

class SMWServer : public NetworkEventHandler
{
private:
    std::string serverName;
    NetPkgs::ServerInfo serverInfo;

    uint32_t currentPlayerCount;
    uint32_t maxPlayerCount;
    std::unordered_map<uint64_t, Player> players;

    uint32_t roomCreateID; // temporary
    uint32_t currentRoomCount;
    std::unordered_map<uint32_t, Room> rooms;


    void sendServerInfo(NetPeer&);
    void sendConnectOK(NetPeer&);
    void playerConnectsServer(uint64_t playerID, const void* data, size_t dataLength);
    void sendVisibleRoomEntries(NetPeer&);
    void removeInactivePlayers();

    void playerCreatesRoom(uint64_t playerID, const void* data, size_t dataLength);
    void playerJoinsRoom(uint64_t playerID, const void* data, size_t dataLength);
    void playerLeavesRoom(uint64_t playerID);
    void hostChangesMap(uint64_t playerID, const void* data, size_t dataLength);
    void hostChangesGameModeSettings(uint64_t playerID, const void* data, size_t dataLength);
    void playerChangesSkin(uint64_t playerID, const void* data, size_t dataLength);
    void playerSendsChatMsg(uint64_t playerID, const void* data, size_t dataLength);

    void playerStartsRoom(uint64_t playerID);
    void startRoomIfEveryoneReady(uint64_t playerID);
    void sendInputToHost(uint64_t playerID);
    void sendHostStateToOtherPlayers(uint64_t playerID);


    void readConfig();

    void sendCode(NetPeer&, uint8_t code);
    void sendCode(NetPeer*, uint8_t code);
    void sendCode(uint64_t playerID, uint8_t code);

public:
    SMWServer();
    ~SMWServer();

    bool init();
    void cleanup();

    void update(bool& running);

    // NetworkEventHandler methods
    void onConnect(NetPeer*);
    void onReceive(NetPeer&, const uint8_t*, size_t);
    void onDisconnect(NetPeer& client);
};

#endif // SMWSERVER_H
