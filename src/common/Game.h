#pragma once


/// Creates the user-writable settings directory, if it doesn't exist yet.
void ensureSettingsDir();


class App {
public:
    static constexpr int screenWidth = 640;
    static constexpr int screenHeight = 480;
    static constexpr int menuTransparency = 72;
};
