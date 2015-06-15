#ifndef PLAYER_BURNUP_TIMER
#define PLAYER_BURNUP_TIMER

class CPlayer;

class PlayerBurnupTimer
{
public:
    PlayerBurnupTimer();
    void update(CPlayer& player);

private:
    short timer;
    short starttimer;

friend class MysteryMushroomTempPlayer;
};

#endif // PLAYER_BURNUP_TIMER
