#pragma once

#include <stdint.h>

class CPlayer;

class PlayerSecretCode
{
public:
	PlayerSecretCode();
	void reset();
	void update(CPlayer &player, const uint8_t keymask);

private:
	unsigned short timer = 0;
	unsigned short index = 0;
};
