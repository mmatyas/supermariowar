#pragma once

#include "IO_Block.h"


class B_BounceBlock : public IO_Block {
public:
	B_BounceBlock(gfxSprite *nspr, Vec2s pos, bool fHidden);

	void update() override;
	void draw() override;
	void reset() override;

	bool collide(CPlayer * player, short direction, bool useBehavior) override;
	bool collide(IO_MovingObject * object, short direction) override;

	bool hittop(CPlayer * player, bool useBehavior) override;
	bool hitbottom(CPlayer * player, bool useBehavior) override;

	bool hittop(IO_MovingObject * object) override;

	void triggerBehavior() override;
};
