#ifndef SMW_GAMEOBJECT_BLOCK_MO_Yoshi_H
#define SMW_GAMEOBJECT_BLOCK_MO_Yoshi_H

class MO_Yoshi : public IO_MovingObject
{
	public:
		MO_Yoshi(gfxSprite *nspr, short iColor);
		~MO_Yoshi(){};

		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void placeYoshi();
    short getColor() {
        return color;
    }

	private:
		short timer;
		short color;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_Yoshi_H
