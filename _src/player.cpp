#include "global.h"
#include <math.h>

extern bool SwapPlayers(short iUsingPlayerID);
extern void EnterBossMode(short bossType);
extern short g_iWinningPlayer;

CPlayer::CPlayer(short iGlobalID, short iLocalID, short iTeamID, short iSubTeamID, short iColorID, gfxSprite * nsprites[PGFX_LAST], CScore *nscore, short * respawn, CPlayerAI * ai)
{
	globalID = iGlobalID;
	localID = iLocalID;
	teamID = iTeamID;
	subTeamID = iSubTeamID;
	colorID = iColorID;

	//AI stuff
	pPlayerAI = ai;
	if(pPlayerAI)
		pPlayerAI->SetPlayer(this);
	
	playerKeys = &game_values.playerInput.outputControls[iGlobalID];
	playerDevice = game_values.playerInput.inputControls[globalID]->iDevice;

	score = nscore;

	spr = PGFX_JUMPING_R;

	for(short i = 0; i < PGFX_LAST; i++)
		sprites[i] = nsprites[i];

	sprswitch = 0;

	spawninvincible = false;
	spawninvincibletimer = 0;
	
	carriedItem = NULL;
	ownerPlayerID = -1;
	ownerColorOffsetX = 0;
	jailed = 0;

	spawntext = 20;  //set it to 20 so there is an immediate text spawned upon winning
	
	powerupused = -1;

	respawncounter = respawn;
	SetupNewPlayer();
	*respawncounter = 0;
	
	fAcceptingItem = false;
	fPressedAcceptItem = false;

	pSuicideCreditPlayer = NULL;
	iSuicideCreditTimer = 0;

	konamiIndex = 0;
	secret_spring_index = 0;
	secret_spike_index = 0;
}

CPlayer::~CPlayer()
{
	if(pPlayerAI)
		delete pPlayerAI;
}

void CPlayer::move()
{
	if(invincible)
		game_values.playinvinciblesound = true;

	if(game_values.secrets)
	{
		int keymask =
			(playerKeys->game_jump.fPressed?1:0) |
			(playerKeys->game_down.fPressed?2:0) |
			(playerKeys->game_left.fPressed?4:0) |
			(playerKeys->game_right.fPressed?8:0) |
			(playerKeys->game_turbo.fPressed?16:0) |
			(playerKeys->game_powerup.fPressed?32:0);

		for(short bossType = 0; bossType < 3; bossType++)
		{
			if(game_values.bosspeeking == bossType)
			{
				static const int boss_code[3][7] = { {2,1,4,8,16,16,32},
													 {1,1,4,8,2,2,32},
													 {4,8,4,8,16,16,0} };

				static const int boss_code_length[3] = {7, 7, 6};
		        
				if (keymask & boss_code[bossType][boss_index[bossType]]) 
					boss_index[bossType]++;
				else if (keymask & ~boss_code[bossType][boss_index[bossType]]) 
					boss_index[bossType] = 0;
		        
				if (boss_index[bossType] == boss_code_length[bossType])
				{
					boss_index[bossType] = 0;
					EnterBossMode(bossType);
				}
			}
			else
			{
				boss_index[bossType] = 0;
			}
		}

		if (konamiIndex < 11)
		{
			static const int konami_code[11] = {1,1,2,2,4,8,4,8,48,48,48};
	        
			if (keymask & konami_code[konamiIndex]) 
				konamiIndex++;
			else if (keymask & ~konami_code[konamiIndex]) 
				konamiIndex = 0;
	        
			extern int g_tanookiFlag;
			if (konamiIndex == 11)
			{
				ifsoundonplay(sfx_transform); 
				g_tanookiFlag++; 
			}
		}

		//Super kick shells and BTBs
		if(keymask & 2)
			superKickIndex++;
		else if(keymask & ~2)
			superKickIndex = 0;

		//Homing bullet bills
		if (game_values.gamepowerups[globalID] == 10 && homingBillsIndex < 4)
		{
			static const int homingBillsCode[4] = {16, 16, 16, 32};
	        
			if (keymask & homingBillsCode[homingBillsIndex]) 
				homingBillsIndex++;
			else if (keymask & ~homingBillsCode[homingBillsIndex]) 
				homingBillsIndex = 0;
	        
			if (homingBillsIndex == 4)
			{
				homingBillsIndex = 0;
				ifsoundonplay(sfx_transform); 
				homingBills = true;
			}
		}

		if(game_values.gamepowerups[globalID] != 10 && powerupused != 10)
			homingBills = false;

		if (powerup == 2)
		{
			static const int super_hammer_throw_code_left[4] = {8, 8, 4, 16};
		    
			if (keymask & super_hammer_throw_code_left[super_hammer_throw_left_index]) 
				super_hammer_throw_left_index++;
			else if (keymask & ~super_hammer_throw_code_left[super_hammer_throw_left_index])
				super_hammer_throw_left_index = 0;
		    
			if (super_hammer_throw_left_index >= 4)
			{
				super_hammer_throw_left_index = 0;
				shoot_left_super_hammer = true;
			}

			static const int super_hammer_throw_code_right[4] = {4, 4, 8, 16};
		        
			if (keymask & super_hammer_throw_code_right[super_hammer_throw_right_index]) 
				super_hammer_throw_right_index++;
			else if (keymask & ~super_hammer_throw_code_right[super_hammer_throw_right_index])
				super_hammer_throw_right_index = 0;
		    
			if (super_hammer_throw_right_index >= 4)
			{
				super_hammer_throw_right_index = 0;
				shoot_right_super_hammer = true;
			}
		}
		else
		{
			super_hammer_throw_left_index = 0;
			super_hammer_throw_right_index = 0;

			shoot_left_super_hammer = false;
			shoot_right_super_hammer = false;
		}

		if (powerup == 5)
		{
			static const int super_sledge_hammer_throw_code[3] = {2, 2, 16};
		    
			if (keymask & super_sledge_hammer_throw_code[super_sledge_hammer_throw_index]) 
				super_sledge_hammer_throw_index++;
			else if (keymask & ~super_sledge_hammer_throw_code[super_sledge_hammer_throw_index])
				super_sledge_hammer_throw_index = 0;
		    
			if (super_sledge_hammer_throw_index >= 3)
			{
				super_sledge_hammer_throw_index = 0;
				shoot_super_sledge_hammer = true;
			}
		}
		else
		{
			super_sledge_hammer_throw_index = 0;
			shoot_super_sledge_hammer = false;
		}

		if (powerup == 4)
		{
			if(playerKeys->game_left.fDown)
			{
				holdleft++;
				holdlefttolerance = 0;
			}
			else if(holdleft >= 60 && holdlefttolerance < 15)
			{
				holdlefttolerance++;
			}
			else
			{
				holdleft = 0;
				holdlefttolerance = 0;
				super_boomerang_throw_index_right = 0;
			}

			if(playerKeys->game_right.fDown)
			{
				holdright++;
				holdrighttolerance = 0;
			}
			else if(holdright >= 60 && holdrighttolerance < 15)
			{
				holdrighttolerance++;
			}
			else
			{
				holdright = 0;
				holdrighttolerance = 0;
				super_boomerang_throw_index_left = 0;
			}

			if(holdleft >= 60)
			{
				static const int super_boomerang_throw_code_right[2] = {8, 16};
		    
				if (keymask & super_boomerang_throw_code_right[super_boomerang_throw_index_right]) 
					super_boomerang_throw_index_right++;
				else if (keymask & ~super_boomerang_throw_code_right[super_boomerang_throw_index_right])
					super_boomerang_throw_index_right = 0;
				
				if (super_boomerang_throw_index_right >= 2)
				{
					super_boomerang_throw_index_right = 0;
					shoot_super_boomerang = true;
				}
			}

			if(holdright >= 60)
			{
				static const int super_boomerang_throw_code_left[2] = {4, 16};
		    
				if (keymask & super_boomerang_throw_code_left[super_boomerang_throw_index_left]) 
					super_boomerang_throw_index_left++;
				else if (keymask & ~super_boomerang_throw_code_left[super_boomerang_throw_index_left])
					super_boomerang_throw_index_left = 0;
				
				if (super_boomerang_throw_index_left >= 2)
				{
					super_boomerang_throw_index_left = 0;
					shoot_super_boomerang = true;
				}
			}
		}
		else
		{
			holdleft = 0;
			holdlefttolerance = 0;
			
			holdright = 0;
			holdrighttolerance = 0;

			super_boomerang_throw_index_left = 0;
			super_boomerang_throw_index_right = 0;

			shoot_super_boomerang = false;
		}

		if (bobomb)
		{
			static const int super_bobomb_code[7] = {4, 8, 4, 8, 2, 2, 16};
		        
			if (keymask & super_bobomb_code[super_bobomb_index]) 
				super_bobomb_index++;
			else if (keymask & ~super_bobomb_code[super_bobomb_index])
				super_bobomb_index = 0;
		    
			if (super_bobomb_index >= 7)
			{
				super_bobomb_index = 0;
				shoot_super_bobomb = true;
			}
		}
		else
		{
			super_bobomb_index = 0;
			shoot_super_bobomb = false;
		}

		if(game_values.superboomerang[globalID] == 1)
		{
			if(keymask & 16)
				game_values.superboomerang[globalID] = 2;
		}

		//Fly using feather
		if(playerKeys->game_down.fDown)
		{
			holddown++;
			holddowntolerance = 0;
		}
		else if(holddown >= 120 && holddowntolerance < 15)
		{
			holddowntolerance++;
		}
		else
		{
			holddown = 0;
			holddowntolerance = 0;
		}

		int keymaskdown =
			(playerKeys->game_jump.fDown?1:0) |
			(playerKeys->game_down.fDown?2:0) |
			(playerKeys->game_left.fDown?4:0) |
			(playerKeys->game_right.fDown?8:0) |
			(playerKeys->game_turbo.fDown?16:0) |
			(playerKeys->game_powerup.fDown?32:0);

		if(powerup == 1)
		{
			static const int ryu_fireball_code_left[4] = {2,6,4,20};

			if (keymaskdown == ryu_fireball_code_left[ryu_fireball_index_left])
				ryu_fireball_index_left++;
			else if(ryu_fireball_index_left > 0 && keymaskdown == ryu_fireball_code_left[ryu_fireball_index_left - 1])
			{}
			else
				ryu_fireball_index_left = 0;
		    
			if (ryu_fireball_index_left >= 4)
			{
				ryu_fireball_index_left = 0;
				shoot_left_fireball = true;
			}

			static const int ryu_fireball_code_right[4] = {2,10,8,24};

			if (keymaskdown == ryu_fireball_code_right[ryu_fireball_index_right])
				ryu_fireball_index_right++;
			else if(ryu_fireball_index_right > 0 && keymaskdown == ryu_fireball_code_right[ryu_fireball_index_right - 1])
			{}
			else
				ryu_fireball_index_right = 0;
		    
			if (ryu_fireball_index_right >= 4)
			{
				ryu_fireball_index_right = 0;
				shoot_right_fireball = true;
			}
		}
		else
		{
			shoot_left_fireball = false;
			shoot_right_fireball = false;

			ryu_fireball_index_left = 0;
			ryu_fireball_index_right = 0;
		}

		if(invincible)
		{
			if(keymask & 4)
				dashLeftIndex++;
			else if(keymask & ~4)
				dashLeftIndex = 0;
			
			if(dashLeftIndex >= 3 && keymaskdown == 20)
			{
				dashLeftIndex = 0;
				dashLeft = true;
			}

			if(keymaskdown != 20)
				dashLeft = false;


			if(keymask & 8)
				dashRightIndex++;
			else if(keymask & ~8)
				dashRightIndex = 0;

			if(dashRightIndex >= 3 && keymaskdown == 24)
			{
				dashRightIndex = 0;
				dashRight = true;
			}
			
			if(keymaskdown != 24)
				dashRight = false;
		}
		else
		{
			dashLeft = false;
			dashRight = false;
		}

		if(super_pow && (game_values.screenshaketimer > 0 || powerupused == 9))
		{
			if(game_values.screenshaketimer > 0)
			{
				if(++super_pow_timer >= 60)
				{
					game_values.screenshaketimer = 0;
					super_pow_timer = 0;
					super_pow = false;
				}

				if(keymask & 16)
				{
					game_values.screenshaketimer += 10;
					
					IO_Block * block = (IO_Block*)objectblocks.getRandomObject();

					if(block)
						block->triggerBehavior();
				}
			}
		}
		else
		{
			super_pow = false;
			super_pow_timer = 0;
		}

		if(super_mod && (game_values.screenshaketimer > 0 || powerupused == 16))
		{
			if(game_values.screenshaketimer > 0)
			{
				if(++super_mod_timer >= 60)
				{
					game_values.screenshaketimer = 0;
					super_mod_timer = 0;
					super_mod = false;
				}

				if(keymask & 16)
				{
					game_values.screenshaketimer += 10;
					
					IO_Block * block = (IO_Block*)objectblocks.getRandomObject();

					if(block)
						block->triggerBehavior();
				}
			}
		}
		else
		{
			super_mod = false;
			super_mod_timer = 0;
		}

		//Super Pow
		if (game_values.gamepowerups[globalID] == 9)
		{
			static const int super_pow_code[3] = {2, 2, 32};
	        
			if (keymask & super_pow_code[super_pow_index]) 
				super_pow_index++;
			else if (keymask & ~super_pow_code[super_pow_index]) 
				super_pow_index = 0;
	        
			if (super_pow_index >= 3)
			{
				super_pow_index = 0;
				ifsoundonplay(sfx_transform); 
				super_pow = true;
			}
		}

		//Super Mod
		if (game_values.gamepowerups[globalID] == 16)
		{
			static const int super_mod_code[3] = {1, 1, 32};
	        
			if (keymask & super_mod_code[super_mod_index]) 
				super_mod_index++;
			else if (keymask & ~super_mod_code[super_mod_index]) 
				super_mod_index = 0;
	        
			if (super_mod_index >= 3)
			{
				super_mod_index = 0;
				ifsoundonplay(sfx_transform); 
				super_mod = true;
			}
		}

		if(game_values.gamemode->gamemode == game_mode_stomp && !game_values.redkoopas)
		{
			if (redKoopaIndex < 7)
			{
				static const int red_koopa_code[7] = {2,2,4,2,8,2,1};
		        
				if (keymask & red_koopa_code[redKoopaIndex]) 
					redKoopaIndex++;
				else if (keymask & ~red_koopa_code[redKoopaIndex]) 
					redKoopaIndex = 0;
		        
				if (redKoopaIndex == 7)
				{
					ifsoundonplay(sfx_transform); 
					game_values.redkoopas = true; 
				}
			}
		}

		if(!game_values.redthrowblocks)
		{
			if (redThrowBlockIndex < 8)
			{
				static const int red_throw_block_code[8] = {1,2,1,2,8,4,1,2};
		        
				if (keymask & red_throw_block_code[redThrowBlockIndex]) 
					redThrowBlockIndex++;
				else if (keymask & ~red_throw_block_code[redThrowBlockIndex]) 
					redThrowBlockIndex = 0;
		        
				if (redThrowBlockIndex == 8)
				{
					ifsoundonplay(sfx_transform); 
					game_values.redthrowblocks = true;

					for(short iBlock = 0; iBlock < objectblocks.list_end; iBlock++)
					{
						if(((IO_Block*)objectblocks.list[iBlock])->getBlockType() == block_throw)
						{
							((B_ThrowBlock*)objectblocks.list[iBlock])->SetType(true);
						}
					}
				}
			}
		}

		if(!game_values.viewblocks)
		{
			if (viewBlockIndex < 7)
			{
				static const int view_block_code[7] = {8,2,4,2,1,1,1};
		        
				if (keymask & view_block_code[viewBlockIndex]) 
					viewBlockIndex++;
				else if (keymask & ~view_block_code[viewBlockIndex]) 
					viewBlockIndex = 0;
		        
				if (viewBlockIndex == 7)
				{
					ifsoundonplay(sfx_transform); 
					game_values.viewblocks = true;

					for(short iBlock = 0; iBlock < objectblocks.list_end; iBlock++)
					{
						IO_Block * block = (IO_Block*)objectblocks.list[iBlock];
						if(block->getBlockType() == block_powerup)
						{
							block->dead = true;
							B_ViewBlock * viewBlock = new B_ViewBlock(&spr_viewblock, block->col * TILESIZE, block->row * TILESIZE);
							
							if(block->state != 0)
							{
								viewBlock->state = 3;
								viewBlock->timer = ((B_PowerupBlock*)block)->timer;
							}

							g_map.blockdata[block->col][block->row] = viewBlock;
							objectblocks.add(viewBlock);
						}
					}
				}
			}
		}

		if (secret_spring_index < 9)
		{
			static const int secret_spring_code[9] = {2,1,2,1,2,1,2,2,16};
	        
			if (keymask & secret_spring_code[secret_spring_index]) 
				secret_spring_index++;
			else if (keymask & ~secret_spring_code[secret_spring_index]) 
				secret_spring_index = 0;
	        
			if (secret_spring_index == 9)
			{
				ifsoundonplay(sfx_transform); 
				objectsplayer.add(new CO_Spring(&spr_spring));
			}
		}

		if (secret_spike_index < 6)
		{
			static const int secret_spike_code[6] = {16,2,4,8,1,16};
	        
			if (keymask & secret_spike_code[secret_spike_index]) 
				secret_spike_index++;
			else if (keymask & ~secret_spike_code[secret_spike_index]) 
				secret_spike_index = 0;
	        
			if (secret_spike_index == 6)
			{
				ifsoundonplay(sfx_transform); 
				objectsplayer.add(new CO_Spike(&spr_spike));
			}
		}
	}

    if (tanooki && state == player_ready)
    {
        // If the down key is ever released, manually untransform by releasing the down key
        if (!playerKeys->game_down.fDown && statue_timer)
        {
            // Untransform from statue
            statue_timer = 1;
        }

        // Become the tanooki
        else if (playerKeys->game_turbo.fPressed && playerKeys->game_down.fDown && !statue_lock && !lockfire && powerupused == -1)
        {
            // set the amount of time you get to remain in statue form
            statue_timer = 123;

            // perform tansformation effects
            eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + (HALFPW) - 16, iy + (HALFPH) - 16, 3, 8));
            ifsoundonplay(sfx_transform);

            // Neutralize lateral velocity
            const float tv = 1.6f;
            if (velx > tv) velx = tv;
            else if (velx < -tv) velx = -tv;

            // Prevent you from shooting
            lockfire = true;

			// Prevent you from falling through solid-on-top blocks
			lockfall = true;

            // Prevent you from becoming the statue twice before touching the ground
            statue_lock = true;
        }

        // Count down the statue timer, which leads to a forced detransformation
        if (statue_timer == 1)
        {
            // Untransform from statue
            statue_timer = 0;

            // Release invincibility
            spawninvincible = false;

            // Slight upward velocity to escape spikes / lava
            if(!inair)
				vely = -8.0;

            // perform transformation effects
            eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + (HALFPW) - 16, iy + (HALFPH) - 16, 3, 8));
            ifsoundonplay(sfx_transform);

			statue_lock = false;
        }

        // Player is a statue
        else if (statue_timer > 0)
        {
            // Prevent player from shooting while a statue
            lockfire = true;

            // Prevent player from jumping
            lockjump = true;

            // Don't fall through passable platforms
            fallthrough = false;

            // Decrement statue timer
            statue_timer--;

            // Become invincible
            spawninvincible = true;

			statue_lock = true;
        }
    }

	if(hammertimer > 0)
		hammertimer--;

	if(throw_star > 0)
		throw_star--;

	if(pSuicideCreditPlayer)
	{
		if(--iSuicideCreditTimer <= 0)
		{
			iSuicideCreditTimer = 0;
			pSuicideCreditPlayer = NULL;
		}
	}

	iSrcOffsetX = 0;
	if(invincible)
		iSrcOffsetX = animationstate;
	else if(game_values.gamemode->tagged == this)
		iSrcOffsetX = 160;
	else if(game_values.gamemode->star == this)
		iSrcOffsetX = game_values.gamemodesettings.star.shine ? 224 : 192;
	else if(game_values.slowdownfreeze && game_values.slowdownon != teamID)
		iSrcOffsetX = 256;
	else if(spawninvincible)
		iSrcOffsetX = 128;

	if(state != player_ready)
	{
		if(state == player_wait)
		{
			//use 31 frames to do 0.5 second increments
			if(*respawncounter > 0 && ++waittimer >= 31)
			{
				waittimer = 0;
				(*respawncounter)--;
			}

			if(*respawncounter <= 0)
			{
				*respawncounter = 0;
				state = player_spawning;

				if(game_values.spawnstyle == 0)
				{
					eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
				}
				else if(game_values.spawnstyle == 1)
				{
					eyecandyback.add(new EC_Door(&spr_spawndoor[colorID], sprites[spr], ix + HALFPW - 16, iy + HALFPH - 16, 1, iSrcOffsetX));
				}
			}

			return;
		}
		
		if(state == player_spawning)
		{
			if(game_values.spawnstyle == 0)
			{
				state = player_ready;
			}
			else if(game_values.spawnstyle == 1)
			{
				//Wait for door eyecandy to open to let mario out (20 frames for door to appear and 16 frames to open)
				if(++spawntimer > 36)
				{
					spawntimer = 0;
					state = player_ready;
				}
			}
			else if(game_values.spawnstyle == 2)
			{
				spawnradius -= 2.0f;
				spawnangle += 0.05f;

				if(spawnradius < 0.0f)
					state = player_ready;
			
				if(++spawntimer > 1)
				{
					spawntimer = 0;

					short ix1 = ix - PWOFFSET + (short)(spawnradius * cos(spawnangle));
					short iy1 = iy - PHOFFSET + (short)(spawnradius * sin(spawnangle));
						
					short ix2 = ix - PWOFFSET + (short)(spawnradius * cos(spawnangle + HALF_PI));
					short iy2 = iy - PHOFFSET + (short)(spawnradius * sin(spawnangle + HALF_PI));

					short ix3 = ix - PWOFFSET + (short)(spawnradius * cos(spawnangle + PI));
					short iy3 = iy - PHOFFSET + (short)(spawnradius * sin(spawnangle + PI));

					short ix4 = ix - PWOFFSET + (short)(spawnradius * cos(spawnangle + THREE_HALF_PI));
					short iy4 = iy - PHOFFSET + (short)(spawnradius * sin(spawnangle + THREE_HALF_PI));

					eyecandyfront.add(new EC_SingleAnimation(&spr_spawnsmoke[colorID], ix1, iy1, 4, 4));
					eyecandyfront.add(new EC_SingleAnimation(&spr_spawnsmoke[colorID], ix2, iy2, 4, 4));
					eyecandyfront.add(new EC_SingleAnimation(&spr_spawnsmoke[colorID], ix3, iy3, 4, 4));
					eyecandyfront.add(new EC_SingleAnimation(&spr_spawnsmoke[colorID], ix4, iy4, 4, 4));
				}
			}
		}
		else if(iswarping())
		{
			fOldY = fy;
			fOldX = fx;
			
			if(state == player_entering_warp_left)
			{
				xi(ix - 1);
				increasewarpcounter(PW + PWOFFSET);
			}
			else if(state == player_entering_warp_right)
			{
				xi(ix + 1);
				increasewarpcounter(PW + PWOFFSET);
			}
			else if(state == player_entering_warp_up)
			{
				yi(iy - 1);
				increasewarpcounter(TILESIZE - PHOFFSET);
			}
			else if(state == player_entering_warp_down)
			{
				yi(iy + 1);
				increasewarpcounter(PH + PHOFFSET);
			}
			else if(state == player_exiting_warp_left)
			{
				xi(ix - 1);
				decreasewarpcounter();
			}
			else if(state == player_exiting_warp_right)
			{
				xi(ix + 1);
				decreasewarpcounter();
			}
			else if(state == player_exiting_warp_up)
			{
				yi(iy - 1);
				decreasewarpcounter();
			}
			else if(state == player_exiting_warp_down)
			{
				yi(iy + 1);
				decreasewarpcounter();
			}
		}
	}
	else if(powerupused > -1)
	{
		powerupradius -= (float)game_values.storedpowerupdelay / 2.0f;
		powerupangle += 0.05f;

		if(powerupradius < 0.0f)
		{
			switch(powerupused)
			{
				case 1:
				{
					game_values.gamemode->playerextraguy(*this, 1);
					ifsoundonplay(sfx_extraguysound);
					break;
				}
				case 2:
				{
					game_values.gamemode->playerextraguy(*this, 2);
					ifsoundonplay(sfx_extraguysound);
					break;
				}
				case 3:
				{
					game_values.gamemode->playerextraguy(*this, 3);
					ifsoundonplay(sfx_extraguysound);
					break;
				}
				case 4:
				{
					game_values.gamemode->playerextraguy(*this, 5);
					ifsoundonplay(sfx_extraguysound);
					break;
				}
				case 5:
				{
					powerup = 0;
					SetPowerup(1);
					break;
				}
				case 6:
				{
					makeinvincible();
					break;
				}
				case 7:
				{
					turnslowdownon(holddown > 310);
					break;
				}
				case 8:
				{
					eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
					ifsoundonplay(sfx_transform);
					bobomb = true;
					break;
				}
				case 9:
				{
					ifsoundonplay(sfx_thunder);
					game_values.screenshaketimer = 20;
					game_values.screenshakeplayerid = globalID;
					game_values.screenshaketeamid = teamID;
					game_values.screenshakekillinair = false;
					game_values.screenshakekillscount = 0;
					break;
				}
				case 10:
				{
					game_values.bulletbilltimer[globalID] = 400;
					game_values.bulletbillspawntimer[globalID] = 0;
					game_values.bulletbillhoming[globalID] = homingBills;
					homingBills = false;
					break;
				}
				case 11:
				{
					powerup = 0;
					SetPowerup(2);
					break;
				}
				case 12:
				{
					CO_Shell * shell = new CO_Shell(0, 0, 0, true, true, true, false); 
					objectsplayer.add(shell);
					shell->UsedAsStoredPowerup(this);
					break;
				}
				case 13:
				{
					CO_Shell * shell = new CO_Shell(1, 0, 0, false, true, true, false);
					objectsplayer.add(shell);
					shell->UsedAsStoredPowerup(this);
					break;
				}
				case 14:
				{
					CO_Shell * shell = new CO_Shell(2, 0, 0, false, false, true, true); 
					objectsplayer.add(shell);
					shell->UsedAsStoredPowerup(this);
					break;
				}
				case 15:
				{
					CO_Shell * shell = new CO_Shell(3, 0, 0, false, true, false, false); 
					objectsplayer.add(shell);
					shell->UsedAsStoredPowerup(this);
					break;
				}
				case 16:
				{
					ifsoundonplay(sfx_thunder);
					game_values.screenshaketimer = 20;
					game_values.screenshakeplayerid = globalID;
					game_values.screenshaketeamid = teamID;
					game_values.screenshakekillinair = true;
					break;
				}
				case 17:
				{
					eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
					ifsoundonplay(sfx_collectfeather);
					powerup = 3;
					iCapeTimer = 4;
					break;
				}
				case 18:
				{
					SwapPlayers(localID);
					break;
				}
				case 19:
				{
					powerup = 0;
					SetPowerup(4);
					break;
				}
				case 21:  //sledge hammer
				{
					powerup = 0;
					SetPowerup(5);
					break;
				}
				case 22:  //golden podobo
				{
					short numPodobos = rand() % 6 + 10;
					for(short iPodobo = 0; iPodobo < numPodobos; iPodobo++)
					{
						objectsfront.add(new OMO_Podobo(&spr_podobo, (short)(rand() % 608), -(float(rand() % 9) / 2.0f) - 9.0f, globalID, teamID, colorID));
					}
					ifsoundonplay(sfx_thunder);
					break;
				}
				case 23:  //bombs
				{
					powerup = 0;
					SetPowerup(6);
					break;
				}
			}
				
			powerupused = -1;
		}
	}

	//Animate invincibility
	if(invincible)
	{
		animationtimer++;

		if((animationtimer > 3 && invincibletimer < 480) || animationtimer > 6)
		{
			animationtimer = 0;

			animationstate += 32;
			if(animationstate > 96)
				animationstate = 0;
		}

		if(++invincibletimer > 580)
		{
			animationstate = 0;
			animationtimer = 0;
			invincibletimer = 0;
			invincible = false;
		}
	}

	//if player is warping or spawning don't pay attention to controls
	if(state == player_ready)
	{
		//If player is shielded, count down that timer
		if(spawninvincibletimer > 0)
			spawninvincible = --spawninvincibletimer > 0;

		short lrn = 0;	//move left-right-no: -1.. left 0 no 1 ... right

		if(pPlayerAI)
		{
			//Calculate movement every 4th frame (speed up optimization)
			if(game_values.cputurn == globalID)
				cpu_think();
		}

        if (!statue_timer && (!game_values.slowdownfreeze || game_values.slowdownon == teamID))
        {
            if(playerKeys->game_right.fDown)
                lrn++;
            
            if(playerKeys->game_left.fDown)
                lrn--;
        }

		//Used for bouncing off of note blocks
		if(superjumptimer > 0)
			superjumptimer--;

		//jump pressed?
		if(!game_values.slowdownfreeze || game_values.slowdownon == teamID)
		{
			if(playerKeys->game_jump.fDown)
			{
				if(!lockjump)
				{
					if(powerup == 3 && holddown >= 120 && !flying)
					{
						holddown = 0;
						ifsoundonplay(sfx_collectfeather);
						flying = true;
						lockjump = true;

						if(game_values.featherlimit > 0)
						{
							DecreaseProjectileLimit();
						}
					}
					else if(!inair && superjumptimer == 0)
					{	//only if on the ground and the jump key was released somewhen after it was pressed the last time

						bool fFellThrough = false;
						if(playerKeys->game_down.fDown)
						{
							//Check to see what the player is standing on

							short txl = ix / TILESIZE;
							short txr;
							
							fPrecalculatedY = fy + vely;

							short ty = ((short)fPrecalculatedY + PH) / TILESIZE;

							if(txl < 0)
								txl += 20;
							else if(txl > 19)
								txl -= 20;

							if(ix + PW >= 640)
								txr = (ix + PW - 640) / TILESIZE;
							else
								txr = (ix + PW) / TILESIZE;

							TileType lefttile = g_map.map(txl, ty);
							TileType righttile = g_map.map(txr, ty);

							if((lefttile == tile_solid_on_top && (righttile == tile_solid_on_top || righttile == tile_nonsolid)) ||
								(righttile == tile_solid_on_top && (lefttile == tile_solid_on_top || lefttile == tile_nonsolid)))
							{
								fFellThrough = true;
							}

							if(!fFellThrough && platform)
							{
								fPrecalculatedY += platform->fOldVelY;
								platform->gettiletypes(this, &lefttile, &righttile);

								if((lefttile == tile_solid_on_top && (righttile == tile_solid_on_top || righttile == tile_nonsolid)) ||
								(righttile == tile_solid_on_top && (lefttile == tile_solid_on_top || lefttile == tile_nonsolid)))
								{
									fFellThrough = true;
								}
							}
						}

						if(fFellThrough)
						{
							lockfall = true;
							fallthrough = true;
						}
						else
						{
							Jump(lrn, 1.0f);
							ifsoundonplay(sfx_jump);
						}

						lockjump = true;
					}
					else if(superjumptimer > 0)
					{
						if(holddown > 60)
						{
							holddown = 0;
							vely = -VELSUPERJUMP;
							ifsoundonplay(sfx_superspring);
						}
						else
						{
							vely = -VELTURBOJUMP;
							ifsoundonplay(sfx_springjump);
						}

						superjumptimer = 0;
						lockjump = true;
					}
					else if(powerup == 3)
					{
						if(featherjump < game_values.featherjumps)
						{
							if(game_values.featherlimit == 0 || projectilelimit > 0)
							{
								if(featherjump < game_values.featherjumps)
								{
									Jump(lrn, 0.8f);
									ifsoundonplay(sfx_capejump);
									lockjump = true;
								}
								
								featherjump++;	
							}

							if(game_values.featherlimit > 0)
							{
								DecreaseProjectileLimit();
							}
						}
					}
				}
			}
			else
			{
				lockjump = false;		//the jump key is not pressed: the player may jump again if he is on the ground

				if(vely < -VELSTOPJUMP)
					vely = -VELSTOPJUMP;

				flying = false;
				flyingtimer = 0;
			}
		}

		if(flying)
		{
			if(++flyingtimer > 300)
			{
				flyingtimer = 0;
				flying = false;
			}

			if(playerKeys->game_down.fDown && vely < 1.0f)
			{
				vely += 1.0f;
			}
			else if(!playerKeys->game_down.fDown && vely > -1.0f)
			{
				vely -= 1.0f;
				inair = true;
			}
		}


		if(!game_values.slowdownfreeze || game_values.slowdownon == teamID)
		{
			if(playerKeys->game_down.fDown)
			{
				if(!lockfall && !inair && playerDevice == DEVICE_KEYBOARD)
				{
					lockfall = true;
					fallthrough = true;
				}
			}
			else
			{
				lockfall = false;
			}
		}
		
		//POWERUP RELEASE
		if(playerKeys->game_powerup.fDown && (!game_values.slowdownfreeze || game_values.slowdownon == teamID) && statue_timer == 0)
		{
			if(game_values.gamepowerups[globalID] > -1)
			{
				powerupused = game_values.gamepowerups[globalID];
				game_values.gamepowerups[globalID] = -1;

				powerupradius = 100.0f;
				powerupangle = (float)(rand()%1000 * 0.00628f);

				ifsoundonplay(sfx_storedpowerupsound);
			}
		}
		
		fPressedAcceptItem = playerKeys->game_turbo.fPressed;

		//Projectiles
		if(!game_values.slowdownfreeze || game_values.slowdownon == teamID)
		{
			if(playerKeys->game_turbo.fDown)
			{
				fAcceptingItem = carriedItem == NULL;

				if(!lockfire)
				{
					if(bobomb) //If we're a bob-omb, explode
					{
						bobomb = false;

						if(shoot_super_bobomb)
						{
							shoot_super_bobomb = false;

							CO_Bomb * bomb = new CO_Bomb(&spr_bomb, ix + HALFPW - 14, iy + 2, 0.0f, 0.0f, 4, globalID, teamID, colorID, rand() % 120 + 240);
							
							if(AcceptItem(bomb))
							{
								bomb->owner = this;
								bomb->MoveToOwner();
							}

							objectsfront.add(bomb);
							projectiles[globalID]++;
							
							eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
							ifsoundonplay(sfx_transform);
						}
						else
						{
							objectsfront.add(new OMO_Explosion(&spr_explosion, ix + HALFPW - 96, iy + HALFPH - 64, 2, 4, globalID, teamID));
							ifsoundonplay(sfx_bobombsound);
						}
					}
					else
					{
						if(powerup == 1 && projectiles[globalID] < 2)
						{
							if(game_values.fireballlimit == 0 || projectilelimit > 0)
							{
								if(shoot_left_fireball)
								{
									objectsfront.add(new MO_SuperFireball(&spr_superfireball, ix + HALFPW - 30, iy + HALFPH - 16, 4, -6.0f, 0.0f, 4, globalID, teamID, colorID));
									ifsoundonplay(sfx_spit); 
								}
								else if(shoot_right_fireball)
								{
									objectsfront.add(new MO_SuperFireball(&spr_superfireball, ix + HALFPW - 2, iy + HALFPH - 16, 4, 6.0f, 0.0f, 4, globalID, teamID, colorID));
									ifsoundonplay(sfx_spit); 
								}
								else
								{
									objectcollisionitems.add(new MO_Fireball(&spr_fireball, ix + 6, iy, 4, IsPlayerFacingRight(), 5, globalID, teamID, colorID));
									ifsoundonplay(sfx_fireball);
								}

								projectiles[globalID]++;
							}

							if(game_values.fireballlimit > 0)
							{
								DecreaseProjectileLimit();
							}
						}
						else if(powerup == 2 && projectiles[globalID] < 2 && hammertimer == 0)
						{
							if(game_values.hammerlimit == 0 || projectilelimit > 0)
							{
								if(shoot_left_super_hammer || shoot_right_super_hammer)
								{
									shoot_left_super_hammer = false;
									shoot_right_super_hammer = false;
									objectsfront.add(new MO_Hammer(&spr_hammer, ix + 6, iy, 6, (IsPlayerFacingRight() ? 5.0f : -5.0f), -1.0f, 5, globalID, teamID, colorID, true));
									objectsfront.add(new MO_Hammer(&spr_hammer, ix + 6, iy, 6, (IsPlayerFacingRight() ? 5.0f : -5.0f), 0.0f, 5, globalID, teamID, colorID, true));
									objectsfront.add(new MO_Hammer(&spr_hammer, ix + 6, iy, 6, (IsPlayerFacingRight() ? 5.0f : -5.0f), 1.0f, 5, globalID, teamID, colorID, true));
									projectiles[globalID] += 3;
								}
								else
								{
									if(IsPlayerFacingRight())
										objectsfront.add(new MO_Hammer(&spr_hammer, ix + 8, iy, 6, velx + 2.0f, -HAMMERTHROW, 5, globalID, teamID, colorID, false));
									else
										objectsfront.add(new MO_Hammer(&spr_hammer, ix - 14, iy, 6, velx - 2.0f, -HAMMERTHROW, 5, globalID, teamID, colorID, false));

									projectiles[globalID]++;
								}
								
								hammertimer = game_values.hammerdelay;
								ifsoundonplay(sfx_fireball);
							}

							if(game_values.hammerlimit > 0)
							{
								DecreaseProjectileLimit();
							}
						}
						else if(powerup == 4 && projectiles[globalID] < 1) //only allow one boomerang
						{
							if(game_values.boomeranglimit == 0 || projectilelimit > 0)
							{
								objectsfront.add(new MO_Boomerang(&spr_boomerang, ix, iy + HALFPH - 16, 4, IsPlayerFacingRight(), 5, globalID, teamID, colorID, shoot_super_boomerang));
								projectiles[globalID]++;

								if(shoot_super_boomerang)
									game_values.superboomerang[globalID] = 1;

								shoot_super_boomerang = false;
							}

							if(game_values.boomeranglimit > 0)
							{
								DecreaseProjectileLimit();
							}
						}
						else if(powerup == 5 && projectiles[globalID] < 1 && hammertimer == 0)
						{
							if(game_values.hammerlimit == 0 || projectilelimit > 0)
							{
								if(IsPlayerFacingRight())
									objectsfront.add(new MO_SledgeHammer(&spr_sledgehammer, ix + 8, iy, 8, velx + 2.0f, -HAMMERTHROW, 5, globalID, teamID, colorID, shoot_super_sledge_hammer));
								else
								objectsfront.add(new MO_SledgeHammer(&spr_sledgehammer, ix - 18, iy, 8, velx - 2.0f, -HAMMERTHROW, 5, globalID, teamID, colorID, shoot_super_sledge_hammer));

								shoot_super_sledge_hammer = false;

								hammertimer = game_values.hammerdelay;
								projectiles[globalID]++;
								
								ifsoundonplay(sfx_fireball);
							}

							if(game_values.hammerlimit > 0)
							{
								DecreaseProjectileLimit();
							}
						}
						else if(powerup == 6 && projectiles[globalID] < 2)
						{
							CO_Bomb * bomb = new CO_Bomb(&spr_bomb, ix + HALFPW - 14, iy + 2, 0.0f, 0.0f, 4, globalID, teamID, colorID, rand() % 120 + 240);
							
							if(AcceptItem(bomb))
							{
								bomb->owner = this;
								bomb->MoveToOwner();
							}

							objectsfront.add(bomb);
							projectiles[globalID]++;
							
							ifsoundonplay(sfx_fireball);
						}
					}

					lockfire = true;
				}
			}
			else
			{
				lockfire = false;
				fAcceptingItem = false;

				if(carriedItem)
				{
					if(playerKeys->game_down.fDown)
						carriedItem->Drop();
					else
					{
						//Make sure the shell/block is out in front of player before kicking it
						if(carriedItem->getMovingObjectType() == movingobject_shell || carriedItem->getMovingObjectType() == movingobject_throwblock)
							carriedItem->MoveToOwner();

						carriedItem->Kick(superKickIndex > 1);
					}

					carriedItem = NULL;
				}
			}
		}

		shoot_left_fireball = false;
		shoot_right_fireball = false;

		if(lrn == 1)
		{
			if(onice)
				velx += VELMOVINGADDICE;
			else if(dashRight)
				velx += VELDASHADD;
			else
				velx += VELMOVINGADD;		//move right

			float maxVel = 0;
			if((game_values.slowdownon != -1 && game_values.slowdownon != teamID) || jailed > 0)
				maxVel = game_values.slowdownfreeze ? 0.0f : VELSLOWMOVING;
			else if(dashRight)
				maxVel = VELDASHMOVING;
			else if(playerKeys->game_turbo.fDown)
				maxVel = VELTURBOMOVING + (game_values.gamemode->tagged == this ? TAGGEDBOOST : 0.0f);
			else
				maxVel = VELMOVING + (game_values.gamemode->tagged == this ? TAGGEDBOOST : 0.0f);
			
			if(velx > maxVel)
				velx = maxVel;

			if(velx < 0.0f && !inair)
				game_values.playskidsound = true;
		}
		else if(lrn == -1)
		{
			if(onice)
				velx -= VELMOVINGADDICE;
			else if(dashLeft)
				velx -= VELDASHADD;
			else
				velx -= VELMOVINGADD;		//move left

			float maxVel = 0;
			if((game_values.slowdownon != -1 && game_values.slowdownon != teamID) || jailed > 0)
				maxVel = game_values.slowdownfreeze ? 0.0f : -VELSLOWMOVING;
			else if(dashLeft)
				maxVel = -VELDASHMOVING;
			else if(playerKeys->game_turbo.fDown)
				maxVel = -VELTURBOMOVING - (game_values.gamemode->tagged == this ? TAGGEDBOOST : 0.0f);
			else
				maxVel = -VELMOVING - (game_values.gamemode->tagged == this ? TAGGEDBOOST : 0.0f);
			
			if(velx < maxVel)
				velx = maxVel;

			if(velx > 0.0f && !inair)
				game_values.playskidsound = true;
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
		}
		
		fOldX = fx;
		fOldY = fy;
			
		collision_detection_map();
	}

	//Player can be killed by map so only do this code if he is still living
	if(state == player_ready)
	{
		//Deal with terminal burnup velocity
		if(vely >= MAXVELY && !invincible)
		{
			if(++burnupstarttimer >= 20)
			{
				if(burnupstarttimer == 20)
					ifsoundonplay(sfx_burnup);

				if(++burnuptimer > 80)
				{
					KillPlayerMapHazard();
					return;
				}
				else
				{
					eyecandyback.add(new EC_SingleAnimation(&spr_burnup, ix + HALFPW - 16, iy + HALFPH - 16, 5, 4));
				}
			}
		}
		else
		{
			burnuptimer = 0;
			burnupstarttimer = 0;
		}

		//Deal with out of arena timer
		if(iy < 0)
		{
			if((iy + PH < -1 || (iy + PH <= 1 && vely <= 0.8f)) && game_values.outofboundstime > 0 && !invincible)
			{
				if(++outofarenatimer > 62)
				{
					outofarenatimer = 0;

					if(--outofarenadisplaytimer < 0)
					{
						KillPlayerMapHazard();
						return;
					}
				}
			}
		}

		//Deal with release from jail timer
		if(jailed > 0 && game_values.gamemodesettings.jail.timetofree > 1)
		{
			if(--jailed <= 0)
			{
				jailed = 0;
				eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
				ifsoundonplay(sfx_transform);
			}
		}
	}

	SetSprite();
}

void CPlayer::SetSprite()
{
	//Use correct sprite (and animate)
	if(!game_values.pausegame && !game_values.exitinggame && !game_values.swapplayers)
	{
		//if player is warping from below, set them in the air
		if(state > player_ready)
		{
			if(state == player_exiting_warp_down || state == player_entering_warp_up)
				inair = true;
			else
				inair = false;
		}

		//lockjump is true when we are in the air (even if we fell of an edge)
		if(state == player_spawning)
		{
			if((spr & 0x1) == 0)
				spr = PGFX_JUMPING_R;
			else
				spr = PGFX_JUMPING_L;
		}
		else if(inair)
		{
			frictionslidetimer = 0;

			if(IsPlayerFacingRight())
				spr = PGFX_JUMPING_R;
			else
				spr = PGFX_JUMPING_L;
		}
		else
		{
			if(velx > 0.0f)
			{
				if(playerKeys->game_left.fDown && !playerKeys->game_right.fDown && state == player_ready)
				{
					spr = PGFX_STOPPING_R;

					if(++frictionslidetimer > 3)
					{
						frictionslidetimer = 0;
						eyecandyfront.add(new EC_SingleAnimation(&spr_frictionsmoke, ix, iy + PH - 12, 4, 4));
					}
				}
				else
				{
					if(onice && !playerKeys->game_right.fDown && !playerKeys->game_left.fDown)
						spr = PGFX_STANDING_R;
					else
					{
						if(--sprswitch < 1)
						{
							if(spr == PGFX_STANDING_R)
								spr = PGFX_RUNNING_R;
							else
								spr = PGFX_STANDING_R;

							sprswitch = 4;
						}
						else
						{
							//If animation timer hasn't fired, make sure we're facing the correct direction
							if((spr & 0x1) == 1)
								spr = PGFX_STANDING_R;
						}
					}
				}
			}
			else if(velx < 0.0f)
			{
				if(playerKeys->game_right.fDown && !playerKeys->game_left.fDown && state == player_ready)
				{
					spr = PGFX_STOPPING_L;

					if(++frictionslidetimer > 3)
					{
						frictionslidetimer = 0;
						eyecandyfront.add(new EC_SingleAnimation(&spr_frictionsmoke, ix + PW - 16, iy + PH - 12, 4, 4));
					}
				}
				else
				{
					if(onice && !playerKeys->game_right.fDown && !playerKeys->game_left.fDown)
						spr = PGFX_STANDING_L;
					else
					{
						if(--sprswitch < 1)
						{
							if(spr == PGFX_STANDING_L)
								spr = PGFX_RUNNING_L;
							else
								spr = PGFX_STANDING_L;

							sprswitch = 4;
						}
						else
						{
							//If animation timer hasn't fired, make sure we're facing the correct direction
							if((spr & 0x1) == 0)
								spr = PGFX_STANDING_L;
						}
					}
					
				}
			}
			else
			{	
				//standing
				if(playerKeys->game_left.fDown)
					spr = PGFX_STANDING_L;
				else if(playerKeys->game_right.fDown)
					spr = PGFX_STANDING_R;
				else
				{
					if((spr & 0x1) == 0)
						spr = PGFX_STANDING_R;
					else
						spr = PGFX_STANDING_L;
				}
			}
		}
	}
	else if(game_values.swapplayers)
	{
		short iSpriteDirection = 0;

		if(fNewSwapX < fOldSwapX)
			iSpriteDirection = 1;

		if(--sprswitch < 1)
		{
			if(spr == PGFX_STANDING_R + iSpriteDirection)
				spr = PGFX_RUNNING_R + iSpriteDirection;
			else
				spr = PGFX_STANDING_R + iSpriteDirection;

			sprswitch = 4;
		}
	}
}

void CPlayer::Jump(short iMove, float jumpModifier)
{
	if((game_values.slowdownon != -1 && game_values.slowdownon != teamID) || jailed > 0)
		vely = -VELSLOWJUMP * jumpModifier;
	else if(ABS(velx) > VELMOVING && iMove != 0 && playerKeys->game_turbo.fDown)
		vely = -VELTURBOJUMP * jumpModifier;
	else
		vely = -VELJUMP * jumpModifier;

	inair = true;
	
	//Need to help the player off the platform otherwise it will collide with them again
	if(platform)
	{
		//if(platform->fVelY < 0.0f)
		//	yf(fy + platform->fVelY);

		platform = NULL;
	}

	//printf("Player Jumped!\n");
}

void CPlayer::increasewarpcounter(short iGoal)
{
	if(++warpcounter > iGoal)
	{
		warpcounter = iGoal;
		chooseWarpExit();
	}
}

void CPlayer::decreasewarpcounter()
{
	if(--warpcounter < 0)
	{
		warpcounter = 0;
		state = player_ready;
	}
}

void CPlayer::chooseWarpExit()
{
	WarpExit * exit = g_map.getRandomWarpExit(warpconnection, warpid);
	xi(exit->x);
	yi(exit->y);
	fOldX = fx;
	fOldY = fy;

	lockjump = false;
	if(exit->direction == 0)
	{
		state = player_exiting_warp_up;
		warpcounter = PH + PHOFFSET;
		velx = 0.0f;
		vely = -4.0f;
		warpplane = exit->warpy * TILESIZE;
	}
	else if(exit->direction == 1)
	{
		state = player_exiting_warp_right;
		warpcounter = PW + PWOFFSET;
		velx = 1.0f;
		vely = 1.0f;
		warpplane = exit->warpx * TILESIZE + TILESIZE;
	}
	else if(exit->direction == 2)
	{
		state = player_exiting_warp_down;
		warpcounter = TILESIZE - PHOFFSET;
		velx = 0.0f;
		vely = 1.1f;
		inair = true;
		warpplane = exit->warpy * TILESIZE + TILESIZE;
	}
	else if(exit->direction == 3)
	{
		state = player_exiting_warp_left;
		warpcounter = PW + PWOFFSET;
		velx = -1.0f;
		vely = 1.0f;
		warpplane = exit->warpx * TILESIZE;
	}

	if(game_values.spawninvincibility > 0)
	{
		if(!spawninvincible || spawninvincibletimer < game_values.spawninvincibility)
		{
			spawninvincibletimer = game_values.spawninvincibility;
			spawninvincible = true;
		}
	}
}


void CPlayer::cpu_think()
{
	pPlayerAI->Think(playerKeys);
}


void CPlayer::die(short deathStyle, bool fTeamRemoved)
{
	short iDeathSprite = deathStyle == 0 ? PGFX_DEADFLYING : PGFX_DEAD;

	gfxSprite * corpseSprite = sprites[iDeathSprite];

	if(diedas == 1 || game_values.gamemode->chicken == this)
		corpseSprite = spr_chocobo[colorID][iDeathSprite];
	else if(diedas == 2 || bobomb)
		corpseSprite = spr_bobomb[colorID][iDeathSprite];
	
	if(deathStyle == 0)
		eyecandyfront.add(new EC_FallingObject(corpseSprite, ix + HALFPW - 16, iy + PH - 32, -VELTURBOJUMP, iSrcOffsetX, 0, 32, 32));
	else
		eyecandyback.add(new EC_Corpse(corpseSprite, (float)(ix - PWOFFSET), (float)(iy+PH-32), iSrcOffsetX));

	if(carriedItem)
	{
		carriedItem->Drop();
		carriedItem = NULL;
	}

	if(!fTeamRemoved)
	{
		if(game_values.screencrunch)
			y_shake += CRUNCHAMOUNT;

		if(y_shake > CRUNCHMAX)
			y_shake = CRUNCHMAX;
	
		SetupNewPlayer();
	}
}

void CPlayer::SetupNewPlayer()
{
	pScoreboardSprite = sprites;

	FindSpawnPoint();
	collision_detection_checksides();
	
	velx = 0.0f;
	
	if(game_values.spawnstyle == 1)
		vely = 0.0f;
	else
		vely = -(VELJUMP / 2);	//make the player jump up on respawn

	fOldX = fx;
	fOldY = fy;

	inair = true;
	onice = false;
	lockjump = true;
	superjumptimer = 0;
	powerup	= 0;
	projectilelimit = 0;
	bobomb = false;
	hammertimer = 0;

	tanooki = false;
    statue_lock = false;
    statue_timer = 0;

	//Only reset the tanooki index if it hasn't been triggered yet
	if(konamiIndex != 11)
		konamiIndex = 0;

	//Only reset the secret spring index if it hasn't been triggered yet
	if(secret_spring_index != 9)
		secret_spring_index = 0;

	if(secret_spike_index != 6)
		secret_spike_index = 0;

	superKickIndex = 0;

	invincible = false;
	invincibletimer = 0;

	animationstate = 0;
	animationtimer = 0;

	killsinrow = 0;
	killsinrowinair = 0;
	awardangle = 0.0f;
	featherjump = 0;
	holddown = 0;
	holddowntolerance = 0;
	holdleft = 0;
	holdlefttolerance = 0;
	holdright = 0;
	holdrighttolerance = 0;
	flying = false;
	flyingtimer = 0;

	ryu_fireball_index_left = 0;
	ryu_fireball_index_right = 0;
	shoot_left_fireball = false;
	shoot_right_fireball = false;

	super_hammer_throw_left_index = 0;
	super_hammer_throw_right_index = 0;
	shoot_left_super_hammer = false;
	shoot_right_super_hammer = false;

	super_bobomb_index = 0;
	shoot_super_bobomb = false;

	super_sledge_hammer_throw_index = 0;
	shoot_super_sledge_hammer = false;

	super_boomerang_throw_index_left = 0;
	super_boomerang_throw_index_right = 0;
	shoot_super_boomerang = false;

	super_pow_index = 0;
	super_pow = false;
	super_pow_timer = 0;

	super_mod_index = 0;
	super_mod = false;
	super_mod_timer = 0;

	dashLeftIndex = 0;
	dashRightIndex = 0;
	dashLeft = false;
	dashRight = false;
	dashSparkleAnimationTimer = 0;

	homingBillsIndex = 0;
	homingBills = false;

	redKoopaIndex = 0;
	redThrowBlockIndex = 0;
	viewBlockIndex = 0;
	
	for(short bossType = 0; bossType < 3; bossType++)
		boss_index[bossType] = 0;

	iCapeTimer = 0;
	iCapeFrameX = 0;
	iCapeFrameY = 0;
	fCapeUp = false;
	iCapeYOffset = 0;

	frictionslidetimer = 0;
	bobombsmoketimer = 0;

	spawnangle = (float)(rand()%1000 * 0.00628f);  //random new spawn angle
	spawnradius = 100.0f;
	spawntimer = 0;
	waittimer = 0;
	*respawncounter = game_values.respawn;
	state = player_wait;

	fallthrough = false;
	diedas = 0;
	iSrcOffsetX = 0;

	burnuptimer = 0;
	burnupstarttimer = 0;

	outofarenatimer = 0;
	outofarenadisplaytimer = game_values.outofboundstime - 1;

	warpcounter = 0;

	powerupused = -1;

	platform = NULL;
	iHorizontalPlatformCollision = -1;
	iVerticalPlatformCollision = -1;

	
	if(game_values.gamemode->getgamemode() == game_mode_survival)
	{
		if(game_values.gamemodesettings.survival.shield)
		{
			if(game_values.spawninvincibility > 0)
				spawninvincibletimer = game_values.spawninvincibility;
			else
				spawninvincibletimer = 62;

			spawninvincible = true;
		}
	}
	else if(game_values.spawninvincibility > 0)
	{
		spawninvincibletimer = game_values.spawninvincibility;
		spawninvincible = true;
	}

	throw_star = 0;
}

void CPlayer::FindSpawnPoint()
{
	g_map.findspawnpoint(0, &ix, &iy, PW, PH, false);
	fx = (float)ix;
	fy = (float)iy;
}


void CPlayer::spawnText(char * szText)
{
	if(++spawntext >= 20)
	{
		eyecandyfront.add(new EC_GravText(&game_font_large, ix + HALFPW, iy, szText, -VELJUMP));
		spawntext = 0;	//spawn text every 20 frames
	}
}

bool CPlayer::bouncejump()
{
	if(playerKeys->game_jump.fDown)
	{
		lockjump = true;
		vely = -VELJUMP;
		return true;
	}
	else
	{
		vely = -VELJUMP / 2;	//jump a little off of collision object
		return false;
	}
}



bool CPlayer::isstomping(CPlayer &o)
{
	//printf("ID: %d  Stomp old: %d <= %d  new: %d >= %d  vely: %.2f\n", globalID, oldy + PH, o.oldy, iy + PH, o.iy, vely);

	if(fOldY + PH <= o.fOldY && iy + PH >= o.iy)
	{
		//don't reposition if player is warping when he kills the other player
		if(state == player_ready)
		{
			yi(o.iy - PH);		//set new position to top of other player
			collision_detection_checktop();
			platform = NULL;
		}
		
		bool fKillPotential = false;
		if(vely > 1.0f)
			fKillPotential = true;

		bouncejump();

		if(fKillPotential)
		{
			PlayerKilledPlayer(*this, o, death_style_squish, (featherjump > 0 ? kill_style_feather : kill_style_stomp));
		}
		else
		{
			if(game_values.gamemode->tagged == &o && isready() && o.isready())
			{
				game_values.gamemode->tagged = this;
				o.spawninvincible = true;
				o.spawninvincibletimer = 60;
				eyecandyfront.add(new EC_GravText(&game_font_large, game_values.gamemode->tagged->ix + HALFPW, game_values.gamemode->tagged->iy + PH, "Tagged!", -VELJUMP*1.5));
				eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, game_values.gamemode->tagged->ix + HALFPW - 16, game_values.gamemode->tagged->iy + HALFPH - 16, 3, 8));
				ifsoundonplay(sfx_transform);
			}
			else if(game_values.gamemode->tagged == this && isready() && o.isready())
			{
				game_values.gamemode->tagged = &o;
				spawninvincibletimer = 60;
				spawninvincible = true;
				eyecandyfront.add(new EC_GravText(&game_font_large, game_values.gamemode->tagged->ix + HALFPW, game_values.gamemode->tagged->iy + PH, "Tagged!", -VELJUMP*1.5));
				eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, game_values.gamemode->tagged->ix + HALFPW - 16, game_values.gamemode->tagged->iy + HALFPH - 16, 3, 8));
				ifsoundonplay(sfx_transform);
			}
		}

		return true;
	}

	return false;
}

void CPlayer::explodeawards()
{
	if(killsinrow < MINAWARDSNEEDED)
		return;
		
	ifsoundonplay(sfx_cannon);

	short numawards = (killsinrow > MAXAWARDS ? MAXAWARDS : killsinrow);
	float addangle = TWO_PI / (float)numawards;

	for(short k = 0; k < numawards; k++)
	{
		float angle = (float)k * addangle + awardangle;
		float cosangle = cos(angle);
		float sinangle = sin(angle);

		short awardx = ix + HALFPW - 8 + (short)(30.0f * cosangle);
		short awardy = iy + HALFPH - 8 + (short)(30.0f * sinangle);
		
		float awardvelx = 7.0f * cosangle;
		float awardvely = 7.0f * sinangle;
			
		eyecandyfront.add(new EC_ExplodingAward(&spr_awardsolid, awardx, awardy, awardvelx, awardvely, 30, awards[k]));
	}
}

void CPlayer::addswirlingawards()
{
	if(killsinrow < MINAWARDSNEEDED)
		return;
		
	ifsoundonplay(sfx_cannon);

	short numawards = (killsinrow > MAXAWARDS ? MAXAWARDS : killsinrow);
	float addangle = TWO_PI / (float)numawards;

	for(short k = 0; k < numawards; k++)
	{
		float angle = (float)k * addangle + awardangle;
		eyecandyfront.add(new EC_SwirlingAward(&spr_awardkillsinrow[colorID], ix + HALFPW - 8, iy + HALFPH - 8, angle, 30.0f, 0.05f, 60, numawards - 1));
	}
}

void CPlayer::addrocketawards()
{
	if(killsinrow < MINAWARDSNEEDED)
		return;
		
	ifsoundonplay(sfx_cannon);

	short numawards = (killsinrow > MAXAWARDS ? MAXAWARDS : killsinrow);
	
	float addangle = QUARTER_PI / 10.0f;
	float startangle = -HALF_PI - ((addangle / 2) * (float)(numawards - 1));
	
	for(short k = 0; k < numawards; k++)
	{
		float angle = (float)k * addangle + startangle;
		float awardvelx = 9.0f * cos(angle);
		float awardvely = 9.0f * sin(angle);
			
		eyecandyfront.add(new EC_RocketAward(&spr_awardkillsinrow[colorID], ix + HALFPW - 8, iy + HALFPH - 8, awardvelx, awardvely, 80, numawards - 1, 10));
	}
}

void CPlayer::DeathAwards()
{
	if(game_values.awardstyle == award_style_halo)
		explodeawards();
	else if(game_values.awardstyle == award_style_souls && killsinrow >= MINAWARDSNEEDED)
		eyecandyfront.add(new EC_SoulsAward(&spr_awardsouls, &spr_awardsoulspawn, ix + HALFPW, iy + HALFPH, 60, 9.0f, killsinrow, awards));
}

void CPlayer::AddKillsInRowInAirAward()
{
	float angle = 0.0f; 
	for(short k = 0; k < 15; k++)
	{
		float vel = 7.0f + ((k % 2) * 5.0f);
		float awardvelx = vel * cos(angle);
		float awardvely = vel * sin(angle);
		
		eyecandyfront.add(new EC_FallingObject(&spr_bonus, ix + HALFPW - 8, iy + HALFPH - 8, awardvelx, awardvely, 4, 2, 0, colorID * 16, 16, 16));
		angle -= (float)PI / 14;
	}
}

void PlayerKilledPlayer(CPlayer &killer, CPlayer &killed, short deathstyle, killstyle style)
{
	//If this player is already dead, then don't kill him again
	if(killed.state != player_ready)
		return;

	if(killed.bobomb)
	{
		killed.diedas = 2;
		killer.bobomb = true;
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, killer.ix + HALFPW - 16, killer.iy + HALFPH - 16, 3, 8));
		ifsoundonplay(sfx_transform);
	}

	if(game_values.gamemode->chicken == &killer && style != kill_style_pow)
	{
		ifsoundonplay(sfx_chicken);
	}
	else
	{
		if(deathstyle == 0)
			ifsoundonplay(sfx_deathsound);
		else
			ifsoundonplay(sfx_mip);
	}

	if(killer.teamID != killed.teamID)
		AddAwardKill(&killer, &killed, style);

	if(game_values.awardstyle != award_style_none)
		killed.DeathAwards();
	
	/*
	if ( ! game_values.gamemode->playerkilledplayer(killer, killed) )
	{	//true if the player was deleted
		
		//now kill the player (don't call this function earlier because we need the old position, etc.	
		if(deathstyle == 0)
			killed.jumpDeath();
		else
			killed.die();
	}*/

	//now kill the player (don't call this function earlier because we need the old position, etc.	
	if(!game_values.gamemode->playerkilledplayer(killer, killed))
		killed.die(deathstyle, false);
}

void AddAwardKill(CPlayer * killer, CPlayer * killed, killstyle style)
{
	killer->killsinrow++;

	if(killer->killsinrow >= 10 && game_values.secrets)
	{
		if(rand() % 5 == 0)
		{
			if(!game_values.gamemode->gameover && game_values.bosspeeking == -1)
			{
				eyecandyfront.add(new EC_BossPeeker(&spr_sledgebrothers, rand()%90 + 90, 2));
				
				backgroundmusic[0].stop();
				ifsoundonstop(sfx_invinciblemusic);
				ifsoundonstop(sfx_timewarning);
				ifsoundonplay(sfx_bowserlaugh);
			}
		}
	}

	if(killer->inair && (style == kill_style_stomp || style == kill_style_goomba || style == kill_style_koopa || style == kill_style_cheepcheep || style == kill_style_bulletbill || style == kill_style_feather))
		killer->killsinrowinair++;

	//Play announcer
	bool fSoundPlayed = false;
	if(killer->killsinrowinair > 1)
	{
		if(ifsoundonandreadyplay(sfx_announcer[9]) > -1)
			fSoundPlayed = true;
	}

	if(killed && killed->killsinrow >= 2 && !fSoundPlayed)
	{
		if(ifsoundonandreadyplay(sfx_announcer[10]) > -1)
			fSoundPlayed = true;
	}

	short awardIndex = 0;
	if(killer->killsinrow >= 2)
	{	
		awardIndex = (killer->killsinrow - 2) >= PAWARD_LAST ? PAWARD_LAST -1 : (killer->killsinrow - 2);
		
		if(!fSoundPlayed)
		{
			if(ifsoundonandreadyplay(sfx_announcer[awardIndex]) > -1)
				fSoundPlayed = true;
		}
	}

	//Add eyecandy
	if(game_values.awardstyle != award_style_none)
	{
		if(game_values.awardstyle == award_style_halo)
			killer->awards[(killer->killsinrow - 1) % MAXAWARDS] = (short)style;
		else if(game_values.awardstyle == award_style_souls)
		{
			if(killed)
				killer->awards[(killer->killsinrow - 1) % MAXAWARDS] = killed->colorID;
			else if(style == kill_style_goomba)
				killer->awards[(killer->killsinrow - 1) % MAXAWARDS] = 4; //soul id for goomba
			else if(style == kill_style_bulletbill)
				killer->awards[(killer->killsinrow - 1) % MAXAWARDS] = 5; //soul id for bullet bill
			else if(style == kill_style_cheepcheep)
				killer->awards[(killer->killsinrow - 1) % MAXAWARDS] = 6; //soul id for cheep cheep
			else if(style == kill_style_koopa)
				killer->awards[(killer->killsinrow - 1) % MAXAWARDS] = 7; //soul id for koopa
			else
				killer->awards[(killer->killsinrow - 1) % MAXAWARDS] = 8; //soul id for ?
				
		}
		else if(game_values.awardstyle == award_style_swirl)
			killer->addswirlingawards();
		else if(game_values.awardstyle == award_style_fireworks)
			killer->addrocketawards();
		
		if(killer->killsinrowinair > 1)
			killer->AddKillsInRowInAirAward();

		//if we have enough kills in a row -> spawn an award	

		if(game_values.awardstyle == award_style_text)
		{
			if(killer->killsinrow >= 2)
			{	
				char	text[128];				//text to show
				sprintf(text, "%d - %s", killer->killsinrow, awards[awardIndex].name);

				//now add the eyecandy
				eyecandyfront.add(new EC_GravText(awards[awardIndex].font, killer->ix + HALFPW, killer->iy+PH, text, -VELJUMP));
			}

			//if we stopped the other players run show another award
			if(killed && killed->killsinrow >= 2)
			{
				short a = (killed->killsinrow - 2) >= PAWARD_LAST ? PAWARD_LAST -1 : (killed->killsinrow - 2);
				char text[128];
				sprintf(text, "%s Stopped!",  awards[a].name);

				eyecandyfront.add(new EC_GravText(awards[a].font, killed->ix + HALFPW, killed->iy+PH, text, -VELJUMP*1.3f));
			}
		}
	}
}

//this is no member of CPlayer, but can only be used with CPlayer
//and it belongs to the context p2p collision detection + response (CPlayer->isstomping, collisionhandler, ...)

//handles a collision between two players (is being called if o1, o2 collide)
void collisionhandler_p2p(CPlayer &o1, CPlayer &o2)
{
	//If teams tag each other
	if(o1.teamID == o2.teamID)
	{
		if(game_values.gamemode->gamemode == game_mode_jail && game_values.gamemodesettings.jail.tagfree)
		{
			if(o1.jailed > 0)
			{
				eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, o1.ix + HALFPW - 16, o1.iy + HALFPH - 16, 3, 8));
				ifsoundonplay(sfx_transform);
				o1.jailed = 0;
			}

			if(o2.jailed > 0)
			{
				eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, o2.ix + HALFPW - 16, o2.iy + HALFPH - 16, 3, 8));
				ifsoundonplay(sfx_transform);
				o2.jailed = 0;
			}
		}
		
		//Don't collision detect players on same team if friendly fire is turned off
		if(!game_values.friendlyfire)
			return;
	}

	//--- 1. is player invincible? ---
	if(o1.invincible && !o2.invincible && !o2.spawninvincible)
	{
		PlayerKilledPlayer(o1, o2, death_style_jump, kill_style_star);
		return;
	}
	if(!o1.invincible && !o1.spawninvincible && o2.invincible)
	{
		PlayerKilledPlayer(o2, o1, death_style_jump, kill_style_star);
		return;
	}
	
	//If neither can touch each other, then return
	if((o1.invincible && o2.invincible) || (o1.spawninvincible && o2.spawninvincible) ||
		(o1.iswarping() && o2.iswarping()))
		return;

	//--- 2. stomping other player? ---
	if(!o2.spawninvincible && !o2.invincible && o1.isstomping(o2))
		return;
	if(!o1.spawninvincible && !o1.invincible && o2.isstomping(o1))
		return;
	
	//If either is shielded return because there is no pushback collision detection
	if((o1.spawninvincible && (game_values.gamemode->tagged != &o1 || !game_values.gamemodesettings.tag.tagontouch)) || 
		(o2.spawninvincible && (game_values.gamemode->tagged != &o2 || !game_values.gamemodesettings.tag.tagontouch)))
		return;

	//--- 3. push back (horizontal) ---
	if(o1.ix < o2.ix)				//o1 is left -> o1 pushback left, o2 pushback right
		_collisionhandler_p2p_pushback(o1, o2);
	else
		_collisionhandler_p2p_pushback(o2, o1);
}

//handles a collision between a player and an object
bool collisionhandler_p2o(CPlayer &o1, CObject &o2)
{
	return o2.collide(&o1);
}

//calculates the new positions for both players when they are pushing each other
void _collisionhandler_p2p_pushback(CPlayer &o1, CPlayer &o2)
{
	//o1 is left to o2
	//  |o1||o2|
	//-----------

	//Transfer tag on touching other players
	if(game_values.gamemode->gamemode == game_mode_tag && game_values.gamemodesettings.tag.tagontouch)
	{
		if(game_values.gamemode->tagged == &o1 && o1.isready() && o2.isready())
		{
			game_values.gamemode->tagged = &o2;
			o1.spawninvincible = true;
			o1.spawninvincibletimer = 60;
			eyecandyfront.add(new EC_GravText(&game_font_large, game_values.gamemode->tagged->ix + HALFPW, game_values.gamemode->tagged->iy + PH, "Tagged!", -VELJUMP*1.5));
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, game_values.gamemode->tagged->ix + HALFPW - 16, game_values.gamemode->tagged->iy + HALFPH - 16, 3, 8));
			ifsoundonplay(sfx_transform);
		}
		else if(game_values.gamemode->tagged == &o2 && o1.isready() && o2.isready())
		{
			game_values.gamemode->tagged = &o1;
			o2.spawninvincible = true;
			o2.spawninvincibletimer = 60;
			eyecandyfront.add(new EC_GravText(&game_font_large, game_values.gamemode->tagged->ix + HALFPW, game_values.gamemode->tagged->iy + PH, "Tagged!", -VELJUMP*1.5));
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, game_values.gamemode->tagged->ix + HALFPW - 16, game_values.gamemode->tagged->iy + HALFPH - 16, 3, 8));
			ifsoundonplay(sfx_transform);
		}
	}

	bool overlapcollision = false;
	if(o1.ix + PW < 320 && o2.ix > 320)
		overlapcollision = true;

	if((o1.velx == 0 && o2.iswarping() && o2.velx != 0) || o1.iswarping())
	{
		if(overlapcollision)
		{//o2 reposition to the right side of o1, o1 stays
			o2.xi(o1.ix - PW + 639);
			o2.collision_detection_checkleft();
		}
		else
		{
			o2.xi(o1.ix + PW + 1);
			o2.collision_detection_checkright();
		}
	}
	else if((o2.velx == 0 && o1.iswarping() && o1.velx != 0) || o2.iswarping())
	{
		if(overlapcollision)
		{//o1 reposition to the left side of o2, o2 stays
			o1.xi(o2.ix + PW - 639);	
			o1.collision_detection_checkright();
		}
		else
		{
			o1.xi(o2.ix - PW - 1);	
			o1.collision_detection_checkleft();
		}
	}
	else if(!o1.iswarping() && !o2.iswarping())
	{	//both objects moving - calculate middle and set both objects
		
		if(overlapcollision)
		{
			short middle = o2.ix - 640 + ((o1.ix + PW) - o2.ix - 640) / 2;		//no ABS needed (o1.x < o2.x -> o1.x+w > o2.x !)
			o1.xi(middle + 1);	//o1 is left
			o2.xi(middle - PW + 639);		//o2 is right

			o1.collision_detection_checkright();
			o2.collision_detection_checkleft();
		}
		else
		{
			short middle = o2.ix + ((o1.ix + PW) - o2.ix) / 2;		//no ABS needed (o1.x < o2.x -> o1.x+w > o2.x !)
			//printf("hlf:%f, o1x:%f, o2x:%f\n", hlf, o1.x, o2.x);
			o1.xi(middle - PW - 1);	//o1 is left
			o2.xi(middle + 1);		//o2 is right

			o1.collision_detection_checkleft();
			o2.collision_detection_checkright();
		}
	}

	float absv1 = 0.0f;
	float absv2 = 0.0f;
	
	if(overlapcollision)
	{
		absv1 = ( o1.velx < 0 ? o1.velx : -1.0f ) * 1.5f;	//o1 is on the left side (only positive velx counts)
		absv2 = ( o2.velx > 0 ? o2.velx : 1.0f ) * 1.5f;	//o2 right (only negative velx counts)
	}
	else
	{
		absv1 = ( o1.velx > 0 ? o1.velx : 1.0f ) * 1.5f;	//o1 is on the left side (only positive velx counts)
		absv2 = ( o2.velx < 0 ? o2.velx : -1.0f ) * 1.5f;	//o2 right (only negative velx counts)
	}

	if(o1.state == player_ready)
		o1.velx = CapSideVelocity(absv2);
	
	if(o2.state == player_ready)
		o2.velx = CapSideVelocity(absv1);
}


void CPlayer::draw()
{
	//Don't draw a player that is waiting to respawn
	if(state == player_wait || state == player_spawning)
		return;

	//Draw player
	pScoreboardSprite = sprites;

	if (statue_timer)
    {
        if (isready() && (statue_timer < 50) && (statue_timer /3 %2))
                /*|| statue_timer > 110*/   //Looks better with immediate transformation
            return;

		if(iswarping())
			spr_statue.draw(ix - PWOFFSET, iy - 31, 0, 0, 32, 58, (short)state % 4, warpplane);
		else
			spr_statue.draw(ix - PWOFFSET, iy - 31);

        return;
    }
	else if(bobomb) //draw him as bob-omb
	{
		pScoreboardSprite = spr_bobomb[colorID];

		//Add smoke to the top of the bomb
		if(++bobombsmoketimer > 2 && (velx != 0.0f || vely != GRAVITATION) && state == player_ready)
		{
			bobombsmoketimer = 0;
			eyecandyfront.add(new EC_SingleAnimation(&spr_bobombsmoke, ix + HALFPH - 8, iy - PHOFFSET - 8, 4, 4));
		}
	}
	else if(game_values.gamemode->chicken == this) //draw him as chicken
	{
		pScoreboardSprite = spr_chocobo[colorID];
	}

	if(ownerPlayerID > -1)
	{
		if(iswarping())
			spr_ownedtags.draw(ix - PWOFFSET - 8, iy - PHOFFSET - 8, ownerColorOffsetX, 0, 48, 48, (short)state % 4, warpplane);
		else
			spr_ownedtags.draw(ix - PWOFFSET - 8, iy - PHOFFSET - 8, ownerColorOffsetX, 0, 48, 48);
	}
	
	if(powerup == 3)
		DrawCape();

	if(state > player_ready) //warping
		pScoreboardSprite[spr]->draw(ix - PWOFFSET, iy - PHOFFSET, iSrcOffsetX, 0, 32, 32, (short)state % 4, warpplane);
	else
		pScoreboardSprite[spr]->draw(ix - PWOFFSET, iy - PHOFFSET, iSrcOffsetX, 0, 32, 32);

	//Draw the crown on the player
	if(game_values.showwinningcrown && g_iWinningPlayer == teamID)
		spr_crown.draw(ix + HALFPW - (IsPlayerFacingRight() ? 4 : 10), iy - 10);

	if(state < player_ready)
		return;

	if(jailed > 0)
	{
		if(state > player_ready) //warping
			spr_jail.draw(ix - PWOFFSET - 6, iy - PHOFFSET - 6, 0, 0, 44, 44, (short)state % 4, warpplane);
		else
			spr_jail.draw(ix - PWOFFSET - 6, iy - PHOFFSET - 6);
	}

	//Draw the Ring awards
	if(game_values.awardstyle == award_style_halo && killsinrow >= MINAWARDSNEEDED)
	{
		awardangle += 0.02f;

		if(awardangle > TWO_PI)
			awardangle -= TWO_PI;

		short numawards = (killsinrow > MAXAWARDS ? MAXAWARDS : killsinrow);
		float addangle = TWO_PI / (float)numawards;

		short xoffset = ix + HALFPW - 8;
		short yoffset = iy + HALFPH - 8;

		for(short k = 0; k < numawards; k++)
		{
			float angle = (float)k * addangle + awardangle;
			short awardx = xoffset + (short)(30.0f * cos(angle));
			short awardy = yoffset + (short)(30.0f * sin(angle));

			if(state > player_ready) //warping
				spr_award.draw(awardx, awardy, awards[k] * 16, 0, 16, 16, (short)state % 4, warpplane);
			else
				spr_award.draw(awardx, awardy, awards[k] * 16, 0, 16, 16);
		}
	}

	//Draw the powerup ring when a powerup is being used
	if(powerupused > -1)
	{
		short numeyecandy = 8;
		float addangle = TWO_PI / numeyecandy;
		float displayangle = powerupangle;

		for(short k = 0; k < numeyecandy; k++)
		{
			short powerupX = ix + HALFPW - 8 + (short)(powerupradius * cos(displayangle));
			short powerupY = iy + HALFPH - 8 + (short)(powerupradius * sin(displayangle));

			displayangle += addangle;
		
			if(state > player_ready) //warping
				spr_storedpowerupsmall.draw(powerupX, powerupY, powerupused * 16, 0, 16, 16, (short)state %4, warpplane);
			else
				spr_storedpowerupsmall.draw(powerupX, powerupY, powerupused * 16, 0, 16, 16);
		}
	}


	//game_font_large.drawf(ix, iy, "%d", super_boomerang_throw_index_left);
	//game_font_large.drawf(ix, iy + 24, "%d", holdright);

	if((dashLeft && velx < -VELDASHMOVING + 1) || (dashRight && velx > VELDASHMOVING - 1))
	{
		if(++dashSparkleAnimationTimer > 2)
		{
			dashSparkleAnimationTimer = 0;
			eyecandyfront.add(new EC_SingleAnimation(&spr_coinsparkle, ix + HALFPW - 16, iy + HALFPH - 16, 7, 4));
		}
	}
}

void CPlayer::DrawCape()
{
	if(++iCapeTimer > 3)
	{
		if((!inair && velx != 0.0f) || (inair && vely < 1.0f))
		{
			iCapeFrameX += 32;
			if(iCapeFrameX > 96)
				iCapeFrameX = 0;

			iCapeFrameY = 0;
			fCapeUp = true;
			iCapeYOffset = 0;
		}
		else if(!inair)
		{
			if(fCapeUp)
			{
				fCapeUp = false;
				iCapeFrameX = 0;
			}
			else
			{
				iCapeFrameX = 32;
			}

			iCapeFrameY = 96;
			iCapeYOffset = 0;
		}
		/*
		//This code will draw a different jumping cape animation (currently not used)
		else if(inair && vely < 1.0f)
		{
			iCapeFrameX += 32;
			if(iCapeFrameX > 64)
				iCapeFrameX = 0;

			iCapeFrameY = 32;
			fCapeUp = true;
			iCapeYOffset = -8;
		}
		*/
		else if(inair)
		{
			iCapeFrameX += 32;
			if(iCapeFrameX > 64)
				iCapeFrameX = 0;

			iCapeFrameY = 64;
			fCapeUp = true;
			iCapeYOffset = -18;
		}
		
		iCapeTimer = 0;
	}

	bool fPlayerFacingRight = IsPlayerFacingRight();
	if(iswarping())
		spr_cape.draw(ix - PWOFFSET + (fPlayerFacingRight ? - 18 : 18), iy - PHOFFSET + 4 + iCapeYOffset, 0 + (fPlayerFacingRight ? 128 : 0) + iCapeFrameX, iCapeFrameY, 32, 32, (short)state %4, warpplane);
	else
		spr_cape.draw(ix - PWOFFSET + (fPlayerFacingRight ? - 18 : 18), iy - PHOFFSET + 4 + iCapeYOffset, 0 + (fPlayerFacingRight ? 128 : 0) + iCapeFrameX, iCapeFrameY, 32, 32);
}

void CPlayer::drawarrows()
{
	if(iy < 0)
	{
		if(iy + PH < -1 || (iy + PH <= 0 && vely <= 1))
		{
			spr_abovearrows.draw(ix - PWOFFSET, 0, colorID * 32, 0, 32, 26);

			//This displays the out of arena timer before the player is killed
			if(game_values.outofboundstime > 0)
				spr_awardkillsinrow[colorID].draw(ix - PWOFFSET + 8, 18, outofarenadisplaytimer * 16, 0, 16, 16);
		}
	}
}

void CPlayer::updateswap()
{
	if(state != player_ready)
		return;

	if(game_values.swapstyle == 1)
	{
		xf(game_values.swapplayersblink ? fOldSwapX : fNewSwapX);
		yf(game_values.swapplayersblink ? fOldSwapY : fNewSwapY);
	}
	else
	{
		xf(((fNewSwapX - fOldSwapX) * game_values.swapplayersposition) + fOldSwapX);
		yf(((fNewSwapY - fOldSwapY) * game_values.swapplayersposition) + fOldSwapY);
	}

	if(carriedItem)
		carriedItem->MoveToOwner();
}

void CPlayer::drawswap()
{
	if(state != player_ready)
	{
		draw();
		return;
	}
	
	if(game_values.swapstyle != 1)
		SetSprite();
	
	draw();

	if(carriedItem)
		carriedItem->draw();
}

void CPlayer::collision_detection_map()
{
	xf(fx + velx);
	flipsidesifneeded();

	fPrecalculatedY = fy + vely;  //Fixes weird float rounding error.  Must be computed here before casting to int.  Otherwise, this will miss the bottom collision, but then hit the side collision and the player can slide out of 1x1 spaces.

	float fPlatformVelX = 0.0f;
	float fPlatformVelY = 0.0f;

	float fTempY = fy;

	if(platform)
	{
		//if(!onice)
		//{
			fPlatformVelX = platform->fVelX;
			xf(fx + fPlatformVelX);
			flipsidesifneeded();
		//}
		
		fPlatformVelY = platform->fVelY;

		if(platform->fOldVelY < 0.0f)
			fy += platform->fOldVelY;
		
		fPrecalculatedY += platform->fOldVelY;
	}
	
	iHorizontalPlatformCollision = -1;
	iVerticalPlatformCollision = -1;

	g_map.movingPlatformCollision(this);

	if(state != player_ready)
		return;

	fy = fTempY;

	if(fPrecalculatedY + PH < 0.0f)
	{				// on top outside of the screen
		yf(fPrecalculatedY);
		vely = CapFallingVelocity(GRAVITATION + vely);

		if(!platform)
		{
			inair = true;
			onice = false;
			superjumptimer = 0;
			fallthrough = false;
		}

		return;
	}
	else if(fPrecalculatedY + PH >= 480.0f)
	{	//on ground outside of the screen?
		yi(-PH);
		fOldY = (float)(-PH - 1);
		fallthrough = false;
		onice = false;

		return;
	}

	//Could be optimized with bit shift >> 5
	short ty = (short)fy / TILESIZE;
	short ty2 = ((short)fy + PH) / TILESIZE;
	short tx = -1;

	//printf("Before X - ix: %d\tiy: %d\toldx: %.2f\toldy: %.2f\tty: %d\tty2: %d\ttxl: %d\ttxr: %d\tfx: %.2f\tfy: %.2f\tvelx: %.2f\tvely: %.2f\n", ix, iy, fOldX, fOldY, ty, ty2, ix/TILESIZE, (ix+PW)/TILESIZE, fx, fy, velx, vely);

   	//-----------------------------------------------------------------
	//  x axis (--)
	//-----------------------------------------------------------------
	if(fy + PH >= 0.0f)
	{
		if(velx + fPlatformVelX > 0.01f || iHorizontalPlatformCollision == 3)
		{		//moving right
			if(fx + PW >= 640.0f)
			{
				tx = (short)(fx + PW - 640.0f) / TILESIZE;
				fOldX -= 640.0f;
			}
			else
                tx = ((short)fx + PW) / TILESIZE;

			//Just in case tx out of bounds and flipsidesifneeded wasn't called
			if(tx < 0)
				tx += 20;
			else if(tx > 19)
				tx -= 20;

			IO_Block * topblock = g_map.block(tx, ty);
			IO_Block * bottomblock = g_map.block(tx, ty2);

			//first check to see if player hit a warp
			if(g_map.warp(tx,ty)->direction == 3 && g_map.warp(tx,ty2)->direction == 3 && playerKeys->game_right.fDown &&
				g_map.warp(tx,ty)->connection == g_map.warp(tx,ty2)->connection && 
				!g_map.isconnectionlocked(g_map.warp(tx,ty)->connection) && !g_map.isconnectionlocked(g_map.warp(tx,ty2)->connection) &&
				(!game_values.slowdownfreeze || game_values.slowdownon == teamID))
			{
				xf((float)(tx * TILESIZE - PW) - 0.2f);
				enterwarp(g_map.warp(tx, ty2));
				warpplane = ix + PW + 1;

				if(iy - PHOFFSET < ty * TILESIZE)
					yi(ty * TILESIZE + PHOFFSET);
				else if(iy + PH > ty2 * TILESIZE + TILESIZE - 3)
					yi(ty2 * TILESIZE + TILESIZE - PH - 3);

				return;
			}
			else if((topblock && !topblock->isTransparent()) || 
				(bottomblock && !bottomblock->isTransparent()))
			{
				if(topblock && !topblock->isTransparent()) //collide with top block
				{	
					if(iHorizontalPlatformCollision == 3)
					{
						KillPlayerMapHazard();
						return;
					}

					topblock->collide(this, 1, true);
					flipsidesifneeded();
				}
				
				if(bottomblock && !bottomblock->isTransparent()) //then bottom
				{	
					if(iHorizontalPlatformCollision == 3)
					{
						KillPlayerMapHazard();
						return;
					}

					bottomblock->collide(this, 1, true);
					flipsidesifneeded();
				}
			}
			else if((g_map.map(tx, ty) == tile_death || g_map.map(tx, ty2) == tile_death) && !invincible && !spawninvincible)
			{
				KillPlayerMapHazard();
				return;
			}
			else if(g_map.map(tx, ty) & 0x05 || g_map.map(tx, ty2) & 0x05)
			{	//collision on the right side.

				if(iHorizontalPlatformCollision == 3)
				{
					KillPlayerMapHazard();
					return;
				}

				xf((float)(tx * TILESIZE - PW) - 0.2f);			//move to the edge of the tile (tile on the right -> mind the player width)
				fOldX = fx;
				
				if(velx > 0.0f)
					velx = 0.0f;

				flipsidesifneeded();
			}
		}
		else if(velx + fPlatformVelX < -0.01f || iHorizontalPlatformCollision == 1)
		{	//moving left
			tx = (short)fx / TILESIZE;

			//Just in case tx out of bounds and flipsidesifneeded wasn't called
			if(tx < 0)
				tx += 20;
			else if(tx > 19)
				tx -= 20;

			IO_Block * topblock = g_map.block(tx, ty);
			IO_Block * bottomblock = g_map.block(tx, ty2);

			//first check to see if player hit a warp
			if(g_map.warp(tx,ty)->direction == 1 && g_map.warp(tx,ty2)->direction == 1 && playerKeys->game_left.fDown && 
				g_map.warp(tx,ty)->connection == g_map.warp(tx,ty2)->connection && 
				!g_map.isconnectionlocked(g_map.warp(tx,ty)->connection) && !g_map.isconnectionlocked(g_map.warp(tx,ty2)->connection) &&
				(!game_values.slowdownfreeze || game_values.slowdownon == teamID))
			{
				xf((float)(tx * TILESIZE + TILESIZE) + 0.2f);
				enterwarp(g_map.warp(tx, ty2));
				warpplane = ix;

				if(iy - PHOFFSET < ty * TILESIZE)
					yi(ty * TILESIZE + PHOFFSET);
				else if(iy + PH > ty2 * TILESIZE + TILESIZE - 3)
					yi(ty2 * TILESIZE + TILESIZE - PH - 3);

				return;
			}
			else if((topblock && !topblock->isTransparent()) || 
				(bottomblock && !bottomblock->isTransparent()))
			{
				if(topblock && !topblock->isTransparent()) //collide with top block
				{	
					if(iHorizontalPlatformCollision == 1)
					{
						KillPlayerMapHazard();
						return;
					}

					topblock->collide(this, 3, true);
					flipsidesifneeded();
				}
				
				if(bottomblock && !bottomblock->isTransparent()) //then bottom
				{	
					if(iHorizontalPlatformCollision == 1)
					{
						KillPlayerMapHazard();
						return;
					}

					bottomblock->collide(this, 3, true);
					flipsidesifneeded();
				}
			}
			else if((g_map.map(tx, ty) == tile_death || g_map.map(tx, ty2) == tile_death) && !invincible && !spawninvincible)
			{
				KillPlayerMapHazard();
				return;
			}
			else if(g_map.map(tx, ty) & 0x05 || g_map.map(tx, ty2) & 0x05)
			{
				if(iHorizontalPlatformCollision == 1)
				{
					KillPlayerMapHazard();
					return;
				}

				xf((float)(tx * TILESIZE + TILESIZE) + 0.2f);			//move to the edge of the tile
				fOldX = fx;
				
				if(velx < 0.0f)
					velx = 0.0f;

				flipsidesifneeded();
			}
		}
	}

	//-----------------------------------------------------------------
	//  then y axis (|)
	//-----------------------------------------------------------------

	short txl = -1, txr = -1, txc = -1;

	txl = ix / TILESIZE;

	if(txl < 0)
		txl += 20;
	else if(txl > 19)
		txl -= 20;

	if(ix + HALFPW >= 640)
		txc = (ix + HALFPW - 640) / TILESIZE;
	else
		txc = (ix + HALFPW) / TILESIZE;

	if(ix + PW >= 640)
        txr = (ix + PW - 640) / TILESIZE;
	else
		txr = (ix + PW) / TILESIZE;


	//What block is the player aligned to (this will be the block that has the action on it)
	short alignedBlockX = 0;
	short unAlignedBlockX = 0;
	float unAlignedBlockFX = 0;

	//printf("Before Y - ix: %d\tiy: %d\toldx: %.2f\toldy: %d\tty: %.2f\tty2: %d\ttxl: %d\ttxr: %d\tfx: %.2f\tfy: %.2f\tvelx: %.2f\tvely: %.2f\n", ix, iy, fOldX, fOldY, (int)(fPrecalculatedY) / TILESIZE, ((int)(fPrecalculatedY) + PH) / TILESIZE, txl, txr, fx, fy, velx, vely);

	short overlaptxl = txl * TILESIZE + TILESIZE + 1;
	if(overlaptxl > 639)
		overlaptxl -= 640;

	if(ix + HALFPW < overlaptxl)
	{
		alignedBlockX = txl;
		unAlignedBlockX = txr;
		unAlignedBlockFX = (float)(txr * TILESIZE - PW) - 0.2f;
	}
	else
	{
		alignedBlockX = txr;
		unAlignedBlockX = txl;
		unAlignedBlockFX = (float)(txl * TILESIZE + TILESIZE) + 0.2f;
	}

	float fMovingUp = vely;
	if(platform)
		fMovingUp = vely + fPlatformVelY - GRAVITATION;

	if(fMovingUp < -0.01f)
	{	//moving up
		fallthrough = false;

		ty = (short)(fPrecalculatedY) / TILESIZE;

		IO_Block * leftblock = g_map.block(txl, ty);
		IO_Block * centerblock = g_map.block(txc, ty);
		IO_Block * rightblock = g_map.block(txr, ty);
		
		if(g_map.warp(alignedBlockX, ty)->direction == 2 && g_map.warp(unAlignedBlockX, ty)->direction == 2 && playerKeys->game_jump.fDown && 
			g_map.warp(alignedBlockX, ty)->connection == g_map.warp(unAlignedBlockX, ty)->connection && 
			!g_map.isconnectionlocked(g_map.warp(alignedBlockX, ty)->connection) && !g_map.isconnectionlocked(g_map.warp(unAlignedBlockX, ty)->connection) &&
			(!game_values.slowdownfreeze || game_values.slowdownon == teamID))
		{
			yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			enterwarp(g_map.warp(unAlignedBlockX, ty));
			warpplane = iy;

			if(ix - PWOFFSET < txl * TILESIZE + 1)
				xi(txl * TILESIZE + PHOFFSET + 1);
			else if(ix + PW + PWOFFSET > txr * TILESIZE + TILESIZE)
				xi(txr * TILESIZE + TILESIZE - PW - PWOFFSET);

			return;
		}
		
		if(centerblock && !centerblock->isTransparent())
		{
			if(!centerblock->collide(this, 0, true))
			{
				if(iVerticalPlatformCollision == 2)
					KillPlayerMapHazard();

				return;
			}
		}

		//Player hit a solid, ice or death on top
		//or if the player is invincible and hits death or death on bottom
		if(g_map.map(alignedBlockX, ty) & 0x1 ||
			((invincible || spawninvincible) && (g_map.map(alignedBlockX, ty) & 0x4)))
		{
			yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			fOldY = fy - 1.0f;
			
			if(vely < 0.0f)
				vely = -vely * BOUNCESTRENGTH;
			
			if(iVerticalPlatformCollision == 2)
				KillPlayerMapHazard();

			return;
		}

		if(leftblock && !leftblock->isTransparent()) //then left
		{	
			bool useBehavior = alignedBlockX == txl || rightblock == NULL || rightblock->isTransparent();
				
			if(!leftblock->collide(this, 0, useBehavior))
			{
				if(iVerticalPlatformCollision == 2)
					KillPlayerMapHazard();

				return;
			}
		}

		if(rightblock && !rightblock->isTransparent()) //then right
		{	
			bool useBehavior = alignedBlockX == txr || leftblock == NULL || leftblock->isTransparent();
				
			if(!rightblock->collide(this, 0, useBehavior))
			{
				if(iVerticalPlatformCollision == 2)
					KillPlayerMapHazard();

				return;
			}
		}
		
		//Player squeezed around the block, ice or death on top
		//or if the player is invincible and hits death or death on bottom
		if(g_map.map(unAlignedBlockX, ty) & 0x1 ||
			((invincible || spawninvincible) && (g_map.map(unAlignedBlockX, ty) & 0x4)))
		{
			xf(unAlignedBlockFX);
			fOldX = fx;

			yf(fPrecalculatedY);
			vely += GRAVITATION;
		}
		else if(g_map.map(alignedBlockX, ty) & 0x4 || g_map.map(unAlignedBlockX, ty) & 0x4)
		{
			KillPlayerMapHazard();
			return;
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
		ty = ((short)fPrecalculatedY + PH) / TILESIZE;
		
		if(g_map.warp(txl,ty)->direction == 0 && g_map.warp(txr,ty)->direction == 0 && playerKeys->game_down.fDown && 
			g_map.warp(txl, ty)->connection == g_map.warp(txr, ty)->connection && 
			!g_map.isconnectionlocked(g_map.warp(txl,ty)->connection) && !g_map.isconnectionlocked(g_map.warp(txr,ty)->connection) && 
			(!game_values.slowdownfreeze || game_values.slowdownon == teamID))
		{
			yf((float)(ty * TILESIZE - PH) - 0.2f);
			enterwarp(g_map.warp(txr,ty));
			warpplane = iy + PH + 1;

			fallthrough = false;
			platform = NULL;

			if(ix - PWOFFSET < txl * TILESIZE + 1)
				xi(txl * TILESIZE + PHOFFSET + 1);
			else if(ix + PW + PWOFFSET > txr * TILESIZE + TILESIZE)
				xi(txr * TILESIZE + TILESIZE - PW - PWOFFSET);

			return;
		}
		
		IO_Block * leftblock = g_map.block(txl, ty);
		IO_Block * rightblock = g_map.block(txr, ty);

		if((leftblock && !leftblock->isTransparent()) || 
			(rightblock && !rightblock->isTransparent()))
		{
			bool collisionresult = true;
			if(leftblock && !leftblock->isTransparent()) //collide with left block
			{	
				collisionresult &= leftblock->collide(this, 2, alignedBlockX == txl || rightblock == NULL || rightblock->isTransparent());
				
				//If player was bumped and killed then return
				if(state != player_ready)
					return;
			}
			
			if(rightblock && !rightblock->isTransparent()) //then right
			{	
				collisionresult &= rightblock->collide(this, 2, alignedBlockX == txr || leftblock == NULL || leftblock->isTransparent());

				//If player was bumped and killed then return
				if(state != player_ready)
					return;
			}

			if(!collisionresult)
			{
				platform = NULL;
				onice = false;

				if(iVerticalPlatformCollision == 0)
					KillPlayerMapHazard();

				return;
			}
		}
		
		TileType lefttile = g_map.map(txl, ty);
		TileType righttile = g_map.map(txr, ty);

		bool fSolidTileUnderPlayer = (lefttile & 0x5) == 1 || lefttile == tile_death_on_bottom || (righttile & 0x5) == 1 || righttile == tile_death_on_bottom;

		if((lefttile == tile_solid_on_top || righttile == tile_solid_on_top) && fOldY + PH <= ty * TILESIZE)
		{	//on ground
			//Deal with player down jumping through solid on top tiles
			if(fallthrough && !fSolidTileUnderPlayer)
			{
				yf((float)(ty * TILESIZE - PH) + 0.2f);

				if(!platform)
				{
					inair = true;
				}
			}
			else
			{
				//we were above the tile in the previous frame
				yf((float)(ty * TILESIZE - PH) - 0.2f);
				vely = GRAVITATION;

				if(!platform)
				{
					inair = false;
					featherjump = 0;
					killsinrowinair = 0;
				}
			}

			fOldY = fy - GRAVITATION;
			
			if(!platform)
			{
				fallthrough = false;
				onice = false;
			}

			platform = NULL;

			if(iVerticalPlatformCollision == 0)
				KillPlayerMapHazard();

			return;
		}
		
		if(fSolidTileUnderPlayer ||
			((invincible || spawninvincible) && (lefttile == tile_death_on_top || righttile == tile_death_on_top ||
			lefttile == tile_death || righttile == tile_death)))
		{	//on ground

			yf((float)(ty * TILESIZE - PH) - 0.2f);
			vely = GRAVITATION;				//1 so we test against the ground again int the next frame (0 would test against the ground in the next+1 frame)

			if(!platform)
			{
				TileType alignedtile = g_map.map(alignedBlockX, ty);

				if(alignedtile == tile_ice || (alignedtile == tile_nonsolid && g_map.map(unAlignedBlockX, ty) == tile_ice))
					onice = true;
				else 
					onice = false;

				inair = false;
				featherjump = 0;
				killsinrowinair = 0;
			}
			platform = NULL;

			if(iVerticalPlatformCollision == 0)
			{
				KillPlayerMapHazard();
				return;
			}
		}
		else if(lefttile == tile_death_on_top || righttile == tile_death_on_top ||
			lefttile == tile_death || righttile == tile_death)
		{
			KillPlayerMapHazard();
			return;
		}
		else
		{
			//falling (in air)
			yf(fPrecalculatedY);
			vely = CapFallingVelocity(GRAVITATION + vely);

			if(!platform)
			{
				superjumptimer = 0;
				inair = true;
			}
		}
	}

	if(!platform)
	{
		fallthrough = false;
		
		if(inair)
			onice = false;
	}
	
	//printf("After Y - ix: %d\tiy: %d\toldx: %.2f\toldy: %.2f\tty: %d\tty2: %d\ttxl: %d\ttxr: %d\tfx: %.2f\tfy: %.2f\tvelx: %.2f\tvely: %.2f\n\n", ix, iy, fOldX, fOldY, ty, ty, txl, txr, fx, fy, velx, vely);
}

void CPlayer::KillPlayerMapHazard()
{
	if(pSuicideCreditPlayer)
	{
		PlayerKilledPlayer(*pSuicideCreditPlayer, *this, death_style_jump, kill_style_bounce);
	}
	else
	{
		DeathAwards();
			
		if(!game_values.gamemode->playerkilledself(*this))
			die(death_style_jump, false);

		ifsoundonplay(sfx_deathsound);
	}
}

bool CPlayer::collision_detection_checktop()
{
	if(iy < 0.0f)
		return false;

	short ty = iy / TILESIZE;

	if(ty < 0 || ty >= MAPHEIGHT)
		return false;

	short txl = ix / TILESIZE;

	if(txl < 0 || txl >= MAPWIDTH)
		return false;

	short txr = -1;
	if(ix + PW >= 640)
		txr = (ix + PW - 640) / TILESIZE;
	else
		txr = (ix + PW) / TILESIZE;

	if(txr < 0 || txr >= MAPWIDTH)
		return false;
	
	if((g_map.map(txl, ty) != tile_nonsolid && g_map.map(txl, ty) != tile_solid_on_top) || 
		(g_map.map(txr, ty) != tile_nonsolid && g_map.map(txr, ty) != tile_solid_on_top) ||
		(g_map.block(txl, ty) && !g_map.block(txl, ty)->isTransparent()) || 
		(g_map.block(txr, ty) && !g_map.block(txr, ty)->isTransparent()))
	{
		yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
		return true;
	}

	return false;
}

bool CPlayer::collision_detection_checkleft()
{
	if(fy < 0.0f)
		return false;

	short ty = (short)fy / TILESIZE;

	if(ty < 0 || ty >= MAPHEIGHT)
		return false;

	short ty2 = ((short)fy + PH) / TILESIZE;

	if(ty2 < 0 || ty2 >= MAPHEIGHT)
		return false;

	short tx = ix / TILESIZE;

	if(tx < 0 || tx >= MAPWIDTH)
		return false;

	if((g_map.map(tx, ty) != tile_nonsolid && g_map.map(tx, ty) != tile_solid_on_top) ||
		(g_map.map(tx, ty2) != tile_nonsolid && g_map.map(tx, ty2) != tile_solid_on_top) ||
		(g_map.block(tx, ty) && !g_map.block(tx, ty)->isTransparent()) || 
		(g_map.block(tx, ty2) && !g_map.block(tx, ty2)->isTransparent()))
	{
		xf((float)(tx * TILESIZE + TILESIZE) + 0.2f);
		flipsidesifneeded();
		return true;
	}

	return false;
}

bool CPlayer::collision_detection_checkright()
{
	if(fy < 0.0f)
		return false;

	short ty = (short)fy / TILESIZE;

	if(ty < 0 || ty >= MAPHEIGHT)
		return false;

	short ty2 = ((short)fy + PH) / TILESIZE;

	if(ty2 < 0 || ty2 >= MAPHEIGHT)
		return false;

	short tx = -1;

	if(ix + PW >= 640)
		tx = (ix + PW - 640) / TILESIZE;
	else
		tx = (ix + PW) / TILESIZE;

	if(tx < 0 || tx >= MAPWIDTH)
		return false;

	if((g_map.map(tx, ty) != tile_nonsolid && g_map.map(tx, ty) != tile_solid_on_top) ||
		(g_map.map(tx, ty2) != tile_nonsolid && g_map.map(tx, ty2) != tile_solid_on_top) ||
		(g_map.block(tx, ty) && !g_map.block(tx, ty)->isTransparent()) || 
		(g_map.block(tx, ty2) && !g_map.block(tx, ty2)->isTransparent()))
	{
		xf((float)(tx * TILESIZE - PW) - 0.2f);
		flipsidesifneeded();
		return true;
	}

	return false;
}

void CPlayer::collision_detection_checksides()
{
	//First figure out where the corners of this object are touching
	Uint8 iCase = 0;

	short txl = ix / TILESIZE;

	short txr = -1;
	if(ix + PW >= 640)
		txr = (ix + PW - 640) / TILESIZE;
	else
		txr = (ix + PW) / TILESIZE;

	short ty = iy / TILESIZE;
	short ty2 = (iy + PH) / TILESIZE;

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

	if(iy + PW >= 0.0f)
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
			break;
		
		//[X][ ]
		//[ ][ ]
		case 1:
		{
			if(ix + (PW >> 1) > txl * TILESIZE + TILESIZE)
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
			if(ix + (PW >> 1) < txr * TILESIZE)
			{
				xf((float)(txr * TILESIZE - PW) - 0.2f);
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
			if(ix + (PW >> 1) > txl * TILESIZE + TILESIZE)
			{
				xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty2 * TILESIZE - PH) - 0.2f);
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
			if(ix + (PW >> 1) > txl * TILESIZE + TILESIZE)
			{
				yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
				xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty2 * TILESIZE - PH) - 0.2f);
				xf((float)(txr * TILESIZE - PW) - 0.2f);
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
			if(ix + (PW >> 1) < txr * TILESIZE)
			{
				xf((float)(txr * TILESIZE - PW) - 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty2 * TILESIZE - PH) - 0.2f);
			}

			break;
		}

		//[X][ ]
		//[ ][X]
		case 9:
		{
			if(ix + (PW >> 1) > txl * TILESIZE + TILESIZE)
			{
				yf((float)(ty2 * TILESIZE - PH) - 0.2f);
				xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
				flipsidesifneeded();
			}
			else
			{
				yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
				xf((float)(txr * TILESIZE - PW) - 0.2f);
				flipsidesifneeded();
			}

			break;
		}

		//[ ][X]
		//[ ][X]
		case 10:
		{
			xf((float)(txr * TILESIZE - PW) - 0.2f);
			flipsidesifneeded();
			break;
		}

		//[X][X]
		//[ ][X]
		case 11:
		{
			yf((float)(ty * TILESIZE + TILESIZE) + 0.2f);
			xf((float)(txr * TILESIZE - PW) - 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][ ]
		//[X][X]
		case 12:
		{
			yf((float)(ty2 * TILESIZE - PH) - 0.2f);
			break;
		}

		//[X][ ]
		//[X][X]
		case 13:
		{
			yf((float)(ty2 * TILESIZE - PH) - 0.2f);
			xf((float)(txl * TILESIZE + TILESIZE) + 0.2f);
			flipsidesifneeded();
			break;
		}

		//[ ][X]
		//[X][X]
		case 14:
		{
			yf((float)(ty2 * TILESIZE - PH) - 0.2f);
			xf((float)(txr * TILESIZE - PW) - 0.2f);
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
}

void CPlayer::enterwarp(Warp * warp)
{
	if(warp->direction == 0)
	{
		state = player_entering_warp_down;
		vely = 0.0f;
		velx = 0.0f;
	}
	else if(warp->direction == 1)
	{
		state = player_entering_warp_left;
		vely = 0.0f;
		velx = -1.0f;
	}
	else if(warp->direction == 2)
	{
		state = player_entering_warp_up;
		vely = 0.0f;
		velx = 0.0f;
	}
	else if(warp->direction == 3)
	{
		state = player_entering_warp_right;
		vely = 0.0f;
		velx = 1.0f;
	}

	warpconnection = warp->connection;
	warpid = warp->id;

	if(game_values.warplocks > 0)
		g_map.lockconnection(warpconnection);

	ifsoundonplay(sfx_pipe);
}

void CPlayer::flipsidesifneeded()
{
	//Use ix here to avoid rounding issues (can crash if txr evals to over the right side of screen)
	if(ix < 0 || fx < 0.0f)
	{
		//This avoids rounding errors
		xf(fx + 640.0f);
		fOldX += 640.0f;
		//printf("Flipped Left\n");
	}
	else if(ix >= 640 || fx >= 640.0f)
	{
		xf(fx - 640.0f);
		fOldX -= 640.0f;
		//printf("Flipped Right\n");
	}
}

void CPlayer::makeinvincible()
{
	invincible = true;
	invincibletimer = 0;
	animationstate = 0;
	animationtimer = 0;
	spawninvincible = false;
	spawninvincibletimer = 0;

	//Stop the invincible music if a player is already invincible 
	//(we don't want two invincible music sounds playing at the same time)
	ifsoundonstop(sfx_invinciblemusic);
	
	if(!game_values.gamemode->gameover)
	{
		game_values.playinvinciblesound = true;

		if(game_values.music && game_values.sound)
			backgroundmusic[0].stop();
	}
}

void CPlayer::turnslowdownon(bool fSuperFreeze)
{
	game_values.slowdownon = teamID;
	game_values.slowdowncounter = 0;
	game_values.slowdownfreeze = fSuperFreeze;
}

//Returns true if player facing right, false if left
bool CPlayer::IsPlayerFacingRight()
{
	if(game_values.swapplayers && game_values.swapstyle == 0)
	{
		if(fNewSwapX < fOldSwapX)
			return false;
		else 
			return true;
	}

	if(state == player_ready)
	{
		if(playerKeys->game_left.fDown && playerKeys->game_right.fDown && velx != 0.0f)
		{
			if(velx > 0.0f)
				return true;
			else
				return false;
		}
		else
		{
			if(playerKeys->game_left.fDown)
				return false;
			else if(playerKeys->game_right.fDown)
				return true;
		}
	}
	
	if(spr == PGFX_STOPPING_R)
		return false;
	else if(spr == PGFX_STOPPING_L)
		return true;
	else if((spr & 0x1) == 0)
		return true;

	return false;
}

bool CPlayer::AcceptItem(MO_CarriedObject * item)
{
	if(fAcceptingItem && statue_timer == 0)
	{
		carriedItem = item;
		fAcceptingItem = false;
		return true;
	}

	return false;
}

void CPlayer::SetPowerup(short iPowerup)
{
	//Play sounds for collecting a powerup
	if(powerup == iPowerup || (bobomb && iPowerup == 0) || iPowerup > 6)
	{
		ifsoundonplay(sfx_storepowerup);
	}
	else if(iPowerup == 0)
	{
		ifsoundonplay(sfx_transform);
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
	}
	else if(iPowerup == 3)
	{
		ifsoundonplay(sfx_collectfeather);
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, ix + HALFPW - 16, iy + HALFPH - 16, 3, 8));
	}
	else
	{
		ifsoundonplay(sfx_collectpowerup);
	}

    if(iPowerup == 0)
	{
		if(bobomb)
			game_values.gamepowerups[globalID] = 8;

		bobomb = true;
	}
	else if(iPowerup > 6)
	{
		if(iPowerup == 7)
			game_values.gamepowerups[globalID] = 9;
		else if(iPowerup == 8)
			game_values.gamepowerups[globalID] = 16;
		else if(iPowerup == 9)
			game_values.gamepowerups[globalID] = 10;
		else if(iPowerup > 9)
			game_values.gamepowerups[globalID] = iPowerup + 2; //Storing shells
	}
	else
	{
		if(powerup == 1)
			game_values.gamepowerups[globalID] = 5;
		else if(powerup == 2)
			game_values.gamepowerups[globalID] = 11;
		else if(powerup == 3)
			game_values.gamepowerups[globalID] = 17;
		else if(powerup == 4)
			game_values.gamepowerups[globalID] = 19;
		else if(powerup == 5)
			game_values.gamepowerups[globalID] = 21;
		else if(powerup == 6)
			game_values.gamepowerups[globalID] = 23;

		powerup = iPowerup;
		projectilelimit = 0;

		if(powerup == 1)
		{
			if(game_values.fireballlimit > 0)
				projectilelimit = game_values.fireballlimit;
		}
		else if(powerup == 2)
		{
			if(game_values.hammerlimit > 0)
				projectilelimit = game_values.hammerlimit;
		}
		else if(powerup == 3)
		{
			if(game_values.featherlimit > 0)
				projectilelimit = game_values.featherlimit;
		}
		else if(powerup == 4)
		{
			if(game_values.boomeranglimit > 0)
				projectilelimit = game_values.boomeranglimit;
		}
		else if(powerup == 5)
		{
			if(game_values.hammerlimit > 0)
				projectilelimit = game_values.hammerlimit;
		}
	}

	//Minor fix for becoming caped to draw animation correctly
	if(iPowerup == 3)
		iCapeTimer = 4;
}

void CPlayer::DecreaseProjectileLimit()
{
	if(--projectilelimit <= 0)
	{
		projectilelimit = 0;
		powerup = 0;
		ifsoundonplay(sfx_powerdown);
	}
}
