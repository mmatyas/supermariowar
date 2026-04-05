#pragma once

#include "IO_Block.h"

class B_DonutBlock : public IO_Block {
public:
	B_DonutBlock(gfxSprite *nspr, Vec2s pos);

	void draw() override;
	void update() override;

	bool hittop(CPlayer * player, bool useBehavior) override;

	void triggerBehavior(short iPlayerId);

private:
	short counter;
	short jigglex;
	short jigglecounter;
};
