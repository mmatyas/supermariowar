#ifndef SMW_GAMEOBJECT_BLOCK_MO_CollectionCard_H
#define SMW_GAMEOBJECT_BLOCK_MO_CollectionCard_H

class MO_CollectionCard : public IO_MovingObject
{
	public:
		MO_CollectionCard(gfxSprite *nspr, short iType, short iValue, short iUncollectableTime, float dvelx, float dvely, short ix, short iy);
		~MO_CollectionCard(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeCard();

    short getType() {
        return type;
    }
    short getValue() {
        return value;
    }

	private:
		short timer;
		short type;
		short value;

		short sparkleanimationtimer;
		short sparkledrawframe;

		short uncollectabletime;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_CollectionCard_H
