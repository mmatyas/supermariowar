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
#include "player_components/PlayerShield.h"
#include "player_components/PlayerSpinStatus.h"
#include "player_components/PlayerSuicideTimer.h"
#include "player_components/PlayerSuperStomp.h"
#include "player_components/PlayerTail.h"
#include "player_components/PlayerTanookiSuit.h"
#include "player_components/PlayerWarpStatus.h"
#include "player_components/PlayerWings.h"

namespace NetPkgs {
    struct MapCollision;
    struct P2PCollision;
}

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
// TODO: check if this could be a child of CObject
class CPlayer
{
public:
    CPlayer(short iGlobalID, short iLocalID,
            short iTeamID, short iSubTeamID, short iTeamColorID,
            gfxSprite * nsprites[PGFX_LAST],
            CScore *nscore, short * respawnCounter, CPlayerAI * ai);
    ~CPlayer();

    void Init();

    /* Player info */

    short getGlobalID() const { return globalID; }
    short getTeamID() const   { return teamID;   }
    short getColorID() const  { return colorID;  }

    short leftX() const { return ix; }
    short rightX() const { return ix + PW; }
    short centerX() const { return ix + HALFPW; }
    short topY() const { return iy; }
    short bottomY() const { return iy + PH; }
    short centerY() const { return iy + HALFPH; }
    bool isFacingRight() const;

    float getVelX() const { return velx; }
    float getVelY() const { return vely; }

    bool isready() const { return state == player_ready; }
    bool isspawning() const { return state == player_spawning; }
    bool iswarping() const { return state > player_ready; }
    bool isdead() const { return state == player_dead; }

    bool isInvincible() const;
    bool isShielded() const;
    bool isFrozen() const { return frozen; }
    short GetWarpPlane() const { return warpstatus.getWarpPlane(); }
    short GetWarpState() const { return state % 4; }
    bool IsInvincibleOnBottom() const;
    bool IsSuperStomping() const;
    bool IsBobomb() const { return bobomb; }
    bool IsTanookiStatue() const { return tanookisuit.isStatue(); }

    CScore& Score();
    PlayerJail& Jail() { return jail; }
    PlayerShield& Shield() { return shield; }

    /* Drawing */

    void draw();
    void drawOutOfScreenIndicators();
    void drawswap();

    /* Actions */

    void updateswap();
    void move();

    void CommitAction();
    bool bouncejump();

    void collidesWith(CPlayer*);
    bool collidesWith(CObject*);

    void die(short deathStyle, bool fTeamRemoved, bool fKillCarriedItem);
    void SetCorpseType(short); // see diedas
    short CorpseType(short) const { return diedas; }

    /* Award effects and score */

    void spawnText(const char * szText);
    void DeathAwards();
    void AddKillerAward(CPlayer* killed, killstyle style);
    void AddKillsInRowInAirAward();
    gfxSprite ** GetScoreboardSprite() { return pScoreboardSprite; }

    /* Map elements */

    bool PressedAcceptItemKey() const { return fPressedAcceptItem; }
    bool IsAcceptingItem() const { return fAcceptingItem && tanookisuit.notStatue() && !kuriboshoe.is_on(); }
    bool AcceptItem(MO_CarriedObject* item);

    void SetKuriboShoe(KuriboShoeType type);

    /* Powerup related */

    void SetPowerup(short iPowerup);
    void SetStoredPowerup(short iPowerup);
    void StripPowerups();

    void DecreaseProjectileLimit();
    void decreaseProjectilesCount() {
        if (projectiles > 0)
            projectiles--;
    }
    void increaseProjectilesCount(unsigned amount) {
        projectiles += amount;
    }

    bool shyguy; //if true, the player is a shyguy in shyguy mode

private:
    short globalID;
    short teamID;

    void SetupNewPlayer();

    void cpu_think();

		void updateSprite();
		void Jump(short iMove, float jumpModifier, bool fKuriboBounce);

		void update_waitingForRespawn();
		void update_respawning();
		void update_usePowerup();
		void update_spriteColor();
		void updateFrozenStatus(int keymask);

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

        void trySuperJumping(short movement_direction);
		void tryCapeDoubleJump(short movement_direction);
		void tryStartFlying();
		void tryShakingTail();
		void tryReleasingPowerup();
		void useSpecialPowerup();
		void releaseCarriedItem();
		void updateFlyingStatus();
		void enableFreeFall();

		void accelerate(float);
		void accelerateRight();
		void accelerateLeft();
		void decreaseVelocity();

		bool FindSpawnPoint();
		void collision_detection_map();
		void flipsidesifneeded();
		void makeinvincible();
		void makefrozen(short iTime);
		void turnslowdownon();
		bool isstomping(CPlayer * o);

		void mapcolldet_handlePlatformVelocity(float &fPlatformVelX, float &fPlatformVelY);
		bool mapcolldet_handleOutOfScreen();
		void mapcolldet_moveHorizontally(short direction);
		void mapcolldet_moveUpward(short, short, short, short, short, float);
		void mapcolldet_moveDownward(short, short, short, short, short, float);

		PlayerCollisions collisions;
		PlayerAwardEffects awardeffects;

		short KillPlayerMapHazard(bool fForce, killstyle ks, bool fKillCarriedItem, short iPlayerId = -1);

		bool canSuperStomp() const;
		bool wantsToSuperStomp() const;
		bool highJumped() const;

        void draw_spotlight();
        void draw_powerupRing();
        void draw_winnerCrown();

        void triggerPowerup();

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

		uint8_t sprite_state;
		short sprswitch;

		PlayerInvincibility invincibility;
		PlayerShield shield;

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

		bool net_waitingForPowerupTrigger;

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

		friend class OMO_KingOfTheHillZone; // update()

		friend class CGM_CaptureTheFlag; // playerkilledself()
		friend class CGM_Chase; // think()
		friend class CGM_Chicken; // think()
		friend class CGM_Eggs; // playerkilledself()
		friend class CGM_Frenzy; // think()
		friend class CGM_Owned; // think()
		friend class CGM_ShyGuyTag; // init()
		friend class CGM_Star; // think()

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

		friend class CO_Star;
		friend class CO_Flag;

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
		friend struct Net_PlayerData;
		friend struct NetPkgs::MapCollision;
		friend struct NetPkgs::P2PCollision;

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
