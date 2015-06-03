#ifndef PLAYER_SECRETCODE
#define PLAYER_SECRETCODE

#include <stdint.h>

class CPlayer;

class PlayerSecretCode
{
public:
	PlayerSecretCode();
	void reset();
	void update(CPlayer &player, const uint8_t keymask);

private:
	unsigned short timer;
	unsigned short index;
};


#endif // PLAYER_SECRETCODE
