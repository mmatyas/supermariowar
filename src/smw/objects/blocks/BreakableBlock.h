#ifndef SMW_GAMEOBJECT_BLOCK_BREAKABLE_H
#define SMW_GAMEOBJECT_BLOCK_BREAKABLE_H

#include "IO_Block.h"

class gfxSprite;
class CPlayer;
class IO_MovingObject;

class B_BreakableBlock : public IO_Block
{
	public:
		B_BreakableBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed);
		~B_BreakableBlock(){};

    BlockType getBlockType() {
        return block_breakable;
    }

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);

		void triggerBehavior();

	private:
		short iNumSprites;
		short animationSpeed;
		short drawFrame;
		short animationTimer;
		short animationWidth;
};

#endif // SMW_GAMEOBJECT_BLOCK_BREAKABLE_H
