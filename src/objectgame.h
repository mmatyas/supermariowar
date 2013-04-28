#ifndef _OBJECTGAME_H
#define _OBJECTGAME_H

class IO_MovingObject;

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

		void KillPlayersAndObjectsInsideBlock(short playerID);

	protected:
		void BounceMovingObject(IO_MovingObject * object);

		short iBumpPlayerID;
		short iBumpTeamID;

		float fposx, fposy;  //position to return to (for note and bumpable blocks)
		short iposx, iposy; //position of the block (doesn't move)

		bool hidden, ishiddentype;
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

		void triggerBehavior(short iPlayerId);

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

		void triggerBehavior(short playerID);

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

		void FlipState(short playerID);

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

class MO_Powerup : public IO_MovingObject
{
	public:
		MO_Powerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1);
		virtual ~MO_Powerup(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player);
		virtual void nospawn(short y);

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

class PU_SecretPowerup : public MO_Powerup
{
	public:
		PU_SecretPowerup(gfxSprite * nspr, short x, short y, short type);
		~PU_SecretPowerup(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void place();
	
	private:
		short sparkleanimationtimer;
		short sparkledrawframe;
		short itemtype;
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

		void nospawn(short y);

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

class PU_CoinPowerup : public MO_Powerup
{
	public:
		PU_CoinPowerup(gfxSprite *nspr, short x, short y, short color, short value);
		~PU_CoinPowerup(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

	protected:
		short iColorOffsetY;
		short iValue;

		short sparkleanimationtimer;
		short sparkledrawframe;
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
		Spotlight * sSpotlight;
};

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
		Spotlight * sSpotlight;
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

class MO_IceBlast : public IO_MovingObject
{
	public:
		MO_IceBlast(gfxSprite *nspr, short x, short y, float fVelyX, short iGlobalID, short iTeamID, short iColorID);
		~MO_IceBlast(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

	private:
		short colorOffset;
		short ttl;
		Spotlight * sSpotlight;
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
		Spotlight * sSpotlight;
};

class MO_Coin : public IO_MovingObject
{
	public:
		MO_Coin(gfxSprite *nspr, float velx, float vely, short ix, short iy, short color, short team, short type, short uncollectabletime, bool placecoin);
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
		short iTeam;
		short iUncollectableTime;
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
		virtual void draw();
		virtual bool collide(CPlayer *) {return false;}

		virtual void MoveToOwner();

		virtual void Drop();
		virtual void Kick();

		bool HasOwner() {return owner != NULL;}

		bool IsCarriedByKuriboShoe() { return fCarriedByKuriboShoe; }

	protected:

		bool fCarriedByKuriboShoe;

		CPlayer * owner;
		bool fSmoking;

		float dKickX, dKickY;
		short iOwnerLeftOffset, iOwnerRightOffset, iOwnerUpOffset;
	
	private:
		void init();

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
		
		void placeEgg();
		void Drop();

		short getColor() {return color;}

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
	friend class MO_Yoshi;
	friend class CGM_Eggs;
};

class CO_Star : public MO_CarriedObject
{
	public:
		CO_Star(gfxSprite *nspr, short type, short id);
		~CO_Star(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void placeStar();

		short getType() {return iType;}
		void setPlayerColor(short iColor) {iOffsetY = 64 + (iColor << 5);}

	private:
		short timer;
		short iType, iOffsetY;
		short sparkleanimationtimer;
		short sparkledrawframe;
		short iID;

	friend class CPlayer;
	friend class CGM_Star;
};

//Declaring class for use in MO_FlagBase (some compilers complain if this isn't here)
class CO_Flag;

class MO_FlagBase : public IO_MovingObject
{
	public:
		MO_FlagBase(gfxSprite *nspr, short iTeamID, short iColorID);
		~MO_FlagBase(){};

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
		CO_Flag(gfxSprite *nspr, MO_FlagBase * base, short iTeamID, short iColorID);
		~CO_Flag(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		void MoveToOwner();

		void placeFlag();
		void Drop();

		bool GetInBase() {return fInBase;}
		short GetTeamID() {return teamID;}

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

class MO_Yoshi : public IO_MovingObject
{
	public:
		MO_Yoshi(gfxSprite *nspr, short iColor);
		~MO_Yoshi(){};

		void update();
		bool collide(CPlayer * player);
		void collide(IO_MovingObject * object);
		void placeYoshi();
		short getColor() {return color;}

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
		
		short multiplier;
		short multipliertimer;

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

		short getType() {return type;}
		short getValue() {return value;}

	private:
		short timer;
		short type;
		short value;

		short sparkleanimationtimer;
		short sparkledrawframe;

		short uncollectabletime;
};

class MO_WalkingEnemy : public IO_MovingObject
{
	public:
		MO_WalkingEnemy(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth, bool moveToRight, bool killOnWeakWeapon, bool fBouncing, bool fallOffLedges);
		virtual ~MO_WalkingEnemy(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player);
		virtual void collide(IO_MovingObject * object);
		virtual void place();

		virtual bool hittop(CPlayer * player) = 0;
		virtual bool hitother(CPlayer * player);

		virtual void ShatterDie();
		virtual void Die() {}
		virtual void DieAndDropShell(bool fBounce, bool fFlip) { if(frozen){ShatterDie();return;} dead = true; DropShell(fBounce, fFlip); }
		virtual void DropShell(bool fBounce, bool fFlip) {}

		killstyle getKillStyle() {return killStyle;}

	protected:
		float spawnradius;
		float spawnangle;

		short iSpawnIconOffset;
		killstyle killStyle;

		short burnuptimer;
		bool fKillOnWeakWeapon;
		bool fBouncing;
		bool fFallOffLedges;

		bool frozen;
		short frozentimer;
		float frozenvelocity;
		short frozenanimationspeed;
};

class MO_Goomba : public MO_WalkingEnemy
{
	public:
		MO_Goomba(gfxSprite *nspr, bool moveToRight, bool fBouncing);
		virtual ~MO_Goomba(){};

		void draw();
		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DieAndDropShell(bool fBounce, bool fFlip) { Die(); }
};

class MO_Koopa : public MO_WalkingEnemy
{
	public:
		MO_Koopa(gfxSprite *nspr, bool moveToRight, bool red, bool fBouncing, bool fFallOffLedges);
		~MO_Koopa(){};

		void draw();
		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DropShell(bool fBounce, bool fFlip);

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
		void DropShell(bool fBounce, bool fFlip);
};

class MO_Spiny : public MO_WalkingEnemy
{
	public:
		MO_Spiny(gfxSprite *nspr, bool moveToRight);
		~MO_Spiny(){};

		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DropShell(bool fBounce, bool fFlip);
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
		void ShatterDie();

	private:
		short iColorOffsetY;
		bool frozen;
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
		
		void SetLastAction(short type);

		short iType;

		short iActionState;
		short iDestLocationX[5];

		void randomaction();
		void move();
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
		short need_action[6];

		short last_action;
		short last_action_count;
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

		bool KillPlayer(CPlayer * player);
		void Drop();
		void Kick();

		void collide(IO_MovingObject * object);
		void CheckAndDie();
		void Die();
		void ShatterDie();

		void SideBounce(bool fRightSide);
		void AddMovingKill(CPlayer * killer);

		bool IsThreat() {return state == 1 || state == 3;}

		void Flip();

		void nospawn(short y, bool fBounce);

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

		bool frozen;
		short frozentimer;
		float frozenvelocity;
		short frozenanimationspeed;

		Spotlight * sSpotlight;

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
	
	friend class MO_AttackZone;
	friend class MO_SpinAttack;

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

		bool KillPlayer(CPlayer * player);
		void Drop();
		void Kick();

		void collide(IO_MovingObject * object);
		void CheckAndDie();
		void Die();
		void ShatterDie();

		void SideBounce(bool fRightSide);

	private:
		void DieHelper();

		short iType;
		
		short iDeathTime;
		bool fDieOnBounce;
		bool fDieOnPlayerCollision;
		short iBounceCounter;
		short iNoOwnerKillTime;

		bool frozen;
		short frozentimer;
		float frozenvelocity;
		short frozenanimationspeed;

		Spotlight * sSpotlight;

	friend class CPlayer;
	friend class MO_Explosion;
	friend class MO_BulletBill;
	friend class MO_Goomba;
	//friend class MO_SledgeBrother;
	friend class MO_CheepCheep;
	friend class MO_SpinAttack;
	friend class MO_AttackZone;

	friend class CO_Shell;

	friend class B_ThrowBlock;
	friend class B_WeaponBreakableBlock;

	friend void RunGame();
};

class CO_ThrowBox : public MO_CarriedObject
{
	public:
		CO_ThrowBox(gfxSprite * nspr, short x, short y, short item);
		~CO_ThrowBox(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		bool KillPlayer(CPlayer * player);
		
		void Drop();
		void Kick();

		void collide(IO_MovingObject * object);
		void Die();
		void ShatterDie();

		void SideBounce(bool fRightSide);
		float BottomBounce();

		bool HasKillVelocity();

	private:
		void DieHelper();

		short iItem;
		
		bool frozen;
		short frozentimer;
		short frozenanimationspeed;

		Spotlight * sSpotlight;

	friend class CPlayer;
	friend class MO_Explosion;
	friend class MO_BulletBill;
	friend class MO_Goomba;
	friend class MO_CheepCheep;
	friend class MO_SpinAttack;
	friend class MO_AttackZone;

	friend class CO_ThrowBlock;
	friend class CO_Shell;
	friend class B_WeaponBreakableBlock;

	friend void RunGame();
};

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
		CO_KuriboShoe(gfxSprite *nspr, short ix, short iy, bool fSticky);
		~CO_KuriboShoe(){};

	private:

		void hittop(CPlayer * player);

		bool fSticky;
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

		void place();

		void Die();
		
	protected:
		
		short iColorOffsetY;
		short ttl;

		Spotlight * sSpotlight;

	friend class CPlayer;
};


class OMO_PipeCoin: public IO_OverMapObject
{
	public:
		OMO_PipeCoin(gfxSprite *nspr, float velx, float vely, short ix, short iy, short teamid, short colorid, short uncollectabletime);
		~OMO_PipeCoin(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		short GetColor() {return iColorID;}
		short GetTeam() {return iTeamID;}

	private:
		short iTeamID, iColorID;

		short sparkleanimationtimer;
		short sparkledrawframe;

		short iUncollectableTime;
};

class OMO_PipeBonus: public IO_OverMapObject
{
	public:
		OMO_PipeBonus(gfxSprite *nspr, float velx, float vely, short ix, short iy, short type, short duration, short uncollectabletime);
		~OMO_PipeBonus(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

		short GetType() {return iType;}

	private:
		short iType, iDuration;
		short iUncollectableTime;
};

//object phanto
class OMO_Phanto : public IO_OverMapObject
{
	public:
		OMO_Phanto(gfxSprite *nspr, short x, short y, float velx, float vely, short type);
		~OMO_Phanto(){};

		void update();
		bool collide(CPlayer * player);

		short GetType() {return iType;}

	private:
		short iType;

		float dMaxSpeedX, dMaxSpeedY;
		float dReactionSpeed, dSpeedRatio;
		short iSpeedTimer;
};

class CO_PhantoKey : public MO_CarriedObject
{
	public:
		CO_PhantoKey(gfxSprite *nspr);
		~CO_PhantoKey(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		
		void placeKey();

	private:
		short relocatetimer;

		short sparkleanimationtimer;
		short sparkledrawframe;
		
	friend class CGM_Chase;
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

#endif //_OBJECTGAME_H

