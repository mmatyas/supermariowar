#ifndef SMW_GAMEOBJECT_BLOCK_POWERUP_H
#define SMW_GAMEOBJECT_BLOCK_POWERUP_H

#include "IO_Block.h"
#include "GlobalConstants.h"

class gfxSprite;
class CPlayer;
class IO_MovingObject;

class B_PowerupBlock : public IO_Block
{
    public:
        B_PowerupBlock(gfxSprite* nspr, short x, short y,
            short iNumSpr, short aniSpeed, bool fHidden, short * piSettings);
        virtual ~B_PowerupBlock();

        BlockType getBlockType() {
            return block_powerup;
        }

        void draw();
        void update();
        void reset();

        bool collide(CPlayer* player, short direction, bool useBehavior);
        bool collide(IO_MovingObject* object, short direction);

        bool hittop(CPlayer* player, bool useBehavior);
        bool hitbottom(CPlayer* player, bool useBehavior);

        bool hittop(IO_MovingObject* object);
        bool hitright(IO_MovingObject* object);
        bool hitleft(IO_MovingObject* object);

        void triggerBehavior();
        virtual short SelectPowerup();

    protected:
        short iCountWeight;

        short timer;
        bool side;
        short iNumSprites;
        short animationSpeed;
        short drawFrame;
        short animationTimer;
        short animationWidth;

        short settings[NUM_POWERUPS];

    friend class CPlayer;
};

#endif // SMW_GAMEOBJECT_BLOCK_POWERUP_H
