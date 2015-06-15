#ifndef SMW_GAMEOBJECT_CO_KURIBOSHOE_H
#define SMW_GAMEOBJECT_CO_KURIBOSHOE_H

#include "Spring.h"

class CO_KuriboShoe : public CO_Spring
{
	public:
		CO_KuriboShoe(gfxSprite *nspr, short ix, short iy, bool fSticky);
		~CO_KuriboShoe(){};

	private:
		void hittop(CPlayer * player);

		bool fSticky;
};

#endif // SMW_GAMEOBJECT_CO_KURIBOSHOE_H
