#pragma once

#include "IO_Block.h"


class B_SwitchBlock : public IO_Block {
public:
	B_SwitchBlock(gfxSprite *nspr, short x, short y, short colorID, short state);
	~B_SwitchBlock(){};

	void draw() override;
	bool collide(CPlayer * player, short direction, bool useBehavior) override;

    bool isTransparent() override {
        return state != 0;
    }

	bool hittop(CPlayer * player, bool useBehavior) override;
	bool hitbottom(CPlayer * player, bool useBehavior) override;
	bool hitright(CPlayer * player, bool useBehavior) override;
	bool hitleft(CPlayer * player, bool useBehavior) override;

	bool hittop(IO_MovingObject * object) override;
	bool hitbottom(IO_MovingObject * object) override;
	bool hitright(IO_MovingObject * object) override;
	bool hitleft(IO_MovingObject * object) override;

	void FlipState(short playerID);

private:
	short iSrcX;
};
