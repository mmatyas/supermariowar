#pragma once

#include "IO_Block.h"

class gfxSprite;
class CPlayer;
class IO_MovingObject;


class B_BreakableBlock : public IO_Block {
public:
	B_BreakableBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed);

	void draw() override;
	void update() override;

	bool hittop(CPlayer * player, bool useBehavior) override;
	bool hitbottom(CPlayer * player, bool useBehavior) override;

	bool hittop(IO_MovingObject * object) override;
	bool hitright(IO_MovingObject * object) override;
	bool hitleft(IO_MovingObject * object) override;

	void triggerBehavior() override;

private:
	short iNumSprites;
	short animationSpeed;
	short drawFrame;
	short animationTimer;
	short animationWidth;
};
