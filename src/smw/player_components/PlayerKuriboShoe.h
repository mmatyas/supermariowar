#pragma once

#include <stdint.h>

class CPlayer;

enum KuriboShoeType {
	NONE = 0,
	NORMAL = 1,
	STICKY = 2
};

class PlayerKuriboShoe {
public:
	void reset();
	void update(CPlayer &player, uint8_t keymask);
	void draw(CPlayer &player);

	bool is_on() const;

	KuriboShoeType getType() const;
	void setType(KuriboShoeType);

private:
	void update_gettingOutOfTheShoe(CPlayer &player, uint8_t keymask);
	void update_animation();
	void update_SuperStomp(CPlayer &player);

	KuriboShoeType type = NONE;
	uint8_t animationTimer = 0;
	uint8_t animationFrame = 0;
	uint8_t exitTimer = 0;
	uint8_t exitIndex = 0;
};
