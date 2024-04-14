#pragma once

#include "gfx/gfxSprite.h"

#include <array>

class CPlayer;
class IO_Block;
class IO_MovingObject;

enum ObjectType {
    object_none = 0,
    object_block = 1,
    object_moving = 2,
    object_overmap = 3,
    object_area = 4,
    object_frenzycard = 5,
    object_race_goal = 6,
    object_thwomp = 7,
    object_kingofthehill_area = 8,
    object_bowserfire = 9,
    object_orbithazard = 10,
    object_bulletbillcannon = 11,
    object_flamecannon = 12,
    object_pathhazard = 13,
    object_pipe_coin = 14,
    object_pipe_bonus = 15,
    object_phanto = 16
};

float CapFallingVelocity(float vel);
float CapSideVelocity(float vel);

//object base class
class CObject {
public:
    CObject(gfxSprite* nspr, short x, short y);
    virtual ~CObject() = default;

    virtual void draw(){};
    virtual void update() = 0;
    virtual bool collide(CPlayer*){ return false; }
    virtual void collide(IO_MovingObject*){}

    virtual ObjectType getObjectType(){ return objectType; }

    void setXf(float xf) {
        fx = xf;
        ix = static_cast<short>(fx);
    }
    void setXi(short xi) {
        ix = xi;
        fx = static_cast<float>(ix);
    }
    void setYf(float yf) {
        fy = yf;
        iy = static_cast<short>((fy < 0.0f) ? (fy - 1.0f) : fy);
    }
    void setYi(short yi) {
        iy = yi;
        fy = static_cast<float>(iy);
    }

    int iNetworkID;
    short ix, iy;

    short collisionWidth;
    short collisionHeight;
    short collisionOffsetX = 0;
    short collisionOffsetY = 0;

    short GetState() const {
        return state;
    }
    bool isDead() const {
        return dead;
    }
    bool GetWrap() const {
        return spr ? spr->GetWrap() : true;
    }

    /// Returns the blocks touching each of the four corners,
    std::array<IO_Block*, 4> GetCollisionBlocks() const;

protected:
    ObjectType objectType = object_none;

    short iw = 0, ih = 0;
    float fx = 0.f, fy = 0.f;
    float velx = 0.f, vely = 0.f;

    gfxSprite* spr = nullptr;
    short state = 0;
    bool dead = false;

    friend class CObjectContainer;
    friend class CPlayer;
    friend void RunGame();
    friend void shakeScreen();
    friend void handleP2ObjCollisions();
    friend class CO_Shell;
    friend class B_BreakableBlock;
    friend class B_WeaponBreakableBlock;
};
