#include "global.h"
extern	gfxFont			font[3];

#include <string.h>

extern short CountAliveTeams(short * lastteam);

void RemovePlayersButTeam(short teamid)
{
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(list_players[iPlayer]->teamID != teamid)
		{
			list_players[iPlayer]->state = player_dead;
		}
	}
}

void RemovePlayersButHighestScoring()
{
	short iMaxScore = -1;

	//Figure out what the maximum score is
	for(short iScore = 0; iScore < score_cnt; iScore++)
	{
		if(score[iScore]->score > iMaxScore)
			iMaxScore = score[iScore]->score;
	}

	//Remove all players that don't have that max score
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(list_players[iPlayer]->score->score < iMaxScore)
		{
			list_players[iPlayer]->state = player_dead;
		}
	}
}

void SetupScoreBoard(bool fOrderMatters)
{
	short i, j;
	
	bool draw[4] = {false, false, false, false};

	short oldmax = -1;
	short max = -1;
	for(i = 0; i < score_cnt; i++)
	{
		oldmax = max;
		max = -1;

		for(j = 0; j < score_cnt; j++)
		{
			if(!draw[j])
			{	
				//If this player's score is bigger
				if(max == -1 || score[j]->score > score[max]->score)
				{
					max = j;
				}
				else if(score[j]->score == score[max]->score && score[j]->order > score[max]->order) //or it is tied but they died later in the game
				{
					max = j;
				}
			}
		}

		score[max]->displayorder = i;
		score[max]->place = i;

		if(!fOrderMatters && i > 0)
		{
			if(score[oldmax]->score == score[max]->score)
				score[max]->place = score[oldmax]->place;
		}

		draw[max] = true;	//this is the next biggest score - it doesn't belong to the remaining scores from now on
	}
}

void ShowScoreBoard()
{
	game_values.showscoreboard = true;

	for(short i = 0; i < score_cnt; i++)
	{
		score[i]->destx = 309 - 34 * game_values.teamcounts[i];
		score[i]->desty = score[i]->displayorder * 45 + 150;
	}

	if(game_values.music)
	{
		ifsoundonstop(sfx_invinciblemusic);
		ifsoundonstop(sfx_timewarning);
		ifsoundonstop(sfx_slowdownmusic);
		ifsoundonstop(sfx_bowserlaugh);
		
		backgroundmusic[1].play(true, false);
	}
}

void RemoveTeam(short teamid)
{
	//If we have already removed this team then return
	if(score[teamid]->order > -1)
		return;

	//kill all players on the dead team
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(list_players[iPlayer]->teamID == teamid)
		{
			list_players[iPlayer]->state = player_dead;
		}
	}

	score[teamid]->order = game_values.teamdeadcounter++;
}

CGameMode::CGameMode()
{
	winningteam = -1;
	gamemode = game_mode_frag;
	SetupModeStrings("Free Play", "Frags", 5);
	fReverseScoring = false;
}

CGameMode::~CGameMode()
{}

void CGameMode::init()  //called once when the game is started
{
	if(goal == 1000)
		goal = 999; //Cap goal for 3 digit scoreboard

	chicken = NULL; 
	tagged = NULL;
	star = NULL;
	frenzyowner = NULL;

	winningteam = -1; 
	gameover = false; 
	playedwarningsound = false;

	for(short iScore = 0; iScore < score_cnt; iScore++)
	{
		score[iScore]->SetScore(0);
	}
};

void CGameMode::displayplayertext()
{
	if(winningteam > -1)
	{
		for(short k = 0; k < list_players_cnt; k++)
		{
			if(list_players[k]->teamID == winningteam)
				list_players[k]->spawnText("Winner!");
		}
	}
}

void CGameMode::playwarningsound()
{
	playedwarningsound = true;
	ifsoundonstop(sfx_invinciblemusic);
	ifsoundonstop(sfx_bowserlaugh);

	if(game_values.music && game_values.sound)
		backgroundmusic[0].stop();

	ifsoundonplay(sfx_timewarning);
}

void CGameMode::SetupModeStrings(char * szMode, char * szGoal, short iGoalSpacing)
{
	strcpy(szModeName, szMode);
	strcpy(szGoalName, szGoal);
	
	for(short iMode = 0; iMode < GAMEMODE_NUM_OPTIONS; iMode++)
	{
		if(iMode == GAMEMODE_NUM_OPTIONS - 1)
		{
			modeOptions[iMode].iValue = -1;
			strcpy(modeOptions[iMode].szName, "Unlimited");
		}
		else
		{
			modeOptions[iMode].iValue = (iMode + 1) * iGoalSpacing;
			sprintf(modeOptions[iMode].szName, "%d", modeOptions[iMode].iValue);
		}
	}
}

bool CGameMode::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	//Penalize killing your team mates
	if(!gameover)
	{
		if(inflictor.teamID == other.teamID)
			inflictor.score->AdjustScore(-1);
		else
			inflictor.score->AdjustScore(1);
	}

	return false;
};

bool CGameMode::playerkilledself(CPlayer &player)
{
	if(player.bobomb)
		player.diedas = 2; //flag to use bobomb corpse sprite

	return false;
}

void CGameMode::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
		player.score->AdjustScore(iType);
}

CPlayer * CGameMode::GetHighestScorePlayer(bool fGetHighest)
{
	short i, j;
	short count = 1;
	short tiedplayers[4];
	tiedplayers[0] = 0;

	for(j = 0; j < list_players_cnt; j++)
	{
		if(!list_players[j]->isdead())
		{
			count = 1;
			tiedplayers[0] = j;
			break;
		}
	}

	for(i = j + 1; i < list_players_cnt; i++)
	{
		if(!list_players[i]->isdead())
		{
			if((!fGetHighest && list_players[i]->score->score < list_players[tiedplayers[0]]->score->score) ||
				(fGetHighest && list_players[i]->score->score > list_players[tiedplayers[0]]->score->score))
			{
				count = 1;
				tiedplayers[0] = i;
			}
			else if(list_players[i]->score->score == list_players[tiedplayers[0]]->score->score)
			{
				tiedplayers[count] = i;
				count++;
			}
		}
	}

	return list_players[tiedplayers[rand() % count]];
}


short CGameMode::GetClosestGoal(short iGoal)
{
	short iDifference = 16000;
	short iOptionValue = 0;

	for(short iOption = 0; iOption < GAMEMODE_NUM_OPTIONS - 1; iOption++)
	{
		short iDiff = abs(modeOptions[iOption].iValue - iGoal);
		if(iDiff < iDifference)
		{
			iOptionValue = modeOptions[iOption].iValue;
			iDifference = iDiff;
		}
	}

	return iOptionValue;
}

//fraglimit:

CGM_Frag::CGM_Frag() : CGameMode()
{
	goal = 20;
	gamemode = game_mode_frag;

	SetupModeStrings("Frag Limit", "Kills", 5);
};


void CGM_Frag::think()
{
	if(gameover)
		displayplayertext();
}


bool CGM_Frag::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	if(!gameover)
	{
		//Penalize killing your team mates
		if(inflictor.teamID == other.teamID)
			inflictor.score->AdjustScore(-1);
		else
			inflictor.score->AdjustScore(1);

		return CheckWinner(inflictor);
	}

	return false;
}

bool CGM_Frag::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	if(!gameover)
		player.score->AdjustScore(-1);

	return false;
}

void CGM_Frag::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(iType);
		CheckWinner(player);
	}
}

bool CGM_Frag::CheckWinner(CPlayer &player)
{
	if(goal == -1)
		return false;

	if(player.score->score >= goal)
	{
		winningteam = player.teamID;
		gameover = true;

		RemovePlayersButTeam(winningteam);
		SetupScoreBoard(false);
		ShowScoreBoard();

		return true;
	}
	else if(player.score->score >= goal - 2 && !playedwarningsound)
	{
		playwarningsound();
	}

	return false;
}


//timelimit
CGM_TimeLimit::CGM_TimeLimit() : CGameMode()
{
	goal = 60;
	gamemode = game_mode_timelimit;

	SetupModeStrings("Time Limit", "Time", 30);

	iFramesPerSecond = 1000 / WAITTIME;
};

void CGM_TimeLimit::init()
{
	CGameMode::init();

	if(goal == -1)
		timeleft = 0;
	else
		timeleft = goal;

	if(game_values.scoreboardstyle == 0)
		iScoreOffsetX = 5;
	else
		iScoreOffsetX = 291;

	SetDigitCounters();
	framesleft_persecond = iFramesPerSecond;
}


void CGM_TimeLimit::think()
{
	if(gameover)
		displayplayertext();

	if(timeleft > 0 || goal == -1)
	{
		if(--framesleft_persecond < 1)
		{
			framesleft_persecond = iFramesPerSecond;

			if(goal == -1)
			{
				timeleft++;
			}
			else
			{
				if(--timeleft == 20 && !playedwarningsound)
				{
					playwarningsound();
				}
			}

			SetDigitCounters();
		}
	}
	else if(timeleft == 0 && goal > 0)
	{		//the game ends
		SetupScoreBoard(false);
		ShowScoreBoard();
		timeleft--;

		RemovePlayersButHighestScoring();
		gameover = true;

		CountAliveTeams(&winningteam);
	}
}


bool CGM_TimeLimit::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	if(!gameover)
	{
		//Penalize killing your team mates
		if(inflictor.teamID == other.teamID)
			inflictor.score->AdjustScore(-1);
		else
			inflictor.score->AdjustScore(1);
	}

	return false;
}

bool CGM_TimeLimit::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	if(player.score->score > 0 && !gameover)
		player.score->AdjustScore(-1);

	return false;
}

void CGM_TimeLimit::draw()
{
	drawtime();
}

void CGM_TimeLimit::drawtime()
{
	spr_timershade.draw(iScoreOffsetX, 5);
	spr_scoretext.draw(iDigitRightDstX, 13, iDigitRightSrcX, 0, 16, 16);
	
	if(iDigitLeftSrcX > 0)
	{
		spr_scoretext.draw(iDigitMiddleDstX, 13, iDigitMiddleSrcX, 0, 16, 16);
		spr_scoretext.draw(iDigitLeftDstX, 13, iDigitLeftSrcX, 0, 16, 16);
	}
	else if(iDigitMiddleSrcX > 0)
	{
		spr_scoretext.draw(iDigitMiddleDstX, 13, iDigitMiddleSrcX, 0, 16, 16);
	}
}

void CGM_TimeLimit::SetDigitCounters()
{
	short iDigits = timeleft;
	while(iDigits > 999)
		iDigits -= 1000;
		
	iDigitLeftSrcX = iDigits / 100 * 16;
	iDigitMiddleSrcX = iDigits % 100 / 10 * 16;
	iDigitRightSrcX = iDigits % 10 * 16;

	if(iDigitLeftSrcX == 0)
	{
		if(iDigitMiddleSrcX == 0)
		{
			iDigitRightDstX = iScoreOffsetX + 21;
		}
		else
		{
			iDigitMiddleDstX = iScoreOffsetX + 12;
			iDigitRightDstX = iScoreOffsetX + 30;
		}
	}
	else
	{
		iDigitLeftDstX = iScoreOffsetX + 3;
		iDigitMiddleDstX = iScoreOffsetX + 21;
		iDigitRightDstX = iScoreOffsetX + 39;
	}
}

//mariowar (x lives - counting down)
CGM_Classic::CGM_Classic() : CGameMode()
{
	goal = 10;
	gamemode = game_mode_classic;

	SetupModeStrings("Classic", "Lives", 5);
};

void CGM_Classic::init()
{
	CGameMode::init();

	fReverseScoring = goal == -1;

	for(short iScore = 0; iScore < score_cnt; iScore++)
	{
		if(fReverseScoring)
			score[iScore]->SetScore(0);
		else
			score[iScore]->SetScore(goal);
	}
}


void CGM_Classic::think()
{
	if(gameover)
		displayplayertext();
}


bool CGM_Classic::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	if(!gameover)
	{
		if(fReverseScoring)
		{
			other.score->AdjustScore(1);
		}
		else
		{
			other.score->AdjustScore(-1);

			if(!playedwarningsound)
			{
				short countscore = 0;
				for(short k = 0; k < score_cnt; k++)
				{
					if(inflictor.score == score[k])
						continue;

					countscore += score[k]->score;
				}

				if(countscore <= 2)
				{
					playwarningsound();
				}
			}

			if(other.score->score <= 0)
			{
				RemoveTeam(other.teamID);
				return true;
			}
		}
	}

	return false;
}

bool CGM_Classic::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	if(!gameover)
	{
		if(fReverseScoring)
		{
			player.score->AdjustScore(1);
		}
		else
		{
			player.score->AdjustScore(-1);

			if(!playedwarningsound)
			{
				short countscore = 0;
				bool playwarning = false;
				for(short j = 0; j < score_cnt; j++)
				{
					for(short k = 0; k < score_cnt; k++)
					{
						if(j == k)
							continue;

						countscore += score[k]->score;
					}

					if(countscore <= 2)
					{
						playwarning = true;
						break;
					}

					countscore = 0;
				}

				if(playwarning)
					playwarningsound();
			}

			if(player.score->score <= 0)
			{
				RemoveTeam(player.teamID);
				return true;
			}
		}
	}
	return false;
}

void CGM_Classic::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		if(fReverseScoring)
			player.score->AdjustScore(-iType);
		else
        	player.score->AdjustScore(iType);
	}
}


//capture the chicken
//one player is the chicken
//if he is stomped the attacker becomes the chicken.
//get points for being the chicken
CGM_Chicken::CGM_Chicken() : CGameMode()
{
	goal = 200;
	gamemode = game_mode_chicken;

	SetupModeStrings("Chicken", "Points", 50);
}

void CGM_Chicken::think()
{
	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		if(chicken)
		{
			if( chicken->velx > VELMOVING_CHICKEN )
				chicken->velx = VELMOVING_CHICKEN;
			else if(chicken->velx < -VELMOVING_CHICKEN)
				chicken->velx = -VELMOVING_CHICKEN;

			static short counter = 0;

			if(chicken->isready() && chicken->statue_timer == 0)
			{
				if(++counter >= game_values.pointspeed)
				{
					counter = 0;
					chicken->score->AdjustScore(1);
				}
			}

			CheckWinner(*chicken);
		}	
	}
}

void CGM_Chicken::draw()
{
	//Draw the chicken indicator around the chicken
	if(game_values.gamemodesettings.chicken.usetarget && !gameover && chicken)
	{
		if(chicken->iswarping())
			spr_chicken.draw(chicken->ix - PWOFFSET - 16, chicken->iy - PHOFFSET - 16, 0, 0, 64, 64, (short)chicken->state % 4, chicken->warpplane);
		else if(chicken->isready())
			spr_chicken.draw(chicken->ix + HALFPW - 32, chicken->iy + HALFPH - 32);
	}
}


bool CGM_Chicken::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	if(!chicken)
	{
		chicken = &inflictor;
		eyecandyfront.add(new EC_GravText(&game_font_large, inflictor.ix + (HALFPW), inflictor.iy + PH, "Chicken!", -VELJUMP*1.5));
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, inflictor.ix + (HALFPW) - 16, inflictor.iy + (HALFPH) - 16, 3, 8));
		ifsoundonplay(sfx_transform);
	}
	else
	{
		if(&other == chicken)
		{
			other.diedas = 1; //flag to use chicken corpse sprite
			chicken = &inflictor;
			eyecandyfront.add(new EC_GravText(&game_font_large, inflictor.ix + (HALFPW), inflictor.iy + PH, "Chicken!", -VELJUMP*1.5));
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, inflictor.ix + (HALFPW) - 16, inflictor.iy + (HALFPH) - 16, 3, 8));
			ifsoundonplay(sfx_transform);
		}
		else if(&inflictor == chicken)
		{
			if(!gameover)
			{
				inflictor.score->AdjustScore(5);
				return CheckWinner(inflictor);
			}
		}
	}
	
	return false;
}

bool CGM_Chicken::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	if(chicken == &player)
	{
		player.diedas = 1; //flag to use chocobo corpse sprite

		if(!gameover)
			chicken = NULL;
	}

	return false;
}

void CGM_Chicken::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(10 * iType);
		CheckWinner(player);
	}
}

bool CGM_Chicken::CheckWinner(CPlayer &player)
{
	if(goal == -1)
		return false;

	if(player.score->score >= goal)
	{
		player.score->SetScore(goal);
		winningteam = player.teamID;
		gameover = true;

		SetupScoreBoard(false);
		ShowScoreBoard();
		RemovePlayersButTeam(winningteam);
		return true;
	}
	else if(player.score->score >= goal * 0.8 && !playedwarningsound)
	{
		playwarningsound();
	}

	return false;
}


//tag mode (leper mode suggestion from ziotok)
//one player is "it"
//if he killes another player, they become "it"
//the player that is "it" loses life until dead.
//the "it" player is chosen at random.  Someone is
//always "it".
CGM_Tag::CGM_Tag() : CGameMode()
{
	goal = 200;
	gamemode = game_mode_tag;

	SetupModeStrings("Tag", "Points", 50);
}

void CGM_Tag::init()
{
	CGameMode::init();
	fReverseScoring = goal == -1;

	for(short iScore = 0; iScore < score_cnt; iScore++)
	{
		if(fReverseScoring)
			score[iScore]->SetScore(0);
		else
			score[iScore]->SetScore(goal);
	}
}

void CGM_Tag::think()
{
	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		if(!tagged)
		{
			tagged = GetHighestScorePlayer(!fReverseScoring);
		}

		static short counter = 0;

		if(tagged->isready())
		{
			if(++counter >= game_values.pointspeed)
			{
				counter = 0;

				if(fReverseScoring)
					tagged->score->AdjustScore(1);
				else
					tagged->score->AdjustScore(-1);
			}
		}

		if(fReverseScoring)
			return;
		
		short countscore = 0;
		bool playwarning = false;
		for(short j = 0; j < score_cnt; j++)
		{
			for(short k = 0; k < score_cnt; k++)
			{
				if(j == k)
					continue;

				countscore += score[k]->score;
			}

			if(countscore <= goal * 0.2)
			{
				playwarning = true;
				break;
			}

			countscore = 0;
		}

		if(playwarning && !playedwarningsound)
		{
			playwarningsound();
		}

		if(tagged->score->score <= 0)
		{
			RemoveTeam(tagged->teamID);
			tagged = NULL;
		}
	}
}

bool CGM_Tag::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	if(&inflictor == tagged)
	{
		tagged = &other;
		inflictor.spawninvincible = true;
		inflictor.spawninvincibletimer = 60;
		eyecandyfront.add(new EC_GravText(&game_font_large, other.ix + (HALFPW), other.iy + PH, "Tagged!", -VELJUMP*1.5));
		eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, other.ix + (HALFPW) - 16, other.iy + (HALFPH) - 16, 3, 8));
		ifsoundonplay(sfx_transform);
	}

	if(!gameover)
	{
		if(fReverseScoring)
		{
			other.score->AdjustScore(5);
			return false;
		}
		else
		{
			other.score->AdjustScore(-5);
		}

		short countscore = 0;
		for(short k = 0; k < score_cnt; k++)
		{
			if(inflictor.score == score[k])
				continue;

			countscore += score[k]->score;
		}

		if(countscore <= goal * 0.2 && !playedwarningsound)
		{
			playwarningsound();
		}

		if(other.score->score <= 0)
		{
			other.score->SetScore(0);
			
			RemoveTeam(other.teamID);
			return true;
		}
	}

	return false;
}

bool CGM_Tag::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	if(!gameover)
	{
		if(fReverseScoring)
		{
			player.score->AdjustScore(5);
			return false;
		}
		else
		{
			player.score->AdjustScore(-5);
		}

		short countscore = 0;
		bool playwarning = false;
		for(short j = 0; j < score_cnt; j++)
		{
			for(short k = 0; k < score_cnt; k++)
			{
				if(j == k)
					continue;

				countscore += score[k]->score;
			}

			if(countscore <= goal * 0.2)
			{
				playwarning = true;
				break;
			}

			countscore = 0;
		}

		if(playwarning && !playedwarningsound)
		{
			playwarningsound();
		}

		if(player.score->score <= 0)
		{
			player.score->SetScore(0);
			RemoveTeam(player.teamID);
			return true;
		}
	}
	
	return false;
}

void CGM_Tag::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		if(fReverseScoring)
			player.score->AdjustScore(-10 * iType);
		else
			player.score->AdjustScore(10 * iType);
	}
}


//Coin mode:
//Collect randomly appearing coins on map
//First one to set amount wins
CGM_Coins::CGM_Coins() : CGameMode()
{
	goal = 20;
	gamemode = game_mode_coins;

	SetupModeStrings("Coin Collection", "Coins", 5);
};

void CGM_Coins::init()
{
	CGameMode::init();

	for(short iCoin = 0; iCoin < game_values.gamemodesettings.coins.quantity; iCoin++)
		objectsplayer.add(new MO_Coin(&spr_coin, 4, 8));
}


void CGM_Coins::think()
{
	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		if(goal > -1)
		{
			for(short i = 0; i < list_players_cnt; i++)
			{
				if(list_players[i]->score->score >= goal)
				{
					winningteam = list_players[i]->teamID;
					gameover = true;

					RemovePlayersButTeam(winningteam);
					SetupScoreBoard(false);
					ShowScoreBoard();
				}
				else if(list_players[i]->score->score >= goal - 2 && !playedwarningsound)
				{
					playwarningsound();
				}
			}
		}
	}
}


bool CGM_Coins::playerkilledplayer(CPlayer &player, CPlayer &other)
{
	if(game_values.gamemodesettings.coins.penalty)
		other.score->AdjustScore(-1);

	return false;
}

bool CGM_Coins::playerkilledself(CPlayer &player)
{
	if(game_values.gamemodesettings.coins.penalty)
		player.score->AdjustScore(-1);

	return false;
}

void CGM_Coins::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(iType);

		if(goal > -1)
		{
			if(player.score->score >= goal)
			{
				winningteam = player.teamID;
				gameover = true;

				RemovePlayersButTeam(winningteam);
				SetupScoreBoard(false);
				ShowScoreBoard();
			}
			else if(player.score->score >= goal - 2 && !playedwarningsound)
			{
				playwarningsound();
			}
		}
	}
}


//Egg mode:
//Grab the egg and return it to Yoshi
//Score 1 point for each
CGM_Eggs::CGM_Eggs() : CGameMode()
{
	goal = 20;
	gamemode = game_mode_eggs;

	SetupModeStrings("Yoshi's Eggs", "Eggs", 5);
};

void CGM_Eggs::init()
{
	CGameMode::init();

	objectsplayer.add(new OMO_Yoshi(&spr_yoshi));
	objectsplayer.add(new CO_Egg(&spr_egg));
}


void CGM_Eggs::think()
{
	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		if(goal > -1)
		{
			for(short i = 0; i < list_players_cnt; i++)
			{
				if(list_players[i]->score->score >= goal)
				{
					winningteam = list_players[i]->teamID;
					gameover = true;

					RemovePlayersButTeam(winningteam);
					SetupScoreBoard(false);
					ShowScoreBoard();
				}
				else if(list_players[i]->score->score >= goal - 2 && !playedwarningsound)
				{
					playwarningsound();
				}
			}
		}
	}
}


bool CGM_Eggs::playerkilledplayer(CPlayer &, CPlayer &)
{
	return false;
}

bool CGM_Eggs::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	if(player.carriedItem && player.carriedItem->getObjectType() == object_egg)
	{
		((CO_Egg*)player.carriedItem)->placeEgg();
	}

	return false;
}

void CGM_Eggs::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(iType);

		if(goal > -1)
		{
			if(player.score->score >= goal)
			{
				winningteam = player.teamID;
				gameover = true;

				RemovePlayersButTeam(winningteam);
				SetupScoreBoard(false);
				ShowScoreBoard();
			}
			else if(player.score->score >= goal - 2 && !playedwarningsound)
			{
				playwarningsound();
			}
		}
	}
}


//Fireball:
//Frag limit death match, but firepower cards appear randomly
CGM_Frenzy::CGM_Frenzy() : CGM_Frag()
{
	gamemode = game_mode_frenzy;
	strcpy(szModeName, "Frenzy");
}

void CGM_Frenzy::init()
{
	CGameMode::init();
	timer = 0;

	iItemWeightCount = 0;
	for(short iPowerup = 0; iPowerup < 12; iPowerup++)
		iItemWeightCount += game_values.gamemodesettings.frenzy.powerupweight[iPowerup];

	if(iItemWeightCount == 0)
		iItemWeightCount = 1;
}


void CGM_Frenzy::think()
{
	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		short iPowerupQuantity = game_values.gamemodesettings.frenzy.quantity;

		if((iPowerupQuantity != 0 && ++timer >= game_values.gamemodesettings.frenzy.rate) || (iPowerupQuantity == 0 && !frenzyowner))
		{
			timer = 0;

			if(0 == iPowerupQuantity)
				iPowerupQuantity = 1;
			if(5 < iPowerupQuantity)
				iPowerupQuantity = list_players_cnt + iPowerupQuantity - 7;

			if(objectsplayer.countTypes(object_frenzycard) < iPowerupQuantity)
			{
				//Randomly choose a powerup from the weighted list
				int iRandPowerup = rand() % iItemWeightCount + 1;
				iSelectedPowerup = 0;
				int iWeightCount = game_values.gamemodesettings.frenzy.powerupweight[iSelectedPowerup];

				while(iWeightCount < iRandPowerup)
					iWeightCount += game_values.gamemodesettings.frenzy.powerupweight[++iSelectedPowerup];

				objectsplayer.add(new MO_FrenzyCard(&spr_frenzycards, 12, 8, iSelectedPowerup));
			}
		}
	}

	if(frenzyowner)
	{
		if(0 == iSelectedPowerup)
		{
			if(!frenzyowner->bobomb)
				frenzyowner = NULL;
		}
		else if(5 > iSelectedPowerup)
		{
			if(frenzyowner->powerup != iSelectedPowerup)
				frenzyowner = NULL;
		}
		else if(5 == iSelectedPowerup)
		{
			if(game_values.gamepowerups[frenzyowner->globalID] != 9)
				frenzyowner = NULL;
		}
		else if(6 == iSelectedPowerup)
		{
			if(game_values.gamepowerups[frenzyowner->globalID] != 16)
				frenzyowner = NULL;
		}
		else if(7 == iSelectedPowerup)
		{
			if(game_values.gamepowerups[frenzyowner->globalID] != 10)
				frenzyowner = NULL;
		}
	}
}


//Thwomp Mode! - just like mario war classic, but you have
// to dodge thwomps from the sky.  Idea from ziotok.
CGM_Survival::CGM_Survival() : CGM_Classic()
{
	goal = 20;
	gamemode = game_mode_survival;

	strcpy(szModeName, "Survival");
}

void CGM_Survival::init()
{
	CGM_Classic::init();

	rate = 3 * game_values.gamemodesettings.survival.density;
	timer = (short)(rand() % 21 - 10 + rate);
	ratetimer = 0;

	iEnemyWeightCount = 0;
	for(short iEnemy = 0; iEnemy < 3; iEnemy++)
		iEnemyWeightCount += game_values.gamemodesettings.survival.enemyweight[iEnemy];

	if(iEnemyWeightCount == 0)
		iEnemyWeightCount = 1;
}

void CGM_Survival::think()
{
	if(gameover) 
	{
		displayplayertext();
	}
	else
	{
		if(--timer <= 0)
		{
			if(++ratetimer == 10)
			{
				ratetimer = 0;
				
				if(--rate < game_values.gamemodesettings.survival.density)
					rate = game_values.gamemodesettings.survival.density;
			}

			//Randomly choose an enemy from the weighted list
			int iRandEnemy = rand() % iEnemyWeightCount + 1;
			iSelectedEnemy = 0;
			int iWeightCount = game_values.gamemodesettings.survival.enemyweight[iSelectedEnemy];

			while(iWeightCount < iRandEnemy)
				iWeightCount += game_values.gamemodesettings.survival.enemyweight[++iSelectedEnemy];

			if(0 == iSelectedEnemy)
			{
				objectsfront.add(new OMO_Thwomp(&spr_thwomp, (short)(rand() % 591), (float)game_values.gamemodesettings.survival.speed / 2.0f + (float)(rand()%20)/10.0f));
				timer = (short)(rand() % 21 - 10 + rate);
			}
			else if(1 == iSelectedEnemy)
			{
				objectsfront.add(new OMO_Podobo(&spr_podobo, (short)(rand() % 608), -(float(rand() % 9) / 2.0f) - 8.0f, -1, -1, -1));
				timer = (short)(rand() % 21 - 10 + rate - 20);
			}
			else
			{
				float dSpeed = ((float)(rand() % 21 + 20)) / 10.0f;
				float dVel = rand() % 2 ? dSpeed : -dSpeed;

				short x = -54;
				if(dVel < 0)
					x = 694;

				objectsfront.add(new OMO_BowserFire(&spr_bowserfire, x, (short)(rand() % 448), dVel, 0.0f, -1, -1, -1));
				timer = (short)(rand() % 21 - 10 + rate);
			}
		}
	}
}


//Domination (capture the area blocks)
//Touch all the dotted blocks to turn them your color
//The more blocks you have, the faster you rack up points

CGM_Domination::CGM_Domination() : CGameMode()
{
	goal = 200;
	gamemode = game_mode_domination;

	SetupModeStrings("Domination", "Points", 50);
}

void CGM_Domination::init()
{
	CGameMode::init();
	
	short iNumAreas = game_values.gamemodesettings.domination.quantity;

	if(iNumAreas > 18)
		iNumAreas = 2 * list_players_cnt + iNumAreas - 22;
	else if(iNumAreas > 10)
		iNumAreas = list_players_cnt + iNumAreas - 12;

	for(short k = 0; k < iNumAreas; k++)
		objectcollisionitems.add(new OMO_Area(&spr_areas, iNumAreas));
}

void CGM_Domination::think()
{
	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		if(goal > -1)
		{
			for(short i = 0; i < list_players_cnt; i++)
			{
				if(list_players[i]->score->score >= goal)
				{
					winningteam = list_players[i]->teamID;
					gameover = true;

					RemovePlayersButTeam(winningteam);
					SetupScoreBoard(false);
					ShowScoreBoard();
				}
				else if(list_players[i]->score->score >= goal * 0.8 && !playedwarningsound)
				{
					playwarningsound();
				}
			}
		}
	}
}

bool CGM_Domination::playerkilledplayer(CPlayer &player, CPlayer &other)
{
	//Update areas the dead player owned
	objectcollisionitems.adjustPlayerAreas(&player, &other);
	
	return false;
}

bool CGM_Domination::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	//Update areas the dead player owned
	objectcollisionitems.adjustPlayerAreas(NULL, &player);

	return false;
}

void CGM_Domination::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(10 * iType);

		if(goal > -1)
		{
			if(player.score->score >= goal)
			{
				player.score->SetScore(goal);
				winningteam = player.teamID;
				gameover = true;

				SetupScoreBoard(false);
				ShowScoreBoard();
				RemovePlayersButTeam(winningteam);
			}
			else if(player.score->score >= goal * 0.8 && !playedwarningsound)
			{
				playwarningsound();
			}
		}
	}
}


//Owned:
//Players rack up points like domination for each player they have "owned"
CGM_Owned::CGM_Owned() : CGameMode()
{
	goal = 200;
	gamemode = game_mode_owned;

	SetupModeStrings("Owned", "Points", 50);
}

void CGM_Owned::think()
{
	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		static short counter = 0;

		if(++counter >= game_values.pointspeed)
		{
			counter = 0;

			for(short i = 0; i < list_players_cnt; i++)
			{
				for(short k = 0; k < list_players_cnt; k++)
				{
					if(i == k)
						continue;

					if(list_players[k]->ownerPlayerID == list_players[i]->globalID)
						list_players[i]->score->AdjustScore(1);
				}
				
				if(goal > -1)
				{
					if(list_players[i]->score->score >= goal)
					{
						list_players[i]->score->SetScore(goal);

						winningteam = list_players[i]->teamID;
						gameover = true;

						RemovePlayersButTeam(winningteam);
						SetupScoreBoard(false);
						ShowScoreBoard();
					}
					else if(list_players[i]->score->score >= goal * 0.8 && !playedwarningsound)
					{
						playwarningsound();
					}
				}
			}
		}
	}
}

bool CGM_Owned::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	if(!gameover)
	{
		//Give a bonus to the killer if he already owned this player
        if(other.ownerPlayerID == inflictor.globalID)	
			inflictor.score->AdjustScore(5);

		//Release all players owned by the killed player
		for(short i = 0; i < list_players_cnt; i++)
		{
			if(list_players[i]->ownerPlayerID == other.globalID)
			{
				list_players[i]->ownerPlayerID = -1;
			}
		}

		//Assign owned status to the killed player
		if(other.teamID != inflictor.teamID)
		{
			other.ownerPlayerID = inflictor.globalID;
			other.ownerColorOffsetX = inflictor.colorID * 48;
		}
		
		return CheckWinner(inflictor);
	}

	return false;
}

bool CGM_Owned::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	for(short i = 0; i < list_players_cnt; i++)
	{
		if(list_players[i]->ownerPlayerID == player.globalID)
		{
			list_players[i]->ownerPlayerID = -1;
		}
	}
	
	return false;
}

void CGM_Owned::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(10 * iType);
		CheckWinner(player);
	}
}

bool CGM_Owned::CheckWinner(CPlayer &player)
{
	if(goal == -1)
		return false;
	
	if(player.score->score >= goal)
	{
		player.score->SetScore(goal);
		winningteam = player.teamID;
		gameover = true;

		SetupScoreBoard(false);
		ShowScoreBoard();
		RemovePlayersButTeam(winningteam);
		return true;
	}
	else if(player.score->score >= goal * 0.8 && !playedwarningsound)
	{
		playwarningsound();
	}

	return false;
}


//Owned:
//Frag limit death match, but players get bonus frags for the number of players they have "owned"
CGM_Jail::CGM_Jail() : CGM_Frag()
{
	gamemode = game_mode_jail;
	goal = 20;

	strcpy(szModeName, "Jail");
}

bool CGM_Jail::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	if(!gameover)
	{
		//Penalize killing your teammates
		if(inflictor.teamID == other.teamID)
		{
			inflictor.score->AdjustScore(-1);
		}
		else
		{
			inflictor.score->AdjustScore(1);

			if(inflictor.jailed > 0)
			{
				eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, inflictor.ix + (HALFPW) - 16, inflictor.iy + (HALFPH) - 16, 3, 8));
				ifsoundonplay(sfx_transform);
				inflictor.jailed = 0;
			}

			other.jailed = game_values.gamemodesettings.jail.timetofree;

			short jailedteams[4];
			
			short i;
			for(i = 0; i < score_cnt; i++)
				jailedteams[i] = game_values.teamcounts[i];

			//Figure out which teams have been jailed
			for(i = 0; i < list_players_cnt; i++)
			{
				if(list_players[i]->jailed > 0)
				{
					jailedteams[list_players[i]->teamID]--;
				}
			}

			//Determine if a single team is the only one not completely jailed
			short iTeamPoint = -1;
			for(i = 0; i < score_cnt; i++)
			{
				if(jailedteams[i] == 0)
					continue;

				if(iTeamPoint < 0)
				{
					iTeamPoint = i;
				}
				else
				{
					iTeamPoint = -1;
					break;
				}
			}

			if(iTeamPoint >= 0)
			{
				short numjailedplayers = 0;

				for(short i = 0; i < list_players_cnt; i++)
				{
					//If they weren't just the one killed and they were jailed, give them a transform cloud
					if(list_players[i] != &other && list_players[i]->jailed > 0)
					{
						eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, list_players[i]->ix + (HALFPW) - 16, list_players[i]->iy + (HALFPH) - 16, 3, 8));
						ifsoundonplay(sfx_transform);
					}

					if(list_players[i]->jailed > 0 && list_players[i]->teamID != inflictor.teamID)
						numjailedplayers++;
					
					list_players[i]->jailed = 0;
				}

				//Give extra bonus score for being on the non-jailed team
				if(numjailedplayers > 1)
					score[iTeamPoint]->AdjustScore(1);
			}
		}

		//Don't end the game if the goal is infinite
		if(goal == -1)
			return false;

		if(inflictor.score->score >= goal)
		{
			winningteam = inflictor.teamID;
			gameover = true;

			RemovePlayersButTeam(winningteam);

			SetupScoreBoard(false);
			ShowScoreBoard();

			return true;
		}
		else if(inflictor.score->score >= goal - 3 && !playedwarningsound)
		{
			playwarningsound();
		}
	}

	return false;
}

void CGM_Jail::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(iType);
		player.jailed = 0;

		//Don't end the game if the goal is infinite
		if(goal == -1)
			return;

		if(player.score->score >= goal)
		{
			player.score->SetScore(goal);
			winningteam = player.teamID;
			gameover = true;

			RemovePlayersButTeam(winningteam);
			SetupScoreBoard(false);
			ShowScoreBoard();
		}
		else if(player.score->score >= goal - 3 && !playedwarningsound)
		{
			playwarningsound();
		}
	}
}


//Stomp mode:
//Kill randomly appearing goomobas on map
//First one to kill the limit wins
CGM_Stomp::CGM_Stomp() : CGameMode() 
{
	gamemode = game_mode_stomp;
	SetupModeStrings("Stomp", "Kills", 10);
}

void CGM_Stomp::init()
{
	CGameMode::init();
	ResetSpawnTimer();

	iEnemyWeightCount = 0;
	for(short iEnemy = 0; iEnemy < 3; iEnemy++)
		iEnemyWeightCount += game_values.gamemodesettings.stomp.enemyweight[iEnemy];

	if(iEnemyWeightCount == 0)
		iEnemyWeightCount = 1;
}


void CGM_Stomp::think()
{
	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		for(short i = 0; i < list_players_cnt; i++)
		{
			CheckWinner(*list_players[i]);
		}
	}

	if(!gameover)
	{
		//Randomly spawn enemies
		if(--spawntimer <= 0)
		{
			ResetSpawnTimer();

			//Randomly choose an enemy from the weighted list
			int iRandEnemy = rand() % iEnemyWeightCount + 1;
			iSelectedEnemy = 0;
			int iWeightCount = game_values.gamemodesettings.stomp.enemyweight[iSelectedEnemy];

			while(iWeightCount < iRandEnemy)
				iWeightCount += game_values.gamemodesettings.stomp.enemyweight[++iSelectedEnemy];

			if(0 == iSelectedEnemy)
				objectcollisionitems.add(new MO_Goomba(&spr_goomba, 2, 8, rand() % 2 == 0, 30, 20, 1, 11));
			else if(1 == iSelectedEnemy)
				objectcollisionitems.add(new MO_Koopa(game_values.redkoopas ? &spr_redkoopa : &spr_koopa, 2, 8, rand() % 2 == 0, 30, 28, 1, 25, game_values.redkoopas));
			else
				objectsfront.add(new OMO_CheepCheep(&spr_cheepcheep));
		}
	}
}

bool CGM_Stomp::playerkilledplayer(CPlayer &player, CPlayer &other)
{
	return false;
}

bool CGM_Stomp::playerkilledself(CPlayer &player)
{
	return false;
}

void CGM_Stomp::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(iType);
		CheckWinner(player);
	}
}

void CGM_Stomp::ResetSpawnTimer()
{
	spawntimer = (short)(rand() % game_values.gamemodesettings.stomp.rate) + game_values.gamemodesettings.stomp.rate;
}

bool CGM_Stomp::CheckWinner(CPlayer &player)
{
	if(goal == -1)
		return false;

	if(player.score->score >= goal)
	{
		player.score->SetScore(goal);
		winningteam = player.teamID;
		gameover = true;

		SetupScoreBoard(false);
		ShowScoreBoard();
		RemovePlayersButTeam(winningteam);
		return true;
	}
	else if(player.score->score >= goal - 2 && !playedwarningsound)
	{
		playwarningsound();
	}

	return false;
}


//Race
//Touch all the flying blocks in order
//Each successful curcuit you complete (before getting killed)
//Counts as one point
CGM_Race::CGM_Race() : CGameMode()
{
	goal = 10;
	gamemode = game_mode_race;

	SetupModeStrings("Race", "Laps", 5);
}

void CGM_Race::init()
{
	CGameMode::init();
	
	for(short iRaceGoal = 0; iRaceGoal < game_values.gamemodesettings.race.quantity; iRaceGoal++)
		objectsfront.add(new OMO_RaceGoal(&spr_racegoal, iRaceGoal));

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
		nextGoal[iPlayer] = 0;
}

void CGM_Race::think()
{
	if(gameover)
		displayplayertext();
}

bool CGM_Race::playerkilledplayer(CPlayer &, CPlayer &other)
{
	PenalizeRaceGoals(other);
	return false;
}

bool CGM_Race::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);
	PenalizeRaceGoals(player);
	return false;
}

void CGM_Race::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		player.score->AdjustScore(1 + (iType == 5 ? 1 : 0));

		//Don't end the game if the goal is infinite
		if(goal == -1)
			return;

		if(player.score->score >= goal)
		{
			player.score->SetScore(goal);
			winningteam = player.teamID;
			gameover = true;

			RemovePlayersButTeam(winningteam);
			SetupScoreBoard(false);
			ShowScoreBoard();
		}
		else if(player.score->score >= goal - 2 && !playedwarningsound)
		{
			playwarningsound();
		}
	}
}


void CGM_Race::setNextGoal(short teamID) 
{
	if(++nextGoal[teamID] >= game_values.gamemodesettings.race.quantity)
	{
		nextGoal[teamID] = 0;
		objectsfront.removePlayerRaceGoals(teamID, -1);
		
		if(!gameover)
		{
			score[teamID]->AdjustScore(1);

			//Don't end the game if the goal is infinite
			if(goal == -1)
				return;
	
			if(score[teamID]->score >= goal)
			{
				score[teamID]->SetScore(goal);
				winningteam = teamID;
				gameover = true;
				
				RemovePlayersButTeam(winningteam);
				SetupScoreBoard(false);
				ShowScoreBoard();

			}
			else if(score[teamID]->score >= goal - 2 && !playedwarningsound)
			{
				playwarningsound();
			}
		}
	}
}

//Player loses control of his areas
void CGM_Race::PenalizeRaceGoals(CPlayer &player)
{
	objectsfront.removePlayerRaceGoals(player.teamID, nextGoal[player.teamID] - 1);

	if(2 == game_values.gamemodesettings.race.penalty)
		nextGoal[player.teamID] = 0;
	else if(1 == game_values.gamemodesettings.race.penalty)
		if(nextGoal[player.teamID] > 0)
			nextGoal[player.teamID]--;
}



//star
CGM_Star::CGM_Star() : CGM_TimeLimit()
{
	goal = 5;
	gamemode = game_mode_star;
	SetupModeStrings("Star", "Lives", 1);
};

void CGM_Star::init()
{
	CGM_TimeLimit::init();

	timeleft = game_values.gamemodesettings.star.time;
	SetDigitCounters();

	fReverseScoring = goal == -1;

	star = GetHighestScorePlayer(!fReverseScoring && !game_values.gamemodesettings.star.shine);
									
	starItem = new CO_Star(&spr_star);
	objectsplayer.add(starItem);

	for(short iScore = 0; iScore < score_cnt; iScore++)
	{
		if(fReverseScoring)
			score[iScore]->SetScore(0);
		else
			score[iScore]->SetScore(goal);
	}
}


void CGM_Star::think()
{
	if(gameover)
	{
		displayplayertext();
		return;
	}

	if(!star)
		star = GetHighestScorePlayer(!fReverseScoring && !game_values.gamemodesettings.star.shine);

	if(timeleft > 0)
	{
		if(--framesleft_persecond < 1)
		{
			timeleft--;
			framesleft_persecond = iFramesPerSecond;
			SetDigitCounters();

			if(timeleft <= 5 && timeleft > 0)
				ifsoundonplay(sfx_starwarning);
		}
	}
	
	if(timeleft == 0)
	{		//the game ends
		timeleft = game_values.gamemodesettings.star.time;
		SetDigitCounters();

		ifsoundonplay(sfx_thunder);

		if(game_values.gamemodesettings.star.shine)
		{
			for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
			{
				if(star->teamID == list_players[iPlayer]->teamID)
					continue;

				//Let the cleanup function remove the player on the last kill
				if(score[list_players[iPlayer]->teamID]->score > 1)
					list_players[iPlayer]->KillPlayerMapHazard();
			}
		}
		else
		{
			if(score[star->teamID]->score > 1)
				star->KillPlayerMapHazard();
		}

		if(game_values.gamemodesettings.star.shine)
		{
			if(fReverseScoring)
			{
				star->score->AdjustScore(1);
			}
			else
			{
				for(short iTeam = 0; iTeam < score_cnt; iTeam++)
				{
					if(star->teamID == iTeam)
						continue;

					score[iTeam]->AdjustScore(-1);
			
					if(score[iTeam]->score <= 0)
					{
						RemoveTeam(iTeam);
					}
				}
			}

			star = GetHighestScorePlayer(false);
			starItem->placeStar();
		}
		else
		{
			if(fReverseScoring)
			{
				star->score->AdjustScore(1);
			}
			else
			{
				star->score->AdjustScore(-1);
				
				if(star->score->score <= 0)
				{
					RemoveTeam(star->teamID);
					star = NULL;
				}
			}
		}

		if(!fReverseScoring)
		{
			if(!playedwarningsound)
			{
				short countscore = 0;
				for(short j = 0; j < score_cnt; j++)
				{
					for(short k = 0; k < score_cnt; k++)
					{
						if(j == k)
							continue;

						countscore += score[k]->score;
					}

					if(countscore <= 1)
					{
						playwarningsound();
						break;
					}

					countscore = 0;
				}
			}
		}
	}
}

void CGM_Star::draw()
{
	if(!gameover)
		drawtime();
}


bool CGM_Star::playerkilledplayer(CPlayer &, CPlayer &)
{
	return false;
}

bool CGM_Star::playerkilledself(CPlayer &player)
{
	return CGameMode::playerkilledself(player);
}

void CGM_Star::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
	{
		if(fReverseScoring)
			player.score->AdjustScore(-1 - (iType == 5 ? 1 : 0));
		else
			player.score->AdjustScore(1 + (iType == 5 ? 1 : 0));
	}
}



//Capture The Flag mode - each team has a base and a flag
//Protect your colored flag from being taken and score a point
//for stealing another teams flag and returning it to your base
CGM_CaptureTheFlag::CGM_CaptureTheFlag() : CGameMode()
{
	goal = 20;
	gamemode = game_mode_ctf;

	SetupModeStrings("Capture The Flag", "Flags", 5);
};

void CGM_CaptureTheFlag::init()
{
	CGameMode::init();

	bool fTeamUsed[4] = {false, false, false, false};

	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		short iTeamID = list_players[iPlayer]->teamID;
		if(!fTeamUsed[iTeamID])
		{
			fTeamUsed[iTeamID] = true;

			short iColorID = list_players[iPlayer]->colorID;
			OMO_FlagBase * base = new OMO_FlagBase(&spr_flagbases, iTeamID, iColorID);
			CO_Flag * flag = new CO_Flag(&spr_flags, base, iTeamID, iColorID);
			objectcollisionitems.add(base);
			objectsplayer.add(flag);
		}
	}
}


void CGM_CaptureTheFlag::think()
{
	if(gameover)
	{
		displayplayertext();
		return;
	}
	
	if(goal > -1)
	{
		for(short i = 0; i < list_players_cnt; i++)
		{
			if(list_players[i]->score->score >= goal)
			{
				winningteam = list_players[i]->teamID;
				gameover = true;

				RemovePlayersButTeam(winningteam);
				SetupScoreBoard(false);
				ShowScoreBoard();
			}
			else if(list_players[i]->score->score >= goal - 2 && !playedwarningsound)
			{
				playwarningsound();
			}
		}
	}
}


bool CGM_CaptureTheFlag::playerkilledplayer(CPlayer &, CPlayer &)
{
	return false;
}

bool CGM_CaptureTheFlag::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	if(player.carriedItem && player.carriedItem->getObjectType() == object_flag)
	{
		((CO_Flag*)player.carriedItem)->placeFlag();
	}

	return false;
}

void CGM_CaptureTheFlag::playerextraguy(CPlayer &player, short iType)
{
	if(gameover)
		return;
	
	player.score->AdjustScore(iType);

	if(goal > -1)
	{
		if(player.score->score >= goal)
		{
			winningteam = player.teamID;
			gameover = true;

			RemovePlayersButTeam(winningteam);
			SetupScoreBoard(false);
			ShowScoreBoard();
		}
		else if(player.score->score >= goal - 2 && !playedwarningsound)
		{
			playwarningsound();
		}
	}
}

//King of the Hill (Control an area for a certain amount of time)
CGM_KingOfTheHill::CGM_KingOfTheHill() : CGM_Domination()
{
	goal = 200;
	gamemode = game_mode_koth;

	SetupModeStrings("King Of The Hill", "Points", 50);
}

void CGM_KingOfTheHill::init()
{
	CGameMode::init();
	objectsfront.add(new OMO_KingOfTheHillZone(&spr_kingofthehillarea));
}

bool CGM_KingOfTheHill::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	return false;
}

bool CGM_KingOfTheHill::playerkilledself(CPlayer &player)
{
	return CGameMode::playerkilledself(player);
}


//Boss Mode
//Person to score fatal hit to boss wins!
CGM_Boss::CGM_Boss() : CGameMode() 
{
	gamemode = game_mode_boss;
	SetupModeStrings("Boss", "Lives", 5);
	iBossType = 0;
}

void CGM_Boss::init()
{
	CGameMode::init();
	enemytimer = (short)(rand() % 120) + 120;
	poweruptimer = 120;

	for(short iScore = 0; iScore < score_cnt; iScore++)
		score[iScore]->SetScore(5);

	objectcollisionitems.add(new MO_SledgeBrother(&spr_sledgebrothers, (iBossType == 0 ? 256 : (iBossType == 1 ? 256 : 320)), iBossType));
}


void CGM_Boss::think()
{
	if(!gameover && list_players_cnt == 0)
	{
		gameover = true;

		if(game_values.music)
		{
			ifsoundonstop(sfx_invinciblemusic);
			ifsoundonstop(sfx_timewarning);
			ifsoundonstop(sfx_slowdownmusic);
			ifsoundonplay(sfx_gameover);
			
			backgroundmusic[1].stop();
		}
	}

	if(gameover)
	{
		displayplayertext();
	}
	else
	{
		if(iBossType == 0)
		{
			//Randomly spawn koopas
			if(--enemytimer <= 0)
			{
				objectcollisionitems.add(new MO_Koopa(game_values.redkoopas ? &spr_redkoopa : &spr_koopa, 2, 8, rand() % 2 == 0, 30, 28, 1, 25, game_values.redkoopas));
				enemytimer = (short)(rand() % 120) + 120;  //Spawn koopas slowly
			}
		}
		else if(iBossType == 1)
		{
		
		}
		else if(iBossType == 2)
		{
			if(--enemytimer <= 0)
			{
				objectsfront.add(new OMO_Podobo(&spr_podobo, (short)(rand() % 608), -(float(rand() % 9) / 2.0f) - 9.0f, -1, -1, -1));
				enemytimer = (short)(rand() % 80 + 60);
			}

			if(--poweruptimer <= 0)
			{
				poweruptimer = (short)(rand() % 80 + 60);

				if(objectsplayer.countTypes(object_frenzycard) < list_players_cnt)
				{
					objectsplayer.add(new MO_FrenzyCard(&spr_frenzycards, 12, 8, 0));
				}
			}
		}
	}
}

void CGM_Boss::draw()
{
	if(gameover)
	{
		if(winningteam > -1)
		{
			game_font_large.drawCentered(320, 96, "Congratulations!");
			if(iBossType == 0)
				game_font_large.drawCentered(320, 118, "The Powerful Sledge Hammer Is Yours");
			else if(iBossType == 1)
				game_font_large.drawCentered(320, 118, "The Super Bomb Is Yours");
			else if(iBossType == 2)
				game_font_large.drawCentered(320, 118, "The Golden Podobo Is Yours");
		}
		else
		{
			game_font_large.drawCentered(320, 96, "You Failed To Defeat");
			
			if(iBossType == 0)
				game_font_large.drawCentered(320, 118, "The Mighty Sledge Brother");
			else if(iBossType == 1)
				game_font_large.drawCentered(320, 118, "The Mighty Bomb Brother");
			else if(iBossType == 2)
				game_font_large.drawCentered(320, 118, "The Mighty Flame Brother");
		}
	}
}

bool CGM_Boss::playerkilledplayer(CPlayer &inflictor, CPlayer &other)
{
	if(!gameover)
	{
		other.score->AdjustScore(-1);

		if(!playedwarningsound)
		{
			short countscore = 0;
			for(short k = 0; k < score_cnt; k++)
				countscore += score[k]->score;

			if(countscore <= 2)
				playwarningsound();
		}

		if(other.score->score <= 0)
		{
			RemoveTeam(other.teamID);
			return true;
		}
	}

	return false;
}

bool CGM_Boss::playerkilledself(CPlayer &player)
{
	CGameMode::playerkilledself(player);

	if(!gameover)
	{
		player.score->AdjustScore(-1);

		if(!playedwarningsound)
		{
			short countscore = 0;
			for(short k = 0; k < score_cnt; k++)
				countscore += score[k]->score;

			if(countscore <= 2)
				playwarningsound();
		}

		if(player.score->score <= 0)
		{
			RemoveTeam(player.teamID);
			return true;
		}
	}

	return false;
}

void CGM_Boss::playerextraguy(CPlayer &player, short iType)
{
	if(!gameover)
		player.score->AdjustScore(iType);
}

bool CGM_Boss::SetWinner(CPlayer * player)
{
	winningteam = player->teamID;
	gameover = true;

	RemovePlayersButTeam(winningteam);

	for(short iScore = 0; iScore < score_cnt; iScore++)
	{
		if(winningteam == iScore)
			continue;

		score[iScore]->SetScore(0);
	}

	SetupScoreBoard(false);

	if(game_values.music)
	{
		ifsoundonstop(sfx_invinciblemusic);
		ifsoundonstop(sfx_timewarning);
		ifsoundonstop(sfx_slowdownmusic);
		ifsoundonstop(sfx_bowserlaugh);
		
		backgroundmusic[1].play(true, false);
	}

	game_values.noexit = true;
	
	if(iBossType == 0)
		objectcollisionitems.add(new PU_SledgeHammerPowerup(&spr_sledgehammerpowerup, 304, -32, 1, 32000, 30, 30, 1, 1));
	else if(iBossType == 1)
		objectcollisionitems.add(new PU_BombPowerup(&spr_bombpowerup, 304, -32, 1, 32000, 30, 30, 1, 1));
	else if(iBossType == 2)
		objectcollisionitems.add(new PU_PodoboPowerup(&spr_podobopowerup, 304, -32, 1, 32000, 30, 30, 1, 1));

	return true;
}

void CGM_Boss::SetBossType(short bosstype)
{
	iBossType = bosstype;
}

