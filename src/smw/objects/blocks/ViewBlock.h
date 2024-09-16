#pragma once

#include "PowerupBlock.h"


class B_ViewBlock : public B_PowerupBlock {
public:
	B_ViewBlock(gfxSprite *nspr, Vec2s pos, bool fHidden, const short * piSettings);

	void draw() override;
	void update() override;
	short SelectPowerup() override;

protected:
	void GetNextPowerup();

	short poweruptimer;
	short powerupindex;

	bool fNoPowerupsSelected;
	short iCountWeight;
};
