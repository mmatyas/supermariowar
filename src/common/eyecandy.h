#ifndef EYECANDY_H
#define EYECANDY_H

#include "gfx.h"
#include "GlobalConstants.h"

#include <vector>

//this system is pretty cool ;)
//it can be easily changed to handle every object in the game


//eyecandy base class
class CEyecandy
{
public:
    CEyecandy() {
        dead = false;
    }
    virtual ~CEyecandy() {}

    virtual void update() = 0;
    virtual void draw() = 0;

protected:
    bool dead;

    friend class CEyecandyContainer;
};

class EC_StillImage : public CEyecandy
{
public:
    EC_StillImage(gfxSprite * nspr, short dstx, short dsty, short srcx, short srcy, short w, short h);
    virtual ~EC_StillImage() {}

    virtual void update() {}
    virtual void draw();

protected:
    gfxSprite * spr;

    short iSrcX, iSrcY;
    short ix, iy;
    short iw, ih;
};

//animated eyecandy base class
class EC_Animated : public CEyecandy
{
public:
    EC_Animated(gfxSprite * nspr, short dstx, short dsty, short srcx, short srcy, short w, short h, short speed, short frames);
    virtual ~EC_Animated() {}

    virtual void animate();

    virtual void update();
    virtual void draw();

protected:
    gfxSprite * spr;

    short iAnimationX, iAnimationY;
    short iAnimationW, iAnimationH;
    short iAnimationSpeed;
    short iAnimationFrames;

    short iAnimationTimer;
    short iAnimationFrame;

    short ix, iy;
};

class EC_OscillatingAnimation : public EC_Animated
{
public:
    EC_OscillatingAnimation(gfxSprite * nspr, short dstx, short dsty, short srcx, short srcy, short w, short h, short speed, short frames);
    virtual ~EC_OscillatingAnimation() {}

    virtual void animate();

protected:
    bool fForward;
};

//actual eyecandy implementations
class EC_Corpse : public EC_StillImage
{
public:
    EC_Corpse(gfxSprite *nspr, float nx, float ny, short iSrcOffsetX);
    ~EC_Corpse() {}
    void update();

private:
    float dx, dy;
    short tx, tx2;
    float vely;
    short timeleft;
    short offsetx;
};


class EC_Cloud : public EC_StillImage
{
public:
    EC_Cloud(gfxSprite *nspr, float nx, float ny, float nvelx, short srcx, short srcy, short w, short h);
    ~EC_Cloud() {}
    void update();

private:
    float dx, dy;
    float velx;
};

class EC_Ghost : public EC_Animated
{
public:
    EC_Ghost(gfxSprite *nspr, float nx, float ny, float nvelx, short ianimationspeed, short inumframes, short srcx, short srcy, short w, short h);
    ~EC_Ghost() {}
    void update();

private:
    float dx, dy;
    float velx;
};

class EC_Leaf : public EC_OscillatingAnimation
{
public:
    EC_Leaf(gfxSprite *nspr, float nx, float ny);
    ~EC_Leaf() {}

    void update();

private:
    void NextLeaf();

    float dx, dy;
    float velx, vely;
};

class EC_Snow : public EC_StillImage
{
public:
    EC_Snow(gfxSprite *nspr, float nx, float ny, short type);
    ~EC_Snow() {}

    void update();

private:
    float dx, dy;
    float velx, vely;
};

class EC_Rain : public EC_StillImage
{
public:
    EC_Rain(gfxSprite *nspr, float nx, float ny);
    ~EC_Rain() {}

    void update();

private:
    void NextRainDrop();

    float dx, dy;
    float velx, vely;
};

class EC_Bubble : public EC_OscillatingAnimation
{
public:
    EC_Bubble(gfxSprite *nspr, float nx, float ny);
    ~EC_Bubble() {}

    void update();

private:
    void NextBubble();

    float dx, dy;
    float velx, vely;
};

class EC_GravText : public CEyecandy
{
public:
    EC_GravText(gfxFont *font, short nx, short ny, const char *text, float nvely);
    ~EC_GravText();
    void draw();
    void update();

private:
    gfxFont *font;
    float x, y;
    short w;
    float vely;
    char *text;
};

class EC_Announcement : public CEyecandy
{
public:
    EC_Announcement(gfxFont *nfont, gfxSprite *nsprite, const char *ntext, short icon, short time, short y);
    ~EC_Announcement();
    void draw();
    void update();

private:
    gfxFont *font;
    gfxSprite *sprite;

    short ix, iy;
    char *text;

    short iTime, iTimer;
    short iIcon;

    short iFontY, iFontOffsetX;

    SDL_Rect rSrcRect[4];
    SDL_Rect rDstRect[4];
};

class EC_FallingObject : public EC_Animated
{
public:
    EC_FallingObject(gfxSprite *nspr, short x, short y, float nvelx, float nvely, short animationframes, short animationspeed, short srcOffsetX, short srcOffsetY, short w, short h);

    void update();

private:
    float fx, fy;
    float vely, velx;
};

class EC_SingleAnimation : public EC_Animated
{
public:
    EC_SingleAnimation(gfxSprite *nspr, short nx, short ny, short iframes, short irate);
    EC_SingleAnimation(gfxSprite *nspr, short nx, short ny, short iframes, short irate, short offsetx, short offsety, short w, short h);

    void update();
};

class EC_LoopingAnimation : public EC_Animated
{
public:
    EC_LoopingAnimation(gfxSprite *nspr, short x, short y, short iframes, short irate, short loops, short ioffsetx, short ioffsety, short w, short h);
    void update();

private:
    short iCountLoops;
    short iLoops;
};

/*
//actual eyecandy implementations
class EC_Award : public EC_LoopingAnimation
{
	public:
		EC_Award(gfxSprite *nspr, short ix, short iy, short destx, short desty, short iframes, short irate);
		~EC_Award() {}
		void update();

	private:
		float dx, dy;
		float vely, velx;
};
*/

class EC_ExplodingAward : public CEyecandy
{
public:
    EC_ExplodingAward(gfxSprite *nspr, short nx, short ny, float nvelx, float nvely, short timetolive, short awardID);
    void draw();
    void update();

private:
    gfxSprite *spr;
    float x, y;
    short w, h;
    short timer, ttl, id;
    float velx, vely;
};

class EC_SwirlingAward : public CEyecandy
{
public:
    EC_SwirlingAward(gfxSprite *nspr, short nx, short ny, float nangle, float nradius, float nvel, short timetolive, short srcX, short srcY, short iw, short ih, short animationRate = 0, short animationFrames = 0);
    void draw();
    void update();

private:
    gfxSprite *spr;
    short x, y;
    short w, h;
    short timer, ttl;
    short iSrcX, iSrcY;
    float vel, angle, radius;

    short iAnimationRate, iAnimationFrames;
    short iAnimationTimer, iAnimationFrame, iAnimationEndFrame;
};

class EC_RocketAward : public CEyecandy
{
public:
    EC_RocketAward(gfxSprite *nspr, short nx, short ny, float nvelx, float nvely, short timetolive, short srcX, short srcY, short iw, short ih, short animationRate = 0, short animationFrames = 0);
    void draw();
    void update();

private:
    gfxSprite *spr;
    float x, y;
    short w, h;
    float velx, vely;
    short timer, ttl;
    short iSrcX, iSrcY;

    short iAnimationRate, iAnimationFrames;
    short iAnimationTimer, iAnimationFrame, iAnimationEndFrame;
};

class EC_FloatingObject : public CEyecandy
{
public:
    EC_FloatingObject(gfxSprite *nspr, short nx, short ny, float nvelx, float nvely, short timetolive, short nsrcx, short nsrcy, short nwidth, short nheight);
    void draw();
    void update();

private:
    gfxSprite *spr;
    float x, y;
    short w, h;
    float velx, vely;
    short timer, ttl;
    short srcx, srcy;
};

class EC_SoulsAward : public CEyecandy
{
public:
    EC_SoulsAward(gfxSprite *nspr, gfxSprite *nspr2, short nx, short ny, short timetolive, float nSpeed, short nSouls, short * nSoulArray);
    ~EC_SoulsAward();
    void draw();
    void update();

private:
    gfxSprite *spr;
    gfxSprite *spawnspr;

    short x, y;
    short numSouls;
    short * id;
    short ttl, timer, count;
    float speed;

    short animationCount, frame;
    bool endmode;
    short w, h;
};

class EC_Door : public CEyecandy
{
public:
    EC_Door(gfxSprite *nspr, gfxSprite *mario, short nx, short ny, short irate, short iOffsetX, short iColor);
    virtual void draw();
    virtual void update();

protected:
    gfxSprite *spr;
    gfxSprite *mariospr;

    short x, y;
    short frame, timer;
    short iw, ih;
    short rate;
    short state;
    short offsetx;
    short offsety;
    short colorOffset;
};

/*
class EC_BossPeeker : public CEyecandy
{
	public:
		EC_BossPeeker(gfxSprite *nspr, short speed, short bossType);
		void draw();
		void update();

	private:
		gfxSprite *spr;
		int iSpeed;
		int iBossColorOffsetY;
		int timer;
		int state;
		int ix, iy;
};
*/

class EC_SuperStompExplosion : public CEyecandy
{
public:
    EC_SuperStompExplosion(gfxSprite *nspr, short x, short y, short irate);
    void update();
    void draw();

protected:
    gfxSprite *spr;
    short ix, iy;
    short iAnimationFrame, iAnimationTimer;
    short iRate;
};

//eyecandy container
class CEyecandyContainer
{
public:
    CEyecandyContainer();
    ~CEyecandyContainer();

    short add(CEyecandy *ec);

    void update() {
        for (short i = 0; i < list_end; i++)
            list[i]->update();
    };
    void draw() {
        for (short i = 0; i < list_end; i++)
            list[i]->draw();
    };

    void clean();

    void cleandeadobjects();

public:
    CEyecandy *list[MAXEYECANDY];
    short		 list_end;

    void remove(short i);
};

class Spotlight
{
public:
    Spotlight(short x, short y, short size);
    ~Spotlight() {}

    void Update();
    void UpdatePosition(short x, short y);
    void Draw();

    bool IsDead() {
        return iState >= 3;
    }

private:
    short ix, iy;
    short iEndSize;
    short iTransparency;
    short iState;
    bool fUpdated;

    short iSizeCounter;
    short iSize;

    short iHalfWidth;
    short iWidth;
    SDL_Rect rSrc;
};

class SpotlightManager
{
public:
    Spotlight * AddSpotlight(short ix, short iy, short iSize);
    void DrawSpotlights();
    void ClearSpotlights();

private:
    std::vector<Spotlight*> spotlightList;

};

#endif // EYECANDY_H
