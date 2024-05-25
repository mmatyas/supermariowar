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

protected:
    MovingPlatform* m_platform;

    float m_speed;
    float dVelX[2], dVelY[2];

    Vec2f m_startPos;
    Vec2f m_endPos;

    float dCurrentX[2], dCurrentY[2];

    friend class MovingPlatform;
    friend class CMap;
    friend void loadcurrentmap();
    friend void insert_platforms_into_map();
};


class StraightPath : public MovingPlatformPath {
public:
    StraightPath(float vel, Vec2f startPos, Vec2f endPos, bool preview);

    PlatformPathType typeId() const override { return PlatformPathType::Straight; }
    bool Move(short type) override;
    void Reset() override;

protected:
    void SetVelocity(short type);

    short iOnStep[2];
    short iSteps;
    Vec2f* m_goalPoint[2] = {&m_startPos, &m_startPos};

    float m_angle;

    friend class MovingPlatform;
    friend class CMap;
    friend void loadcurrentmap();
    friend void insert_platforms_into_map();
};


class StraightPathContinuous : public StraightPath {
public:
    StraightPathContinuous(float vel, Vec2f startPos, float angle, bool preview);

    PlatformPathType typeId() const override { return PlatformPathType::StraightContinuous; }
    bool Move(short type) override;
    void Reset() override;

private:
    Vec2f m_edge;

    friend class MovingPlatform;
    friend class CMap;
    friend void loadcurrentmap();
    friend void insert_platforms_into_map();
};


class EllipsePath : public MovingPlatformPath {
public:
    EllipsePath(float vel, float dAngle, Vec2f radius, Vec2f centerPos, bool preview);

    PlatformPathType typeId() const override { return PlatformPathType::Ellipse; }
    bool Move(short type) override;
    void SetPosition(short type);
    void Reset() override;

private:
    Vec2f m_radius;
    float m_startAngle;
    float m_angle[2];

    friend class MovingPlatform;
    friend class CMap;
    friend void loadcurrentmap();
    friend void insert_platforms_into_map();
};


class FallingPath : public MovingPlatformPath {
public:
    FallingPath(Vec2f startPos);

    PlatformPathType typeId() const override { return PlatformPathType::Falling; }
    bool Move(short type) override;
    void Reset() override;

private:
    friend class MovingPlatform;
};
