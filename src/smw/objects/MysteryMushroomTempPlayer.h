#ifndef SMW_GAMEOBJECT_BLOCK_MysteryMushroomTempPlayer_H
#define SMW_GAMEOBJECT_BLOCK_MysteryMushroomTempPlayer_H

class CPlayer;
class MovingPlatform;

class MysteryMushroomTempPlayer {
public:
    MysteryMushroomTempPlayer()
    {
        fUsed = false;
    }

    void SetPlayer(CPlayer* player, short iPowerup);
    void GetPlayer(CPlayer* player, short* iPowerup);

    bool fUsed;

private:
    float fx, fy;
    float fOldX, fOldY;
    float velx, vely;

    short burnupstarttimer, burnuptimer;

    bool inair, onice;

    MovingPlatform* platform;

    short gamepowerup;

    short iOldPowerupX, iOldPowerupY;
};

#endif  // SMW_GAMEOBJECT_BLOCK_MysteryMushroomTempPlayer_H
