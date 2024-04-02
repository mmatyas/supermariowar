#pragma once

#include "IO_Block.h"

class B_DonutBlock : public IO_Block {
public:
	B_DonutBlock(gfxSprite *nspr, short x, short y);
	~B_DonutBlock(){};

    BlockType getBlockType() const override {
        return BlockType::Donut;
    }

	void draw() override;
	void update() override;

	bool hittop(CPlayer * player, bool useBehavior) override;

	void triggerBehavior(short iPlayerId);

private:
	short counter;
	short jigglex;
	short jigglecounter;
};
