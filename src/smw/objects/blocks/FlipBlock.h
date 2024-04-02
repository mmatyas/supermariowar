#pragma once

#include "IO_Block.h"

class B_FlipBlock : public IO_Block {
public:
	B_FlipBlock(gfxSprite *nspr, short x, short y, bool fHidden);
	~B_FlipBlock(){};

    BlockType getBlockType() const override {
        return BlockType::Flip;
    }

	void draw() override;
	void update() override;
	void reset() override;

	bool collide(CPlayer * player, short direction, bool useBehavior) override;
	bool collide(IO_MovingObject * object, short direction) override;

    bool isTransparent() override {
        return state == 1;
    }

	bool hittop(CPlayer * player, bool useBehavior) override;
	bool hitbottom(CPlayer * player, bool useBehavior) override;
	bool hitright(CPlayer * player, bool useBehavior) override;
	bool hitleft(CPlayer * player, bool useBehavior) override;

	bool hittop(IO_MovingObject * object) override;
	bool hitbottom(IO_MovingObject * object) override;
	bool hitright(IO_MovingObject * object) override;
	bool hitleft(IO_MovingObject * object) override;

	void triggerBehavior() override;

private:
	void explode();

	short counter;
	short frame;
	short timer;
	short animationWidth;
};
