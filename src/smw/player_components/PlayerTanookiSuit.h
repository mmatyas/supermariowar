#ifndef PLAYER_TANOOKISUIT
#define PLAYER_TANOOKISUIT

#include <stdint.h>

class CPlayer;

class PlayerTanookiSuit
{
public:
    PlayerTanookiSuit();
    void reset();
    void update(CPlayer &player);
    void onPickup();

    bool isOn();
    bool isStatue();
    bool notStatue();
    bool isBlinking();

    void allowStatue();

private:
    bool tanooki;
    bool statue_lock;
    int statue_timer;
    short tanookilimit;

    bool canTurnIntoStatue(CPlayer &player);
    bool canSuperStomp(CPlayer &player);
    void startSuperStomping(CPlayer &player);

};

#endif // PLAYER_TANOOKISUIT
