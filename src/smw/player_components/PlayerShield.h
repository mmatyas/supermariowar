#ifndef PLAYER_SHIELD
#define PLAYER_SHIELD

class CPlayer;

/*
    This changes the way the shield behaves. It can be set to No Shield,
    Soft (pass through players), Soft With Stomp (pass through players,
    but you can stomp them), and Hard (stomp players but you can't pass
    through them).
*/
enum PlayerShieldType {
	OFF = 0,
	SOFT = 1,
	SOFT_WITH_STOMP = 2,
	HARD = 3
};

class PlayerShield
{
public:
	void reset();
    void update();

    bool is_on() const;
    void turn_on();
    void abort();

    PlayerShieldType getType() const;
    void setType(PlayerShieldType);

    unsigned short time_left() const;

private:
	PlayerShieldType type;
	short timer;

#ifdef _DEBUG
friend class GameplayState;
#endif
};

#endif // PLAYER_SHIELD
