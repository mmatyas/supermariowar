#ifndef SMW_GAMEOBJECT_BLOCK_MO_FrenzyCard_H
#define SMW_GAMEOBJECT_BLOCK_MO_FrenzyCard_H

class MO_FrenzyCard : public IO_MovingObject
{
	public:
		MO_FrenzyCard(gfxSprite *nspr, short iType);
		~MO_FrenzyCard(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeCard();

	private:
		short timer;
		short type;

		short sparkleanimationtimer;
		short sparkledrawframe;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_FrenzyCard_H
