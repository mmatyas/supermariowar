#ifndef AI_H
#define AI_H

#include <cstdio>
#include <map>

#include "Game.h"

class CObject;
class CPlayer;
struct COutputControl;

class NearestObjects
{
public:
    NearestObjects() {
        Reset();
    }
    ~NearestObjects() {}

    void Reset() {
        player = NULL;
        goal = NULL;
        stomp = NULL;
        teammate = NULL;
        threat = NULL;

        playerdistance = App::screenWidth * 1000;
        goaldistance = App::screenWidth * 1000;
        stompdistance = App::screenWidth * 1000;
        teammatedistance = App::screenWidth * 1000;
        threatdistance = App::screenWidth * 1000;

        playerwrap = false;
        goalwrap = false;
        stompwrap = false;
        teammatewrap = false;
        threatwrap = false;
    }

    CPlayer * player;
    CPlayer * teammate;

    CObject * goal;
    CObject * stomp;
    CObject * threat;

    int playerdistance;
    int teammatedistance;
    int goaldistance;
    int stompdistance;
    int threatdistance;

    bool playerwrap;
    bool teammatewrap;
    bool goalwrap;
    bool stompwrap;
    bool threatwrap;
};

struct AttentionObject {
    int iID;	  //Global ID of this object
    short iType;  //Ignore it, high priority, etc.
    short iTimer;  //When it the attention expires, 0 for never
};

class CPlayerAI
{
public:
    CPlayerAI();
    virtual ~CPlayerAI();

    virtual void Init();

    void SetPlayer(CPlayer * player) {
        pPlayer = player;
    }

    virtual void Think(COutputControl * playerKeys);

    void GetNearestObjects();
    void DistanceToObject(CObject * object, CObject ** target, int * nearest, bool * wrap);
    void DistanceToObjectCenter(CObject * object, CObject ** target, int * nearest, bool * wrap);
    void DistanceToPlayer(CPlayer * player, CPlayer ** target, int * nearest, bool * wrap);

protected:
    CPlayer * pPlayer;

private:
    short iFallDanger;
    NearestObjects nearestObjects;

    std::map<int, AttentionObject*> attentionObjects;
    AttentionObject currentAttentionObject;
};


/**************************************************
* Inherit from the CPlayerAI class and create your own!
***************************************************/
class CSimpleAI : public CPlayerAI
{
public:
    CSimpleAI() {}
    ~CSimpleAI() {}
    void Think(COutputControl * playerKeys);

};

#endif // AI_H
