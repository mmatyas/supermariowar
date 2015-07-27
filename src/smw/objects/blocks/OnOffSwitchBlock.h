#ifndef SMW_GAMEOBJECT_BLOCK_ONOFFSWITCH_H
#define SMW_GAMEOBJECT_BLOCK_ONOFFSWITCH_H

#include "IO_Block.h"

class B_OnOffSwitchBlock : public IO_Block
{
	public:
		B_OnOffSwitchBlock(gfxSprite *nspr, short x, short y, short colorID, short state);
		~B_OnOffSwitchBlock(){};

    BlockType getBlockType() {
        return block_onoff_switch;
    }

		void update();
		void draw();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);

    void FlipState() {
        state = (state < 3 ? state + 3 : state - 3);
    }

		void triggerBehavior(short playerID);

	private:
		short iColorID;
		short iSrcX;
};

#endif // SMW_GAMEOBJECT_BLOCK_ONOFFSWITCH_H
