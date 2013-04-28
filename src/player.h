#ifndef _PLAYER_H
#define _PLAYER_H

#include "ai.h"
enum PlayerState {player_wait, player_spawning, player_dead, player_ready, player_entering_warp_up, player_entering_warp_right, player_entering_warp_down, player_entering_warp_left, player_exiting_warp_down, player_exiting_warp_left, player_exiting_warp_up, player_exiting_warp_right};
enum PlayerAction {player_action_none, player_action_bobomb, player_action_fireball, player_action_hammer, player_action_boomerang, player_action_iceblast, player_action_bomb, player_action_spincape, player_action_spintail};

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

			for(short iSubScore = 0; iSubScore < 3; iSubScore++)
				subscore[iSubScore] = 0;
			
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

		void AdjustScore(short iValue);
		
		void SetScore(short iValue)
		{
			score = iValue;
			SetDigitCounters();
		}

		//keeps track of what the actual score value is
		short score;

		//keeps track of other scoring elements for some games (health, collected cards, etc)
		short subscore[3];

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

	//friend class CGM_Star;
};


enum killstyle{kill_style_stomp = 0, kill_style_star = 1, kill_style_fireball = 2, kill_style_bobomb = 3, kill_style_bounce = 4, kill_style_pow = 5, kill_style_goomba = 6, kill_style_bulletbill = 7, kill_style_hammer = 8, kill_style_shell = 9, kill_style_throwblock = 10, kill_style_cheepcheep = 11, kill_style_koopa = 12, kill_style_boomerang = 13, kill_style_feather = 14, kill_style_iceblast = 15, kill_style_podobo = 16, kill_style_bomb = 17, kill_style_leaf = 18, kill_style_pwings = 19, kill_style_kuriboshoe = 20, kill_style_poisonmushroom = 21, kill_style_environment = 22, kill_style_push = 23, kill_style_buzzybeetle = 24, kill_style_spiny = 25, kill_style_phanto = 26, KILL_STYLE_LAST};
enum awardstyle{award_style_none = 0, award_style_fireworks = 1, award_style_swirl = 2, award_style_halo = 3, award_style_souls = 4, award_style_text = 5};
enum deathstyle{death_style_jump = 0, death_style_squish = 1, death_style_shatter = 2};


//the player class - a lot of optimization can be done here
//(especially the collision detection stuff in collision_detection_map())
class CPlayer
{
	public:
		CPlayer(short iGlobalID, short iLocalID, short iTeamID, short iSubTeamID, short iTeamColorID, gfxSprite * nsprites[PGFX_LAST], CScore *nscore, short * respawn, CPlayerAI * ai);
		~CPlayer();

		void Init();

		void draw();
		void drawarrows();
		void drawsuicidetimer();
		void updateswap();
		void drawswap();
		void move();
		void mapcollisions();
		void cpu_think();

		void CommitAction();

		void die(short deathStyle, bool fTeamRemoved, bool fKillCarriedItem);

		void spawnText(const char * szText);
		void DeathAwards();
		void AddKillsInRowInAirAward();
		void SetupNewPlayer();

		short getGlobalID() {return globalID;}
		short getTeamID() {return teamID;}
		short getColorID() {return colorID;}

		bool bouncejump();

		short GetWarpPlane() {return warpplane;}
		bool IsPlayerFacingRight();
		
		bool IsAcceptingItem() {return fAcceptingItem && statue_timer == 0 && iKuriboShoe == 0;}
		bool PressedAcceptItemKey() {return fPressedAcceptItem;}
		bool AcceptItem(MO_CarriedObject * item);

		void SetPowerup(short iPowerup);
		gfxSprite ** GetScoreboardSprite() {return pScoreboardSprite;}

		void DecreaseProjectileLimit();

		bool isready() {return state == player_ready;}
		bool isspawning() {return state == player_spawning;}
		bool iswarping() {return state > player_ready;}
		bool isdead() {return state == player_dead;}

		void SetKuriboShoe(short iType);
		bool IsInvincibleOnBottom() {return invincible || shield || iKuriboShoe;}
		bool IsSuperStomping() {return fSuperStomp;}

		void SetStoredPowerup(short iPowerup);
		void StripPowerups();

	private:

		void ResetSuicideTime();
		void SetSprite();
		void Jump(short iMove, float jumpModifier, bool fKuriboBounce);
		
		void SpinCape();
		void DrawCape();

		void SpinPlayer();

		void ShakeTail();
		void SpinTail();
		void DrawTail();

		void DrawWings();

		void ClearPowerupStates();

		void xf(float xf){fx = xf; ix = (short)fx;};
 		void xi(short xi){ix = xi; fx = (float)ix;};
		void yf(float yf){fy = yf; iy = (short)fy;};
		void yi(short yi){iy = yi; fy = (float)iy;};
		
		bool FindSpawnPoint();
		void collision_detection_map();
		bool collision_detection_checktop();
		bool collision_detection_checkleft();
		bool collision_detection_checkright();
		void collision_detection_checksides();
		void flipsidesifneeded();
		void fliptopifneeded();
		void makeinvincible();
		void makefrozen(short iTime);
		void turnslowdownon();
		bool isstomping(CPlayer * o);
		void explodeawards();
		void addswirlingawards();
		void addrocketawards();

		short KillPlayerMapHazard(bool fForce, killstyle ks, bool fKillCarriedItem, short iPlayerId = -1);
		
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
		float oldvelx;

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

		short extrajumps;	//true when player has used a feather jump in air (only allow one feather jump per time in air)
		bool flying;
		short flyingtimer;

		short iTailTimer;
		short iTailState;
		short iTailFrame;
		
		short iSpinTimer;
		short iSpinState;

		short iWingsTimer;
		short iWingsFrame;

		short superjumptimer;		//this is true when a player is able to do a super jump off a note block
		short superjumptype;		//type of the note block they hit
		short hammertimer;		//Only allow the player to throw powerful hammers so fast
		
		short frictionslidetimer;  //this controls how much friction smoke is generated when sliding
		short bobombsmoketimer;   //timer for the bobomb smoke animation
		short rainsteptimer;

		CPlayerAI * pPlayerAI;

		gfxSprite *sprites[PGFX_LAST];
		short animationstate;
		short animationtimer;

		short spr;
		short sprswitch;
	
		bool invincible;
		short invincibletimer;

		short shield;
		short shieldtimer;

		bool frozen;
		short frozentimer;

		short iKuriboShoe;
		short iKuriboShoeAnimationTimer;
		short iKuriboShoeAnimationFrame;
		short iKuriboShoeExitTimer;
		short iKuriboShoeExitIndex;

		short iSecretCodeTimer;
		short iSecretCodeIndex;

		short iDumpCollectionCardTimer;
		short iDumpCollectionCardIndex;

		bool fSuperStomp;
		short iSuperStompTimer;
		short iSuperStompExitTimer;
		bool superstomp_lock;

		short burnuptimer;
		short burnupstarttimer;

		short outofarenatimer;
		short outofarenadisplaytimer;

		short suicidetimer;
		short suicidecounttimer;
		short suicidedisplaytimer;

		short action;

		short powerup;
		short projectilelimit;
		short tanookilimit;

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
		short iPlatformCollisionPlayerId;

		//ID of the player that owns this player for Owned mode
		short ownerPlayerID;
		short ownerColorOffsetX;

		//If greater than 0, the player is jailed and moves slowly for Jail mode
		short jail;
		short jailcolor;
		short jailtimer;

		//if true, the player is a shyguy in shyguy mode
		bool shyguy;

		bool fallthrough;

		short diedas;  //What sprite the player died as 0 == mario, 1 == chocobo, 2 == bobomb

		short spawntext;

		short iSuicideCreditPlayerID;
		short iSuicideCreditTimer;

		Spotlight * sSpotlight;

		friend bool coldec_player2player(CPlayer * o1, CPlayer * o2);
		friend void collisionhandler_p2p(CPlayer * o1, CPlayer * o2);
		friend void _collisionhandler_p2p_pushback(CPlayer * o1, CPlayer * o2);

		friend void TransferTag(CPlayer * o1, CPlayer * o2);
		friend void TransferShyGuy(CPlayer * o1, CPlayer * o2);
		friend void BounceAssistPlayer(CPlayer * o1, CPlayer * o2);

		friend bool coldec_player2obj(CPlayer * o1, CObject * o2);
		friend bool collisionhandler_p2o(CPlayer * o1, CObject * o2);

		friend short PlayerKilledPlayer(short iKiller, CPlayer * killed, short deathstyle, killstyle style, bool fForce, bool fKillCarriedItem);
		friend short PlayerKilledPlayer(CPlayer * killer, CPlayer * killed, short deathstyle, killstyle style, bool fForce, bool fKillCarriedItem);

		friend void AddAwardKill(CPlayer * killer, CPlayer * killed, killstyle style);
		friend void RemovePlayersButHighestScoring();
		friend void RemovePlayersButTeam(short teamID);
		friend bool RemoveTeam(short teamid);
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
		friend class CGM_ShyGuyTag;
		friend class CGM_Coins;
		friend class CGM_Survival;
		friend class CGM_Eggs;
		friend class CGM_Domination;
		friend class CGM_KingOfTheHill;
		friend class OMO_KingOfTheHillZone;
		friend class CGM_Race;
		friend class CGM_Frenzy;
		friend class CGM_Owned;
		friend class CGM_Star;
		friend class CGM_CaptureTheFlag;
		friend class CGM_Stomp;
		friend class CGM_Greed;
		friend class CGM_Health;
		friend class CGM_Collection;
		friend class CGM_Chase;

		friend class CGM_Bonus;
		friend class CGM_Pipe_MiniGame;
		friend class CGM_Boss_MiniGame;
		friend class CGM_Boxes_MiniGame;
		
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
		friend class B_WeaponBreakableBlock;

		friend class PU_ExtraGuyPowerup;
		friend class PU_StarPowerup;
		friend class PU_PoisonPowerup;
		friend class PU_MysteryMushroomPowerup;
        friend class PU_Tanooki;
		friend class PU_ExtraHeartPowerup;
		friend class MysteryMushroomTempPlayer;
		friend class PU_TreasureChestBonus;
		friend class PU_CoinPowerup;

		friend class CO_Egg;
		friend class CO_Star;
		friend class CO_Flag;
		
		friend class MO_Coin;
		friend class MO_CollectionCard;

		friend class IO_MovingObject;

		friend class MO_Yoshi;
		friend class OMO_Thwomp;
		friend class MO_Podobo;
		friend class OMO_BowserFire;
		friend class OMO_Area;
		friend class OMO_RaceGoal;
		friend class MO_Explosion;
		friend class MO_SpinAttack;
		friend class MO_AttackZone;
		friend class OMO_OrbitHazard;
		friend class OMO_StraightPathHazard;
		friend class IO_FlameCannon;
		friend class MO_PirhanaPlant;
		friend class OMO_PipeCoin;
		friend class OMO_PipeBonus;
		friend class OMO_Phanto;

		friend class MO_CarriedObject;

		friend class MO_Fireball;
		friend class MO_Hammer;
		friend class MO_IceBlast;
		friend class MO_Boomerang;
		friend class CO_Shell;
		friend class CO_ThrowBlock;
		friend class CO_Spring;
		friend class CO_Spike;
		friend class CO_Bomb;
		friend class CO_KuriboShoe;
		friend class CO_ThrowBox;

		friend class MO_SuperFireball;
		friend class MO_SledgeHammer;
		
		friend class MO_WalkingEnemy;
		friend class MO_Goomba;
		friend class MO_Koopa;
		friend class MO_BuzzyBeetle;
		friend class MO_Spiny;
		friend class MO_SledgeBrother;
		friend class MO_CheepCheep;
		friend class MO_BulletBill;
		friend class MO_FlagBase;

		friend class MO_BonusHouseChest;

		friend class FallingPath;
		friend class MovingPlatform;

		friend bool SwapPlayers(short iUsingPlayerID);
		friend CPlayer * GetPlayerFromGlobalID(short iGlobalID);

		friend class CPlayerAI;
		friend class CSimpleAI;
};


void collisionhandler_p2p(CPlayer * o1, CPlayer * o2);
void _collisionhandler_p2p__pushback(CPlayer * o1, CPlayer * o2);

void TransferTag(CPlayer * o1, CPlayer * o2);
void TransferShyGuy(CPlayer * o1, CPlayer * o2);
void BounceAssistPlayer(CPlayer * o1, CPlayer * o2);

#endif  //_PLAYER_H

