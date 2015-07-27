#ifndef SMW_GAMEOBJECT_BLOCK_FLIP_H
#define SMW_GAMEOBJECT_BLOCK_FLIP_H

#include "IO_Block.h"

class B_FlipBlock : public IO_Block
{
	public:
		B_FlipBlock(gfxSprite *nspr, short x, short y, bool fHidden);
		~B_FlipBlock(){};

    BlockType getBlockType() {
        return block_flip;
    }

		void draw();
		void update();
		void reset();

		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool collide(IO_MovingObject * object, short direction);

    bool isTransparent() {
        return state == 1;
    }

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitbottom(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);

		void triggerBehavior();

	private:

		void explode();

		short counter;
		short frame;
		short timer;
		short animationWidth;
};

#endif // SMW_GAMEOBJECT_BLOCK_FLIP_H
