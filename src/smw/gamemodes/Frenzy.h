#pragma once

#include "Frag.h"

//Just like frag limit, but firepower cards appear randomly
class CGM_Frenzy : public CGM_Frag
{
    public:
        CGM_Frenzy();
        virtual ~CGM_Frenzy() {}

        void init();
        void think();
        char *getMenuString(char *buffer64);

        void setFrenzyOwner(CPlayer* palyer);

    private:
        short timer;
        short iSelectedPowerup;
        short iItemWeightCount;
        CPlayer* m_frenzyowner = nullptr;
};
