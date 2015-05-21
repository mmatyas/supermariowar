#ifndef SMW_GAMEOBJECT_BLOCK_CO_PhantoKey_H
#define SMW_GAMEOBJECT_BLOCK_CO_PhantoKey_H

class CO_PhantoKey : public MO_CarriedObject
{
	public:
		CO_PhantoKey(gfxSprite *nspr);
		~CO_PhantoKey(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void placeKey();

	private:
		short relocatetimer;

		short sparkleanimationtimer;
		short sparkledrawframe;

	friend class CGM_Chase;
};


#endif // SMW_GAMEOBJECT_BLOCK_CO_PhantoKey_H
