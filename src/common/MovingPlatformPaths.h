#pragma once

#include "math/Vec2.h"

class MovingPlatform;


/// Type ID used by the map files as a numeric identifier.
enum class PlatformPathType: unsigned char {
    Straight,
    StraightContinuous,
    Ellipse,
    Falling,
};


class MovingPlatformPath {
public:
    MovingPlatformPath(float speed, Vec2f startPos, Vec2f endPos, bool preview);
    virtual ~MovingPlatformPath() = default;

    virtual PlatformPathType typeId() const = 0;
    virtual bool Move(short type) = 0;
    virtual void Reset();

    void SetPlatform(MovingPlatform* platform) {
        m_platform = platform;
    }

    float speed() const { return m_speed; }
    const Vec2f& currentPos0() const { return m_currentPos[0]; }
    const Vec2f& currentPos1() const { return m_currentPos[1]; }
    const Vec2f& velocity0() const { return m_velocity[0]; }

protected:
    MovingPlatform* m_platform = nullptr;

    Vec2f m_startPos;
    Vec2f m_endPos;
    float m_speed;

    Vec2f m_velocity[2];
    Vec2f m_currentPos[2];
};


class StraightPath : public MovingPlatformPath {
public:
    StraightPath(float speed, Vec2f startPos, Vec2f endPos, bool preview);

    PlatformPathType typeId() const override { return PlatformPathType::Straight; }
    bool Move(short type) override;
    void Reset() override;

    const Vec2f& startPos() const { return m_startPos; }
    const Vec2f& endPos() const { return m_endPos; }
    float angle() const { return m_angle; }

protected:
    void SetVelocity(short type);

    float m_angle;
    short m_steps;

    unsigned short m_currentStep[2] = {0, 0};
    Vec2f* m_goalPoint[2] = {&m_startPos, &m_startPos};
};


class StraightPathContinuous : public StraightPath {
public:
    StraightPathContinuous(float speed, Vec2f startPos, float angle, bool preview);

    PlatformPathType typeId() const override { return PlatformPathType::StraightContinuous; }
    bool Move(short type) override;
    void Reset() override;

private:
    Vec2f m_edge;
};


class EllipsePath : public MovingPlatformPath {
public:
    EllipsePath(float speed, float dAngle, Vec2f radius, Vec2f centerPos, bool preview);

    PlatformPathType typeId() const override { return PlatformPathType::Ellipse; }
    bool Move(short type) override;
    void SetPosition(short type);
    void Reset() override;

    const Vec2f& centerPos() const { return m_startPos; }
    const Vec2f& radius() const { return m_radius; }
    float startAngle() const { return m_startAngle; }

private:
    Vec2f m_radius;
    float m_startAngle;

    float m_angle[2];
};


class FallingPath : public MovingPlatformPath {
public:
    FallingPath(Vec2f startPos);

    PlatformPathType typeId() const override { return PlatformPathType::Falling; }
    bool Move(short type) override;
    void Reset() override;
};
