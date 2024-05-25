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
    , m_speed(speed)
{

    for (short type = 0; type < 2; type++) {
        dVelX[type] = 0.0f;
        dVelY[type] = 0.0f;
    }

    if (preview) {
        m_startPos /= 2.f;
        m_endPos /= 2.f;
        m_speed /= 2.0f;
    }
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

StraightPath::StraightPath(float vel, Vec2f startPos, Vec2f endPos, bool preview)
    : MovingPlatformPath(vel, std::move(startPos), std::move(endPos), preview)
{
    float dWidth = m_endPos.x - m_startPos.x;
    float dHeight = m_endPos.y - m_startPos.y;
    float dLength = 0.0f;

           //Lock angle to vertical
    if (dWidth == 0) {
        if (dHeight > 0)
            m_angle = HALF_PI;
        else
            m_angle = THREE_HALF_PI;

        dLength = fabs(dHeight);
    } else if (dHeight == 0) { //Lock angle to horizontal
        if (dWidth > 0)
            m_angle = 0.0f;
        else
            m_angle = PI;

        dLength = fabs(dWidth);
    } else {
        m_angle = atan2(dHeight, dWidth);
        dLength = sqrt(dHeight * dHeight + dWidth * dWidth);
    }

    iSteps = (short)(dLength / m_speed) + 1;

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
    dVelX[type] = m_speed * cos(m_angle);
    dVelY[type] = m_speed * sin(m_angle);
    if (m_goalPoint[type] == &m_startPos) {
        dVelX[type] *= -1.f;
        dVelY[type] *= -1.f;
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

StraightPathContinuous::StraightPathContinuous(float speed, Vec2f startPos, float angle, bool preview)
    : StraightPath(speed, std::move(startPos), Vec2f::zero(), preview)
{
    m_angle = angle;

    for (short type = 0; type < 2; type++) {
        m_goalPoint[type] = &m_endPos;
        SetVelocity(type);
    }

    m_edge.x = App::screenWidth;
    m_edge.y = App::screenHeight;
    if (preview) {
        m_edge /= 2.f;
    }
}

bool StraightPathContinuous::Move(short type)
{
    dCurrentX[type] += dVelX[type];
    dCurrentY[type] += dVelY[type];

    float dx = dCurrentX[type] - (float)m_platform->iHalfWidth;
    if (dx < 0.0f)
        dCurrentX[type] += m_edge.x;
    else if (dx >= m_edge.x)
        dCurrentX[type] -= m_edge.x;

    if (dCurrentY[type] + (float)m_platform->iHalfHeight < 0.0f)
        dCurrentY[type] += m_edge.y + (float)m_platform->iHeight;
    else if (dCurrentY[type] - (float)m_platform->iHalfHeight >= m_edge.y)
        dCurrentY[type] -= m_edge.y + (float)m_platform->iHeight;

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
EllipsePath::EllipsePath(float vel, float angle, Vec2f radius, Vec2f centerPos, bool preview)
    : MovingPlatformPath(vel, std::move(centerPos), Vec2f::zero(), preview)
    , m_radius(std::move(radius))
    , m_startAngle(angle)
{
    if (preview) {
        m_radius /= 2.f;
        m_speed *= 2.f;
    }

    for (short type = 0; type < 2; type++) {
        m_angle[type] = m_startAngle;
        SetPosition(type);
    }
}

bool EllipsePath::Move(short type)
{
    float dOldCurrentX = dCurrentX[type];
    float dOldCurrentY = dCurrentY[type];

    m_angle[type] += m_speed;

    if (m_speed < 0.0f) {
        while (m_angle[type] < 0.0f)
            m_angle[type] += TWO_PI;
    } else {
        while (m_angle[type] >= TWO_PI)
            m_angle[type] -= TWO_PI;
    }

    SetPosition(type);

    dVelX[type] = dCurrentX[type] - dOldCurrentX;
    dVelY[type] = dCurrentY[type] - dOldCurrentY;

    return false;
}

void EllipsePath::SetPosition(short type)
{
    dCurrentX[type] = m_radius.x * cos(m_angle[type]) + m_startPos.x;
    dCurrentY[type] = m_radius.y * sin(m_angle[type]) + m_startPos.y;
}

void EllipsePath::Reset()
{
    for (short type = 0; type < 2; type++) {
        m_angle[type] = m_startAngle;
        SetPosition(type);
    }

    MovingPlatformPath::Reset();
}

//------------------------------------------------------------------------------
// Falling path (for falling donut blocks)
//------------------------------------------------------------------------------

FallingPath::FallingPath(Vec2f startPos)
    : MovingPlatformPath(0.0f, std::move(startPos), Vec2f::zero(), false)
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
