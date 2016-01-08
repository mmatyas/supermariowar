#ifndef UI_SLIDER_FIELD_H
#define UI_SLIDER_FIELD_H

#include "ui/MI_SelectField.h"

class MI_SliderField : public MI_SelectField
{
public:

    MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, const char * name, short width, short indent1, short indent2);
    virtual ~MI_SliderField();

    void SetPosition(short x, short y);

    //Draws every frame
    virtual void Draw();
    //Sends player input to control on every frame
    MenuCodeEnum SendInput(CPlayerInput * playerInput);

protected:

    gfxSprite * sprSlider;
    short iIndent2;

};

#endif // UI_SLIDER_FIELD_H
