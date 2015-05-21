#ifndef SMW_GAMEOBJECT_BLOCK_MO_Podobo_H
#define SMW_GAMEOBJECT_BLOCK_MO_Podobo_H

class MO_Podobo : public IO_MovingObject
{
	public:
		MO_Podobo(gfxSprite *nspr, short x, short y, float nspeed, short playerid, short teamid, short colorid, bool isSpawned);
		~MO_Podobo(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

	private:

		short iColorOffsetY;

		bool fIsSpawned;
		short iHiddenPlane;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_Podobo_H
