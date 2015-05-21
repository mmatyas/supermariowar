#ifndef SMW_GAMEOBJECT_BLOCK_CO_Spike_H
#define SMW_GAMEOBJECT_BLOCK_CO_Spike_H

class CO_Spike : public CO_Spring
{
	public:
		CO_Spike(gfxSprite *nspr, short ix, short iy);
		~CO_Spike(){};

	private:

		void hittop(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_CO_Spike_H
