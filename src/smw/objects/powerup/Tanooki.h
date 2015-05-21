#ifndef SMW_GAMEOBJECT_BLOCK_PU_Tanooki_H
#define SMW_GAMEOBJECT_BLOCK_PU_Tanooki_H

class PU_Tanooki : public MO_Powerup
{
    public:
        PU_Tanooki(short x, short y);
        bool collide(CPlayer *player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_Tanooki_H
