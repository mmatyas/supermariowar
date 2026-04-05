#pragma once

#include "MovingObjectTypes.h"
#include "ObjectBase.h"

class MovingPlatform;

class IO_MovingObject : public CObject {
public:
    IO_MovingObject(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1, short iAnimationOffsetX = -1, short iAnimationOffsetY = -1, short iAnimationHeight = -1, short iAnimationWidth = -1);

    void draw() override;
    void update() override;
    virtual void animate();

    bool collide(CPlayer* player) override;
    void collide(IO_MovingObject*) override {}

    MovingObjectType getMovingObjectType() const { return movingObjectType; }

    void applyfriction();
    void collision_detection_map();
    bool collision_detection_checksides();
    void flipsidesifneeded();

    virtual void SideBounce(bool fRightSide) {}
    virtual float BottomBounce() { return bounce; }
    void KillObjectMapHazard(short playerID = -1);

    virtual void CheckAndDie() {
        dead = true;
    }
    virtual void Die() {
        dead = true;
    }

    bool CollidesWithMap() const {
        return fObjectCollidesWithMap;
    }

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

    // Pointer to the platform the object is riding
    MovingPlatform* platform;
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

    friend class MovingPlatform;

    friend void removeifprojectile(IO_MovingObject* object, bool playsound, bool forcedead);
    friend void shakeScreen();
};
