#pragma once

#include "Frag.h"

//Similar to frag limit but:
//When a player is killed by another player, they get a "jail" marker
//Jailed players move slowly
//Jailed players can be freed by teammates
//If all players on a team are jailed, bonus kill goes to other team
//Similar to frag limit, but players get bonus frags for the number of players they have "owned"
class CGM_Jail : public CGM_Frag
{
    public:
        CGM_Jail();
        virtual ~CGM_Jail() {}

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);
        char *getMenuString(char *buffer64);
};
