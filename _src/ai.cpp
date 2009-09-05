#include "global.h"
#include "ai.h"

/* 
Controlling the Player
----------------------
If a player is AI controlled, then the Think() method is called once every 4 game frames 
(62 avg. fps / 4 = 15 times/sec)
The method is called with a reference to the player's COutputControl struct, which is what is
used to send input to the player.  This is the same struct that the gamepad/keyboard fills out.
Essentially, you are replacing the gamepad.

The COutputControl has 6 bools you should set to control the player. Before you start setting this object,
you should probably clear out the values and set everything to false.  The controls are:

playerKeys->game_left -> Move the player left
playerKeys->game_right -> Move right (setting both right and left is like setting neither)
playerKeys->game_jump -> Make the player jump (player's jump will be cut short if jump is set false before 
						 the maximum jump height is achieved)
playerKeys->game_down -> Set true and jump to true if you want player to fall through a solid on top tile
playerKeys->game_turbo -> Make the player run faster or shoot a projectile or hold it down to pick up an item
playerKeys->game_powerup -> Use a stored powerup item

Each of the 6 inputs above have an fPressed and fDown member.  The game code uses only the fDown (input
held down).  The menu uses the fPressed events so your AI will not use those here.  You'll notice in the
current AI that we're only setting the fDown member.  

Current AI
----------
The way the current AI works is, first we call GetNearestObjects().  This fills out the nearestObjects 
class with the nearest player, goal (can be powerups, mode goals like coins, pretty much anything good
to collect), threat (shells, fireballs, etc), teammate (for freeing from jail), and stomp goal (for
figuring out things to jump on in stomp mode).

This class is then used by the Think() method to determine what to do.  The first part of Think() determines
which of the close items are more important.  Evading threats is more important than stomping goals, which is
more important than tagging teammates/tagging goals, etc.

Once that is figured out, then we deal with the single situation that we picked (set in actionType).
Each actionType has special handling which involves things like figuring out whether to run away or to head
towards an opponent, whether to drop my current item to pick up another, etc.

After the single closest most important item is dealt with, we worry about trying to avoid falling onto hazards.

Then lastly, we use stored powerups if it is to our advantage (like don't use a projectile powerup if we
already have one).

When you watch the current AI in action, you'll notice that it is very good at stomping other players.  It is
also very good at collecting objects on open maps.  But since it doesn't have any path finding, it is very poor
at navigating the map so in maps with lots of solid platforms, it has trouble jumping onto those platforms to
get to its goal.


Accessing The Map
-----------------
There are some things that will probably be useful when creating your own AI that the current code doesn't use.
First is the map tiles.  This will tell you what tiles are in the map to help you write path finding.  Use:

int tile = g_map.map(column, row); 

to get the tile at that column and row.  There are 20 columns and 15 rows.  To convert object position to map
col and row, use:

int tile = g_map.map(object->x / TILESIZE, object->y / TILESIZE); 

The int returned is an bit flag int of all the attributes of tiles like solid, solid-on-top, death on various sides, ice, etc.

Second is the interaction blocks in the map.  You might want to know where a [?] block is so you can go hit it.

short blocktype = g_map.blockat(col, row)->iType;

blocktype will now be set to the type id for the block in that column and row.  If it is set to -1, then
there is no interaction block in that space.

0 == Breakable block
1 == Powerup Block
2 == Donut Block
3 == Flip Block
4 == Bounce Block
5 == Note Block
6 == Throw Block
7 to 10 == On/Off Switch
11 to 14 == On/Off [!] Block

Another thing that is probably useful is the state of the block at that location.  A player probably isn't interested
in a [?] that has been used and won't give you a powerup.

To get this information, we use:

if(g_map.blockat(col, row)->iType == 1)
{
	B_PowerupBlock * block = (B_PowerupBlock*) g_map.block(col, row);
	if(block->GetState() == 0)
	{
		//Contains powerup and ready to be hit
	}
}

Flip blocks and On/Off Blocks have a solid state and a fall through state.  To get this information, use:

if(g_map.blockat(col, row)->iType == 3)
{
	B_FlipBlock * block = (B_FlipBlock*) g_map.block(col, row);
	if(block->isTransparent())
	{
		//Flip block is spinning and player will fall through
	}
	else
	{
		//Block is solid
	}
}

Substitute "B_FlipBlock" for "B_SwitchBlock" to do switch blocks.

Player Object
-------------
You also have access to the player object that you are controlling.  You must be careful not to set
any of the settings of this player object and use them read only.  There are a lot of assumptions in the 
player code that these values will not be modified by the AI.  Here are some members you will probably 
find useful:

1. Player is in the air
pPlayer->inair

2. True if player is on ice:
pPlayer->onice

3. Player's velocity
pPlayer->velx;
pPlayer->vely;

4. Player's position:
pPlayer->ix;
pPlayer->iy;

5. Get the object the player is currently carrying:

MO_CarriedObject * object = pPlayer->carriedItem;
MovingObjectType mot = object->getMovingObjectType();

if(mot == movingobject_shell)
	then do something with the shell

other moving object enums you'll probably want to use:
movingobject_throwblock, movingobject_egg, movingobject_star, movingobject_flag

6. Player powerups
pPlayer->powerup == -1 for none, 0 for bobomb, 1 for fireballs, 2 for hammer, 3 for feather, 4 for boomerang

7. Figure out what direction the player is facing (good for knowing if you should throw a shell or other object)
pPlayer->IsPlayerFacingRight();

8. Player is invincible
pPlayer->invincible

9. True if the player is a bobomb:
pPlayer->bobomb

10. Player is "it" in tagged mode:
game_values.gamemode->tagged == pPlayer

11. Player is "it" in chicken mode:
game_values.gamemode->chicken == pPlayer

12. Player has star in star mode:
if(game_values.gamemode->gamemode == game_mode_star)
{
	bool playerIsStar = ((CGM_Star*)game_values.gamemode)->isplayerstar(pPlayer);
}

13. If greater than 0, player is jailed in jail mode:
pPlayer->jailtimer;

14. When true, these actions were pressed and need to be released before pressing action again:
pPlayer->lockfall
pPlayer->lockjump
pPlayer->lockfire


Difficulty Settings
-------------------
Your AI code will also need to deal with difficutly settings.  You can access the difficulty level
using the global variable: game_values.cpudifficulty.  0 is the easiest setting and 4 is the most
difficult.  Your AI should be unhindered and make the smartest decisions for setting 4.

The current AI uses the difficulty setting in a very simple way.  It randomly just doesn't do anything
on its turn based on the setting.  For setting 0, it only makes a decision 25% of the time, where at
setting 4, it makes decisions 100% of the time.  See below.


Using Your New AI
--------------------
When you create your own AI class, look for where we create the CPlayer objects at the beginning of the
RunGame() function in main.cpp.  You'll see we create a new CPlayerAI() for every player that has
game_values.playercontrol[iPlayer] set to 2 (AI controlled).  Simply replace: 

ai = new CPlayerAI();

with:

ai = new CYourAIClass();

and the computer players will now be controlled by your AI.

Issues With The Current AI
--------------------------
1. Bots don't use the tanooki powerup.
2. If a bot is holding something(Not a ztar/shine/egg) in ztar, or yoshi's egg mode, they can't pick up the ztar/shine/egg.
3. Bots can't pick up throwable blocks that they are standing on.

*/

CPlayerAI::CPlayerAI()
{
	currentAttentionObject.iID = -1;
	currentAttentionObject.iType = 0;
	currentAttentionObject.iTimer = 0;
}

CPlayerAI::~CPlayerAI()
{
	std::map<int, AttentionObject*>::iterator itr = attentionObjects.begin(), lim = attentionObjects.end();
	while (itr != lim)
	{
		delete (itr->second);
		itr++;
	}

	attentionObjects.clear();
}

//Setup AI so that it can ignore or pay attention to some objects
void CPlayerAI::Init()
{
	//Scan yoshi's egg mode objects to make sure that we ignore eggs with out matching yoshis
	if(game_values.gamemode->gamemode == game_mode_eggs)
	{
		bool fYoshi[4] = {false, false, false, false};
		//Scan Yoshis to see which ones are present
		for(short i = 0; i < objectcontainer[1].list_end; i++)
		{
			CObject * object = objectcontainer[1].list[i];

			if(object_moving == object->getObjectType())
			{
				IO_MovingObject * movingobject = (IO_MovingObject*)object;

				if(movingobject_yoshi == movingobject->getMovingObjectType())
				{
					MO_Yoshi * yoshi = (MO_Yoshi*)movingobject;
					fYoshi[yoshi->getColor()] = true;
				}
			}
		}

		//Now scan eggs and ignore any egg that doesn't have a yoshi
		for(short i = 0; i < objectcontainer[1].list_end; i++)
		{
			CObject * object = objectcontainer[1].list[i];

			if(object_moving == object->getObjectType())
			{
				IO_MovingObject * movingobject = (IO_MovingObject*)object;

				if(movingobject_egg == movingobject->getMovingObjectType())
				{
					CO_Egg * egg = (CO_Egg*)movingobject;
					if(!fYoshi[egg->getColor()])
					{
						AttentionObject * ao = new AttentionObject();
						ao->iID = egg->iNetworkID;
						ao->iType = 1;     //Ignore this object
						ao->iTimer = 0;		//Ignore it forever

						attentionObjects[ao->iID] = ao;
					}
				}
			}
		}
	}
}

void CPlayerAI::Think(COutputControl * playerKeys)
{
	short iDecisionPercentage[5] = {25, 35, 50, 75, 100};

	//Clear out the old input settings
	playerKeys->game_left.fDown = false;
	playerKeys->game_right.fDown = false;

	//A percentage of the time the cpu will do nothing based on the difficulty level
	if(rand() % 100 > iDecisionPercentage[game_values.cpudifficulty] && pPlayer->isready())
		return;

	playerKeys->game_jump.fDown = false;
	playerKeys->game_down.fDown = false;
	playerKeys->game_turbo.fDown = false;
	playerKeys->game_powerup.fDown = false;

	if(pPlayer->isdead() || pPlayer->isspawning())
		return;

	/***************************************************
	* 1. Figure out what objects are nearest to us
	***************************************************/
	GetNearestObjects();

	short iTenSeconds = 15 * iDecisionPercentage[game_values.cpudifficulty] / 10;

	//If there is a goal, then make sure we aren't paying attention to it for too long
	if(nearestObjects.goal)
	{
		if(currentAttentionObject.iID == nearestObjects.goal->iNetworkID)
		{
			//If we have been paying attention to this goal for too long, then start ignoring it
			if(++currentAttentionObject.iTimer > iTenSeconds)
			{
				AttentionObject * ao = new AttentionObject();
				ao->iID = currentAttentionObject.iID;
				ao->iType = 1;     //Ignore this object
				ao->iTimer = iTenSeconds;

				attentionObjects[ao->iID] = ao;
			}
		}
		else
		{
			currentAttentionObject.iID = nearestObjects.goal->iNetworkID;
			currentAttentionObject.iTimer = 0;
		}
	}
	else
	{
		currentAttentionObject.iID = -1;
	}

	//Expire attention objects
	std::map<int, AttentionObject*>::iterator itr = attentionObjects.begin(), lim = attentionObjects.end();
	while(itr != lim)
	{
		if(itr->second->iTimer > 0)
		{
			if(--(itr->second->iTimer) == 0)
			{
				delete itr->second;
			
				attentionObjects.erase(itr++);
				lim = attentionObjects.end();
			}
		}

		++itr;
	}

	short iStoredPowerup = game_values.gamepowerups[pPlayer->globalID];
	MO_CarriedObject * carriedItem = pPlayer->carriedItem;
	short ix = pPlayer->ix;
	short iy = pPlayer->iy;
	short iTeamID = pPlayer->teamID;

	/***************************************************
	* 2. Figure out priority of actions
	***************************************************/

	short actionType = 0;
	if(nearestObjects.threat && nearestObjects.threatdistance < 14400)
	{
		actionType = 2;
	}
	else if(nearestObjects.stomp && nearestObjects.stompdistance < 14400)
	{
		actionType = 4;
	}
	else if(nearestObjects.goal && nearestObjects.teammate)
	{
		if(nearestObjects.goaldistance < nearestObjects.teammatedistance)
			actionType = 1;
		else
			actionType = 3;
	}
	else if(nearestObjects.teammate)
	{
		actionType = 3;
	}
	else if(nearestObjects.goal)
	{
		if(nearestObjects.playerdistance < 4096)
			actionType = 0;
		else
			actionType = 1;
	}
	else
	{
		actionType = 0;
	}
	
	/***************************************************
	* 3. Deal with closest item
	***************************************************/
	//return if no players, goals, or threats available
	if((actionType != 0 || nearestObjects.player) && (actionType != 1 || nearestObjects.goal) && (actionType != 2 || nearestObjects.threat) && 
		(actionType != 3 || nearestObjects.teammate) && (actionType != 4 || nearestObjects.stomp) && iFallDanger == 0)
	{
		//Use turbo
		if(pPlayer->bobomb)
		{
			if(nearestObjects.playerdistance <= 4096 || nearestObjects.stompdistance <= 4096 || nearestObjects.threatdistance <= 4096)
				playerKeys->game_turbo.fDown = true;
		}
		else
		{
			if(pPlayer->powerup == -1 || !(rand() % 20))
				playerKeys->game_turbo.fDown = true;

			if(carriedItem)
			{
				//Hold important mode goal objects (we'll drop the star later if it is a bad star)
				MovingObjectType carriedobjecttype = carriedItem->getMovingObjectType();
				if(carriedobjecttype == movingobject_egg || carriedobjecttype == movingobject_flag ||
					carriedobjecttype == movingobject_star || carriedobjecttype == movingobject_phantokey)
				{
					playerKeys->game_turbo.fDown = true;
				}
			}
		}

		if(actionType == 0)  //Kill nearest player
		{
			CPlayer * player = nearestObjects.player;
			bool * moveToward;
			bool * moveAway;
			
			if((player->ix > ix && nearestObjects.playerwrap) || (player->ix < ix && !nearestObjects.playerwrap))
			{
				moveToward = &playerKeys->game_left.fDown;
				moveAway = &playerKeys->game_right.fDown;
			}
			else
			{
				moveToward = &playerKeys->game_right.fDown;
				moveAway = &playerKeys->game_left.fDown;
			}

			//Move player relative to opponent when we are playing star mode and we are holding a star
			if(game_values.gamemode->gamemode == game_mode_star)
			{
				if(carriedItem && carriedItem->getMovingObjectType() == movingobject_star)
				{
					if(((CO_Star*)carriedItem)->getType() == 1)
						*moveAway = true;
					else
					{
						*moveToward = true;

						//If player is close
						if (player->iy <= iy && player->iy > iy - 60 &&
							player->ix - ix < 90 && player->ix - ix > -90)
						{
							//And we are facing toward that player, throw the star
							if(((player->ix > ix && pPlayer->IsPlayerFacingRight())) ||
								(player->ix < ix && !pPlayer->IsPlayerFacingRight()))
							{
								pPlayer->throw_star = 30;
								playerKeys->game_turbo.fDown = false;
							}
						}
					}
				}
			}
			//Move player relative to opponent when we are carrying a weapon like a shell or throwblock
			else if(carriedItem)
			{
				if(carriedItem->getMovingObjectType() == movingobject_shell || carriedItem->getMovingObjectType() == movingobject_throwblock)
				{
					*moveToward = true;

					//If player is close
					if (player->iy > iy - 10 && player->iy < iy + 30 &&
						abs(player->ix - ix) < 150)
					{
						//And we are facing toward that player, throw the projectile
						if(((player->ix > ix && pPlayer->IsPlayerFacingRight())) ||
							(player->ix < ix && !pPlayer->IsPlayerFacingRight()))
						{
							playerKeys->game_turbo.fDown = false;
						}
					}
				}
			}
			else if(game_values.gamemode->tagged == player || player->invincible || player->shyguy || game_values.gamemode->chicken == pPlayer)
			{
				*moveAway = true;
			}
			else if(pPlayer->invincible || pPlayer->shyguy  || pPlayer->bobomb || game_values.gamemode->tagged == pPlayer)
			{
				*moveToward = true;
			}
			else if(player->iy >= iy && !player->invincible && !player->bobomb)
			{					
				*moveToward = true;
			}
			else
			{
				if(nearestObjects.playerdistance < 8100)	//else if player is near but higher, run away (left)
					*moveAway = true;
			}
			

			if (player->iy <= iy &&					//jump if player is higher or at the same level and
				player->ix - ix < 45 &&				//player is very near
				player->ix - ix > -45)
			{				//or if player is high
				playerKeys->game_jump.fDown = true;
			}
			else if (player->iy > iy &&			//try to down jump if player is below us
				player->ix - ix < 45 &&				
				player->ix - ix > -45 && rand() % 2)
			{				//or if player is high
				playerKeys->game_jump.fDown = true;
				playerKeys->game_down.fDown = true;

				if(!pPlayer->superstomp_lock)
				{
					//If the player is tanooki, then try to super stomp on them
					if(pPlayer->tanooki)
					{
						playerKeys->game_turbo.fPressed = true;
						pPlayer->lockfire = false;
					}
					else if(pPlayer->iKuriboShoe > 0)  //else if the player has the shoe then stomp
					{
						playerKeys->game_down.fPressed = true;
						playerKeys->game_jump.fPressed = true;
					}
				}
			}
			else if(iy - player->iy > 70)  //If the player is significatly below us, then jump
			{
				playerKeys->game_jump.fDown = true;
			}
			else
			{
				if(!(rand() % 60))
					playerKeys->game_jump.fDown = true;
			}
		}
		else if(actionType == 1)  //Go for goal
		{
			CObject * goal = nearestObjects.goal;
			
			if((goal->ix > ix && nearestObjects.goalwrap) || (goal->ix < ix && !nearestObjects.goalwrap))
				playerKeys->game_left.fDown = true;
			else
				playerKeys->game_right.fDown = true;

			if (goal->iy <= iy && goal->ix - ix < 45 && goal->ix - ix > -45)
			{
				playerKeys->game_jump.fDown = true;
			}
			else if (goal->iy > iy && goal->ix - ix < 45 && goal->ix - ix > -45)
			{
				playerKeys->game_jump.fDown = true;
				playerKeys->game_down.fDown = true;
			}
			else if(iy - goal->iy > 70)
			{
				playerKeys->game_jump.fDown = true;
			}
			else
			{
				if(!(rand() % 60))
					playerKeys->game_jump.fDown = true;
			}

			if(goal->getObjectType() == object_moving && ((IO_MovingObject*)goal)->getMovingObjectType() == movingobject_egg)
				playerKeys->game_turbo.fDown = true;
			else if(goal->getObjectType() == object_moving && ((IO_MovingObject*) goal)->getMovingObjectType() == movingobject_star && pPlayer->throw_star == 0)
				playerKeys->game_turbo.fDown = true;
			else if(goal->getObjectType() == object_moving && ((IO_MovingObject*) goal)->getMovingObjectType() == movingobject_flag)
				playerKeys->game_turbo.fDown = true;

			//Drop current item if we're going after another carried item
			if(carriedItem)
			{
				if(goal->getObjectType() == object_moving)
				{
					MovingObjectType goalobjecttype = ((IO_MovingObject*)goal)->getMovingObjectType();
					if(goalobjecttype == movingobject_egg || goalobjecttype == movingobject_flag ||
						goalobjecttype == movingobject_star || goalobjecttype == movingobject_phantokey)
					{
						MovingObjectType carriedobjecttype = carriedItem->getMovingObjectType();

						//If we are holding something that isn't a mode goal object, then drop it
						if(carriedobjecttype != movingobject_egg && carriedobjecttype != movingobject_flag &&
							carriedobjecttype != movingobject_star && carriedobjecttype != movingobject_phantokey)
						{
							playerKeys->game_turbo.fDown = false;
						}
					}
				}
			}

			//Open treasure chests in bonus houses in world mode
			if(goal->getObjectType() == object_moving && ((IO_MovingObject*)goal)->getMovingObjectType() == movingobject_treasurechest)
				playerKeys->game_turbo.fPressed = true;
		}
		else if(actionType == 2)  //Evade Threat
		{
			CObject * threat = nearestObjects.threat;

			//If threat, always use turbo
			playerKeys->game_turbo.fDown = true;

			if((threat->ix > ix && nearestObjects.threatwrap) || (threat->ix < ix && !nearestObjects.threatwrap))
				playerKeys->game_right.fDown = true;
			else
				playerKeys->game_left.fDown = true;

			if (threat->iy <= iy && threat->ix - ix < 60 && threat->ix - ix > -60)
			{
				playerKeys->game_jump.fDown = true;
				playerKeys->game_down.fDown = true;
			}
			else if (threat->iy > iy && threat->ix - ix < 60 && threat->ix - ix > -60)
			{
				playerKeys->game_jump.fDown = true;
			}
		}
		else if(actionType == 3)  //Tag teammate
		{
			CPlayer * teammate = nearestObjects.teammate;

			if((teammate->ix > ix && nearestObjects.teammatewrap) || (teammate->ix < ix && !nearestObjects.teammatewrap))
				playerKeys->game_left.fDown = true;
			else
				playerKeys->game_right.fDown = true;

			if (teammate->iy <= iy && teammate->ix - ix < 45 && teammate->ix - ix > -45)
			{
				playerKeys->game_jump.fDown = true;
			}
			else if (teammate->iy > iy && teammate->ix - ix < 45 && teammate->ix - ix > -45)
			{
				playerKeys->game_jump.fDown = true;
				playerKeys->game_down.fDown = true;
			}
			else if(iy - teammate->iy > 70)
			{
				playerKeys->game_jump.fDown = true;
			}
			else
			{
				if(!(rand() % 60))
					playerKeys->game_jump.fDown = true;
			}
		}
		else if(actionType == 4)  //Stomp something (goomba, koopa, cheepcheep)
		{
			CObject * stomp = nearestObjects.stomp;
			bool * moveToward;
			bool * moveAway;
			
			if((stomp->ix > ix && nearestObjects.stompwrap) || (stomp->ix < ix && !nearestObjects.stompwrap))
			{
				moveToward = &playerKeys->game_left.fDown;
				moveAway = &playerKeys->game_right.fDown;
			}
			else
			{
				moveToward = &playerKeys->game_right.fDown;
				moveAway = &playerKeys->game_left.fDown;
			}

			if(stomp->iy > iy + PH || pPlayer->shyguy || pPlayer->invincible || 
				(carriedItem && (carriedItem->getMovingObjectType() == movingobject_shell || carriedItem->getMovingObjectType() == movingobject_throwblock)))
			{	//if true stomp target is lower or at the same level, run toward
				*moveToward = true;
			}
			else
			{
				if(nearestObjects.stompdistance < 8100)
					*moveAway = true;
				else
					*moveToward = true;
			}


			if (stomp->iy <= iy + PH && nearestObjects.stompdistance < 2025)
			{
				playerKeys->game_jump.fDown = true;
			}
			else if(stomp->iy > iy + PH && nearestObjects.stompdistance < 2025)
			{
				playerKeys->game_jump.fDown = true;
				playerKeys->game_down.fDown = true;

				if(!pPlayer->superstomp_lock)
				{
					//If the player is tanooki, then try to super stomp on them
					if(pPlayer->tanooki)
					{
						playerKeys->game_turbo.fPressed = true;
						pPlayer->lockfire = false;
					}
					else if(pPlayer->iKuriboShoe > 0)  //else if the player has the shoe then stomp
					{
						playerKeys->game_down.fPressed = true;
						playerKeys->game_jump.fPressed = true;
					}
				}
			}
			else
			{
				if(!(rand() % 60))
					playerKeys->game_jump.fDown = true;
			}
		}
	}

	//"Star Mode" specific stuff
	//Drop the star if we're not it
	if(game_values.gamemode->gamemode == game_mode_star)
	{
		CGM_Star * starmode = (CGM_Star*)game_values.gamemode;

		if(carriedItem && carriedItem->getMovingObjectType() == movingobject_star && ((CO_Star*)carriedItem)->getType() == 0 && !starmode->isplayerstar(pPlayer))
			playerKeys->game_turbo.fDown = false;
		else if(starmode->isplayerstar(pPlayer) && pPlayer->throw_star == 0)
			playerKeys->game_turbo.fDown = true;
	}

	if(carriedItem)
	{
		MovingObjectType carriedobjecttype = carriedItem->getMovingObjectType();

		//"Phanto Mode" specific stuff
		if(game_values.gamemode->gamemode == game_mode_chase)
		{
			//Ignore the key if a phanto is really close
			if(nearestObjects.threat && nearestObjects.threat->getObjectType() == object_phanto && nearestObjects.threatdistance < 4096)
			{
				playerKeys->game_turbo.fDown = false;

				//Ignore the key for a little while
				if(attentionObjects.find(carriedItem->iNetworkID) != attentionObjects.end())
				{
					AttentionObject * ao = attentionObjects[carriedItem->iNetworkID];
					ao->iType = 1;
					ao->iTimer = iDecisionPercentage[game_values.cpudifficulty] / 3;
				}
				else
				{
					AttentionObject * ao = new AttentionObject();
					ao->iID = carriedItem->iNetworkID;
					ao->iType = 1;
					ao->iTimer = iDecisionPercentage[game_values.cpudifficulty] / 3;
					attentionObjects[ao->iID] = ao;
				}
			}
		}
		//If we are holding something that we are ignoring, drop it
		else if(attentionObjects.find(carriedItem->iNetworkID) != attentionObjects.end())
		{
			playerKeys->game_turbo.fDown = false;
		}

		//Drop live bombs if they are not yours
		if(carriedobjecttype == movingobject_bomb)
		{
			CO_Bomb * bomb = (CO_Bomb*)carriedItem;
			if(bomb->iTeamID != pPlayer->teamID)
				playerKeys->game_turbo.fDown = false;
		}
		else if(carriedobjecttype == movingobject_carried || carriedobjecttype == movingobject_throwbox)  //Drop springs,spikes,shoes
		{
			playerKeys->game_turbo.fDown = false;
		}
	}

	//Let go of the jump button so that we clear "lockjump" so we can jump again when we hit the ground if we want to
	if(pPlayer->inair && pPlayer->vely > 0 && (pPlayer->powerup != 3 || (pPlayer->powerup == 3 && pPlayer->lockjump)))
		playerKeys->game_jump.fDown = false;

	/***************************************************
	* 4. Deal with falling onto spikes (this needs to be improved)
	***************************************************/

	//Make sure we don't jump over something that could kill us
	short iDeathY = iy / TILESIZE;
	short iDeathX1 = ix / TILESIZE;
	short iDeathX2;
	
	if(ix + PW >= 640)
		iDeathX2 = (ix + PW - 640) / TILESIZE;
	else
		iDeathX2 = (ix + PW) / TILESIZE;
	
	if(iDeathY < 0)
		iDeathY = 0;

	//short depth = -1;
	while(iDeathY < MAPHEIGHT)
	{
		int ttLeftTile = g_map.map(iDeathX1, iDeathY);
		int ttRightTile = g_map.map(iDeathX2, iDeathY);

		bool fDeathTileUnderPlayer = ((ttLeftTile & tile_flag_death_on_top) && (ttRightTile & tile_flag_death_on_top)) ||
									 ((ttLeftTile & tile_flag_death_on_top) && !(ttRightTile & tile_flag_solid)) ||
									 (!(ttLeftTile & tile_flag_solid) && (ttRightTile & tile_flag_death_on_top));

		if(fDeathTileUnderPlayer)
		{
			if(iFallDanger == 0)
				iFallDanger = (playerKeys->game_right.fDown ? -1 : 1);

			goto ExitDeathCheck;
		}
		else if((ttLeftTile & tile_flag_solid) || (ttLeftTile & tile_flag_solid_on_top) || g_map.block(iDeathX1, iDeathY) ||
			(ttRightTile & tile_flag_solid) || (ttRightTile & tile_flag_solid_on_top) || g_map.block(iDeathX2, iDeathY))
		{
			iFallDanger = 0;
			goto ExitDeathCheck;
		}
		

		//Look through all platforms and see if we are hitting solid or death tiles in them
		for(short iPlatform = 0; iPlatform < g_map.iNumPlatforms; iPlatform++)
		{
			int lefttile = g_map.platforms[iPlatform]->GetTileTypeFromCoord(ix, iDeathY << 5);
			int righttile = g_map.platforms[iPlatform]->GetTileTypeFromCoord(ix + PW, iDeathY << 5);

			bool fDeathTileUnderPlayer = ((lefttile & tile_flag_death_on_top) && (righttile & tile_flag_death_on_top)) ||
									 ((lefttile & tile_flag_death_on_top) && !(righttile & tile_flag_solid)) ||
									 (!(lefttile & tile_flag_solid) && (righttile & tile_flag_death_on_top));

			if(fDeathTileUnderPlayer)
			{
				if(iFallDanger == 0)
					iFallDanger = (playerKeys->game_right.fDown ? -1 : 1);

				goto ExitDeathCheck;
			}
			else if(((lefttile & tile_flag_solid) || (lefttile & tile_flag_solid_on_top)) ||
				((righttile & tile_flag_solid) && (righttile & tile_flag_solid_on_top)))
			{
				iFallDanger = 0;
				goto ExitDeathCheck;
			}
		}

		iDeathY++;
	}

//If we are done checking for death under the player, come here
ExitDeathCheck:

	//There is a death tile below us so move to the side that is safest
	if(iFallDanger < 0)
	{
		playerKeys->game_right.fDown = true;
		playerKeys->game_left.fDown = false;
		playerKeys->game_jump.fDown = true;
		playerKeys->game_turbo.fDown = true;
		playerKeys->game_down.fDown = false;
	}
	else if(iFallDanger > 0)
	{
		playerKeys->game_right.fDown = false;
		playerKeys->game_left.fDown = true;
		playerKeys->game_jump.fDown = true;
		playerKeys->game_turbo.fDown = true;
		playerKeys->game_down.fDown = false;
	}

	//Make sure we don't jump up into something that can kill us
	iDeathY = iy / TILESIZE;

	if(iDeathY < 0)
		iDeathY = 0;
	
	short heightlimit = 3;
	while(iDeathY >= 0 && heightlimit > 0)
	{
		int ttLeftTile = g_map.map(iDeathX1, iDeathY);
		int ttRightTile = g_map.map(iDeathX2, iDeathY);

		if((ttLeftTile & tile_flag_solid && (ttLeftTile & tile_flag_death_on_bottom) == 0) ||
			(ttRightTile & tile_flag_solid && (ttRightTile & tile_flag_death_on_bottom) == 0) ||
			g_map.block(iDeathX1, iDeathY) || g_map.block(iDeathX2, iDeathY))
		{
			break;
		}
		else if(ttLeftTile & tile_flag_death_on_bottom || ttRightTile & tile_flag_death_on_bottom)
		{
			playerKeys->game_jump.fDown = false;
			break;
		}

		iDeathY--;
		heightlimit--;
	}

	/***************************************************
	* 5. Use stored powerups
	***************************************************/

	if(iStoredPowerup > 0)
	{
		//use 1-5up, clock, pow, bulletbill, mod, podobo, right away
		if(iStoredPowerup == 1  || iStoredPowerup == 2 || iStoredPowerup == 3 || iStoredPowerup == 4 ||
			iStoredPowerup == 7 || iStoredPowerup == 9 || iStoredPowerup == 10 || iStoredPowerup == 16 || 
			iStoredPowerup == 22)
		{
			playerKeys->game_powerup.fDown = true;
		}
		else if(((iStoredPowerup == 5 || iStoredPowerup == 11 || iStoredPowerup == 17 || iStoredPowerup == 19 || iStoredPowerup == 21 || iStoredPowerup == 23 || iStoredPowerup == 24 || iStoredPowerup == 25) && 
				pPlayer->powerup == -1) || //Use fireflower, hammer, feather, boomerang, cape, wings, sledge, bombs
				(iStoredPowerup == 6 && !pPlayer->invincible) || //Use star
				(iStoredPowerup == 8 && !pPlayer->bobomb) || //use bob-omb
				(iStoredPowerup >= 12 && iStoredPowerup <= 15 && !carriedItem) || //Use shell 
				(iStoredPowerup == 20 && !pPlayer->tanooki)) //use tanooki
		{
			playerKeys->game_powerup.fDown = true;
		}
		else if(iStoredPowerup == 18) //mystery mushroom
		{
			//See if another player has a powerup
			for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
			{
				if(iPlayer == pPlayer->localID || list_players[iPlayer]->teamID == iTeamID)
					continue;

				if(game_values.gamepowerups[list_players[iPlayer]->globalID] > 0)
				{
					playerKeys->game_powerup.fDown = true;
					break;
				}
			}
		}
		else if(iStoredPowerup == 26) //jail key
		{
			if(pPlayer->jailtimer > 0)
				playerKeys->game_powerup.fDown = true;
		}
	}
}

void CPlayerAI::GetNearestObjects()
{
	nearestObjects.Reset();

	MO_CarriedObject * carriedItem = pPlayer->carriedItem;
	bool fInvincible = pPlayer->invincible || pPlayer->shield || pPlayer->shyguy;
	short iTeamID = pPlayer->teamID;

	std::map<int, AttentionObject*>::iterator lim = attentionObjects.end();

	for(short i = 0; i < objectcontainer[1].list_end; i++)
	{
		CObject * object = objectcontainer[1].list[i];

		if(attentionObjects.find(object->iNetworkID) != lim)
		{
			//DistanceToObject(object, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
			continue;
		}

		ObjectType type = object->getObjectType();

		switch (type)
		{
			case object_moving:
			{
				IO_MovingObject * movingobject = (IO_MovingObject*)object;
				MovingObjectType movingtype = movingobject->getMovingObjectType();
				
				if(carriedItem == movingobject)
					continue;

				if(movingobject_shell == movingtype)
				{
					CO_Shell * shell = (CO_Shell*)movingobject;
					
					if(shell->IsThreat())
					{
						if(fInvincible)
							continue;
	
						DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
					}
					else if(carriedItem)
					{
						continue;
					}
					else
					{
						DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
					}
				}
				else if(movingobject_throwblock == movingtype || (movingobject_throwbox == movingtype && ((CO_ThrowBox*)movingobject)->HasKillVelocity()))
				{
					if(fInvincible)
						continue;
	
					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if(movingobject_pirhanaplant == movingtype)
				{
					MO_PirhanaPlant * plant = (MO_PirhanaPlant*)movingobject;
					if(plant->state > 0)
						DistanceToObjectCenter(plant, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);				
				}
				else if(movingobject_flag == movingtype)
				{
					CO_Flag * flag = (CO_Flag*)movingobject;
					
					if(flag->GetInBase() && flag->GetTeamID() == iTeamID)
						continue;

					if(carriedItem && carriedItem->getMovingObjectType() == movingobject_flag)
						continue;

					DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);			
				}
				else if(movingobject_yoshi == movingtype)
				{
					MO_Yoshi * yoshi = (MO_Yoshi*)movingobject;

					if(!carriedItem || carriedItem->getMovingObjectType() != movingobject_egg)
						continue;

					CO_Egg * egg = (CO_Egg*)carriedItem;
					
					if(yoshi->getColor() != egg->getColor())
						continue;

					DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else if(movingobject_egg == movingtype)
				{
					if(carriedItem && carriedItem->getMovingObjectType() == movingobject_egg)
						continue;

					DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else if(movingobject_throwbox == movingtype)
				{
					if(carriedItem)
						continue;

					DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else if(movingobject_star == movingtype)
				{
					if(carriedItem && carriedItem->getMovingObjectType() == movingobject_star)
						continue;

					CGM_Star * starmode = (CGM_Star*)game_values.gamemode;
					CO_Star * star = (CO_Star*)movingobject;
					
					if(star->getType() == 1 || starmode->isplayerstar(pPlayer))
					{
						DistanceToObject(star, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
					}
					else
					{
						DistanceToObject(star, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
					}
				}
				else if(movingobject_coin == movingtype || movingobject_phantokey == movingtype)
				{
					DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else if(movingobject_collectioncard == movingtype)
				{
					int iNumHeldCards = pPlayer->score->subscore[0];
					int iHeldCards = pPlayer->score->subscore[1];

					if(iNumHeldCards == 3)
					{
						int iThreeHeldCards = iHeldCards && 63;

						//If bot is holding all of the same type of card, then ignore all other cards
						if(iThreeHeldCards == 42 || iThreeHeldCards == 21 || iThreeHeldCards == 0)
						{
							DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
							break;
						}

						int iTwoHeldCards = iHeldCards && 15;
						if(iTwoHeldCards == 10 || iTwoHeldCards == 5 || iTwoHeldCards == 0)
						{
							DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);				
							break;
						}
					}
					else
					{
						MO_CollectionCard * card = (MO_CollectionCard*)movingobject;

						if(iNumHeldCards == 2)
						{
							int iTwoHeldCards = iHeldCards && 15;
						
							if(iTwoHeldCards == 10 || iTwoHeldCards == 5 || iTwoHeldCards == 0)
							{
								int iCardValue = card->getValue();
								if(card->getType() == 1 && ((iTwoHeldCards == 10 && iCardValue != 2) || (iTwoHeldCards == 5 && iCardValue != 1) || (iTwoHeldCards == 0 && iCardValue != 0)))
								{
									DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
									break;
								}
							}
						}
						else if(iNumHeldCards == 1)
						{
							int iHeldCard = iHeldCards && 3;
							int iCardValue = card->getValue();
							if(card->getType() == 1 || iHeldCard != iCardValue)
							{
								DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
								break;
							}
						}
					}

					if(iNumHeldCards == 3)
						DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
					else
						DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);				
				}

				break;
			}
			case object_frenzycard:
			{
				DistanceToObject(object, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				break;
			}

			case object_pipe_coin:
			{
				OMO_PipeCoin * coin = (OMO_PipeCoin*)object;

				if(coin->GetColor() != 0)
				{
					if(coin->GetTeam() == -1 || coin->GetTeam() == pPlayer->teamID)
						DistanceToObject(coin, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else
				{
					DistanceToObject(coin, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}

				break;
			}

			case object_pipe_bonus:
			{
				OMO_PipeBonus * bonus = (OMO_PipeBonus*)object;

				if(bonus->GetType() != 5)
				{
					DistanceToObject(bonus, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else
				{
					DistanceToObject(bonus, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				
				break;
			}

			case object_pathhazard:
			case object_orbithazard:
			{
				DistanceToObject(object, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				break;
			}

			case object_phanto:
			{
				OMO_Phanto * phanto = (OMO_Phanto*)object;

				if(phanto->GetType() == 2 || (carriedItem && carriedItem->getMovingObjectType() == movingobject_phantokey))
				{
					DistanceToObjectCenter(object, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}

				break;
			}

			case object_flamecannon:
			{
				IO_FlameCannon * cannon = (IO_FlameCannon *)object;

				if(cannon->state > 0)
					DistanceToObject(cannon, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);

				break;
			}

			default:
			{
				DistanceToObject(object, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				break;
			}
		}
	}

	for(short i = 0; i < objectcontainer[0].list_end; i++)
	{
		CObject * object = objectcontainer[0].list[i];

		if(attentionObjects.find(object->iNetworkID) != lim)
		{
			//DistanceToObject(object, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
			continue;
		}

		ObjectType type = object->getObjectType();

		switch (type)
		{
			case object_moving:
			{
				IO_MovingObject * movingobject = (IO_MovingObject*)object;
				MovingObjectType movingtype = movingobject->getMovingObjectType();
				
				if(carriedItem == movingobject)
					continue;

				if(movingobject_powerup == movingtype)
				{
					DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else if((movingobject_fireball == movingtype && ((MO_Fireball*)movingobject)->iTeamID != iTeamID)
					|| movingobject_poisonpowerup == movingtype)
				{
					if(fInvincible)
						continue;

					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if((movingobject_goomba == movingtype || movingobject_koopa == movingtype) && movingobject->GetState() == 1)
				{
					DistanceToObject(movingobject, &nearestObjects.stomp, &nearestObjects.stompdistance, &nearestObjects.stompwrap);
				}
				else if(movingobject_sledgebrother == movingtype)
				{
					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if(movingobject_treasurechest == movingtype)
				{
					DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else if(movingobject_flagbase == movingtype)
				{
					MO_FlagBase * flagbase = (MO_FlagBase*)movingobject;
					
					if(!carriedItem || carriedItem->getMovingObjectType() != movingobject_flag || flagbase->GetTeamID() != iTeamID)
						continue;

					DistanceToObject(movingobject, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				}
				else
				{
					continue;
				}
				break;
			}

			case object_area:
			{
				if(((OMO_Area*)object)->getColorID() == pPlayer->colorID)
					continue;
			
				DistanceToObject(object, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				break;
			}

			case object_kingofthehill_area:
			{
				DistanceToObjectCenter(object, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				break;
			}

			default:
			{
				DistanceToObject(object, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				break;
			}
		}
	}

	for(short i = 0; i < objectcontainer[2].list_end; i++)
	{
		CObject * object = objectcontainer[2].list[i];

		if(attentionObjects.find(object->iNetworkID) != lim)
		{
			//DistanceToObject(object, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
			continue;
		}

		ObjectType type = object->getObjectType();

		switch (type)
		{
			case object_moving:
			{
				IO_MovingObject * movingobject = (IO_MovingObject*)object;
				MovingObjectType movingtype = movingobject->getMovingObjectType();
				
				if(carriedItem == movingobject)
					continue;

				if(movingobject_cheepcheep == movingtype)
				{
					DistanceToObject(movingobject, &nearestObjects.stomp, &nearestObjects.stompdistance, &nearestObjects.stompwrap);
				}
				else if(movingobject_bulletbill == movingtype)
				{
					if(fInvincible)
						continue;

					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if(movingobject_hammer == movingtype && ((MO_Hammer*)movingobject)->iTeamID != iTeamID)
				{
					if(fInvincible)
						continue;

					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if(movingobject_iceblast == movingtype && ((MO_IceBlast*)movingobject)->iTeamID != iTeamID)
				{
					if(fInvincible)
						continue;

					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if(movingobject_boomerang == movingtype && ((MO_Boomerang*)movingobject)->iTeamID != iTeamID)
				{
					if(fInvincible)
						continue;

					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if(movingobject_bomb == movingtype && ((CO_Bomb*)movingobject)->iTeamID != iTeamID)
				{
					if(fInvincible)
						continue;

					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if(movingobject_podobo == movingtype && ((MO_Podobo*)movingobject)->iTeamID != iTeamID)
				{
					if(fInvincible)
						continue;

					DistanceToObject(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				}
				else if(movingobject_explosion == movingtype && ((MO_Explosion*)movingobject)->iTeamID != iTeamID)
				{
					if(fInvincible)
						continue;

					DistanceToObjectCenter(movingobject, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
					break;
				}

				break;
			}
			case object_thwomp:
			case object_bowserfire:
			{
				if(fInvincible)
					continue;

				DistanceToObjectCenter(object, &nearestObjects.threat, &nearestObjects.threatdistance, &nearestObjects.threatwrap);
				break;
			}
			case object_race_goal:
			{
				if(game_values.gamemode->gamemode != game_mode_race)
					continue;

				OMO_RaceGoal * racegoal = (OMO_RaceGoal*)object;

				if(racegoal->getGoalID() != ((CGM_Race*)game_values.gamemode)->getNextGoal(iTeamID))
					continue;
			
				DistanceToObject(racegoal, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				break;
			}
			

			default:
			{
				DistanceToObject(object, &nearestObjects.goal, &nearestObjects.goaldistance, &nearestObjects.goalwrap);
				break;
			}
		}
	}

	//Figure out where the other players are
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(iPlayer == pPlayer->localID || list_players[iPlayer]->state != player_ready)
			continue;

		//Find players in jail on own team to tag
		if(game_values.gamemode->gamemode == game_mode_jail)
		{
			if(list_players[iPlayer]->jailtimer > 0 && list_players[iPlayer]->teamID == iTeamID)
			{
				DistanceToPlayer(list_players[iPlayer], &nearestObjects.teammate, &nearestObjects.teammatedistance, &nearestObjects.teammatewrap);
			}
		}

		if(list_players[iPlayer]->teamID == iTeamID || list_players[iPlayer]->state != player_ready)
			continue;

		//If there is a chicken, only focus on stomping him
		if(game_values.gamemode->chicken)
		{
			if(game_values.gamemode->chicken->teamID != iTeamID && game_values.gamemode->chicken != list_players[iPlayer])
				continue;
		}

		DistanceToPlayer(list_players[iPlayer], &nearestObjects.player, &nearestObjects.playerdistance, &nearestObjects.playerwrap);
	}
}

void CPlayerAI::DistanceToObject(CObject * object, CObject ** target, int * nearest, bool * wrap)
{
	//Calculate normal screen
	short tx = object->ix - pPlayer->ix;
	short ty = object->iy - pPlayer->iy;
	bool fScreenWrap = false;
	
	//See if it is a shorter distance wrapping around the screen
	if(tx > 320)
	{
		tx = 640 - tx;
		fScreenWrap = true;
	}
	else if(tx < -320)
	{
		tx = 640 + tx;
		fScreenWrap = true;
	}

	int distance_player_pow2 = tx*tx + ty*ty;    // a^2 = b^2 + c^2 :)

	if (distance_player_pow2 < *nearest)
	{
		*target = object;
		*nearest = distance_player_pow2;
		*wrap = fScreenWrap;
	}
}

void CPlayerAI::DistanceToObjectCenter(CObject * object, CObject ** target, int * nearest, bool * wrap)
{
	//Calculate normal screen
	short tx = object->ix + (object->collisionWidth >> 1) - pPlayer->ix - HALFPW;
	short ty = object->iy + (object->collisionHeight >> 1) - pPlayer->iy - HALFPH;
	bool fScreenWrap = false;
	
	if(tx > 320)
	{
		tx = 640 - tx;
		fScreenWrap = true;
	}
	else if(tx < -320)
	{
		tx = 640 + tx;
		fScreenWrap = true;
	}

	int distance_player_pow2 = tx*tx + ty*ty;

	if (distance_player_pow2 < *nearest)
	{
		*target = object;
		*nearest = distance_player_pow2;
		*wrap = fScreenWrap;
	}
}


void CPlayerAI::DistanceToPlayer(CPlayer * player, CPlayer ** target, int * nearest, bool * wrap)
{
	//Calculate normal screen
	short tx = player->ix - pPlayer->ix;
	short ty = player->iy - pPlayer->iy;
	bool fScreenWrap = false;
	
	if(tx > 320)
	{
		tx = 640 - tx;
		fScreenWrap = true;
	}
	else if(tx < -320)
	{
		tx = 640 + tx;
		fScreenWrap = true;
	}

	int distance_player_pow2 = tx*tx + ty*ty;

	if (distance_player_pow2 < *nearest)
	{
		*target = player;
		*nearest = distance_player_pow2;
		*wrap = fScreenWrap;
	}
}

/**************************************************
* CSimpleAI class
***************************************************/
//This simple ai makes the player jump every so often
//The jump is to the maximum jump height, then the game_jump.fDown is released
void CSimpleAI::Think(COutputControl * playerKeys)
{
	playerKeys->game_left.fDown = false;
	playerKeys->game_right.fDown = false;
	
	playerKeys->game_down.fDown = false;
	playerKeys->game_turbo.fDown = false;
	playerKeys->game_powerup.fDown = false;

	if(pPlayer->isdead() || pPlayer->isspawning())
		return;

	//Hold down jump until player starts moving down again, then release jump button
	if(pPlayer->inair)
	{
		if(pPlayer->vely > 0.0f)
			playerKeys->game_jump.fDown = false;
		else
			playerKeys->game_jump.fDown = true;
	}
	else
	{
		//Try to jump 1 out of 50 chances when on ground
		if(rand() % 50 == 0)
			playerKeys->game_jump.fDown = true;
		else
			playerKeys->game_jump.fDown = false;
	}
}
