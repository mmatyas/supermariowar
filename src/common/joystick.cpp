#include "joystick.h"

#include "GameValues.h"
#include "GlobalConstants.h"
#include "input.h"

#include <cstdio>
#include <cstring>
#include <format>

extern CGameValues game_values;

JoystickManager& JoystickManager::instance()
{
    static JoystickManager joystickManager;
    return joystickManager;
}

void JoystickManager::init()
{
    SDL_SetJoystickEventsEnabled(true);
    scan();
}

void JoystickManager::shutdown()
{
    for (const auto& [deviceId, joystick] : m_opened)
        SDL_CloseJoystick(joystick);

    m_opened.clear();
}

void JoystickManager::scan()
{
    int count = 0;
    SDL_JoystickID* deviceIds = SDL_GetJoysticks(&count);
    if (deviceIds) {
        for (int i = 0; i < count; i++) {
            open(deviceIds[i]);
        }
        SDL_free(deviceIds);
    }

    // Drop handles of devices that went away without a remove event
    // (eg. because no one consumed the event queue for a while)
    for (auto it = m_opened.begin(); it != m_opened.end(); ) {
        if (!SDL_JoystickConnected(it->second)) {
            SDL_CloseJoystick(it->second);
            it = m_opened.erase(it);
        } else {
            ++it;
        }
    }
}

void JoystickManager::handleEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_JOYSTICK_ADDED) {
        open(event.jdevice.which);
    }
    else if (event.type == SDL_EVENT_JOYSTICK_REMOVED) {
        const SDL_JoystickID deviceId = event.jdevice.which;
        close(deviceId);
        releasePlayersUsingDevice(deviceId);
    }
}

bool JoystickManager::isAttached(SDL_JoystickID deviceId) const
{
    return deviceId != DEVICE_KEYBOARD_ID && m_opened.contains(deviceId);
}

std::string JoystickManager::nameFor(SDL_JoystickID deviceId) const
{
    if (const char* name = SDL_GetJoystickNameForID(deviceId))
        return name;

    return std::format("Joystick {}", deviceId);
}

std::vector<SDL_JoystickID> JoystickManager::instanceIdsForGuid(const SDL_GUID& guid) const
{
    const auto sameGuid = [&guid](SDL_JoystickID joyId) {
        const SDL_GUID deviceGuid = SDL_GetJoystickGUIDForID(joyId);
        return std::memcmp(guid.data, deviceGuid.data, sizeof(SDL_GUID)) == 0;
    };

    std::vector<SDL_JoystickID> deviceIds;
    for (const auto& [joyId, joy] : m_opened) {
        if (sameGuid(joyId))
            deviceIds.push_back(joyId);
    }

    return deviceIds;
}

SDL_Joystick* JoystickManager::handleFor(SDL_JoystickID deviceId)
{
    const auto it = m_opened.find(deviceId);
    return it != m_opened.end() ? it->second : NULL;
}

bool JoystickManager::open(SDL_JoystickID deviceId)
{
    if (m_opened.find(deviceId) != m_opened.end())
        return true;

    SDL_Joystick* joystick = SDL_OpenJoystick(deviceId);
    if (!joystick) {
        printf("%s", std::format("[pad] Could not open joystick {}: {}\n", deviceId, SDL_GetError()).c_str());
        return false;
    }

    printf("%s", std::format("[pad] Opened joystick {} ({})\n", deviceId, nameFor(deviceId)).c_str());

    m_opened[deviceId] = joystick;
    return true;
}

void JoystickManager::close(SDL_JoystickID deviceId)
{
    const auto it = m_opened.find(deviceId);
    if (it == m_opened.end())
        return;

    SDL_CloseJoystick(it->second);
    m_opened.erase(it);
}

void JoystickManager::releasePlayersUsingDevice(SDL_JoystickID deviceId)
{
    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        CInputPlayerControl* control = game_values.playerInput.inputControls[iPlayer];
        if (!control || control->iDevice != deviceId)
            continue;

        // Switch the player back to the keyboard configuration
        game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][0];
        game_values.playerInput.inputControls[iPlayer]->iDevice = DEVICE_KEYBOARD_ID;

        // Release all keys of the player, they can no longer be pressed up
        COutputControl& outputControl = game_values.playerInput.outputControls[iPlayer];
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
            outputControl.keys[iKey].fDown = false;
            outputControl.keys[iKey].fPressed = false;
        }
    }
}
