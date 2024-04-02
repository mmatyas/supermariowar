#pragma once

#include "PowerupBlock.h"


class B_ViewBlock : public B_PowerupBlock {
public:
	B_ViewBlock(gfxSprite *nspr, short x, short y, bool fHidden, short * piSettings);
	~B_ViewBlock(){};

    BlockType getBlockType() const override {
        return BlockType::View;
    }

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
