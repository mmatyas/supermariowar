#ifndef NETWORK_PROTOCOL_GAME_PACKAGES_H
#define NETWORK_PROTOCOL_GAME_PACKAGES_H

#include "ProtocolDefinitions.h"
#include "ProtocolPackages.h" // from common netplay dir

#include <cassert>
#include <cstring>
#include <stdint.h>

/*

    Gameplay-specific packages

*/


namespace NetPkgs {

/*

    Pre-game packages

*/

struct SyncOK : MessageHeader {
    SyncOK() : MessageHeader(NET_P2G_SYNC_OK) {}
};

struct StartGame : MessageHeader {
    StartGame() : MessageHeader(NET_G2E_GAME_START) {}
};


/*

    Gameplay packages

*/

struct LeaveGame : MessageHeader {
    LeaveGame() : MessageHeader(NET_P2G_LEAVE_GAME) {}
};

// 2 byte bit field instead of 8*2, but you can't use arrays :(
struct RawInput {
    uint16_t    flags;

    RawInput()
        : flags(0)
    {}

    void setPlayerKey(uint8_t keyNum, bool down, bool pressed) {
        assert(keyNum < 8);
        flags |= (down ? 1 << (keyNum << 1) : 0);
        flags |= (pressed ? 1 << ((keyNum << 1) + 1) : 0);
    }

    void getPlayerKey(uint8_t keyNum, bool& down, bool& pressed) {
        assert(keyNum < 8);
        down = (flags & (1 << (keyNum << 1)));
        pressed = (flags & (1 << ((keyNum << 1) + 1)));
    }
};

struct ClientInput : MessageHeader {
    uint8_t     input_id;
    RawInput    input;

    ClientInput(const COutputControl* playerControl)
        : MessageHeader(NET_P2G_LOCAL_KEYS)
        , input_id(0)
    {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.setPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }

    void readKeys(COutputControl* playerControl) {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.getPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }
};

struct RemoteInput : MessageHeader {
    uint8_t     playerNumber;
    RawInput    input;

    RemoteInput(uint8_t player, RawInput& input)
        : MessageHeader(NET_G2P_REMOTE_KEYS)
        , playerNumber(player)
        , input(input)
    {}

    RemoteInput(uint8_t player)
        : MessageHeader(NET_G2P_REMOTE_KEYS)
        , playerNumber(player)
    {}

    void readKeys(COutputControl* playerControl) {
        assert(playerControl);
        for (uint8_t k = 0; k < 8; k++)
            input.getPlayerKey(k, playerControl->keys[k].fDown, playerControl->keys[k].fPressed);
    }

    void writeKeys(const COutputControl& playerControl) {
        for (uint8_t k = 0; k < 8; k++)
            input.setPlayerKey(k,
                playerControl.keys[k].fDown,
                playerControl.keys[k].fPressed);
    }
};

struct GameState : MessageHeader {
    float player_x[4];
    float player_y[4];
    float player_xvel[4];
    float player_yvel[4];
    uint8_t last_confirmed_local_input_id;

    GameState()
        : MessageHeader(NET_G2P_GAME_STATE)
        , player_x {0,0,0,0}
        , player_y {0,0,0,0}
        , player_xvel {0,0,0,0}
        , player_yvel {0,0,0,0}
        , last_confirmed_local_input_id(0)
    {}

    // SEND
    void setPlayerCoord(uint8_t playerNum, float x, float y) {
        assert(playerNum < 4);
        player_x[playerNum] = x;
        player_y[playerNum] = y;
    }

    void setPlayerVel(uint8_t playerNum, float xvel, float yvel) {
        assert(playerNum < 4);
        player_xvel[playerNum] = xvel;
        player_yvel[playerNum] = yvel;
    }

    // RECEIVE
    void getPlayerCoord(uint8_t playerNum, float& x, float& y) {
        assert(playerNum < 4);
        x = player_x[playerNum];
        y = player_y[playerNum];
    }

    void getPlayerVel(uint8_t playerNum, float& xvel, float& yvel) {
        assert(playerNum < 4);
        xvel = player_xvel[playerNum];
        yvel = player_yvel[playerNum];
    }
};

struct RequestPowerup : MessageHeader {
    RequestPowerup() : MessageHeader(NET_P2G_REQ_POWERUP) {}
};

struct StartPowerup : MessageHeader {
    uint8_t player_id;
    uint8_t powerup_id;
    uint32_t delay; // time between source player and game host

    StartPowerup(uint8_t playerID, uint8_t powerupID, uint32_t delay)
        : MessageHeader(NET_G2P_START_POWERUP)
        , player_id(playerID)
        , powerup_id(powerupID)
        , delay(delay)
    {
        assert(playerID < 4 || playerID == 0xFF /* error value */);
        assert(powerupID < 128);
    }
};

struct TriggerPowerup : MessageHeader {
    uint8_t player_id;
    uint8_t powerup_id;
    float player_x;
    float player_y;

    TriggerPowerup(uint8_t playerID, uint8_t powerupID, float playerX, float playerY)
        : MessageHeader(NET_G2P_TRIGGER_POWERUP)
        , player_id(playerID)
        , powerup_id(powerupID)
        , player_x(playerX)
        , player_y(playerY)
    {
        // TODO: reduce bit count
        assert(playerID < 4);
        assert(powerupID < 128);
    }
};

struct MapCollision : MessageHeader {
    uint8_t player_id;
    float player_x;
    float player_y;
    float player_xvel;
    float player_yvel;

    MapCollision(): MessageHeader(NET_G2P_TRIGGER_MAPCOLL) {}

    MapCollision(const CPlayer& player)
        : MessageHeader(NET_G2P_TRIGGER_MAPCOLL)
        , player_id(player.getGlobalID())
        , player_x(player.fx)
        , player_y(player.fy)
        , player_xvel(player.velx)
        , player_yvel(player.vely)
    {}

    void fill(CPlayer& player) {
        player_id = player.getGlobalID();
        player_x = player.fx;
        player_y = player.fy;
        player_xvel = player.velx;
        player_yvel = player.vely;
    }
};

struct P2PCollision : MessageHeader {
    uint8_t player_id[2];
    float player_x[2];
    float player_y[2];
    float player_xvel[2];
    float player_yvel[2];
    float player_oldy[2]; // used by stomp detection

    P2PCollision(): MessageHeader(NET_G2P_TRIGGER_P2PCOLL) {}

    P2PCollision(const CPlayer& p1, const CPlayer& p2)
        : MessageHeader(NET_G2P_TRIGGER_P2PCOLL)
    {
        player_id[0] = p1.getGlobalID();
        player_x[0] = p1.fx;
        player_y[0] = p1.fy;
        player_xvel[0] = p1.velx;
        player_yvel[0] = p1.vely;
        player_oldy[0] = p1.fOldY;

        player_id[1] = p2.getGlobalID();
        player_x[1] = p2.fx;
        player_y[1] = p2.fy;
        player_xvel[1] = p2.velx;
        player_yvel[1] = p2.vely;
        player_oldy[1] = p2.fOldY;
    }
};

} // namespace NetPkgs

#endif // NETWORK_PROTOCOL_GAME_PACKAGES_H
