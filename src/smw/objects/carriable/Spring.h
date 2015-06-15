#ifndef SMW_GAMEOBJECT_CO_SPRING_H
#define SMW_GAMEOBJECT_CO_SPRING_H

#include "objects/moving/CarriedObject.h"

class CPlayer;
class gfxSprite;

class CO_Spring : public MO_CarriedObject
{
	public:
		CO_Spring(gfxSprite *nspr, short ix, short iy, bool fsuper);
		~CO_Spring(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void place();

	protected:

		virtual void hittop(CPlayer * player);
		void hitother(CPlayer * player);

		bool fSuper;
		short iOffsetY;

	friend class CPlayer;
};

#endif // SMW_GAMEOBJECT_CO_SPRING_H
