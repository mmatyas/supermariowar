#include "objecthazard.h"

#include "map.h"
#include "ObjectContainer.h"
#include "ResourceManager.h"
#include "objects/IO_BulletBillCannon.h"
#include "objects/IO_FlameCannon.h"
#include "objects/moving/MO_PirhanaPlant.h"
#include "objects/overmap/WO_OrbitHazard.h"

extern CObjectContainer noncolcontainer;
extern CObjectContainer objectcontainer[3];
extern CMap* g_map;
extern CResourceManager* rm;


void LoadMapHazards(bool fPreview)
{
    //Make sure we don't have any objects created before we create them from the map settings
    noncolcontainer.clean();
    objectcontainer[0].clean();
    objectcontainer[1].clean();
    objectcontainer[2].clean();

    //Create objects for all the map hazards
    for (const MapHazard& hazard : g_map->maphazards) {
        const Vec2s pos(hazard.ix * 16, hazard.iy * 16);

        switch (hazard.itype) {
            case MapHazardType::FireBar:
                for (short iFireball = 0; iFireball < hazard.iparam[0]; iFireball++)
                    objectcontainer[1].add(new OMO_OrbitHazard(&rm->spr_hazard_fireball[fPreview ? 1 : 0], pos + Vec2s(16, 16), (float)(iFireball * 24), hazard.dparam[0], hazard.dparam[1], 4, 8, 18, 18, 0, 0, 0, hazard.dparam[0] < 0.0f ? 18 : 0, 18, 18));
                break;

            case MapHazardType::RotoDisc: {
                float dSector = TWO_PI / hazard.iparam[0];
                for (short iRotoDisc = 0; iRotoDisc < hazard.iparam[0]; iRotoDisc++) {
                    float dAngle = hazard.dparam[1] + iRotoDisc * dSector;
                    if (dAngle > TWO_PI)
                        dAngle -= TWO_PI;

                    objectcontainer[1].add(new OMO_OrbitHazard(&rm->spr_hazard_rotodisc[fPreview ? 1 : 0], pos + Vec2s(16, 16), hazard.dparam[2], hazard.dparam[0], dAngle, 21, 8, 32, 32, 0, 0, 0, 0, 32, 32));
                }
                break;
            }

            case MapHazardType::BulletBill:
                noncolcontainer.add(new IO_BulletBillCannon(pos, hazard.iparam[0], hazard.dparam[0], fPreview));
                break;

            case MapHazardType::FlameCannon:
                objectcontainer[1].add(new IO_FlameCannon(pos, hazard.iparam[0], hazard.iparam[1]));
                break;

            case MapHazardType::PirhanaGreen:
            case MapHazardType::PirhanaRed:
            case MapHazardType::PirhanaTall:
            case MapHazardType::PirhanaShort: {
                const short iPirhanaPlantType = static_cast<short>(hazard.itype) - static_cast<short>(MapHazardType::PirhanaGreen);
                objectcontainer[1].add(new MO_PirhanaPlant(pos, iPirhanaPlantType, hazard.iparam[0], hazard.iparam[1], fPreview));
                break;
            }
        }
    }
}
