#pragma once

#include "IO_Block.h"
#include "objects/SwitchColor.h"


class B_OnOffSwitchBlock : public IO_Block {
public:
	B_OnOffSwitchBlock(gfxSprite *nspr, short x, short y, SwitchColor color, short state);
	~B_OnOffSwitchBlock(){};

	void update() override;
	void draw() override;

	bool hittop(CPlayer * player, bool useBehavior) override;
	bool hitbottom(CPlayer * player, bool useBehavior) override;

	bool hittop(IO_MovingObject * object) override;
	bool hitleft(IO_MovingObject * object) override;
	bool hitright(IO_MovingObject * object) override;

    void FlipState() {
        state = (state < 3 ? state + 3 : state - 3);
    }

	void triggerBehavior(short playerID);

private:
	SwitchColor m_color;
	short iSrcX;
};
