#pragma once

#include "EditorBase.h"


class EditorDisplayHelp : public EditorBase {
protected:
    bool isSetupTransparent() const override { return true; }

    void renderSetup(CResourceManager& rm) override;
    void onSetupKeypress(const SDL_KeyboardEvent& event) override;
};
