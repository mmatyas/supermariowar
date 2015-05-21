#ifndef SMW_GAMEOBJECT_BLOCK_B_DonutBlock_H
#define SMW_GAMEOBJECT_BLOCK_B_DonutBlock_H

class B_DonutBlock : public IO_Block
{
	public:
		B_DonutBlock(gfxSprite *nspr, short x, short y);
		~B_DonutBlock(){};

    BlockType getBlockType() {
        return block_donut;
    }

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);

		void triggerBehavior(short iPlayerId);

	private:
		short counter;
		short jigglex;
		short jigglecounter;
};

#endif // SMW_GAMEOBJECT_BLOCK_B_DonutBlock_H
