#pragma once

#include "IO_Block.h"
#include "objects/SwitchColor.h"


class B_SwitchBlock : public IO_Block {
public:
	B_SwitchBlock(gfxSprite *nspr, Vec2s pos, SwitchColor color, short state);

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
