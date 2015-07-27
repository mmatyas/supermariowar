#ifndef SMW_GAMEOBJECT_BLOCK_NOTE_H
#define SMW_GAMEOBJECT_BLOCK_NOTE_H

#include "IO_Block.h"

class B_NoteBlock : public IO_Block
{
	public:
		B_NoteBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short type, bool fHidden);
		~B_NoteBlock(){};

    BlockType getBlockType() {
        return block_note;
    }

		void draw();
		void update();
		void reset();

		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool collide(IO_MovingObject * object, short direction);

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);

	private:
		short iNumSprites;
		short animationSpeed;
		short drawFrame;
		short animationTimer;
		short animationWidth;
		short iType;
		short iTypeOffsetY;
};

#endif // SMW_GAMEOBJECT_BLOCK_NOTE_H
