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

MovingPlatformPath::MovingPlatformPath(float speed, Vec2f startPos, Vec2f endPos, bool preview)
    : m_startPos(std::move(startPos))
    , m_endPos(std::move(endPos))
{
    dVelocity = speed;

    for (short type = 0; type < 2; type++) {
        dVelX[type] = 0.0f;
        dVelY[type] = 0.0f;
    }

    if (preview) {
        m_startPos /= 2.f;
        m_endPos /= 2.f;
        dVelocity /= 2.0f;
    }

    m_platform = nullptr;
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
    : MovingPlatformPath(vel, Vec2f(startX, startY), Vec2f(endX, endY), preview)
{
    float dWidth = m_endPos.x - m_startPos.x;
    float dHeight = m_endPos.y - m_startPos.y;
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

        dCurrentX[type] = m_goalPoint[type]->x;
        dCurrentY[type] = m_goalPoint[type]->y;

        m_goalPoint[type] = (m_goalPoint[type] == &m_endPos) ? &m_startPos : &m_endPos;

        SetVelocity(type);
    }

    return false;
}

void StraightPath::SetVelocity(short type)
{
    if (m_goalPoint[type] == &m_endPos) {
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

        dCurrentX[type] = m_startPos.x;
        dCurrentY[type] = m_startPos.y;

        m_goalPoint[type] = &m_endPos;
        SetVelocity(type);
    }

    MovingPlatformPath::Reset();
}


//------------------------------------------------------------------------------
// Straight Path Continuous
//------------------------------------------------------------------------------

StraightPathContinuous::StraightPathContinuous(float vel, float startX, float startY, float angle, bool preview) :
    StraightPath(vel, startX, startY, 0.f, 0.f, preview)
{
    dAngle = angle;

    for (short type = 0; type < 2; type++) {
        m_goalPoint[type] = &m_endPos;
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

    float dx = dCurrentX[type] - (float)m_platform->iHalfWidth;
    if (dx < 0.0f)
        dCurrentX[type] += dEdgeX;
    else if (dx >= dEdgeX)
        dCurrentX[type] -= dEdgeX;

    if (dCurrentY[type] + (float)m_platform->iHalfHeight < 0.0f)
        dCurrentY[type] += dEdgeY + (float)m_platform->iHeight;
    else if (dCurrentY[type] - (float)m_platform->iHalfHeight >= dEdgeY)
        dCurrentY[type] -= dEdgeY + (float)m_platform->iHeight;

    return false;
}

void StraightPathContinuous::Reset()
{
    for (short type = 0; type < 2; type++) {
        dCurrentX[type] = m_startPos.x;
        dCurrentY[type] = m_startPos.y;
    }

    MovingPlatformPath::Reset();
}

//------------------------------------------------------------------------------
// Ellipse Path
//------------------------------------------------------------------------------
EllipsePath::EllipsePath(float vel, float angle, float radiusx, float radiusy, float centerx, float centery, bool preview) :
    MovingPlatformPath(vel, Vec2f(centerx, centery), Vec2f::zero(), preview)
{
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
    dCurrentX[type] = dRadiusX * cos(dAngle[type]) + m_startPos.x;
    dCurrentY[type] = dRadiusY * sin(dAngle[type]) + m_startPos.y;
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
    MovingPlatformPath(0.0f, Vec2f(startX, startY), Vec2f::zero(), false)
{}

bool FallingPath::Move(short type)
{
    dVelY[type] = CapFallingVelocity(dVelY[type] + GRAVITATION);

    if (m_platform->fy - m_platform->iHalfHeight >= App::screenHeight) {
        //If a player is standing on this platform, clear him off
        for (CPlayer* player : players) {
            if (player->platform == m_platform) {
                player->platform = nullptr;
                player->vely = dVelY[type];
            }
        }

        m_platform->fDead = true;
    }

    dCurrentY[type] += dVelY[type];

    return false;
}

void FallingPath::Reset()
{
    for (short type = 0; type < 2; type++) {
        dCurrentX[type] = m_startPos.x;
        dCurrentY[type] = m_startPos.y;
    }

           //Skip correctly setting the path "shadow" as a perf optimization
           //This does have the risk of a player spawning inside a falling donut block by accident
           //MovingPlatformPath::Reset();
}
