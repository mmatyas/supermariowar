#ifndef SMW_GAMEOBJECT_CO_SPIKE_H
#define SMW_GAMEOBJECT_CO_SPIKE_H

#include "Spring.h"

class CO_Spike : public CO_Spring
{
	public:
		CO_Spike(gfxSprite *nspr, short ix, short iy);
		~CO_Spike(){};

	private:
		void hittop(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_CO_SPIKE_H
