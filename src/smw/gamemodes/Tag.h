#pragma once

#include "GameMode.h"


class CGM_Tag : public CGameMode {
public:
    CGM_Tag();

    void init() override;
    void think() override;
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
    void playerextraguy(CPlayer &player, short iType) override;

    CPlayer* tagged() const { return m_tagged; }
    void setTagged(CPlayer* player);

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 100;
    }
#endif

private:
    CPlayer* m_tagged = nullptr;
};
