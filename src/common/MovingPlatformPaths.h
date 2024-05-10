#pragma once

class MovingPlatform;


class MovingPlatformPath {
public:
    MovingPlatformPath(float vel, float startX, float startY, float endX, float endY, bool preview);
    virtual ~MovingPlatformPath() = default;

    virtual bool Move(short type) = 0;  // Never let this class be instanciated
    virtual void Reset();

    void SetPlatform(MovingPlatform* platform) {
        pPlatform = platform;
    }

    short GetType() const {
        return iType;
    }

    float posX(short mode) const { return dCurrentX[mode]; }
    float posY(short mode) const { return dCurrentY[mode]; }
    float velX(short mode) const { return dVelX[mode]; }
    float velY(short mode) const { return dVelY[mode]; }

protected:
    MovingPlatform* pPlatform = nullptr;

    float dVelocity = 0.f;
    float dVelX[2], dVelY[2];

    float dPathPointX[2], dPathPointY[2];

    float dCurrentX[2], dCurrentY[2];

    short iType;

    friend class CMap;
};


class StraightPath : public MovingPlatformPath {
public:
    StraightPath(float vel, float startX, float startY, float endX, float endY, bool preview);
    ~StraightPath() override = default;

    bool Move(short type) override;
    void Reset() override;

protected:
    void SetVelocity(short type);

    short iOnStep[2];
    short iSteps;
    short iGoalPoint[2];

    float dAngle;

    friend class CMap;
};


class StraightPathContinuous : public StraightPath {
public:
    StraightPathContinuous(float vel, float startX, float startY, float angle, bool preview);
    ~StraightPathContinuous() override = default;

    bool Move(short type) override;
    void Reset() override;

private:
    float dEdgeX, dEdgeY;
};


class EllipsePath : public MovingPlatformPath {
public:
    EllipsePath(float vel, float dAngle, float dRadiusX, float dRadiusY, float dCenterX, float dCenterY, bool preview);
    ~EllipsePath() override = default;

    bool Move(short type) override;
    void SetPosition(short type);
    void Reset() override;

private:
    float dRadiusX, dRadiusY;
    float dAngle[2], dStartAngle;

    friend class CMap;
    friend void loadcurrentmap();
};


class FallingPath : public MovingPlatformPath {
public:
    FallingPath(float startX, float startY);
    ~FallingPath() override = default;

    bool Move(short type) override;
    void Reset() override;
};
