#ifndef _PLAYER_H
#define _PLAYER_H

#include "ai.h"
enum PlayerState {player_wait, player_spawning, player_dead, player_ready, player_entering_warp_up, player_entering_warp_right, player_entering_warp_down, player_entering_warp_left, player_exiting_warp_down, player_exiting_warp_left, player_exiting_warp_up, player_exiting_warp_right};

class CObject;
class MO_CarriedObject;

class CScore
{
	public:
		CScore(short iPlace)
		{
			place = iPlace;
			displayorder = iPlace;
			score = 0;
			x = 0;
			y = 0;
			destx = 0;
			desty = 0;
			order = 0;
			fromx = 0;
			fromy = 0;
			iDigitRight = 0;
			iDigitMiddle = 0;
			iDigitLeft = 0;
		}

		~CScore() {}

		void AdjustScore(short iValue)
		{
			score += iValue;

			if(score < 0)
				score = 0;

			SetDigitCounters();
		}

		void SetScore(short iValue)
		{
			score = iValue;
			SetDigitCounters();
		}

		short score;

		//Where to display score
		short x;
		short y;

		short destx;
		short desty;

		short place;
		short displayorder;
		short order;  //the order in which the team died

		short fromx;
		short fromy;

		//One less array dereference doing vars like this
		short iDigitRight;
		short iDigitMiddle;
		short iDigitLeft;

	private:

		void SetDigitCounters()
		{
			short iDigits = score;
			while(iDigits > 999)
				iDigits -= 1000;
				
			iDigitLeft = iDigits / 100 * 16;
			iDigitMiddle = iDigits % 100 / 10 * 16;
			iDigitRight = iDigits % 10 * 16;
		}

	friend class CGM_Star;
};


enum killstyle{kill_style_stomp = 0, kill_style_star = 1, kill_style_fireball = 2, kill_style_bobomb = 3, kill_style_bounce = 4, kill_style_pow = 5, kill_style_goomba = 6, kill_style_bulletbill = 7, kill_style_hammer = 8, kill_style_shell = 9, kill_style_throwblock = 10, kill_style_cheepcheep = 11, kill_style_koopa = 12, kill_style_boomerang = 13, kill_style_feather = 14};
enum awardstyle{award_style_none = 0, award_style_fireworks = 1, award_style_swirl = 2, award_style_halo = 3, award_style_souls = 4, award_style_text = 5};
enum deathstyle{death_style_jump = 0, death_style_squish = 1};


//the player class - a lot of optimization can be done here
//(especially the collision detection stuff in collision_detection_map())
class CPlayer
{
	public:
		CPlayer(short iGlobalID, short iLocalID, short iTeamID, short iSubTeamID, short iTeamColorID, gfxSprite * nsprites[PGFX_LAST], CScore *nscore, short * respawn, CPlayerAI * ai);
		~CPlayer();

		void draw();
		void drawarrows();
		void updateswap();
		void drawswap();
		void move();
		void mapcollisions();
		void cpu_think();

		void die(short deathStyle, bool fTeamRemoved);

		void spawnText(char * szText);
		void DeathAwards();
		void AddKillsInRowInAirAward();
		void SetupNewPlayer();

		short getGlobalID() {return globalID;}

		bool bouncejump();

		short GetWarpPlane() {return warpplane;}
		bool IsPlayerFacingRight();
		
		bool IsAcceptingItem() {return fAcceptingItem;}
		bool PressedAcceptItemKey() {return fPressedAcceptItem;}
		bool AcceptItem(MO_CarriedObject * item);

		void SetPowerup(short iPowerup);
		gfxSprite ** GetScoreboardSprite() {return pScoreboardSprite;}

		void DecreaseProjectileLimit();

		bool isready() {return state == player_ready;}
		bool isspawning() {return state == player_spawning;}
		bool iswarping() {return state > player_ready;}
		bool isdead() {return state == player_dead;}

	private:
		void SetSprite();
		void Jump(short iMove, float jumpModifier);
		void DrawCape();

		void xf(float xf){fx = xf; ix = (short)fx;};
 		void xi(short xi){ix = xi; fx = (float)ix;};
		void yf(float yf){fy = yf; iy = (short)fy;};
		void yi(short yi){iy = yi; fy = (float)iy;};
		
		void FindSpawnPoint();
		void collision_detection_map();
		bool collision_detection_checktop();
		bool collision_detection_checkleft();
		bool collision_detection_checkright();
		void collision_detection_checksides();
		void flipsidesifneeded();
		void fliptopifneeded();
		void makeinvincible();
		void turnslowdownon(bool fSuperFreeze);
		bool isstomping(CPlayer &o);
		void explodeawards();
		void addswirlingawards();
		void addrocketawards();

		void KillPlayerMapHazard();
		
        void enterwarp(Warp * warp);
		void chooseWarpExit();
		void increasewarpcounter(short iGoal);
		void decreasewarpcounter();
		
		COutputControl * playerKeys;
		short playerDevice;

        // tanooki
        bool tanooki;
        bool statue_lock;
        int statue_timer;
        int konamiIndex;

		//super kick
		short superKickIndex;

		//secret spring
		short secret_spring_index;
		short secret_spike_index;

		CScore *score;
		short killsinrow;
		short killsinrowinair;

		float awardangle;
		short awards[MAXAWARDS];

		short globalID;
		short localID;
		short teamID;
		short subTeamID;
		short colorID;

		short ix, iy;			//x, y coordinate (top left of the player rectangle)
		float fx, fy;
		float velx, vely;	//velocity on x, y axis
		float fOldX, fOldY;
		float fPrecalculatedY;

		float fNewSwapX, fNewSwapY; //For moving players around during player swap effect
		float fOldSwapX, fOldSwapY;
		short iSrcOffsetX;
		gfxSprite ** pScoreboardSprite;

		short iNewPowerupX, iNewPowerupY;  //For moving powerups around during player swap effect
		short iOldPowerupX, iOldPowerupY;
		
		bool inair;			//true... player is in the air, false... player is on the ground
							//inair is set in CPlayer::collision_detection_map()
		bool onice;			//on ice... friction goes way down

		bool lockjump;		//is the player allowed to jump
		bool lockfall;		//on PC verion, the down key makes player fall through solid_on_top tiles
		bool lockfire;		//the player is allowed to shoot a fireball
		short throw_star;

		int holddown;
		int holddowntolerance;
		int holdleft;
		int holdlefttolerance;
		int holdright;
		int holdrighttolerance;

		short featherjump;	//true when player has used a feather jump in air (only allow one feather jump per time in air)
		bool flying;
		short flyingtimer;

		short ryu_fireball_index_left;
		short ryu_fireball_index_right;
		bool shoot_left_fireball;
		bool shoot_right_fireball;

		short super_sledge_hammer_throw_index;
		bool shoot_super_sledge_hammer;

		short super_hammer_throw_left_index;
		short super_hammer_throw_right_index;
		bool shoot_right_super_hammer;
		bool shoot_left_super_hammer;

		short super_boomerang_throw_index_left;
		short super_boomerang_throw_index_right;
		bool shoot_super_boomerang;

		short super_bobomb_index;
		bool shoot_super_bobomb;

		short super_pow_index;
		bool super_pow;
		short super_pow_timer;

		short super_mod_index;
		bool super_mod;
		short super_mod_timer;

		short dashLeftIndex;
		short dashRightIndex;
		bool dashLeft;
		bool dashRight;
		short dashSparkleAnimationTimer;

		short homingBillsIndex;
		bool homingBills;

		short redKoopaIndex;
		short redThrowBlockIndex;
		short viewBlockIndex;
		short boss_index[3];

		short superjumptimer;		//this is true when a player is able to do a super jump off a note block
		short hammertimer;		//Only allow the player to throw powerful hammers so fast
		
		short frictionslidetimer;  //this controls how much friction smoke is generated when sliding
		short bobombsmoketimer;   //timer for the bobomb smoke animation

		CPlayerAI * pPlayerAI;

		gfxSprite *sprites[PGFX_LAST];
		short animationstate;
		short animationtimer;

		short spr;
		short sprswitch;
	
		bool invincible;
		short invincibletimer;

		bool spawninvincible;
		short spawninvincibletimer;

		short burnuptimer;
		short burnupstarttimer;

		short outofarenatimer;
		short outofarenadisplaytimer;

		short powerup;
		short projectilelimit;

		bool bobomb;
		short iCapeTimer;
		short iCapeFrameX;
		short iCapeFrameY;
		short fCapeUp;
		short iCapeYOffset;

		PlayerState state;
		float spawnradius;
		float spawnangle;
		short spawntimer;

		short waittimer;
		short * respawncounter;

		short warpcounter;
		short warpconnection;
		short warpid;
		short warpplane;

		short powerupused;
		float powerupradius;
		float powerupangle;

		//Pointer to the object the player is carrying
		bool fAcceptingItem;
		bool fPressedAcceptItem;
		MO_CarriedObject * carriedItem;

		//Pointer to the platform the player is riding
		MovingPlatform * platform;
		short iHorizontalPlatformCollision;
		short iVerticalPlatformCollision;

		//ID of the player that owns this player for Owned mode
		short ownerPlayerID;
		short ownerColorOffsetX;
		//If greater than 0, the player is jailed and moves slowly for Jail mode
		short jailed;

		bool fallthrough;

		short diedas;  //What sprite the player died as 0 == mario, 1 == chocobo, 2 == bobomb

		short spawntext;

		CPlayer * pSuicideCreditPlayer;
		short iSuicideCreditTimer;

		friend bool coldec_player2player(CPlayer &o1, CPlayer &o2);
		friend void collisionhandler_p2p(CPlayer &o1, CPlayer &o2);
		friend void _collisionhandler_p2p_pushback(CPlayer &o1, CPlayer &o2);

		friend bool coldec_player2obj(CPlayer &o1, CObject &o2);
		friend bool collisionhandler_p2o(CPlayer &o1, CObject &o2);

		friend void PlayerKilledPlayer(CPlayer &killer, CPlayer &killed, short deathstyle, killstyle style);
		friend void AddAwardKill(CPlayer * killer, CPlayer * killed, killstyle style);
		friend void RemovePlayersButHighestScoring();
		friend void RemovePlayersButTeam(short teamID);
		friend void RemoveTeam(short teamid);
		friend void CleanDeadPlayers();
		friend short CountAliveTeams(short * lastteam);
		friend void RunGame();
		
		friend class CObjectContainer;

		friend class CGameMode;
		friend class CGM_Frag;
		friend class CGM_TimeLimit;
		friend class CGM_Jail;
		friend class CGM_Classic;
		friend class CGM_Chicken;
		friend class CGM_Tag;
		friend class CGM_Coins;
		friend class CGM_Survival;
		friend class CGM_Eggs;
		friend class CGM_Domination;
		friend class OMO_KingOfTheHillZone;
		friend class CGM_Race;
		friend class CGM_Frenzy;
		friend class CGM_Owned;
		friend class CGM_Star;
		friend class CGM_CaptureTheFlag;
		friend class CGM_Stomp;
		friend class CGM_Boss;
		
		friend class MI_InputDevice;
		friend class MI_InputLeft;
		friend class MI_InputRight;
		friend class MI_InputDown;
		friend class MI_InputJump;
		friend class MI_InputTurbo;
		friend class MI_InputPower;

		friend class IO_Block;
		friend class B_NoteBlock;
		friend class B_BreakableBlock;
		friend class B_PowerupBlock;
		friend class B_DonutBlock;
		friend class B_FlipBlock;
		friend class B_BounceBlock;
		friend class B_ThrowBlock;
		friend class B_OnOffSwitchBlock;
		friend class B_SwitchBlock;

		friend class PU_ExtraGuyPowerup;
		friend class PU_StarPowerup;
		friend class PU_FirePowerup;
		friend class PU_HammerPowerup;
		friend class PU_SledgeHammerPowerup;
		friend class PU_PodoboPowerup;
		friend class PU_PoisonPowerup;
		friend class PU_ClockPowerup;
		friend class PU_BobombPowerup;
		friend class PU_PowPowerup;
		friend class PU_ModPowerup;
		friend class PU_BulletBillPowerup;
		friend class PU_FeatherPowerup;
		friend class PU_MysteryMushroomPowerup;
        friend class PU_Tanooki;
		friend class PU_BombPowerup;
		friend class MysteryMushroomTempPlayer;
		friend class MO_Coin;

		friend class CO_Egg;
		friend class CO_Star;
		friend class CO_Flag;
		friend class OMO_Yoshi;
		friend class OMO_Thwomp;
		friend class OMO_Podobo;
		friend class OMO_BowserFire;
		friend class OMO_Area;
		friend class OMO_RaceGoal;
		friend class OMO_Explosion;

		friend class MO_CarriedObject;

		friend class MO_Fireball;
		friend class MO_Hammer;
		friend class MO_SledgeHammer;
		friend class MO_Boomerang;
		friend class MO_SuperFireball;
		friend class CO_Shell;
		friend class CO_ThrowBlock;
		friend class CO_Spring;
		friend class CO_Spike;
		friend class CO_Bomb;

		friend class MO_Goomba;
		friend class MO_Koopa;
		friend class MO_SledgeBrother;
		friend class OMO_CheepCheep;
		friend class OMO_BulletBill;
		friend class OMO_FlagBase;

		friend class MovingPlatform;

		friend bool SwapPlayers(short iUsingPlayerID);
		friend CPlayer * GetPlayerFromGlobalID(short iGlobalID);

		friend class CPlayerAI;
		friend class CSimpleAI;
};


void collisionhandler_p2p(CPlayer &o1, CPlayer &o2);
void _collisionhandler_p2p__pushback(CPlayer &o1, CPlayer &o2);

#endif  //_PLAYER_H

