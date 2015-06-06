#ifndef PLAYER_COLLISIONS
#define PLAYER_COLLISIONS

class CPlayer;
class CObject;

class PlayerCollisions
{
public:
	bool checktop(CPlayer&);
	void checksides(CPlayer&);

	bool handle_p2o(CPlayer* o1, CObject* o2);
	void handle_p2p(CPlayer* o1, CPlayer* o2);

private:
	bool checkleft(CPlayer&);
	bool checkright(CPlayer&);

	bool is_stomping(CPlayer& player, CPlayer& other);
	void handle_p2p_pushback(CPlayer* o1, CPlayer* o2);
};

#endif // PLAYER_COLLISIONS


