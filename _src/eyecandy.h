#ifndef _EYECANDY_H
#define _EYECANDY_H


//this system is pretty cool ;)
//it can be easily changed to handle every object in the game


//eyecandy base class
class CEyecandy
{
	public:
		CEyecandy() {dead = false;}
		virtual ~CEyecandy(){;};

		virtual void draw(){printf("CEyecandy::draw() - NO!\n");};
		virtual void update(){printf("CEyecandy::update() - NO!\n");};
	
	protected:
		bool dead;
	
	friend class CEyecandyContainer;
};





//actual eyecandy implementations
class EC_Corpse : public CEyecandy
{
	public:
		EC_Corpse(gfxSprite *nspr, float nx, float ny, short iSrcOffsetX);
		~EC_Corpse(){;};
		void draw();
		void update();

	private:
		gfxSprite *spr;
		float x, y;
		short tx, tx2;
		float vely;
		short timeleft;
		short offsetx;
};


class EC_Cloud : public CEyecandy
{
	public:
		EC_Cloud(gfxSprite *nspr, float nx, float ny, float nvelx);
		~EC_Cloud(){;};
		void draw();
		void update();

	private:
		gfxSprite *spr;
		float x, y;
		float velx;
		short w;
};


class EC_Ghost : public CEyecandy
{
	public:
		EC_Ghost(gfxSprite *nspr, float nx, float ny, float nvelx, short ianimationspeed, short inumframes);
		~EC_Ghost(){;};
		void draw();
		void update();

	private:
		gfxSprite *spr;
		float x, y;
		float velx;
		short w, h;

		short animationoffset;
		short animationspeed;
		short animationtimer;
		short animationframe;
		short animationwidth;
		short numframes;
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

class EC_FallingObject : public CEyecandy
{
	public:
		EC_FallingObject(gfxSprite *nspr, short x, short y, float nvelx, float nvely, short numSprites, short animationRate, short srcOffsetX, short srcOffsetY, short w, short h);
		EC_FallingObject(gfxSprite *nspr, short x, short y, float nvely, short srcOffsetX, short srcOffsetY, short w, short h);

		void draw();
		void update();

	private:
		gfxSprite *spr;
		float fx, fy;
		short iw, ih;
		float vely, velx;
		
		short iSrcOffsetX;
		short iSrcOffsetY;

		short iNumSprites;
		short iAnimationRate;
		short iDrawFrame;
		short iAnimationTimer;
		short iAnimationWidth;
};

class EC_SingleAnimation : public CEyecandy
{
	public:
		EC_SingleAnimation(gfxSprite *nspr, short nx, short ny, short iframes, short irate);
		void update();
		void draw();

	protected:
		gfxSprite *spr;
		float x, y;
		short frames, frame, counter;
		short iw, ih;
		short rate;
		short iAnimationWidth;
};

class EC_LoopingAnimation : public CEyecandy
{
	public:
		EC_LoopingAnimation(gfxSprite *nspr, short x, short y, short iframes, short irate, short iloops, short ioffsetx, short ioffsety, short istartoffsetx, short iwidth, short iheight);
		void update();
		void draw();
	
	private:
		gfxSprite *spr;
		short ix, iy;
		short frame, counter;
		short iw, ih;
		short rate;
		short iAnimationWidth;

		short countloops;
		short loops;

		short iOffsetX, iOffsetY;

};

/*
//actual eyecandy implementations
class EC_Award : public EC_LoopingAnimation
{
	public:
		EC_Award(gfxSprite *nspr, short ix, short iy, short destx, short desty, short iframes, short irate);
		~EC_Award(){;};
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
		EC_SwirlingAward(gfxSprite *nspr, short nx, short ny, float nangle, float nradius, float nvel, short timetolive, short awardID);
		void draw();
		void update();

	private:
		gfxSprite *spr;
		short x, y;
		short w, h;
		short timer, ttl, id;
		float vel, angle, radius;
};

class EC_RocketAward : public CEyecandy
{
	public:
		EC_RocketAward(gfxSprite *nspr, short nx, short ny, float nvelx, float nvely, short timetolive, short awardID, short numAwards);
		void draw();
		void update();

	private:
		gfxSprite *spr;
		float x, y;
		short w, h;
		float velx, vely;
		short timer, ttl, id;
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
		EC_Door(gfxSprite *nspr, gfxSprite *mario, short nx, short ny, short irate, short iOffsetX);
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
};

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


//eyecandy container
class CEyecandyContainer
{
	public:
		CEyecandyContainer();
		~CEyecandyContainer();

		short add(CEyecandy *ec);

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

		void cleandeadobjects();

	public:
		CEyecandy *list[MAXEYECANDY];
		short		 list_end;

		void remove(short i);
};

#endif

