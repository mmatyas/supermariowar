#ifndef _OBJECT_H
#define _OBJECT_H

enum ObjectType{object_none = 0, object_block = 1, object_moving = 2, object_overmap = 3, object_area = 4, object_frenzycard = 5, object_race_goal = 6, object_thwomp = 7, object_kingofthehill_area = 8, object_bowserfire = 9, object_orbithazard = 10, object_bulletbillcannon = 11, object_flamecannon = 12, object_pathhazard = 13, object_pipe_coin = 14, object_pipe_bonus = 15, object_phanto = 16};
enum MovingObjectType{movingobject_none = 0, movingobject_powerup = 1, movingobject_fireball = 2, movingobject_goomba = 3, movingobject_bulletbill = 4, movingobject_hammer = 5, movingobject_poisonpowerup = 6, movingobject_shell = 7, movingobject_throwblock = 8, movingobject_egg = 9, movingobject_star = 10, movingobject_flag = 11, movingobject_cheepcheep = 12, movingobject_koopa = 13, movingobject_boomerang = 14, movingobject_carried = 15, movingobject_iceblast = 16, movingobject_bomb = 17, movingobject_podobo = 18, movingobject_treasurechest = 19, movingobject_attackzone = 20, movingobject_pirhanaplant = 21, movingobject_explosion = 22, movingobject_buzzybeetle = 23, movingobject_spiny = 24, movingobject_phantokey = 25, movingobject_flagbase = 26, movingobject_yoshi = 27, movingobject_coin = 28, movingobject_collectioncard = 29, movingobject_sledgebrother = 30, movingobject_sledgehammer = 31, movingobject_superfireball = 32, movingobject_throwbox = 33, MOVINGOBJECT_LAST};
enum BlockType{block_none, block_powerup, block_view, block_breakable, block_note, block_donut, block_flip, block_bounce, block_throw, block_onoff_switch, block_onoff, block_weaponbreakable};

class IO_MovingObject;

//object base class
class CObject
{
	public:

		CObject(gfxSprite *nspr, short x, short y);
		virtual ~CObject(){};

		virtual void draw(){};
		virtual void update() = 0;
		virtual bool collide(CPlayer *){return false;}
		virtual void collide(IO_MovingObject *){}
		
		virtual ObjectType getObjectType(){return objectType;}
		
		void xf(float xf){fx = xf; ix = (short)fx;};
 		void xi(short xi){ix = xi; fx = (float)ix;};
		void yf(float yf){fy = yf; if(fy < 0.0f) iy = (short)(fy - 1.0f); else iy = (short)fy;};
		void yi(short yi){iy = yi; fy = (float)iy;};

		int iNetworkID;
		short ix, iy;

		short collisionWidth;
		short collisionHeight;
		short collisionOffsetX;
		short collisionOffsetY;

		short GetState() {return state;}
		bool GetDead() {return dead;}

		bool GetWrap() { if (spr) return spr->GetWrap(); return true; }

		void GetCollisionBlocks(IO_Block * blocks[4]);

		//virtual short writeNetworkUpdate(char * pData);
		//virtual void readNetworkUpdate(short size, char * pData);

	protected:
		ObjectType objectType;

		short iw, ih;
		float fx, fy;
		float velx, vely;
		
		gfxSprite *spr;
		short state;
		bool dead;

		short index;

	friend class CObjectContainer;
	friend class CPlayer;
	friend void RunGame();
	friend class CO_Shell;
	friend class B_BreakableBlock;
	friend class B_WeaponBreakableBlock;
};

class IO_MovingObject : public CObject
{
	public:
		IO_MovingObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1, short iAnimationOffsetX = -1, short iAnimationOffsetY = -1, short iAnimationHeight = -1, short iAnimationWidth = -1);
		virtual ~IO_MovingObject(){};

		virtual void draw();
		virtual void update();
		virtual void animate();

		virtual bool collide(CPlayer * player);
		
		void collide(IO_MovingObject *){}
		MovingObjectType getMovingObjectType() {return movingObjectType;}
		void applyfriction();
		void collision_detection_map();
		bool collision_detection_checksides();
		void flipsidesifneeded();

		virtual void SideBounce(bool fRightSide) {}
		virtual float BottomBounce() {return bounce;}
		void KillObjectMapHazard(short playerID = -1);

		virtual void CheckAndDie() {dead = true;}
		virtual void Die() {dead = true;}

		bool CollidesWithMap() {return fObjectCollidesWithMap;}

		short iPlayerID;
		short iTeamID;

	protected:
		float fOldX, fOldY;
		float fPrecalculatedY;

		short iNumSprites;
		short drawframe;
		short animationtimer;

		float bounce;

		short animationspeed;
		short animationWidth;
		short animationOffsetX;
		short animationOffsetY;

		bool inair;
		bool onice;

		MovingObjectType movingObjectType;

		//Pointer to the platform the object is riding
		MovingPlatform * platform;
		short iHorizontalPlatformCollision;
		short iVerticalPlatformCollision;

		bool fObjectDiesOnSuperDeathTiles;
		bool fObjectCollidesWithMap;

	friend class IO_Block;
	friend class B_PowerupBlock;
	friend class B_BreakableBlock;
	friend class B_NoteBlock;
	friend class B_DonutBlock;
	friend class B_FlipBlock;
	friend class B_BounceBlock;
	friend class MO_Explosion;
	friend class B_OnOffSwitchBlock;
	friend class B_SwitchBlock;
	friend class B_WeaponBreakableBlock;

	friend class MO_BulletBill;
	friend class MO_WalkingEnemy;
	friend class MO_Goomba;
	friend class MO_CheepCheep;
	friend class MO_PirhanaPlant;

	friend class MovingPlatform;

	friend void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead);
	friend void RunGame();
};

class IO_OverMapObject : public CObject
{
	public:
		IO_OverMapObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1, short iAnimationOffsetX = -1, short iAnimationOffsetY = -1, short iAnimationHeight = -1, short iAnimationWidth = -1);
		virtual ~IO_OverMapObject(){};

		virtual void draw();
		virtual void draw(short iOffsetX, short iOffsetY);
		virtual void update();
		virtual void animate();

		virtual bool collide(CPlayer *){return false;}
		virtual void collide(IO_MovingObject *){}
		
	protected:

		short iNumSprites;
		short drawframe;
		short animationtimer;
		short animationWidth;

		short animationOffsetX, animationOffsetY;
		short animationspeed;

};

//object container
class CObjectContainer
{
	public:
		CObjectContainer();
		~CObjectContainer();

		void add(CObject *cio);

		void update()
		{
			for(short i = 0; i < list_end; i++)
				list[i]->update();
		};

		void draw()
		{
			for(short i = list_end - 1; i >= 0; i--)
				list[i]->draw();
		};

		void clean();

		bool isBlockAt(short x, short y);

		void cleandeadobjects();

		float getClosestObject(short x, short y, short objectType);
		float getClosestMovingObject(short x, short y, short movingObjectType);
		short countTypes(ObjectType type);
		short countMovingTypes(MovingObjectType type);
		void adjustPlayerAreas(CPlayer * player, CPlayer * other);
		void removePlayerRaceGoals(short id, short iGoal);
		void pushBombs(short x, short y);

		CObject * getRandomObject();

	public:
		CObject *list[MAXOBJECTS];
		short		 list_end;
};

#endif //_OBJECT_H

