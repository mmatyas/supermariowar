#ifndef _AI_H
#define _AI_H

class CObject;

class NearestObjects
{
	public:
		NearestObjects() {Reset();}
		~NearestObjects() {}

		void Reset()
		{
			player = NULL;
			goal = NULL;
			stomp = NULL;
			teammate = NULL;
			threat = NULL;

			playerdistance = 640000;
			goaldistance = 640000;
			stompdistance = 640000;
			teammatedistance = 640000;
			threatdistance = 640000;

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

class CPlayerAI
{
	public:
		CPlayerAI() {}
		virtual ~CPlayerAI() {}

		void SetPlayer(CPlayer * player) {pPlayer = player;}

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

#endif  //_AI_H

