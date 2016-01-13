#ifndef PLAYER_INVINCIBILITY
#define PLAYER_INVINCIBILITY

class CPlayer;

class PlayerInvincibility
{
public:
	void reset();
    void update(CPlayer& player);

    bool is_on() const;
    void turn_on(CPlayer& player);

private:
    bool invincible;
	short timer;
};

#endif // PLAYER_INVINCIBILITY
