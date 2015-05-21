#ifndef SMW_GAMEOBJECT_BLOCK_CO_Egg_H
#define SMW_GAMEOBJECT_BLOCK_CO_Egg_H

class CO_Egg : public MO_CarriedObject
{
	public:
		CO_Egg(gfxSprite *nspr, short iColor);
		~CO_Egg(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void placeEgg();
		void Drop();

    short getColor() {
        return color;
    }

	private:
		short relocatetimer;
		short explosiondrawframe, explosiondrawtimer;

		CPlayer * owner_throw;
		short owner_throw_timer;

		short sparkleanimationtimer;
		short sparkledrawframe;

		short color;

		short egganimationrates[6];

	friend class CPlayer;
	friend class MO_Yoshi;
	friend class CGM_Eggs;
};

#endif // SMW_GAMEOBJECT_BLOCK_CO_Egg_H
