#pragma once

#include <SDL3/SDL.h>

#include <map>
#include <string>

class JoystickManager {
public:
    static JoystickManager& instance();

    void init();
    void shutdown();

    void handleEvent(const SDL_Event& event);

    const std::map<SDL_JoystickID, SDL_Joystick*>& openedDevices() const { return m_opened; }
    bool isAttached(SDL_JoystickID deviceId) const;

    std::string nameFor(SDL_JoystickID deviceId) const;
    SDL_Joystick* handleFor(SDL_JoystickID deviceId);

private:
    JoystickManager() = default;
    JoystickManager(const JoystickManager&) = delete;
    JoystickManager& operator=(const JoystickManager&) = delete;

    void scan();
    bool open(SDL_JoystickID deviceId);
    void close(SDL_JoystickID deviceId);
    void releasePlayersUsingDevice(SDL_JoystickID deviceId);

    std::map<SDL_JoystickID, SDL_Joystick*> m_opened;
};
