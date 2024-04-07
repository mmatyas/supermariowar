#ifndef SMW_GAMEOBJECT_BLOCK_PU_PWingsPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_PWingsPowerup_H

class PU_PWingsPowerup : public MO_Powerup
{
    public:
        PU_PWingsPowerup(gfxSprite * nspr, short x, short y);
        bool collide(CPlayer *player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_PWingsPowerup_H
