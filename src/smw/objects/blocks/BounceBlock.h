#pragma once

#include "IO_Block.h"


class B_BounceBlock : public IO_Block {
public:
	B_BounceBlock(gfxSprite *nspr, short x, short y, bool fHidden);
	~B_BounceBlock(){};

    BlockType getBlockType() const override {
        return BlockType::Bounce;
    }

	void update();
	void draw();
	void reset();

	bool collide(CPlayer * player, short direction, bool useBehavior);
	bool collide(IO_MovingObject * object, short direction);

	bool hittop(CPlayer * player, bool useBehavior);
	bool hitbottom(CPlayer * player, bool useBehavior);

	bool hittop(IO_MovingObject * object);

	void triggerBehavior();
};
