#ifndef GAMESTATE_H
#define GAMESTATE_H

class GameState
{
    public:
        virtual bool init() { return true; }
        virtual void update() = 0;
        virtual void cleanup() {}

    protected:
        virtual void onEnterState() {}
        virtual void onLeaveState() {}

    friend class GameStateManager;
};

class GameStateManager
{
    public:
        GameState* currentState;

        void changeStateTo(GameState* newState) {
            //assert(newState != 0);
            currentState->onLeaveState();
            currentState = newState;
            currentState->onEnterState();
        }

        static GameStateManager& instance() {
            static GameStateManager gsm;
            return gsm;
        }

    private:

        GameStateManager() {}
        GameStateManager(GameStateManager const&);
        void operator=(GameStateManager const&);

};

#endif // GAMESTATE_H
