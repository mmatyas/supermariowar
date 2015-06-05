#ifndef PLAYER_AWARDEFFECTS
#define PLAYER_AWARDEFFECTS

#include "GlobalConstants.h"
#include "PlayerKillStyles.h"

class CPlayer;

class PlayerAwardEffects
{
public:
    void addDeathAward(CPlayer&);
    void addKillerAward(CPlayer& killer, CPlayer* killed, killstyle style);
    void addKillsInRowInAirAward(CPlayer&);

    void drawRingAward(CPlayer& player);

private:
    void addExploding(CPlayer&);
    void addSwirling(CPlayer&);
    void addRocket(CPlayer&);

    float awardangle;
    short awards[MAXAWARDS];
};


#endif // PLAYER_AWARDEFFECTS
