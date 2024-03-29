#pragma once

#include "IO_Block.h"


class B_SwitchBlock : public IO_Block {
public:
	B_SwitchBlock(gfxSprite *nspr, short x, short y, short colorID, short state);
	~B_SwitchBlock(){};

    BlockType getBlockType() const override {
        return BlockType::OnOff;
    }

	void draw();
	bool collide(CPlayer * player, short direction, bool useBehavior);

    bool isTransparent() {
        return state != 0;
    }

	bool hittop(CPlayer * player, bool useBehavior);
	bool hitbottom(CPlayer * player, bool useBehavior);
	bool hitright(CPlayer * player, bool useBehavior);
	bool hitleft(CPlayer * player, bool useBehavior);

	bool hittop(IO_MovingObject * object);
	bool hitbottom(IO_MovingObject * object);
	bool hitright(IO_MovingObject * object);
	bool hitleft(IO_MovingObject * object);

	void FlipState(short playerID);

private:
	short iSrcX;
};
