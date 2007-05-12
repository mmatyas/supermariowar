#ifndef _OBJECT_H
#define _OBJECT_H

enum ObjectType{object_none = 0, object_block = 1, object_moving = 2, object_overmap = 3, object_area = 4, object_egg = 5, object_frenzycard = 6, object_yoshi = 7, object_explosion = 8, object_race_goal = 9, object_star = 10, object_flag = 11, object_flagbase = 12, object_thwomp = 13, object_kingofthehill_area = 14, object_bowserfire = 15, object_coin = 16};
enum MovingObjectType{movingobject_none = 0, movingobject_powerup = 1, movingobject_fireball = 2, movingobject_goomba = 3, movingobject_bulletbill = 4, movingobject_hammer = 5, movingobject_poisonpowerup = 6, movingobject_shell = 7, movingobject_throwblock = 8, movingobject_egg = 9, movingobject_star = 10, movingobject_flag = 11, movingobject_cheepcheep = 12, movingobject_koopa = 13, movingobject_mysterymushroompowerup = 15, movingobject_boomerang = 16, movingobject_spring = 17, movingobject_sledgehammer = 18, movingobject_sledgebrother = 19, movingobject_spike = 20, movingobject_bomb = 21, movingobject_superfireball = 22, movingobject_podobo = 23};
enum BlockType{block_none, block_powerup, block_view, block_breakable, block_note, block_donut, block_flip, block_bounce, block_throw, block_onoff_switch, block_onoff};

class IO_MovingObject;

//object base class
class CObject
{
	public:

		CObject(gfxSprite *nspr, short x, short y);
		virtual ~CObject(){};

		virtual void draw(){printf("CObject::draw() - NO!\n");};
		virtual void update(){printf("CObject::update() - NO!\n");};
		virtual bool collide(CPlayer *){printf("CObject::collide() - NO!\n"); return false;};
		virtual void collide(IO_MovingObject *){printf("CObject::collide() - NO!\n");};
		
		virtual ObjectType getObjectType(){printf("CObject::getObjectType() - NO!\n"); return object_none;};
		
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

		//virtual short writeNetworkUpdate(char * pData);
		//virtual void readNetworkUpdate(short size, char * pData);

	protected:
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
};

class IO_Block : public CObject
{
	public:
		IO_Block(gfxSprite *nspr, short x, short y);
		~IO_Block(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player, short direction, bool useBehavior);
		virtual bool collide(IO_MovingObject * object, short direction);
		ObjectType getObjectType(){return object_block;}
		virtual BlockType getBlockType(){printf("CObject::getBlockType() - NO!\n"); return block_none;}

		virtual bool isTransparent() {return false;}

		virtual bool hittop(CPlayer * player, bool useBehavior);
		virtual bool hitbottom(CPlayer * player, bool useBehavior);
		virtual bool hitright(CPlayer * player, bool useBehavior);
		virtual bool hitleft(CPlayer * player, bool useBehavior);

		virtual bool hittop(IO_MovingObject * object);
		virtual bool hitbottom(IO_MovingObject * object);
		virtual bool hitright(IO_MovingObject * object);
		virtual bool hitleft(IO_MovingObject * object);
	
		virtual void triggerBehavior() {}

	protected:
		void BounceMovingObject(IO_MovingObject * object);

		CPlayer * bumpPlayer;
		float fposx, fposy;  //position to return to (for note and bumpable blocks)
		short	  iposx, iposy; //position of the block (doesn't move)

		short col, row;

	friend class CPlayer;
};

class B_PowerupBlock : public IO_Block
{
	public:
		B_PowerupBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed);
		~B_PowerupBlock(){};

		BlockType getBlockType(){return block_powerup;}

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);

		void triggerBehavior();
		virtual short SelectPowerup();

	protected:
		short timer;
		bool side;
		short iNumSprites;
		short animationSpeed;
		short drawFrame;
		short animationTimer;
		short animationWidth;

	friend class CPlayer;
};

class B_ViewBlock : public B_PowerupBlock
{
	public:
		B_ViewBlock(gfxSprite *nspr, short x, short y);
		~B_ViewBlock(){};

		BlockType getBlockType(){return block_view;}

		void draw();
		void update();

		short SelectPowerup();

	protected:
		void GetNextPowerup();

		short poweruptimer;
		short powerupindex;

		bool fNoPowerupsSelected;
		short iCountWeight;
};

class B_BreakableBlock : public IO_Block
{
	public:
		B_BreakableBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed);
		~B_BreakableBlock(){};

		BlockType getBlockType(){return block_breakable;}

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);

		void triggerBehavior();

	private:
		short iNumSprites;
		short animationSpeed;
		short drawFrame;
		short animationTimer;
		short animationWidth;
};


class B_NoteBlock : public IO_Block
{
	public:
		B_NoteBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed);
		~B_NoteBlock(){};

		BlockType getBlockType(){return block_note;}

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);

	private:
		short iNumSprites;
		short animationSpeed;
		short drawFrame;
		short animationTimer;
		short animationWidth;
};

class B_DonutBlock : public IO_Block
{
	public:
		B_DonutBlock(gfxSprite *nspr, short x, short y);
		~B_DonutBlock(){};

		BlockType getBlockType(){return block_donut;}

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);

		void triggerBehavior();

	private:
		short counter;
		short jigglex;
		short jigglecounter;
};

class B_FlipBlock : public IO_Block
{
	public:
		B_FlipBlock(gfxSprite *nspr, short x, short y);
		~B_FlipBlock(){};

		BlockType getBlockType(){return block_flip;}

		void draw();
		void update();
		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool isTransparent() {return state != 0;}

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitbottom(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);
	
		void triggerBehavior();

	private:
		short counter;
		short frame;
		short timer;
		short animationWidth;
};

class B_BounceBlock : public IO_Block
{
	public:
		B_BounceBlock(gfxSprite *nspr, short x, short y);
		~B_BounceBlock(){};

		BlockType getBlockType(){return block_bounce;}

		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		void triggerBehavior();
};

class B_ThrowBlock : public IO_Block
{
	public:
		B_ThrowBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed);
		~B_ThrowBlock(){};

		BlockType getBlockType(){return block_throw;}

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);

		void GiveBlockToPlayer(CPlayer * player);
		void triggerBehavior();
		void SetType(bool superblock);

	private:
		short iNumSprites;
		short animationSpeed;
		short drawFrame;
		short animationTimer;
		short animationWidth;
		bool fSuper;
};


class B_OnOffSwitchBlock : public IO_Block
{
	public:
		B_OnOffSwitchBlock(gfxSprite *nspr, short x, short y, short colorID, short state);
		~B_OnOffSwitchBlock(){};

		BlockType getBlockType(){return block_onoff_switch;}

		void update();
		void draw();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);

		void FlipState() {state = (state < 3 ? state + 3 : state - 3);}

		void triggerBehavior();

	private:
		short iColorID;
		short iSrcX;
};

class B_SwitchBlock : public IO_Block
{
	public:
		B_SwitchBlock(gfxSprite *nspr, short x, short y, short colorID, short state);
		~B_SwitchBlock(){};

		BlockType getBlockType(){return block_onoff;}

		void draw();
		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool isTransparent() {return state != 0;}

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitbottom(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);

		void FlipState() {state = 1 - state;}		

	private:
		short iSrcX;
};


class IO_MovingObject : public CObject
{
	public:
		IO_MovingObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1, short iAnimationOffsetX = -1, short iAnimationOffsetY = -1, short iAnimationHeight = -1, short iAnimationWidth = -1);
		virtual ~IO_MovingObject(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player);
		
		void collide(IO_MovingObject *){}
		ObjectType getObjectType(){return objectType;}
		MovingObjectType getMovingObjectType() {return movingObjectType;}
		void collision_detection_map();
		bool collision_detection_checksides();
		void flipsidesifneeded();

		virtual void SideBounce() {}
		virtual float BottomBounce() {return bounce;}
		void KillObjectMapHazard();

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

		ObjectType objectType;
		MovingObjectType movingObjectType;

		//Pointer to the platform the object is riding
		MovingPlatform * platform;
		short iHorizontalPlatformCollision;
		short iVerticalPlatformCollision;

	friend class IO_Block;
	friend class B_PowerupBlock;
	friend class B_BreakableBlock;
	friend class B_NoteBlock;
	friend class B_DonutBlock;
	friend class B_FlipBlock;
	friend class B_BounceBlock;
	friend class OMO_Explosion;
	friend class B_OnOffSwitchBlock;
	friend class B_SwitchBlock;
	friend class MO_SledgeBrother;
	friend class MovingPlatform;

	friend void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead);
	friend void RunGame();
};

class MO_Powerup : public IO_MovingObject
{
	public:
		MO_Powerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1);
		virtual ~MO_Powerup(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player);
		MovingObjectType getMovingObjectType() {return movingObjectType;}

	protected:
		float desty;
};

class PU_Tanooki : public MO_Powerup
{
    public:
        PU_Tanooki(short x, short y);
        bool collide(CPlayer *player);
};

class PU_StarPowerup : public MO_Powerup
{
	public:
		PU_StarPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_StarPowerup(){};

		bool collide(CPlayer * player);

};

class PU_ExtraGuyPowerup : public MO_Powerup
{
	public:
		PU_ExtraGuyPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short type);
		~PU_ExtraGuyPowerup(){};

		bool collide(CPlayer * player);

	private:
		short iType;
};

class PU_PoisonPowerup : public MO_Powerup
{
	public:
		PU_PoisonPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_PoisonPowerup(){};

		bool collide(CPlayer * player);
};

class PU_MysteryMushroomPowerup : public MO_Powerup
{
	public:
		PU_MysteryMushroomPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_MysteryMushroomPowerup(){};

		bool collide(CPlayer * player);
};

class PU_FirePowerup : public MO_Powerup
{
	public:
		PU_FirePowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_FirePowerup(){};

		bool collide(CPlayer * player);

};

class PU_HammerPowerup : public MO_Powerup
{
	public:
		PU_HammerPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_HammerPowerup(){};

		bool collide(CPlayer * player);
};

class PU_SledgeHammerPowerup : public MO_Powerup
{
	public:
		PU_SledgeHammerPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_SledgeHammerPowerup(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		float BottomBounce();
	
	private:
		short sparkleanimationtimer;
		short sparkledrawframe;
		short numbounces;
};

class PU_PodoboPowerup : public PU_SledgeHammerPowerup
{
	public:
		PU_PodoboPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_PodoboPowerup(){};

		bool collide(CPlayer * player);
};

class PU_ClockPowerup : public MO_Powerup
{
	public:
		PU_ClockPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_ClockPowerup(){};

		bool collide(CPlayer * player);
};

class PU_BobombPowerup : public MO_Powerup
{
	public:
		PU_BobombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BobombPowerup(){};

		bool collide(CPlayer * player);
};

class PU_BombPowerup : public PU_SledgeHammerPowerup
{
	public:
		PU_BombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BombPowerup(){};

		bool collide(CPlayer * player);
};

class PU_PowPowerup : public MO_Powerup
{
	public:
		PU_PowPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_PowPowerup(){};

		bool collide(CPlayer * player);
};

class PU_ModPowerup : public MO_Powerup
{
	public:
		PU_ModPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_ModPowerup(){};

		bool collide(CPlayer * player);
};

class PU_BulletBillPowerup : public MO_Powerup
{
	public:
		PU_BulletBillPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BulletBillPowerup(){};

		bool collide(CPlayer * player);
};

class PU_FeatherPowerup : public IO_MovingObject
{
	public:
		PU_FeatherPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_FeatherPowerup(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

	private:
		bool fFloatDirectionRight;
		float dFloatAngle;
		float desty;
		float dFloatCenterX, dFloatCenterY;
};

class PU_BoomerangPowerup : public MO_Powerup
{
	public:
		PU_BoomerangPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BoomerangPowerup(){};

		bool collide(CPlayer * player);
};

class MO_Fireball : public IO_MovingObject
{
	public:
		MO_Fireball(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);
		~MO_Fireball(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short playerID;
		short teamID;
		short colorOffset;

	private:
		short ttl;
};

class MO_SuperFireball : public IO_MovingObject
{
	public:
		MO_SuperFireball(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);
		~MO_SuperFireball(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short playerID;
		short teamID;

	private:
		short colorOffset;
		short directionOffset;
		short ttl;
};

class MO_Hammer : public IO_MovingObject
{
	public:
		MO_Hammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);
		~MO_Hammer(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short playerID;
		short teamID;
		short colorOffset;

	private:
		short ttl;
		bool fSuper;
};

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

class MO_Boomerang : public IO_MovingObject
{
	public:
		MO_Boomerang(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superBoomerang);
		~MO_Boomerang(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short playerID;
		short teamID;
		short colorOffset;

	private:
		void forcedead();

		bool fMoveToRight;
		bool fFlipped;
		short iStateTimer;

		short iStyle;

		bool fSuperBoomerang;
		short smoketimer;
};

class MO_Coin : public IO_MovingObject
{
	public:
		MO_Coin(gfxSprite *nspr, short iNumSpr, short aniSpeed);
		~MO_Coin(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeCoin();

	private:
		short timer;
		short sparkleanimationtimer;
		short sparkledrawframe;
};

class IO_OverMapObject : public CObject
{
	public:
		IO_OverMapObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed);
		virtual ~IO_OverMapObject(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer *){return false;}
		virtual void collide(IO_MovingObject *){}

		ObjectType getObjectType(){return objectType;}
		MovingObjectType getMovingObjectType() {return movingObjectType;}
		
	protected:

		short iNumSprites;
		short drawframe;
		short animationtimer;
		short animationWidth;

		float bounce;

		short animationspeed;

		ObjectType objectType;
		MovingObjectType movingObjectType;
};

class OMO_Thwomp : public IO_OverMapObject
{
	public:
		OMO_Thwomp(gfxSprite *nspr, short x, float nspeed);
		~OMO_Thwomp(){};

		void update();
		bool collide(CPlayer * player);
};

class OMO_Podobo : public IO_MovingObject
{
	public:
		OMO_Podobo(gfxSprite *nspr, short x, float nspeed, short playerid, short teamid, short colorid);
		~OMO_Podobo(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

		short iPlayerID;

	private:
		
		short iTeamID;
		short iColorOffsetY;

};

class OMO_BowserFire : public IO_OverMapObject
{
	public:
		OMO_BowserFire(gfxSprite *nspr, short x, short y, float velx, float vely, short id, short teamid, short colorid);
		~OMO_BowserFire(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

	private:
		short iPlayerID;
		short iTeamID;
		short iColorOffsetY;
};

class MO_CarriedObject : public IO_MovingObject
{
	public:
		MO_CarriedObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth);
		MO_CarriedObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~MO_CarriedObject();

		virtual void update() {}
		virtual void draw() {}
		virtual bool collide(CPlayer *) {return false;}

		virtual void MoveToOwner(){}

		virtual void Drop() {}
		virtual void Kick(bool superkick) {}

	protected:

		CPlayer * owner;
		bool fSmoking;

	friend class B_ThrowBlock;
};

class CO_Egg : public MO_CarriedObject
{
	public:
		CO_Egg(gfxSprite *nspr);
		~CO_Egg(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		
		void MoveToOwner();

		void placeEgg();
		void Drop();
		void Kick(bool superkick);

	private:
		short timer;
		CPlayer * owner_throw;
		short owner_throw_timer;
		
		short sparkleanimationtimer;
		short sparkledrawframe;
	
	friend class CPlayer;
	friend class OMO_Yoshi;
	friend class CGM_Eggs;
};

class CO_Star : public MO_CarriedObject
{
	public:
		CO_Star(gfxSprite *nspr);
		~CO_Star(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void MoveToOwner();

		void placeStar();
		void Drop();
		void Kick(bool superkick);

	private:
		short timer;
		short iOffsetY;
		short sparkleanimationtimer;
		short sparkledrawframe;

	friend class CPlayer;
	friend class CGM_Star;
};

//Declaring class for use in OMO_FlagBase (some compilers complain if this isn't here)
class CO_Flag;

class OMO_FlagBase : public IO_OverMapObject
{
	public:
		OMO_FlagBase(gfxSprite *nspr, short iTeamID, short iColorID);
		~OMO_FlagBase(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void placeFlagBase();
		void scoreFlag(CO_Flag * flag, CPlayer * player);
		void setFlag(CO_Flag * flag) {homeflag = flag;}

		short GetTeamID() {return teamID;}
	
	private:
		short teamID;
		short iGraphicOffsetX;
		float angle;
		float anglechange;
		short anglechangetimer;
		float speed;
		CO_Flag * homeflag;

	friend class CO_Flag;
	friend class CPlayer;
};

class CO_Flag : public MO_CarriedObject
{
	public:
		CO_Flag(gfxSprite *nspr, OMO_FlagBase * base, short iTeamID, short iColorID);
		~CO_Flag(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void MoveToOwner();

		void placeFlag();
		void Drop();
		void Kick(bool superkick);

		bool GetInBase() {return fInBase;}
		short GetTeamID() {return teamID;}

	private:
		short timer;
		OMO_FlagBase * flagbase;
		short teamID;
		bool fLastFlagDirection;
		bool fInBase;
		CPlayer * owner_throw;
		short owner_throw_timer;

	friend class CPlayer;
	friend class CGM_CTF;
	friend class OMO_FlagBase;
};

class OMO_Yoshi : public IO_OverMapObject
{
	public:
		OMO_Yoshi(gfxSprite *nspr);
		~OMO_Yoshi(){};

		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void placeYoshi();

	private:
		short timer;
};

class OMO_Area : public IO_OverMapObject
{
	public:
		OMO_Area(gfxSprite *nspr, short numAreas);
		~OMO_Area(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void placeArea();
		void reset();
		short getColorID() {return colorID;}
		void setOwner(CPlayer * player);
	
	private:
		short playerID;
		short colorID;
		short teamID;
		short scoretimer;
		short frame;
		short relocatetimer;
		short numareas;
		short totalTouchingPlayers;
		CPlayer * touchingPlayer;

	friend class CObjectContainer;
};

class OMO_KingOfTheHillZone : public IO_OverMapObject
{
	public:
		OMO_KingOfTheHillZone(gfxSprite *nspr);
		~OMO_KingOfTheHillZone(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void placeArea();
		void reset();
		short getColorID() {return colorID;}
	
	private:
		CPlayer * playersTouching[4];
		short playersTouchingCount[4];
		short totalTouchingPlayers;

		short playerID;
		short colorID;
		short scoretimer;
		short frame;
		short relocatetimer;
		short size;

	friend class CObjectContainer;
};

class OMO_RaceGoal : public IO_OverMapObject
{
	public:
		OMO_RaceGoal(gfxSprite *nspr, short id);
		~OMO_RaceGoal(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void placeRaceGoal();
		void reset(short teamID) {tagged[teamID] = -1;}
		short isTagged(short teamID) {return tagged[teamID];}
		short getGoalID() {return goalID;}
	
	private:
		short tagged[4];
		short goalID;
		float angle;
		float anglechange;
		short anglechangetimer;
		float speed;
			
	friend class CObjectContainer;
};

class MO_FrenzyCard : public IO_MovingObject
{
	public:
		MO_FrenzyCard(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iType);
		~MO_FrenzyCard(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeFrenzyCard();

	private:
		short timer;
		short type;

		short sparkleanimationtimer;
		short sparkledrawframe;
};

class OMO_Explosion : public IO_OverMapObject
{
	public:
		OMO_Explosion(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short id, short iTeamID);
		~OMO_Explosion(){};

		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
	private:
		short playerID;
		short teamID;
		short timer;
};

class MO_Goomba : public IO_MovingObject
{
	public:
		MO_Goomba(gfxSprite *nspr, short iNumSpr, short aniSpeed, bool moveToRight, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		virtual ~MO_Goomba(){};

		void draw();
		virtual void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void place();

		virtual bool hittop(CPlayer * player);
		bool hitother(CPlayer * player);

		virtual void Die();

	protected:
		float spawnradius;
		float spawnangle;

		short iSpawnIconOffset;
		killstyle killStyle;

		short burnuptimer;
};

class MO_Koopa : public MO_Goomba
{
	public:
		MO_Koopa(gfxSprite *nspr, short iNumSpr, short aniSpeed, bool moveToRight, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, bool red);
		~MO_Koopa(){};

		void update();
		bool hittop(CPlayer * player);
		void Die();

	private:
		float spawnradius;
		float spawnangle;

		bool fRed;
};

class OMO_CheepCheep : public IO_MovingObject
{
	public:
		OMO_CheepCheep(gfxSprite *nspr);
		~OMO_CheepCheep(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void place();

		bool hittop(CPlayer * player);
		bool hitother(CPlayer * player);

		void Die();

	private:
		short iColorOffsetY;
};

class OMO_BulletBill : public IO_MovingObject
{
	public:
		OMO_BulletBill(gfxSprite *nspr, short y, float nspeed, short playerID, bool homing);
		~OMO_BulletBill(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

		bool hittop(CPlayer * player);
		bool hitother(CPlayer * player);
		
		void Die();
		void HomeToNearestPlayer();
		void SetDirectionOffset();

	private:
		short iPlayerID;
		short iColorID;
		short iTeamID;
		
		short iColorOffsetY;
		short iDirectionOffsetY;

		bool fHoming;
		CPlayer * pHomingPlayer;
		float fMaxVel;
		float fGoalVelX;
		float fGoalVelY;

	friend class MO_Goomba;
	friend class OMO_CheepCheep;
	friend class OMO_Podobo;
};

class MO_SledgeBrother : public IO_MovingObject
{
	public:
		MO_SledgeBrother(gfxSprite *nspr, short platformY, short type);
		virtual ~MO_SledgeBrother(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

		bool hit(CPlayer * player);

		void Die();
		void Damage(short playerID);

	protected:
		
		short iType;

		short iActionState;
		short iDestLocationX[5];

		void randomaction();
		void move(bool moveright);
		void throwprojectile();
		void taunt();
		void turn();
		void jump();
		void wait(short min, short max);

		short location;

		short throwing_timer;
		
		short hit_timer;
		short hit_movement_timer;
		short hit_offset_y;

		short leg_offset_x;
		short leg_movement_timer;

		short arm_offset_x;
		short arm_movement_timer;

		short taunt_timer;
		short wait_timer;

		short hit_points;
		bool face_right;

		short iDestX;
		
		short iPlatformY;
		short need_attack;
};

class CO_Shell : public MO_CarriedObject
{
	public:
		CO_Shell(short type, short x, short y, bool dieOnMovingPlayerCollision, bool dieOnHoldingPlayerCollision, bool dieOnFire, bool killBouncePlayer);
		~CO_Shell(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		bool HitTop(CPlayer * player);
		bool HitOther(CPlayer * player);

		void UsedAsStoredPowerup(CPlayer * player);

		void MoveToOwner();

		bool KillPlayer(CPlayer * player);
		void Drop();
		void Kick(bool superkick);

		void collide(IO_MovingObject * object);
		void CheckAndDie();
		void Die();

		void SideBounce();
		void AddMovingKill(CPlayer * killer);

		bool IsThreat() {return state == 1 || state == 3;}

	private:
		short iShellType;

		short playerID;
		short iIgnoreBounceTimer;
		short iDestY;

		bool fDieOnMovingPlayerCollision;
		bool fDieOnHoldingPlayerCollision;
		bool fDieOnFire;
		bool fKillBouncePlayer;

		short iDeathTime;
		
		short iColorOffsetY;
		short iBounceCounter;
		short iKillCounter;

	friend class CPlayer;
	friend class OMO_Explosion;
	friend class OMO_BulletBill;
	friend class MO_Goomba;
	friend class MO_Koopa;
	friend class MO_SledgeBrother;
	friend class OMO_CheepCheep;
	friend void RunGame();
};

class CO_ThrowBlock : public MO_CarriedObject
{
	public:
		CO_ThrowBlock(gfxSprite * nspr, short x, short y, bool superblock);
		~CO_ThrowBlock(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		bool HitTop(CPlayer * player);
		bool HitOther(CPlayer * player);

		void MoveToOwner();

		bool KillPlayer(CPlayer * player);
		void Drop();
		void Kick(bool superkick);

		void collide(IO_MovingObject * object);
		void Die();

		void SideBounce();

	private:
		
		short playerID;
		short iDeathTime;
		bool fSuper;
		short iBounceCounter;

	friend class CPlayer;
	friend class OMO_Explosion;
	friend class OMO_BulletBill;
	friend class MO_Goomba;
	friend class MO_SledgeBrother;
	friend class OMO_CheepCheep;
	friend class B_ThrowBlock;
	friend void RunGame();
};

class CO_Spring : public MO_CarriedObject
{
	public:
		CO_Spring(gfxSprite *nspr);
		~CO_Spring(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void MoveToOwner();

		void place();
		void Drop();
		void Kick(bool superkick);

	protected:

		virtual void hittop(CPlayer * player);
		void hitother(CPlayer * player);

		float spawnradius;
		float spawnangle;

		short iSpawnIconX;

	friend class CPlayer;
};

class CO_Spike : public CO_Spring
{
	public:
		CO_Spike(gfxSprite *nspr);
		~CO_Spike(){};

	private:

		void hittop(CPlayer * player);
};

class CO_Bomb : public MO_CarriedObject
{
	public:
		CO_Bomb(gfxSprite *nspr, short x, short y, float fVelX, float fVelY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, short timetolive);
		~CO_Bomb(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void MoveToOwner();

		void place();
		void Drop();
		void Kick(bool superkick);

		void Die();

		short playerID;
		short teamID;
		
	protected:
		
		short iColorOffsetY;
		short ttl;

	friend class CPlayer;
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
			for(short i = 0; i < list_end; i++)
				list[i]->draw();
		};

		void clean();

		bool isBlockAt(short x, short y);

		void cleandeadobjects();

		float getClosestObject(short x, short y, short objectType);
		short countTypes(ObjectType type);
		void adjustPlayerAreas(CPlayer * player, CPlayer * other);
		void removePlayerRaceGoals(short id, short iGoal);

		CObject * getRandomObject();

	public:
		CObject *list[MAXOBJECTS];
		short		 list_end;
};

class MysteryMushroomTempPlayer
{
	public:
		MysteryMushroomTempPlayer()
		{
			fUsed = false;
		}

		~MysteryMushroomTempPlayer() {}

		void SetPlayer(CPlayer * player, short iPowerup);
		void GetPlayer(CPlayer * player, short * iPowerup);

		bool fUsed;

	private:
		
		float fx, fy;
		float fOldX, fOldY;
		float velx, vely;

		bool bobomb;
		short powerup;

		short burnupstarttimer, burnuptimer;

		bool inair, onice, invincible;
		short invincibletimer;

		MovingPlatform * platform;

		short iCapeFrameX, iCapeFrameY, iCapeTimer, iCapeYOffset;
		short gamepowerup;

		short iOldPowerupX, iOldPowerupY;
};

#endif

