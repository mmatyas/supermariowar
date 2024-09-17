#pragma once

#include "ObjectBase.h"


class IO_BulletBillCannon : public CObject {
public:
    IO_BulletBillCannon(Vec2s pos, short freq, float vel, bool preview);

    void draw() override {}
    void update() override;

    bool collide(CPlayer*) override { return false; }
    void collide(IO_MovingObject*) override {}

private:
    void SetNewTimer();

    short m_freq = 0;
    short m_timer = 0;
    float m_vel = 0.f;
    bool m_preview = false;
};
