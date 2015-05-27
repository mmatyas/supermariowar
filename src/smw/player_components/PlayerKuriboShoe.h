#ifndef PLAYER_KURIBO_SHOE
#define PLAYER_KURIBO_SHOE

#include <stdint.h>

class CPlayer;

enum KuriboShoeType {
	NONE = 0,
	NORMAL = 1,
	STICKY = 2
};

class PlayerKuriboShoe
{
public:
	void reset();
	void update(CPlayer &player, uint8_t keymask);
	void draw(CPlayer &player);

	bool is_on();

	KuriboShoeType getType();
	void setType(KuriboShoeType);

private:
	void update_gettingOutOfTheShoe(CPlayer &player, uint8_t keymask);
	void update_animation();

	KuriboShoeType type;
	uint8_t animationTimer;
	uint8_t animationFrame;
	uint8_t exitTimer;
	uint8_t exitIndex;
};


#endif // PLAYER_KURIBO_SHOE
