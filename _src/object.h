#ifndef _OBJECT_H
#define _OBJECT_H

enum ObjectType{object_none = 0, object_block = 1, object_moving = 2, object_overmap = 3, object_area = 4, object_egg = 5, object_frenzycard = 6, object_yoshi = 7, object_race_goal = 8, object_star = 9, object_flag = 10, object_flagbase = 11, object_thwomp = 12, object_kingofthehill_area = 13, object_bowserfire = 14, object_coin = 15, object_collectioncard = 16, object_orbithazard = 17, object_bulletbillcannon = 18, object_flamecannon = 19, object_pathhazard = 20};
enum MovingObjectType{movingobject_none = 0, movingobject_powerup = 1, movingobject_fireball = 2, movingobject_goomba = 3, movingobject_bulletbill = 4, movingobject_hammer = 5, movingobject_poisonpowerup = 6, movingobject_shell = 7, movingobject_throwblock = 8, movingobject_egg = 9, movingobject_star = 10, movingobject_flag = 11, movingobject_cheepcheep = 12, movingobject_koopa = 13, movingobject_boomerang = 14, movingobject_carried = 15, movingobject_iceblast = 16, movingobject_bomb = 17, movingobject_podobo = 18, movingobject_treasurechest = 19, movingobject_attackzone = 20, movingobject_pirhanaplant = 21, movingobject_explosion = 22, movingobject_buzzybeetle = 23, movingobject_spiny = 24, MOVINGOBJECT_LAST};
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

class IO_Block : public CObject
{
	public:
		IO_Block(gfxSprite *nspr, short x, short y);
		~IO_Block(){};

		virtual void draw();
		virtual void update();
		virtual void reset();

		//Returns true if we should continue to check for collisions in this direction
		virtual bool collide(CPlayer * player, short direction, bool useBehavior);
		virtual bool collide(IO_MovingObject * object, short direction);
		
		virtual BlockType getBlockType() = 0;

		virtual bool isTransparent() {return false;}
		virtual bool isHidden() {return hidden;}

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

		short iBumpPlayerID;
		short iBumpTeamID;

		float fposx, fposy;  //position to return to (for note and bumpable blocks)
		short iposx, iposy; //position of the block (doesn't move)

		bool hidden, oldhidden, ishiddentype;
		short iHiddenTimer;

		short col, row;

	friend class CPlayer;
};

class B_PowerupBlock : public IO_Block
{
	public:
		B_PowerupBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, bool fHidden, short * piSettings);
		~B_PowerupBlock(){};

		BlockType getBlockType(){return block_powerup;}

		void draw();
		void update();
		void reset();

		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool collide(IO_MovingObject * object, short direction);

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);

		void triggerBehavior();
		virtual short SelectPowerup();

	protected:
		short iCountWeight;

		short timer;
		bool side;
		short iNumSprites;
		short animationSpeed;
		short drawFrame;
		short animationTimer;
		short animationWidth;

		short settings[NUM_POWERUPS];

	friend class CPlayer;
};

class B_ViewBlock : public B_PowerupBlock
{
	public:
		B_ViewBlock(gfxSprite *nspr, short x, short y, bool fHidden, short * piSettings);
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
		B_NoteBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short type, bool fHidden);
		~B_NoteBlock(){};

		BlockType getBlockType(){return block_note;}

		void draw();
		void update();
		void reset();

		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool collide(IO_MovingObject * object, short direction);

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
		short iType;
		short iTypeOffsetY;
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
		B_FlipBlock(gfxSprite *nspr, short x, short y, bool fHidden);
		~B_FlipBlock(){};

		BlockType getBlockType(){return block_flip;}

		void draw();
		void update();
		void reset();

		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool collide(IO_MovingObject * object, short direction);

		bool isTransparent() {return state == 1;}

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
		
		void explode();

		short counter;
		short frame;
		short timer;
		short animationWidth;
};

class B_BounceBlock : public IO_Block
{
	public:
		B_BounceBlock(gfxSprite *nspr, short x, short y, bool fHidden);
		~B_BounceBlock(){};

		BlockType getBlockType(){return block_bounce;}

		void update();
		void draw();
		void reset();

		bool collide(CPlayer * player, short direction, bool useBehavior);
		bool collide(IO_MovingObject * object, short direction);

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);

		void triggerBehavior();
};

class B_ThrowBlock : public IO_Block
{
	public:
		B_ThrowBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short type);
		~B_ThrowBlock(){};

		BlockType getBlockType(){return block_throw;}

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);

		void GiveBlockToPlayer(CPlayer * player);
		void triggerBehavior();

	private:
		short iNumSprites;
		short animationSpeed;
		short drawFrame;
		short animationTimer;
		short animationWidth;
		short iType;
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

		void FlipState();

	private:
		short iSrcX;
};

class B_WeaponBreakableBlock : public IO_Block
{
	public:
		B_WeaponBreakableBlock(gfxSprite *nspr, short x, short y, short type);
		~B_WeaponBreakableBlock(){};

		BlockType getBlockType(){return block_weaponbreakable;}

		void draw();
		void update();

		bool hittop(CPlayer * player, bool useBehavior);
		bool hitbottom(CPlayer * player, bool useBehavior);
		bool hitleft(CPlayer * player, bool useBehavior);
		bool hitright(CPlayer * player, bool useBehavior);

		bool hittop(IO_MovingObject * object);
		bool hitbottom(IO_MovingObject * object);
		bool hitright(IO_MovingObject * object);
		bool hitleft(IO_MovingObject * object);

		void triggerBehavior(short iPlayerID, short iTeamID);

	private:
		bool objecthitside(IO_MovingObject * object);

		short iType;
		short iDrawOffsetX;

	friend class MO_Hammer;
	friend class MO_Boomerang;
	friend class MO_Explosion;
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

		virtual void SideBounce() {}
		virtual float BottomBounce() {return bounce;}
		void KillObjectMapHazard();

		virtual void CheckAndDie() {dead = true;}
		virtual void Die() {dead = true;}

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

	//friend class MO_SledgeBrother;
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

	protected:
		float desty;
};

class PU_Tanooki : public MO_Powerup
{
    public:
        PU_Tanooki(short x, short y);
        bool collide(CPlayer *player);
};

class PU_PWingsPowerup : public MO_Powerup
{
    public:
        PU_PWingsPowerup(gfxSprite * nspr, short x, short y);
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

class PU_IceWandPowerup : public MO_Powerup
{
	public:
		PU_IceWandPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_IceWandPowerup(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
	
	private:
		short sparkleanimationtimer;
		short sparkledrawframe;
};

class PU_PodoboPowerup : public MO_Powerup
{
	public:
		PU_PodoboPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_PodoboPowerup(){};

		bool collide(CPlayer * player);
};

class PU_TreasureChestBonus : public MO_Powerup
{
	public:
		PU_TreasureChestBonus(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iBonusItem);
		~PU_TreasureChestBonus(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		float BottomBounce();
	
	private:
		short sparkleanimationtimer;
		short sparkledrawframe;
		short numbounces;
		short bonusitem;
		short drawbonusitemx, drawbonusitemy;
		short drawbonusitemtimer;
};

class MO_BonusHouseChest : public IO_MovingObject
{
	public:
		MO_BonusHouseChest(gfxSprite *nspr, short ix, short iy, short iBonusItem);
		~MO_BonusHouseChest(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
	
	private:
		short bonusitem;
		short drawbonusitemy;
		short drawbonusitemtimer;
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

class PU_BombPowerup : public MO_Powerup
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
		PU_FeatherPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_FeatherPowerup(){};

		void update();
		void draw();
		virtual bool collide(CPlayer * player);

	private:
		bool fFloatDirectionRight;
		float dFloatAngle;
		float desty;
		float dFloatCenterX, dFloatCenterY;
};

class PU_LeafPowerup : public PU_FeatherPowerup
{
	public:
		PU_LeafPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_LeafPowerup(){};

		bool collide(CPlayer * player);
};

class PU_BoomerangPowerup : public MO_Powerup
{
	public:
		PU_BoomerangPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BoomerangPowerup(){};

		bool collide(CPlayer * player);
};

class PU_ExtraHeartPowerup : public MO_Powerup
{
	public:
		PU_ExtraHeartPowerup(gfxSprite *nspr, short x, short y);
		~PU_ExtraHeartPowerup(){};

		bool collide(CPlayer * player);
};

class PU_ExtraTimePowerup : public MO_Powerup
{
	public:
		PU_ExtraTimePowerup(gfxSprite *nspr, short x, short y);
		~PU_ExtraTimePowerup(){};

		bool collide(CPlayer * player);
};

class PU_JailKeyPowerup : public MO_Powerup
{
	public:
		PU_JailKeyPowerup(gfxSprite *nspr, short x, short y);
		~PU_JailKeyPowerup(){};

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

		short colorOffset;

	private:
		short ttl;
};

/*
class MO_SuperFireball : public IO_MovingObject
{
	public:
		MO_SuperFireball(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);
		~MO_SuperFireball(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

	private:
		short colorOffset;
		short directionOffset;
		short ttl;
};
*/

class MO_Hammer : public IO_MovingObject
{
	public:
		MO_Hammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);
		~MO_Hammer(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short colorOffset;

	private:
		short ttl;
		bool fSuper;
};

class MO_IceBlast : public IO_MovingObject
{
	public:
		MO_IceBlast(gfxSprite *nspr, short x, short y, float fVelyX, short iGlobalID, short iTeamID, short iColorID);
		~MO_IceBlast(){};

		void update();
		bool collide(CPlayer * player);

		short colorOffset;
};

class MO_Boomerang : public IO_MovingObject
{
	public:
		MO_Boomerang(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);
		~MO_Boomerang(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

		short colorOffset;

	private:
		void forcedead();

		bool fMoveToRight;
		bool fFlipped;
		short iStateTimer;

		short iStyle;
};

class MO_Coin : public IO_MovingObject
{
	public:
		MO_Coin(gfxSprite *nspr, float velx, float vely, short ix, short iy, short color, short type, short uncollectabletime);
		~MO_Coin(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeCoin();

	private:
		short timer;
		short sparkleanimationtimer;
		short sparkledrawframe;

		short iType;
		short iUncollectableTime;
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

		//MovingObjectType getMovingObjectType() {return movingObjectType;}
		
	protected:

		short iNumSprites;
		short drawframe;
		short animationtimer;
		short animationWidth;

		short animationOffsetX, animationOffsetY;
		short animationspeed;

		//MovingObjectType movingObjectType;
};

class OMO_Thwomp : public IO_OverMapObject
{
	public:
		OMO_Thwomp(gfxSprite *nspr, short x, float nspeed);
		~OMO_Thwomp(){};

		void update();
		bool collide(CPlayer * player);
};

class MO_Podobo : public IO_MovingObject
{
	public:
		MO_Podobo(gfxSprite *nspr, short x, float nspeed, short playerid, short teamid, short colorid);
		~MO_Podobo(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

	private:
		
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
		virtual void Kick() {}

	protected:

		CPlayer * owner;
		bool fSmoking;

	friend class B_ThrowBlock;
	friend class CPlayer;
};

class CO_Egg : public MO_CarriedObject
{
	public:
		CO_Egg(gfxSprite *nspr, short iColor);
		~CO_Egg(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		
		void MoveToOwner();

		void placeEgg();
		void Drop();
		void Kick();

	private:
		short relocatetimer;
		short explosiondrawframe, explosiondrawtimer;

		CPlayer * owner_throw;
		short owner_throw_timer;
		
		short sparkleanimationtimer;
		short sparkledrawframe;
	
		short color;

		short egganimationrates[6];

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
		void Kick();

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
		void placeFlagBase(bool fInit);
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

		short timer;

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
		void Kick();

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
		bool centerflag;

	friend class CPlayer;
	friend class CGM_CTF;
	friend class OMO_FlagBase;
};

class OMO_Yoshi : public IO_OverMapObject
{
	public:
		OMO_Yoshi(gfxSprite *nspr, short iColor);
		~OMO_Yoshi(){};

		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void placeYoshi();

	private:
		short timer;
		short color;
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
		short iPlayerID;
		short iTeamID;

		short colorID;
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

		short iPlayerID;

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
		short quantity;
		bool isfinishline;
			
	friend class CObjectContainer;
};

class MO_FrenzyCard : public IO_MovingObject
{
	public:
		MO_FrenzyCard(gfxSprite *nspr, short iType);
		~MO_FrenzyCard(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeCard();

	private:
		short timer;
		short type;

		short sparkleanimationtimer;
		short sparkledrawframe;
};

class MO_CollectionCard : public IO_MovingObject
{
	public:
		MO_CollectionCard(gfxSprite *nspr, short iType, short iValue, short iUncollectableTime, float dvelx, float dvely, short ix, short iy);
		~MO_CollectionCard(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeCard();

	private:
		short timer;
		short type;
		short value;

		short sparkleanimationtimer;
		short sparkledrawframe;

		short uncollectabletime;
};

class MO_Explosion : public IO_MovingObject
{
	public:
		MO_Explosion(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short id, short iTeamID, killstyle style);
		~MO_Explosion(){};

		void update();
		bool collide(CPlayer * player);
		
	private:
		
		short timer;
		killstyle iStyle;
};

class MO_WalkingEnemy : public IO_MovingObject
{
	public:
		MO_WalkingEnemy(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth, bool moveToRight, bool killOnWeakWeapon, bool fBouncing);
		virtual ~MO_WalkingEnemy(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player);
		virtual void collide(IO_MovingObject * object);
		virtual void place();

		virtual bool hittop(CPlayer * player) = 0;
		virtual bool hitother(CPlayer * player);

		virtual void Die() {}
		virtual void DieAndDropShell() { dead = true; DropShell(); }
		virtual void DropShell() {}

	protected:
		float spawnradius;
		float spawnangle;

		short iSpawnIconOffset;
		killstyle killStyle;

		short burnuptimer;
		bool fKillOnWeakWeapon;
		bool fBouncing;
};

class MO_Goomba : public MO_WalkingEnemy
{
	public:
		MO_Goomba(gfxSprite *nspr, bool moveToRight, bool fBouncing);
		virtual ~MO_Goomba(){};

		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DieAndDropShell() { Die(); }
};

class MO_Koopa : public MO_WalkingEnemy
{
	public:
		MO_Koopa(gfxSprite *nspr, bool moveToRight, bool red, bool fBouncing);
		~MO_Koopa(){};

		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DropShell();

	private:

		bool fRed;
};

class MO_BuzzyBeetle : public MO_WalkingEnemy
{
	public:
		MO_BuzzyBeetle(gfxSprite *nspr, bool moveToRight);
		~MO_BuzzyBeetle(){};

		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DropShell();
};

class MO_Spiny : public MO_WalkingEnemy
{
	public:
		MO_Spiny(gfxSprite *nspr, bool moveToRight);
		~MO_Spiny(){};

		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DropShell();
};

class MO_CheepCheep : public IO_MovingObject
{
	public:
		MO_CheepCheep(gfxSprite *nspr);
		~MO_CheepCheep(){};

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

class MO_BulletBill : public IO_MovingObject
{
	public:
		MO_BulletBill(gfxSprite *nspr, gfxSprite *nsprdead, short x, short y, float nspeed, short playerID, bool isspawned);
		~MO_BulletBill(){};

		void update();
		void draw();
		void draw(short iOffsetX, short iOffsetY);
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

		bool hittop(CPlayer * player);
		bool hitother(CPlayer * player);
		
		void Die();
		void SetDirectionOffset();

	private:
		gfxSprite * spr_dead;

		short iColorID;
		
		short iColorOffsetY;
		short iDirectionOffsetY;

		bool fIsSpawned;
		short iHiddenDirection;
		short iHiddenPlane;

	friend class MO_Podobo;
};

/*
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
*/

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
		void Kick();

		void collide(IO_MovingObject * object);
		void CheckAndDie();
		void Die();

		void SideBounce();
		void AddMovingKill(CPlayer * killer);

		bool IsThreat() {return state == 1 || state == 3;}

		void Flip();

	private:
		void Stop();

		short iShellType;

		short iIgnoreBounceTimer;
		short iDestY;

		bool fDieOnMovingPlayerCollision;
		bool fDieOnHoldingPlayerCollision;
		bool fDieOnFire;
		bool fKillBouncePlayer;

		short iDeathTime;
		short iNoOwnerKillTime;
		
		short iColorOffsetY;
		short iBounceCounter;
		short iKillCounter;

		bool fFlipped;
		short iFlippedOffset;

	friend class CPlayer;
	friend class MO_Explosion;
	friend class MO_BulletBill;
	friend class MO_Goomba;
	friend class MO_Koopa;
	friend class MO_BuzzyBeetle;
	friend class MO_Spiny;
	//friend class MO_SledgeBrother;
	friend class MO_CheepCheep;
	friend class MO_PirhanaPlant;
	
	friend class B_WeaponBreakableBlock;
	
	friend void RunGame();
};

class CO_ThrowBlock : public MO_CarriedObject
{
	public:
		CO_ThrowBlock(gfxSprite * nspr, short x, short y, short type);
		~CO_ThrowBlock(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		bool HitTop(CPlayer * player);
		bool HitOther(CPlayer * player);

		void MoveToOwner();

		bool KillPlayer(CPlayer * player);
		void Drop();
		void Kick();

		void collide(IO_MovingObject * object);
		void CheckAndDie();
		void Die();

		void SideBounce();

	private:
		short iType;
		
		short iDeathTime;
		bool fDieOnBounce;
		bool fDieOnPlayerCollision;
		short iBounceCounter;
		short iNoOwnerKillTime;

	friend class CPlayer;
	friend class MO_Explosion;
	friend class MO_BulletBill;
	friend class MO_Goomba;
	//friend class MO_SledgeBrother;
	friend class MO_CheepCheep;
	friend class MO_SpinAttack;
	friend class MO_AttackZone;

	friend class B_ThrowBlock;
	friend class B_WeaponBreakableBlock;

	friend void RunGame();
};

class CO_Spring : public MO_CarriedObject
{
	public:
		CO_Spring(gfxSprite *nspr, short ix, short iy);
		~CO_Spring(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void MoveToOwner();

		void place();
		void Drop();
		void Kick();

	protected:

		virtual void hittop(CPlayer * player);
		void hitother(CPlayer * player);

	friend class CPlayer;
};

class CO_Spike : public CO_Spring
{
	public:
		CO_Spike(gfxSprite *nspr, short ix, short iy);
		~CO_Spike(){};

	private:

		void hittop(CPlayer * player);
};

class CO_KuriboShoe : public CO_Spring
{
	public:
		CO_KuriboShoe(gfxSprite *nspr, short ix, short iy);
		~CO_KuriboShoe(){};

	private:

		void hittop(CPlayer * player);
};

class MO_AttackZone : public IO_MovingObject
{
	public:
		MO_AttackZone(short playerID, short teamID, short x, short y, short w, short h, short time, killstyle style, bool dieoncollision);
		~MO_AttackZone(){};

		virtual void update();
		virtual void draw() {} //This is invisible

		virtual bool collide(CPlayer * player);
		virtual void collide(IO_MovingObject * object);

		void Die();

	protected:

		bool fDieOnCollision;

		short iTimer;
		killstyle iStyle;

	friend class B_WeaponBreakableBlock;
};


class MO_SpinAttack : public MO_AttackZone
{
	public:
		MO_SpinAttack(short playerID, short teamID, killstyle style, bool direction, short offsety);
		~MO_SpinAttack(){};

		void update();

		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);

	private:

		bool fDirection;
		short iOffsetY;
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
		void Kick();

		void Die();
		
	protected:
		
		short iColorOffsetY;
		short ttl;

	friend class CPlayer;
};

class OMO_OrbitHazard : public IO_OverMapObject
{
	public:
		OMO_OrbitHazard(gfxSprite *nspr, short x, short y, float radius, float vel, float angle, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth);
		~OMO_OrbitHazard() {}

		void update();

		bool collide(CPlayer *);

	private:
		void CalculatePosition();

		float dAngle, dVel, dRadius;
		float dCenterX, dCenterY;
};

class OMO_StraightPathHazard : public IO_OverMapObject
{
	public:
		OMO_StraightPathHazard(gfxSprite *nspr, short x, short y, float angle, float vel, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth);
		~OMO_StraightPathHazard() {}

		void update();

		bool collide(CPlayer *);

	private:

		float dAngle, dVel;
};

class IO_BulletBillCannon : public CObject
{
	public:
		IO_BulletBillCannon(short x, short y, short freq, float vel, bool preview);
		~IO_BulletBillCannon() {}

		void draw() {}
		void update();

		bool collide(CPlayer *) {return false;}
		void collide(IO_MovingObject *) {}
		
	private:
		void SetNewTimer();

		short iFreq, iTimer;
		float dVel;
		bool fPreview;
};


class IO_FlameCannon : public CObject
{
	public:
		IO_FlameCannon(short x, short y, short freq, bool isfacingright);
		~IO_FlameCannon() {}

		void draw();
		void draw(short iOffsetX, short iOffsetY);
		void update();

		bool collide(CPlayer * player);
		void collide(IO_MovingObject *) {}
				
	private:
		void SetNewTimer();

		short iFreq, iTimer, iCycle;
		short iFlameX, iFlameY;

};


class MO_PirhanaPlant : public IO_MovingObject
{
	public:
		MO_PirhanaPlant(short x, short y, short type, short freq, short direction, bool preview);
		~MO_PirhanaPlant() {}

		void draw();
		void draw(short iOffsetX, short iOffsetY);
		void update();

		bool collide(CPlayer * player);
		void collide(IO_MovingObject *);
		
	private:
		void SetNewTimer();
		void KillPlant();
		float GetFireballAngle();

		short iType, iDirection;
		short iFreq, iTimer;
		//short iHiddenPlane, iHiddenDirection;
		short iSrcX, iSrcY;
		short iAnimationTimer, iAnimationX;
		short iFacing;
		short iActionTimer;

		bool fPreview;
};

class OMO_PipeCoin: public IO_OverMapObject
{
	public:
		OMO_PipeCoin(gfxSprite *nspr, float velx, float vely, short ix, short iy, short teamid, short colorid, short uncollectabletime);
		~OMO_PipeCoin(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

	private:
		short iTeamID, iColorID;

		short timer;
		short sparkleanimationtimer;
		short sparkledrawframe;

		short iUncollectableTime;
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

