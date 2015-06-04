#ifndef PLAYER_CARDCOLLECTION
#define PLAYER_CARDCOLLECTION

#include <stdint.h>

class CPlayer;

class PlayerCardCollection
{
public:
	void reset();
	void update(CPlayer &player, const uint8_t keymask);

private:
	unsigned short timer;
	unsigned short index;
};


#endif // PLAYER_CARDCOLLECTION
