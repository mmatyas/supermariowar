#pragma once

#include "IO_Block.h"
#include "objects/ThrowBlockType.h"


class B_ThrowBlock : public IO_Block {
public:
	B_ThrowBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, ThrowBlockType type);

	void draw() override;
	void update() override;

	bool hittop(CPlayer * player, bool useBehavior) override;
	bool hitright(CPlayer * player, bool useBehavior) override;
	bool hitleft(CPlayer * player, bool useBehavior) override;

	void GiveBlockToPlayer(CPlayer * player);
	void triggerBehavior() override;

private:
	short iNumSprites;
	short animationSpeed;
	short drawFrame;
	short animationTimer;
	short animationWidth;
	ThrowBlockType iType;
};
