#pragma once

class CPlayer;

class PlayerBurnupTimer {
public:
    void update(CPlayer& player);

private:
    short timer = 0;
    short starttimer = 0;

friend class MysteryMushroomTempPlayer;
};
