#pragma once

#include "gfx/gfxPalette.h"

class CPlayer;

class PlayerInvincibility {
public:
	void reset();
    void update(CPlayer& player);

    bool is_on() const;
    PlayerPalette getPlayerPalette() const;
    void turn_on(CPlayer& player);

private:
    bool invincible = false;
	short timer = 0;
};
