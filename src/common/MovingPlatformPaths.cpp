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
        m_velocity[type] = Vec2f::zero();
    }

    if (preview) {
        m_startPos /= 2.f;
        m_endPos /= 2.f;
        m_speed /= 2.f;
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
StraightPath::StraightPath(float speed, Vec2f startPos, Vec2f endPos, bool preview)
    : MovingPlatformPath(speed, startPos, endPos, preview)
    , m_goalPos{m_endPos, m_endPos}
{
    const float width = m_endPos.x - m_startPos.x;
    const float height = m_endPos.y - m_startPos.y;
    float length = 0.f;

    if (width == 0) {
        //Lock angle to vertical
        m_angle = (height > 0) ? HALF_PI : THREE_HALF_PI;
        length = ::fabs(height);
    } else if (height == 0) {
        //Lock angle to horizontal
        m_angle = (width > 0) ? 0.f: PI;
        length = ::fabs(width);
    } else {
        m_angle = atan2(height, width);
        length = sqrt(height * height + width * width);
    }

    iSteps = (short)(length / m_speed) + 1;

    for (short type = 0; type < 2; type++)
        SetVelocity(type);
}

bool StraightPath::Move(short type)
{
    m_currentPos[type] += m_velocity[type];

    if (++iOnStep[type] >= iSteps) {
        iOnStep[type] = 0;

        m_currentPos[type] = m_movesForward[type] ? m_endPos : m_startPos;
        m_movesForward[type] = !m_movesForward[type];

        SetVelocity(type);
    }

    return false;
}

void StraightPath::SetVelocity(short type)
{
    m_velocity[type].x = m_speed * cos(m_angle);
    m_velocity[type].y = m_speed * sin(m_angle);
    if (!m_movesForward[type]) {
        m_velocity[type] *= -1.f;
    }

    //Fix rounding errors
    if (::fabs(m_velocity[type].x) < 0.01f)
        m_velocity[type].x = 0.f;

    if (::fabs(m_velocity[type].y) < 0.01f)
        m_velocity[type].y = 0.f;
}

void StraightPath::Reset()
{
    for (short type = 0; type < 2; type++) {
        iOnStep[type] = 0;

        m_currentPos[type] = m_startPos;
        m_goalPos[type] = m_endPos;
        m_movesForward[type] = true;

        SetVelocity(type);
    }

    MovingPlatformPath::Reset();
}


//------------------------------------------------------------------------------
// Straight Path Continuous
//------------------------------------------------------------------------------

StraightPathContinuous::StraightPathContinuous(float speed, Vec2f startPos, float angle, bool preview)
    : StraightPath(speed, startPos, Vec2f::zero(), preview)
    , m_edge{App::screenWidth, App::screenHeight}
{
    m_angle = angle;

    if (preview) {
        m_edge /= 2.f;
    }
}

bool StraightPathContinuous::Move(short type)
{
    m_currentPos[type].x += m_velocity[type].x;
    m_currentPos[type].y += m_velocity[type].y;

    float dx = m_currentPos[type].x - (float)m_platform->iHalfWidth;
    if (dx < 0.f)
        m_currentPos[type].x += m_edge.x;
    else if (dx >= m_edge.x)
        m_currentPos[type].x -= m_edge.x;

    if (m_currentPos[type].y + (float)m_platform->iHalfHeight < 0.f)
        m_currentPos[type].y += m_edge.y + (float)m_platform->iHeight;
    else if (m_currentPos[type].y - (float)m_platform->iHalfHeight >= m_edge.y)
        m_currentPos[type].y -= m_edge.y + (float)m_platform->iHeight;

    return false;
}

void StraightPathContinuous::Reset()
{
    for (short type = 0; type < 2; type++) {
        m_currentPos[type] = m_startPos;
    }

    MovingPlatformPath::Reset();
}

//------------------------------------------------------------------------------
// Ellipse Path
//------------------------------------------------------------------------------
EllipsePath::EllipsePath(float speed, float angle, Vec2f radius, Vec2f center, bool preview)
    : MovingPlatformPath(speed, center, Vec2f::zero(), preview)
    , m_radius(std::move(radius))
    , m_angle{angle, angle}
    , m_startAngle(angle)
{
    if (preview) {
        m_radius /= 2.f;
        m_speed *= 2.f;
    }

    for (short type = 0; type < 2; type++) {
        SetPosition(type);
    }
}

bool EllipsePath::Move(short type)
{
    Vec2f oldPos = m_currentPos[type];

    m_angle[type] += m_speed;

    if (m_speed < 0.f) {
        while (m_angle[type] < 0.f)
            m_angle[type] += TWO_PI;
    } else {
        while (m_angle[type] >= TWO_PI)
            m_angle[type] -= TWO_PI;
    }

    SetPosition(type);

    m_velocity[type] = m_currentPos[type] - oldPos;

    return false;
}

void EllipsePath::SetPosition(short type)
{
    m_currentPos[type].x = m_radius.x * cos(m_angle[type]) + m_startPos.x;
    m_currentPos[type].y = m_radius.y * sin(m_angle[type]) + m_startPos.y;
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
    : MovingPlatformPath(0.f, std::move(startPos), Vec2f::zero(), false)
{}

bool FallingPath::Move(short type)
{
    m_velocity[type].y = CapFallingVelocity(m_velocity[type].y + GRAVITATION);

    if (m_platform->fy - m_platform->iHalfHeight >= App::screenHeight) {
        //If a player is standing on this platform, clear him off
        for (CPlayer* player : players) {
            if (player->platform == m_platform) {
                player->platform = nullptr;
                player->vely = m_velocity[type].y;
            }
        }

        m_platform->fDead = true;
    }

    m_currentPos[type].y += m_velocity[type].y;

    return false;
}

void FallingPath::Reset()
{
    for (short type = 0; type < 2; type++) {
        m_currentPos[type] = m_startPos;
    }

    //Skip correctly setting the path "shadow" as a perf optimization
    //This does have the risk of a player spawning inside a falling donut block by accident
    //MovingPlatformPath::Reset();
}
