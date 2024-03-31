#pragma once

#include "ui/MI_SelectField.h"


class MI_SliderField : public MI_SelectField<short> {
public:
    MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, const char * name, short width, short indent1, short indent2);
    virtual ~MI_SliderField();

    void SetPosition(short x, short y);

    //Draws every frame
    void Draw() override;
    //Sends player input to control on every frame
    MenuCodeEnum SendInput(CPlayerInput * playerInput) override;

protected:
    gfxSprite* m_sprSlider;
    short m_indent2;
};
