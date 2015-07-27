#ifndef SMW_GAMEOBJECT_BLOCK_WEAPONBREAKABLE_H
#define SMW_GAMEOBJECT_BLOCK_WEAPONBREAKABLE_H

#include "IO_Block.h"

class B_WeaponBreakableBlock : public IO_Block
{
	public:
		B_WeaponBreakableBlock(gfxSprite *nspr, short x, short y, short type);
		~B_WeaponBreakableBlock(){};

    BlockType getBlockType() {
        return block_weaponbreakable;
    }

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitbottom(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);

		void triggerBehavior(short iPlayerID, short iTeamID);

	private:
		bool objecthitside(IO_MovingObject * object);

		short iType;
		short iDrawOffsetX;

	friend class MO_Hammer;
	friend class MO_Boomerang;
	friend class MO_Explosion;
};

#endif // SMW_GAMEOBJECT_BLOCK_WEAPONBREAKABLE_H
