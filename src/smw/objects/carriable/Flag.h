#ifndef SMW_GAMEOBJECT_CO_FLAG_H
#define SMW_GAMEOBJECT_CO_FLAG_H

#include "objects/moving/CarriedObject.h"

class CPlayer;
class gfxSprite;
class MO_FlagBase;

class CO_Flag : public MO_CarriedObject
{
	public:
		CO_Flag(gfxSprite *nspr, MO_FlagBase * base, short iTeamID, short iColorID);
		~CO_Flag(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void MoveToOwner();

		void placeFlag();
		void Drop();

		bool GetInBase() { return fInBase; }
		short GetTeamID() { return teamID; }

	private:
		short timer;
		MO_FlagBase * flagbase;
		short teamID;
		bool fLastFlagDirection;
		bool fInBase;
		CPlayer * owner_throw;
		short owner_throw_timer;
		bool centerflag;

	friend class CPlayer;
	friend class CGM_CTF;
	friend class MO_FlagBase;
};

#endif // SMW_GAMEOBJECT_CO_FLAG_H
