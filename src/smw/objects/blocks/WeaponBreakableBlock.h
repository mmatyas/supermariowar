#pragma once

#include "IO_Block.h"


enum class WeaponDamageType : unsigned char {
    Fireball,
    Feather,
    Shell,
    Bomb,
    Boomerang,
    Hammer,
    KuriboShoe,
    PWings,
    Star,
    Leaf,
};


class B_WeaponBreakableBlock : public IO_Block {
public:
    B_WeaponBreakableBlock(gfxSprite *nspr, short x, short y, WeaponDamageType type);

    BlockType getBlockType() const override {
        return BlockType::WeaponBreakable;
    }

    void draw() override;
    void update() override;

    bool hittop(CPlayer* player, bool useBehavior) override;
    bool hitbottom(CPlayer* player, bool useBehavior) override;
    bool hitleft(CPlayer* player, bool useBehavior) override;
    bool hitright(CPlayer* player, bool useBehavior) override;

    bool hittop(IO_MovingObject* object) override;
    bool hitbottom(IO_MovingObject* object) override;
    bool hitright(IO_MovingObject* object) override;
    bool hitleft(IO_MovingObject* object) override;

    void triggerBehavior(short iPlayerID, short iTeamID);

    WeaponDamageType type() const { return iType; }

private:
    bool objecthitside(IO_MovingObject * object);

    const WeaponDamageType iType;
    const short iDrawOffsetX;
};
