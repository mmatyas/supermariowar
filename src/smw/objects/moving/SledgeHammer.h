#ifndef SMW_GAMEOBJECT_BLOCK_MO_SledgeHammer_H
#define SMW_GAMEOBJECT_BLOCK_MO_SledgeHammer_H

class MO_SledgeHammer : public IO_MovingObject
{
	public:
		MO_SledgeHammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);
		~MO_SledgeHammer(){};

		void update();
		bool collide(CPlayer * player);
		void explode();
		void draw();

		short playerID;
		short teamID;
		short colorOffset;

	private:
		short ttl;
		bool fSuper;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_SledgeHammer_H
