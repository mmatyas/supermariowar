#pragma once

enum KillStyle {
    Stomp,
    Star,
    Fireball,
    Bobomb,
    Bounce,
    Pow,
    Goomba,
    BulletBill,
    Hammer,
    Shell,
    ThrowBlock,
    CheepCheep,
    Koopa,
    Boomerang,
    Feather,
    IceBlast,
    Podobo,
    Bomb,
    Leaf,
    PWings,
    KuriboShoe,
    PoisonMushroom,
    Environment,
    Push,
    BuzzyBeetle,
    Spiny,
    Phanto,
};

constexpr bool ShouldPenalizeHazardDeath(KillStyle style, bool losePointsOnHazardDeath)
{
    return style != Environment || losePointsOnHazardDeath;
}
