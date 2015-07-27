#ifndef SMW_GAMEOBJECT_BLOCK_BOUNCE_H
#define SMW_GAMEOBJECT_BLOCK_BOUNCE_H

#include "IO_Block.h"

class B_BounceBlock : public IO_Block
{
	public:
		B_BounceBlock(gfxSprite *nspr, short x, short y, bool fHidden);
		~B_BounceBlock(){};

    BlockType getBlockType() {
        return block_bounce;
    }

		void update();
		void draw();
		void reset();

		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool collide(IO_MovingObject * object, short direction);

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);

		void triggerBehavior();
};

#endif // SMW_GAMEOBJECT_BLOCK_BOUNCE_H
