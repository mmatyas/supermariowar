#pragma once

#include "IO_Block.h"


/// NOTE: The elements are ordered!
enum class NoteBlockType : unsigned char {
	Blue,
	Gray,
	Red,
};


class B_NoteBlock : public IO_Block {
public:
	B_NoteBlock(gfxSprite *nspr, Vec2s pos, short iNumSpr, short aniSpeed, NoteBlockType type, bool fHidden);

	void draw() override;
	void update() override;
	void reset() override;

	bool collide(CPlayer * player, short direction, bool useBehavior) override;
	bool collide(IO_MovingObject * object, short direction) override;

	bool hittop(CPlayer * player, bool useBehavior) override;
	bool hitbottom(CPlayer * player, bool useBehavior) override;
	bool hitright(CPlayer * player, bool useBehavior) override;
	bool hitleft(CPlayer * player, bool useBehavior) override;

	bool hittop(IO_MovingObject * object) override;

private:
	short iNumSprites;
	short animationSpeed;
	short drawFrame;
	short animationTimer;
	short animationWidth;
	NoteBlockType iType;
	short iTypeOffsetY;
};
