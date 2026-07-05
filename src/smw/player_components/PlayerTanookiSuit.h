#pragma once

class CPlayer;

class PlayerTanookiSuit {
public:
    PlayerTanookiSuit();
    void reset();
    void update(CPlayer &player);
    void onPickup();

    bool isOn() const;
    bool isStatue() const;
    bool notStatue() const;
    bool isBlinking() const;

    void allowStatue();

    void drawStatue(CPlayer&);

private:
    bool tanooki_on = false; // player is wearing the tanooki suit
    bool statue_lock = false;
    unsigned short statue_timer = 0;
    unsigned short statue_uses_left = 0;

    bool canTurnIntoStatue(CPlayer &player);
    bool canSuperStomp(CPlayer &player);
    void startSuperStomping(CPlayer &player);
};
