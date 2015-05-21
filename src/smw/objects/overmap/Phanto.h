#ifndef SMW_GAMEOBJECT_BLOCK_OMO_Phanto_H
#define SMW_GAMEOBJECT_BLOCK_OMO_Phanto_H

class OMO_Phanto : public IO_OverMapObject
{
	public:
		OMO_Phanto(gfxSprite *nspr, short x, short y, float velx, float vely, short type);
		~OMO_Phanto(){};

		void update();
		bool collide(CPlayer * player);

    short GetType() {
        return iType;
    }

	private:
		short iType;

		float dMaxSpeedX, dMaxSpeedY;
		float dReactionSpeed, dSpeedRatio;
		short iSpeedTimer;
};

#endif // SMW_GAMEOBJECT_BLOCK_OMO_Phanto_H
