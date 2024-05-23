#pragma once

class MovingPlatform;


class MovingPlatformPath {
public:
    MovingPlatformPath(float vel, float startX, float startY, float endX, float endY, bool preview);
    virtual ~MovingPlatformPath() = default;

    virtual bool Move(short type) = 0;
    virtual void Reset();

    void SetPlatform(MovingPlatform* platform) {
        pPlatform = platform;
    }

    short GetType() const { return iType; }

protected:
    MovingPlatform* pPlatform;

    float dVelocity;
    float dVelX[2], dVelY[2];

    float dPathPointX[2], dPathPointY[2];

    float dCurrentX[2], dCurrentY[2];

    short iType;

    friend class MovingPlatform;
    friend class CMap;
    friend void loadcurrentmap();
    friend void insert_platforms_into_map();
};


class StraightPath : public MovingPlatformPath {
public:
    StraightPath(float vel, float startX, float startY, float endX, float endY, bool preview);

    bool Move(short type) override;
    void Reset() override;

protected:
    void SetVelocity(short type);

    short iOnStep[2];
    short iSteps;
    short iGoalPoint[2];

    float dAngle;

    friend class MovingPlatform;
    friend class CMap;
    friend void loadcurrentmap();
    friend void insert_platforms_into_map();
};


class StraightPathContinuous : public StraightPath {
public:
    StraightPathContinuous(float vel, float startX, float startY, float angle, bool preview);

    bool Move(short type) override;
    void Reset() override;

private:
    float dEdgeX, dEdgeY;

    friend class MovingPlatform;
    friend class CMap;
    friend void loadcurrentmap();
    friend void insert_platforms_into_map();
};


class EllipsePath : public MovingPlatformPath {
public:
    EllipsePath(float vel, float dAngle, float dRadiusX, float dRadiusY, float dCenterX, float dCenterY, bool preview);

    bool Move(short type) override;
    void SetPosition(short type);
    void Reset() override;

private:
    float dRadiusX, dRadiusY;
    float dAngle[2], dStartAngle;

    friend class MovingPlatform;
    friend class CMap;
    friend void loadcurrentmap();
    friend void insert_platforms_into_map();
};


class FallingPath : public MovingPlatformPath {
public:
    FallingPath(float startX, float startY);

    bool Move(short type) override;
    void Reset() override;

private:
    friend class MovingPlatform;
};
