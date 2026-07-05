#pragma once

class CPlayer;

class PlayerSuperStomp {
public:
    PlayerSuperStomp();
    void reset();

    bool isStomping() const;
    bool isInSuperStompState() const;
    void startSuperStomping(CPlayer &player);

    void update(CPlayer &player);
    void update_onGroundHit(CPlayer &player);

private:
    // lock on -> floating -> falling -> hitting the ground, create damage zone, lock off

    bool superstomp_lock = false; // When the player starts super stomping this becomes true
    short floating_timer = 0; // then the player floats for some time
    bool is_stomping = false; // then starts falling
    short iSuperStompExitTimer = 0; // but after some time, loses the strength of the stomp
};
