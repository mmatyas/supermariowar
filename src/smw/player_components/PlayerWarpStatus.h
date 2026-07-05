#pragma once

class CPlayer;
struct Warp;

class PlayerWarpStatus {
public:
    void update(CPlayer& player);
    void enterWarp(CPlayer& player, Warp* warp);

    short getWarpPlane() const;

private:
    void increasewarpcounter(CPlayer& player, short iGoal);
    void decreasewarpcounter(CPlayer& player);
    void chooseWarpExit(CPlayer& player);

    short warpcounter = 0;
    short warpconnection = 0;
    short warpid = 0;
    short warpplane = 0;
};
