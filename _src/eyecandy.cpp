
#include <string.h>
#include <math.h>

#include "global.h"

//------------------------------------------------------------------------------
// class cloud
//------------------------------------------------------------------------------
EC_Cloud::EC_Cloud(gfxSprite *nspr, float nx, float ny, float nvelx)
{
	spr = nspr;
	x = nx;
	y = ny;
	velx = nvelx;
	w = (short)spr->getWidth();
}


void EC_Cloud::draw()
{
	spr->draw((short)x, (short)y);
}


void EC_Cloud::update()
{
	x += velx;
	
	if( x > 639.0f)
		x -= 639.0f;
	else if( x < 0.0f)
		x += 640.0f;
}


//------------------------------------------------------------------------------
// class ghost
//------------------------------------------------------------------------------
EC_Ghost::EC_Ghost(gfxSprite *nspr, float nx, float ny, float nvelx, short ianimationspeed, short inumframes)
{
	spr = nspr;
	x = nx;
	y = ny;
	velx = nvelx;
	w = (short)spr->getWidth() >> 1;
	h = (short)spr->getHeight() >> 1;

	animationoffset = nvelx < 0.0f ? 32 : 0;
	animationspeed = ianimationspeed - 1;
	animationtimer = 0;
	animationframe = 0;
	animationwidth = (short)spr->getWidth();
	numframes = inumframes;
}


void EC_Ghost::draw()
{
	spr->draw((short)x, (short)y, animationframe, animationoffset, w, h);
}


void EC_Ghost::update()
{
	x += velx;

	if( x > 639.0f)
		x -= 639.0f;
	else if( x < 0.0f)
		x += 640.0f;

	if(++animationtimer > animationspeed)
	{
		animationtimer = 0;
		animationframe += w;

		if(++animationframe >= animationwidth)
		{
			animationframe = 0;
		}
	}
}

//------------------------------------------------------------------------------
// class corpse
//------------------------------------------------------------------------------
EC_Corpse::EC_Corpse(gfxSprite *nspr, float nx, float ny, short iSrcOffsetX) :
	CEyecandy()
{
	spr = nspr;
	x = nx;
	y = ny;
	vely = 1.0f;
	timeleft = CORPSESTAY;
	offsetx = iSrcOffsetX;

	if(x + PWOFFSET < 0.0f)
		tx = (short) ( (x + 640.0f + PWOFFSET) / TILESIZE );
	else
		tx = (short) ( (x + PWOFFSET) / TILESIZE );

	if(x + PWOFFSET + PW >= 640.0f)
		tx2 = (short) ( (x + PWOFFSET + PW - 640.0f) / TILESIZE );
	else
		tx2 = (short) ( (x + PWOFFSET + PW) / TILESIZE );
}


void EC_Corpse::draw()
{
	spr->draw((short)x, (short)y, offsetx, 0, 32, 32);
}


void EC_Corpse::update()
{
	if(vely != 0.0f)
	{
		short nexty = (short)(y + 32.0f + vely);

		if(nexty >= 480)
		{
			dead = true;
			return;
		}

		if(nexty >= 0)
		{
			short ty = nexty / TILESIZE;
			
			if(g_map.map(tx, ty) == tile_solid_on_top || g_map.map(tx2, ty) == tile_solid_on_top)
			{	//on ground on tile solid_on_top
				if((y + 32.0f - vely) / TILESIZE < ty)
				{	//only if we were above the tile in the previous frame
					y		= (float) (ty * TILESIZE - 32);
					vely	= 0.0f;
					return;
				}
			}

			IO_Block * leftblock = g_map.block(tx, ty);
			IO_Block * rightblock = g_map.block(tx2, ty);

			if((g_map.map(tx, ty) & 0x05) > 0 || (g_map.map(tx2, ty) & 0x05) > 0 ||
				(leftblock && !leftblock->isTransparent()) || (rightblock && !rightblock->isTransparent()))
			{	//on ground
				y		= (float) (ty * TILESIZE - 32);
				vely	= 0.0f;
				return;
			}
		}
		
		//falling (in air)
		y		+= vely;
		vely	= CapFallingVelocity(GRAVITATION + vely);
	}
	else
	{
		if(timeleft > 0)
			timeleft--;
		else
			dead = true;
	}
}



//------------------------------------------------------------------------------
// class EC_GravText
//------------------------------------------------------------------------------

EC_GravText::EC_GravText(gfxFont *nfont, short nx, short ny, const char *ntext, float nvely) :
	CEyecandy()
{
	font = nfont;
	x = (float)(nx - (font->getWidth(ntext) / 2));
	y = (float)ny;
	w = (short)font->getWidth(ntext);
	
	text = new char[strlen(ntext)+1];	//someone should test if we got the memory...
	
	if(text)
		strcpy(text, ntext);
	else
		dead = true;

	vely = nvely;
}


void EC_GravText::update()
{
	y += vely;
	vely += GRAVITATION;

	if(y > 480)
		dead = true;
}


void EC_GravText::draw()
{
	font->draw((short)x, (short)y, text);

	if(x < 0)
		font->draw((short)x + 640, (short)y, text);
	else if(x + w > 639)
		font->draw((short)x - 640, (short)y, text);
	
}


EC_GravText::~EC_GravText()
{
	delete [] text;
	text = NULL;
}


//------------------------------------------------------------------------------
// class EC_FallingObject
//------------------------------------------------------------------------------

EC_FallingObject::EC_FallingObject(gfxSprite *nspr, short x, short y, float nvelx, float nvely, short numSprites, short animationRate, short srcOffsetX, short srcOffsetY, short w, short h) :
	CEyecandy()
{
	spr = nspr;
	fx = (float)x;
	fy = (float)y;
	vely = nvely;
	velx = nvelx;
	
	iSrcOffsetX = srcOffsetX;
	iSrcOffsetY = srcOffsetY;

	iw = w;
	if(iw == 0)
		iw = (short)spr->getWidth() / numSprites;

	ih = h;
	if(ih == 0)
		ih = (short)spr->getHeight();
	
	iNumSprites = numSprites;
	iAnimationRate = animationRate;
	iDrawFrame = iSrcOffsetX;
	iAnimationTimer = 0;
	iAnimationWidth = iNumSprites * iw;
}

EC_FallingObject::EC_FallingObject(gfxSprite *nspr, short x, short y, float nvely, short srcOffsetX, short srcOffsetY, short w, short h) :
	CEyecandy()
{
	spr = nspr;
	fx = (float)x;
	fy = (float)y;
	vely = nvely;
	velx = 0.0f;
	
	iSrcOffsetX = srcOffsetX;
	iSrcOffsetY = srcOffsetY;
	
	iw = w;
	if(iw == 0)
		iw = (short)spr->getWidth();

	ih = h;
	if(ih == 0)
		ih = (short)spr->getHeight();

	iNumSprites = 1;
	iAnimationRate = 0;
	iDrawFrame = iSrcOffsetX;
	iAnimationTimer = 0;
	iAnimationWidth = iw;
}

void EC_FallingObject::update()
{
	fy += vely;
	fx += velx;
	
	if(fy >= 480.0f)
	{
		dead = true;
		return;
	}

	vely += GRAVITATION;

	if(iAnimationRate > 0)
	{
		if(++iAnimationTimer == iAnimationRate)
		{
			iAnimationTimer = 0;
			iDrawFrame += iw;

			if(iDrawFrame >= iAnimationWidth)
				iDrawFrame = iSrcOffsetX;
		}
	}
}


void EC_FallingObject::draw()
{
	spr->draw((short)fx, (short)fy, iDrawFrame, iSrcOffsetY, iw, ih);	
}

//------------------------------------------------------------------------------
// class EC_SingleAnimation
//------------------------------------------------------------------------------

EC_SingleAnimation::EC_SingleAnimation(gfxSprite *nspr, short nx, short ny, short iframes, short irate) :
	CEyecandy()
{
	spr = nspr;
	x = (float)nx;
	y = (float)ny;
	frames = iframes;
	frame = 0;
	counter = 0;
	rate = irate - 1;

	iw = (short)spr->getWidth() / iframes;
	ih = (short)spr->getHeight();

	iAnimationWidth = (short)spr->getWidth();
}


void EC_SingleAnimation::update()
{
	if(++counter > rate)
	{
		counter = 0;
		frame += iw;

		if(frame >= iAnimationWidth)
		{
			dead = true;
		}
	}
}


void EC_SingleAnimation::draw()
{
	spr->draw((short)x, (short)y, frame, 0, iw, ih);
}

//------------------------------------------------------------------------------
// class EC_LoopingAnimation
//------------------------------------------------------------------------------

EC_LoopingAnimation::EC_LoopingAnimation(gfxSprite *nspr, short x, short y, short iframes, short irate, short iloops, short ioffsetx, short ioffsety, short istartoffsetx, short iwidth, short iheight) :
	CEyecandy()
{
	spr = nspr;
	ix = x;
	iy = y;
	frame = ioffsetx + istartoffsetx * iwidth;
	counter = 0;
	rate = irate - 1;

	iw = iwidth;
	ih = iheight;

	iOffsetX = ioffsetx;
	iOffsetY = ioffsety;

	iAnimationWidth = ioffsetx + iframes * iwidth;

	countloops = 0;
	loops = iloops;
}

void EC_LoopingAnimation::update()
{
	if(++counter > rate)
	{
		counter = 0;
		
		frame += iw;
		if(frame >= iAnimationWidth)
		{
			if(loops > 0 && ++countloops >= loops)
				dead = true;

			frame = iOffsetX;
		}
	}
}

void EC_LoopingAnimation::draw()
{
	spr->draw(ix, iy, frame, iOffsetY, iw, ih);
}

//------------------------------------------------------------------------------
// class award
//------------------------------------------------------------------------------
/*
EC_Award::EC_Award(gfxSprite *nspr, short ix, short iy, short destx, short desty, short iframes, short irate) :
	EC_LoopingAnimation(nspr, ix, iy, iframes, irate, 0)
{
	dx = (float)destx;
	dy = (float)desty;

	velx = (dx - x) / 100.0f;
	vely = (dy - y) / 100.0f;
}


void EC_Award::update()
{
	EC_LoopingAnimation::update();

	x += velx;
	y += vely;

	if( fabsf(dx - x) < fabsf(velx) && fabsf(dy - y) < fabsf(vely))
	{
		dead = true;
	}
}
*/


//------------------------------------------------------------------------------
// class EC_ExplodingAward
//------------------------------------------------------------------------------

EC_ExplodingAward::EC_ExplodingAward(gfxSprite *nspr, short nx, short ny, float nvelx, float nvely, short timetolive, short awardID) :
	CEyecandy()
{
	spr = nspr;
	x = (float)nx;
	y = (float)ny;
	vely = nvely;
	velx = nvelx;
	timer = 0;
	ttl = timetolive;
	
	w = 16;
	h = 16;

	id = awardID * w;
}


void EC_ExplodingAward::update()
{
	y += vely;
	x += velx;
	
	if(++timer > ttl)
	{
		dead = true;
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, (short)x + (w >> 1) - 16, (short)y + (h >> 1) - 16, 3, 8));
	}
}


void EC_ExplodingAward::draw()
{
	spr->draw((short)x, (short)y, id, 0, w, h);
}

//------------------------------------------------------------------------------
// class EC_SwirlingAward
//------------------------------------------------------------------------------

EC_SwirlingAward::EC_SwirlingAward(gfxSprite *nspr, short nx, short ny, float nangle, float nradius, float nvel, short timetolive, short awardID) :
	CEyecandy()
{
	spr = nspr;
	x = nx;
	y = ny;
	vel = nvel;
	angle = nangle;
	radius = nradius;
	timer = 0;
	ttl = timetolive;

	w = (short)spr->getWidth() / 10;
	h = (short)spr->getHeight();

	id = awardID * w;
}


void EC_SwirlingAward::update()
{
	angle += vel;
	radius += 3.0f;
	
	if(++timer > ttl)
	{
		short awardx = x + (short)(radius * cos(angle)) + (w >> 1) - 16;
		short awardy = y + (short)(radius * sin(angle)) + (h >> 1) - 16;
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, awardx, awardy, 3, 8));
		
		dead = true;
	}
}


void EC_SwirlingAward::draw()
{
	short awardx = x + (short)(radius * cos(angle));
	short awardy = y + (short)(radius * sin(angle));
		
	spr->draw(awardx, awardy, id, 0, w, h);
}


//------------------------------------------------------------------------------
// class EC_RocketAward
//------------------------------------------------------------------------------

EC_RocketAward::EC_RocketAward(gfxSprite *nspr, short nx, short ny, float nvelx, float nvely, short timetolive, short awardID, short numAwards) :
	CEyecandy()
{
	spr = nspr;
	x = (float)nx;
	y = (float)ny;
	velx = nvelx;
	vely = nvely;

	timer = 0;
	ttl = timetolive;
	
	w = (short)spr->getWidth() / numAwards;
	h = (short)spr->getHeight();

	id = awardID * w;
}


void EC_RocketAward::update()
{
	vely += 0.2f;

	y += vely;
	x += velx;

	if(++timer > ttl)
	{
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, (short)x + (w >> 1) - 16, (short)y + (h >> 1) - 16, 3, 8));
		dead = true;
	}
}


void EC_RocketAward::draw()
{
	spr->draw((short)x, (short)y, id, 0, w, h);
}

//------------------------------------------------------------------------------
// class EC_FloatingAward
//------------------------------------------------------------------------------

EC_FloatingObject::EC_FloatingObject(gfxSprite *nspr, short nx, short ny, float nvelx, float nvely, short timetolive, short nsrcx, short nsrcy, short nwidth, short nheight) :
	CEyecandy()
{
	spr = nspr;
	x = (float)nx;
	y = (float)ny;
	velx = nvelx;
	vely = nvely;
	srcx = nsrcx;
	srcy = nsrcy;
	w = nwidth;
	h = nheight;

	timer = 0;
	ttl = timetolive;
}


void EC_FloatingObject::update()
{
	y += vely;
	x += velx;

	if(++timer > ttl)
	{
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, (short)x + (w >> 1) - 16, (short)y + (h >> 1) - 16, 3, 8));
		dead = true;
	}
}


void EC_FloatingObject::draw()
{
	spr->draw((short)x, (short)y, srcx, srcy, w, h);
}

//------------------------------------------------------------------------------
// class EC_SoulsAward
//------------------------------------------------------------------------------

EC_SoulsAward::EC_SoulsAward(gfxSprite *nspr, gfxSprite *nspr2, short nx, short ny, short timetolive, float nSpeed, short nSouls, short * nSoulArray) :
	CEyecandy()
{
	spr = nspr;
	spawnspr = nspr2;

	x = nx;
	y = ny;
	
	ttl = timetolive;
	numSouls = nSouls;

	if(numSouls > MAXAWARDS)
		numSouls = MAXAWARDS;

	id = new short[numSouls];

	for(short k = 0; k < numSouls; k++)
		id[k] = nSoulArray[k];

	timer = 0;
	speed = nSpeed;
	count = 0;

	if(numSouls < 1)
		dead = true;

	animationCount = 0;
	frame = 0;
	endmode = false;

	w = (short)nspr2->getWidth() / 7;
	h = (short)nspr2->getHeight();

}

EC_SoulsAward::~EC_SoulsAward()
{
	delete [] id;
}

void EC_SoulsAward::update()
{
	if(!endmode && ++timer > speed)
	{
		timer = 0;

		float addangle = QUARTER_PI / 20.0f;
		float startangle = -HALF_PI;
	
		float angle = (float)(rand()%21 - 10) * addangle + startangle;
		float velx = speed * cos(angle);
		float vely = speed * sin(angle);

		eyecandyfront.add(new EC_RocketAward(&spr_awardsouls, x - 8, y - 8, velx, vely, ttl, id[count], 12));
		
		if(++count >= numSouls)
		{
			endmode = true;
			frame = 4;
		}
	}

	if(++animationCount > 6)
	{
		animationCount = 0;

		frame++;

		if(!endmode && frame >= 4)
			frame = 0;
		else if(endmode && frame >= 7)
			dead = true; 
	}
}


void EC_SoulsAward::draw()
{
	spawnspr->draw(x - 16, y - 16, frame * w, 0, w, h);
}

//------------------------------------------------------------------------------
// class EC_DoorFront
//------------------------------------------------------------------------------

EC_Door::EC_Door(gfxSprite *nspr, gfxSprite *nmariospr, short nx, short ny, short irate, short xOffset) :
	CEyecandy()
{
	spr = nspr;
	mariospr = nmariospr;

	x = nx;
	y = ny;
	timer = 0;
	rate = irate;

	iw = (short)spr->getWidth() / 10;
	ih = (short)spr->getHeight();

	offsety = ih;
	state = 0;
	frame = 0;

	offsetx = xOffset;
}


void EC_Door::update()
{
	if(++timer > rate)
	{
		timer = 0;

		if(state == 0)
		{
			offsety -= 3;

			if(offsety <= 0)
			{
				state = 1;
				offsety = 0;
			}

			frame = 1;	
		}
		else if(state == 1)
		{
			if(++frame >= 9)
			{
				state = 2;
				frame = 9;
			}
		}
		else if(state == 2)
		{
			if(--frame <= 1)
			{
				state = 3;
				frame = 1;
			}
		}
		else if(state == 3)
		{
			offsety += 3;

			if(offsety >= 32)
			{
				dead = true;
				offsety = 32;
			}

			frame = 1;	
		}
	}
}

void EC_Door::draw()
{
	spr->draw(x, y + offsety, 0, 0, iw, ih - offsety);

	if(state == 1)
		mariospr->draw(x + 16 - HALFPW - PWOFFSET, y + 16 - HALFPH - PHOFFSET + offsety, offsetx, 0, iw, ih - offsety);

	spr->draw(x, y + offsety, frame * iw, 0, iw, ih - offsety);
}

//------------------------------------------------------------------------------
// class EC_BossPeeker
//------------------------------------------------------------------------------

EC_BossPeeker::EC_BossPeeker(gfxSprite *nspr, short speed, short bossType) :
	CEyecandy()
{
	spr = nspr;
	iSpeed = speed;
	iBossColorOffsetY = bossType * 64;

	timer = 0;
	state = 0;
	ix = 592;
	iy = 480;

	game_values.bosspeeking = bossType;
}


void EC_BossPeeker::update()
{
	if(state == 0)
	{
		iy -= 2;
		if(iy <= 432)
		{
			iy = 432;
			state = 1;
		}
	}
	else if(state == 1)
	{
		if(++timer == iSpeed)
			state = 2;
	}
	else if(state == 2)
	{
		iy += 2;
		if(iy >= 480)
		{
			dead = true;
			game_values.bosspeeking = -1;
		}
	}
}


void EC_BossPeeker::draw()
{
	spr->draw(ix, iy, 192, iBossColorOffsetY, 48, 64);
}

//------------------------------------------------------------------------------
// class eyecandy_container
//------------------------------------------------------------------------------
CEyecandyContainer::CEyecandyContainer()
{
	for(short i = 0; i < MAXEYECANDY; i++)
		list[i] = NULL;

	list_end = 0;
}


CEyecandyContainer::~CEyecandyContainer()
{
	clean();
}


void CEyecandyContainer::clean()
{
	for(short i = 0; i < list_end; i++)
	{
		delete list[i];
		list[i] = NULL;
	}
	list_end = 0;
}


short CEyecandyContainer::add(CEyecandy *ec)
{
	if(list_end < MAXEYECANDY)
	{
		list[list_end] = ec;
		ec->dead = false;
		list_end++;

		return list_end - 1;
	}
	else
	{
		delete ec;	//otherwise memory leak!
		printf("eyecandy list full!\n");
	}

	return -1;
}


void CEyecandyContainer::remove(short i)
{	
	delete list[i];
	list_end--;

	if(i != list_end)
	{		//if we didn't remove the last element we move the last elemnt in the new free place in the eyecandy list
		list[i] = list[list_end];
	}
}

void CEyecandyContainer::cleandeadobjects()
{
	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->dead)
		{
			delete list[i];
			list_end--;

			if(i != list_end)
			{
				list[i] = list[list_end];
			}

			i--;
		}
	}
}

