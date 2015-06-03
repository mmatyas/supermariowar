#ifndef PLAYER_TANOOKISUIT
#define PLAYER_TANOOKISUIT

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
    bool tanooki_on; // player is wearing the tanooki suit
    bool statue_lock;
    unsigned short statue_timer;
    unsigned short statue_uses_left;

    bool canTurnIntoStatue(CPlayer &player);
    bool canSuperStomp(CPlayer &player);
    void startSuperStomping(CPlayer &player);
};

#endif // PLAYER_TANOOKISUIT
