#ifndef PLAYER_INVINCIBILITY
#define PLAYER_INVINCIBILITY

#include "gfx.h"

class CPlayer;

class PlayerInvincibility
{
public:
	void reset();
    void update(CPlayer& player);

    bool is_on() const;
    PlayerPalette getPlayerPalette() const;
    void turn_on(CPlayer& player);

private:
    bool invincible;
	short timer;
};

#endif // PLAYER_INVINCIBILITY
