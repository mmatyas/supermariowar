#include <string.h>
#include <math.h>

#include "global.h"

extern short iKingOfTheHillZoneLimits[4][4];
extern void PlayerKilledPlayer(CPlayer &killer, CPlayer &killed, short deathstyle, short killstyle);
extern void AddAwardKill(CPlayer * killer, CPlayer * killed, killstyle style);
extern short LookupTeamID(short id);
extern bool SwapPlayers(short iUsingPlayerID);
extern short scorepowerupoffsets[3][3];
extern void AddHammerKill(short numkills);

extern CPlayer * GetPlayerFromGlobalID(short iGlobalID);

void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead)
{
	if((object->movingObjectType == movingobject_fireball || object->movingObjectType == movingobject_superfireball || object->movingObjectType == movingobject_hammer || object->movingObjectType == movingobject_sledgehammer || object->movingObjectType == movingobject_boomerang) && !object->dead)
	{
		short iPlayerID = -1;
		bool fDie = true;

		if(object->movingObjectType == movingobject_fireball)
		{
			iPlayerID = ((MO_Fireball*)object)->playerID;
		}
		else if(object->movingObjectType == movingobject_hammer)
		{
			iPlayerID = ((MO_Hammer*)object)->playerID;

			if(!game_values.hammerpower)
				fDie = false;
		}
		else if(object->movingObjectType == movingobject_sledgehammer)
		{
			MO_SledgeHammer * hammer = (MO_SledgeHammer*)object;
			iPlayerID = hammer->playerID;
			hammer->explode();
		}
		else if(object->movingObjectType == movingobject_boomerang)
		{
			iPlayerID = ((MO_Boomerang*)object)->playerID;
			game_values.superboomerang[iPlayerID] = 0;
		}
		else if(object->movingObjectType == movingobject_superfireball)
		{
			iPlayerID = ((MO_SuperFireball*)object)->playerID;
		}

		if(fDie || forcedead)
		{
			if(iPlayerID > -1 && projectiles[iPlayerID] > 0)
				projectiles[iPlayerID]--;

			object->dead = true;
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, object->ix + (object->iw >> 1) - 16, object->iy + (object->ih >> 1) - 16, 3, 4));
		}
		
		if(playsound)
			ifsoundonplay(sfx_hit);
	}
}

//------------------------------------------------------------------------------
// class Object base class
//------------------------------------------------------------------------------
CObject::CObject(gfxSprite *nspr1, short x, short y)
{
	dead = false;
	
	spr = nspr1;
	xi(x);
	yi(y);

	iw = (short)spr->getWidth();
	ih = (short)spr->getHeight();

	velx = 0.0f;
	vely = 0.0f;
	
	state = 0;

	collisionWidth = iw;
	collisionHeight = ih;
	collisionOffsetX = 0;
	collisionOffsetY = 0;

	iNetworkID = g_iNextNetworkID++;
}

/*
short CObject::writeNetworkUpdate(char * pData)
{
	//To send different messages from the same object
	//put in a message type ID that tells it to write and
	//read the message differently
	WriteIntToBuffer(&pData[0], iNetworkID);
	WriteShortToBuffer(&pData[4], ix);
	WriteShortToBuffer(&pData[6], iy);
	WriteFloatToBuffer(&pData[8], velx);
	WriteFloatToBuffer(&pData[12], vely);
	return 16;
}

void CObject::readNetworkUpdate(short size, char * pData)
{
	//ReadIntFromBuffer(&iNetworkID, &pData[0]);
	ReadShortFromBuffer(&ix, &pData[4]);
	ReadShortFromBuffer(&iy, &pData[6]);
	ReadFloatFromBuffer(&velx, &pData[8]);
	ReadFloatFromBuffer(&vely, &pData[12]);
}*/


//------------------------------------------------------------------------------
// class Block base class
//------------------------------------------------------------------------------

IO_Block::IO_Block(gfxSprite *nspr, short x, short y) :
	CObject(nspr, x, y)
{
	bumpPlayer = NULL;

	fposx = fx;
	fposy = fy;

	iposx = x;
	iposy = y;

	col = x / TILESIZE;
	row = y / TILESIZE;
}


void IO_Block::draw()
{
	spr->draw(ix, iy);
}


void IO_Block::update()
{}

bool IO_Block::collide(CPlayer * player, short direction, bool useBehavior)
{
	if(direction == 2)
		return hittop(player, useBehavior);
	else if(direction == 0)
		return hitbottom(player, useBehavior);
	else if(direction == 1)
		return hitleft(player, useBehavior);
	else
		return hitright(player, useBehavior);
}

bool IO_Block::hittop(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		player->yf((float)(iposy - PH) - 0.2f);
		player->inair = false;
		player->fallthrough = false;
		player->killsinrowinair = 0;
		player->featherjump = 0;
		player->vely = GRAVITATION;
	}

	return false;
}

bool IO_Block::hitbottom(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		//Player bounces off 
		player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
		player->yf((float)(iposy + ih) + 0.2f);
	}

	return false;
}

bool IO_Block::hitright(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		player->xf((float)(iposx + iw) + 0.2f);
		player->fOldX = player->fx;
		
		if(player->velx < 0.0f)
			player->velx = 0.0f;
	}

	return false;
}

bool IO_Block::hitleft(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		player->xf((float)(iposx - PW) - 0.2f);
		player->fOldX = player->fx;
		
		if(player->velx > 0.0f)
			player->velx = 0.0f;
	}

	return false;
}

bool IO_Block::collide(IO_MovingObject * object, short direction)
{
	if(direction == 2)
		return hittop(object);
	else if(direction == 0)
		return hitbottom(object);
	else if(direction == 1)
		return hitleft(object);
	else
		return hitright(object);
}

bool IO_Block::hittop(IO_MovingObject * object)
{
	object->yf((float)(iposy - object->collisionHeight) - 0.2f);
	object->fOldY = object->fy;
	object->vely = object->BottomBounce();
	return true;
}

bool IO_Block::hitbottom(IO_MovingObject * object)
{
	object->yf((float)(iposy + ih) + 0.2f);
	object->fOldY = object->fy;
	object->vely = -object->vely;
	return true;
}

bool IO_Block::hitright(IO_MovingObject * object)
{
	object->xf((float)(iposx + iw) + 0.2f);
	object->fOldX = object->fx;

	if(object->velx < 0.0f)
		object->velx = -object->velx;

	return true;
}

bool IO_Block::hitleft(IO_MovingObject * object)
{
	object->xf((float)(iposx - object->collisionWidth) - 0.2f);
	object->fOldX = object->fx;
	
	if(object->velx > 0.0f)
		object->velx = -object->velx;

	return true;
}

void IO_Block::BounceMovingObject(IO_MovingObject * object)
{
	MovingObjectType type = object->getMovingObjectType();
	if(type == movingobject_goomba || type == movingobject_koopa)
	{
		ifsoundonplay(sfx_kicksound);
		killstyle style = kill_style_goomba;
		
		if(type == movingobject_goomba)
		{
			((MO_Goomba*)object)->Die();
			style = kill_style_goomba;
			game_values.enemyhammerkills = 0;
		}
		else if(type == movingobject_koopa)
		{
			((MO_Koopa*)object)->Die();
			style = kill_style_koopa;
			game_values.enemyhammerkills = 0;
		}

		if(!game_values.gamemode->gameover && bumpPlayer)
		{
			AddAwardKill(bumpPlayer, NULL, style);
			bumpPlayer->score->AdjustScore(1);
		}
	}
	else
	{
		object->vely = -VELNOTEBLOCKREPEL;
	}
}

//------------------------------------------------------------------------------
// class powerup block
//------------------------------------------------------------------------------
int g_tanookiFlag = 0;

B_PowerupBlock::B_PowerupBlock(gfxSprite *nspr1, short x, short y, short iNumSpr, short aniSpeed) :
	IO_Block(nspr1, x, y)
{
	iw = (short)spr->getWidth() >> 2;
	ih = (short)spr->getHeight() >> 1;  //This sprite has two images (unused and used blocks)
	collisionWidth = iw;
	timer = 0;
	side = true;
	iNumSprites = iNumSpr;
	animationSpeed = aniSpeed;
	animationTimer = 0;
	animationWidth = (short)spr->getWidth();
	drawFrame = 0;
}


void B_PowerupBlock::draw()
{
	spr->draw(ix, iy, drawFrame, state == 0 ? 0 : ih, iw, ih);
}

void B_PowerupBlock::update()
{
	if(state > 0)
	{
		yf(fy + vely);
		
		if(state == 1 && fabsf(fposy - fy) > 10.0f)
		{
			vely = -vely;
			state = 2;
			bumpPlayer = NULL;
		}
		else if(state == 2 && fabsf(fposy - fy) < VELBLOCKBOUNCE)
		{
			vely = 0.0f;
			state = 3;
			yi(iposy);

			short iSelectedPowerup = SelectPowerup();

            if (-1 == iSelectedPowerup) 
				objectcollisionitems.add(new PU_Tanooki(ix + 1, iy + 1));
            else if(0 == iSelectedPowerup)
				objectcollisionitems.add(new PU_PoisonPowerup(&spr_poisonpowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1));
			else if(1 == iSelectedPowerup)
				objectcollisionitems.add(new PU_ExtraGuyPowerup(&spr_1uppowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1, 1));
			else if(2 == iSelectedPowerup)
				objectcollisionitems.add(new PU_ExtraGuyPowerup(&spr_2uppowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1, 2));
			else if(3 == iSelectedPowerup)
				objectcollisionitems.add(new PU_ExtraGuyPowerup(&spr_3uppowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1, 3));
			else if(4 == iSelectedPowerup)
				objectcollisionitems.add(new PU_ExtraGuyPowerup(&spr_5uppowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1, 5));
			else if(5 == iSelectedPowerup)
				objectcollisionitems.add(new PU_FirePowerup(&spr_firepowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1));
			else if(6 == iSelectedPowerup)
				objectcollisionitems.add(new PU_StarPowerup(&spr_starpowerup, ix + 1, iy - 1, 4, side, 2, 30, 30, 1, 1));
			else if(7 == iSelectedPowerup)
				objectcollisionitems.add(new PU_ClockPowerup(&spr_clockpowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1));
			else if(8 == iSelectedPowerup)
				objectcollisionitems.add(new PU_BobombPowerup(&spr_bobombpowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1));
			else if(9 == iSelectedPowerup)
				objectcollisionitems.add(new PU_PowPowerup(&spr_powpowerup, ix + 1, iy - 1, 8, side, 8, 30, 30, 1, 1));
			else if(10 == iSelectedPowerup)
				objectcollisionitems.add(new PU_BulletBillPowerup(&spr_bulletbillpowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1));
			else if(11 == iSelectedPowerup)
				objectcollisionitems.add(new PU_HammerPowerup(&spr_hammerpowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1));
			else if(12 == iSelectedPowerup)
				objectsplayer.add(new CO_Shell(0, ix + 1, iy - 1, true, true, true, false));
			else if(13 == iSelectedPowerup)
				objectsplayer.add(new CO_Shell(1, ix + 1, iy - 1, false, true, true, false));
			else if(14 == iSelectedPowerup)
				objectsplayer.add(new CO_Shell(2, ix + 1, iy - 1, false, false, true, true));
			else if(15 == iSelectedPowerup)
				objectsplayer.add(new CO_Shell(3, ix + 1, iy - 1, false, true, false, false));
			else if(16 == iSelectedPowerup)
				objectcollisionitems.add(new PU_ModPowerup(&spr_modpowerup, ix + 1, iy - 1, 8, side, 8, 30, 30, 1, 1));
			else if(17 == iSelectedPowerup)
				objectcollisionitems.add(new PU_FeatherPowerup(&spr_featherpowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1));
			else if(18 == iSelectedPowerup)
				objectcollisionitems.add(new PU_MysteryMushroomPowerup(&spr_mysterymushroompowerup, ix + 1, iy - 1, 1, side, 32000, 30, 30, 1, 1));
			else if(19 == iSelectedPowerup)
				objectcollisionitems.add(new PU_BoomerangPowerup(&spr_boomerangpowerup, ix + 1, iy - 1, 1, side, 32000, 30, 26, 1, 5));
			
			ifsoundonplay(sfx_sprout);
		}
		else if(state == 3)
		{
			if(++timer >= game_values.itemrespawntime)
			{
				timer = 0;
				state = 0;
			}
		}
	}

	if(++animationTimer >= animationSpeed)
	{
		animationTimer = 0;

		drawFrame += iw;
		if(drawFrame >= animationWidth)
		{
			drawFrame = 0;
		}
	}
}

bool B_PowerupBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(state == 1)
	{
		if(bumpPlayer && !player->invincible && !player->spawninvincible && (player->teamID != bumpPlayer->teamID || game_values.friendlyfire))
			PlayerKilledPlayer(*bumpPlayer, *player, death_style_jump, kill_style_bounce);
		else
			player->vely = -VELNOTEBLOCKREPEL;
	}
	else if(useBehavior)
	{
		player->vely = GRAVITATION;
	}

	return false;
}

bool B_PowerupBlock::hitbottom(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		//Player bounces off 
		player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
		player->yf((float)(iposy + ih) + 0.2f);

		ifsoundonplay(sfx_bump);

		if(state == 0)
		{
			bumpPlayer = player;
			vely = -VELBLOCKBOUNCE;
			state = 1;
			side = player->ix + HALFPW < ix + (iw >> 1);
		}
	}

	return false;
}

bool B_PowerupBlock::hittop(IO_MovingObject * object)
{
	object->yf((float)(iposy - object->collisionHeight) - 0.2f);
	object->fOldY = object->fy;
	
	if(state == 1 && object->bounce == GRAVITATION)
	{
		BounceMovingObject(object);
		return false;
	}
	else
	{
		object->vely = object->BottomBounce();
	}

	return true;
}

bool B_PowerupBlock::hitright(IO_MovingObject * object)
{
	//Object bounces off
	object->xf((float)(iposx + iw) + 0.2f);
	object->fOldX = object->fx;

	if(object->velx < 0.0f)
		object->velx = -object->velx;

	MovingObjectType type = object->getMovingObjectType();
	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_egg /*|| type == movingobject_star*/)
	{
		if(type == movingobject_shell)
		{
			if(object->state != 1)
				return false;
		}

		ifsoundonplay(sfx_bump);

		if(state == 0)
		{
			bumpPlayer = NULL;
			vely = -VELBLOCKBOUNCE;
			state = 1;
			side = false;
		}

		return true;
	}

	return false;
}

bool B_PowerupBlock::hitleft(IO_MovingObject * object)
{
	//Object bounces off
	object->xf((float)(iposx - object->collisionWidth) - 0.2f);
	object->fOldX = object->fx;

	if(object->velx > 0.0f)
		object->velx = -object->velx;

	MovingObjectType type = object->getMovingObjectType();
	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_egg /*|| type == movingobject_star*/)
	{
		if(type == movingobject_shell)
		{
			if(object->state != 1)
				return false;
		}

		ifsoundonplay(sfx_bump);

		if(state == 0)
		{
			bumpPlayer = NULL;
			vely = -VELBLOCKBOUNCE;
			state = 1;
			side = true;
		}

		return true;
	}

	return false;
}

void B_PowerupBlock::triggerBehavior()
{
	if(state == 0)
	{
		ifsoundonplay(sfx_bump);

		bumpPlayer = NULL;
		vely = -VELBLOCKBOUNCE;
		state = 1;
		side = true;
	}
}

short B_PowerupBlock::SelectPowerup()
{
	if (g_tanookiFlag) 
	{
		g_tanookiFlag--; 
		return -1;
	}

	short iCountWeight = 0;
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		iCountWeight += game_values.powerupweights[iPowerup];

	if(iCountWeight == 0)
		iCountWeight = 1;

	int iRandPowerup = rand() % iCountWeight + 1;
	int iSelectedPowerup = 0;
	int iPowerupWeightCount = game_values.powerupweights[iSelectedPowerup];

	while(iPowerupWeightCount < iRandPowerup)
		iPowerupWeightCount += game_values.powerupweights[++iSelectedPowerup];

	return iSelectedPowerup;
}

//------------------------------------------------------------------------------
// class view powerup block
//------------------------------------------------------------------------------
B_ViewBlock::B_ViewBlock(gfxSprite *nspr1, short x, short y) :
	B_PowerupBlock(nspr1, x, y, 1, 32000)
{
	poweruptimer = 0;
	powerupindex = rand() % NUM_POWERUPS;

	iw = 32;
	ih = 32;

	iCountWeight = 0;
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		iCountWeight += game_values.powerupweights[iPowerup];

	fNoPowerupsSelected = iCountWeight == 0;
	GetNextPowerup();
}


void B_ViewBlock::draw()
{
	//Draw powerup behind block
	if(state == 0 && !fNoPowerupsSelected)
		spr_storedpoweruplarge.draw(ix, iy, powerupindex * 32, 0, 32, 32);

	B_PowerupBlock::draw();
}

void B_ViewBlock::update()
{
	B_PowerupBlock::update();

	if(state == 0 && !fNoPowerupsSelected)
	{
		if(++poweruptimer > game_values.powerupweights[powerupindex] * 10)
		{
			poweruptimer = 0;

			GetNextPowerup();
		}
	}
}

short B_ViewBlock::SelectPowerup()
{
	return powerupindex;
}

void B_ViewBlock::GetNextPowerup()
{
	int iRandPowerup = rand() % iCountWeight + 1;
	powerupindex = 0;
	int iPowerupWeightCount = game_values.powerupweights[powerupindex];

	while(iPowerupWeightCount < iRandPowerup)
		iPowerupWeightCount += game_values.powerupweights[++powerupindex];
}

//------------------------------------------------------------------------------
// class breakable block
//------------------------------------------------------------------------------
B_BreakableBlock::B_BreakableBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed) :
	IO_Block(nspr, x, y)
{
	iw = (short)nspr->getWidth() >> 2;
	iNumSprites = iNumSpr;
	animationSpeed = aniSpeed;
	animationTimer = 0;
	animationWidth = (short)nspr->getWidth();
	drawFrame = 0;
}

void B_BreakableBlock::draw()
{
	if(state == 0)
		spr->draw(ix, iy, drawFrame, 0, iw, ih);
}

void B_BreakableBlock::update()
{
	if(state > 0)
	{
		if(state == 1)
		{
			state = 2;
		}
		else if(state == 2)
		{
			bumpPlayer = NULL;
			dead = true;
			g_map.blockdata[col][row] = NULL;
		}
	}

	if(++animationTimer >= animationSpeed)
	{
		animationTimer = 0;

		drawFrame += iw;
		if(drawFrame >= animationWidth)
		{
			drawFrame = 0;
		}
	}
}


bool B_BreakableBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(state == 1 || state == 2)
	{
		if(bumpPlayer && !player->invincible && !player->spawninvincible && (player->teamID != bumpPlayer->teamID || game_values.friendlyfire))
			PlayerKilledPlayer(*bumpPlayer, *player, death_style_jump, kill_style_bounce);
		else
			player->vely = -VELNOTEBLOCKREPEL;
	}
	else if(useBehavior)
	{
		player->vely = GRAVITATION;
	}

	return false;
}

bool B_BreakableBlock::hitbottom(CPlayer * player, bool useBehavior)
{
	if(useBehavior && state == 0)
	{
		triggerBehavior();
		
		//When breaking a block, you smash through with a small velocity, but this allows for breaking two blocks at once
		/*
		if(player->vely < -VELMAXBREAKBLOCK)
			player->vely = -VELMAXBREAKBLOCK;
		*/
		player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
		player->yf((float)(iposy + ih) + 0.2f);
		
		bumpPlayer = player;
	}

	return false;
}

bool B_BreakableBlock::hittop(IO_MovingObject * object)
{
	object->yf((float)(iposy - object->collisionHeight) - 0.2f);
	object->fOldY = object->fy;
	
	if((state == 1  || state == 2) && object->bounce == GRAVITATION)
	{
		BounceMovingObject(object);
		return false;
	}
	else
	{
		object->vely = object->BottomBounce();
	}

	return true;
}

bool B_BreakableBlock::hitright(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->xf((float)(iposx + iw) + 0.2f);
		object->fOldX = object->fx;

		if(object->velx < 0.0f)
			object->velx = -object->velx;

		MovingObjectType type = object->getMovingObjectType();
		if(type == movingobject_shell || type == movingobject_throwblock /*|| type == movingobject_egg || type == movingobject_star*/)
		{
			if(type == movingobject_shell)
			{
				if(object->state != 1)
					return false;
			}

			triggerBehavior();

			return true;
		}
	}

	return false;
}

bool B_BreakableBlock::hitleft(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->xf((float)(iposx - object->collisionWidth) - 0.2f);
		object->fOldX = object->fx;
		
		if(object->velx > 0.0f)
			object->velx = -object->velx;

		MovingObjectType type = object->getMovingObjectType();
		if(type == movingobject_shell || type == movingobject_throwblock /*|| type == movingobject_egg || type == movingobject_star*/)
		{
			if(type == movingobject_shell)
			{
				if(object->state != 1)
					return false;
			}

			triggerBehavior();
			
			return true;
		}
	}

	return false;
}

void B_BreakableBlock::triggerBehavior()
{
	if(state == 0)
	{
		eyecandyfront.add(new EC_FallingObject(&spr_brokenyellowblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 0, 0));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenyellowblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 0, 0));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenyellowblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 0, 0));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenyellowblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 0, 0));

		state = 1;
		ifsoundonplay(sfx_breakblock);
	}
}

//------------------------------------------------------------------------------
// class note block
//------------------------------------------------------------------------------
B_NoteBlock::B_NoteBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed) :
	IO_Block(nspr, x, y)
{
	iw = (short)nspr->getWidth() >> 2;
	iNumSprites = iNumSpr;
	animationSpeed = aniSpeed;
	animationTimer = 0;
	drawFrame = 0;
	animationWidth = (short)spr->getWidth();
}

void B_NoteBlock::draw()
{
	spr->draw(ix, iy, drawFrame, 0, iw, ih);
}

void B_NoteBlock::update()
{
	if(state > 0)
	{
		xf(fx + velx);
		yf(fy + vely);

		if(state == 1 && fabsf(fposx - fx) > 10.0f)
		{
			velx = -velx;
			state = 2;
		}
		else if(state == 2 && fabsf(fposx - fx) < VELNOTEBLOCKBOUNCE)
		{
			velx = 0.0f;
			state = 0;
			xf(fposx);
			yf(fposy);
		}
		else if(state == 3 && fabsf(fposy - fy) > 10.0f)
		{
			vely = -vely;
			state = 4;
		}
		else if(state == 4 && fabsf(fposy - fy) < VELNOTEBLOCKBOUNCE)
		{
			vely = 0.0f;
			state = 0;
			xf(fposx);
			yf(fposy);
			bumpPlayer = NULL;
		}
	}

	if(++animationTimer >= animationSpeed)
	{
		animationTimer = 0;

		drawFrame += iw;
		if(drawFrame >= animationWidth)
		{
			drawFrame = 0;
		}
	}
}

bool B_NoteBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(useBehavior)
	{
		player->superjumptimer = 4;
		player->vely = -VELNOTEBLOCKREPEL;
	
		if(state == 0)
		{
			vely = VELNOTEBLOCKBOUNCE;
			state = 3;
		}

		ifsoundonplay(sfx_bump);
	}
	
	return false;
}

bool B_NoteBlock::hitbottom(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		player->yf((float)(iposy + ih) + 0.2f);
		player->vely = VELNOTEBLOCKREPEL;

		if(state == 0)
		{
			bumpPlayer = player;
			vely = -VELNOTEBLOCKBOUNCE;
			state = 3;
		}

		ifsoundonplay(sfx_bump);
	}

	return false;
}

bool B_NoteBlock::hitright(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		player->xf((float)(iposx + iw) + 0.2f);
		player->fOldX = player->fx;
		player->velx = VELNOTEBLOCKREPEL;

		if(state == 0)
		{
			velx = -VELNOTEBLOCKBOUNCE;
			state = 1;
		}

		ifsoundonplay(sfx_bump);
	}

	return false;
}

bool B_NoteBlock::hitleft(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		player->xf((float)(iposx - PW) - 0.2f);
		player->fOldX = player->fx;
		player->velx = -VELNOTEBLOCKREPEL;

		if(state == 0)
		{
			velx = VELNOTEBLOCKBOUNCE;
			state = 1;
		}

		ifsoundonplay(sfx_bump);
	}

	return false;
}

bool B_NoteBlock::hittop(IO_MovingObject * object)
{
	object->yf((float)(iposy - object->collisionHeight) - 0.2f);
	object->fOldY = object->fy;
	
	if(state == 3 && object->bounce == GRAVITATION)
	{
		BounceMovingObject(object);
		return false;
	}
	else
	{
		object->vely = object->BottomBounce();
	}

	return true;
}

//------------------------------------------------------------------------------
// class donut block
//------------------------------------------------------------------------------
B_DonutBlock::B_DonutBlock(gfxSprite *nspr, short x, short y) :
	IO_Block(nspr, x, y)
{
	counter = 0;
	jigglex = 0;
	jigglecounter = 0;
}


void B_DonutBlock::draw()
{
	spr->draw(ix + jigglex, iy);
}


void B_DonutBlock::update()
{
	//If a player is standing on us, jiggle and then fall
	if(state == 0)
	{
		counter = 0;
		jigglex = 0;
		jigglecounter = 0;
	}
	else
	{
		if(++jigglecounter > 1)
		{
			jigglecounter = 0;

			if(jigglex == 2)
				jigglex = -2;
			else
				jigglex = 2;
		}

		if(++counter > 50)
		{
			triggerBehavior();
		}
	}

	state = 0;
}

bool B_DonutBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(useBehavior)
		state = 1;
	
	return false;
}

void B_DonutBlock::triggerBehavior()
{
	eyecandyfront.add(new EC_FallingObject(&spr_donutblock, ix, iy, 0.0f, 0, 0, 0, 0));
	dead = true;
	g_map.blockdata[col][row] = NULL;
}

//------------------------------------------------------------------------------
// class flip block
// Known Bug:  If you have two flip blocks vertically aligned like this:
//
// ['']  or  ['']['']  etc.
// ['']      ['']['']
//
// And you hit both of them from the bottom, then keep jumping so that the bottom one
// stops to support you, and then the top one stops spinning, you can "hide" under the
// top block.  I was thinking about fixing this, but I don't know what the correct 
// behavior should be if you're in a flip block when it stops spinning.  Plus, it is kind
// of cool to hide under these blocks.  You are somewhat invincible, but someone can come 
// along and hit your support out from under you and you'll fall out and if they have star
// power, you're dead.  Plus fireballs have a large enough collision box that you can be hit
// with them too.
//------------------------------------------------------------------------------
B_FlipBlock::B_FlipBlock(gfxSprite *nspr, short x, short y) :
	IO_Block(nspr, x, y)
{
	iw = (short)spr->getWidth() >> 2;
	collisionWidth = iw;

	counter = 0;
	frame = 0;
	timer = 0;
	animationWidth = (short)spr->getWidth();
}

void B_FlipBlock::draw()
{
	spr->draw(ix, iy, frame, 0, iw, ih);
}

void B_FlipBlock::update()
{
	if(state == 1)
	{
		if(++counter >= 10)
		{
			counter = 0;
			frame += iw;

			if(frame >= animationWidth)
			{
				frame = 0;
			}
		}

		if(++timer >= 240)
		{
			frame = 0;
			counter = 0;
			timer = 0;
			state = 0;
		}
	}
}

bool B_FlipBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
	if(player->fOldY + PH <= iposy && direction == 2)
		return hittop(player, useBehavior);
	else if(player->fOldY >= iposy + ih && direction == 0)
		return hitbottom(player, useBehavior);
	else if(player->fOldX + PW <= iposx && direction == 1)
		return hitleft(player, useBehavior);
	else if(player->fOldX >= iposx + iw && direction == 3)
		return hitright(player, useBehavior);

	return true;
}

bool B_FlipBlock::hittop(CPlayer * player, bool useBehavior)
{
	if(useBehavior && state == 0)
	{
		player->yf((float)(iposy - PH) - 0.2f);
		player->inair = false;
		player->fallthrough = false;
		player->killsinrowinair = 0;
		player->featherjump = 0;
		player->vely = GRAVITATION;
		return false;
	}

	return true;
}

bool B_FlipBlock::hitbottom(CPlayer * player, bool useBehavior)
{
	if(useBehavior && state == 0)
	{
		player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
		player->yf((float)(iposy + ih) + 0.2f);
		
		triggerBehavior();
		return false;
	}

	return true;
}

bool B_FlipBlock::hitright(CPlayer * player, bool useBehavior)
{
	if(useBehavior && state == 0)
	{
		player->xf((float)(iposx + iw) + 0.2f);
		player->fOldX = player->fx;

		if(player->velx < 0.0f)
			player->velx = 0.0f;

		return false;
	}
	
	return true;
}

bool B_FlipBlock::hitleft(CPlayer * player, bool useBehavior)
{
	if(useBehavior && state == 0)
	{
		player->xf((float)(iposx - PW) - 0.2f);
		player->fOldX = player->fx;

		if(player->velx > 0.0f)
			player->velx = 0.0f;

		return false;
	}

	return true;
}

bool B_FlipBlock::hittop(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->yf((float)(iposy - object->collisionHeight) - 0.2f);
		object->fOldY = object->fy;
		object->vely = object->BottomBounce();
	}

	return true;
}

bool B_FlipBlock::hitbottom(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->yf((float)(iposy + ih) + 0.2f);
		object->vely = -object->vely;
	}
	
	return true;
}

bool B_FlipBlock::hitright(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->xf((float)(iposx + iw) + 0.2f);
		object->fOldX = object->fx;

		if(object->velx < 0.0f)
			object->velx = -object->velx;

		MovingObjectType type = object->getMovingObjectType();
		if(type == movingobject_shell || type == movingobject_throwblock)
		{
			if(type == movingobject_shell)
			{
				if(object->state != 1)
					return false;
			}

			eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 0, 0));
			eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 0, 0));
			eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 0, 0));
			eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 0, 0));

			ifsoundonplay(sfx_breakblock);

			dead = true;
			g_map.blockdata[col][row] = NULL;
		}
	}

	return true;
}

bool B_FlipBlock::hitleft(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->xf((float)(iposx - object->collisionWidth) - 0.2f);
		object->fOldX = object->fx;
		
		if(object->velx > 0.0f)
			object->velx = -object->velx;

		MovingObjectType type = object->getMovingObjectType();
		if(type == movingobject_shell || type == movingobject_throwblock)
		{
			if(type == movingobject_shell)
			{
				if(object->state != 1)
					return false;
			}

			eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 0, 0));
			eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 0, 0));
			eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 0, 0));
			eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 0, 0));
			
			ifsoundonplay(sfx_breakblock);

			dead = true;
			g_map.blockdata[col][row] = NULL;
		}
	}

	return true;
}

void B_FlipBlock::triggerBehavior()
{
	if(state == 0)
	{
		state = 1;
		frame = iw;
	}
}

//------------------------------------------------------------------------------
// class On/Off Switch block
//------------------------------------------------------------------------------
B_OnOffSwitchBlock::B_OnOffSwitchBlock(gfxSprite *nspr, short x, short y, short colorID, short iState) :
	IO_Block(nspr, x, y)
{
	iw = (short)spr->getWidth() >> 2;
	collisionWidth = iw;
	ih = (short)spr->getHeight() >> 2;
	collisionHeight = ih;

	iColorID = colorID;
	iSrcX = colorID * 32;

	state = (iState == 0 ? 0 : 3);
}

void B_OnOffSwitchBlock::update()
{
	if(state != 0 && state != 3)
	{
		yf(fy + vely);

		if((state == 1 || state == 4) && fabsf(fposy - fy) > 10.0f)
		{
			bumpPlayer = NULL;
			vely = -vely;
			state++;
		}
		else if((state == 2 || state == 5) && fabsf(fposy - fy) < VELBLOCKBOUNCE)
		{
			vely = 0.0f;
			state -= 2;
			yf(fposy);
		}
	}
}

void B_OnOffSwitchBlock::draw()
{
	spr->draw(ix, iy, iSrcX, (state == 0 ? 0 : 32), iw, ih);
}

bool B_OnOffSwitchBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(state == 1 || state == 4)
	{
		if(bumpPlayer && !player->invincible && !player->spawninvincible && (player->teamID != bumpPlayer->teamID || game_values.friendlyfire))
			PlayerKilledPlayer(*bumpPlayer, *player, death_style_jump, kill_style_bounce);
		else
			player->vely = -VELNOTEBLOCKREPEL;
	}
	else if(useBehavior)
	{
		player->vely = GRAVITATION;
	}

	return false;
}

bool B_OnOffSwitchBlock::hitbottom(CPlayer * player, bool useBehavior)
{
	//Player bounces off 
	if(useBehavior)
	{
		player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
		player->yf((float)(iposy + ih) + 0.2f);

		if(state == 0 || state == 3)
		{
			bumpPlayer = player;
			
			triggerBehavior();
		}
		else
		{
			ifsoundonplay(sfx_bump);
		}
	}

	return false;
}

bool B_OnOffSwitchBlock::hittop(IO_MovingObject * object)
{
	object->yf((float)(iposy - object->collisionHeight) - 0.2f);
	object->fOldY = object->fy;
	
	if((state == 1 || state == 4) && object->bounce == GRAVITATION)
	{
		BounceMovingObject(object);
		return false;
	}
	else
	{
		object->vely = object->BottomBounce();
	}

	return true;
}

bool B_OnOffSwitchBlock::hitright(IO_MovingObject * object)
{
	//Object bounces off
	object->xf((float)(iposx + iw) + 0.2f);
	object->fOldX = object->fx;

	if(object->velx < 0.0f)
		object->velx = -object->velx;

	MovingObjectType type = object->getMovingObjectType();
	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_egg /*|| type == movingobject_star*/)
	{
		if(type == movingobject_shell)
		{
			if(object->state != 1)
				return false;
		}

		if(state == 0 || state == 3)
		{
			bumpPlayer = NULL;
			triggerBehavior();
		}
		else
		{
			ifsoundonplay(sfx_bump);
		}

		return true;
	}

	return false;
}

bool B_OnOffSwitchBlock::hitleft(IO_MovingObject * object)
{
	//Object bounces off
	object->xf((float)(iposx - object->collisionWidth) - 0.2f);
	object->fOldX = object->fx;

	if(object->velx > 0.0f)
		object->velx = -object->velx;

	MovingObjectType type = object->getMovingObjectType();
	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_egg /*|| type == movingobject_star*/)
	{
		if(type == movingobject_shell)
		{
			if(object->state != 1)
				return false;
		}

		if(state == 0 || state == 3)
		{
			bumpPlayer = NULL;
			triggerBehavior();
		}
		else
		{
			ifsoundonplay(sfx_bump);
		}

		return true;
	}

	return false;
}

void B_OnOffSwitchBlock::triggerBehavior()
{
	if(state == 0 || state == 3)
	{
		ifsoundonplay(sfx_switchpress);
		vely = -VELBLOCKBOUNCE;
		
		state++;
		
		//Switch all the switch blocks and all the on/off blocks of the same color
		std::list<IO_Block*>::iterator iterateSwitches = g_map.switchBlocks[iColorID].begin();

		while (iterateSwitches != g_map.switchBlocks[iColorID].end())
		{
			((B_OnOffSwitchBlock*)(*iterateSwitches))->FlipState();
			iterateSwitches++;
		}

		//Switch all the switch blocks
		iterateSwitches = g_map.switchBlocks[iColorID + 4].begin();

		while (iterateSwitches != g_map.switchBlocks[iColorID + 4].end())
		{
			((B_SwitchBlock*)(*iterateSwitches))->FlipState();
			iterateSwitches++;
		}
	}
}

//------------------------------------------------------------------------------
// class switch block
//------------------------------------------------------------------------------

B_SwitchBlock::B_SwitchBlock(gfxSprite *nspr, short x, short y, short colorID, short iState) :
	IO_Block(nspr, x, y)
{
	iw = (short)spr->getWidth() >> 2;
	collisionWidth = iw;
	ih = (short)spr->getHeight() >> 2;
	collisionHeight = ih;

	state = iState;
	iSrcX = colorID * 32;
}


void B_SwitchBlock::draw()
{
	spr->draw(ix, iy, iSrcX, (state == 0 ? 64 : 96), iw, ih);
}

bool B_SwitchBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
	if(state != 0 || !useBehavior)
		return true;

	if(player->fOldY + PH <= iposy && direction == 2)
		return hittop(player, useBehavior);
	else if(player->fOldY >= iposy + ih && direction == 0)
		return hitbottom(player, useBehavior);
	else if(player->fOldX + PW <= iposx && direction == 1)
		return hitleft(player, useBehavior);
	else if(player->fOldX >= iposx + iw && direction == 3)
		return hitright(player, useBehavior);

	return true;
}

bool B_SwitchBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);
	return false;
}

bool B_SwitchBlock::hitbottom(CPlayer * player, bool)
{
	player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
	player->yf((float)(iposy + ih) + 0.2f);
	vely = -VELBLOCKBOUNCE;
	return false;
}

bool B_SwitchBlock::hitright(CPlayer * player, bool)
{
	player->xf((float)(iposx + iw) + 0.2f);
	player->fOldX = player->fx;

	if(player->velx < 0.0f)
		player->velx = 0.0f;

	return false;
}

bool B_SwitchBlock::hitleft(CPlayer * player, bool)
{
	player->xf((float)(iposx - PW) - 0.2f);
	player->fOldX = player->fx;

	if(player->velx > 0.0f)
		player->velx = 0.0f;

	return false;
}

bool B_SwitchBlock::hittop(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->yf((float)(iposy - object->collisionHeight) - 0.2f);
		object->fOldY = object->fy;
		object->vely = object->BottomBounce();
	}

	return true;
}

bool B_SwitchBlock::hitbottom(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->yf((float)(iposy + ih) + 0.2f);
		object->vely = -object->vely;
	}
	
	return true;
}

bool B_SwitchBlock::hitright(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->xf((float)(iposx + iw) + 0.2f);

		if(object->velx < 0.0f)
			object->velx = -object->velx;
	}

	return true;
}

bool B_SwitchBlock::hitleft(IO_MovingObject * object)
{
	if(state == 0)
	{
		object->xf((float)(iposx - object->collisionWidth) - 0.2f);
		
		if(object->velx > 0.0f)
			object->velx = -object->velx;
	}

	return true;
}

//------------------------------------------------------------------------------
// class bounce block
//------------------------------------------------------------------------------
B_BounceBlock::B_BounceBlock(gfxSprite *nspr1, short x, short y) :
	IO_Block(nspr1, x, y)
{}

void B_BounceBlock::update()
{
	if(state > 0)
	{
		yf(fy + vely);

		if(state == 1 && fabsf(fposy - fy) > 10.0f)
		{
			bumpPlayer = NULL;
			vely = -vely;
			state = 2;
		}
		else if(state == 2 && fabsf(fposy - fy) < VELBLOCKBOUNCE)
		{
			vely = 0.0f;
			state = 0;
			yf(fposy);
		}
	}
}

bool B_BounceBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(state == 1)
	{
		if(bumpPlayer && !player->invincible && !player->spawninvincible && (player->teamID != bumpPlayer->teamID || game_values.friendlyfire))
			PlayerKilledPlayer(*bumpPlayer, *player, death_style_jump, kill_style_bounce);
		else
			player->vely = -VELNOTEBLOCKREPEL;
	}
	else if(useBehavior)
	{
		player->vely = GRAVITATION;
	}

	return false;
}

bool B_BounceBlock::hitbottom(CPlayer * player, bool useBehavior)
{
	//Player bounces off 
	if(useBehavior)
	{
		player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
		player->yf((float)(iposy + ih) + 0.2f);

		ifsoundonplay(sfx_bump);
		bumpPlayer = player;

		triggerBehavior();
	}

	return false;
}

bool B_BounceBlock::hittop(IO_MovingObject * object)
{
	object->yf((float)(iposy - object->collisionHeight) - 0.2f);
	object->fOldY = object->fy;
	
	if(state == 1 && object->bounce == GRAVITATION)
	{
		BounceMovingObject(object);
		return false;
	}
	else
	{
		object->vely = object->BottomBounce();
	}

	return true;
}

void B_BounceBlock::triggerBehavior()
{
	if(state == 0)
	{
		vely = -VELBLOCKBOUNCE;
		state = 1;
	}
}

//------------------------------------------------------------------------------
// class throw block
//------------------------------------------------------------------------------
B_ThrowBlock::B_ThrowBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed) :
	IO_Block(nspr, x, y)
{
	iw = nspr->getWidth() >> 2;
	ih = nspr->getHeight() >> 1;
	iNumSprites = iNumSpr;
	animationSpeed = aniSpeed;
	animationTimer = 0;
	drawFrame = 0;
	animationWidth = (short)nspr->getWidth();
	fSuper = false;
}

void B_ThrowBlock::draw()
{
	spr->draw(ix, iy, drawFrame, fSuper ? 32 : 0, iw, ih);
}

void B_ThrowBlock::update()
{
	if(++animationTimer >= animationSpeed)
	{
		animationTimer = 0;

		drawFrame += iw;
		if(drawFrame >= animationWidth)
		{
			drawFrame = 0;
		}
	}
}

bool B_ThrowBlock::hittop(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		if(player->PressedAcceptItemKey() && player->IsAcceptingItem())
		{
			GiveBlockToPlayer(player);
			return true;
		}
		else
		{
			player->yf((float)(iposy - PH) - 0.2f);
			player->fOldY = player->fy;
			player->inair = false;
			player->fallthrough = false;
			player->killsinrowinair = 0;
			player->featherjump = 0;
			player->vely = GRAVITATION;
			return false;
		}
	}

	return false;
}

bool B_ThrowBlock::hitright(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		if(player->IsAcceptingItem())
		{
			GiveBlockToPlayer(player);
			return true;
		}
		else
		{
			player->xf((float)(iposx + iw) + 0.2f);
			player->fOldX = player->fx;

			if(player->velx < 0.0f)
				player->velx = 0.0f;

			return false;
		}
	}
	
	return true;
}

bool B_ThrowBlock::hitleft(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		if(player->IsAcceptingItem())
		{
			GiveBlockToPlayer(player);
			return true;
		}
		else
		{
			player->xf((float)(iposx - PW) - 0.2f);
			player->fOldX = player->fx;

			if(player->velx > 0.0f)
				player->velx = 0.0f;

			return false;
		}
	}
	
	return true;
}

void B_ThrowBlock::GiveBlockToPlayer(CPlayer * player)
{
	dead = true;
	g_map.blockdata[col][row] = NULL;

	CO_ThrowBlock * block = new CO_ThrowBlock(&spr_blueblock, ix, iy, fSuper);
	if(player->AcceptItem(block))
	{
		block->owner = player;
		block->playerID = player->globalID;
		objectsplayer.add(block);
	}
	else
		delete block;
}

void B_ThrowBlock::triggerBehavior()
{
	dead = true;
	g_map.blockdata[col][row] = NULL;

	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy, -1.5f, -7.0f, 6, 2, 0, fSuper ? 16 : 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy, 1.5f, -7.0f, 6, 2, 0, fSuper ? 16 : 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy + 16, -1.5f, -4.0f, 6, 2, 0, fSuper ? 16 : 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy + 16, 1.5f, -4.0f, 6, 2, 0, fSuper ? 16 : 0, 16, 16));
	
	ifsoundonplay(sfx_breakblock);
}

void B_ThrowBlock::SetType(bool superblock)
{
	fSuper = superblock;
}

//------------------------------------------------------------------------------
// class MovingObject (all moving objects inheirit from this class)
//------------------------------------------------------------------------------
IO_MovingObject::IO_MovingObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
	CObject(nspr, x, y)
{
	iNumSprites = iNumSpr;

	if(iAnimationWidth > -1)
		iw = iAnimationWidth;
	else
		iw = (short)spr->getWidth() / iNumSprites;

	if(iAnimationHeight > -1)
		ih = iAnimationHeight;

	animationtimer = 0;
	animationWidth = (short)spr->getWidth();

	fOldX = fx;
	fOldY = fy - ih;

	animationspeed = aniSpeed;
	
	objectType = object_moving;
	movingObjectType = movingobject_none;

	if(iCollisionWidth > -1)
	{
		collisionWidth = iCollisionWidth;
		collisionHeight = iCollisionHeight;
		collisionOffsetX = iCollisionOffsetX;
		collisionOffsetY = iCollisionOffsetY;
	}
	else
	{
		collisionWidth = iw;
		collisionHeight = ih;
		collisionOffsetX = 0;
		collisionOffsetY = 0;
	}

	if(iAnimationOffsetX > -1)
	{
		animationOffsetX = iAnimationOffsetX;
		animationOffsetY = iAnimationOffsetY;
	}
	else
	{
		animationOffsetX = 0;
		animationOffsetY = 0;
	}

	drawframe = animationOffsetX;

	inair = false;
	onice = false;

	platform = NULL;
	iHorizontalPlatformCollision = -1;
	iVerticalPlatformCollision = -1;
}


void IO_MovingObject::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}


void IO_MovingObject::update()
{
	fOldX = fx;
	fOldY = fy;

	collision_detection_map();
	
	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = animationOffsetX;
	}
}

bool IO_MovingObject::collide(CPlayer *)
{
	dead = true;
	return false;
}

void IO_MovingObject::collision_detection_map()
{
	xf(fx + velx);
	flipsidesifneeded();

	fPrecalculatedY = fy + vely;  //Fixes weird float rounding error.  Must be computed here before casting to int.  Otherwise, this will miss the bottom collision, but then hit the side collision and the player can slide out of 1x1 spaces.

	float fPlatformVelX = 0.0f;
	float fPlatformVelY = 0.0f;

	float fTempY = fy;

	if(platform)
	{
		if(!onice)
		{
			fPlatformVelX = platform->fVelX;
			xf(fx + fPlatformVelX);
			flipsidesifneeded();
		}
		
		fPlatformVelY = platform->fVelY;

		if(platform->fOldVelY < 0.0f)
			fy += platform->fOldVelY;
		
		fPrecalculatedY += platform->fOldVelY;
	}

	iHorizontalPlatformCollision = -1;
	iVerticalPlatformCollision = -1;

	g_map.movingPlatformCollision(this);

	fy = fTempY;

	if(fPrecalculatedY + collisionHeight < 0.0f)
	{				// on top outside of the screen
		yf(fPrecalculatedY);
		vely = CapFallingVelocity(GRAVITATION + vely);
		
		if(!platform)
		{
			inair = true;
			onice = false;
		}

		return;
	}
	else if(fPrecalculatedY + collisionHeight >= 480.0f)
	{	//on ground outside of the screen?
		yi(-collisionHeight);
		fOldY = fy - 1.0f;
		onice = false;
		return;
	}

	//Could be optimized with bit shift >> 5
	short ty = (short)fy / TILESIZE;
	short ty2 = ((short)fy + collisionHeight) / TILESIZE;
	short tx = -1;
	

	//-----------------------------------------------------------------
	//  x axis first (--)
	//-----------------------------------------------------------------
	if(fy + collisionHeight >= 0.0f)
	{
		if(velx + fPlatformVelX > 0.01f || iHorizontalPlatformCollision == 3)
		{		//moving right
			if(fx + collisionWidth >= 640.0f)
			{
				tx = (short)(fx + collisionWidth - 640.0f) / TILESIZE;
				fOldX -= 640.0f;
			}
			else
                tx = ((short)fx + collisionWidth) / TILESIZE;

			IO_Block * topblock = g_map.block(tx, ty);
			IO_Block * bottomblock = g_map.block(tx, ty2);

			if((topblock && !topblock->isTransparent()) || 
				(bottomblock && !bottomblock->isTransparent()))
			{
				bool processOtherBlock = true;
				if(topblock && !topblock->isTransparent()) //collide with top block
				{	
					if(iHorizontalPlatformCollision == 3)
					{
						KillObjectMapHazard();
						return;
					}

					topblock->collide(this, 1);
					flipsidesifneeded();
					removeifprojectile(this, true, true);

					processOtherBlock = false;

					SideBounce();
				}
				
				if(processOtherBlock && bottomblock && !bottomblock->isTransparent()) //then bottom
				{	
					if(iHorizontalPlatformCollision == 3)
					{
						KillObjectMapHazard();
						return;
					}

					bottomblock->collide(this, 1);
					flipsidesifneeded();
					removeifprojectile(this, true, true);

					SideBounce();
				}
			}
			else if((g_map.map(tx, ty) & 0x05) > 0 || (g_map.map(tx, ty2) & 0x05) > 0)
			{	//collision on the right side.

				if(iHorizontalPlatformCollision == 3)
				{
					KillObjectMapHazard();
					return;
				}

				xf((float)(tx * TILESIZE - collisionWidth) - 0.2f); //move to the edge of the tile (tile on the right -> mind the object width)
				fOldX = fx;
				
				if(velx > 0.0f)
					velx = -velx;

				flipsidesifneeded();
				removeifprojectile(this, true, true);

				SideBounce();
			}
		}
		else if(velx + fPlatformVelX < -0.01f || iHorizontalPlatformCollision == 1)
		{	//moving left
			tx = (short)fx / TILESIZE;

			//Just in case fx < 0 and wasn't caught by flipsidesifneeded()
			if(tx < 0)
				tx = 0;

			IO_Block * topblock = g_map.block(tx, ty);
			IO_Block * bottomblock = g_map.block(tx, ty2);

			if((topblock && !topblock->isTransparent()) || 
				(bottomblock && !bottomblock->isTransparent()))
			{
				bool processOtherBlock = true;
				if(topblock && !topblock->isTransparent()) //collide with top block
				{	
					if(iHorizontalPlatformCollision == 1)
					{
						KillObjectMapHazard();
						return;
					}

					topblock->collide(this, 3);
					flipsidesifneeded();
					removeifprojectile(this, true, true);

					processOtherBlock = false;

					SideBounce();
				}
				
				if(processOtherBlock && bottomblock && !bottomblock->isTransparent()) //then bottom
				{	
					if(iHorizontalPlatformCollision == 1)
					{
						KillObjectMapHazard();
						return;
					}

					bottomblock->collide(this, 3);
					flipsidesifneeded();
					removeifprojectile(this, true, true);

					SideBounce();
				}
			}
			else if((g_map.map(tx, ty) & 0x05) > 0 || (g_map.map(tx, ty2) & 0x05) > 0)
			{
				if(iHorizontalPlatformCollision == 1)
				{
					KillObjectMapHazard();
					return;
				}

				xf((float)(tx * TILESIZE + TILESIZE) + 0.2f);			//move to the edge of the tile
				fOldX = fx;

				if(velx < 0.0f)
					velx = -velx;

				flipsidesifneeded();
				removeifprojectile(this, true, true);

				SideBounce();
			}
		}
	}

	short txl = -1, txr = -1;

	txl = ix / TILESIZE;

	if(ix + collisionWidth >= 640)
        txr = (ix + collisionWidth - 640) / TILESIZE;
	else
		txr = (ix + collisionWidth) / TILESIZE;

	//-----------------------------------------------------------------
	//  then y axis (|)
	//-----------------------------------------------------------------

	float fMovingUp = vely;
	if(platform)
		fMovingUp = vely + fPlatformVelY - bounce;

	if(fMovingUp < -0.01f)
	{
		ty = (short)(fPrecalculatedY) / TILESIZE;

		IO_Block * leftblock = g_map.block(txl, ty);
		IO_Block * rightblock = g_map.block(txr, ty);
		
		if(leftblock && !leftblock->isTransparent()) //then left
		{	
			if(iVerticalPlatformCollision == 2)
				KillObjectMapHazard();

			leftblock->collide(this, 0);
			return;
		}
		
		if(rightblock && !rightblock->isTransparent()) //then right
		{	
			if(iVerticalPlatformCollision == 2)
				KillObjectMapHazard();

			rightblock->collide(this, 0);
			return;
		}

		if((g_map.map(txl, ty) & 0x05) > 0 || (g_map.map(txr, ty) & 0x05) > 0)
		{
			if(iVerticalPlatformCollision == 2)
				KillObjectMapHazard();

			yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			fOldY = fy - 1.0f;
			
			if(vely < 0.0f)
				vely = -vely;	
		}
		else
		{
			yf(fPrecalculatedY);
			vely += GRAVITATION;
		}

		if(!platform)
		{
			inair = true;
        	onice = false;
		}
	}
	else
	{	//moving down / on ground
		ty = ((short)fPrecalculatedY + collisionHeight) / TILESIZE;

		IO_Block * leftblock = g_map.block(txl, ty);
		IO_Block * rightblock = g_map.block(txr, ty);

		if((leftblock && !leftblock->isTransparent()) || 
			(rightblock && !rightblock->isTransparent()))
		{
			bool processOtherBlock = true;
			if(leftblock && !leftblock->isTransparent()) //collide with left block
			{	
				processOtherBlock = leftblock->collide(this, 2);

				if(!platform)
				{
					inair = false;
					onice = false;
				}
			}
			
			if(processOtherBlock && rightblock && !rightblock->isTransparent()) //then right
			{	
				rightblock->collide(this, 2);
				
				if(!platform)
				{
					inair = false;
					onice = false;
				}
			}
			
			if(iVerticalPlatformCollision == 0)
				KillObjectMapHazard();

			return;
		}

		TileType leftTile = g_map.map(txl, ty);
		TileType rightTile = g_map.map(txr, ty);

		if(leftTile == tile_solid_on_top || rightTile == tile_solid_on_top)
		{
			if((fOldY + collisionHeight) / TILESIZE < ty)
			{
				vely = BottomBounce();
				yf((float)(ty * TILESIZE - collisionHeight) - 0.2f);
				fOldY = fy - GRAVITATION;
				
				if(!platform)
				{
					inair = false;
					onice = false;
				}

				platform = NULL;

				if(iVerticalPlatformCollision == 0)
					KillObjectMapHazard();

				return;
			}
		}

		if((leftTile & 0x05) > 0 || (rightTile & 0x05) > 0)
		{	
			vely = BottomBounce();
			yf((float)(ty * TILESIZE - collisionHeight) - 0.2f);
			fOldY = fy;

			if(!platform)
			{
				inair = false;

				if((leftTile == tile_ice && (rightTile == tile_ice || rightTile == tile_nonsolid)) ||
					(rightTile == tile_ice && (leftTile == tile_ice || leftTile == tile_nonsolid)))
					onice = true;
				else 
					onice = false;
			}
			
			platform = NULL;

			if(iVerticalPlatformCollision == 0)
				KillObjectMapHazard();
		}
		else
		{
			yf(fPrecalculatedY);
			vely = CapFallingVelocity(GRAVITATION + vely);
			
			if(!platform)
				inair = true;
		}
	}

	if(!platform && inair)
		onice = false;
}

//This method checks the object against the map and moves it outside of any tiles or blocks it might be inside of
bool IO_MovingObject::collision_detection_checksides()
{
	//First figure out where the corners of this object are touching
	Uint8 iCase = 0;

	short txl = ix / TILESIZE;

	short txr = -1;
	if(ix + collisionWidth >= 640)
		txr = (ix + collisionWidth - 640) / TILESIZE;
	else
		txr = (ix + collisionWidth) / TILESIZE;

	short ty = iy / TILESIZE;
	short ty2 = (iy + collisionHeight) / TILESIZE;

	if(iy >= 0)
	{
		if(ty < MAPHEIGHT)
		{
			if(txl >= 0 && txl < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txl, ty);

				if((block && !block->isTransparent()) || (g_map.map(txl, ty) & 0x5) > 0)
				{
					iCase |= 0x01;
				}
			}

			if(txr >= 0 && txr < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txr, ty);

				if((block && !block->isTransparent()) || (g_map.map(txr, ty) & 0x5) > 0)
				{
					iCase |= 0x02;
				}
			}
		}

	}

	if(iy + collisionHeight >= 0.0f)
	{
		if(ty2 < MAPHEIGHT)
		{
			if(txl >= 0 && txl < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txl, ty2);

				if((block && !block->isTransparent()) || (g_map.map(txl, ty2) & 0x5) > 0)
				{
					iCase |= 0x04;
				}
			}

			if(txr >= 0 && txr < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txr, ty2);

				if((block && !block->isTransparent()) || (g_map.map(txr, ty2) & 0x5) > 0)
				{
					iCase |= 0x08;
				}
			}
		}
	}

	//Then determine which way is the best way to move this object out of the solid map areas
	switch(iCase)
	{
		//Do nothing
		//[ ][ ]
		//[ ][ ]
		case 0:
			return false;
			break;
		
		//[X][ ]
		//[ ][ ]
		case 1:
		{
			if(ix + (collisionWidth >> 1) > txl * TILESIZE + TILESIZE)
			{
				xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			}

			break;
		}

		//[ ][X]
		//[ ][ ]
		case 2:
		{
			if(ix + (collisionWidth >> 1) < txr * TILESIZE)
			{
				xf((float)(txr * TILESIZE - collisionWidth) - 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			}

			break;
		}

		//[X][X]
		//[ ][ ]
		case 3:
		{
			yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			break;
		}

		//[ ][ ]
		//[X][ ]
		case 4:
		{
			if(ix + (collisionWidth >> 1) > txl * TILESIZE + TILESIZE)
			{
				xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty2 * TILESIZE - collisionHeight) - 0.2f);
			}

			break;
		}

		//[X][ ]
		//[X][ ]
		case 5:
		{
			xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][X]
		//[X][ ]
		case 6:
		{
			if(ix + (collisionWidth >> 1) > txl * TILESIZE + TILESIZE)
			{
				yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
				xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty2 * TILESIZE - collisionHeight) - 0.2f);
				xf((float)(txr * TILESIZE - collisionWidth) - 0.2f);
				flipsidesifneeded();
			}

			break;
		}

		//[X][X]
		//[X][ ]
		case 7:
		{
			yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][ ]
		//[ ][X]
		case 8:
		{
			if(ix + (collisionWidth >> 1) < txr * TILESIZE)
			{
				xf((float)(txr * TILESIZE - collisionWidth) - 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty2 * TILESIZE - collisionHeight) - 0.2f);
			}

			break;
		}

		//[X][ ]
		//[ ][X]
		case 9:
		{
			if(ix + (collisionWidth >> 1) > txl * TILESIZE + TILESIZE)
			{
				yf((float)(ty2 * TILESIZE - collisionHeight) - 0.2f);
				xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
				xf((float)(txr * TILESIZE - collisionWidth) - 0.2f);
				flipsidesifneeded();
			}

			break;
		}

		//[ ][X]
		//[ ][X]
		case 10:
		{
			xf((float)(txr * TILESIZE - collisionWidth) - 0.2f);
			flipsidesifneeded();
			break;
		}

		//[X][X]
		//[ ][X]
		case 11:
		{
			yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			xf((float)(txr * TILESIZE - collisionWidth) - 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][ ]
		//[X][X]
		case 12:
		{
			yf((float)(ty2 * TILESIZE - collisionHeight) - 0.2f);
			break;
		}

		//[X][ ]
		//[X][X]
		case 13:
		{
			yf((float)(ty2 * TILESIZE - collisionHeight) - 0.2f);
			xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][X]
		//[X][X]
		case 14:
		{
			yf((float)(ty2 * TILESIZE - collisionHeight) - 0.2f);
			xf((float)(txr * TILESIZE - collisionWidth) - 0.2f);
			flipsidesifneeded();
			break;
		}

		//If object is completely inside a block, default to moving it down
		//[X][X]
		//[X][X]
		case 15:
		{
			yf((float)(ty2 * TILESIZE + TILESIZE) + 0.2f);
			break;
		}

		default:
			break;
	}

	return true;
}

void IO_MovingObject::flipsidesifneeded()
{
	//Use ix here to avoid rounding issues (can crash if txr evals to over the right side of screen)
	if(ix < 0 || fx < 0.0f)
	{
		xf(fx + 640.0f);
		fOldX += 640.0f;
	}
	else if(ix >= 640 || fx >= 640.0f)
	{
		xf(fx - 640.0f);
		fOldX -= 640.0f;
	}
}

void IO_MovingObject::KillObjectMapHazard()
{
	if(!dead)
	{
		dead = true;
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + (iw >> 1) - 16, iy + (ih >> 1) - 16, 3, 4));

		if(movingObjectType == movingobject_fireball)
		{
			short iPlayerID = ((MO_Fireball*)this)->playerID;

			if(projectiles[iPlayerID] > 0)
				projectiles[iPlayerID]--;

			ifsoundonplay(sfx_hit);
		}
		else
		{
			ifsoundonplay(sfx_transform);

			if(movingObjectType == movingobject_egg)
			{
				dead = false;
				((CO_Egg*)this)->placeEgg();
			}
			else if(movingObjectType == movingobject_star)
			{
				dead = false;
				((CO_Star*)this)->placeStar();
			}
			else if(movingObjectType == movingobject_flag)
			{
				dead = false;
				((CO_Flag*)this)->placeFlag();
			}
		}
	}
}

//------------------------------------------------------------------------------
// class powerup
//------------------------------------------------------------------------------
MO_Powerup::MO_Powerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	desty = fy - collisionHeight;
	movingObjectType = movingobject_powerup;
	bounce = GRAVITATION;
}

void MO_Powerup::draw()
{
	if(state == 0)
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, (short)(ih - fy + desty));
	else
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);
}


void MO_Powerup::update()
{
	//Have the powerup grow out of the powerup block
	if(state == 0)
	{
		yf(fy - 2.0f);

		if(fy <= desty)
		{
			state = 1;
			vely = 1.0f;
		}
	}
	else //Then have it obey the physics
	{
		fOldX = fx;
		fOldY = fy;

		collision_detection_map();
	}
	
	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

}

bool MO_Powerup::collide(CPlayer *)
{
	dead = true;
	return false;
}

//------------------------------------------------------------------------------
// tanooki suit
//------------------------------------------------------------------------------
PU_Tanooki::PU_Tanooki(short x, short y)
    : MO_Powerup(&spr_tanooki, x, y, 1, 32000, 30, 30, 1, 1)
{
}

bool PU_Tanooki :: collide (CPlayer *player)
{
	dead = true;

	if(player->tanooki)
	{
		ifsoundonplay(sfx_storepowerup);
		game_values.gamepowerups[player->globalID] = 20;
	}
    else
    {
		ifsoundonplay(sfx_collectpowerup);
        player->tanooki = true;
    }
    return false;
}

//------------------------------------------------------------------------------
// class star powerup
//------------------------------------------------------------------------------
PU_StarPowerup::PU_StarPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	if(moveToRight)
		velx = 2.0f;
	else
		velx = -2.0f;

	bounce = -VELPOWERUPBOUNCE;
}

bool PU_StarPowerup::collide(CPlayer * player)
{
	dead = true;

	if(!player->invincible)
	{
		player->makeinvincible();
	}
	else
	{
		ifsoundonplay(sfx_storepowerup);
		game_values.gamepowerups[player->globalID] = 6;
	}

	return false;
}

//------------------------------------------------------------------------------
// class 1up powerup
//------------------------------------------------------------------------------
PU_ExtraGuyPowerup::PU_ExtraGuyPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short type) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	if(moveToRight)
		velx = 1.0f + (float)type;
	else
		velx = -1.0f - (float)type;

	iType = type;
}

bool PU_ExtraGuyPowerup::collide(CPlayer * player)
{
	game_values.gamemode->playerextraguy(*player, iType);
	ifsoundonplay(sfx_extraguysound);

	eyecandyfront.add(new EC_FloatingObject(&spr_extralife, player->ix + HALFPW - 19, player->iy - 16, 0.0f, -1.5f, 62, player->colorID * 38, (iType == 5 ? 3 : iType - 1) * 16, 38, 16));
	
	dead = true;
	return false;
}

//------------------------------------------------------------------------------
// class poison powerup
//------------------------------------------------------------------------------
PU_PoisonPowerup::PU_PoisonPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	if(moveToRight)
		velx = 2.0f;
	else
		velx = -2.0f;

	movingObjectType = movingobject_poisonpowerup;
}

bool PU_PoisonPowerup::collide(CPlayer * player)
{
	if(player->invincible)
	{
		dead = true;
		return false;
	}

	if(player->spawninvincible)
		return false;

	player->DeathAwards();
	
	if(!game_values.gamemode->playerkilledself(*player))
		player->die(0, false);
	
	ifsoundonplay(sfx_deathsound);

	dead = true;

	return true;
}

//------------------------------------------------------------------------------
// class mystery mushroom powerup
//------------------------------------------------------------------------------
PU_MysteryMushroomPowerup::PU_MysteryMushroomPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	if(moveToRight)
		velx = 2.0f;
	else
		velx = -2.0f;

	movingObjectType = movingobject_mysterymushroompowerup;
}

bool PU_MysteryMushroomPowerup::collide(CPlayer * player)
{
	dead = true;

	if(!SwapPlayers(player->localID))
	{
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, player->ix + (HALFPW) - 16, player->iy + (HALFPH) - 16, 3, 8));
		ifsoundonplay(sfx_spit);
	}
	
	return false;
}

//------------------------------------------------------------------------------
// class fire powerup
//------------------------------------------------------------------------------
PU_FirePowerup::PU_FirePowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_FirePowerup::collide(CPlayer * player)
{
	player->SetPowerup(1);
	dead = true;
	return false;
}

//------------------------------------------------------------------------------
// class hammer powerup
//------------------------------------------------------------------------------
PU_HammerPowerup::PU_HammerPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_HammerPowerup::collide(CPlayer * player)
{
	player->SetPowerup(2);
	dead = true;
	return false;
}	

//------------------------------------------------------------------------------
// class sledge hammer powerup
//------------------------------------------------------------------------------
PU_SledgeHammerPowerup::PU_SledgeHammerPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;

	sparkleanimationtimer = 0;
	sparkledrawframe = 0;
	bounce = -VELPOWERUPBOUNCE * 2;
	numbounces = 5;
	state = 2;
}

void PU_SledgeHammerPowerup::update()
{
	MO_Powerup::update();

	if(++sparkleanimationtimer >= 4)
	{
		sparkleanimationtimer = 0;
		sparkledrawframe += 32;
		if(sparkledrawframe >= 480)
			sparkledrawframe = 0;
	}
}

void PU_SledgeHammerPowerup::draw()
{
	MO_Powerup::draw();

	//Draw sparkles
	spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

bool PU_SledgeHammerPowerup::collide(CPlayer * player)
{
	//player->SetPowerup(5);

	if(state == 1)
	{
		ifsoundonplay(sfx_storepowerup);
		game_values.storedpowerups[player->globalID] = 21;
		game_values.gamepowerups[player->globalID] = 21;
		dead = true;
		game_values.noexit = false;
	}

	return false;
}

float PU_SledgeHammerPowerup::BottomBounce()
{
	if(state == 2)
	{
		if(--numbounces <= 0)
		{
			numbounces = 0;
			state = 1;
			bounce = GRAVITATION;
		}
		else
		{
			if(vely > 0.0f)
				bounce = -vely / 2.0f;
			else
				bounce /= 2.0f;
		}
	}

	return bounce;
}


//------------------------------------------------------------------------------
// class podobo powerup
//------------------------------------------------------------------------------
PU_PodoboPowerup::PU_PodoboPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	PU_SledgeHammerPowerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{}

bool PU_PodoboPowerup::collide(CPlayer * player)
{
	if(state == 1)
	{
		ifsoundonplay(sfx_storepowerup);
		game_values.storedpowerups[player->globalID] = 22;
		game_values.gamepowerups[player->globalID] = 22;
		dead = true;
		game_values.noexit = false;
	}

	return false;
}

//------------------------------------------------------------------------------
// class bomb powerup
//------------------------------------------------------------------------------
PU_BombPowerup::PU_BombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	PU_SledgeHammerPowerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{}

bool PU_BombPowerup::collide(CPlayer * player)
{
	if(state == 1)
	{
		ifsoundonplay(sfx_storepowerup);
		game_values.storedpowerups[player->globalID] = 23;
		game_values.gamepowerups[player->globalID] = 23;
		dead = true;
		game_values.noexit = false;
	}

	return false;
}

//------------------------------------------------------------------------------
// class clock powerup
//------------------------------------------------------------------------------
PU_ClockPowerup::PU_ClockPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_ClockPowerup::collide(CPlayer * player)
{
	dead = true;
	ifsoundonplay(sfx_storepowerup);
	game_values.gamepowerups[player->globalID] = 7;
	return false;
}

//------------------------------------------------------------------------------
// class bobomb powerup
//------------------------------------------------------------------------------
PU_BobombPowerup::PU_BobombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_BobombPowerup::collide(CPlayer * player)
{
	dead = true;

	if(player->bobomb)
	{
		ifsoundonplay(sfx_storepowerup);
		game_values.gamepowerups[player->globalID] = 8;
	}
	else
	{
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, player->ix + (HALFPW) - 16, player->iy + (HALFPH) - 16, 3, 8));
		ifsoundonplay(sfx_transform);
		player->bobomb = true;
	}

	return false;
}

//------------------------------------------------------------------------------
// class POW powerup
//------------------------------------------------------------------------------
PU_PowPowerup::PU_PowPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_PowPowerup::collide(CPlayer * player)
{
	dead = true;
	ifsoundonplay(sfx_storepowerup);
	game_values.gamepowerups[player->globalID] = 9;

	return false;
}

//------------------------------------------------------------------------------
// class MOd powerup
//------------------------------------------------------------------------------
PU_ModPowerup::PU_ModPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_ModPowerup::collide(CPlayer * player)
{
	dead = true;
	ifsoundonplay(sfx_storepowerup);
	game_values.gamepowerups[player->globalID] = 16;
	return false;
}

//------------------------------------------------------------------------------
// class BulletBill powerup
//------------------------------------------------------------------------------
PU_BulletBillPowerup::PU_BulletBillPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_BulletBillPowerup::collide(CPlayer * player)
{
	dead = true;
	ifsoundonplay(sfx_storepowerup);
	game_values.gamepowerups[player->globalID] = 10;
	return false;
}

//------------------------------------------------------------------------------
// class feather powerup
//------------------------------------------------------------------------------
PU_FeatherPowerup::PU_FeatherPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	desty = fy - collisionHeight;
	movingObjectType = movingobject_powerup;

	iw = (short)nspr->getWidth() >> 1;

	velx = 0.0f;
	bounce = GRAVITATION;
	fFloatDirectionRight = true;
	dFloatAngle = HALF_PI;
}

void PU_FeatherPowerup::draw()
{
	if(state == 0)
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, iw, (short)(ih - fy + desty));
	else if(state == 1)
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, iw, ih);
	else
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, (fFloatDirectionRight ? 0 : 32), 0, iw, ih);
}


void PU_FeatherPowerup::update()
{
	//Have the powerup grow out of the powerup block
	if(state == 0)
	{
		yf(fy - 4.0f);

		if(fy <= desty)
		{
			state = 1;
		}
	}
	else if(state == 1)
	{
		fOldX = fx;
		fOldY = fy;

		yf(fy - 4.0f);
		
		if(fy <= desty - 128.0f)
		{
			state = 2;
			dFloatCenterY = fy - 64.0f;
			dFloatCenterX = fx;
		}
	}
	else
	{
		if(!fFloatDirectionRight)
		{
			dFloatAngle += 0.035f;

			if(dFloatAngle >= THREE_QUARTER_PI)
			{
				dFloatAngle = THREE_QUARTER_PI;
				fFloatDirectionRight = true;
			}
		}
		else
		{
			dFloatAngle -= 0.035f;

			if(dFloatAngle <= QUARTER_PI)
			{
				dFloatAngle = QUARTER_PI;
				fFloatDirectionRight = false;
			}
		}

		dFloatCenterY += 1.0f;

		xf(64.0f * cos(dFloatAngle) + dFloatCenterX);
		yf(64.0f * sin(dFloatAngle) + dFloatCenterY);

		if(fy >= 480.0f)
			dead = true;
	}
}

bool PU_FeatherPowerup::collide(CPlayer * player)
{
	player->SetPowerup(3);
	eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, player->ix + (HALFPW) - 16, player->iy + (HALFPH) - 16, 3, 8));
	dead = true;
	return false;
}

//------------------------------------------------------------------------------
// class boomerang powerup
//------------------------------------------------------------------------------
PU_BoomerangPowerup::PU_BoomerangPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_BoomerangPowerup::collide(CPlayer * player)
{
	player->SetPowerup(4);
	dead = true;
	return false;
}	

//------------------------------------------------------------------------------
// class fireball
//------------------------------------------------------------------------------
MO_Fireball::MO_Fireball(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() >> 2, (short)nspr->getHeight() >> 3, 0, 0)
{
	if(moveToRight)
		velx = 5.0f;
	else
		velx = -5.0f;

	//fireball sprites have both right and left sprites in them
	ih = ih >> 3;

	bounce = -FIREBALLBOUNCE;

	playerID = iGlobalID;
	teamID = iTeamID;
	colorOffset = iColorID * 36;
	movingObjectType = movingobject_fireball;
	state = 1;

	ttl = game_values.fireballttl;
}

void MO_Fireball::update()
{
	IO_MovingObject::update();

	if(--ttl <= 0)
	{
		removeifprojectile(this, true, true);
	}
}

bool MO_Fireball::collide(CPlayer * player)
{
	if(playerID != player->globalID && (game_values.friendlyfire || teamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);

			if(!player->invincible)
			{
				//Find the player that shot this fireball so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(playerID);
				
				if(killer)
				{
					PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_fireball);
				}
				else
				{
					player->DeathAwards();

					if(!game_values.gamemode->playerkilledself(*player))
						player->die(0, false);

					ifsoundonplay(sfx_deathsound);
				}

				return true;
			}
		}
	}

	return false;
}

void MO_Fireball::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, (velx > 0 ? 0 : 18) + colorOffset, iw, ih);
}

//------------------------------------------------------------------------------
// class super fireball
//------------------------------------------------------------------------------
MO_SuperFireball::MO_SuperFireball(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, nspr->getWidth() / iNumSpr, nspr->getHeight() / 10, 0, 0)
{
	ih /= 10;

	playerID = iGlobalID;
	teamID = iTeamID;
	colorOffset = (iColorID + 1) * 64;
	directionOffset = velx < 0.0f ? 0 : 32;
	movingObjectType = movingobject_superfireball;
	state = 1;
	velx = fVelyX;
	vely = fVelyY;
	ttl = rand() % 30 + 60;
	
	drawframe = 0;
}

void MO_SuperFireball::update()
{
	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	xf(fx + velx);
	yf(fy + vely);
	
	if(ix < 0)
		xi(ix + 640);
	else if(ix > 639)
		xi(ix - 640);
	
	if(iy > 480 || iy < -ih || --ttl <= 0)
		removeifprojectile(this, false, true);
}

bool MO_SuperFireball::collide(CPlayer * player)
{
	if(playerID != player->globalID && (game_values.friendlyfire || teamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);

			if(!player->invincible)
			{
				//Find the player that shot this hammer so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(playerID);

				if(killer)
				{
					PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_hammer);
				}
				else
				{
					player->DeathAwards();

					if(!game_values.gamemode->playerkilledself(*player))
						player->die(0, false);

					ifsoundonplay(sfx_deathsound);
				}

				return true;
			}
		}
	}

	return false;
}

void MO_SuperFireball::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset + directionOffset, iw, ih);
}


//------------------------------------------------------------------------------
// class hammer
//------------------------------------------------------------------------------
MO_Hammer::MO_Hammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() >> 2, 0, 0)
{
	ih = ih >> 2;

	playerID = iGlobalID;
	teamID = iTeamID;
	colorOffset = iColorID * 28;
	movingObjectType = movingobject_hammer;
	state = 1;
	velx = fVelyX;
	vely = fVelyY;
	ttl = game_values.hammerttl;

	fSuper = superHammer;

	if(velx > 0.0f)
		drawframe = 0;
	else
		drawframe = animationWidth - iw;
}

void MO_Hammer::update()
{
	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		if(velx > 0)
		{
			drawframe += iw;
			if(drawframe >= animationWidth)
				drawframe = 0;
		}
		else
		{
			drawframe -= iw;
			if(drawframe < 0)
				drawframe = animationWidth - iw;
		}
	}

	xf(fx + velx);
	yf(fy + vely);
	
	if(!fSuper)
		vely += GRAVITATION;

	if(ix < 0)
		xi(ix + 640);
	else if(ix > 639)
		xi(ix - 640);
	
	if(iy > 480 || --ttl <= 0 || (fSuper && iy < -ih))
		removeifprojectile(this, false, true);
}

bool MO_Hammer::collide(CPlayer * player)
{
	if(playerID != player->globalID && (game_values.friendlyfire || teamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);

			if(!player->invincible)
			{
				//Find the player that shot this hammer so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(playerID);

				if(killer)
				{
					PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_hammer);
				}
				else
				{
					player->DeathAwards();

					if(!game_values.gamemode->playerkilledself(*player))
						player->die(0, false);

					ifsoundonplay(sfx_deathsound);
				}

				return true;
			}
		}
	}

	return false;
}

void MO_Hammer::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset, iw, ih);
}

//------------------------------------------------------------------------------
// class sledge hammer
//------------------------------------------------------------------------------
MO_SledgeHammer::MO_SledgeHammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() / 5, 0, 0)
{
	ih = collisionHeight;

	playerID = iGlobalID;
	teamID = iTeamID;
	colorOffset = (iColorID + 1) * 32;
	movingObjectType = movingobject_sledgehammer;
	state = 1;
	velx = fVelyX;
	vely = fVelyY;

	fSuper = superHammer;

	if(velx > 0.0f)
		drawframe = 0;
	else
		drawframe = animationWidth - iw;
}

void MO_SledgeHammer::update()
{
	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		if(velx > 0)
		{
			drawframe += iw;
			if(drawframe >= animationWidth)
				drawframe = 0;
		}
		else
		{
			drawframe -= iw;
			if(drawframe < 0)
				drawframe = animationWidth - iw;
		}
	}

	xf(fx + velx);
	yf(fy + vely);
	
	vely += GRAVITATION;

	if(ix < 0)
		xi(ix + 640);
	else if(ix > 639)
		xi(ix - 640);
	
	if(iy >= 480)
		removeifprojectile(this, false, true);
}

bool MO_SledgeHammer::collide(CPlayer * player)
{
	if(playerID != player->globalID && (game_values.friendlyfire || teamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);
			
			if(!player->invincible)
			{
				//Find the player that shot this hammer so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(playerID);

				if(killer)
				{
					PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_hammer);
				}
				else
				{
					player->DeathAwards();

					if(!game_values.gamemode->playerkilledself(*player))
						player->die(0, false);

					ifsoundonplay(sfx_deathsound);
				}

				return true;
			}
		}
	}

	return false;
}

void MO_SledgeHammer::explode()
{
	if(playerID > -1 && iy < 480)
	{
		if(fSuper)
		{
			objectsfront.add(new OMO_Explosion(&spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, playerID, teamID));
			ifsoundonplay(sfx_bobombsound);
		}
		else
		{
			short iCenterX = ix + (iw >> 1) - 14;
			short iCenterY = iy + (ih >> 1) - 14;
			short iColorID = colorOffset / 32 - 1;

			for(short iHammer = 0; iHammer < 3; iHammer++)
			{
				float dAngle = (float)(rand() % 628) / 100.0f;
				float dVel = (float)(rand() % 5) / 2.0f + 3.0f;
				float dVelX = dVel * cos(dAngle);
				float dVelY = dVel * sin(dAngle);
				objectsfront.add(new MO_Hammer(&spr_hammer, iCenterX, iCenterY, 6, dVelX, dVelY, 5, playerID, teamID, iColorID, true));
			}

			projectiles[playerID] += 3;

			ifsoundonplay(sfx_cannon);
		}
	}
}

void MO_SledgeHammer::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset, iw, ih);
}

//------------------------------------------------------------------------------
// class boomerang
//------------------------------------------------------------------------------
MO_Boomerang::MO_Boomerang(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superBoomerang) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() >> 3, 0, 0)
{
	fSuperBoomerang = superBoomerang;
	smoketimer = 0;

	//boomerangs sprites have both right and left sprites in them
	ih = ih >> 3;

	playerID = iGlobalID;
	teamID = iTeamID;
	colorOffset = iColorID * 64;
	movingObjectType = movingobject_boomerang;
	state = 1;
	
	fMoveToRight = moveToRight;

	if(moveToRight)
		velx = 5.0f;
	else
		velx = -5.0f;

	vely = 0.0f;

	fFlipped = false;

	//Don't let boomerang start off the screen or it won't rebound correctly
	if(moveToRight && fx + iw >= 640.0f)
		xf(fx - 640.0f);
	else if(!moveToRight && fx < 0.0f)
		xf(fx + 640.0f);

	iStateTimer = 0;

	if(game_values.boomerangstyle == 3)
		iStyle = rand() % 3;
	else
		iStyle = game_values.boomerangstyle;
}

void MO_Boomerang::update()
{
	if(!sfx_boomerang.isplaying())
		ifsoundonplay(sfx_boomerang);

	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	if(game_values.superboomerang[playerID] == 2)
	{
		removeifprojectile(this, false, true);

		objectsfront.add(new OMO_Explosion(&spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, playerID, teamID));
		ifsoundonplay(sfx_bobombsound);
	}

	if(iStyle == 0) //Flat style
	{
		fOldX = fx;
		xf(fx + velx);

		if(fMoveToRight && fx + iw >= 640.0f && fOldX + iw < 640.0f)
		{
			if(fFlipped)
			{
				forcedead();
				return;
			}
			else
			{
				xf(640.0f - iw);
				fFlipped = true;
				fMoveToRight = false;
				velx = -velx;
			}
		}
		else if(!fMoveToRight && fx < 0.0f && fOldX >= 0.0f)
		{
			if(fFlipped)
			{
				forcedead();
				return;
			}
			else
			{
				xf(0.0f);
				fFlipped = true;
				fMoveToRight = true;
				velx = -velx;
			}
		}
	}
	else if(iStyle == 1)
	{
		//Attempting to emulate the SMB3 boomerang behavior
		iStateTimer++;

		fOldX = fx;
		xf(fx + velx);

		if(fx < 0.0f)
			xf(fx + 640.0f);
		else if(fx + iw >= 640.0f)
			xf(fx - 640.0f);

		if(state == 1)
		{
			fOldY = fy;
			yf(fy - 3.2f);

			if(iStateTimer >= 20)
			{
				iStateTimer = 0;
				state = 2;
			}
		}
		else if(state == 2)
		{
			if(iStateTimer >= 26)
			{
				iStateTimer = 0;
				state = 3;
			}
		}
		else if(state == 3)
		{
			fOldY = fy;
			yf(fy + 1.0f);

			if(fMoveToRight)
			{
				//Add amount so that by time fy lowers by two tiles, we turn around the boomerang
				velx -= 0.15625f;

				if(velx <= -5.0f)
				{
					velx = -5.0f;
					state = 4;
					fFlipped = true;
				}
			}
			else
			{
				velx += 0.15625f;

				if(velx >= 5.0f)
				{
					velx = 5.0f;
					state = 4;
					fFlipped = true;
				}
			}

			iStateTimer = 0;
		}
		else if(state == 4)
		{
			if(iStateTimer >= 46)
			{
				if((fMoveToRight && fx < 0.0f && fOldX >= 0.0f) ||
					(!fMoveToRight && fx + iw >= 640.0f && fOldX + iw < 640.0f))
				{
					forcedead();
					return;
				}
			}
		}
	}
	else if(iStyle == 2)   //Zelda style boomerang
	{
		iStateTimer++;

		fOldX = fx;
		xf(fx + velx);

		if(fx < 0.0f)
			xf(fx + 640.0f);
		else if(fx + iw >= 640.0f)
			xf(fx - 640.0f);

		if(iStateTimer > game_values.boomeranglife)
		{
			forcedead();
			return;
		}

		if(state == 1)
		{
			if(iStateTimer >= 64)
			{
				state = 2;
				fFlipped = true;
				
				/*
				CPlayer * player = GetPlayerFromGlobalID(playerID);

				//No wrap boomerang
				if(player)
				{
					if((player->ix < ix && velx > 0) || (player->ix > ix && velx < 0))
						velx = -velx;
				}
				else
				{
					velx = -velx;
				}
				*/

				velx = -velx;  //Wrap Boomerang
			}
		}
		else if(state == 2)
		{
			fOldY = fy;
			yf(fy + vely);

			//Follow the player zelda style
			CPlayer * player = GetPlayerFromGlobalID(playerID);
			
			if(player)
			{
				bool fWrap = false;
				if(abs(player->ix - ix) > 320)
					fWrap = true;

				if((player->ix < ix && !fWrap) || (player->ix > ix && fWrap))  //Wrap Boomerang
				//if(player->ix < ix)  //No Wrap Boomerang
				{
					velx -= 0.2f;
					
					if(velx < -5.0f)
						velx = -5.0f;
				}
				else 
				{
					velx += 0.2f;

					if(velx > 5.0f)
						velx = 5.0f;
				}

				if(player->iy < iy)
				{
					vely -= 0.2f;
					
					if(vely < -3.0f)
						vely = -3.0f;
				}
				else 
				{
					vely += 0.2f;

					if(vely > 3.0f)
						vely = 3.0f;
				}
			}
			else 
			{
				//Remove boomerang if player was removed from game
				forcedead();
				return;

				/*
				//Die at nearest edge if player was removed from game
				if(velx > 0)
					velx = 5.0f;
				else
					velx = -5.0f;

				if((fx < 0.0f && fOldX >= 0.0f) ||
					(fx + iw >= 640.0f && fOldX + iw < 640.0f))
				{
					forcedead();
					return;
				}
				*/
			}
		}
	}
}

//Call to kill boomerang when it is not caught by player
void MO_Boomerang::forcedead()
{
	removeifprojectile(this, false, true);
	ifsoundonstop(sfx_boomerang);

	if(game_values.boomeranglimit == 0)
		return;

	//Penalize player if they did not catch it
	CPlayer * player = GetPlayerFromGlobalID(playerID);

	if(player)
	{
		if(player->projectilelimit > 0)
			player->DecreaseProjectileLimit();
	}
}

bool MO_Boomerang::collide(CPlayer * player)
{
	if(playerID != player->globalID && (game_values.friendlyfire || teamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);

			if(!player->invincible)
			{
				//Find the player that shot this boomerang so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(playerID);

				if(killer)
				{
					PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_boomerang);
				}
				else
				{
					player->DeathAwards();

					if(!game_values.gamemode->playerkilledself(*player))
						player->die(0, false);

					ifsoundonplay(sfx_deathsound);
				}

				return true;
			}
		}
	}
	else if(playerID == player->globalID && fFlipped)
	{
		removeifprojectile(this, false, true);
	}

	return false;
}

void MO_Boomerang::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset + (fMoveToRight ? 0 : 32), iw, ih);

	if(fSuperBoomerang)
	{
		if(++smoketimer > 2)
		{
			smoketimer = 0;
			eyecandyfront.add(new EC_SingleAnimation(&spr_bobombsmoke, ix - collisionOffsetX + (iw >> 1) - 8, iy - collisionOffsetY + (ih >> 1) - 8, 4, 4));
		}
	}
}

//------------------------------------------------------------------------------
// class bomb
//------------------------------------------------------------------------------
CO_Bomb::CO_Bomb(gfxSprite *nspr, short x, short y, float fVelX, float fVelY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, short timetolive) :
	MO_CarriedObject(nspr, x, y, 5, aniSpeed, 24, 24, 4, 14)
{
	iw = 28;
	ih = 38;

	bounce = GRAVITATION;

	playerID = iGlobalID;
	teamID = iTeamID;
	iColorOffsetY = (iColorID + 1) * 38;
	
	movingObjectType = movingobject_bomb;
	state = 1;

	ttl = timetolive;

	velx = fVelX;
	vely = fVelY;
}

bool CO_Bomb::collide(CPlayer * player)
{
	if(state == 1 && owner == NULL)
	{
		if(player->AcceptItem(this))
		{
			owner = player;

			velx = 0.0f;
			vely = 0.0f;

			if(playerID == -1)
			{
				playerID = owner->globalID;
				teamID = owner->teamID;
				iColorOffsetY = (owner->colorID + 1) * 38;
			}
		}
	}
	
	return false;
}

void CO_Bomb::update()
{
	if(--ttl <= 0)
		Die();

	if(dead)
		return;

	if(owner)
	{
		MoveToOwner();
	}
	else
	{
		//Add air/ground friction
		if(velx > 0.0f)
		{
			if(inair)
				velx -= VELAIRFRICTION;
			else if(onice)
				velx -= VELICEFRICTION;
			else
				velx -= VELMOVINGFRICTION;

			if(velx < 0.0f)
				velx = 0.0f;
		}
		else if(velx < 0.0f)
		{
			if(inair)
				velx += VELAIRFRICTION;
			else if(onice)
				velx += VELICEFRICTION;
			else
				velx += VELMOVINGFRICTION;

			if(velx > 0.0f)
				velx = 0.0f;
		}

		//Collision detect map
		fOldX = fx;
		fOldY = fy;

		collision_detection_map();
	}

	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
		{
			drawframe = 0;
		}
	}
}

void CO_Bomb::draw()
{
	if(owner && owner->iswarping())
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
	else
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);
}

void CO_Bomb::MoveToOwner()
{
	if(owner)
	{
		xi(owner->ix + (owner->IsPlayerFacingRight() ? 14 : -16));
		yi(owner->iy + PH - 40 + collisionOffsetY);
	}
}

void CO_Bomb::Drop()
{
	if(owner)
		owner->carriedItem = NULL;

	owner = NULL;

	collision_detection_checksides();
}

void CO_Bomb::Kick(bool superkick)
{
	velx = owner->velx + (owner->IsPlayerFacingRight() ? 2.0f : -2.0f);
	vely = -4.0f;
	ifsoundonplay(sfx_kicksound);

	Drop();
}

void CO_Bomb::Die()
{
	if(owner)
	{
		owner->carriedItem = NULL;
		owner = NULL;
	}

	if(playerID > -1 && projectiles[playerID] > 0)
		projectiles[playerID]--;

	dead = true;
	objectsfront.add(new OMO_Explosion(&spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, playerID, teamID));
	ifsoundonplay(sfx_bobombsound);
}

//------------------------------------------------------------------------------
// class coin (for coin mode)
//------------------------------------------------------------------------------
MO_Coin::MO_Coin(gfxSprite *nspr, short iNumSpr, short aniSpeed) :
	IO_MovingObject(nspr, 0, 0, iNumSpr, aniSpeed)
{
	state = 1;
	objectType = object_coin;

	sparkleanimationtimer = 0;
	sparkledrawframe = 0;

	placeCoin();
}

bool MO_Coin::collide(CPlayer * player)
{
	if(!game_values.gamemode->gameover)
		player->score->AdjustScore(1);

	eyecandyfront.add(new EC_SingleAnimation(&spr_coinsparkle, ix, iy, 7, 4));

	ifsoundonplay(sfx_coin);
	placeCoin();

	return false;
}

void MO_Coin::update()
{
	if(++animationtimer >= animationspeed)
	{
		animationtimer = 0;
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	if(++sparkleanimationtimer >= 4)
	{
		sparkleanimationtimer = 0;
		sparkledrawframe += 32;
		if(sparkledrawframe >= 480)
			sparkledrawframe = 0;
	}

	if(++timer > 1500)
		placeCoin();
}

void MO_Coin::draw()
{
	IO_MovingObject::draw();

	//Draw sparkles
	spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_Coin::placeCoin()
{
	timer = 0;
	
	short tries = 0;
	short x = 0, y = 0;
	do
	{
		if(++tries > 32)
			break;

		g_map.findspawnpoint(1, &x, &y, collisionWidth, collisionHeight, false);
	}
	while(objectsplayer.getClosestObject(x, y, object_coin) <= 150.0f);

	xi(x);
	yi(y);
}

//------------------------------------------------------------------------------
// class OverMapObject - moving objects that don't collide with map or objects, just player
//------------------------------------------------------------------------------
IO_OverMapObject::IO_OverMapObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed) :
	CObject(nspr, x, y)
{
	iNumSprites = iNumSpr;

	iw = (short)spr->getWidth() / iNumSprites;
	collisionWidth = iw;
	drawframe = 0;

	animationspeed = aniSpeed;
	animationtimer = 0;
	animationWidth = (short)spr->getWidth();

	objectType = object_overmap;
	movingObjectType = movingobject_none;
}


void IO_OverMapObject::draw()
{
	spr->draw(ix, iy, drawframe, 0, iw, ih);
}


void IO_OverMapObject::update()
{
	xf(fx + velx);
	yf(fy + vely);

	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}
}


//------------------------------------------------------------------------------
// class thwomp (for thwomp mode)
//------------------------------------------------------------------------------
OMO_Thwomp::OMO_Thwomp(gfxSprite *nspr, short x, float nspeed) :
	IO_OverMapObject(nspr, x, (short)-nspr->getHeight(), 1, 32000)
{
	objectType = object_thwomp;
	vely = nspeed;
}

void OMO_Thwomp::update()
{
	IO_OverMapObject::update();

	if(iy > 479)
		dead = true;
}

bool OMO_Thwomp::collide(CPlayer * player)
{
	if(!player->invincible && !player->spawninvincible && (player->score->score > 0 || game_values.gamemode->goal == -1))
	{
		player->DeathAwards();
		
		if(!game_values.gamemode->playerkilledself(*player))
			player->die(0, false);

		ifsoundonplay(sfx_deathsound);
		return true;
	}
	return false;
}


//------------------------------------------------------------------------------
// class podobo (for survival mode)
//------------------------------------------------------------------------------
OMO_Podobo::OMO_Podobo(gfxSprite *nspr, short x, float dVelY, short playerid, short teamid, short colorid) :
	IO_MovingObject(nspr, x, 480, 4, 6)
{
	objectType = object_moving;
	movingObjectType = movingobject_podobo;
	vely = dVelY;

	ih = 32;
	collisionHeight = ih;

	iPlayerID = playerid;
	iTeamID = teamid;
	iColorOffsetY = (colorid + 1) * 64;
}

void OMO_Podobo::update()
{
	//Special slow podobo gravity
	vely += 0.2f;

	xf(fx + velx);
	yf(fy + vely);

	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	if(iy > 479 && vely > 0.0f)
		dead = true;
}

void OMO_Podobo::draw()
{
	spr->draw(ix, iy, drawframe, iColorOffsetY + (vely > 0.0f ? 32 : 0), iw, ih);
}

bool OMO_Podobo::collide(CPlayer * player)
{
	if(player->globalID != iPlayerID && (game_values.friendlyfire || iTeamID != player->teamID) && !player->invincible && !player->spawninvincible)
	{
		//Find the player that made this explosion so we can attribute a kill
		CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

		if(killer)
		{
			PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_bounce);
		}
		else
		{
			player->DeathAwards();

			if(!game_values.gamemode->playerkilledself(*player))
				player->die(0, false);

			ifsoundonplay(sfx_deathsound);
		}

		return true;
	}

	return false;
}

void OMO_Podobo::collide(IO_MovingObject * object)
{
	if(iPlayerID == -1)
		return;

	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill)
	{
		//Same team bullet bills don't kill each other
		if(type == movingobject_bulletbill && ((OMO_BulletBill*) object)->iTeamID == iTeamID)
			return;

		if(type == movingobject_shell)
		{
			((CO_Shell*)object)->Die();
			ifsoundonplay(sfx_kicksound);
		}
		else if(type == movingobject_throwblock)
		{
			((CO_ThrowBlock*) object)->Die();
			ifsoundonplay(sfx_kicksound);
		}
		else if(type == movingobject_bulletbill)
		{
			((OMO_BulletBill*) object)->Die();
			ifsoundonplay(sfx_kicksound);
		}
	}
}

//------------------------------------------------------------------------------
// class Bowser Fire (for survival mode)
//------------------------------------------------------------------------------
OMO_BowserFire::OMO_BowserFire(gfxSprite *nspr, short x, short y, float dVelX, float dVelY, short id, short teamid, short colorid) :
	IO_OverMapObject(nspr, x, y, 3, 6)
{
	objectType = object_bowserfire;
	velx = dVelX;
	vely = dVelY;

	ih = 32;
	collisionHeight = ih;

	iPlayerID = id;
	iTeamID = teamid;
	iColorOffsetY = (colorid + 1) * 64;
}

void OMO_BowserFire::update()
{
	IO_OverMapObject::update();

	if((velx < 0 && ix < -iw) || (velx > 0 && ix > 640))
	{
		if(iPlayerID != -1 && projectiles[iPlayerID] > 0)
			projectiles[iPlayerID]--;

		dead = true;
	}
}

void OMO_BowserFire::draw()
{
	spr->draw(ix, iy, drawframe, (velx > 0.0f ? 32 : 0) + iColorOffsetY, iw, ih);
}

bool OMO_BowserFire::collide(CPlayer * player)
{
	if(player->globalID != iPlayerID && (game_values.friendlyfire || iTeamID != player->teamID) && !player->invincible && !player->spawninvincible)
	{
		//Find the player that made this explosion so we can attribute a kill
		CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

		if(killer)
		{
			PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_fireball);
		}
		else
		{
			player->DeathAwards();

			if(!game_values.gamemode->playerkilledself(*player))
				player->die(0, false);

			ifsoundonplay(sfx_deathsound);
		}

		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
// class thwomp (for thwomp mode)
//------------------------------------------------------------------------------
OMO_BulletBill::OMO_BulletBill(gfxSprite *nspr, short y, float nspeed, short playerID, bool homing) :
	IO_MovingObject(nspr, 0, y, 3, 8, 30, 28, 1, 2)
{
	fMaxVel = fabs(nspeed);
	velx = nspeed;
	vely = 0.0f;

	fGoalVelX = velx;
	fGoalVelY = vely;

	ih = (short)nspr->getHeight() >> 3;
	//collisionHeight = ih;
	
	if(velx < 0)
		xi(640 + iw);
	else
		xi(-iw);

	movingObjectType = movingobject_bulletbill;
	state = 1;

	iPlayerID = playerID;
	iColorID = game_values.colorids[playerID];
	iTeamID = LookupTeamID(iPlayerID);

	iColorOffsetY = 64 * iColorID;
	SetDirectionOffset();

	pHomingPlayer = NULL;
	fHoming = homing;

	HomeToNearestPlayer();
}

void OMO_BulletBill::update()
{
	if(pHomingPlayer)
	{
		if(fGoalVelX < 0.0f && velx > fGoalVelX)
			velx -= 0.2f;

		if(fGoalVelX > 0.0f && velx < fGoalVelX)
			velx += 0.2f;

		if(fGoalVelY < 0.0f && vely > fGoalVelY)
			vely -= 0.2f;

		if(fGoalVelY > 0.0f && vely < fGoalVelY)
			vely += 0.2f;
	}

	xf(fx + velx);
	yf(fy + vely);

	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	if((velx < 0 && ix < -iw) || (velx > 0 && ix > 640))
		dead = true;
}

void OMO_BulletBill::draw()
{
	spr->draw(ix, iy, drawframe, iColorOffsetY + iDirectionOffsetY, iw, ih);
}

bool OMO_BulletBill::collide(CPlayer * player)
{
	if(player->invincible)
	{
		AddAwardKill(player, NULL, kill_style_bulletbill);
		ifsoundonplay(sfx_kicksound);
		
		Die();
	}
	else
	{
		if(player->fOldY + PH <= iy && player->iy + PH >= iy)
			return hittop(player);
		else
			return hitother(player);
	}

	return false;
}


bool OMO_BulletBill::hittop(CPlayer * player)
{
	player->yi(iy - PH - 1);
	player->bouncejump();
	player->collision_detection_checktop();
	player->platform = NULL;
	
	AddAwardKill(player, NULL, kill_style_bulletbill);

	ifsoundonplay(sfx_mip);
	
	Die();

	return false;
}

bool OMO_BulletBill::hitother(CPlayer * player)
{
	if(player->spawninvincible || player->globalID == iPlayerID)
		return false;

	if(!game_values.friendlyfire && iTeamID == player->teamID)
		return false;

	//Find the player that owns this bullet bill so we can attribute a kill
	CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

	if(killer)
	{
		PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_bulletbill);
	}
	else
	{
		player->DeathAwards();

		if(!game_values.gamemode->playerkilledself(*player))
			player->die(0, false);

		ifsoundonplay(sfx_deathsound);
	}
	
	return true;
}

void OMO_BulletBill::collide(IO_MovingObject * object)
{
	removeifprojectile(object, true, false);

	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill)
	{
		//Don't kill things with shells that are sitting still
		if(type == movingobject_shell && ((CO_Shell*)object)->state == 2)
			return;

		//Same team bullet bills don't kill each other
		if(type == movingobject_bulletbill && ((OMO_BulletBill*) object)->iTeamID == iTeamID)
			return;

		if(type == movingobject_shell)
		{
			((CO_Shell*)object)->Die();
			ifsoundonplay(sfx_kicksound);
			Die();
		}
		else if(type == movingobject_throwblock)
		{
			((CO_ThrowBlock*) object)->Die();
			ifsoundonplay(sfx_kicksound);
			Die();
		}
		else if(type == movingobject_bulletbill)
		{
			OMO_BulletBill * bulletbill = (OMO_BulletBill*) object;
			bulletbill->dead = true;
			dead = true;

			short iOffsetX = 0;
			if(ix + iw < bulletbill->ix)
				iOffsetX = 640;
			else if(bulletbill->ix + bulletbill->iw < ix)
				iOffsetX = -640;

			short iCenterX = ((ix + iOffsetX - bulletbill->ix) >> 1) + (bulletbill->ix + (bulletbill->iw >> 1));
			short iCenterY = ((iy - bulletbill->iy) >> 1) + (bulletbill->iy + (bulletbill->ih >> 1));

			objectsfront.add(new OMO_Explosion(&spr_explosion, iCenterX - 96, iCenterY - 64, 2, 4, -1, -1));
			ifsoundonplay(sfx_bobombsound);
		}
	}
}

void OMO_BulletBill::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(&spr_bulletbilldead, ix, iy, -VELJUMP / 2.0f, velx > 0 ? 0 : 32, iColorID * 32, 32, 32));
}

void OMO_BulletBill::HomeToNearestPlayer()
{
	if(!fHoming)
		return;

	int iDistance = 640000;
	for(short k = 0; k < list_players_cnt; k++)
	{
		if(list_players[k]->globalID == iPlayerID)
			continue;

		if(list_players[k]->isready())
		{
			int iXValue = list_players[k]->ix - ix;
			int iYValue = list_players[k]->iy - iy;

			int playerDistance = iXValue * iXValue + iYValue * iYValue;

			if(playerDistance < iDistance)
			{
				iDistance = playerDistance;
				pHomingPlayer = list_players[k];
			}
		}
	}

	if(pHomingPlayer)
	{
		float fSlope = (float)(pHomingPlayer->ix - ix) / (float)(pHomingPlayer->iy - iy);
		
		if(pHomingPlayer->ix > ix)
			fGoalVelX = fMaxVel;
		else
			fGoalVelX = -fMaxVel;

		if(pHomingPlayer->iy > iy)
			fGoalVelY = fMaxVel / fabs(fSlope);
		else
			fGoalVelY = -fMaxVel / fabs(fSlope);

		SetDirectionOffset();
	}
}

void OMO_BulletBill::SetDirectionOffset()
{
	iDirectionOffsetY = velx < 0 ? 0 : 32;
}


//------------------------------------------------------------------------------
// class CarriedObject - all objects players can carry inheirit from this class
//------------------------------------------------------------------------------

MO_CarriedObject::MO_CarriedObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	owner = NULL;
	fSmoking = false;
}

MO_CarriedObject::MO_CarriedObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
	owner = NULL;	
	fSmoking = false;
}

MO_CarriedObject::~MO_CarriedObject()
{}


//------------------------------------------------------------------------------
// class egg (for egg mode)
//------------------------------------------------------------------------------
CO_Egg::CO_Egg(gfxSprite *nspr) :
	MO_CarriedObject(nspr, 0, 0, 2, 8, 30, 30, 1, 1)
{
	state = 1;
	iw = 32;
	bounce = GRAVITATION;
	objectType = object_egg;
	movingObjectType = movingobject_egg;
	owner_throw = NULL;
	owner_throw_timer = 0;

	sparkleanimationtimer = 0;
	sparkledrawframe = 0;
	
	placeEgg();
}

bool CO_Egg::collide(CPlayer * player)
{
	if(owner == NULL)
	{
		if(player->AcceptItem(this))
		{
			owner = player;
			owner_throw = player;
		}
	}

	return false;
}

void CO_Egg::update()
{
	if(owner)
	{
		MoveToOwner();
		timer = 0;
		owner_throw = owner;
	}
	else if(++timer > 1500)
	{
		placeEgg();
		owner_throw = NULL;
	}
	else
	{
		if(owner_throw && --owner_throw_timer <= 0)
		{
			owner_throw_timer = 0;
			owner_throw = NULL;
		}

		//Add air/ground friction
		if(velx > 0.0f)
		{
			if(inair)
				velx -= VELAIRFRICTION;
			else if(onice)
				velx -= VELICEFRICTION;
			else
				velx -= VELMOVINGFRICTION;

			if(velx < 0.0f)
				velx = 0.0f;
		}
		else if(velx < 0.0f)
		{
			if(inair)
				velx += VELAIRFRICTION;
			else if(onice)
				velx += VELICEFRICTION;
			else
				velx += VELMOVINGFRICTION;

			if(velx > 0.0f)
				velx = 0.0f;
		}

		//Collision detect map
		IO_MovingObject::update();
	}

	if(++sparkleanimationtimer >= 4)
	{
		sparkleanimationtimer = 0;
		sparkledrawframe += 32;
		if(sparkledrawframe >= 480)
			sparkledrawframe = 0;
	}
}

void CO_Egg::draw()
{
	if(owner)
	{
		if(owner->iswarping())
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 64, 0, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 64, 0, iw, ih);
	}
	else
	{
		if(velx != 0.0f)
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 64, 0, iw, ih);  //keep the egg still while it's moving
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);
	}
}

void CO_Egg::MoveToOwner()
{
	if(owner)
	{
		xi(owner->ix + (owner->IsPlayerFacingRight() ? HALFPW : HALFPW - 32));
		yi(owner->iy + PH - 32 + collisionOffsetY);
	}
}

void CO_Egg::placeEgg()
{
	timer = 0;

	short tries = 0;
	short x = 0, y = 0;
	do
	{
		if(++tries > 32)
			break;

		g_map.findspawnpoint(1, &x, &y, collisionWidth, collisionHeight, false);
	}
	while(objectsplayer.getClosestObject(x, y, object_yoshi) < 250.0f);

	xi(x);
	yi(y);

	vely = GRAVITATION;
	velx = 0.0f;

	owner_throw = NULL;
	owner_throw_timer = 0;

	Drop();
}

void CO_Egg::Drop()
{
	if(owner)
		owner->carriedItem = NULL;

	owner = NULL;

	collision_detection_checksides();

	owner_throw_timer = 62;
}

void CO_Egg::Kick(bool superkick)
{
	velx = owner->velx + (owner->IsPlayerFacingRight() ? 2.0f : -2.0f);
	vely = -4.0f;
	ifsoundonplay(sfx_kicksound);

	Drop();
}

//------------------------------------------------------------------------------
// class star (for star mode)
//------------------------------------------------------------------------------
CO_Star::CO_Star(gfxSprite *nspr) :
	MO_CarriedObject(nspr, 0, 0, 8, 8, 30, 30, 1, 1)
{
	state = 1;
	iw = 32;
	ih = 32;
	bounce = GRAVITATION;
	objectType = object_star;
	movingObjectType = movingobject_star;
	
	iOffsetY = game_values.gamemodesettings.star.shine ? 32 : 0;

	sparkleanimationtimer = 0;
	sparkledrawframe = 0;

	placeStar();
}

bool CO_Star::collide(CPlayer * player)
{
	timer = 0;
	if(owner == NULL)
	{
		if(player->throw_star == 0 && player->AcceptItem(this))
		{
			owner = player;
		}
	}

	if(player->spawninvincible || player->invincible || game_values.gamemode->star == player || game_values.gamemode->gameover)
		return false;

	if(game_values.gamemode->star)
	{
		CPlayer * oldstar = game_values.gamemode->star;
		oldstar->spawninvincible = true;
		oldstar->spawninvincibletimer = 60;
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, oldstar->ix + (HALFPW) - 16, oldstar->iy + (HALFPH) - 16, 3, 8));

		if(owner == oldstar)
		{
			oldstar->throw_star = 30;
			Kick(false);
		}
	}

	game_values.gamemode->star = player;
	
	if(game_values.gamemodesettings.star.shine)
		eyecandyfront.add(new EC_GravText(&game_font_large, player->ix + HALFPW, player->iy + PH, "Shine Get!", -VELJUMP*1.5));
	else
		eyecandyfront.add(new EC_GravText(&game_font_large, player->ix + HALFPW, player->iy + PH, "Ztarred!", -VELJUMP*1.5));

	eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, player->ix + (HALFPW) - 16, player->iy + (HALFPH) - 16, 3, 8));
	ifsoundonplay(sfx_transform);

	return false;
}

void CO_Star::update()
{
	if(owner)
	{
		MoveToOwner();
		timer = 0;
	}
	else if(++timer > 300)
	{
		placeStar();
	}
	else
	{
		//Add air/ground friction
		if(velx > 0.0f)
		{
			if(inair)
				velx -= VELAIRFRICTION;
			else if(onice)
				velx -= VELICEFRICTION;
			else
				velx -= VELMOVINGFRICTION;

			if(velx < 0.0f)
				velx = 0.0f;
		}
		else if(velx < 0.0f)
		{
			if(inair)
				velx += VELAIRFRICTION;
			else if(onice)
				velx += VELICEFRICTION;
			else
				velx += VELMOVINGFRICTION;

			if(velx > 0.0f)
				velx = 0.0f;
		}
		
		//Collision detect map
		IO_MovingObject::update();
	}

	if(++sparkleanimationtimer >= 4)
	{
		sparkleanimationtimer = 0;
		sparkledrawframe += 32;
		if(sparkledrawframe >= 480)
			sparkledrawframe = 0;
	}
}

void CO_Star::draw()
{
	if(owner)
	{
		if(owner->iswarping())
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih);
	}
	else
	{
		if(velx != 0.0f)
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih);  //keep the star still while it's moving
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iOffsetY, iw, ih);
	}

	if(owner && owner->iswarping())
		spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, game_values.gamemodesettings.star.shine ? 0 : 32, 32, 32, (short)owner->state % 4, owner->GetWarpPlane());
	else
		spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, game_values.gamemodesettings.star.shine ? 0 : 32, 32, 32);
}

void CO_Star::MoveToOwner()
{
	if(owner)
	{
		xi(owner->ix + (owner->IsPlayerFacingRight() ? HALFPW : HALFPW - 32));
		yi(owner->iy + PH - 32 + collisionOffsetY);
	}
}

void CO_Star::placeStar()
{
	timer = 0;

	if(game_values.gamemode->star)
	{
		CPlayer * star = game_values.gamemode->star;
		xf(star->fx + HALFPW - 16.0f);
		yf(star->fy + HALFPH - 17.0f);

		velx = star->velx;
		vely = star->vely;
	}

	Drop();
}

void CO_Star::Drop()
{
	if(owner)
		owner->carriedItem = NULL;

	owner = NULL;

	collision_detection_checksides();
}

void CO_Star::Kick(bool superkick)
{
	if(owner)
	{
		velx = owner->velx + (owner->IsPlayerFacingRight() ? 3.0f : -3.0f);
		vely = -6.0f;
		ifsoundonplay(sfx_kicksound);
	}

	Drop();
}

//------------------------------------------------------------------------------
// class flag base (for CTF mode)
//------------------------------------------------------------------------------
OMO_FlagBase::OMO_FlagBase(gfxSprite *nspr, short iTeamID, short iColorID) :
	IO_OverMapObject(nspr, 1280, 960, 5, 32000)  //use 1280 and 960 so when placing base, it doesn't interfere (look in getClosestObject())
{
	state = 1;
	iw = 32;
	ih = 32;
	collisionWidth = 32;
	collisionHeight = 32;

	objectType = object_flagbase;
	teamID = iTeamID;
	iGraphicOffsetX = iColorID * 48;
	
	angle = (float)(rand()%1000 * 0.00628f);
	anglechange = (float)(rand()%100 * 0.0002f);
	anglechangetimer = (short)(rand() % 50 + 100);

	velx = sin(angle);
	vely = cos(angle);

	homeflag = NULL;

	placeFlagBase();

	speed = (float)game_values.gamemodesettings.flag.speed / 4.0f;
}

bool OMO_FlagBase::collide(CPlayer * player)
{
	if(teamID == player->teamID && player->carriedItem && player->carriedItem->getObjectType() == object_flag)
	{
		CO_Flag * flag = (CO_Flag*)player->carriedItem;
		scoreFlag(flag, player);
	}

	return false;
}

void OMO_FlagBase::draw()
{
	spr->draw(ix - 8, iy - 8, iGraphicOffsetX, 0, 48, 48);
}


void OMO_FlagBase::update()
{
	if(--anglechangetimer <= 0)
	{
		anglechange = (float)((rand()%101 - 50) * 0.0002f);
		anglechangetimer = (short)(rand() % 50 + 100);
	}
	
	angle += anglechange;

	velx = speed * sin(angle);
	vely = speed * cos(angle);
	
	IO_OverMapObject::update();

	ix = (short)fx;
	iy = (short)fy;

	if(ix < 0)
	{
		velx = -velx;
		ix = 0;
		fx = (float)ix;

		angle = atan2(velx, vely);
	}
	else if(ix + collisionWidth > 639)
	{
		velx = -velx;
		ix = 639 - collisionWidth;
		fx = (float)ix;

		angle = atan2(velx, vely);
	}
	
	if(iy < 0)
	{
		vely = -vely;
		iy = 0;
		fy = (float)iy;

		angle = atan2(velx, vely);
	}
	else if(iy + collisionHeight > 479)
	{
		vely = -vely;
		iy = 479 - collisionHeight;
		fy = (float)iy;

		angle = atan2(velx, vely);
	}
}

void OMO_FlagBase::placeFlagBase()
{
	short tries = 0;
	short x = 0, y = 0;
	do
	{
		if(++tries > 32)
			break;

		g_map.findspawnpoint(1, &x, &y, collisionWidth, collisionHeight, true);
	}
	while(objectcollisionitems.getClosestObject(x, y, object_flagbase) <= 200.0f);

	xi(x);
	yi(y);
}

void OMO_FlagBase::collide(IO_MovingObject * object)
{
	if(object->getMovingObjectType() == movingobject_flag)
	{
		CO_Flag * flag = (CO_Flag*)object;

		if(flag->owner_throw)
		{
			CPlayer * player = flag->owner_throw;

			if(teamID == player->teamID)
			{
				scoreFlag(flag, player);
			}
		}
	}
}

void OMO_FlagBase::scoreFlag(CO_Flag * flag, CPlayer * player)
{
	if(flag->teamID == teamID)
	{
		flag->placeFlag();
		ifsoundonplay(sfx_areatag);
	}
	else if(!game_values.gamemodesettings.flag.homescore || homeflag != NULL)
	{
		flag->placeFlag();
		if(!game_values.gamemode->gameover)
			player->score->AdjustScore(1);
		
		ifsoundonplay(sfx_racesound);

		if(game_values.gamemodesettings.flag.pointmove)
		{
			ix = 1280;
			iy = 960;
			placeFlagBase();
		}
	}
}

//------------------------------------------------------------------------------
// class flag (for Capture the Flag mode)
//------------------------------------------------------------------------------
CO_Flag::CO_Flag(gfxSprite *nspr, OMO_FlagBase * base, short iTeamID, short iColorID) :
	MO_CarriedObject(nspr, 0, 0, 4, 8, 30, 30, 1, 1, 0, iColorID * 64, 32, 32)
{
	state = 1;
	bounce = GRAVITATION;
	objectType = object_flag;
	movingObjectType = movingobject_flag;
	flagbase = base;
	teamID = iTeamID;
	fLastFlagDirection = false;
	owner_throw = NULL;
	owner_throw_timer = 0;
	
	placeFlag();
}

bool CO_Flag::collide(CPlayer * player)
{
	if(owner == NULL && (!fInBase || teamID != player->teamID))
	{
		if(game_values.gamemodesettings.flag.touchreturn && teamID == player->teamID)
		{
			placeFlag();
			ifsoundonplay(sfx_areatag);
		}
		else if(player->AcceptItem(this))
		{
			owner = player;
			owner_throw = player;
			flagbase->setFlag(NULL);
		}
	}
	
	return false;
}

void CO_Flag::update()
{
	if(owner)
	{
		MoveToOwner();
		timer = 0;
		fInBase = false;
		owner_throw = owner;
	}
	else if(fInBase)
	{
		xf(flagbase->fx);
		yf(flagbase->fy);
		owner_throw = NULL;
	}
	else if(game_values.gamemodesettings.flag.autoreturn > 0 && ++timer > game_values.gamemodesettings.flag.autoreturn)
	{
		timer = 0;
		placeFlag();
		owner_throw = NULL;
	}
	else
	{
		if(owner_throw && --owner_throw_timer < 0)
		{
			owner_throw_timer = 0;
			owner_throw = NULL;
		}

		//Add air/ground friction
		if(velx > 0.0f)
		{
			if(inair)
				velx -= VELAIRFRICTION;
			else if(onice)
				velx -= VELICEFRICTION;
			else
				velx -= VELMOVINGFRICTION;

			if(velx < 0.0f)
				velx = 0.0f;
		}
		else if(velx < 0.0f)
		{
			if(inair)
				velx += VELAIRFRICTION;
			else if(onice)
				velx += VELICEFRICTION;
			else
				velx += VELMOVINGFRICTION;

			if(velx > 0.0f)
				velx = 0.0f;
		}

		//Collision detect map
		fOldX = fx;
		fOldY = fy;

		collision_detection_map();
	}

	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = animationOffsetX;
	}
}

void CO_Flag::draw()
{
	if(owner)
	{
		if(owner->iswarping())
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih);
	}
	else
	{
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih);
	}
}

void CO_Flag::MoveToOwner()
{
	if(owner)
	{
		fLastFlagDirection = owner->IsPlayerFacingRight();
		xi(owner->ix + HALFPW + (fLastFlagDirection ? -32 : 0));
		yi(owner->iy + PH - 38 + collisionOffsetY);
	}
}

void CO_Flag::placeFlag()
{
	if(flagbase)
	{
		Drop();
		fInBase = true;
		xi(flagbase->ix);
		yi(flagbase->iy);
		fLastFlagDirection = false;
		flagbase->setFlag(this);
	}
	
	owner_throw = NULL;
	owner_throw_timer = 0;

	timer = 0;
}

void CO_Flag::Drop()
{
	if(owner)
		owner->carriedItem = NULL;

	owner = NULL;

	collision_detection_checksides();

	owner_throw_timer = 62;
}

void CO_Flag::Kick(bool superkick)
{
	velx = owner->velx + (owner->IsPlayerFacingRight() ? 2.0f : -2.0f);
	vely = -4.0f;
	ifsoundonplay(sfx_kicksound);

	Drop();
}

//------------------------------------------------------------------------------
// class yoshi (for egg mode)
//------------------------------------------------------------------------------
OMO_Yoshi::OMO_Yoshi(gfxSprite *nspr) :
	IO_OverMapObject(nspr, 0, 0, 2, 8)
{
	state = 1;
	placeYoshi();
	objectType = object_yoshi;
}

bool OMO_Yoshi::collide(CPlayer * player)
{
	if(player->carriedItem && player->carriedItem->getObjectType() == object_egg)
	{
		if(!game_values.gamemode->gameover)
			player->score->AdjustScore(1);
	
		placeYoshi();

		CO_Egg * egg = (CO_Egg*)player->carriedItem;
		egg->placeEgg();

		ifsoundonplay(sfx_yoshi);
	}

	return false;
}

void OMO_Yoshi::update()
{
	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	if(++timer > 2000)
		placeYoshi();
}

void OMO_Yoshi::placeYoshi()
{
	timer = 0;

	for(short tries = 0; tries < 64; tries++)
	{
		ix = (short)(rand()%(640 - iw));
		iy = (short)(rand()%(480 - ih - TILESIZE));	//don't spawn too low

		short ixl = ix / TILESIZE;
		short ixr = (ix + iw) / TILESIZE;
		short iyt = iy / TILESIZE;
		short iyb = (iy + ih) / TILESIZE;

		if((g_map.map(ixl, iyt) == tile_nonsolid || g_map.map(ixl, iyt) == tile_solid_on_top) &&
		   (g_map.map(ixr, iyt) == tile_nonsolid || g_map.map(ixr, iyt) == tile_solid_on_top) &&
		   (g_map.map(ixl, iyb) == tile_nonsolid || g_map.map(ixl, iyb) == tile_solid_on_top) &&
		   (g_map.map(ixr, iyb) == tile_nonsolid || g_map.map(ixr, iyb) == tile_solid_on_top) &&
			!g_map.block(ixl, iyt) && !g_map.block(ixr, iyt) && !g_map.block(ixl, iyb) && !g_map.block(ixr, iyb))
		{
			//spawn on ground, but not on spikes
			short iDeathY = (iy+ih)/TILESIZE;
			short iDeathX1 = ix/TILESIZE;
			short iDeathX2 = (ix+iw)/TILESIZE;
			
			while(iDeathY < MAPHEIGHT)
			{
				TileType ttLeftTile = g_map.map(iDeathX1, iDeathY);
				TileType ttRightTile = g_map.map(iDeathX2, iDeathY);

				if(ttLeftTile == tile_solid || ttLeftTile == tile_solid_on_top || ttLeftTile == tile_death_on_bottom || ttLeftTile == tile_ice ||
					ttRightTile == tile_solid || ttRightTile == tile_solid_on_top || ttRightTile == tile_death_on_bottom || ttRightTile == tile_ice ||
					g_map.block(iDeathX1, iDeathY) || g_map.block(iDeathX2, iDeathY))
				{
					short top = (iDeathY * TILESIZE - ih) / TILESIZE;
					
					if(g_map.spawn(1, iDeathX1, top) && g_map.spawn(1, iDeathX2, top) && 
						g_map.spawn(1, iDeathX1, iDeathY - 1) && g_map.spawn(1, iDeathX2, iDeathY - 1))
					{
						xi(ix);
						yi(iDeathY * TILESIZE - ih);
						return;
					}

					break;
				}
				else if(ttLeftTile == tile_death_on_top || ttRightTile == tile_death_on_top ||
					ttLeftTile == tile_death || ttRightTile == tile_death)
				{
					break;
				}

				iDeathY++;
			}
		}
	}

	ix = 320;
	iy = 240;
}

void OMO_Yoshi::collide(IO_MovingObject * object)
{
	if(object->getMovingObjectType() == movingobject_egg)
	{
		CO_Egg * egg = (CO_Egg*)object;

		if(egg->owner_throw)
		{
			CPlayer * player = egg->owner_throw;

			if(!game_values.gamemode->gameover)
				player->score->AdjustScore(1);

			placeYoshi();
			egg->placeEgg();

			ifsoundonplay(sfx_yoshi);
		}
	}
}

//------------------------------------------------------------------------------
// class area (for Domination mode)
//------------------------------------------------------------------------------
OMO_Area::OMO_Area(gfxSprite *nspr, short iNumAreas) :
	IO_OverMapObject(nspr, 1280, 960, 5, 32000)
{
	iw = (short)spr->getWidth() / 5;
	collisionWidth = iw;

	objectType = object_area;
	playerID = -1;
	colorID = -1;
	teamID = -1;
	scoretimer = 0;
	state = 1;
	frame = 0;
	relocatetimer = 0;
	numareas = iNumAreas;
	totalTouchingPlayers = 0;
	touchingPlayer = NULL;
	
	placeArea();
}

bool OMO_Area::collide(CPlayer * player)
{
	if(player->statue_timer == 0)
	{
		totalTouchingPlayers++;

		if(totalTouchingPlayers == 1)
		{
			touchingPlayer = player;
		}
		else
		{
			touchingPlayer = NULL;
			reset();
		}
	}

	return false;
}

void OMO_Area::draw()
{
	spr->draw(ix, iy, frame, 0, iw, ih);
}

void OMO_Area::update()
{
	if(touchingPlayer)
		setOwner(touchingPlayer);

	if(playerID != -1 && !game_values.gamemode->gameover)
	{
		if(++scoretimer >= (game_values.pointspeed << 1))
		{
			scoretimer = 0;
			list_players[playerID]->score->AdjustScore(1);
		}
	}

	if(game_values.gamemodesettings.domination.relocationfrequency > 0)
	{
		if(++relocatetimer >= game_values.gamemodesettings.domination.relocationfrequency)
		{
			relocatetimer = 0;
			placeArea();
		}
	}

	totalTouchingPlayers = 0;
	touchingPlayer = NULL;
}

void OMO_Area::placeArea()
{
	short tries = 0;
	short x = 0, y = 0;
	do
	{
		if(++tries > 32)
			break;

		g_map.findspawnpoint(1, &x, &y, collisionWidth, collisionHeight, true);
	}
	while(objectcollisionitems.getClosestObject(x, y, object_area) <= (200.0f - ((numareas - 3) * 25.0f)));

	xi(x);
	yi(y);
}

void OMO_Area::reset()
{
	playerID = -1;
	colorID = -1;
	teamID = -1;
	scoretimer = 0;
	frame = 0;
}

void OMO_Area::setOwner(CPlayer * player)
{
	if(colorID != player->colorID)
	{
		colorID = player->colorID;
		playerID = player->localID;
		teamID = player->teamID;

		frame = (colorID + 1) * iw;
		ifsoundonplay(sfx_areatag);

		if(game_values.secrets)
		{
			//See if all the areas are owned by this player to have a boss peek
			bool fAllTagged = true;
			short iNumBases = 0;
			for(short iObject = 0; iObject < objectcollisionitems.list_end; iObject++)
			{
				if(objectcollisionitems.list[iObject]->getObjectType() == object_area)
				{
					iNumBases++;

					OMO_Area * area = (OMO_Area*)objectcollisionitems.list[iObject];
					if(area->teamID != teamID)
					{
						fAllTagged = false;
						break;
					}
				}
			}

			if(fAllTagged && iNumBases >= 3 && rand() % 3 == 0)
			{
				if(!game_values.gamemode->gameover && game_values.bosspeeking == -1)
				{
					eyecandyfront.add(new EC_BossPeeker(&spr_sledgebrothers, rand()%90 + 90, 1));
					
					backgroundmusic[0].stop();
					ifsoundonstop(sfx_invinciblemusic);
					ifsoundonstop(sfx_timewarning);
					ifsoundonplay(sfx_bowserlaugh);
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
// class KingOfTheHillArea (for King of the Hill mode)
//------------------------------------------------------------------------------
OMO_KingOfTheHillZone::OMO_KingOfTheHillZone(gfxSprite *nspr) :
	IO_OverMapObject(nspr, 0, 0, 5, 32000)
{
	size = game_values.gamemodesettings.kingofthehill.areasize;
	iw = TILESIZE * size;
	collisionWidth = iw;
	ih = TILESIZE * size;
	collisionHeight = ih;

	objectType = object_kingofthehill_area;
	colorID = -1;
	scoretimer = 0;
	state = 1;
	frame = 0;
	relocatetimer = 0;
	playerID = -1;
	
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		playersTouching[iPlayer] = NULL;
		playersTouchingCount[iPlayer] = 0;
	}

	totalTouchingPlayers = 0;

	placeArea();
}

bool OMO_KingOfTheHillZone::collide(CPlayer * player)
{
	if(player->statue_timer == 0)
	{
		playersTouching[player->teamID] = player;
		playersTouchingCount[player->teamID]++;
		totalTouchingPlayers++;
	}
	return false;
}

void OMO_KingOfTheHillZone::draw()
{
	for(short iRow = 0; iRow < size; iRow++)
	{
		short iYPiece = TILESIZE;
		if(iRow == 0)
			iYPiece = 0;
		if(iRow == size - 1)
			iYPiece = TILESIZE * 2;

		for(short iCol = 0; iCol < size; iCol++)
		{
			short iXPiece = TILESIZE;
			if(iCol == 0)
				iXPiece = 0;
			if(iCol == size - 1)
				iXPiece = TILESIZE * 2;

			spr->draw(ix + iCol * TILESIZE, iy + iRow * TILESIZE, iXPiece + frame, iYPiece, TILESIZE, TILESIZE);
		}
	}	
}

void OMO_KingOfTheHillZone::update()
{
	short iMax = 0; 
	short iMaxTeam = -1;

	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		if(playersTouchingCount[iTeam] > iMax)
		{
			iMax = playersTouchingCount[iTeam];
			iMaxTeam = iTeam;
		}
	}

	if((iMax << 1) > totalTouchingPlayers) //If the max touching player team is greater than the rest of the touching players
	{
		colorID = playersTouching[iMaxTeam]->colorID;
		playerID = playersTouching[iMaxTeam]->localID;
		frame = (colorID + 1) * TILESIZE * 3;
	}
	else
	{
		colorID = -1;
		playerID = -1;
		frame = 0;
	}

	if(playerID != -1 && !game_values.gamemode->gameover)
	{
		scoretimer += (iMax << 1) - totalTouchingPlayers;  //Speed of point accumulation is proportional to how many players are in zone

		if(scoretimer >= game_values.pointspeed)
		{
			scoretimer = 0;
			list_players[playerID]->score->AdjustScore(1);
		}
	}

	if(game_values.gamemodesettings.kingofthehill.relocationfrequency > 0)
	{
		if(++relocatetimer >= game_values.gamemodesettings.kingofthehill.relocationfrequency)
		{
			relocatetimer = 0;
			placeArea();
		}
	}

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		playersTouching[iPlayer] = NULL;
		playersTouchingCount[iPlayer] = 0;
	}

	totalTouchingPlayers = 0;
}

void OMO_KingOfTheHillZone::placeArea()
{
	relocatetimer = 0;
	colorID = -1;
	playerID = -1;
	frame = 0;

	short x;
	short y;

	g_map.CalculatePlatformNoSpawnZones();

	for(short iLoop = 0; iLoop < 64; iLoop++)
	{
		x = (short)(rand()%(MAPWIDTH - size + 1));
		y = (short)(rand()%(MAPHEIGHT - size));

		//First move the zone down so it is sitting on atleast 1 solid tile
		short iFindY = y + size;
		short iOldFindY = iFindY;
		bool fTryAgain = false;
		bool fDone = false;

		while(!fDone)
		{
			for(short iCol = 0; iCol < size; iCol++)
			{
				TileType type = g_map.map(x + iCol, iFindY);
				if(type == tile_solid_on_top || type == tile_solid || type == tile_death_on_bottom || type == tile_ice || g_map.block(x + iCol, iFindY))
				{
					fDone = true;
					break;
				}
			}

			if(fDone)
				break;

			if(++iFindY >= MAPHEIGHT)
				iFindY = size;

			if(iFindY == iOldFindY)  //If we didn't find solid ground in that loop, look for a new place for the zone
			{
				fTryAgain = true;
				break;
			}
		}

		if(fTryAgain)
			continue;

		y = iFindY - size;
	
		//Now verify that the area is not completely covered with solid tiles
		short iCountSolidTiles = 0;
		for(short iRow = 0; iRow < size; iRow++)
		{
			for(short iCol = 0; iCol < size; iCol++)
			{
				//If there is a solid tile inside the zone
				if((g_map.map(x + iCol, y + iRow) & 0x5) > 0 || !g_map.spawn(1, x + iCol, y + iRow) || g_map.block(x + iCol, y + iRow))
				{
					iCountSolidTiles++;

					//Be more picky in the first few loops, but allow solid tiles to be in
					if((iLoop < 16 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][0]) || (iLoop < 32 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][1]) || 
						(iLoop < 48 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][2]) || (iLoop < 63 && iCountSolidTiles > iKingOfTheHillZoneLimits[size - 2][3]))
					{
						fTryAgain = true;
						break;
					}
				}
			}

			if(fTryAgain)
				break;
		}

		if(fTryAgain)
			continue;

		//Verify zone is not in a platform
		if(g_map.IsInPlatformNoSpawnZone(x * TILESIZE, y * TILESIZE, size * TILESIZE, size * TILESIZE))
			continue;

		break;
	}

	ix = x * TILESIZE;
	iy = y * TILESIZE;
}

void OMO_KingOfTheHillZone::reset()
{
	playerID = -1;
	colorID = -1;
	scoretimer = 0;
	frame = 0;
}

//------------------------------------------------------------------------------
// class race goal (for Race mode)
//------------------------------------------------------------------------------
short flagpositions[3][4][2] = { { {18, 20}, {34, 20}, {0, 0}, {0, 0} },
							{ {18, 20}, {34, 20}, {26, 36}, {0, 0} },
							{ {18, 20}, {34, 20}, {18, 36}, {34, 36} } };

OMO_RaceGoal::OMO_RaceGoal(gfxSprite *nspr, short id) :
	IO_OverMapObject(nspr, 0, 0, 2, 8)
{
	iw = (short)spr->getWidth() >> 1;
	ih = (short)spr->getHeight() >> 1;
	collisionWidth = 36;
	collisionHeight = 36;
	collisionOffsetX = 16;
	collisionOffsetY = 18;

	objectType = object_race_goal;
	state = 1;

	goalID = id;

	for(short k = 0; k < 4; k++)
		tagged[k] = -1;

	angle = (float)(rand()%1000 * 0.00628f);
	anglechange = (float)(rand()%100 * 0.0002f);
	anglechangetimer = (short)(rand() % 50 + 100);

	velx = sin(angle);
	vely = cos(angle);

	placeRaceGoal();

	speed = (float)game_values.gamemodesettings.race.speed / 4.0f;
}

bool OMO_RaceGoal::collide(CPlayer * player)
{
	if(game_values.gamemode->getgamemode() == game_mode_race && player->statue_timer == 0)
	{
		CGM_Race * gamemode = (CGM_Race*)game_values.gamemode;
	
		if(tagged[player->teamID] != player->colorID && gamemode->getNextGoal(player->teamID) >= goalID)
		{
			tagged[player->teamID] = player->colorID;
			
			if(goalID == game_values.gamemodesettings.race.quantity - 1)
                ifsoundonplay(sfx_racesound);
			else
				ifsoundonplay(sfx_areatag);
		}

		if(gamemode->getNextGoal(player->teamID) == goalID)
			gamemode->setNextGoal(player->teamID);
	}
	return false;
}

void OMO_RaceGoal::draw()
{
	if(goalID == game_values.gamemodesettings.race.quantity - 1)
	{
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 54, iw, ih);	
	}
	else
	{
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);	

		for(short k = 0; k < score_cnt; k++)
		{
			if(tagged[k] > -1)
				spr_bonus.draw(ix - collisionOffsetX + flagpositions[score_cnt - 2][k][0], iy - collisionOffsetY + flagpositions[score_cnt - 2][k][1], 0, tagged[k] * 16, 16, 16);
		}
	}

	spr_racetext.draw(ix - collisionOffsetX + 26, iy - collisionOffsetY, (goalID + 1) * 16, 0, 16, 16);
}

void OMO_RaceGoal::update()
{
	if(--anglechangetimer <= 0)
	{
		anglechange = (float)((rand()%101 - 50) * 0.0002f);
		anglechangetimer = (short)(rand() % 50 + 100);
	}
	
	angle += anglechange;

	velx = speed * sin(angle);
	vely = speed * cos(angle);
	
	IO_OverMapObject::update();

	ix = (short)fx;
	iy = (short)fy;

	if(ix < 0)
	{
		velx = -velx;
		ix = 0;
		fx = (float)ix;

		angle = atan2(velx, vely);
	}
	else if(ix + collisionWidth > 639)
	{
		velx = -velx;
		ix = 639 - collisionWidth;
		fx = (float)ix;

		angle = atan2(velx, vely);
	}
	
	if(iy < 0)
	{
		vely = -vely;
		iy = 0;
		fy = (float)iy;

		angle = atan2(velx, vely);
	}
	else if(iy + collisionHeight > 479)
	{
		vely = -vely;
		iy = 479 - collisionHeight;
		fy = (float)iy;

		angle = atan2(velx, vely);
	}
}

void OMO_RaceGoal::placeRaceGoal()
{
	short tries = 0;
	short x = 0, y = 0;
	do
	{
		if(++tries > 32)
			break;

		x = (short)(rand() % (640 - collisionWidth));
		y = (short)(rand() % (480 - collisionHeight));
	}
	while(objectsfront.getClosestObject(x, y, object_race_goal) <= 250.0f - (game_values.gamemodesettings.race.quantity * 25.0f));

	xi(x);
	yi(y);
}

//------------------------------------------------------------------------------
// class frenzycard (for fire frenzy mode)
//------------------------------------------------------------------------------
MO_FrenzyCard::MO_FrenzyCard(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iType) :
	IO_MovingObject(nspr, 0, 0, iNumSpr, aniSpeed, -1, -1, -1, -1, 0, iType * 32, 32, 32)
{
	state = 1;
	objectType = object_frenzycard;
	type = iType;
	
	sparkleanimationtimer = 0;
	sparkledrawframe = 0;

	placeFrenzyCard();
}

bool MO_FrenzyCard::collide(CPlayer * player)
{
	if(type < 8 || game_values.gamemodesettings.frenzy.storedshells)
	{
		//Hack to skip sledge hammer and bombs for now
		if(type > 4)
			type += 2;

		player->SetPowerup(type);
		game_values.gamemode->frenzyowner = player;
	}
	else
	{
		switch(type)
		{
			case 8:
			{
				CO_Shell * shell = new CO_Shell(0, 0, 0, true, true, true, false); 
				objectsplayer.add(shell);
				shell->UsedAsStoredPowerup(player);
				break;
			}
			case 9:
			{
				CO_Shell * shell = new CO_Shell(1, 0, 0, false, true, true, false);
				objectsplayer.add(shell);
				shell->UsedAsStoredPowerup(player);
				break;
			}
			case 10:
			{
				CO_Shell * shell = new CO_Shell(2, 0, 0, false, false, true, true); 
				objectsplayer.add(shell);
				shell->UsedAsStoredPowerup(player);
				break;
			}
			case 11:
			{
				CO_Shell * shell = new CO_Shell(3, 0, 0, false, true, false, false); 
				objectsplayer.add(shell);
				shell->UsedAsStoredPowerup(player);
				break;
			}
		}
	}

	dead = true;
	return false;
}

void MO_FrenzyCard::update()
{
	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	if(++sparkleanimationtimer >= 4)
	{
		sparkleanimationtimer = 0;
		sparkledrawframe += 32;
		if(sparkledrawframe >= 480)
			sparkledrawframe = 0;
	}

	if(++timer > 1500)
		placeFrenzyCard();
}

void MO_FrenzyCard::draw()
{
	IO_MovingObject::draw();
	
	//Draw sparkles
	spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_FrenzyCard::placeFrenzyCard()
{
	timer = 0;

	short tries = 0;
	short x = 0, y = 0;
	do
	{
		if(++tries > 32)
			break;

		g_map.findspawnpoint(1, &x, &y, collisionWidth, collisionHeight, false);
	}
	while(objectsplayer.getClosestObject(x, y, object_frenzycard) <= 150.0f);

	xi(x);
	yi(y);
}

//------------------------------------------------------------------------------
// class explosion (for bob-omb mode)
//------------------------------------------------------------------------------
OMO_Explosion::OMO_Explosion(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short playerid, short iTeamID) :
	IO_OverMapObject(nspr, x, y, iNumSpr, aniSpeed)
{
	state = 1;
	playerID = playerid;
	teamID = iTeamID;
	timer = 0;
	objectType = object_explosion;
}

bool OMO_Explosion::collide(CPlayer * player)
{
	if(player->globalID != playerID && (game_values.friendlyfire || teamID != player->teamID) && !player->invincible && !player->spawninvincible)
	{
		//Find the player that made this explosion so we can attribute a kill
		CPlayer * killer = GetPlayerFromGlobalID(playerID);

		if(killer)
		{
			PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_bobomb);
		}
		else
		{
			player->DeathAwards();

			if(!game_values.gamemode->playerkilledself(*player))
				player->die(0, false);
		}

		return true;
	}

	return false;
}

void OMO_Explosion::update()
{
	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	if(++timer >= 48)
		dead = true;
}

void OMO_Explosion::collide(IO_MovingObject * object)
{
	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_goomba || type == movingobject_koopa || type == movingobject_cheepcheep || type == movingobject_bulletbill || type == movingobject_shell || type == movingobject_throwblock)
	{
		object->dead = true;
		
		if(game_values.gamemode->gamemode == game_mode_stomp && 
			(type == movingobject_goomba || type == movingobject_koopa || type == movingobject_cheepcheep)
			&& !game_values.gamemode->gameover)
		{
			//Attribute the kill
			CPlayer * killer = GetPlayerFromGlobalID(playerID);

			if(killer)
			{
				if(type == movingobject_goomba)
					AddAwardKill(killer, NULL, kill_style_goomba);
				else if(type == movingobject_koopa)
					AddAwardKill(killer, NULL, kill_style_koopa);
				else if(type == movingobject_cheepcheep)
					AddAwardKill(killer, NULL, kill_style_cheepcheep);

				killer->score->AdjustScore(1);
			}
		}

		
		if(type == movingobject_goomba)
		{
			ifsoundonplay(sfx_kicksound);
			((MO_Goomba*)object)->Die();
			game_values.enemyhammerkills = 0;
		}
		else if(type == movingobject_koopa)
		{
			ifsoundonplay(sfx_kicksound);
			((MO_Koopa*)object)->Die();
			game_values.enemyhammerkills = 0;
		}
		else if(type == movingobject_cheepcheep)
		{
			ifsoundonplay(sfx_kicksound);
			((OMO_CheepCheep*)object)->Die();
			game_values.enemyhammerkills = 0;
		}
		else if(type == movingobject_bulletbill)
		{
			ifsoundonplay(sfx_kicksound);
			((OMO_BulletBill*)object)->Die();
		}
		else if(type == movingobject_shell)
		{
			((CO_Shell*)object)->Die();
		}
		else if(type == movingobject_throwblock)
		{
			((CO_ThrowBlock*)object)->Die();
		}
	}
	else if(type == movingobject_sledgebrother)
	{
		((MO_SledgeBrother*)object)->Damage(playerID);
	}
}

//------------------------------------------------------------------------------
// class goomba
//------------------------------------------------------------------------------
MO_Goomba::MO_Goomba(gfxSprite *nspr, short iNumSpr, short aniSpeed, bool moveToRight, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	IO_MovingObject(nspr, 0, 0, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	if(moveToRight)
		velx = 1.0f;
	else
		velx = -1.0f;

	movingObjectType = movingobject_goomba;
	bounce = GRAVITATION;
	
	spawnradius = 100.0f;
	spawnangle = (float)(rand()%1000 * 0.00628f);
	inair = true;

	iSpawnIconOffset = 64;
	killStyle = kill_style_goomba;

	burnuptimer = 0;

	place();
}

void MO_Goomba::draw()
{
	if(state == 0)
	{
		short numeyecandy = 8;
		float addangle = TWO_PI / numeyecandy;
		float displayangle = spawnangle;

		for(short k = 0; k < numeyecandy; k++)
		{
			short spawnX = ix + (collisionWidth >> 1) - 8 + (short)(spawnradius * cos(displayangle));
			short spawnY = iy + (collisionHeight >> 1) - 8 + (short)(spawnradius * sin(displayangle));

			displayangle += addangle;
		
			spr_awardsouls.draw(spawnX, spawnY, iSpawnIconOffset, 0, 16, 16);
		}
	}
	else
	{
		IO_MovingObject::draw();
	}
}

void MO_Goomba::update()
{
	if(state == 0)
	{
		spawnradius -= 2.0f;
		spawnangle += 0.05f;

		if(spawnradius < 10.0f)
			state = 1;
	}
	else
	{
		IO_MovingObject::update();
	}

	//Deal with terminal burnup velocity
	if(vely >= MAXVELY)
	{
		if(++burnuptimer > 20)
		{
			if(burnuptimer > 80)
				KillObjectMapHazard();
			else
				eyecandyback.add(new EC_SingleAnimation(&spr_burnup, ix + (collisionWidth >> 1) - 16, iy + (collisionHeight >> 1) - 16, 5, 4));
		}
	}
	else
	{
		burnuptimer = 0;
	}
}
	

bool MO_Goomba::collide(CPlayer * player)
{
	if(state == 0)
		return false;

	if(player->invincible)
	{
		AddAwardKill(player, NULL, killStyle);

		if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
			player->score->AdjustScore(1);

		ifsoundonplay(sfx_kicksound);
		Die();
		game_values.enemyhammerkills = 0;
	}
	else
	{
		if(player->fOldY + PH <= fOldY && player->iy + PH >= iy)
			return hittop(player);
		else
			return hitother(player);
	}

	return false;
}

bool MO_Goomba::hittop(CPlayer * player)
{
	player->yi(iy - PH - 1);
	player->bouncejump();
	player->collision_detection_checktop();
	player->platform = NULL;
	dead = true;

	AddAwardKill(player, NULL, killStyle);

	if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
		player->score->AdjustScore(1);

	ifsoundonplay(sfx_mip);
	eyecandyback.add(new EC_Corpse(&spr_goombadead, (float)(ix - collisionOffsetX), (float)(iy + collisionHeight - 32), 0));

	return false;
}

bool MO_Goomba::hitother(CPlayer * player)
{
	if(player->spawninvincible)
		return false;

	player->DeathAwards();

	if(!game_values.gamemode->playerkilledself(*player))
		player->die(0, false);

	ifsoundonplay(sfx_deathsound);

	return true;
}

void MO_Goomba::collide(IO_MovingObject * object)
{
	if(state == 0)
		return;

	if(!object->GetDead())
	{
		removeifprojectile(object, false, false);

		MovingObjectType type = object->getMovingObjectType();

		if(type == movingobject_fireball || type == movingobject_superfireball || type == movingobject_hammer || type == movingobject_sledgehammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill || type == movingobject_podobo)
		{
			//Don't kill goombas with non-moving shells
			if(type == movingobject_shell && ((CO_Shell*)object)->state == 2)
				return;

			if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
			{
				short iPlayerID = -1;
				if(type == movingobject_fireball)
				{
					iPlayerID = ((MO_Fireball*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_hammer)
				{
					iPlayerID = ((MO_Hammer*)object)->playerID;
					AddHammerKill(1);
				}
				else if(type == movingobject_sledgehammer)
				{
					iPlayerID = ((MO_SledgeHammer*)object)->playerID;
					AddHammerKill(2);
				}
				else if(type == movingobject_boomerang)
				{
					iPlayerID = ((MO_Boomerang*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_shell)
				{
					iPlayerID = ((CO_Shell*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_throwblock)
				{
					iPlayerID = ((CO_ThrowBlock*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_bulletbill)
				{
					iPlayerID = ((OMO_BulletBill*)object)->iPlayerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_superfireball)
				{
					iPlayerID = ((MO_SuperFireball*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_podobo)
				{
					iPlayerID = ((OMO_Podobo*)object)->iPlayerID;
					game_values.enemyhammerkills = 0;
				}

				//Find the player that shot this fireball so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

				if(killer)
				{
					AddAwardKill(killer, NULL, killStyle);
					killer->score->AdjustScore(1);

					if(type == movingobject_shell)
						((CO_Shell*)object)->AddMovingKill(killer);
				}
			}

			ifsoundonplay(sfx_kicksound);
			Die();
		
			if(type == movingobject_shell)
			{
				CO_Shell * shell = (CO_Shell*)object;
				shell->CheckAndDie();
			}
			else if(type == movingobject_throwblock)
			{
				((CO_ThrowBlock*)object)->Die();
			}
			else if(type == movingobject_bulletbill)
			{
				((OMO_BulletBill*)object)->Die();
			}
		}
	}
}


void MO_Goomba::place()
{
	g_map.findspawnpoint(1, &ix, &iy, collisionWidth, collisionHeight, false);
	fx = (float)ix;
	fy = (float)iy;
}

void MO_Goomba::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(&spr_goombadeadflying, ix, iy, -VELJUMP / 2.0f, 0, 0, 0, 0));
}



//------------------------------------------------------------------------------
// class koopa
//------------------------------------------------------------------------------
MO_Koopa::MO_Koopa(gfxSprite *nspr, short iNumSpr, short aniSpeed, bool moveToRight, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, bool red) :
	MO_Goomba(nspr, iNumSpr, aniSpeed, moveToRight, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	fRed = red;
	movingObjectType = movingobject_koopa;
	iSpawnIconOffset = fRed ? 144 : 112;
	killStyle = kill_style_koopa;
	ih = 54;
}

void MO_Koopa::update()
{
	if(velx < 0.0f)
		animationOffsetY = 54;
	else
		animationOffsetY = 0;

	MO_Goomba::update();
}

bool MO_Koopa::hittop(CPlayer * player)
{
	player->yi(iy - PH - 14);
	player->bouncejump();
	player->collision_detection_checktop();
	player->platform = NULL;
	dead = true;

	AddAwardKill(player, NULL, kill_style_koopa);

	if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
		player->score->AdjustScore(1);

	ifsoundonplay(sfx_mip);

	//Give the shell a state 2 so it is already spawned but sitting
	CO_Shell * shell;
	
	if(fRed)
		shell = new CO_Shell(1, ix - 1, iy + 8, false, true, true, false);
	else
		shell = new CO_Shell(0, ix - 1, iy + 8, true, true, true, false);
	
	shell->state = 2;
	shell->yi(iy + 8);

	objectsplayer.add(shell);
	
	return false;
}

void MO_Koopa::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(&spr_shelldead, ix, iy, -VELJUMP / 2.0f, fRed ? 32 : 0, 0, 32, 32));
}


//------------------------------------------------------------------------------
// class cheep cheep
//------------------------------------------------------------------------------
OMO_CheepCheep::OMO_CheepCheep(gfxSprite *nspr) :
	IO_MovingObject(nspr, 0, 480, 2, 8, 30, 28, 1, 3)
{
	ih = 32;
	xi((short)(rand() % 608));
	
	velx = 0.0f;
	while(velx == 0.0f)
		velx = float(rand() % 19 - 9) / 2.0f;
	
	//Cheep cheep up velocity is between 9.0 and 13.0 in 0.5 increments
	vely = -(float(rand() % 11) / 2.0f) - 9.0f;

	movingObjectType = movingobject_cheepcheep;
	state = 1;

	iColorOffsetY = (short)(rand() % 3) * 64;
	
	if(velx > 0.0f)
		iColorOffsetY += 32;
}

void OMO_CheepCheep::update()
{
	fOldX = fx;
	fOldY = fy;

	xf(fx + velx);
	yf(fy + vely);

	//Cheep cheep gravitation
	vely += 0.2f;

	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}

	//Remove if cheep cheep has fallen below bottom of screen
	if(vely > 0.0f && iy > 480)
		dead = true;
}

void OMO_CheepCheep::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);
}

bool OMO_CheepCheep::collide(CPlayer * player)
{
	if(player->invincible)
	{
		AddAwardKill(player, NULL, kill_style_cheepcheep);
		ifsoundonplay(sfx_kicksound);

		if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
			player->score->AdjustScore(1);
		
		Die();
		game_values.enemyhammerkills = 0;
	}
	else
	{
		if(player->fOldY + PH <= fOldY && player->iy + PH >= iy)
			return hittop(player);
		else
			return hitother(player);
	}

	return false;
}


bool OMO_CheepCheep::hittop(CPlayer * player)
{
	player->yi(iy - PH - 1);
	player->bouncejump();
	player->collision_detection_checktop();
	player->platform = NULL;
	
	AddAwardKill(player, NULL, kill_style_cheepcheep);

	if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
		player->score->AdjustScore(1);

	ifsoundonplay(sfx_mip);
	
	Die();
	game_values.enemyhammerkills = 0;

	return false;
}

bool OMO_CheepCheep::hitother(CPlayer * player)
{
	if(player->spawninvincible)
		return false;

	player->DeathAwards();

	if(!game_values.gamemode->playerkilledself(*player))
		player->die(0, false);

	ifsoundonplay(sfx_deathsound);

	return true;
}

void OMO_CheepCheep::collide(IO_MovingObject * object)
{
	if(!object->GetDead())
	{
		removeifprojectile(object, false, false);

		MovingObjectType type = object->getMovingObjectType();

		if(type == movingobject_fireball || type == movingobject_superfireball || type == movingobject_hammer || type == movingobject_sledgehammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill || type == movingobject_podobo)
		{
			//Don't kill goombas with non-moving shells
			if(type == movingobject_shell && ((CO_Shell*)object)->state == 2)
				return;

			if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
			{
				short iPlayerID = -1;
				if(type == movingobject_fireball)
				{
					iPlayerID = ((MO_Fireball*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_hammer)
				{
					iPlayerID = ((MO_Hammer*)object)->playerID;
					AddHammerKill(1);
				}
				else if(type == movingobject_sledgehammer)
				{
					iPlayerID = ((MO_SledgeHammer*)object)->playerID;
					AddHammerKill(2);
				}
				else if(type == movingobject_boomerang)
				{
					iPlayerID = ((MO_Boomerang*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_shell)
				{
					iPlayerID = ((CO_Shell*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_throwblock)
				{
					iPlayerID = ((CO_ThrowBlock*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_bulletbill)
				{
					iPlayerID = ((OMO_BulletBill*)object)->iPlayerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_superfireball)
				{
					iPlayerID = ((MO_SuperFireball*)object)->playerID;
					game_values.enemyhammerkills = 0;
				}
				else if(type == movingobject_podobo)
				{
					iPlayerID = ((OMO_Podobo*)object)->iPlayerID;
					game_values.enemyhammerkills = 0;
				}

				//Find the player that shot this projectile so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

				if(killer)
				{
					AddAwardKill(killer, NULL, kill_style_cheepcheep);
					killer->score->AdjustScore(1);

					if(type == movingobject_shell)
						((CO_Shell*)object)->AddMovingKill(killer);
				}
			}

			ifsoundonplay(sfx_kicksound);
			Die();
		
			if(type == movingobject_shell)
			{
				CO_Shell * shell = (CO_Shell*)object;
				shell->CheckAndDie();
			}
			else if(type == movingobject_throwblock)
			{
				((CO_ThrowBlock*)object)->Die();
			}
			else if(type == movingobject_bulletbill)
			{
				((OMO_BulletBill*)object)->Die();
			}
		}
	}
}

void OMO_CheepCheep::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(&spr_cheepcheepdead, ix, iy, -VELJUMP / 2.0f, 0, iColorOffsetY, 32, 32));
}

//------------------------------------------------------------------------------
// class goomba
//------------------------------------------------------------------------------
MO_SledgeBrother::MO_SledgeBrother(gfxSprite *nspr, short platformY, short type) :
	IO_MovingObject(nspr, 0, 0, 8, 0, 32, 56, 8, 8)
{
	iType = type;
	state = 1;
	iActionState = 0;
	location = 2;

	ih = spr->getHeight() / 3;

	movingObjectType = movingobject_sledgebrother;
	bounce = GRAVITATION;
	
	inair = true;

	throwing_timer = 0;
	
	hit_timer = 0;
	hit_movement_timer = 0;
	hit_offset_y = iType * 64;

	leg_offset_x = 0;
	leg_movement_timer = 0;

	arm_offset_x = 0;
	arm_movement_timer = 0;

	taunt_timer = 0;

	wait_timer = 0;

	hit_points = rand() % 4 + 5;
	face_right = false;

	vely = 0.0f;
	velx = 0.0f;
	
	iPlatformY = platformY - collisionHeight;
	yi(iPlatformY);

	for(short iLocation = 0; iLocation < 5; iLocation++)
		iDestLocationX[iLocation] = 84 * iLocation + 128;

	iDestX = iDestLocationX[location];
	xi(iDestX);

	need_attack = 0;
}

void MO_SledgeBrother::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, leg_offset_x + arm_offset_x + (face_right ? 0 : 192), hit_offset_y, iw, ih);
}

void MO_SledgeBrother::update()
{
	if(iActionState == 0)
	{
		randomaction();
	}
	else if(iActionState == 1)
	{
		if(--wait_timer <= 0)
			iActionState = 0;
	}
	else if(iActionState == 2)
	{
		yf(fy + vely);
		vely += GRAVITATION;

		if(iy >= iPlatformY)
		{
			iActionState = 0;
			
			if(iType == 0)
			{
				//Shake screen and kill players
				ifsoundonplay(sfx_thunder);
				game_values.screenshaketimer = 20;
				game_values.screenshakeplayerid = -1;
				game_values.screenshaketeamid = -1;
				game_values.screenshakekillinair = false;
				game_values.screenshakekillscount = 0;
			}
			else if(iType == 1)
			{
				//Spawn thwomps
				ifsoundonplay(sfx_thunder);

				short numThwomps = rand() % 5 + 6;

				for(short iThwomp = 0; iThwomp < numThwomps; iThwomp++)
				{
					objectsfront.add(new OMO_Thwomp(&spr_thwomp, (short)(rand() % 591), 2.0f + (float)(rand()%20)/10.0f));
				}

				/*
				short numBombs = rand() % 5 + 6;

				for(short iBomb = 0; iBomb < numBombs; iBomb++)
				{
					short iRandomX = rand() % 612;
					short iRandomY = rand() % 442;
					eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, iRandomX - 2, iRandomY + 3, 3, 8));
					objectsfront.add(new CO_Bomb(&spr_bomb, iRandomX, iRandomY, 0.0f, 0.0f, 4, -1, -1, -1, rand() % 30 + 30));
				}
				*/
			}
			else if(iType == 2)
			{
				//Spawn lots of podobos
				ifsoundonplay(sfx_thunder);

				short numPodobos = rand() % 5 + 8;

				for(short iPodobo = 0; iPodobo < numPodobos; iPodobo++)
				{
					objectsfront.add(new OMO_Podobo(&spr_podobo, (short)(rand() % 608), -(float(rand() % 9) / 2.0f) - 9.0f, -1, -1, -1));
				}
			}
		}
	}
	else if(iActionState == 3)
	{
		if(--throwing_timer <= 0)
		{
			iActionState = 0;
			throwing_timer = 0;
			arm_offset_x = 0;
		}
	}
	else if(iActionState == 4)
	{
		//move towards destination
		if(ix < iDestX)
		{
			ix++;

			if(ix >= iDestX)
			{
				ix = iDestX;
				iActionState = 0;
				leg_offset_x = 0;
			}
		}
		else if(ix > iDestX)
		{
			ix--;

			if(ix <= iDestX)
			{
				ix = iDestX;
				iActionState = 0;
				leg_offset_x = 0;
			}
		}

		if(++leg_movement_timer == 8)
		{
			leg_movement_timer = 0;
			
			if(leg_offset_x == 0)
				leg_offset_x = 48;
			else
				leg_offset_x = 0;
		}
	}
	else if(iActionState == 5)
	{
		if(--taunt_timer <= 0)
		{
			iActionState = 0;
			arm_offset_x = 0;
			leg_offset_x = 0;
		}

		if(++arm_movement_timer == 8)
		{
			arm_movement_timer = 0;
			
			if(arm_offset_x == 0)
				arm_offset_x = 96;
			else
				arm_offset_x = 0;
		}

		if(++leg_movement_timer == 6)
		{
			leg_movement_timer = 0;
			
			if(leg_offset_x == 0)
				leg_offset_x = 48;
			else
				leg_offset_x = 0;
		}
	}

	if(hit_timer > 0)
	{
		if(--hit_timer <= 0)
		{
			hit_offset_y = iType * 64;
			hit_timer = 0;
		}
		else
		{
			if(++hit_movement_timer == 2)
			{
				hit_movement_timer = 0;
				
				hit_offset_y += 64;

				if(hit_offset_y >= 192)
					hit_offset_y = 0;
			}
		}
	}
}

void MO_SledgeBrother::randomaction()
{
	int randaction = rand() % 20;

	if(iType == 0)
	{
		if(randaction < 1 || need_attack >= 8)
			jump();
		else if(randaction < 12 || need_attack >= 5)
			throwprojectile();
		else if(randaction < 14 || need_attack >= 3)
			turn();
		else if(randaction < 16)
			wait(20, 40);
		else if(randaction < 18)
			taunt();
		else
		{
			if(location == 0)
				move(true);
			else if(location == 4)
				move(false);
			else
				move(rand() % 2 == 0);
		}
	}
	else if(iType == 1)
	{
		if(randaction < 1)
			jump();
		else if(randaction < 6 || need_attack >= 7)
			throwprojectile();
		else if(randaction < 12 || need_attack >= 3)
			turn();
		else if(randaction < 14)
			taunt();
		else
		{
			if(location == 0)
				move(true);
			else if(location == 4)
				move(false);
			else
				move(rand() % 2 == 0);
		}
	}
	else if(iType == 2)
	{
		if(randaction < 1 || need_attack >= 8)
			jump();
		else if(randaction < 12 || need_attack >= 5)
			throwprojectile();
		else if(randaction < 16 || need_attack >= 3)
			turn();
		else if(randaction < 17)
			taunt();
		else
		{
			if(location == 0)
				move(true);
			else if(location == 4)
				move(false);
			else
				move(rand() % 2 == 0);
		}
	}
}

void MO_SledgeBrother::move(bool moveright)
{
	if(moveright)
		location++;
	else
		location--;

	iDestX = iDestLocationX[location];
	iActionState = 4;

	if(iDestX > ix)
		face_right = true;
	else
		face_right = false;

	need_attack += 5;
}

void MO_SledgeBrother::throwprojectile()
{
	throwing_timer = 20;
	iActionState = 3;
	arm_offset_x = 96;

	if(iType == 0)
	{
		float fHammerVelX = ((float)(rand() % 9 + 2)) / 2.0f - (face_right ? 0.0f : 6.0f);
		objectsfront.add(new MO_SledgeHammer(&spr_sledgehammer, (face_right ? ix + 32 : ix) - collisionOffsetX, iy, 8, fHammerVelX, -HAMMERTHROW, 5, -1, -1, -1, false));
	}
	else if(iType == 1)
	{
		float fBombVelX = ((float)(rand() % 5 + 12)) / 2.0f - (face_right ? 0.0f : 14.0f);
		float fBombVelY = -(float)(rand() % 13) / 2.0f - 6.0f;
		objectsfront.add(new CO_Bomb(&spr_bomb, face_right ? ix + iw - 32 : ix - 20, iy, fBombVelX, fBombVelY, 4, -1, -1, -1, rand() % 60 + 120));
	}
	else if(iType == 2)
	{
		float fFireVelX = ((float)(rand() % 9 + 6)) / 2.0f - (face_right ? 0.0f : 10.0f);
		float fFireVelY = (float)(rand() % 17) / 2.0f - 4.0f;
		objectsfront.add(new MO_SuperFireball(&spr_superfireball, face_right ? ix + iw - 32 : ix - 16, iy, 4, fFireVelX, fFireVelY, 4, -1, -1, -1));
	}

	need_attack = 0;
}

void MO_SledgeBrother::taunt()
{
	ifsoundonplayloop(sfx_boomerang, 3);
	taunt_timer = 60;
	iActionState = 5;

	need_attack += 5;
}

void MO_SledgeBrother::turn()
{
	face_right = !face_right;
	wait(10, 30);

	need_attack += 2;
}

void MO_SledgeBrother::jump()
{
	vely = -VELJUMP;
	iActionState = 2;

	need_attack = 0;
}

void MO_SledgeBrother::wait(short min, short max)
{
	wait_timer = rand() % (max - min) + min;
	iActionState = 1;

	need_attack += 3;
}

bool MO_SledgeBrother::collide(CPlayer * player)
{
	if(iActionState == 0)
		return false;

	if(player->invincible)
	{
		ifsoundonplay(sfx_kicksound);
		Die();

		if(game_values.gamemode->gamemode == game_mode_boss)
		{
			((CGM_Boss*)game_values.gamemode)->SetWinner(player);
		}
	}
	else
	{
		return hit(player);
	}

	return false;
}

bool MO_SledgeBrother::hit(CPlayer * player)
{
	if(player->spawninvincible)
		return false;

	player->DeathAwards();

	if(!game_values.gamemode->playerkilledself(*player))
		player->die(0, false);

	ifsoundonplay(sfx_deathsound);

	return true;
}

void MO_SledgeBrother::collide(IO_MovingObject * object)
{
	if(!object->GetDead())
	{
		//Ignore hammers thrown from sledge brother
		if(object->objectType == object_moving)
		{
			if(((IO_MovingObject*)object)->movingObjectType == movingobject_sledgehammer)
			{
				if(((MO_SledgeHammer*)object)->playerID == -1)
					return;
			}

			if(((IO_MovingObject*)object)->movingObjectType == movingobject_superfireball)
			{
				if(((MO_SuperFireball*)object)->playerID == -1)
					return;
			}
		}

		removeifprojectile(object, false, false);

		MovingObjectType type = object->getMovingObjectType();

		if(iType == 0 && (type == movingobject_shell || type == movingobject_throwblock))
		{
			if(type == movingobject_shell && ((CO_Shell*)object)->state == 2)
				return;

			short iPlayerID = -1;
			if(type == movingobject_shell)
				iPlayerID = ((CO_Shell*)object)->playerID;
			else if(type == movingobject_throwblock)
				iPlayerID = ((CO_ThrowBlock*)object)->playerID;

			Damage(iPlayerID);

			ifsoundonplay(sfx_kicksound);
		
			if(type == movingobject_shell)
			{
				((CO_Shell*)object)->Die();
			}
			else if(type == movingobject_throwblock)
			{
				((CO_ThrowBlock*)object)->Die();
			}
		}
	}
}

void MO_SledgeBrother::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(&spr_sledgebrothersdead, ix, iy, -VELJUMP / 2.0f, 0, iType * 64, iw, ih));
}

void MO_SledgeBrother::Damage(short playerID)
{
	if(hit_timer != 0 || playerID == -1)
		return;

	//Find the player that shot this projectile so we can attribute a kill
	CPlayer * killer = GetPlayerFromGlobalID(playerID);

	if(killer)
	{
		if(--hit_points <= 0)
		{
			Die();

			if(game_values.gamemode->gamemode == game_mode_boss)
			{
				((CGM_Boss*)game_values.gamemode)->SetWinner(killer);
			}
		}
		else
		{
			hit_timer = 60;
			ifsoundonplay(sfx_stun);
		}
	}
}


//------------------------------------------------------------------------------
// class shell projectile
//------------------------------------------------------------------------------
//state 1: Shell is moving
//state 2: Shell is waiting to be picked up
//state 3: Shell is being held
CO_Shell::CO_Shell(short type, short x, short y, bool dieOnMovingPlayerCollision, bool dieOnHoldingPlayerCollision, bool dieOnFire, bool killBouncePlayer) :
	MO_CarriedObject(&spr_shell, x, y, 4, 4, 30, 20, 1, 11, 0, type * 32, 32, 32)
{
	iShellType = type;

	state = 0;
	bounce = GRAVITATION;
	objectType = object_moving;
	movingObjectType = movingobject_shell;
	playerID = -1;
	iIgnoreBounceTimer = 0;
	iBounceCounter = 0;

	fDieOnMovingPlayerCollision = dieOnMovingPlayerCollision;
	fDieOnHoldingPlayerCollision = dieOnHoldingPlayerCollision;
	fDieOnFire = dieOnFire;
	fKillBouncePlayer = killBouncePlayer;

	iDeathTime = 0;

	iDestY = iy - collisionHeight;
	fy = iDestY + 32.0f;
	iColorOffsetY = type * 32;

	iKillCounter = 0;
}

bool CO_Shell::collide(CPlayer * player)
{
	if(player->invincible)
	{
		if(state == 0 || state == 2)
		{
			Die();
			return false;
		}
		else if(state == 3)
		{
			if(owner != player)
			{
				Die();
				return false;
			}
		}
		else if(state == 1)
		{
			short flipx = 0;

			if(player->ix + PW < 320 && ix > 320)
				flipx = 640;
			else if(ix + iw < 320 && player->ix > 320)
				flipx = -640;

			if((player->ix + HALFPW + flipx >= ix + (iw >> 1) && velx > 0.0f) || (player->ix + HALFPW + flipx < ix + (iw >> 1) && velx < 0.0f))
			{
				Die();
				return false;	
			}
		}
	}

	if(player->statue_timer == 0)
	{
		if(player->fOldY + PH <= iy && player->iy + PH >= iy)
			return HitTop(player);
		else
			return HitOther(player);
	}

	return false;
}

bool CO_Shell::HitTop(CPlayer * player)
{
	if(player->invincible)
	{
		Die();
		fSmoking = false;
		return false;
	}

	if(fKillBouncePlayer)
	{
		KillPlayer(player);
	}
	else if(state == 2) //Sitting
	{
		owner = player;
		Kick(false);
		fSmoking = false;
		if(player->ix + HALFPW < ix + (iw >> 1))
			velx = 5.0f;
		else
			velx = -5.0f;

		iIgnoreBounceTimer = 10;
	}
	else if(state == 1 && iIgnoreBounceTimer == 0)  //Moving
	{
		owner = NULL;
		velx = 0.0f;
		state = 2;
		fSmoking = false;
		ifsoundonplay(sfx_kicksound);

		player->yi(iy - PH - 1);
		player->bouncejump();
		player->collision_detection_checktop();
		player->platform = NULL;
		iKillCounter = 0;
	}
	else if(state == 3) //Holding
	{
		if(player != owner && (game_values.friendlyfire || player->teamID != owner->teamID))
		{
			if(owner)
				owner->carriedItem = NULL;

			Kick(false);
			fSmoking = false;

			player->yi(iy - PH - 1);
			player->bouncejump();
			player->collision_detection_checktop();
			player->platform = NULL;
		}
	}

	return false;
}

bool CO_Shell::HitOther(CPlayer * player)
{
	if(state == 2) //Sitting
	{
		if(owner == NULL)
		{
			if(player->AcceptItem(this))
			{
				owner = player;
				playerID = owner->globalID;
				state = 3;
			}
			else
			{
				short flipx = 0;

				if(player->ix + PW < 320 && ix > 320)
					flipx = 640;
				else if(ix + iw < 320 && player->ix > 320)
					flipx = -640;

				owner = player;
				Kick(false);
				if(player->ix + HALFPW + flipx < ix + (iw >> 1))
					velx = 5.0f;
				else
					velx = -5.0f;
			}
		}
	}
	else if(state == 1)  //Moving
	{
		short flipx = 0;

		if(player->ix + PW < 320 && ix > 320)
			flipx = 640;
		else if(ix + iw < 320 && player->ix > 320)
			flipx = -640;

		if((player->ix + HALFPW + flipx >= ix + (iw >> 1) && velx > 0.0f) || (player->ix + HALFPW + flipx < ix + (iw >> 1) && velx < 0.0f))
		{
			return KillPlayer(player);
		}
	}
	else if(state == 3)  //Holding
	{
		if(player != owner && (game_values.friendlyfire || player->teamID != owner->teamID))
		{
			playerID = owner->globalID;
			return KillPlayer(player);
		}
	}

	return false;
}

void CO_Shell::UsedAsStoredPowerup(CPlayer * player)
{
	owner = player;
	MoveToOwner();

	if(player->AcceptItem(this))
		state = 3;
	else
		Kick(false);
}

void CO_Shell::MoveToOwner()
{
	if(owner)
	{
		xi(owner->ix + (owner->IsPlayerFacingRight() ? HALFPW: HALFPW - 32));
		yi(owner->iy + PH - 32 + collisionOffsetY);
	}
}

bool CO_Shell::KillPlayer(CPlayer * player)
{
	if(player->spawninvincible || player->invincible)
		return false;

	CheckAndDie();

	//Find the player that shot this shell so we can attribute a kill
	CPlayer * killer = GetPlayerFromGlobalID(playerID);

	if(killer && killer->globalID != player->globalID)
	{
		PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_shell);
		AddMovingKill(killer);
	}
	else
	{
		player->DeathAwards();

		if(!game_values.gamemode->playerkilledself(*player))
			player->die(0, false);

		ifsoundonplay(sfx_deathsound);
	}

	return true;
}

void CO_Shell::AddMovingKill(CPlayer * killer)
{
	if(state == 1 && game_values.awardstyle != award_style_none) //If the shell is moving, the keep track of how many people we kill in a row with it
	{
		if(++iKillCounter > 1)
			killer->AddKillsInRowInAirAward();
	}
}

void CO_Shell::collide(IO_MovingObject * object)
{
	if(!object->GetDead())
	{
		//Don't allow shells to die if they are warping
		if(owner && owner->iswarping())
			return;

		removeifprojectile(object, false, false);

		MovingObjectType type = object->getMovingObjectType();

		if(type == movingobject_fireball || type == movingobject_superfireball || type == movingobject_hammer || type == movingobject_sledgehammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock)
		{
			if(type == movingobject_shell)
			{
				CO_Shell * shell = (CO_Shell*)object;

				//Green shells should die on collision, other shells should not,
				//except if they also hit a non dead on collision shell
				if(fDieOnMovingPlayerCollision || state == 2 || (!shell->fDieOnMovingPlayerCollision && shell->state != 2))
					Die();

				if(shell->fDieOnMovingPlayerCollision || shell->state == 2 || (!fDieOnMovingPlayerCollision && state != 2))
					shell->Die();
			}
			else if(type == movingobject_throwblock)
			{
				Die();
				((CO_ThrowBlock*)object)->Die();
			}
			else
			{
				if(fDieOnFire)
					Die();
			}
		}
	}
}

void CO_Shell::update()
{
	if(state == 1)
	{
		if(game_values.shellttl > 0 && ++iDeathTime >= game_values.shellttl)
		{
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix, iy, 3, 8));
			dead = true;
			ifsoundonplay(sfx_kicksound);

			if(owner)
			{
				owner->carriedItem = NULL;
				owner = NULL;
			}

			return;
		}
	}
	else
		iDeathTime = 0;
	
	//Have the powerup grow out of the powerup block
	if(state == 0)
	{
		yf(fy - 2.0f);

		if(fy <= iDestY)
		{
			state = 2;
			vely = GRAVITATION;
			yf(iDestY);
		}

		return;
	}

	if(iIgnoreBounceTimer > 0)
		iIgnoreBounceTimer--;
	
	if(iBounceCounter > 0)
		iBounceCounter--;

	if(owner)
	{
		MoveToOwner();
		inair = true;
	}
	else
	{
		fOldX = fx;
		fOldY = fy;

		collision_detection_map();
		
		if(++animationtimer == animationspeed)
		{
			animationtimer = 0;
			drawframe += iw;
			if(drawframe >= animationWidth)
				drawframe = 0;
		}
	}
}

void CO_Shell::draw()
{
	if(state == 0)
	{
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY, iw, (short)(ih - fy + iDestY));
	}
	else if(owner)
	{
		if(owner->iswarping())
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY, iw, ih);
	}
	else
	{
		if(state == 2)
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY, iw, ih);
		else if(state == 1)
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);
	}

	if(fSmoking)
	{
		eyecandyback.add(new EC_SingleAnimation(&spr_burnup, ix - collisionOffsetX + (iw >> 1) - 16, iy - collisionOffsetY + (ih >> 1) - 16, 5, 3));
	}
}

void CO_Shell::Drop()
{
	if(owner)
	{
		owner->carriedItem = NULL;
		xi(owner->ix + (owner->IsPlayerFacingRight() ? PW + 1: -33));
	}

	if(collision_detection_checksides())
	{
		//Move back to where it was before checking sides, then kill it
		xi(owner->ix + (owner->IsPlayerFacingRight() ? PW + 1: -33));
		yi(owner->iy + PH - 32 + collisionOffsetY);
		Die();
	}
	else
	{
		owner = NULL;
		state = 2;
	}
}

void CO_Shell::Kick(bool superkick)
{
	float vel = 5.0f;
	if(superkick)
	{
		vel = 10.0f;
		fSmoking = true;
		ifsoundonplay(sfx_cannon);
	}

	velx = owner->IsPlayerFacingRight() ? vel : -vel;
	vely = 0.0f;

	playerID = owner->globalID;

	owner = NULL;

	state = 1;
	
	if(collision_detection_checksides())
		Die();
	else
		ifsoundonplay(sfx_kicksound);
}

void CO_Shell::CheckAndDie()
{
	if((fDieOnMovingPlayerCollision && state == 1) || (fDieOnHoldingPlayerCollision && state == 3))
		Die();
	else if(!fDieOnHoldingPlayerCollision && state == 3 && (rand() % 5) == 0)
		Die();
}

void CO_Shell::Die()
{
	eyecandyfront.add(new EC_FallingObject(&spr_shelldead, ix, iy, -velx / 4.0f, -VELJUMP / 2.0f, 1, 0, iShellType * 32, 0, 32, 32));
	dead = true;
	ifsoundonplay(sfx_kicksound);
	iKillCounter = 0;

	if(owner)
	{
		owner->carriedItem = NULL;
		owner = NULL;
	}
}

void CO_Shell::SideBounce()
{
	if(state == 1)
	{
		if(iBounceCounter == 0)
		{
			eyecandyfront.add(new EC_SingleAnimation(&spr_shellbounce, ix + (velx > 0 ? 0 : collisionWidth) - 21, iy + (collisionHeight >> 1) - 20, 4, 4));
			ifsoundonplay(sfx_bump);

			iBounceCounter = 7; //Allow bounce stars to show on each bounce on a 2x wide pit
		}
	}
}



//------------------------------------------------------------------------------
// class throwable block projectile
//------------------------------------------------------------------------------
//State 1: Moving
//State 2: Holding
CO_ThrowBlock::CO_ThrowBlock(gfxSprite * nspr, short x, short y, bool superblock) :
	MO_CarriedObject(nspr, x, y, 4, 2, 30, 30, 1, 1)
{
	state = 2;
	ih = nspr->getHeight() >> 1;
	bounce = GRAVITATION;
	objectType = object_moving;
	movingObjectType = movingobject_throwblock;
	playerID = -1;
	fSuper = superblock;

	iDeathTime = 0;
	iBounceCounter = 0;
}

bool CO_ThrowBlock::collide(CPlayer * player)
{
	if(player->fOldY + PH <= iy && player->iy + PH >= iy)
		return HitTop(player);
	else
		return HitOther(player);
}

bool CO_ThrowBlock::HitTop(CPlayer * player)
{
	if(player->invincible)
	{
		Die();
	}
	else
	{
		if(state == 1) //moving
		{
			return KillPlayer(player);
		}
		else if(state == 2) //Holding
		{
			if(player != owner)
			{
				if(owner)
					owner->carriedItem = NULL;

				Kick(false);

				player->yi(iy - PH - 1);
				player->bouncejump();
				player->collision_detection_checktop();
				player->platform = NULL;
			}
		}
	}

	return false;
}

bool CO_ThrowBlock::HitOther(CPlayer * player)
{
	if(state == 1)  //Moving
	{
		short flipx = 0;

		if(player->ix + PW < 320 && ix > 320)
			flipx = 640;
		else if(ix + iw < 320 && player->ix > 320)
			flipx = -640;

		if((player->ix + flipx > ix && velx > 0.0f) || (player->ix + flipx <= ix && velx < 0.0f))
		{
			return KillPlayer(player);
		}
	}
	else if(state == 2)  //Holding
	{
		if(player != owner)
		{
			playerID = owner->globalID;
			return KillPlayer(player);
		}
	}

	return false;
}

void CO_ThrowBlock::MoveToOwner()
{
	if(owner)
	{
		xi(owner->ix + (owner->IsPlayerFacingRight() ? 14 : -22));
		yi(owner->iy + PH - 32 + collisionOffsetY);
	}
}


bool CO_ThrowBlock::KillPlayer(CPlayer * player)
{
	if(player->invincible)
	{
		Die();
		return false;
	}

	if(player->spawninvincible)
		return false;

	if(!fSuper)
		Die();

	//Find the player that shot this shell so we can attribute a kill
	CPlayer * killer = GetPlayerFromGlobalID(playerID);

	if(killer && killer->globalID != player->globalID)
	{
		PlayerKilledPlayer(*killer, *player, death_style_jump, kill_style_throwblock);
	}
	else
	{
		player->DeathAwards();

		if(!game_values.gamemode->playerkilledself(*player))
			player->die(0, false);

		ifsoundonplay(sfx_deathsound);
	}

	return true;
}

void CO_ThrowBlock::collide(IO_MovingObject * object)
{
	if(!object->GetDead())
	{
		removeifprojectile(object, false, false);

		if(object->getMovingObjectType() == movingobject_shell)
		{
			Die();
			((CO_Shell*)object)->Die();
		}
		else if(object->getMovingObjectType() == movingobject_throwblock)
		{
			Die();
			((CO_ThrowBlock*)object)->Die();
		}
	}
}

void CO_ThrowBlock::update()
{
	if(game_values.blueblockttl > 0 && ++iDeathTime >= game_values.blueblockttl)
	{
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix, iy, 3, 8));
		dead = true;

		if(owner)
		{
			owner->carriedItem = NULL;
			owner = NULL;
		}

		return;
	}
	
	if(owner)
	{
		MoveToOwner();
		inair = true;
	}
	else
	{
		if(iBounceCounter > 0)
			iBounceCounter--;

		fOldX = fx;
		fOldY = fy;

		collision_detection_map();
	}

	if(++animationtimer == animationspeed)
	{
		animationtimer = 0;
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = 0;
	}
}

void CO_ThrowBlock::draw()
{
	if(owner && owner->iswarping())
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, fSuper ? 32 : 0, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
	else
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, fSuper ? 32 : 0, iw, ih);

	if(fSmoking)
	{
		eyecandyback.add(new EC_SingleAnimation(&spr_burnup, ix - collisionOffsetX + (iw >> 1) - 16, iy - collisionOffsetY + (ih >> 1) - 16, 5, 3));
	}
}

void CO_ThrowBlock::Drop()
{
	Kick(false);
}

void CO_ThrowBlock::Kick(bool superkick)
{
	float vel = 6.5f;
	if(superkick)
	{
		vel = 12.0f;
		fSmoking = true;
		ifsoundonplay(sfx_cannon);
	}

	iDeathTime = 0;

	velx = owner->IsPlayerFacingRight() ? vel : -vel;
	vely = 0.0f;

	playerID = owner->globalID;

	owner = NULL;

	state = 1;
	ifsoundonplay(sfx_kicksound);

	collision_detection_checksides();
}

void CO_ThrowBlock::Die()
{
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy, -1.5f, -7.0f, 6, 2, 0, fSuper ? 16 : 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy, 1.5f, -7.0f, 6, 2, 0, fSuper ? 16 : 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy + 16, -1.5f, -4.0f, 6, 2, 0, fSuper ? 16 : 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy + 16, 1.5f, -4.0f, 6, 2, 0, fSuper ? 16 : 0, 16, 16));
	
	dead = true;
	ifsoundonplay(sfx_breakblock);

	if(owner)
	{
		owner->carriedItem = NULL;
		owner = NULL;
	}
}

void CO_ThrowBlock::SideBounce()
{
	if(!fSuper)
	{
		Die();
	}
	else if(state == 1)
	{
		if(iBounceCounter == 0)
		{
			eyecandyfront.add(new EC_SingleAnimation(&spr_shellbounce, ix + (velx > 0 ? 0 : collisionWidth) - 21, iy + (collisionHeight >> 1) - 20, 4, 4));
			ifsoundonplay(sfx_bump);

			iBounceCounter = 7; //Allow bounce stars to show on each bounce on a 2x wide pit
		}
	}
}

//------------------------------------------------------------------------------
// class spring
//------------------------------------------------------------------------------
CO_Spring::CO_Spring(gfxSprite *nspr) :
	MO_CarriedObject(nspr, 0, 0, 4, 4, 30, 30, 1, 1)
{
	state = 0;
	bounce = GRAVITATION;
	objectType = object_moving;
	movingObjectType = movingobject_spring;
	
	spawnradius = 100.0f;
	spawnangle = (float)(rand()%1000 * 0.00628f);

	iSpawnIconX = 160;

	place();
}

bool CO_Spring::collide(CPlayer * player)
{
	if(state == 1 && owner == NULL)
	{
		if(player->fOldY + PH <= fOldY && player->iy + PH >= iy)
			hittop(player);
		else
			hitother(player);
	}
	
	return false;
}

void CO_Spring::hittop(CPlayer * player)
{
	state = 2;
	drawframe += iw;

	player->yi(iy - PH - 1);
	player->collision_detection_checktop();
	player->platform = NULL;
	player->inair = false;
	player->fallthrough = false;
	player->killsinrowinair = 0;
	player->featherjump = 0;

	player->superjumptimer = 4;
	player->vely = -VELNOTEBLOCKREPEL;
	
	ifsoundonplay(sfx_bump);
}

void CO_Spring::hitother(CPlayer * player)
{
	if(player->AcceptItem(this))
	{
		owner = player;
	}
}

void CO_Spring::update()
{
	if(state == 0)
	{
		spawnradius -= 2.0f;
		spawnangle += 0.05f;

		if(spawnradius < 10.0f)
			state = 1;
	}
	else
	{
		if(owner)
		{
			MoveToOwner();
		}
		else
		{
			//Add air/ground friction
			if(velx > 0.0f)
			{
				if(inair)
					velx -= VELAIRFRICTION;
				else if(onice)
					velx -= VELICEFRICTION;
				else
					velx -= VELMOVINGFRICTION;

				if(velx < 0.0f)
					velx = 0.0f;
			}
			else if(velx < 0.0f)
			{
				if(inair)
					velx += VELAIRFRICTION;
				else if(onice)
					velx += VELICEFRICTION;
				else
					velx += VELMOVINGFRICTION;

				if(velx > 0.0f)
					velx = 0.0f;
			}

			//Collision detect map
			fOldX = fx;
			fOldY = fy;

			collision_detection_map();
		}

		if(state == 2)
		{
			if(++animationtimer == animationspeed)
			{
				animationtimer = 0;
				
				drawframe += iw;
				if(drawframe >= animationWidth)
				{
					drawframe = 0;
					state = 1;
				}
			}
		}
	}
}

void CO_Spring::draw()
{
	if(state == 0)
	{
		short numeyecandy = 8;
		float addangle = TWO_PI / numeyecandy;
		float displayangle = spawnangle;

		for(short k = 0; k < numeyecandy; k++)
		{
			short spawnX = ix + (collisionWidth >> 1) - 8 + (short)(spawnradius * cos(displayangle));
			short spawnY = iy + (collisionHeight >> 1) - 8 + (short)(spawnradius * sin(displayangle));

			displayangle += addangle;
		
			spr_awardsouls.draw(spawnX, spawnY, iSpawnIconX, 0, 16, 16);
		}
	}
	else
	{
		if(owner)
		{
			if(owner->iswarping())
				spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
			else
				spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, iw, ih);
		}
		else
		{
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);
		}
	}
}

void CO_Spring::MoveToOwner()
{
	if(owner)
	{
		xi(owner->ix + (owner->IsPlayerFacingRight() ? 14 : -22));
		yi(owner->iy + PH - 32 + collisionOffsetY);
	}
}

void CO_Spring::place()
{
	g_map.findspawnpoint(1, &ix, &iy, collisionWidth, collisionHeight, false);
	fx = (float)ix;
	fy = (float)iy;

	Drop();
}

void CO_Spring::Drop()
{
	if(owner)
		owner->carriedItem = NULL;

	owner = NULL;

	collision_detection_checksides();
}

void CO_Spring::Kick(bool superkick)
{
	velx = owner->velx + (owner->IsPlayerFacingRight() ? 2.0f : -2.0f);
	vely = -4.0f;
	ifsoundonplay(sfx_kicksound);

	Drop();
}

//------------------------------------------------------------------------------
// class spike
//------------------------------------------------------------------------------
CO_Spike::CO_Spike(gfxSprite *nspr) :
	CO_Spring(nspr)
{
	iw = 32;
	ih = 32;

	movingObjectType = movingobject_spike;

	iSpawnIconX = 176;
}

void CO_Spike::hittop(CPlayer * player)
{
	if(player->isready() && !player->spawninvincible && !player->invincible)
		player->KillPlayerMapHazard();
}

//------------------------------------------------------------------------------
// class object_container
//------------------------------------------------------------------------------
CObjectContainer::CObjectContainer()
{
	for(short i = 0; i < MAXOBJECTS; i++)
		list[i] = NULL;

	list_end = 0;
}


CObjectContainer::~CObjectContainer()
{
	clean();
}


void CObjectContainer::clean()
{
	for(short i = 0; i < list_end; i++)
	{
		delete list[i];
		list[i] = NULL;
	}
	list_end = 0;
}


void CObjectContainer::add(CObject *ec)
{
	if(list_end < MAXOBJECTS)
	{
		list[list_end] = ec;
		ec->index = list_end;	//save index for removing
		list_end++;
	}
	else
	{
		delete ec;	//otherwise memory leak!
		//printf("eyecandy list full!\n");
	}
}

bool CObjectContainer::isBlockAt(short x, short y)
{
	for(short i = 0; i < list_end; i++)
	{
		if(x >= list[i]->ix && x < list[i]->ix + list[i]->iw && 
			y >= list[i]->iy && y < list[i]->iy + list[i]->ih && 
			list[i]->getObjectType() == object_block)
		{
			return true;
		}
	}

	return false;
}

float CObjectContainer::getClosestObject(short ix, short iy, short objectType)
{
	float dist = 800.0f;  //Longest distance from corner to corner

	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() == objectType)
		{
			short x = list[i]->ix - ix;
			short y = list[i]->iy - iy;

			float calcdist = (float)sqrt((double)(x * x + y * y));

			if(calcdist < dist)
				dist = calcdist;
		}
	}

	return dist;
}

short CObjectContainer::countTypes(ObjectType type)
{
	short count = 0;

	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() == type)
		{
			count++;
		}
	}

	return count;
}

void CObjectContainer::adjustPlayerAreas(CPlayer * player, CPlayer * other)
{
	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() == object_area)
		{
			OMO_Area * area = (OMO_Area*)list[i];

			if(area->colorID == other->colorID)
			{
				if(game_values.gamemodesettings.domination.relocateondeath)
					area->placeArea();
	
				if(game_values.gamemodesettings.domination.stealondeath && player)
					area->setOwner(player);
				else if(game_values.gamemodesettings.domination.loseondeath)
					area->reset();
			}
		}
	}
}

void CObjectContainer::removePlayerRaceGoals(short id, short iGoal)
{
	if(game_values.gamemodesettings.race.penalty == 0 && iGoal != -1)
		return;

	for(short i = 0; i < list_end; i++)
	{
		if(list[i]->getObjectType() == object_race_goal)
		{
			OMO_RaceGoal * goal = (OMO_RaceGoal*)list[i];

			if(iGoal == -1 || 2 == game_values.gamemodesettings.race.penalty ||
				(1 == game_values.gamemodesettings.race.penalty && goal->getGoalID() == iGoal))
			{
				goal->reset(id);
			}
		}
	}
}

void CObjectContainer::cleandeadobjects()
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

CObject * CObjectContainer::getRandomObject()
{
	if(list_end == 0)
		return NULL;

	return list[rand() % list_end];
}

void MysteryMushroomTempPlayer::SetPlayer(CPlayer * player, short iPowerup)
{
	fx = player->fx;
	fy = player->fy;

	fOldX = player->fOldX;
	fOldY = player->fOldY;

	velx = player->velx;
	vely = player->vely;

	//bobomb = player->bobomb;
	//powerup = player->powerup;
	
	burnupstarttimer = player->burnupstarttimer;
	burnuptimer = player->burnuptimer;

	inair = player->inair;
	onice = player->onice;
	//invincible = player->invincible;
	//invincibletimer = player->invincibletimer;

	platform = player->platform;
	//iCapeFrameX = player->iCapeFrameX;
	//iCapeFrameY = player->iCapeFrameY;
	//iCapeTimer = player->iCapeTimer;
	//iCapeYOffset = player->iCapeYOffset;

	gamepowerup = iPowerup;

	iOldPowerupX = player->score->x + scorepowerupoffsets[game_values.teamcounts[player->teamID] - 1][player->subTeamID];
	iOldPowerupY = player->score->y + 25;
}
		
void MysteryMushroomTempPlayer::GetPlayer(CPlayer * player, short * iPowerup)
{
	player->fNewSwapX = fx;
	player->fNewSwapY = fy;

	player->iOldPowerupX = iOldPowerupX;
	player->iOldPowerupY = iOldPowerupY;

	player->fOldX = fOldX;
	player->fOldY = fOldY;

	player->velx = velx;
	player->vely = vely;

	//player->bobomb = bobomb;
	//player->powerup = powerup;
	
	player->burnupstarttimer = burnupstarttimer;
	player->burnuptimer = burnuptimer;

	player->inair = inair;
	player->onice = onice;
	//player->invincible = invincible;
	//player->invincibletimer = invincibletimer;

	player->platform = platform;
	//player->iCapeFrameX = iCapeFrameX;
	//player->iCapeFrameY	= iCapeFrameY;
	//player->iCapeTimer = iCapeTimer;
	//player->iCapeYOffset = iCapeYOffset;

	*iPowerup = gamepowerup;

	if(player->carriedItem)
		player->carriedItem->MoveToOwner();
}
