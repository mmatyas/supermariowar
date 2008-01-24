#include <string.h>
#include <math.h>

#include "global.h"

extern short iKingOfTheHillZoneLimits[4][4];
extern void PlayerKilledPlayer(short iKiller, CPlayer * killed, short deathstyle, short killstyle, bool fForce);
extern void PlayerKilledPlayer(CPlayer * killer, CPlayer * killed, short deathstyle, short killstyle, bool fForce);
extern void AddAwardKill(CPlayer * killer, CPlayer * killed, killstyle style);
extern short LookupTeamID(short id);
extern bool SwapPlayers(short iUsingPlayerID);
extern short scorepowerupoffsets[3][3];

extern CPlayer * GetPlayerFromGlobalID(short iGlobalID);

void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead)
{
	if(object->dead)
		return;

	MovingObjectType type = object->movingObjectType;
	if(type == movingobject_fireball || type == movingobject_superfireball || type == movingobject_hammer || type == movingobject_sledgehammer || type == movingobject_boomerang)
	{
		short iPlayerID = object->iPlayerID;
		bool fDie = true;

		if(type == movingobject_hammer && !game_values.hammerpower)
		{
			fDie = false;
		}
		else if(type == movingobject_sledgehammer)
		{
			((MO_SledgeHammer*)object)->explode();
		}
		else if(type == movingobject_boomerang)
		{
			game_values.superboomerang[iPlayerID] = 0;
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

	if(spr)
	{
		iw = (short)spr->getWidth();
		ih = (short)spr->getHeight();
	}

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

//returns the blocks touching each of the four corners
void CObject::GetCollisionBlocks(IO_Block * blocks[4])
{
	short xl = 0;
	if(ix < 0)
		xl = (ix + 640) / TILESIZE;
	else
		xl = ix / TILESIZE;

	short xr = 0;
	if(ix + iw >= 640)
		xr = (ix + iw - 640) / TILESIZE;
	else
		xr = (ix + iw) / TILESIZE;

	blocks[0] = NULL;
	blocks[1] = NULL;

	if(iy >= 0)
	{
		short yt = iy / TILESIZE;

		blocks[0] = g_map.block(xl, yt);
		blocks[1] = g_map.block(xr, yt);
	}

	blocks[2] = NULL;
	blocks[3] = NULL;

	if(iy + ih >= 0)
	{
		short yb = (iy + ih) / TILESIZE;

		blocks[2] = g_map.block(xl, yb);
		blocks[3] = g_map.block(xr, yb);
	}
}


//------------------------------------------------------------------------------
// class Block base class
//------------------------------------------------------------------------------

IO_Block::IO_Block(gfxSprite *nspr, short x, short y) :
	CObject(nspr, x, y)
{
	iBumpPlayerID = -1;
	iBumpTeamID = -1;

	fposx = fx;
	fposy = fy;

	iposx = x;
	iposy = y;

	col = x / TILESIZE;
	row = y / TILESIZE;

	oldhidden = hidden = false;
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
		player->extrajumps = 0;
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
		}
		else if(type == movingobject_koopa)
		{
			((MO_Koopa*)object)->Die();
			style = kill_style_koopa;
		}

		if(!game_values.gamemode->gameover && iBumpPlayerID >= 0)
		{
			CPlayer * player = GetPlayerFromGlobalID(iBumpPlayerID);
			
			if(player)
			{
				AddAwardKill(player, NULL, style);
				player->score->AdjustScore(1);
			}
		}
	}
	else if(type == movingobject_shell)
	{
		((CO_Shell*)object)->Flip();
	}
	else
	{
		object->vely = -VELNOTEBLOCKREPEL;
	}
}

//------------------------------------------------------------------------------
// class powerup block
//------------------------------------------------------------------------------
B_PowerupBlock::B_PowerupBlock(gfxSprite *nspr1, short x, short y, short iNumSpr, short aniSpeed, bool fHidden, short * piSettings) :
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

	oldhidden = hidden = fHidden;

	if(piSettings[0] == -1 || game_values.overridepowerupsettings == 1) //Game Only
	{
		for(short iSetting = 0; iSetting < NUM_POWERUPS; iSetting++)
			settings[iSetting] = game_values.powerupweights[iSetting];
	}
	else if(game_values.overridepowerupsettings == 0) //Map Only
	{
		for(short iSetting = 0; iSetting < NUM_POWERUPS; iSetting++)
			settings[iSetting] = piSettings[iSetting];
	}
	else if(game_values.overridepowerupsettings == 2) //Average
	{
		for(short iSetting = 0; iSetting < NUM_POWERUPS; iSetting++)
			settings[iSetting] = piSettings[iSetting] + game_values.powerupweights[iSetting];
	}
	else if(game_values.overridepowerupsettings == 3) //Weighted
	{
		float dMapWeightCount = 0;
		for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
			dMapWeightCount += piSettings[iPowerup];

		float dGameWeightCount = 0;
		for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
			dGameWeightCount += game_values.powerupweights[iPowerup];

		for(short iSetting = 0; iSetting < NUM_POWERUPS; iSetting++)
		{
			float dWeight = ((float)piSettings[iSetting] / dMapWeightCount + (float)game_values.powerupweights[iSetting] / dGameWeightCount) * 100.0f;

			//Cap lowest value at 1
			if(dWeight < 1.0f && dWeight > 0.0f)
				settings[iSetting] = 1;
			else
				settings[iSetting] = (short)dWeight;
		}
	}

	iCountWeight = 0;
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		iCountWeight += settings[iPowerup];
}


void B_PowerupBlock::draw()
{
	if(!hidden)
		spr->draw(ix, iy, drawFrame, state == 0 ? 0 : ih, iw, ih);
}

void B_PowerupBlock::update()
{
	if(state > 0)
	{
		yf(fy + vely);
		
		if(state == 1 && fabsf(fposy - fy) > 10.0f)
		{
			oldhidden = false;

			vely = -vely;
			state = 2;
			iBumpPlayerID = -1;
		}
		else if(state == 2 && fabsf(fposy - fy) < VELBLOCKBOUNCE)
		{
			vely = 0.0f;
			state = 3;
			yi(iposy);

			if(game_values.gamemode->gamemode == game_mode_health && rand() % 100 < game_values.gamemodesettings.health.percentextralife)
			{
				objectcontainer[0].add(new PU_ExtraHeartPowerup(&spr_extraheartpowerup, ix + 1, iy - 1));
			}
			else if((game_values.gamemode->gamemode == game_mode_timelimit && rand() % 100 < game_values.gamemodesettings.time.percentextratime) ||
				(game_values.gamemode->gamemode == game_mode_star && rand() % 100 < game_values.gamemodesettings.star.percentextratime))
			{
				objectcontainer[0].add(new PU_ExtraTimePowerup(&spr_extratimepowerup, ix + 1, iy - 1));				
			}
			else
			{
				short iSelectedPowerup = SelectPowerup();

				if(0 == iSelectedPowerup)
					objectcontainer[0].add(new PU_PoisonPowerup(&spr_poisonpowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1));
				else if(1 == iSelectedPowerup)
					objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_1uppowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1, 1));
				else if(2 == iSelectedPowerup)
					objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_2uppowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1, 2));
				else if(3 == iSelectedPowerup)
					objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_3uppowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1, 3));
				else if(4 == iSelectedPowerup)
					objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_5uppowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1, 5));
				else if(5 == iSelectedPowerup)
					objectcontainer[0].add(new PU_FirePowerup(&spr_firepowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1));
				else if(6 == iSelectedPowerup)
					objectcontainer[0].add(new PU_StarPowerup(&spr_starpowerup, ix + 1, iy - 1, 4, side, 2, 30, 30, 1, 1));
				else if(7 == iSelectedPowerup)
					objectcontainer[0].add(new PU_ClockPowerup(&spr_clockpowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1));
				else if(8 == iSelectedPowerup)
					objectcontainer[0].add(new PU_BobombPowerup(&spr_bobombpowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1));
				else if(9 == iSelectedPowerup)
					objectcontainer[0].add(new PU_PowPowerup(&spr_powpowerup, ix + 1, iy - 1, 8, side, 8, 30, 30, 1, 1));
				else if(10 == iSelectedPowerup)
					objectcontainer[0].add(new PU_BulletBillPowerup(&spr_bulletbillpowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1));
				else if(11 == iSelectedPowerup)
					objectcontainer[0].add(new PU_HammerPowerup(&spr_hammerpowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1));
				else if(12 == iSelectedPowerup)
					objectcontainer[1].add(new CO_Shell(0, ix + 1, iy - 1, true, true, true, false));
				else if(13 == iSelectedPowerup)
					objectcontainer[1].add(new CO_Shell(1, ix + 1, iy - 1, false, true, true, false));
				else if(14 == iSelectedPowerup)
					objectcontainer[1].add(new CO_Shell(2, ix + 1, iy - 1, false, false, true, true));
				else if(15 == iSelectedPowerup)
					objectcontainer[1].add(new CO_Shell(3, ix + 1, iy - 1, false, true, false, false));
				else if(16 == iSelectedPowerup)
					objectcontainer[0].add(new PU_ModPowerup(&spr_modpowerup, ix + 1, iy - 1, 8, side, 8, 30, 30, 1, 1));
				else if(17 == iSelectedPowerup)
					objectcontainer[0].add(new PU_FeatherPowerup(&spr_featherpowerup, ix + 1, iy - 1, 1, 0, 30, 30, 1, 1));
				else if(18 == iSelectedPowerup)
					objectcontainer[0].add(new PU_MysteryMushroomPowerup(&spr_mysterymushroompowerup, ix + 1, iy - 1, 1, side, 0, 30, 30, 1, 1));
				else if(19 == iSelectedPowerup)
					objectcontainer[0].add(new PU_BoomerangPowerup(&spr_boomerangpowerup, ix + 1, iy - 1, 1, side, 0, 30, 26, 1, 5));
				else if(20 == iSelectedPowerup) 
					objectcontainer[0].add(new PU_Tanooki(ix + 1, iy - 1));
				else if(21 == iSelectedPowerup) 
					objectcontainer[0].add(new PU_SledgeHammerPowerup(&spr_sledgehammerpowerup, ix + 1, iy - 1, 1, 0, 30, 30, 1, 1));
				else if(22 == iSelectedPowerup) 
					objectcontainer[0].add(new PU_PodoboPowerup(&spr_podobopowerup, ix + 1, iy - 1, 1, 0, 30, 30, 1, 1));
				else if(23 == iSelectedPowerup) 
					objectcontainer[0].add(new PU_BombPowerup(&spr_bombpowerup, ix + 1, iy - 1, 1, 0, 30, 30, 1, 1));
				else if(24 == iSelectedPowerup)
					objectcontainer[0].add(new PU_LeafPowerup(&spr_leafpowerup, ix + 1, iy - 1, 1, 0, 30, 30, 1, 1));
				else if(25 == iSelectedPowerup) 
					objectcontainer[0].add(new PU_PWingsPowerup(&spr_pwingspowerup, ix + 1, iy - 1));
			}

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

bool B_PowerupBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
	if(hidden)
	{
		if(player->fOldY >= iposy + ih && direction == 0)
			return hitbottom(player, useBehavior);

		return true;
	}
	else if(oldhidden)
	{
		PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, true);
		return false;
	}

	return IO_Block::collide(player, direction, useBehavior);
}

bool B_PowerupBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(state == 1)
	{
		short iKillType = player_kill_nonkill;
		if(iBumpPlayerID >= 0 && !player->IsInvincibleOnBottom() && (player->teamID != iBumpTeamID || game_values.teamcollision == 2))
			iKillType = PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, false);
		
		if(player_kill_nonkill == iKillType)
			player->vely = -VELNOTEBLOCKREPEL;
	}
	else if(useBehavior)
	{
		player->vely = GRAVITATION;

		if(state == 0)
		{
			if(player->IsSuperStomping())
			{
				state = 1;
				vely = -VELBLOCKBOUNCE;
				side = player->ix + HALFPW < ix + (iw >> 1);
			}
		}
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

		hidden = false;

		if(state == 0)
		{
			ifsoundonplay(sfx_bump);

			iBumpPlayerID = player->globalID;
			iBumpTeamID = player->teamID;

			vely = -VELBLOCKBOUNCE;
			state = 1;
			side = player->ix + HALFPW < ix + (iw >> 1);
		}
	}

	return false;
}

bool B_PowerupBlock::collide(IO_MovingObject * object, short direction)
{
	if(hidden)
	{
		return true;
	}
	else if(oldhidden)
	{
		object->KillObjectMapHazard();
		return false;
	}

	return IO_Block::collide(object, direction);
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
	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_egg || type == movingobject_attackzone /*|| type == movingobject_star*/)
	{
		if(type == movingobject_shell)
		{
			if(object->state != 1)
				return false;
		}

		if(state == 0)
		{
			ifsoundonplay(sfx_bump);

			iBumpPlayerID = -1;
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
	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_egg || type == movingobject_attackzone /*|| type == movingobject_star*/)
	{
		if(type == movingobject_shell)
		{
			if(object->state != 1)
				return false;
		}

		if(state == 0)
		{
			ifsoundonplay(sfx_bump);

			iBumpPlayerID = -1;
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

		iBumpPlayerID = -1;
		vely = -VELBLOCKBOUNCE;
		state = 1;
		side = true;
	}
}

short B_PowerupBlock::SelectPowerup()
{
	if(iCountWeight == 0)
		return -1;

	int iRandPowerup = rand() % iCountWeight + 1;
	int iSelectedPowerup = 0;

	int iPowerupWeightCount = settings[iSelectedPowerup];

	while(iPowerupWeightCount < iRandPowerup)
		iPowerupWeightCount += settings[++iSelectedPowerup];

	return iSelectedPowerup;
}

//------------------------------------------------------------------------------
// class view powerup block
//------------------------------------------------------------------------------
B_ViewBlock::B_ViewBlock(gfxSprite *nspr1, short x, short y, bool fHidden, short * piSettings) :
	B_PowerupBlock(nspr1, x, y, 1, 32000, fHidden, piSettings)
{
	poweruptimer = 0;
	powerupindex = rand() % NUM_POWERUPS;

	iw = 32;
	ih = 32;

	iCountWeight = 0;
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		iCountWeight += settings[iPowerup];

	fNoPowerupsSelected = iCountWeight == 0;
	GetNextPowerup();
}


void B_ViewBlock::draw()
{
	if(hidden)
		return;

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
		if(++poweruptimer > settings[powerupindex] * 10)
		{
			poweruptimer = 0;
			GetNextPowerup();
		}
	}
}

short B_ViewBlock::SelectPowerup()
{
	if(fNoPowerupsSelected)
		return -1;

	return powerupindex;
}

void B_ViewBlock::GetNextPowerup()
{
	if(fNoPowerupsSelected)
		return;

	int iRandPowerup = rand() % iCountWeight + 1;
	powerupindex = 0;
	int iPowerupWeightCount = settings[powerupindex];

	while(iPowerupWeightCount < iRandPowerup)
		iPowerupWeightCount += settings[++powerupindex];
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
			iBumpPlayerID = -1;
			dead = true;
			g_map.blockdata[col][row] = NULL;
			g_map.UpdateTileGap(col, row);
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
		short iKillType = player_kill_nonkill;
		if(iBumpPlayerID >= 0 && !player->IsInvincibleOnBottom() && (player->teamID != iBumpTeamID || game_values.teamcollision == 2))
			iKillType = PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, false);
		
		if(player_kill_nonkill == iKillType)
			player->vely = -VELNOTEBLOCKREPEL;
	}
	else if(useBehavior)
	{
		player->vely = GRAVITATION;

		if(player->IsSuperStomping() && state == 0)
		{
			triggerBehavior();
			return true;
		}
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
		
		iBumpPlayerID = player->globalID;
		iBumpTeamID = player->teamID;
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
		if((type == movingobject_shell && object->state == 1) || type == movingobject_throwblock || type == movingobject_attackzone /*|| type == movingobject_egg || type == movingobject_star*/)
		{
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
		if((type == movingobject_shell && object->state == 1) || type == movingobject_throwblock || type == movingobject_attackzone /*|| type == movingobject_egg || type == movingobject_star*/)
		{
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
		eyecandyfront.add(new EC_FallingObject(&spr_brokenyellowblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenyellowblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenyellowblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 16, 16));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenyellowblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 16, 16));

		state = 1;
		ifsoundonplay(sfx_breakblock);
	}
}

//------------------------------------------------------------------------------
// class note block
//------------------------------------------------------------------------------
B_NoteBlock::B_NoteBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short type, bool fHidden) :
	IO_Block(nspr, x, y)
{
	iw = (short)nspr->getWidth() >> 2;
	ih = TILESIZE;
	iNumSprites = iNumSpr;
	animationSpeed = aniSpeed;
	animationTimer = 0;
	drawFrame = 0;
	animationWidth = (short)spr->getWidth();

	oldhidden = hidden = fHidden;

	iType = type;
	iTypeOffsetY = iType * TILESIZE;
}

void B_NoteBlock::draw()
{
	if(hidden)
		return;

	spr->draw(ix, iy, drawFrame, iTypeOffsetY, iw, ih);
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
			oldhidden = false;
			iBumpPlayerID = -1;
		}
		else if(state == 4 && fabsf(fposy - fy) < VELNOTEBLOCKBOUNCE)
		{
			vely = 0.0f;
			state = 0;
			xf(fposx);
			yf(fposy);
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

bool B_NoteBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
	if(hidden)
	{
		if((player->fOldY >= iposy + ih || state > 1) && direction == 0)
			return hitbottom(player, useBehavior);

		return true;
	}
	else if(oldhidden)
	{
		PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, true);
		return false;
	}

	return IO_Block::collide(player, direction, useBehavior);
}

bool B_NoteBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(useBehavior)
	{
		player->superjumptimer = 4;
		player->superjumptype = iType;
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
			iBumpPlayerID = player->globalID;
			iBumpTeamID = player->teamID;

			vely = -VELNOTEBLOCKBOUNCE;
			state = 3;

			ifsoundonplay(sfx_bump);
		}

		hidden = false;
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

bool B_NoteBlock::collide(IO_MovingObject * object, short direction)
{
	if(hidden)
	{
		return true;
	}
	else if(oldhidden)
	{
		object->KillObjectMapHazard();
		return false;
	}

	return IO_Block::collide(object, direction);
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
	//eyecandyfront.add(new EC_FallingObject(&spr_donutblock, ix, iy, 0.0f, 0, 0, 0, 0));
	
	TilesetTile ** tiledata = new TilesetTile*[1];
	tiledata[0] = new TilesetTile[1];
	tiledata[0][0].iID = 0;
	tiledata[0][0].iCol = 29;
	tiledata[0][0].iRow = 15;

	MapTile ** typedata = new MapTile*[1];
	typedata[0] = new MapTile[1];
	typedata[0][0].iType = tile_solid;
	typedata[0][0].iFlags = tile_flag_solid;

	MovingPlatformPath * path = new MovingPlatformPath(0.0f, (float)ix + 16.0f, (float)iy + 16.0f, 0.0f, 0.0f, true); 
	MovingPlatform * platform = new MovingPlatform(tiledata, typedata, 1, 1, path, true, 0, false);

	g_map.AddTemporaryPlatform(platform);

	dead = true;
	g_map.blockdata[col][row] = NULL;
	g_map.UpdateTileGap(col, row);
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
B_FlipBlock::B_FlipBlock(gfxSprite *nspr, short x, short y, bool fHidden) :
	IO_Block(nspr, x, y)
{
	iw = (short)spr->getWidth() >> 2;
	collisionWidth = iw;

	oldhidden = hidden = fHidden;

	counter = 0;
	frame = 0;
	timer = 0;
	animationWidth = (short)spr->getWidth();
}

void B_FlipBlock::draw()
{
	if(hidden)
		return;

	if(state == 0 || state == 1)
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
			oldhidden = false;
			iBumpPlayerID = -1;

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

			g_map.UpdateTileGap(col, row);
		}
	}
	else if(state == 2)
	{
		state = 3;
	}
	else if(state == 3)
	{
		dead = true;
		g_map.blockdata[col][row] = NULL;
		g_map.UpdateTileGap(col, row);
	}
}

bool B_FlipBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
	if(hidden)
	{
		if(player->fOldY >= iposy + ih && direction == 0)
			return hitbottom(player, useBehavior);

		return true;
	}
	else if(oldhidden)
	{
		PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, true);
		return false;
	}

	if((player->fOldY + PH <= iposy || state > 1) && direction == 2)
		return hittop(player, useBehavior);
	else if((player->fOldY >= iposy + ih || state > 1) && direction == 0)
		return hitbottom(player, useBehavior);
	else if((player->fOldX + PW <= iposx || state > 1) && direction == 1)
		return hitleft(player, useBehavior);
	else if((player->fOldX >= iposx + iw || state > 1) && direction == 3)
		return hitright(player, useBehavior);

	return true;
}

bool B_FlipBlock::hittop(CPlayer * player, bool useBehavior)
{
	if(state == 2 || state == 3)
	{
		IO_Block::hittop(player, useBehavior);

		player->vely = -VELNOTEBLOCKREPEL;
		return false;
	}
	else if(state == 0)
	{
		IO_Block::hittop(player, useBehavior);

		if(player->IsSuperStomping())
		{
			state = 2;
			explode();
			return true;
		}

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
		
		iBumpPlayerID = player->globalID;
		iBumpTeamID = player->teamID;

		hidden = false;

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

bool B_FlipBlock::collide(IO_MovingObject * object, short direction)
{
	if(hidden)
	{
		return true;
	}
	else if(oldhidden)
	{
		object->KillObjectMapHazard();
		return false;
	}

	return IO_Block::collide(object, direction);
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
		if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_attackzone)
		{
			if(type == movingobject_shell)
			{
				if(object->state != 1)
					return false;
			}

			state = 2;
			explode();
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
		if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_attackzone)
		{
			if(type == movingobject_shell)
			{
				if(object->state != 1)
					return false;
			}

			state = 2;
			explode();
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

		g_map.UpdateTileGap(col, row);
	}
}

void B_FlipBlock::explode()
{
	eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenflipblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 16, 16));
	
	ifsoundonplay(sfx_breakblock);
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
			iBumpPlayerID = -1;
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
		short iKillType = player_kill_nonkill;
		if(iBumpPlayerID >= 0 && !player->IsInvincibleOnBottom() && (player->teamID != iBumpTeamID || game_values.teamcollision == 2))
			iKillType = PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, false);
		
		if(player_kill_nonkill == iKillType)
			player->vely = -VELNOTEBLOCKREPEL;
	}
	else if(useBehavior)
	{
		player->vely = GRAVITATION;

		if((state == 0 || state == 3) && player->IsSuperStomping())
			triggerBehavior();
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
			iBumpPlayerID = player->globalID;
			iBumpTeamID = player->teamID;

			triggerBehavior();
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
	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_egg  || type == movingobject_attackzone/*|| type == movingobject_star*/)
	{
		if(type == movingobject_shell)
		{
			if(object->state != 1)
				return false;
		}

		if(state == 0 || state == 3)
		{
			iBumpPlayerID = -1;
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
	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_egg  || type == movingobject_attackzone/*|| type == movingobject_star*/)
	{
		if(type == movingobject_shell)
		{
			if(object->state != 1)
				return false;
		}

		if(state == 0 || state == 3)
		{
			iBumpPlayerID = -1;
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

void B_SwitchBlock::FlipState()
{
	state = 1 - state;
	g_map.UpdateTileGap(col, row);
}

//------------------------------------------------------------------------------
// class bounce block
//------------------------------------------------------------------------------
B_BounceBlock::B_BounceBlock(gfxSprite *nspr1, short x, short y, bool fHidden) :
	IO_Block(nspr1, x, y)
{
	oldhidden = hidden = fHidden;
}

void B_BounceBlock::draw()
{
	if(hidden)
		return;

	IO_Block::draw();
}

void B_BounceBlock::update()
{
	if(state > 0)
	{
		yf(fy + vely);

		if(state == 1 && fabsf(fposy - fy) > 10.0f)
		{
			oldhidden = false;

			iBumpPlayerID = -1;
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

bool B_BounceBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
	if(hidden)
	{
		if(player->fOldY >= iposy + ih && direction == 0)
			return hitbottom(player, useBehavior);

		return true;
	}
	else if(oldhidden)
	{
		PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, true);
		return false;
	}

	return IO_Block::collide(player, direction, useBehavior);
}

bool B_BounceBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(state == 1)
	{
		short iKillType = player_kill_nonkill;
		if(iBumpPlayerID >= 0 && !player->IsInvincibleOnBottom() && (player->teamID != iBumpTeamID || game_values.teamcollision == 2))
			iKillType = PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, false);
		
		if(player_kill_nonkill == iKillType)
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

		iBumpPlayerID = player->globalID;
		iBumpTeamID = player->teamID;

		triggerBehavior();

		hidden = false;
	}

	return false;
}

bool B_BounceBlock::collide(IO_MovingObject * object, short direction)
{
	if(hidden)
	{
		return true;
	}
	else if(oldhidden)
	{
		object->KillObjectMapHazard();
		return false;
	}

	return IO_Block::collide(object, direction);
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
		ifsoundonplay(sfx_bump);
	}
}

//------------------------------------------------------------------------------
// class throw block
//------------------------------------------------------------------------------
B_ThrowBlock::B_ThrowBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short type) :
	IO_Block(nspr, x, y)
{
	iw = 32;
	ih = 32;
	iNumSprites = iNumSpr;
	animationSpeed = aniSpeed;
	animationTimer = 0;
	drawFrame = 0;
	animationWidth = (short)nspr->getWidth();
	iType = type;
}

void B_ThrowBlock::draw()
{
	spr->draw(ix, iy, drawFrame, iType << 5, iw, ih);
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
		player->yf((float)(iposy - PH) - 0.2f);
		player->fOldY = player->fy;
		player->inair = false;
		player->fallthrough = false;
		player->killsinrowinair = 0;
		player->extrajumps = 0;
		player->vely = GRAVITATION;

		if(player->PressedAcceptItemKey() && player->IsAcceptingItem())
		{
			GiveBlockToPlayer(player);
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool B_ThrowBlock::hitright(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		player->xf((float)(iposx + iw) + 0.2f);
		player->fOldX = player->fx;

		if(player->velx < 0.0f)
			player->velx = 0.0f;

		if(player->IsAcceptingItem())
		{
			GiveBlockToPlayer(player);
			return true;
		}
		else
		{
			return false;
		}
	}
	
	return true;
}

bool B_ThrowBlock::hitleft(CPlayer * player, bool useBehavior)
{
	if(useBehavior)
	{
		player->xf((float)(iposx - PW) - 0.2f);
		player->fOldX = player->fx;

		if(player->velx > 0.0f)
			player->velx = 0.0f;

		if(player->IsAcceptingItem())
		{
			GiveBlockToPlayer(player);
			return true;
		}
		else
		{
			return false;
		}
	}
	
	return true;
}

void B_ThrowBlock::GiveBlockToPlayer(CPlayer * player)
{
	dead = true;
	g_map.blockdata[col][row] = NULL;
	g_map.UpdateTileGap(col, row);

	CO_ThrowBlock * block = new CO_ThrowBlock(&spr_blueblock, ix, iy, iType);
	if(player->AcceptItem(block))
	{
		block->owner = player;
		block->iPlayerID = player->globalID;
		objectcontainer[1].add(block);
	}
	else
		delete block;
}

void B_ThrowBlock::triggerBehavior()
{
	dead = true;
	g_map.blockdata[col][row] = NULL;
	g_map.UpdateTileGap(col, row);

	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy, -1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy, 1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy + 16, -1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy + 16, 1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));
	
	ifsoundonplay(sfx_breakblock);
}


//------------------------------------------------------------------------------
// class weapon breakable block
//------------------------------------------------------------------------------
B_WeaponBreakableBlock::B_WeaponBreakableBlock(gfxSprite *nspr, short x, short y, short type) :
	IO_Block(nspr, x, y)
{
	iType = type;
	iw = TILESIZE;
	ih = TILESIZE;

	iDrawOffsetX = type << 5;
}

void B_WeaponBreakableBlock::draw()
{
	if(state == 0)
		spr->draw(ix, iy, iDrawOffsetX, 0, iw, ih);
}

void B_WeaponBreakableBlock::update()
{
	if(state > 0)
	{
		if(state == 1)
		{
			state = 2;
		}
		else if(state == 2)
		{
			iBumpPlayerID = -1;
			dead = true;
			g_map.blockdata[col][row] = NULL;
			g_map.UpdateTileGap(col, row);
		}
	}
}


bool B_WeaponBreakableBlock::hittop(CPlayer * player, bool useBehavior)
{
	IO_Block::hittop(player, useBehavior);

	if(state == 1 || state == 2)
	{
		short iKillType = player_kill_nonkill;
		if(iBumpPlayerID >= 0 && !player->IsInvincibleOnBottom() && (player->teamID != iBumpTeamID || game_values.teamcollision == 2))
			PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, false);
		
		if(player_kill_nonkill == iKillType)
			player->vely = -VELNOTEBLOCKREPEL;
	}
	else if(useBehavior)
	{
		player->vely = GRAVITATION;

		//Save this for when we create a super stomp destroyable block
		if(iType == 6 && player->IsSuperStomping() && state == 0)
		{
			triggerBehavior(player->globalID, player->teamID);
			return false;
		}
		else if(iType == 8 && player->invincible)
		{
			triggerBehavior(player->globalID, player->teamID);
			return false;
		}
	}

	return false;
}

bool B_WeaponBreakableBlock::hitbottom(CPlayer * player, bool useBehavior)
{
	if(useBehavior && state == 0)
	{
		//If the player has a cape and they used it for a feather destroyable block, then kill it
		bool fTriggerBlock = false;
		if(iType == 1 && player->powerup == 3 && player->extrajumps > 0)
		{
			fTriggerBlock = true;
		}
		else if(iType == 7 && player->powerup == 8 && player->flying)
		{
			fTriggerBlock = true;
		}
		else if(iType == 8 && player->invincible)
		{
			fTriggerBlock = true;
		}
		
		if(fTriggerBlock)
			triggerBehavior(player->globalID, player->teamID);
	
		return IO_Block::hitbottom(player, useBehavior);
	}

	return false;
}

bool B_WeaponBreakableBlock::hitleft(CPlayer * player, bool useBehavior)
{
	if(useBehavior && state == 0)
	{
		if(iType == 8 && player->invincible)
			triggerBehavior(player->globalID, player->teamID);

		return IO_Block::hitleft(player, useBehavior);
	}
	
	return false;
}

bool B_WeaponBreakableBlock::hitright(CPlayer * player, bool useBehavior)
{
	if(useBehavior && state == 0)
	{
		if(iType == 8 && player->invincible)
			triggerBehavior(player->globalID, player->teamID);

		return IO_Block::hitright(player, useBehavior);
	}
	
	return false;
}

bool B_WeaponBreakableBlock::hittop(IO_MovingObject * object)
{
	IO_Block::hittop(object);
	
	if((state == 1  || state == 2) && object->bounce == GRAVITATION)
	{
		BounceMovingObject(object);
		return false;
	}

	if(state != 0)
		return true;

	MovingObjectType type = object->getMovingObjectType();

	if(iType == 0 && type == movingobject_fireball)
	{
		triggerBehavior(object->iPlayerID, object->iTeamID);
		removeifprojectile(object, false, true);
		return false;
	}

	return true;
}

bool B_WeaponBreakableBlock::hitbottom(IO_MovingObject * object)
{
	if(state != 0)
		return true;

	IO_Block::hitbottom(object);
	
	MovingObjectType type = object->getMovingObjectType();

	if(iType == 0 && type == movingobject_fireball)
	{
		triggerBehavior(object->iPlayerID, object->iTeamID);
		removeifprojectile(object, false, true);
		return false;
	}

	return true;
}

bool B_WeaponBreakableBlock::hitright(IO_MovingObject * object)
{
	if(state != 0)
		return true;

	IO_Block::hitright(object);

	return objecthitside(object);
}

bool B_WeaponBreakableBlock::hitleft(IO_MovingObject * object)
{
	if(state != 0)
		return true;

	IO_Block::hitleft(object);

	return objecthitside(object);
}

void B_WeaponBreakableBlock::triggerBehavior(short iPlayerID, short iTeamID)
{
	if(state == 0)
	{
		eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 16, 16, 16));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 16, 16, 16));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 16, 16, 16));
		eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 16, 16, 16));

		state = 1;
		ifsoundonplay(sfx_breakblock);

		iBumpPlayerID = iPlayerID;
		iBumpTeamID = iTeamID;
	}
}

bool B_WeaponBreakableBlock::objecthitside(IO_MovingObject * object)
{
	MovingObjectType type = object->getMovingObjectType();
	
	if(iType == 2 && ((type == movingobject_shell && object->state == 1) || type == movingobject_throwblock))
	{
		short iPlayerID = -1;
		short iTeamID = -1;
		if(type == movingobject_shell)
		{
			CO_Shell * shell = (CO_Shell*)object;
			iPlayerID = shell->iPlayerID;
			iTeamID = shell->iTeamID;
		}
		else if(type == movingobject_throwblock)
		{
			CO_ThrowBlock * throwblock = (CO_ThrowBlock*)object;
			iPlayerID = throwblock->iPlayerID;
			iTeamID = throwblock->iTeamID;
		}

		triggerBehavior(iPlayerID, iTeamID);
		return false;
	}
	else if(iType == 0 && type == movingobject_fireball)
	{
		MO_Fireball * fireball = (MO_Fireball*)object;
		triggerBehavior(fireball->iPlayerID, fireball->iTeamID);
		removeifprojectile(object, false, true);
		return false;
	}
	else if(type == movingobject_attackzone)
	{
		MO_AttackZone * zone = (MO_AttackZone*)object;

		if((zone->iStyle == kill_style_leaf && iType == 9) || (zone->iStyle == kill_style_feather && iType == 1))
		{
			triggerBehavior(zone->iPlayerID, zone->iTeamID);
			zone->Die();
			return false;
		}
	}

	return true;
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
	else if(spr)
		iw = (short)spr->getWidth() / iNumSprites;

	if(iAnimationHeight > -1)
		ih = iAnimationHeight;

	animationtimer = 0;

	if(spr)
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

	animate();
}

void IO_MovingObject::animate()
{
	if(animationspeed > 0 && ++animationtimer >= animationspeed)
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

void IO_MovingObject::applyfriction()
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

			bool fTopBlockSolid = topblock && !topblock->isTransparent() && !topblock->isHidden();
			bool fBottomBlockSolid = bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden();
			if(fTopBlockSolid || fBottomBlockSolid)
			{
				bool processOtherBlock = true;
				if(fTopBlockSolid) //collide with top block
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
				
				if(processOtherBlock && fBottomBlockSolid) //then bottom
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
			else if((g_map.map(tx, ty) & tile_flag_solid) || (g_map.map(tx, ty2) & tile_flag_solid))
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

			bool fTopBlockSolid = topblock && !topblock->isTransparent() && !topblock->isHidden();
			bool fBottomBlockSolid = bottomblock && !bottomblock->isTransparent() && !bottomblock->isHidden();
			if(fTopBlockSolid || fBottomBlockSolid)
			{
				bool processOtherBlock = true;
				if(fTopBlockSolid) //collide with top block
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
				
				if(processOtherBlock && fBottomBlockSolid) //then bottom
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
			else if((g_map.map(tx, ty) & tile_flag_solid) || (g_map.map(tx, ty2) & tile_flag_solid))
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
		
		if(leftblock && !leftblock->isTransparent() && !leftblock->isHidden()) //then left
		{	
			if(iVerticalPlatformCollision == 2)
				KillObjectMapHazard();

			leftblock->collide(this, 0);
			return;
		}
		
		if(rightblock && !rightblock->isTransparent() && !rightblock->isHidden()) //then right
		{	
			if(iVerticalPlatformCollision == 2)
				KillObjectMapHazard();

			rightblock->collide(this, 0);
			return;
		}

		if((g_map.map(txl, ty) & tile_flag_solid) || (g_map.map(txr, ty) & tile_flag_solid))
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

		bool fLeftBlockSolid = leftblock && !leftblock->isTransparent() && !leftblock->isHidden();
		bool fRightBlockSolid = rightblock && !rightblock->isTransparent() && !rightblock->isHidden();

		if(fLeftBlockSolid || fRightBlockSolid)
		{
			bool processOtherBlock = true;
			if(fLeftBlockSolid) //collide with left block
			{	
				processOtherBlock = leftblock->collide(this, 2);

				if(!platform)
				{
					inair = false;
					onice = false;
				}
			}
			
			if(processOtherBlock && fRightBlockSolid) //then right
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

		int leftTile = g_map.map(txl, ty);
		int rightTile = g_map.map(txr, ty);

		if((leftTile & tile_flag_solid_on_top) || (rightTile & tile_flag_solid_on_top))
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

		if((leftTile & tile_flag_solid) || (rightTile & tile_flag_solid))
		{	
			vely = BottomBounce();
			yf((float)(ty * TILESIZE - collisionHeight) - 0.2f);
			fOldY = fy;

			if(!platform)
			{
				inair = false;

				if((leftTile & tile_flag_ice && ((rightTile & tile_flag_ice) || rightTile == tile_flag_nonsolid || rightTile == tile_flag_gap)) ||
					(rightTile & tile_flag_ice && ((leftTile & tile_flag_ice) || leftTile == tile_flag_nonsolid || leftTile == tile_flag_gap)))
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

	short txl = -1;
	if(ix < 0)
		txl = (ix + 640) / TILESIZE;
	else
		txl = ix / TILESIZE;

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

				if((block && !block->isTransparent() && !block->isHidden()) || (g_map.map(txl, ty) & tile_flag_solid) > 0)
				{
					iCase |= 0x01;
				}
			}

			if(txr >= 0 && txr < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txr, ty);

				if((block && !block->isTransparent() && !block->isHidden()) || (g_map.map(txr, ty) & tile_flag_solid) > 0)
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

				if((block && !block->isTransparent() && !block->isHidden()) || (g_map.map(txl, ty2) & tile_flag_solid) > 0)
				{
					iCase |= 0x04;
				}
			}

			if(txr >= 0 && txr < MAPWIDTH)
			{
				IO_Block * block = g_map.block(txr, ty2);

				if((block && !block->isTransparent() && !block->isHidden()) || (g_map.map(txr, ty2) & tile_flag_solid) > 0)
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
			if(iPlayerID > -1 && projectiles[iPlayerID] > 0)
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
			else if(movingObjectType == movingobject_bomb)
			{
				if(iPlayerID > -1 && projectiles[iPlayerID] > 0)
					projectiles[iPlayerID]--;
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
	
	animate();
}

bool MO_Powerup::collide(CPlayer *)
{
	if(state > 0)
		dead = true;

	return false;
}

//------------------------------------------------------------------------------
// tanooki suit
//------------------------------------------------------------------------------
PU_Tanooki::PU_Tanooki(short x, short y)
    : MO_Powerup(&spr_tanooki, x, y, 1, 0, 30, 30, 1, 1)
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
// pwings
//------------------------------------------------------------------------------
PU_PWingsPowerup::PU_PWingsPowerup(gfxSprite * nspr, short x, short y)
    : MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{}

bool PU_PWingsPowerup :: collide (CPlayer *player)
{
	player->SetPowerup(8);
	dead = true;
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
	if(state > 0)
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
	if(state > 0)
	{
		game_values.gamemode->playerextraguy(*player, iType);
		ifsoundonplay(sfx_extraguysound);

		eyecandyfront.add(new EC_FloatingObject(&spr_extralife, player->ix + HALFPW - 19, player->iy - 16, 0.0f, -1.5f, 62, player->colorID * 38, (iType == 5 ? 3 : iType - 1) * 16, 38, 16));
		
		dead = true;
	}

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
	if(state > 0)
	{
		if(player->invincible)
		{
			dead = true;
			return false;
		}

		if(player->spawninvincible)
			return false;

		dead = true;

		return player->KillPlayerMapHazard(false, kill_style_poisonmushroom) != player_kill_nonkill;
	}

	return false;
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
}

bool PU_MysteryMushroomPowerup::collide(CPlayer * player)
{
	if(state > 0)
	{
		dead = true;

		if(!SwapPlayers(player->localID))
		{
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, player->ix + (HALFPW) - 16, player->iy + (HALFPH) - 16, 3, 8));
			ifsoundonplay(sfx_spit);
		}
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
	if(state > 0)
	{
		player->SetPowerup(1);
		dead = true;
	}

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
	if(state > 0)
	{
		player->SetPowerup(2);
		dead = true;
	}

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
	if(state == 1)
		spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

bool PU_SledgeHammerPowerup::collide(CPlayer * player)
{
	{
		player->SetPowerup(5);
		dead = true;
	}

	return false;
}

//------------------------------------------------------------------------------
// class podobo powerup
//------------------------------------------------------------------------------
PU_PodoboPowerup::PU_PodoboPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_PodoboPowerup::collide(CPlayer * player)
{
	if(state > 0)
	{
		dead = true;
		ifsoundonplay(sfx_storepowerup);
		game_values.gamepowerups[player->globalID] = 22;
	}

	return false;
}

//------------------------------------------------------------------------------
// class bomb powerup
//------------------------------------------------------------------------------
PU_BombPowerup::PU_BombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;
}

bool PU_BombPowerup::collide(CPlayer * player)
{
	if(state > 0)
	{
		player->SetPowerup(6);
		dead = true;
	}

	return false;
}

//------------------------------------------------------------------------------
// class treasure chest powerup
//------------------------------------------------------------------------------
PU_TreasureChestBonus::PU_TreasureChestBonus(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iBonusItem) :
	MO_Powerup(nspr, 0, 0, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
	velx = 0.0f;

	sparkleanimationtimer = 0;
	sparkledrawframe = 0;
	bounce = -VELPOWERUPBOUNCE * 2;
	numbounces = 5;
	state = 2;
	bonusitem = iBonusItem;
	
	g_map.findspawnpoint(1, &ix, &iy, collisionWidth, collisionHeight, false);
	fx = (float)ix;
	fy = (float)iy;

	drawbonusitemx = 0;
	drawbonusitemy = 0;
	drawbonusitemtimer = 0;
}

void PU_TreasureChestBonus::update()
{
	MO_Powerup::update();

	if(++sparkleanimationtimer >= 4)
	{
		sparkleanimationtimer = 0;
		sparkledrawframe += 32;
		if(sparkledrawframe >= 480)
			sparkledrawframe = 0;
	}

	//Draw rising powerup from chest
	if (state == 3)
	{
		drawbonusitemy -= 2;

		if(--drawbonusitemtimer <= 0)
			state = 4;
	}
	else if (state == 4)
	{
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix, drawbonusitemy, 3, 8));
		dead = true;
	}
}

void PU_TreasureChestBonus::draw()
{
	if(state < 3)
	{
		MO_Powerup::draw();

		//Draw sparkles
		spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
	}
	else
	{
		if(bonusitem >= NUM_POWERUPS)
			spr_worlditems.draw(drawbonusitemx, drawbonusitemy, (bonusitem - NUM_POWERUPS) << 5, 0, 32, 32);
		else
			spr_storedpoweruplarge.draw(drawbonusitemx, drawbonusitemy, bonusitem << 5, 0, 32, 32);
	}
}

bool PU_TreasureChestBonus::collide(CPlayer * player)
{
	if(state == 1)
	{
		ifsoundonplay(sfx_treasurechest);
		//if(game_values.worldpowerupcount[player->teamID] < 32)
        //    game_values.worldpowerups[player->teamID][game_values.worldpowerupcount[player->teamID]++] = bonusitem;
		//else
		//	game_values.worldpowerups[player->teamID][31] = bonusitem;

		state = 3;

		drawbonusitemx = ix;
		drawbonusitemy = iy;
		drawbonusitemtimer = 60;

		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix, iy, 3, 8));

		game_values.noexit = false;
	}

	return false;
}

float PU_TreasureChestBonus::BottomBounce()
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
// class treasure chest powerup
//------------------------------------------------------------------------------
MO_BonusHouseChest::MO_BonusHouseChest(gfxSprite *nspr, short ix, short iy, short iBonusItem) :
	IO_MovingObject(nspr, ix, iy, 1, 0, 64, 64, 0, 0)
{
	iw = 64;
	ih = 64;

	state = 1;
	bonusitem = iBonusItem;

	drawbonusitemy = 0;
	drawbonusitemtimer = 0;

	movingObjectType = movingobject_treasurechest;
}

void MO_BonusHouseChest::update()
{
	//Draw rising powerup from chest
	if (state == 2)
	{
		drawbonusitemy -= 2;

		if(--drawbonusitemtimer <= 0)
		{
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + 16, drawbonusitemy, 3, 8));
			state = 3;
		}
	}
}

void MO_BonusHouseChest::draw()
{
	if(state < 2)
		spr->draw(ix, iy, 0, 0, iw, ih);

	if(state >= 2)
		spr->draw(ix, iy, 128, 0, iw, ih);

	if(state == 2)
	{
		if(bonusitem >= NUM_POWERUPS + NUM_WORLD_POWERUPS) //Score bonuses
		{
			short iBonus = bonusitem - NUM_POWERUPS - NUM_WORLD_POWERUPS;
			short iBonusX = (iBonus % 10) << 5;
			short iBonusY = ((iBonus / 10) << 5) + 32;
			spr_worlditems.draw(ix + 16, drawbonusitemy, iBonusX, iBonusY, 32, 32);
		}
		else if(bonusitem >= NUM_POWERUPS) //World Item
			spr_worlditems.draw(ix + 16, drawbonusitemy, (bonusitem - NUM_POWERUPS) << 5, 0, 32, 32);
		else //Normal Powerup
			spr_storedpoweruplarge.draw(ix + 16, drawbonusitemy, bonusitem << 5, 0, 32, 32);
	}

	if(state >= 2)
		spr->draw(ix, iy, 64, 0, iw, ih);
}

bool MO_BonusHouseChest::collide(CPlayer * player)
{
	if(state == 1 && !game_values.gamemode->gameover && player->playerKeys->game_turbo.fPressed)
	{
		if(bonusitem < NUM_POWERUPS + NUM_WORLD_POWERUPS)
		{
			if(game_values.worldpowerupcount[player->teamID] < 32)
				game_values.worldpowerups[player->teamID][game_values.worldpowerupcount[player->teamID]++] = bonusitem;
			else
				game_values.worldpowerups[player->teamID][31] = bonusitem;
		}
		else
		{
			short iBonus = bonusitem - NUM_POWERUPS - NUM_WORLD_POWERUPS;
			if(iBonus < 10)
				iBonus = iBonus + 1;
			else
				iBonus = 9 - iBonus;

			game_values.tournament_scores[player->teamID].total += iBonus;

			if(game_values.tournament_scores[player->teamID].total < 0)
				game_values.tournament_scores[player->teamID].total = 0;
		}

		ifsoundonplay(sfx_treasurechest);
		state = 2;

		drawbonusitemy = iy + 32;
		drawbonusitemtimer = 75;

		game_values.forceexittimer = 180;
		game_values.gamemode->gameover = true;
		game_values.gamemode->winningteam = player->teamID;
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
	if(state > 0)
	{
		velx = 0.0f;

		dead = true;
		ifsoundonplay(sfx_storepowerup);
		game_values.gamepowerups[player->globalID] = 7;
	}

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
	if(state > 0)
	{
		player->SetPowerup(0);
		dead = true;
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
	if(state > 0)
	{
		dead = true;
		ifsoundonplay(sfx_storepowerup);
		game_values.gamepowerups[player->globalID] = 9;
	}

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
	if(state > 0)
	{
		dead = true;
		ifsoundonplay(sfx_storepowerup);
		game_values.gamepowerups[player->globalID] = 16;
	}

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
	if(state > 0)
	{
		dead = true;
		ifsoundonplay(sfx_storepowerup);
		game_values.gamepowerups[player->globalID] = 10;
	}

	return false;
}

//------------------------------------------------------------------------------
// class feather powerup
//------------------------------------------------------------------------------
PU_FeatherPowerup::PU_FeatherPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
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
	if(state > 0)
	{
		player->SetPowerup(3);
		dead = true;
	}

	return false;
}

//------------------------------------------------------------------------------
// class leaf powerup
//------------------------------------------------------------------------------
PU_LeafPowerup::PU_LeafPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
	PU_FeatherPowerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{}

bool PU_LeafPowerup::collide(CPlayer * player)
{
	if(state > 0)
	{
		player->SetPowerup(7);
		dead = true;
	}

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
	if(state > 0)
	{
		player->SetPowerup(4);
		dead = true;
	}

	return false;
}	


//------------------------------------------------------------------------------
// class special heart powerup for health mode
//------------------------------------------------------------------------------
PU_ExtraHeartPowerup::PU_ExtraHeartPowerup(gfxSprite *nspr, short x, short y) :
	MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{
	velx = 0.0f;
}

bool PU_ExtraHeartPowerup::collide(CPlayer * player)
{
	if(state > 0)
	{
		if(game_values.gamemode->gamemode == game_mode_health)
		{
			if(player->score->subscore[1] < game_values.gamemodesettings.health.maxlife)
				player->score->subscore[1]++;

			if(player->score->subscore[0] < game_values.gamemodesettings.health.maxlife)
				player->score->subscore[0]++;
		}

		dead = true;
	}

	return false;
}

//------------------------------------------------------------------------------
// class special extra time powerup for timed or star mode
//------------------------------------------------------------------------------
PU_ExtraTimePowerup::PU_ExtraTimePowerup(gfxSprite *nspr, short x, short y) :
	MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{
	velx = 0.0f;
}

bool PU_ExtraTimePowerup::collide(CPlayer * player)
{
	if(state > 0)
	{
		if(game_values.gamemode->gamemode == game_mode_timelimit || game_values.gamemode->gamemode == game_mode_star)
		{
			CGM_TimeLimit * timelimitmode = (CGM_TimeLimit*)game_values.gamemode;
			timelimitmode->addtime(30);
		}

		dead = true;
	}

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

	iPlayerID = iGlobalID;
	iTeamID = iTeamID;

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
	if(iPlayerID != player->globalID && (game_values.teamcollision == 2|| iTeamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);

			if(!player->invincible)
			{
				//Find the player that shot this fireball so we can attribute a kill
				PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_fireball, false);
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

	iPlayerID = iGlobalID;
	iTeamID = iTeamID;
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
	animate();

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
	if(iPlayerID != player->globalID && (game_values.teamcollision == 2|| iTeamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);

			if(!player->invincible)
			{
				//Find the player that shot this hammer so we can attribute a kill
				PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_fireball, false);
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

	iPlayerID = iGlobalID;
	iTeamID = iTeamID;
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

	//Detection collision with hammer breakable blocks
	IO_Block * blocks[4];
	GetCollisionBlocks(blocks);
	for(short iBlock = 0; iBlock < 4; iBlock++)
	{
		if(blocks[iBlock] && blocks[iBlock]->getBlockType() == block_weaponbreakable)
		{
			B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)blocks[iBlock];
			if(weaponbreakableblock->iType == 5)
			{
				weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
				removeifprojectile(this, false, false);
				return;
			}
		}
	}
}

bool MO_Hammer::collide(CPlayer * player)
{
	if(iPlayerID != player->globalID && (game_values.teamcollision == 2|| iTeamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);

			if(!player->invincible)
			{
				//Find the player that shot this hammer so we can attribute a kill
				PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_hammer, false);
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

	iPlayerID = iGlobalID;
	iTeamID = iTeamID;
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
	{
		removeifprojectile(this, false, true);
		return;
	}

	//Detection collision with hammer breakable blocks
	IO_Block * blocks[4];
	GetCollisionBlocks(blocks);
	for(short iBlock = 0; iBlock < 4; iBlock++)
	{
		if(blocks[iBlock] && blocks[iBlock]->getBlockType() == block_weaponbreakable)
		{
			B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)blocks[iBlock];
			if(weaponbreakableblock->iType == 5)
			{
				weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
				removeifprojectile(this, false, true);
				return;
			}
		}
	}

}

bool MO_SledgeHammer::collide(CPlayer * player)
{
	if(iPlayerID != player->globalID && (game_values.teamcollision == 2|| iTeamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);
			
			if(!player->invincible)
			{
				//Find the player that shot this hammer so we can attribute a kill
				PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_sledge, false);
				return true;
			}
		}
	}

	return false;
}

void MO_SledgeHammer::explode()
{
	if(iPlayerID > -1 && iy < 480)
	{
		if(fSuper)
		{
			objectcontainer[2].add(new MO_Explosion(&spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, iPlayerID, iTeamID, kill_style_sledge));
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
				objectcontainer[2].add(new MO_Hammer(&spr_hammer, iCenterX, iCenterY, 6, dVelX, dVelY, 5, iPlayerID, iTeamID, iColorID, true));
			}

			projectiles[iPlayerID] += 3;

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

	iPlayerID = iGlobalID;
	iTeamID = iTeamID;
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

	animate();

	if(game_values.superboomerang[iPlayerID] == 2)
	{
		removeifprojectile(this, false, true);

		objectcontainer[2].add(new MO_Explosion(&spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, iPlayerID, iTeamID, kill_style_boomerang));
		ifsoundonplay(sfx_bobombsound);
	}

	//Detection collision with boomerang breakable blocks
	IO_Block * blocks[4];
	GetCollisionBlocks(blocks);
	for(short iBlock = 0; iBlock < 4; iBlock++)
	{
		if(blocks[iBlock] && blocks[iBlock]->getBlockType() == block_weaponbreakable)
		{
			B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)blocks[iBlock];
			if(weaponbreakableblock->iType == 4)
			{
				weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
				forcedead();
				return;
			}
		}
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
				CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

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
			CPlayer * player = GetPlayerFromGlobalID(iPlayerID);
			
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
	CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

	if(player)
	{
		if(player->projectilelimit > 0)
			player->DecreaseProjectileLimit();
	}
}

bool MO_Boomerang::collide(CPlayer * player)
{
	if(iPlayerID != player->globalID && (game_values.teamcollision == 2|| iTeamID != player->teamID))
	{
		if(!player->spawninvincible)
		{
			removeifprojectile(this, false, false);

			if(!player->invincible)
			{
				//Find the player that shot this boomerang so we can attribute a kill
				PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_boomerang, false);
				return true;
			}
		}
	}
	else if(iPlayerID == player->globalID && fFlipped)
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
	MO_CarriedObject(nspr, x, y, 5, aniSpeed, 24, 24, 4, 13)
{
	iw = 28;
	ih = 38;

	bounce = GRAVITATION;

	iPlayerID = iGlobalID;
	iTeamID = iTeamID;
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

			if(iPlayerID == -1)
			{
				iPlayerID = owner->globalID;
				iTeamID = owner->teamID;
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
		applyfriction();

		//Collision detect map
		fOldX = fx;
		fOldY = fy;

		collision_detection_map();
	}

	animate();
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

	if(iPlayerID > -1 && projectiles[iPlayerID] > 0)
		projectiles[iPlayerID]--;

	dead = true;
	objectcontainer[2].add(new MO_Explosion(&spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, iPlayerID, iTeamID, kill_style_bomb));
	ifsoundonplay(sfx_bobombsound);
}

//------------------------------------------------------------------------------
// class coin (for coin mode)
//------------------------------------------------------------------------------
MO_Coin::MO_Coin(gfxSprite *nspr, float dvelx, float dvely, short ix, short iy, short color, short type, short uncollectabletime) :
	IO_MovingObject(nspr, ix, iy, 4, 8, 30, 30, 1, 1, 0, color << 5, 32, 32)
{
	state = 1;
	objectType = object_coin;
	bounce = GRAVITATION;

	sparkleanimationtimer = 0;
	sparkledrawframe = 0;

	iType = type;

	iUncollectableTime = uncollectabletime;
	velx = dvelx;
	vely = dvely;
	
	if(iType == 0)
		placeCoin();
	else
		collision_detection_checksides();
}

bool MO_Coin::collide(CPlayer * player)
{
	if(iType == 1 && iUncollectableTime > 0)
		return false;

	if(!game_values.gamemode->gameover)
		player->score->AdjustScore(1);

	eyecandyfront.add(new EC_SingleAnimation(&spr_coinsparkle, ix, iy, 7, 4));

	ifsoundonplay(sfx_coin);
	
	if(iType == 0)
		placeCoin();
	else
		dead = true;

	return false;
}

void MO_Coin::update()
{
	if(iType == 0)
	{
		animate();

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
	else
	{
		applyfriction();
		IO_MovingObject::update();

		if(--iUncollectableTime < -120)
		{
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix, iy, 3, 8));
			dead = true;
		}
	}
}

void MO_Coin::draw()
{
	IO_MovingObject::draw();

	//Draw sparkles
	if(iType == 0)
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
	while(objectcontainer[1].getClosestObject(x, y, object_coin) <= 150.0f);

	xi(x);
	yi(y);
}

//------------------------------------------------------------------------------
// class OverMapObject - moving objects that don't collide with map or objects, just player
//------------------------------------------------------------------------------
IO_OverMapObject::IO_OverMapObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
	CObject(nspr, x, y)
{
	objectType = object_overmap;
	movingObjectType = movingobject_none;

	iNumSprites = iNumSpr;

	if(iAnimationWidth > -1)
		iw = iAnimationWidth;
	else if(spr)
		iw = (short)spr->getWidth() / iNumSprites;

	if(iAnimationHeight > -1)
		ih = iAnimationHeight;

	animationspeed = aniSpeed;
	animationtimer = 0;

	if(spr)
		animationWidth = (short)spr->getWidth();

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
}

void IO_OverMapObject::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}


void IO_OverMapObject::update()
{
	xf(fx + velx);
	yf(fy + vely);

	animate();
}

void IO_OverMapObject::animate()
{
	if(animationspeed > 0 && ++animationtimer == animationspeed)
	{
		animationtimer = 0;
		drawframe += iw;
		if(drawframe >= animationWidth)
			drawframe = animationOffsetX;
	}
}


//------------------------------------------------------------------------------
// class thwomp (for thwomp mode)
//------------------------------------------------------------------------------
OMO_Thwomp::OMO_Thwomp(gfxSprite *nspr, short x, float nspeed) :
	IO_OverMapObject(nspr, x, (short)-nspr->getHeight(), 1, 0)
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
		return player->KillPlayerMapHazard(false, kill_style_environment) != player_kill_nonkill;

	return false;
}


//------------------------------------------------------------------------------
// class podobo (for survival mode)
//------------------------------------------------------------------------------
MO_Podobo::MO_Podobo(gfxSprite *nspr, short x, float dVelY, short playerid, short teamid, short colorid) :
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

void MO_Podobo::update()
{
	//Special slow podobo gravity
	vely += 0.2f;

	xf(fx + velx);
	yf(fy + vely);

	animate();

	if(iy > 479 && vely > 0.0f)
		dead = true;
}

void MO_Podobo::draw()
{
	spr->draw(ix, iy, drawframe, iColorOffsetY + (vely > 0.0f ? 32 : 0), iw, ih);
}

bool MO_Podobo::collide(CPlayer * player)
{
	if(player->globalID != iPlayerID && (game_values.teamcollision == 2|| iTeamID != player->teamID) && !player->invincible && !player->spawninvincible)
	{
		//Find the player that made this explosion so we can attribute a kill
		PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_podobo, false);
		return true;
	}

	return false;
}

void MO_Podobo::collide(IO_MovingObject * object)
{
	if(iPlayerID == -1)
		return;

	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill)
	{
		//Same team bullet bills don't kill each other
		if(type == movingobject_bulletbill && ((MO_BulletBill*) object)->iTeamID == iTeamID)
			return;

		if(type == movingobject_shell)
			((CO_Shell*)object)->Die();
		else if(type == movingobject_throwblock)
			((CO_ThrowBlock*) object)->Die();
		else if(type == movingobject_bulletbill)
			((MO_BulletBill*) object)->Die();

		ifsoundonplay(sfx_kicksound);
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
	if(player->globalID != iPlayerID && (game_values.teamcollision == 2|| iTeamID != player->teamID) && !player->invincible && !player->spawninvincible)
	{
		//Find the player that made this explosion so we can attribute a kill
		PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_fireball, false);
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
// class thwomp (for thwomp mode)
//------------------------------------------------------------------------------
MO_BulletBill::MO_BulletBill(gfxSprite *nspr, gfxSprite *nsprdead, short x, short y, float nspeed, short playerID, bool isspawned) :
	IO_MovingObject(nspr, x, y, 3, 8, 30, 28, 1, 2)
{
	spr_dead = nsprdead;

	velx = nspeed;
	vely = 0.0f;
	
	movingObjectType = movingobject_bulletbill;
	state = 1;

	fIsSpawned = isspawned;

	ih = 32;
	iw = 32;

	if(fIsSpawned)
	{
		iPlayerID = -1;
		iColorID = 0;
		iTeamID = -1;

		animationspeed = 0;

		if(velx < 0.0f)
		{
			iHiddenDirection = 1;
			iHiddenPlane = ix;
		}
		else
		{
			iHiddenDirection = 3;
			iHiddenPlane = ix + TILESIZE;
		}
	}
	else
	{
		if(velx < 0.0f)
			xi(640 + iw);
		else
			xi(-iw);

		iPlayerID = playerID;
		iColorID = game_values.colorids[iPlayerID];
		iTeamID = LookupTeamID(iPlayerID);
	}

	iColorOffsetY = 64 * iColorID;
	SetDirectionOffset();
}

void MO_BulletBill::update()
{
	xf(fx + velx);
	//yf(fy + vely);

	animate();

	if((velx < 0.0f && ix < -iw) || (velx > 0.0f && ix > 640))
		dead = true;
}

void MO_BulletBill::draw()
{
	if(fIsSpawned)
		spr->draw(ix, iy, drawframe, iColorOffsetY + iDirectionOffsetY, iw, ih, iHiddenDirection, iHiddenPlane);
	else
		spr->draw(ix, iy, drawframe, iColorOffsetY + iDirectionOffsetY, iw, ih);
}

bool MO_BulletBill::collide(CPlayer * player)
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


bool MO_BulletBill::hittop(CPlayer * player)
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

bool MO_BulletBill::hitother(CPlayer * player)
{
	if(player->spawninvincible || player->globalID == iPlayerID)
		return false;

	if(game_values.teamcollision != 2 && iTeamID == player->teamID)
		return false;

	//Find the player that owns this bullet bill so we can attribute a kill
	PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_bulletbill, false);

	return true;
}

void MO_BulletBill::collide(IO_MovingObject * object)
{
	removeifprojectile(object, true, false);

	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_bulletbill)
	{
		MO_BulletBill * bulletbill = (MO_BulletBill*) object;

		//Same team bullet bills don't kill each other
		if(bulletbill->iTeamID == iTeamID)
			return;

		bulletbill->dead = true;
		dead = true;

		short iOffsetX = 0;
		if(ix + iw < bulletbill->ix)
			iOffsetX = 640;
		else if(bulletbill->ix + bulletbill->iw < ix)
			iOffsetX = -640;

		short iCenterX = ((ix + iOffsetX - bulletbill->ix) >> 1) + (bulletbill->ix + (bulletbill->iw >> 1));
		short iCenterY = ((iy - bulletbill->iy) >> 1) + (bulletbill->iy + (bulletbill->ih >> 1));

		objectcontainer[2].add(new MO_Explosion(&spr_explosion, iCenterX - 96, iCenterY - 64, 2, 4, -1, -1, kill_style_bulletbill));
		ifsoundonplay(sfx_bobombsound);
	}
	else if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_attackzone || type == movingobject_explosion)
	{
		//Don't kill things with shells that are sitting still
		if(type == movingobject_shell && object->state == 2)
			return;

		if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_attackzone)
		{
			object->Die();
		}

		ifsoundonplay(sfx_kicksound);
		Die();
	}
}

void MO_BulletBill::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(spr_dead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, velx > 0 ? 0 : 32, iColorID * 32, 32, 32));
}

void MO_BulletBill::SetDirectionOffset()
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
CO_Egg::CO_Egg(gfxSprite *nspr, short iColor) :
	MO_CarriedObject(nspr, 0, 0, 2, 8, 30, 30, 1, 1, 0, iColor << 5, 32, 32)
{
	state = 1;
	bounce = GRAVITATION;
	objectType = object_egg;
	movingObjectType = movingobject_egg;

	owner_throw = NULL;
	owner_throw_timer = 0;

	sparkleanimationtimer = 0;
	sparkledrawframe = 0;
	
	color = iColor;

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

		applyfriction();

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
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 64, color << 5, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 64, color << 5, iw, ih);
	}
	else
	{
		if(velx != 0.0f)
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 64, color << 5, iw, ih);  //keep the egg still while it's moving
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, color << 5, iw, ih);
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
	while(objectcontainer[1].getClosestObject(x, y, object_yoshi) < 250.0f);

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
		applyfriction();
		
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
	IO_OverMapObject(nspr, 1280, 960, 5, 0)  //use 1280 and 960 so when placing base, it doesn't interfere (look in getClosestObject())
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

	placeFlagBase(true);

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
	else if(ix + collisionWidth >= 640)
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
	else if(iy + collisionHeight >= 480)
	{
		vely = -vely;
		iy = 479 - collisionHeight;
		fy = (float)iy;

		angle = atan2(velx, vely);
	}
}

void OMO_FlagBase::placeFlagBase(bool fInit)
{
	short x = 0, y = 0;

	if(fInit && teamID < g_map.iNumFlagBases)
	{
		x = g_map.flagbaselocations[teamID].x;
		y = g_map.flagbaselocations[teamID].y;
	}
	else
	{
		short tries = 0;
		do
		{
			if(++tries > 32)
				break;

			g_map.findspawnpoint(1, &x, &y, collisionWidth, collisionHeight, true);
		}
		while(objectcontainer[0].getClosestObject(x, y, object_flagbase) <= 200.0f);
	}

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
			placeFlagBase(false);
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

		applyfriction();

		//Collision detect map
		fOldX = fx;
		fOldY = fy;

		collision_detection_map();
	}

	animate();
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
OMO_Yoshi::OMO_Yoshi(gfxSprite *nspr, short iColor) :
	IO_OverMapObject(nspr, 0, 0, 2, 8, 52, 56, 0, 0, 0, iColor * 56, 56, 52)
{
	objectType = object_yoshi;
	state = 1;

	color = iColor;

	placeYoshi();
}

bool OMO_Yoshi::collide(CPlayer * player)
{
	if(player->carriedItem && player->carriedItem->getObjectType() == object_egg)
	{
		CO_Egg * egg = (CO_Egg*)player->carriedItem;

		if(egg->color == color)
		{
			if(!game_values.gamemode->gameover)
				player->score->AdjustScore(1);
		
			placeYoshi();

			CO_Egg * egg = (CO_Egg*)player->carriedItem;
			egg->placeEgg();

			ifsoundonplay(sfx_yoshi);
		}
	}

	return false;
}

void OMO_Yoshi::update()
{
	animate();

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

		int upperLeft = g_map.map(ixl, iyt);
		int upperRight = g_map.map(ixr, iyt);
		int lowerLeft = g_map.map(ixl, iyb);
		int lowerRight = g_map.map(ixr, iyb);

		if((upperLeft & tile_flag_solid) == 0 && (upperRight & tile_flag_solid) == 0 &&
			(lowerLeft & tile_flag_solid) == 0 && (lowerRight & tile_flag_solid) == 0 && 
			!g_map.block(ixl, iyt) && !g_map.block(ixr, iyt) && !g_map.block(ixl, iyb) && !g_map.block(ixr, iyb))
		{
			//spawn on ground, but not on spikes
			short iDeathY = (iy+ih)/TILESIZE;
			short iDeathX1 = ix/TILESIZE;
			short iDeathX2 = (ix+iw)/TILESIZE;
			
			while(iDeathY < MAPHEIGHT)
			{
				int ttLeftTile = g_map.map(iDeathX1, iDeathY);
				int ttRightTile = g_map.map(iDeathX2, iDeathY);

				if(((ttLeftTile & tile_flag_solid || ttLeftTile & tile_flag_solid_on_top) && (ttLeftTile & tile_flag_death_on_top) == 0) ||  
					((ttRightTile & tile_flag_solid || ttRightTile & tile_flag_solid_on_top) && (ttRightTile & tile_flag_death_on_top) == 0) ||
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
				else if(ttLeftTile & tile_flag_death_on_top || ttRightTile & tile_flag_death_on_top)
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

		if(egg->color == color && egg->owner_throw)
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
	IO_OverMapObject(nspr, 1280, 960, 5, 0)
{
	iw = (short)spr->getWidth() / 5;
	collisionWidth = iw;

	objectType = object_area;
	iPlayerID = -1;
	iTeamID = -1;

	colorID = -1;
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

	if(iPlayerID != -1 && !game_values.gamemode->gameover)
	{
		if(++scoretimer >= (game_values.pointspeed << 1))
		{
			scoretimer = 0;
			list_players[iPlayerID]->score->AdjustScore(1);
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
	while(objectcontainer[0].getClosestObject(x, y, object_area) <= (200.0f - ((numareas - 3) * 25.0f)));

	xi(x);
	yi(y);
}

void OMO_Area::reset()
{
	iPlayerID = -1;
	iTeamID = -1;

	colorID = -1;
	scoretimer = 0;
	frame = 0;
}

void OMO_Area::setOwner(CPlayer * player)
{
	if(colorID != player->colorID)
	{
		iPlayerID = player->localID;
		iTeamID = player->teamID;
		colorID = player->colorID;

		frame = (colorID + 1) * iw;
		ifsoundonplay(sfx_areatag);

		/*
		if(game_values.secrets)
		{
			//See if all the areas are owned by this player to have a boss peek
			bool fAllTagged = true;
			short iNumBases = 0;
			for(short iObject = 0; iObject < objectcontainer[0].list_end; iObject++)
			{
				if(objectcontainer[0].list[iObject]->getObjectType() == object_area)
				{
					iNumBases++;

					OMO_Area * area = (OMO_Area*)objectcontainer[0].list[iObject];
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
				}
			}
		}*/
	}
}

//------------------------------------------------------------------------------
// class KingOfTheHillArea (for King of the Hill mode)
//------------------------------------------------------------------------------
OMO_KingOfTheHillZone::OMO_KingOfTheHillZone(gfxSprite *nspr) :
	IO_OverMapObject(nspr, 0, 0, 5, 0)
{
	size = game_values.gamemodesettings.kingofthehill.areasize;

	if(size < 2)
		game_values.gamemodesettings.kingofthehill.areasize = size = 2;

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
	iPlayerID = -1;
	
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
		iPlayerID = playersTouching[iMaxTeam]->localID;
		frame = (colorID + 1) * TILESIZE * 3;
	}
	else
	{
		colorID = -1;
		iPlayerID = -1;
		frame = 0;
	}

	if(iPlayerID != -1 && !game_values.gamemode->gameover)
	{
		scoretimer += (iMax << 1) - totalTouchingPlayers;  //Speed of point accumulation is proportional to how many players are in zone

		if(scoretimer >= game_values.pointspeed)
		{
			scoretimer = 0;
			list_players[iPlayerID]->score->AdjustScore(1);
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
	iPlayerID = -1;
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
				int type = g_map.map(x + iCol, iFindY);
				if(((type & tile_flag_solid_on_top || type & tile_flag_solid) && (type & tile_flag_death_on_top) == 0) || g_map.block(x + iCol, iFindY))
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
				if((g_map.map(x + iCol, y + iRow) & tile_flag_solid) || !g_map.spawn(1, x + iCol, y + iRow) || g_map.block(x + iCol, y + iRow))
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
	iPlayerID = -1;
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
	quantity = game_values.gamemodesettings.race.quantity;
	isfinishline = goalID == quantity - 1;
}

bool OMO_RaceGoal::collide(CPlayer * player)
{
	if(game_values.gamemode->getgamemode() == game_mode_race && player->statue_timer == 0)
	{
		CGM_Race * gamemode = (CGM_Race*)game_values.gamemode;
	
		if(tagged[player->teamID] != player->colorID && gamemode->getNextGoal(player->teamID) >= goalID)
		{
			tagged[player->teamID] = player->colorID;
			
			if(isfinishline)
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
	if(isfinishline)
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
	else if(ix + collisionWidth >= 640)
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
	else if(iy + collisionHeight >= 480)
	{
		vely = -vely;
		iy = 479 - collisionHeight;
		fy = (float)iy;

		angle = atan2(velx, vely);
	}
}

void OMO_RaceGoal::placeRaceGoal()
{
	short x = 0, y = 0;

	if(goalID < g_map.iNumRaceGoals)
	{
		x = g_map.racegoallocations[goalID].x;
		y = g_map.racegoallocations[goalID].y;
	}
	else
	{
		short tries = 0;
		do
		{
			if(++tries > 32)
				break;

			x = (short)(rand() % (640 - collisionWidth));
			y = (short)(rand() % (480 - collisionHeight));
		}
		while(objectcontainer[2].getClosestObject(x, y, object_race_goal) <= 250.0f - (quantity * 25.0f));
	}

	xi(x);
	yi(y);
}

//------------------------------------------------------------------------------
// class frenzycard (for fire frenzy mode)
//------------------------------------------------------------------------------
MO_FrenzyCard::MO_FrenzyCard(gfxSprite *nspr, short iType) :
	IO_MovingObject(nspr, 0, 0, 12, 8, -1, -1, -1, -1, 0, iType * 32, 32, 32)
{
	state = 1;
	objectType = object_frenzycard;
	type = iType;
	
	sparkleanimationtimer = 0;
	sparkledrawframe = 0;

	placeCard();
}

bool MO_FrenzyCard::collide(CPlayer * player)
{
	if(type < 8 || game_values.gamemodesettings.frenzy.storedshells)
	{
		//Hack to skip sledge hammer, bombs and leaf for now
		if(type > 4)
			type += 4;

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
				objectcontainer[1].add(shell);
				shell->UsedAsStoredPowerup(player);
				break;
			}
			case 9:
			{
				CO_Shell * shell = new CO_Shell(1, 0, 0, false, true, true, false);
				objectcontainer[1].add(shell);
				shell->UsedAsStoredPowerup(player);
				break;
			}
			case 10:
			{
				CO_Shell * shell = new CO_Shell(2, 0, 0, false, false, true, true); 
				objectcontainer[1].add(shell);
				shell->UsedAsStoredPowerup(player);
				break;
			}
			case 11:
			{
				CO_Shell * shell = new CO_Shell(3, 0, 0, false, true, false, false); 
				objectcontainer[1].add(shell);
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
	animate();

	if(++sparkleanimationtimer >= 4)
	{
		sparkleanimationtimer = 0;
		sparkledrawframe += 32;
		if(sparkledrawframe >= 480)
			sparkledrawframe = 0;
	}

	if(++timer > 1500)
		placeCard();
}

void MO_FrenzyCard::draw()
{
	IO_MovingObject::draw();
	
	//Draw sparkles
	spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_FrenzyCard::placeCard()
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
	while(objectcontainer[1].getClosestObject(x, y, object_frenzycard) <= 150.0f);

	xi(x);
	yi(y);
}

//------------------------------------------------------------------------------
// class collection card (for card collection mode)
//------------------------------------------------------------------------------
MO_CollectionCard::MO_CollectionCard(gfxSprite *nspr, short iType, short iValue, short iUncollectableTime, float dvelx, float dvely, short ix, short iy) :
	IO_MovingObject(nspr, ix, iy, 6, 8, -1, -1, -1, -1, 0, 0, 32, 32)
{
	state = 1;
	objectType = object_collectioncard;
	bounce = GRAVITATION;
	
	sparkleanimationtimer = 0;
	sparkledrawframe = 0;

	type = iType;
	value = iValue;

	uncollectabletime = iUncollectableTime;
	velx = dvelx;
	vely = dvely;
	
	if(iType == 0)
	{
		placeCard();
	}
	else
	{
		collision_detection_checksides();
		animationOffsetY = (value + 1) << 5;
	}
}

bool MO_CollectionCard::collide(CPlayer * player)
{
	//If it is not collectable, return
	if((type == 1 && uncollectabletime > 0) || state != 1)
		return false;

	//Add this card to the team's score
	if(player->score->subscore[0] < 3)
	{
		player->score->subscore[1] |= value << (player->score->subscore[0] << 1);
		player->score->subscore[0]++;
	}
	else
	{
		player->score->subscore[1] &= ~48; //Clear previous card in 3rd slot
		player->score->subscore[1] |= value << 4; //Set card to newly collected one in 3rd slot
	}

	player->score->subscore[2] = 0;

	if(type == 1)
		dead = true;
	else
	{
		state = 2;
		animationspeed = 4;
		animationtimer = 0;
		animationOffsetY = animationOffsetY = (value + 1) << 5;
		drawframe = 96;
	}

	timer = 0;

	return false;
}

void MO_CollectionCard::update()
{
	if(type == 1 || state < 3)
		animate();
	
	//Handle flipping over a card to reveal it's value
	if(state == 2 && drawframe == 0)
	{
		state = 3;
		timer = 0;
	}
	else if(state == 3)
	{
		if(++timer > 200)
		{
			dead = true;
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix, iy, 3, 8));
		}
	}


	if(++sparkleanimationtimer >= 4)
	{
		sparkleanimationtimer = 0;
		sparkledrawframe += 32;
		if(sparkledrawframe >= 480)
			sparkledrawframe = 0;
	}

	if(type == 0)
	{
		if(++timer > 1500)
			placeCard();
	}
	else
	{
		applyfriction();
		IO_MovingObject::update();

		if(--uncollectabletime < -300)
		{
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix, iy, 3, 8));
			dead = true;
		}
	}
}

void MO_CollectionCard::draw()
{
	IO_MovingObject::draw();
	
	//Draw sparkles
	spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_CollectionCard::placeCard()
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
	while(objectcontainer[1].getClosestObject(x, y, object_collectioncard) <= 150.0f);

	xi(x);
	yi(y);
}

//------------------------------------------------------------------------------
// class explosion (for bob-omb mode)
//------------------------------------------------------------------------------
MO_Explosion::MO_Explosion(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short playerid, short iTeamID, killstyle style) :
	IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed)
{
	state = 1;

	iPlayerID = playerid;
	iTeamID = iTeamID;
	timer = 0;
	movingObjectType = movingobject_explosion;
	iStyle = style;
}

bool MO_Explosion::collide(CPlayer * player)
{
	if(player->globalID != iPlayerID && (game_values.teamcollision == 2 || iTeamID != player->teamID) && !player->invincible && !player->spawninvincible)
	{
		//Find the player that made this explosion so we can attribute a kill
		PlayerKilledPlayer(iPlayerID, player, death_style_jump, iStyle, false);
		return true;
	}

	return false;
}

void MO_Explosion::update()
{
	animate();

	//If this is the first frame, look for blocks to kill
	if(timer == 0)
	{
		short iTestY = iy;

		for(short iRow = 0; iRow < 5; iRow++)
		{
			short iTestX = ix;

			if(iTestX < 0)
				iTestX += 640;			

			if(iTestY >= 0 && iTestY < 480)
			{
				short iTestRow = iTestY / TILESIZE;
				for(short iCol = 0; iCol < 7; iCol++)
				{
					IO_Block * block = g_map.block(iTestX / TILESIZE, iTestRow);
					if(block && block->getBlockType() == block_weaponbreakable)
					{
						B_WeaponBreakableBlock * weaponbreakableblock = (B_WeaponBreakableBlock*)block;
						if(weaponbreakableblock->iType == 3)
						{
							weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
						}
					}
				
					iTestX += TILESIZE;

					if(iTestX >= 640)
						iTestX -= 640;
				}
			}

			
			iTestY += TILESIZE;

			if(iTestY >= 480)
				break;
		}
	}

	if(++timer >= 48)
		dead = true;
}

void MO_Explosion::collide(IO_MovingObject * object)
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
			CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

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
		}
		else if(type == movingobject_koopa)
		{
			ifsoundonplay(sfx_kicksound);
			((MO_Koopa*)object)->Die();
		}
		else if(type == movingobject_cheepcheep)
		{
			ifsoundonplay(sfx_kicksound);
			((MO_CheepCheep*)object)->Die();
		}
		else if(type == movingobject_bulletbill)
		{
			ifsoundonplay(sfx_kicksound);
			((MO_BulletBill*)object)->Die();
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
		((MO_SledgeBrother*)object)->Damage(iPlayerID);
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

	return player->KillPlayerMapHazard(false, kill_style_environment) != player_kill_nonkill;
}

void MO_Goomba::collide(IO_MovingObject * object)
{
	if(state == 0)
		return;

	if(!object->GetDead())
	{
		removeifprojectile(object, false, false);

		MovingObjectType type = object->getMovingObjectType();

		if(type == movingobject_fireball || type == movingobject_superfireball || type == movingobject_hammer || type == movingobject_sledgehammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill || type == movingobject_podobo || type == movingobject_attackzone || type == movingobject_explosion)
		{
			//Don't kill goombas with non-moving shells
			if(type == movingobject_shell && object->state == 2)
				return;

			if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
			{
				//Find the player that shot this fireball so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(object->iPlayerID);

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
		
			if(type == movingobject_shell || type == movingobject_throwblock)
			{
				object->CheckAndDie();
			}
			else if(type == movingobject_bulletbill || type == movingobject_attackzone)
			{
				object->Die();
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
	eyecandyfront.add(new EC_FallingObject(&spr_goombadeadflying, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 0, 0, 0, 0));
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

	objectcontainer[1].add(shell);
	
	return false;
}

void MO_Koopa::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(&spr_shelldead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, fRed ? 32 : 0, 0, 32, 32));
}


//------------------------------------------------------------------------------
// class cheep cheep
//------------------------------------------------------------------------------
MO_CheepCheep::MO_CheepCheep(gfxSprite *nspr) :
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

void MO_CheepCheep::update()
{
	fOldX = fx;
	fOldY = fy;

	xf(fx + velx);
	yf(fy + vely);

	//Cheep cheep gravitation
	vely += 0.2f;

	animate();

	//Remove if cheep cheep has fallen below bottom of screen
	if(vely > 0.0f && iy > 480)
		dead = true;
}

void MO_CheepCheep::draw()
{
	spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);
}

bool MO_CheepCheep::collide(CPlayer * player)
{
	if(player->invincible)
	{
		AddAwardKill(player, NULL, kill_style_cheepcheep);
		ifsoundonplay(sfx_kicksound);

		if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
			player->score->AdjustScore(1);
		
		Die();
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


bool MO_CheepCheep::hittop(CPlayer * player)
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

	return false;
}

bool MO_CheepCheep::hitother(CPlayer * player)
{
	if(player->spawninvincible)
		return false;

	return player->KillPlayerMapHazard(false, kill_style_environment) != player_kill_nonkill;
}

void MO_CheepCheep::collide(IO_MovingObject * object)
{
	if(!object->GetDead())
	{
		removeifprojectile(object, false, false);

		MovingObjectType type = object->getMovingObjectType();

		if(type == movingobject_fireball || type == movingobject_superfireball || type == movingobject_hammer || type == movingobject_sledgehammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill || type == movingobject_podobo || type == movingobject_attackzone || type == movingobject_explosion)
		{
			//Don't kill goombas with non-moving shells
			if(type == movingobject_shell && object->state == 2)
				return;

			if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
			{
				//Find the player that shot this projectile so we can attribute a kill
				CPlayer * killer = GetPlayerFromGlobalID(object->iPlayerID);

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
		
			if(type == movingobject_shell || type == movingobject_throwblock)
			{
				object->CheckAndDie();
			}
			else if(type == movingobject_bulletbill || type == movingobject_attackzone)
			{
				object->Die();
			}
		}
	}
}

void MO_CheepCheep::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(&spr_cheepcheepdead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 0, iColorOffsetY, 32, 32));
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
					objectcontainer[2].add(new OMO_Thwomp(&spr_thwomp, (short)(rand() % 591), 2.0f + (float)(rand()%20)/10.0f));
				}

				/*
				short numBombs = rand() % 5 + 6;

				for(short iBomb = 0; iBomb < numBombs; iBomb++)
				{
					short iRandomX = rand() % 612;
					short iRandomY = rand() % 442;
					eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, iRandomX - 2, iRandomY + 3, 3, 8));
					objectcontainer[2].add(new CO_Bomb(&spr_bomb, iRandomX, iRandomY, 0.0f, 0.0f, 4, -1, -1, -1, rand() % 30 + 30));
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
					objectcontainer[2].add(new MO_Podobo(&spr_podobo, (short)(rand() % 608), -(float(rand() % 9) / 2.0f) - 9.0f, -1, -1, -1));
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
		objectcontainer[2].add(new MO_SledgeHammer(&spr_sledgehammer, (face_right ? ix + 32 : ix) - collisionOffsetX, iy, 8, fHammerVelX, -HAMMERTHROW, 5, -1, -1, -1, false));
	}
	else if(iType == 1)
	{
		float fBombVelX = ((float)(rand() % 5 + 12)) / 2.0f - (face_right ? 0.0f : 14.0f);
		float fBombVelY = -(float)(rand() % 13) / 2.0f - 6.0f;
		objectcontainer[2].add(new CO_Bomb(&spr_bomb, face_right ? ix + iw - 32 : ix - 20, iy, fBombVelX, fBombVelY, 4, -1, -1, -1, rand() % 60 + 120));
	}
	else if(iType == 2)
	{
		float fFireVelX = ((float)(rand() % 9 + 6)) / 2.0f - (face_right ? 0.0f : 10.0f);
		float fFireVelY = (float)(rand() % 17) / 2.0f - 4.0f;
		objectcontainer[2].add(new MO_SuperFireball(&spr_superfireball, face_right ? ix + iw - 32 : ix - 16, iy, 4, fFireVelX, fFireVelY, 4, -1, -1, -1));
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

	return player->KillPlayerMapHazard(false, kill_style_environment) != player_kill_nonkill;
}

void MO_SledgeBrother::collide(IO_MovingObject * object)
{
	if(object->GetDead())
		return;

	MovingObjectType type = object->getMovingObjectType();

	//Ignore hammers and fireballs thrown from sledge brother
	if(type == movingobject_sledgehammer || type == movingobject_superfireball)
	{
		if(object->iPlayerID == -1)
			return;
	}

	removeifprojectile(object, false, false);

	if(iType == 0 && (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_attackzone || type == movingobject_explosion))
	{
		if(type == movingobject_shell && object->state == 2)
			return;

		Damage(object->iPlayerID);

		ifsoundonplay(sfx_kicksound);
	
		if(type == movingobject_shell || type == movingobject_throwblock || type == movingobject_attackzone)
		{
			object->Die();
		}
	}
}

void MO_SledgeBrother::Die()
{
	dead = true;
	eyecandyfront.add(new EC_FallingObject(&spr_sledgebrothersdead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 0, iType * 64, iw, ih));
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
	
	iPlayerID = -1;
	iTeamID = -1;

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
	iNoOwnerKillTime = 0;

	fFlipped = false;
	iFlippedOffset = 0;
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
	if(player->invincible || player->fKuriboShoe)
	{
		Die();
		fSmoking = false;
		return false;
	}

	if(fKillBouncePlayer && !fFlipped)
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
		Stop();

		player->yi(iy - PH - 1);
		player->bouncejump();
		player->collision_detection_checktop();
		player->platform = NULL;
	}
	else if(state == 3) //Holding
	{
		if(player != owner && (game_values.teamcollision == 2|| player->teamID != owner->teamID))
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
				iPlayerID = owner->globalID;
				iTeamID = owner->teamID;
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

		if(iNoOwnerKillTime == 0 || player->globalID != iPlayerID || player->ix + HALFPW + flipx >= ix + (iw >> 1) && velx > 0.0f || player->ix + HALFPW + flipx < ix + (iw >> 1) && velx < 0.0f)
			return KillPlayer(player);
	}
	else if(state == 3)  //Holding
	{
		if(player != owner && (game_values.teamcollision == 2|| player->teamID != owner->teamID))
		{
			iPlayerID = owner->globalID;
			iTeamID = owner->teamID;
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
	PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_shell, false);

	CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);
	if(killer && iPlayerID != player->globalID)
	{
		AddMovingKill(killer);
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
	if(object->GetDead())
		return;

	//Don't allow shells to die if they are warping
	if(owner && owner->iswarping())
		return;

	removeifprojectile(object, false, false);

	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_shell)
	{
		CO_Shell * shell = (CO_Shell*)object;

		//Green shells should die on collision, other shells should not,
		//except if they also hit a non dead on collision shell

		if(shell->fSmoking && !fSmoking)
			Die();
		else if(!shell->fSmoking && fSmoking)
			shell->Die();
		else
		{
			if(fDieOnMovingPlayerCollision || state == 2 || (!shell->fDieOnMovingPlayerCollision && shell->state != 2))
				Die();

			if(shell->fDieOnMovingPlayerCollision || shell->state == 2 || (!fDieOnMovingPlayerCollision && state != 2))
				shell->Die();
		}
	}
	else if(type == movingobject_throwblock)
	{
		Die();
		((CO_ThrowBlock*)object)->Die();
	}
	else if(type == movingobject_explosion)
	{
		Die();
	}
	else if(type == movingobject_fireball || type == movingobject_superfireball || type == movingobject_hammer || type == movingobject_sledgehammer || type == movingobject_boomerang)
	{
		if(fDieOnFire)
			Die();
	}
}

void CO_Shell::update()
{
	if(iNoOwnerKillTime > 0)
		iNoOwnerKillTime--;

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
	{
		iDeathTime = 0;
	}
	
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
		IO_MovingObject::update();
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
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
		else
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih);
	}
	else
	{
		if(state == 2)
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih);
		else if(state == 1)
			spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY + iFlippedOffset, iw, ih);
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

	iPlayerID = owner->globalID;
	iTeamID = owner->teamID;

	owner = NULL;
	iNoOwnerKillTime = 30;

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

void CO_Shell::Flip()
{
	if(owner)
	{
		Die();
		return;
	}

	if(!fFlipped)
	{
		fFlipped = true;
		iFlippedOffset = 128;
	}

	Stop();
	vely = -VELJUMP / 2.0;
}

void CO_Shell::Stop()
{
	owner = NULL;
	velx = 0.0f;
	state = 2;
	fSmoking = false;
	ifsoundonplay(sfx_kicksound);
	iKillCounter = 0;
}

//------------------------------------------------------------------------------
// class throwable block projectile
//------------------------------------------------------------------------------
//State 1: Moving
//State 2: Holding
CO_ThrowBlock::CO_ThrowBlock(gfxSprite * nspr, short x, short y, short type) :
	MO_CarriedObject(nspr, x, y, 4, 2, 30, 30, 1, 1)
{
	state = 2;
	ih = 32;
	bounce = GRAVITATION;
	objectType = object_moving;
	movingObjectType = movingobject_throwblock;
	iPlayerID = -1;
	iTeamID = -1;
	
	fDieOnBounce = type != 2;
	fDieOnPlayerCollision = type == 0;

	iType = type;

	iDeathTime = 0;
	iBounceCounter = 0;
	iNoOwnerKillTime = 0;
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

		if(iNoOwnerKillTime == 0 || player->globalID != iPlayerID || player->ix + flipx > ix && velx > 0.0f || player->ix + flipx <= ix && velx < 0.0f)
		{
			return KillPlayer(player);
		}
	}
	else if(state == 2)  //Holding
	{
		if(player != owner)
		{
			iPlayerID = owner->globalID;
			iTeamID = owner->teamID;
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

	CheckAndDie();

	//Find the player that shot this shell so we can attribute a kill
	PlayerKilledPlayer(iPlayerID, player, death_style_jump, kill_style_throwblock, false);
	return true;
}

void CO_ThrowBlock::collide(IO_MovingObject * object)
{
	if(object->GetDead())
		return;

	removeifprojectile(object, false, false);

	MovingObjectType type = object->getMovingObjectType();
	if(type == movingobject_throwblock)
	{
		Die();
		((CO_ThrowBlock*)object)->Die();
	}
	else if(type == movingobject_explosion)
	{
		Die();
	}
}

void CO_ThrowBlock::update()
{
	if(iNoOwnerKillTime > 0)
		iNoOwnerKillTime--;

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

	animate();
}

void CO_ThrowBlock::draw()
{
	if(owner && owner->iswarping())
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iType << 5, iw, ih, (short)owner->state % 4, owner->GetWarpPlane());
	else
		spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iType << 5, iw, ih);

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

	iPlayerID = owner->globalID;
	iTeamID = owner->teamID;

	owner = NULL;
	iNoOwnerKillTime = 30;

	state = 1;

	if(collision_detection_checksides())
		Die();
	else
		ifsoundonplay(sfx_kicksound);
}

void CO_ThrowBlock::CheckAndDie()
{
	if(fDieOnPlayerCollision)
		Die();
}

void CO_ThrowBlock::Die()
{
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy, -1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy, 1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix, iy + 16, -1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));
	eyecandyfront.add(new EC_FallingObject(&spr_brokenblueblock, ix + 16, iy + 16, 1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));
	
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
	if(fDieOnBounce)
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
CO_Spring::CO_Spring(gfxSprite *nspr, short ix, short iy) :
	MO_CarriedObject(nspr, ix, iy, 4, 4, 30, 30, 1, 1)
{
	state = 1;
	bounce = GRAVITATION;
	objectType = object_moving;
	movingObjectType = movingobject_carried;
}

bool CO_Spring::collide(CPlayer * player)
{
	if(owner == NULL)
	{
		if(player->fOldY + PH <= fOldY && player->iy + PH >= iy)
			hittop(player);
		else if(state == 1)
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
	player->extrajumps = 0;

	player->superjumptimer = 4;
	player->superjumptype = 1;
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
	if(owner)
	{
		MoveToOwner();
	}
	else
	{
		applyfriction();

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

void CO_Spring::draw()
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
CO_Spike::CO_Spike(gfxSprite *nspr, short ix, short iy) :
	CO_Spring(nspr, ix, iy)
{
	iw = 32;
	ih = 32;

	movingObjectType = movingobject_carried;
}

void CO_Spike::hittop(CPlayer * player)
{
	if(player->isready() && !player->spawninvincible && !player->invincible && !player->fKuriboShoe)
		player->KillPlayerMapHazard(false, kill_style_environment);
}


//------------------------------------------------------------------------------
// class kuribo's shoe
//------------------------------------------------------------------------------
CO_KuriboShoe::CO_KuriboShoe(gfxSprite *nspr, short ix, short iy) :
	CO_Spring(nspr, ix, iy + 15)
{
	iw = 32;
	ih = 32;

	collisionOffsetY = 15;
	collisionHeight = 16;

	movingObjectType = movingobject_carried;
}

void CO_KuriboShoe::hittop(CPlayer * player)
{
	if(!player->fKuriboShoe)
	{
		dead = true;
		player->SetKuriboShoe();
		ifsoundonplay(sfx_transform);
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, player->ix + HALFPW - 16, player->iy + HALFPH - 16, 3, 8));
	}
}


//------------------------------------------------------------------------------
// attack zone(invisible area that kills objects and players)
//------------------------------------------------------------------------------
MO_AttackZone::MO_AttackZone(short playerId, short teamId, short x, short y, short w, short h, short time, killstyle style, bool dieoncollision) :
	IO_MovingObject(NULL, x, y, 1, 0, w, h, 0, 0)
{
	iPlayerID = playerId;
	iTeamID = teamId;
	iStyle = style;

	objectType = object_moving;
	movingObjectType = movingobject_attackzone;

	iTimer = time;
	fDieOnCollision = dieoncollision;

	state = 1;
}

bool MO_AttackZone::collide(CPlayer * player)
{
	if(player->spawninvincible || player->invincible || dead)
		return false;

	if(game_values.teamcollision != 2 && player->teamID == iTeamID)
		return false;

	CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

	if(killer && killer->globalID == player->globalID)
		return false;

	PlayerKilledPlayer(iPlayerID, player, death_style_jump, iStyle, false);

	Die();

	return true;
}

/*void MO_AttackZone::draw()
{
	if(!dead)
	{
		SDL_Rect r = {ix, iy, collisionWidth, collisionHeight};
		SDL_FillRect(blitdest, &r, 0xf000);
	}
}*/

void MO_AttackZone::update()
{
	if(--iTimer <= 0)
		dead = true;
}

void MO_AttackZone::collide(IO_MovingObject * object)
{
	if(dead)
		return;

	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_shell || type == movingobject_throwblock)
	{
		if(type == movingobject_shell)
		{
			object->Die();
		}
		else if(type == movingobject_throwblock)
		{
			CO_ThrowBlock * block = (CO_ThrowBlock*) object;

			if(!block->owner || block->owner->globalID != iPlayerID)
				block->Die();
		}

		ifsoundonplay(sfx_kicksound);
	}	
}

void MO_AttackZone::Die()
{
	if(fDieOnCollision)
		dead = true;
}


//------------------------------------------------------------------------------
// class spin death (spinning cape or tail)
//------------------------------------------------------------------------------
MO_SpinAttack::MO_SpinAttack(short playerId, short teamId, killstyle style, bool direction, short offsety) :
	MO_AttackZone(playerId, teamId, 0, 0, 24, 12, 16, style, true)
{
	fDirection = direction;
	iOffsetY = offsety;

	state = 0;
	objectType = object_moving;
}

bool MO_SpinAttack::collide(CPlayer * player)
{
	if(iTimer > 11)
		return false;

	return MO_AttackZone::collide(player);
}

/*
void MO_SpinAttack::draw()
{
	if(iTimer <= 11 && !dead)
	{
		SDL_Rect r = {ix, iy, collisionWidth, collisionHeight};
		SDL_FillRect(blitdest, &r, 0xff00);
	}
}
*/

void MO_SpinAttack::update()
{
	MO_AttackZone::update();

	if(iTimer <= 11)
		state = 1;

	CPlayer * owner = GetPlayerFromGlobalID(iPlayerID);

	if(owner)
	{
		//Move to the owner
		xi(owner->ix - PWOFFSET + (fDirection ? 24 : -16));
		yi(owner->iy + PH - iOffsetY);

		if(iTimer < 5 || iy + collisionHeight < 0)
			return;

		//Check block collisions
		short iTop = iy / TILESIZE;
		short iBottom = (iy + collisionHeight) / TILESIZE;

		short iLeft;
		if(ix < 0)			
			iLeft = (ix + 640) / TILESIZE;
		else
			iLeft = ix / TILESIZE;

		short iRight = (ix + collisionWidth) / TILESIZE;

		if(iLeft < 0)
			iLeft += 20;

		if(iLeft >= 20)
			iLeft -= 20;

		if(iRight < 0)
			iRight += 20;

		if(iRight >= 20)
			iRight -= 20;

		IO_Block * topleftblock = NULL;
		IO_Block * toprightblock = NULL;
		IO_Block * bottomleftblock = NULL;
		IO_Block * bottomrightblock = NULL;

		if(iTop >= 0 && iTop < 15)
		{
			topleftblock = g_map.block(iLeft, iTop);
			toprightblock = g_map.block(iRight, iTop);
		}

		if(iBottom >= 0 && iBottom < 15)
		{
			bottomleftblock = g_map.block(iLeft, iBottom);
			bottomrightblock = g_map.block(iRight, iBottom);
		}

		bool fHitBlock = false;
		if(topleftblock && !topleftblock->isTransparent() && !topleftblock->isHidden())
			fHitBlock = topleftblock->collide(this, 3);
		
		if(!fHitBlock && toprightblock && !toprightblock->isTransparent() && !toprightblock->isHidden())
			fHitBlock = toprightblock->collide(this, 1);

		if(!fHitBlock && bottomleftblock && !bottomleftblock->isTransparent() && !bottomleftblock->isHidden())
			fHitBlock = bottomleftblock->collide(this, 3);
		
		if(!fHitBlock && bottomrightblock && !bottomrightblock->isTransparent() && !bottomrightblock->isHidden())
			fHitBlock = bottomrightblock->collide(this, 1);

		if(fHitBlock)
			dead = true;
	}
	else
	{
		dead = true;
	}
}

void MO_SpinAttack::collide(IO_MovingObject * object)
{
	if(dead || iTimer > 11)
		return;

	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_shell || type == movingobject_throwblock)
	{
		if(type == movingobject_shell)
		{
			((CO_Shell*)object)->Flip();
		}
		else if(type == movingobject_throwblock)
		{
			CO_ThrowBlock * block = (CO_ThrowBlock*) object;

			if(!block->owner || block->owner->globalID != iPlayerID)
				block->Die();
		}

		ifsoundonplay(sfx_kicksound);
		Die();
	}	
}

//------------------------------------------------------------------------------
// class OMO Orbit Hazard - component of the fireball string or rotodisc
//------------------------------------------------------------------------------
OMO_OrbitHazard::OMO_OrbitHazard(gfxSprite *nspr, short x, short y, float radius, float vel, float angle, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
	IO_OverMapObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
	objectType = object_orbithazard;
	
	dRadius = radius;
	dVel = vel;
	dAngle = angle;

	dCenterX = (float)x;
	dCenterY = (float)y;

	CalculatePosition();
}

void OMO_OrbitHazard::update()
{
	animate();

	dAngle += dVel;

	if(dAngle < 0.0f)
		dAngle += TWO_PI;
	else if(dAngle >= TWO_PI)
		dAngle -= TWO_PI;

	CalculatePosition();
}

bool OMO_OrbitHazard::collide(CPlayer * player)
{
	if(!player->invincible && !player->spawninvincible)
	{
		return player->KillPlayerMapHazard(false, kill_style_environment) != player_kill_nonkill;
	}

	return false;
}

void OMO_OrbitHazard::CalculatePosition()
{
	xf(dCenterX + dRadius * cos(dAngle) - (float)iw / 2.0f);
	yf(dCenterY + dRadius * sin(dAngle) - (float)ih / 2.0f);
}


//------------------------------------------------------------------------------
// class OMO Straight Path Hazard - straight path fireball
//------------------------------------------------------------------------------
OMO_StraightPathHazard::OMO_StraightPathHazard(gfxSprite *nspr, short x, short y, float angle, float vel, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
	IO_OverMapObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
	objectType = object_pathhazard;
	
	dVel = vel;
	dAngle = angle;

	velx = vel * cos(angle);
	vely = vel * sin(angle);
}

void OMO_StraightPathHazard::update()
{
	IO_OverMapObject::update();

	if(iy + ih < 0 || iy >= 480)
		dead = true;
}

bool OMO_StraightPathHazard::collide(CPlayer * player)
{
	if(!player->spawninvincible)
	{
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + (iw >> 2) - 16, iy + (ih >> 2) - 16, 3, 8));
		dead = true;

		if(!player->invincible)
		{
			return player->KillPlayerMapHazard(false, kill_style_environment) != player_kill_nonkill;
		}
	}

	return false;
}


//------------------------------------------------------------------------------
// class IO_BulletBillCannon - gets update calls and shoots bullet bills based on timer
//------------------------------------------------------------------------------

IO_BulletBillCannon::IO_BulletBillCannon(short x, short y, short freq, float vel) :
	CObject(NULL, x, y)
{
	iFreq = freq;
	dVel = vel;

	SetNewTimer();
}

void IO_BulletBillCannon::update()
{
	if(--iTimer <= 0)
	{
		SetNewTimer();

		objectcontainer[2].add(new MO_BulletBill(&spr_hazard_bulletbill, &spr_hazard_bulletbilldead, ix + (dVel < 0.0f ? 32 : -32), iy, dVel, 0, true));
		ifsoundonplay(sfx_bulletbillsound);
	}
}

void IO_BulletBillCannon::SetNewTimer()
{
	iTimer = iFreq + (rand() % iFreq);
}


//------------------------------------------------------------------------------
// class IO_FlameCannon - shoots a flame
//------------------------------------------------------------------------------

IO_FlameCannon::IO_FlameCannon(short x, short y, short freq, bool isfacingright) :
	CObject(NULL, x, y)
{
	iFreq = freq;
	state = 0;
	SetNewTimer();

	iw = 96;
	ih = 32;

	collisionHeight = 32;
	collisionWidth = 96;
	collisionOffsetX = 0;
	collisionOffsetY = 0;

	if(isfacingright)
	{
		iFlameX = 0;
	}
	else
	{
		ix -= 64;
		iFlameX = 96;
	}
}

void IO_FlameCannon::update()
{
	if(state == 0)  //No flame, waiting
	{
		if(--iTimer <= 0)
		{
			iTimer = 0;
			iCycle = 0;
			iFlameY = 0;

			state = 1;
		}
	}
	else if(state == 1 || state == 3) //Start or end of flame but not deadly yet
	{
		if(++iTimer >= 4)
		{
			iTimer = 0;
			
			iFlameY += 32;
			if(iFlameY > 32)
			{
				iFlameY = 0;

				if(++iCycle >= 4)
				{
					iFlameY = 64;
					iCycle = 0;
					
					if(state == 1)
					{
						state = 2;
					}
					else
					{
						state = 0;
						SetNewTimer();
					}
				}
			}
		}
	}
	else if(state == 2) //Full flame
	{
		if(++iTimer >= 4)
		{
			iTimer = 0;
			
			iFlameY += 32;
			if(iFlameY > 96)
			{
				iFlameY = 64;

				if(++iCycle >= 8)
				{
					state = 3;
					iFlameY = 0;
					iCycle = 0;
				}
			}
		}
	}
}

void IO_FlameCannon::draw()
{
	if(state > 0)
	{
		spr_hazard_flame.draw(ix, iy, iFlameX, iFlameY, 96, 32);
	}
}

bool IO_FlameCannon::collide(CPlayer * player)
{
	if(state == 2 && !player->invincible && !player->spawninvincible)
		return player->KillPlayerMapHazard(false, kill_style_environment) != player_kill_nonkill;

	return false;
}

void IO_FlameCannon::SetNewTimer()
{
	iTimer = iFreq + (rand() % iFreq);
}


//------------------------------------------------------------------------------
// class IO_PirhanaPlant - pirhana plant that appears on a certain frequency
//------------------------------------------------------------------------------

MO_PirhanaPlant::MO_PirhanaPlant(short x, short y, short type, short freq, short direction) :
	IO_MovingObject(NULL, x, y, 0, 0)
{
	iType = type;
	iDirection = direction;
	iFreq = freq;

	movingObjectType = movingobject_pirhanaplant;

	//iHiddenPlane = y;
	//iHiddenDirection = 2 - ((direction / 2) * 2);

	state = 0;
	SetNewTimer();

	iw = 32;

	if(direction == 0)
		iy += 32;

	if(iType == 0 || iType == 1)
	{
		iSrcX = iDirection * 128;
		iSrcY = iType * 48;
	}
	else if(iType == 2 || iType == 3)
	{
		iSrcX = iDirection * 64;
		iSrcY = iType * 48 + (iType == 3 ? 16 : 0);
	}

	if(iType == 2)
		ih = 64;
	else
		ih = 48;

	collisionHeight = 0;
	collisionWidth = 32;
	collisionOffsetX = 0;
	collisionOffsetY = 0;

	iAnimationTimer = 0;
	iAnimationX = 0;

	iActionTimer = rand() % 8;
	iFacing = 0;
}

void MO_PirhanaPlant::update()
{
	if(state == 0)  //waiting to appear
	{
		if(--iTimer <= 0)
		{
			iTimer = 0;
			state = 1;
		}
	}
	else if(state == 1) //appearing
	{
		collisionHeight += 2;

		if(iDirection == 0)
			iy -= 2;

		if(collisionHeight >= ih)
		{
			state = 2;
		}
	}
	else if(state == 2) //extended
	{
		if(++iTimer > 60)
		{
			iTimer = 0;
			state = 3;
		}
	}
	else if(state == 3) //retreating
	{
		collisionHeight -= 2;
		
		if(iDirection == 0)
			iy += 2;

		if(collisionHeight <= 0)
		{
			state = 0;
			SetNewTimer();
		}
	}


	if(iType == 1) //face the plant towards the nearest player
	{
		//Don't do this every frame, just once every 8 frames
		if(state > 0 && ++iActionTimer >= 8)
		{
			int distance_to_player = 640000;
			short iDiffX, iDiffY;

			short iPlantX = ix + 16;
			short iPlantY = iy + (iDirection == 0 ? 16 : ih - 16);

			for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
			{
				CPlayer * player = list_players[iPlayer];
				if(player->state != player_ready)
					continue;

				//Calculate normal screen distance
				short tx = iPlantX - player->ix - PW;
				short ty = iPlantY - player->iy - PH;
				
				int distance_player_pow2 = tx*tx + ty*ty;

				if (distance_player_pow2 < distance_to_player)
				{
					distance_to_player = distance_player_pow2;
					iDiffX = tx;
					iDiffY = ty;
				}
			}

			float dAngle = (float)atan2((double)iDiffX, (double)iDiffY);
			
			if(dAngle >= 0.0f && dAngle < HALF_PI)
				iFacing = 0;
			else if(dAngle >= HALF_PI && dAngle <= PI)
				iFacing = 32;
			else if(dAngle >= -HALF_PI && dAngle < 0.0f)
				iFacing = 64;
			else if(dAngle >= -PI && dAngle < -HALF_PI)
				iFacing = 96;
		}
	}
	else if(iType == 2 || iType == 3) //Animate if these are animated plants
	{
		if(++iAnimationTimer >= 8)
		{
			iAnimationTimer = 0;

			iAnimationX += 32;
			if(iAnimationX > 32)
				iAnimationX = 0;
		}
	}

	//Fire a fireball
	if(iType <= 1 && state == 2 && iTimer == 30)
	{
		objectcontainer[1].add(new OMO_StraightPathHazard(&spr_hazard_fireball, ix + 7, iDirection == 0 ? iy + 7 : iy + ih - 23, GetFireballAngle(), 3.0f, 4, 8, 18, 18, 0, 0, 0, iFacing <= 32 ? 18 : 0, 18, 18));
	}
}

void MO_PirhanaPlant::draw()
{
	if(state > 0)
	{
		if(iDirection == 0)
			spr_hazard_pirhanaplant.draw(ix, iy, iSrcX + iAnimationX + iFacing, iSrcY, 32, collisionHeight);
		else
			spr_hazard_pirhanaplant.draw(ix, iy, iSrcX + iAnimationX + iFacing, iSrcY + ih - collisionHeight, 32, collisionHeight);
	}
}

bool MO_PirhanaPlant::collide(CPlayer * player)
{
	if(state == 0)
		return false;

	if(player->invincible)
	{
		KillPlant();
	}
	else if(!player->spawninvincible)
	{
		return player->KillPlayerMapHazard(false, kill_style_environment) != player_kill_nonkill;
	}

	return false;
}

void MO_PirhanaPlant::collide(IO_MovingObject * object)
{
	if(state == 0)
		return;

	removeifprojectile(object, true, false);

	MovingObjectType type = object->getMovingObjectType();

	if(type == movingobject_fireball || type == movingobject_superfireball || type == movingobject_hammer || type == movingobject_sledgehammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_attackzone || type == movingobject_explosion)
	{
		//Don't kill things with shells that are sitting still
		if(type == movingobject_shell && object->state == 2)
			return;

		if(type == movingobject_shell || type == movingobject_throwblock)
		{
			object->CheckAndDie();
		}
		else if(type == movingobject_attackzone)
		{
			object->Die();
		}

		ifsoundonplay(sfx_kicksound);
		KillPlant();
	}
}

void MO_PirhanaPlant::SetNewTimer()
{
	iTimer = iFreq + (rand() % iFreq);

	//Face the green fireball plant in a random direction
	if(iType == 0)
	{
		//Only point flower towards directions that make sense
		if((ix >> 5) == 19)
			iFacing = rand() % 2;
		else if(ix == 0)
			iFacing = (rand() % 2) + 2;
		else
			iFacing = rand() % 4;

		iFacing <<= 5;
	}
}

void MO_PirhanaPlant::KillPlant()
{
	SetNewTimer();
	state = 0;
	
	if(iDirection == 0)
		iy += collisionHeight;

	collisionHeight = 0;

	eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix, iy - (iDirection == 0 ? 32 : 0), 3, 4));
}

float MO_PirhanaPlant::GetFireballAngle()
{
	if(iFacing == 0)
		return -2.7214f;
	else if(iFacing == 32)
		return 2.7214f;
	else if(iFacing == 64)
		return -0.4202f;
	else if(iFacing == 96)
		return 0.4202f;

	return 0.0f;
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
