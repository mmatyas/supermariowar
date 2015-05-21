#ifndef SMW_GAMEOBJECT_BLOCK_OMO_Thwomp_H
#define SMW_GAMEOBJECT_BLOCK_OMO_Thwomp_H

class OMO_Thwomp : public IO_OverMapObject
{
	public:
		OMO_Thwomp(gfxSprite *nspr, short x, float nspeed);
		~OMO_Thwomp(){};

		void update();
		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_OMO_Thwomp_H
