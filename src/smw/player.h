#ifndef PLAYER_H
#define PLAYER_H

#include "ai.h"
#include "gfx.h"
#include "GlobalConstants.h"
#include "Score.h"
#include "PlayerKillStyles.h"

#include "player_components/PlayerAwardEffects.h"
#include "player_components/PlayerBurnupTimer.h"
#include "player_components/PlayerCape.h"
#include "player_components/PlayerCardCollection.h"
#include "player_components/PlayerCollisions.h"
#include "player_components/PlayerInvincibility.h"
#include "player_components/PlayerJail.h"
#include "player_components/PlayerKuriboShoe.h"
#include "player_components/PlayerOutOfArenaTimer.h"
#include "player_components/PlayerSecretCode.h"
#include "player_components/PlayerSpinStatus.h"
#include "player_components/PlayerSuicideTimer.h"
#include "player_components/PlayerSuperStomp.h"
#include "player_components/PlayerTail.h"
#include "player_components/PlayerTanookiSuit.h"
#include "player_components/PlayerWarpStatus.h"
#include "player_components/PlayerWings.h"

enum PlayerState {
	player_wait,
    player_spawning,
    player_dead,
    player_ready,
    player_entering_warp_up,
    player_entering_warp_right,
    player_entering_warp_down,
    player_entering_warp_left,
    player_exiting_warp_down,
    player_exiting_warp_left,
    player_exiting_warp_up,
    player_exiting_warp_right
};

enum PlayerAction {
	player_action_none,
    player_action_bobomb,
    player_action_fireball,
    player_action_hammer,
    player_action_boomerang,
    player_action_iceblast,
    player_action_bomb,
    player_action_spincape,
    player_action_spintail
};

class CObject;
class MO_CarriedObject;
class MovingPlatform;
class Spotlight;
struct Warp;


enum deathstyle {
	death_style_jump = 0,
    death_style_squish = 1,
    death_style_shatter = 2
};

//the player class - a lot of optimization can be done here
//(especially the collision detection stuff in collision_detection_map())
class CPlayer
{
	public:
		CPlayer(short iGlobalID, short iLocalID, short iTeamID,
			short iSubTeamID, short iTeamColorID,
			gfxSprite * nsprites[PGFX_LAST],
			CScore *nscore, short * respawnCounter, CPlayerAI * ai);
		~CPlayer();

		void Init();

		void draw();
		void drawOutOfScreenIndicators();
		void updateswap();
		void drawswap();
		void move();
		void mapcollisions();
		void cpu_think();

        short leftX() { return ix; }
        short rightX() { return ix + PW; }
        short centerX() { return ix + HALFPW; }
        short topY() { return iy; }
        short bottomY() { return iy + PH; }
        short centerY() { return iy + HALFPH; }

        bool isInvincible();
        bool isShielded() { return shield; }
        bool isFrozen() { return frozen; }

		bool collidesWith(CPlayer*);
		bool collidesWith(CObject*);

		void CommitAction();

		void die(short deathStyle, bool fTeamRemoved, bool fKillCarriedItem);

		void spawnText(const char * szText);
		void DeathAwards();
		void AddKillerAward(CPlayer* killed, killstyle style);
		void AddKillsInRowInAirAward();
		void SetupNewPlayer();

		void updateFrozenStatus(int keymask);
		void accelerateRight();
		void accelerateLeft();
		void decreaseVelocity();

	    short getGlobalID() { return globalID; }
	    short getTeamID()   { return teamID;   }
	    short getColorID()  { return colorID;  }

		bool bouncejump();

    short GetWarpPlane() {
        return warpstatus.getWarpPlane();
    }
		bool IsPlayerFacingRight();

    bool IsAcceptingItem() {
        return fAcceptingItem && tanookisuit.notStatue() && !kuriboshoe.is_on();
    }
    bool PressedAcceptItemKey() {
        return fPressedAcceptItem;
    }
		bool AcceptItem(MO_CarriedObject * item);

		void SetPowerup(short iPowerup);
    gfxSprite ** GetScoreboardSprite() {
        return pScoreboardSprite;
    }

		void DecreaseProjectileLimit();

    bool isready() { return state == player_ready; }
    bool isspawning() { return state == player_spawning; }
    bool iswarping() { return state > player_ready; }
    bool isdead() { return state == player_dead; }

		void SetKuriboShoe(KuriboShoeType type);
    bool IsInvincibleOnBottom() {
        return isInvincible() || shield || kuriboshoe.is_on();
    }
    bool IsSuperStomping() {
        return superstomp.isStomping();
    }

    void decreaseProjectilesCount() {
        if (projectiles > 0)
            projectiles--;
    }

    void increaseProjectilesCount(int amount) {
            projectiles += amount;
    }

        void SetStoredPowerup(short iPowerup);
		void StripPowerups();

		short shield;
		//if true, the player is a shyguy in shyguy mode
		bool shyguy;
		short globalID;
		short teamID;
	private:
		void SetSprite();
		void Jump(short iMove, float jumpModifier, bool fKuriboBounce);

		void update_waitingForRespawn();
		void update_respawning();
		void update_usePowerup();
		void update_spriteColor();

		void tryFallingThroughPlatform(short);

		void ClearPowerupStates();

        void setXf(float xf) {
            fx = xf;
            ix = (short)fx;
        }
        void setXi(short xi) {
            ix = xi;
            fx = (float)ix;
        }
        void setYf(float yf) {
            fy = yf;
            iy = (short)fy;
        }
        void setYi(short yi) {
            iy = yi;
            fy = (float)iy;
        }

		bool FindSpawnPoint();
		void collision_detection_map();
		void flipsidesifneeded();
		void makeinvincible();
		void makefrozen(short iTime);
		void turnslowdownon();
		bool isstomping(CPlayer * o);

		PlayerCollisions collisions;
		PlayerAwardEffects awardeffects;

		short KillPlayerMapHazard(bool fForce, killstyle ks, bool fKillCarriedItem, short iPlayerId = -1);

		bool canSuperStomp();
		bool wantsToSuperStomp();
		bool highJumped();

		COutputControl * playerKeys;
		short playerDevice;

        // tanooki
        PlayerTanookiSuit tanookisuit;

		CScore *score;
		short killsinrow;
		short killsinrowinair;

		short localID;
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

		PlayerTail tail;
		PlayerSpinStatus spin;
		PlayerWings wings;

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

		uint8_t spr;
		short sprswitch;

		PlayerInvincibility invincibility;

		short shieldtimer;

		bool frozen;
		short frozentimer;

		PlayerKuriboShoe kuriboshoe;
		PlayerSecretCode secretcode;
		PlayerCardCollection cardcollection;

		PlayerSuperStomp superstomp;
		PlayerBurnupTimer burnup;
		PlayerOutOfArenaTimer outofarena;
		PlayerSuicideTimer suicidetimer;

		short action;

		short powerup;
		short projectilelimit;
		int projectiles;

		bool bobomb;

		PlayerCape cape;

		PlayerState state;
		float spawnradius;
		float spawnangle;
		short spawntimer;

		short waittimer;
		short * respawncounter;

		PlayerWarpStatus warpstatus;

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
		PlayerJail jail;


		bool fallthrough;

		short diedas;  //What sprite the player died as 0 == mario, 1 == chocobo, 2 == bobomb

		short spawntext;

		short iSuicideCreditPlayerID;
		short iSuicideCreditTimer;

		Spotlight * sSpotlight;

		void TransferTag(CPlayer * o2);
		void TransferShyGuy(CPlayer * o2);
		void BounceAssistPlayer(CPlayer * o2);

		friend bool coldec_player2player(CPlayer * o1, CPlayer * o2);
		friend bool coldec_player2obj(CPlayer * o1, CObject * o2);
		friend bool collisionhandler_p2o(CPlayer * o1, CObject * o2);

		friend short PlayerKilledPlayer(short iKiller, CPlayer * killed, short deathstyle, killstyle style, bool fForce, bool fKillCarriedItem);
		short KilledPlayer(CPlayer * killed, short deathstyle, killstyle style, bool fForce, bool fKillCarriedItem);

		friend void RemovePlayersButHighestScoring();
		friend void RemovePlayersButTeam(short teamID);
		friend bool RemoveTeam(short teamid);
		friend short CountAliveTeams(short * lastteam);
			friend void shakeScreen();
			friend void handleP2PCollisions();
			friend void handleP2ObjCollisions();
			friend void drawScoreboard(short int*);
			friend void drawPlayerSwap();

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

//		friend class MO_Fireball;
//		friend class MO_SuperFireball;
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

		friend class NetClient;
		friend class NetGameHost;

		friend class GameplayState;

		friend class PlayerAwardEffects;
		friend class PlayerBurnupTimer;
		friend class PlayerCollisions;
		friend class PlayerCape;
		friend class PlayerJail;
		friend class PlayerKuriboShoe;
		friend class PlayerTanookiSuit;
		friend class PlayerOutOfArenaTimer;
		friend class PlayerSpinStatus;
		friend class PlayerSuicideTimer;
		friend class PlayerSuperStomp;
		friend class PlayerTail;
		friend class PlayerWarpStatus;
		friend class PlayerWings;
		friend class PlayerInvincibility;
};

#endif // PLAYER_H
