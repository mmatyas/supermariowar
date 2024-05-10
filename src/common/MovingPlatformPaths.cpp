#include "MovingPlatformPaths.h"

#include "Game.h"
#include "GameValues.h"
#include "movingplatform.h"
#include "ObjectBase.h"
#include "player.h"

#include <cmath>

extern CGameValues game_values;
extern std::vector<CPlayer*> players;

//------------------------------------------------------------------------------
// Moving Platform Path base class
//------------------------------------------------------------------------------

MovingPlatformPath::MovingPlatformPath(float vel, float startX, float startY, float endX, float endY, bool preview)
{
    dVelocity = vel;

    for (short type = 0; type < 2; type++) {
        dVelX[type] = 0.0f;
        dVelY[type] = 0.0f;
    }

    dPathPointX[0] = startX;
    dPathPointY[0] = startY;
    dPathPointX[1] = endX;
    dPathPointY[1] = endY;

    if (preview) {
        dPathPointX[0] /= 2.0f;
        dPathPointY[0] /= 2.0f;
        dPathPointX[1] /= 2.0f;
        dPathPointY[1] /= 2.0f;
        dVelocity /= 2.0f;
    }

    pPlatform = NULL;
}

void MovingPlatformPath::Reset()
{
    //advance the spawn test platform
    if (game_values.spawnstyle == SpawnStyle::Door) {
        for (short i = 0; i < 36; i++)
            Move(1);
    } else if (game_values.spawnstyle == SpawnStyle::Swirl) {
        for (short i = 0; i < 50; i++)
            Move(1);
    }
}
//------------------------------------------------------------------------------
// Straight Path
//------------------------------------------------------------------------------

StraightPath::StraightPath(float vel, float startX, float startY, float endX, float endY, bool preview)
    : MovingPlatformPath(vel, startX, startY, endX, endY, preview)
    , iGoalPoint{0, 0}
{
    iType = 0;

    float dWidth = dPathPointX[1] - dPathPointX[0];
    float dHeight = dPathPointY[1] - dPathPointY[0];
    float dLength = 0.0f;

    //Lock angle to vertical
    if (dWidth == 0) {
        if (dHeight > 0)
            dAngle = HALF_PI;
        else
            dAngle = THREE_HALF_PI;

        dLength = fabs(dHeight);
    } else if (dHeight == 0) { //Lock angle to horizontal
        if (dWidth > 0)
            dAngle = 0.0f;
        else
            dAngle = PI;

        dLength = fabs(dWidth);
    } else {
        dAngle = atan2(dHeight, dWidth);
        dLength = sqrt(dHeight * dHeight + dWidth * dWidth);
    }

    iSteps = (short)(dLength / dVelocity) + 1;

    for (short type = 0; type < 2; type++)
        SetVelocity(type);
}

bool StraightPath::Move(short type)
{
    dCurrentX[type] += dVelX[type];
    dCurrentY[type] += dVelY[type];

    if (++iOnStep[type] >= iSteps) {
        iOnStep[type] = 0;

        dCurrentX[type] = dPathPointX[iGoalPoint[type]];
        dCurrentY[type] = dPathPointY[iGoalPoint[type]];

        iGoalPoint[type] = 1 - iGoalPoint[type];

        SetVelocity(type);
    }

    return false;
}

void StraightPath::SetVelocity(short type)
{
    if (iGoalPoint[type] == 1) {
        dVelX[type] = dVelocity * cos(dAngle);
        dVelY[type] = dVelocity * sin(dAngle);
    } else {
        dVelX[type] = -dVelocity * cos(dAngle);
        dVelY[type] = -dVelocity * sin(dAngle);
    }

    //Fix rounding errors
    if (dVelX[type] < 0.01f && dVelX[type] > -0.01f)
        dVelX[type] = 0.0f;

    if (dVelY[type] < 0.01f && dVelY[type] > -0.01f)
        dVelY[type] = 0.0f;
}

void StraightPath::Reset()
{
    for (short type = 0; type < 2; type++) {
        iOnStep[type] = 0;

        dCurrentX[type] = dPathPointX[0];
        dCurrentY[type] = dPathPointY[0];

        iGoalPoint[type] = 1;
        SetVelocity(type);
    }

    MovingPlatformPath::Reset();
}


//------------------------------------------------------------------------------
// Straight Path Continuous
//------------------------------------------------------------------------------

StraightPathContinuous::StraightPathContinuous(float vel, float startX, float startY, float angle, bool preview) :
    StraightPath(vel, startX, startY, 0.0f, 0.0f, preview)
{
    iType = 1;

    dAngle = angle;

    for (short type = 0; type < 2; type++) {
        iGoalPoint[type] = 1;
        SetVelocity(type);
    }

    dEdgeX = App::screenWidth;
    dEdgeY = App::screenHeight;

    if (preview) {
        dEdgeX = App::screenWidth/2;
        dEdgeY = App::screenHeight/2;
    }
}

bool StraightPathContinuous::Move(short type)
{
    dCurrentX[type] += dVelX[type];
    dCurrentY[type] += dVelY[type];

    float dx = dCurrentX[type] - (float)pPlatform->iHalfWidth;
    if (dx < 0.0f)
        dCurrentX[type] += dEdgeX;
    else if (dx >= dEdgeX)
        dCurrentX[type] -= dEdgeX;

    if (dCurrentY[type] + (float)pPlatform->iHalfHeight < 0.0f)
        dCurrentY[type] += dEdgeY + (float)pPlatform->iHeight;
    else if (dCurrentY[type] - (float)pPlatform->iHalfHeight >= dEdgeY)
        dCurrentY[type] -= dEdgeY + (float)pPlatform->iHeight;

    return false;
}

void StraightPathContinuous::Reset()
{
    for (short type = 0; type < 2; type++) {
        dCurrentX[type] = dPathPointX[0];
        dCurrentY[type] = dPathPointY[0];
    }

    MovingPlatformPath::Reset();
}

//------------------------------------------------------------------------------
// Ellipse Path
//------------------------------------------------------------------------------
EllipsePath::EllipsePath(float vel, float angle, float radiusx, float radiusy, float centerx, float centery, bool preview) :
    MovingPlatformPath(vel, centerx, centery, 0.0f, 0.0f, preview)
{
    iType = 2;

    dStartAngle = angle;

    if (preview) {
        dRadiusX = radiusx / 2.0f;
        dRadiusY = radiusy / 2.0f;
        dVelocity *= 2.0f;
    } else {
        dRadiusX = radiusx;
        dRadiusY = radiusy;
    }

    for (short type = 0; type < 2; type++) {
        dAngle[type] = angle;
        SetPosition(type);
    }
}

bool EllipsePath::Move(short type)
{
    float dOldCurrentX = dCurrentX[type];
    float dOldCurrentY = dCurrentY[type];

    dAngle[type] += dVelocity;

    if (dVelocity < 0.0f) {
        while (dAngle[type] < 0.0f)
            dAngle[type] += TWO_PI;
    } else {
        while (dAngle[type] >= TWO_PI)
            dAngle[type] -= TWO_PI;
    }

    SetPosition(type);

    dVelX[type] = dCurrentX[type] - dOldCurrentX;
    dVelY[type] = dCurrentY[type] - dOldCurrentY;

    return false;
}

void EllipsePath::SetPosition(short type)
{
    dCurrentX[type] = dRadiusX * cos(dAngle[type]) + dPathPointX[0];
    dCurrentY[type] = dRadiusY * sin(dAngle[type]) + dPathPointY[0];
}

void EllipsePath::Reset()
{
    for (short type = 0; type < 2; type++) {
        dAngle[type] = dStartAngle;
        SetPosition(type);
    }

    MovingPlatformPath::Reset();
}

//------------------------------------------------------------------------------
// Falling path (for falling donut blocks)
//------------------------------------------------------------------------------

FallingPath::FallingPath(float startX, float startY) :
    MovingPlatformPath(0.0f, startX, startY, 0.0f, 0.0f, false)
{}

bool FallingPath::Move(short type)
{
    dVelY[type] = CapFallingVelocity(dVelY[type] + GRAVITATION);

    if (pPlatform->fy - pPlatform->iHalfHeight >= App::screenHeight) {
        //If a player is standing on this platform, clear him off
        for (CPlayer* player : players) {
            if (player->platform == pPlatform) {
                player->platform = NULL;
                player->vely = dVelY[type];
            }
        }

        pPlatform->fDead = true;
    }

    dCurrentY[type] += dVelY[type];

    return false;
}

void FallingPath::Reset()
{
    for (short type = 0; type < 2; type++) {
        dCurrentX[type] = dPathPointX[0];
        dCurrentY[type] = dPathPointY[0];
    }

    //Skip correctly setting the path "shadow" as a perf optimization
    //This does have the risk of a player spawning inside a falling donut block by accident
    //MovingPlatformPath::Reset();
}
