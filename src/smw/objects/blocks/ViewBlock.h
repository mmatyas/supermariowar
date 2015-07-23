#ifndef SMW_GAMEOBJECT_BLOCK_VIEW_H
#define SMW_GAMEOBJECT_BLOCK_VIEW_H

#include "PowerupBlock.h"

class B_ViewBlock : public B_PowerupBlock
{
	public:
		B_ViewBlock(gfxSprite *nspr, short x, short y, bool fHidden, short * piSettings);
		~B_ViewBlock(){};

    BlockType getBlockType() {
        return block_view;
    }

		void draw();
		void update();

		short SelectPowerup();

	protected:
		void GetNextPowerup();

		short poweruptimer;
		short powerupindex;

		bool fNoPowerupsSelected;
		short iCountWeight;
};

#endif // SMW_GAMEOBJECT_BLOCK_VIEW_H
