#include "uicontrol.h"

#include "GameValues.h"
#include "ResourceManager.h"

#include <cmath>
#include <cstring>

extern CGameValues game_values;
extern CResourceManager* rm;

UI_Control::UI_Control(short x, short y)
{
    fSelected = false;
    fModifying = false;
    fAutoModify = false;
    fDisable = false;

    ix = x;
    iy = y;

    fShow = true;

    for (int iNeighbor = 0; iNeighbor < 4; iNeighbor++)
        neighborControls[iNeighbor] = NULL;

    uiMenu = NULL;

    iControllingTeam = -1;
}

UI_Control& UI_Control::operator= (const UI_Control& other)
{
    if (this != &other) {
        fSelected = other.fSelected;
        fModifying = other.fModifying;
        fAutoModify = other.fAutoModify;
        fDisable = other.fDisable;

        ix = other.ix;
        iy = other.iy;

        fShow = other.fShow;

        for (int iNeighbor = 0; iNeighbor < 4; iNeighbor++)
            neighborControls[iNeighbor] = NULL;

        uiMenu = NULL;
        iControllingTeam = other.iControllingTeam;
    }
    return *this;
}

UI_Control::UI_Control(const UI_Control& other)
{
    *this = other;
}

/**************************************
 * MI_IPField Class
 **************************************/
MI_IPField::MI_IPField(gfxSprite * nspr, short x, short y) :
    UI_Control(x, y)
{
    spr = nspr;

    iSelectedDigit = 0;

    for (short k = 0; k < 12; k++)
        values[k] = 0;

    iDigitPosition[0] = 16;
    iDigitPosition[1] = 34;
    iDigitPosition[2] = 52;

    iDigitPosition[3] = 80;
    iDigitPosition[4] = 98;
    iDigitPosition[5] = 116;

    iDigitPosition[6] = 144;
    iDigitPosition[7] = 162;
    iDigitPosition[8] = 180;

    iDigitPosition[9] = 208;
    iDigitPosition[10] = 226;
    iDigitPosition[11] = 244;

    miModifyImage = new MI_Image(nspr, ix + iDigitPosition[0] - 22, iy - 6, 0, 102, 60, 61, 4, 1, 8);
    miModifyImage->Show(false);
}

MI_IPField::~MI_IPField()
{
    delete miModifyImage;
}

char * MI_IPField::GetValue()
{
    sprintf(szValue, "%d%d%d.%d%d%d.%d%d%d.%d%d%d",
            values[0], values[1], values[2],
            values[3], values[4], values[5],
            values[6], values[7], values[8],
            values[9], values[10], values[11]);

    return szValue;
}

MenuCodeEnum MI_IPField::Modify(bool modify)
{
    miModifyImage->Show(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_IPField::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_right.fPressed) {
            if (++iSelectedDigit > 11)
                iSelectedDigit = 0;

            MoveImage();
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed) {
            if (--iSelectedDigit < 0)
                iSelectedDigit = 11;

            MoveImage();
        }

        if (playerInput->outputControls[iPlayer].menu_up.fPressed) {
            values[iSelectedDigit]++;
            AssignHostAddress();
        }

        if (playerInput->outputControls[iPlayer].menu_down.fPressed) {
            values[iSelectedDigit]--;
            AssignHostAddress();
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyImage->Show(false);
            fModifying = false;
            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    return MENU_CODE_NONE;
}

void MI_IPField::AssignHostAddress()
{
    if (iSelectedDigit == 0 || iSelectedDigit == 3 || iSelectedDigit == 6 || iSelectedDigit == 9) {
        if (values[iSelectedDigit] > 2)
            values[iSelectedDigit] = 0;
        else if (values[iSelectedDigit] < 0)
            values[iSelectedDigit] = 2;
    } else {
        if (values[iSelectedDigit] > 9)
            values[iSelectedDigit] = 0;
        else if (values[iSelectedDigit] < 0)
            values[iSelectedDigit] = 9;
    }
}

void MI_IPField::MoveImage()
{
    miModifyImage->SetPosition(ix + iDigitPosition[iSelectedDigit] - 22, iy - 6);
}

void MI_IPField::Update()
{
    miModifyImage->Update();
}

void MI_IPField::Draw()
{
    if (!fShow)
        return;

    if (fSelected)
        spr->draw(ix, iy, 0, 51, 278, 51);
    else
        spr->draw(ix, iy, 0, 0, 278, 51);

    miModifyImage->Draw();

    for (int iSection = 0; iSection < 12; iSection += 3) {
        if (values[iSection] != 0)
            rm->menu_font_large.drawf(ix + iDigitPosition[iSection], iy + 12, "%i", values[iSection]);

        if (values[iSection] != 0 || values[iSection + 1] != 0)
            rm->menu_font_large.drawf(ix + iDigitPosition[iSection + 1], iy + 12, "%i", values[iSection + 1]);

        rm->menu_font_large.drawf(ix + iDigitPosition[iSection + 2], iy + 12, "%i", values[iSection + 2]);
    }
}

MenuCodeEnum MI_IPField::MouseClick(short iMouseX, short iMouseY)
{
    if (fDisable)
        return MENU_CODE_NONE;

    if (iMouseX >= ix && iMouseX < ix + 278 && iMouseY >= iy && iMouseY < iy + 51) {
        return MENU_CODE_CLICKED;
    }

    return MENU_CODE_NONE;
}

/**************************************
 * MI_Button Class
 **************************************/

MI_Button::MI_Button(gfxSprite * nspr, short x, short y, const char * name, short width, short justified) :
    UI_Control(x, y)
{
    spr = nspr;

    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    iWidth = width;
    iTextJustified = justified;
    fSelected = false;
    menuCode = MENU_CODE_NONE;

    sprImage = NULL;
    iImageSrcX = 0;
    iImageSrcY = 0;
    iImageW = 0;
    iImageH = 0;

    iTextW = (short)rm->menu_font_large.getWidth(name);

    iAdjustmentY = width > 256 ? 0 : 128;
    iHalfWidth = width >> 1;

    onPressFn = [](){}; // do nothing
}

MI_Button::~MI_Button()
{
    delete [] szName;
}

MenuCodeEnum MI_Button::Modify(bool)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    onPressFn();
    return menuCode;
}

MenuCodeEnum MI_Button::SendInput(CPlayerInput *)
{
    //If input is being sent, that means the button is selected i.e. clicked
    onPressFn();
    return menuCode;
}

void MI_Button::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iHalfWidth, 32);
    spr->draw(ix + iHalfWidth, iy, 512 - iWidth + iHalfWidth, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iHalfWidth, 32);

    if (0 == iTextJustified) {
        rm->menu_font_large.drawChopRight(ix + 16 + (iImageW > 0 ? iImageW + 2 : 0), iy + 5, iWidth - 32, szName);

        if (sprImage)
            sprImage->draw(ix + 16, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
    } else if (1 == iTextJustified) {
        rm->menu_font_large.drawCentered(ix + ((iWidth + (iImageW > 0 ? iImageW + 2 : 0)) >> 1), iy + 5, szName);

        if (sprImage)
            sprImage->draw(ix + (iWidth >> 1) - ((iTextW + iImageW) >> 1) - 1, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
    } else {
        rm->menu_font_large.drawRightJustified(ix + iWidth - 16, iy + 5, szName);

        if (sprImage)
            sprImage->draw(ix + iWidth - 18 - iTextW - iImageW, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
    }
}

void MI_Button::SetName(const char * name)
{
    delete [] szName;
    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    iTextW = (short)rm->menu_font_large.getWidth(name);
}

void MI_Button::SetImage(gfxSprite * nsprImage, short x, short y, short w, short h)
{
    sprImage = nsprImage;
    iImageSrcX = x;
    iImageSrcY = y;
    iImageW = w;
    iImageH = h;
}

MenuCodeEnum MI_Button::MouseClick(short iMouseX, short iMouseY)
{
    if (fDisable)
        return MENU_CODE_NONE;

    if (iMouseX >= ix && iMouseX < ix + iWidth && iMouseY >= iy && iMouseY < iy + 32) {
        return menuCode;
    }

    return MENU_CODE_NONE;
}

void MI_Button::SetOnPress(std::function<void()>&& func)
{
    onPressFn = func;
}

/**************************************
 * MI_Image Class
 **************************************/
MI_Image::MI_Image(gfxSprite * nspr, short x, short y, short srcx, short srcy, short w, short h, short numxframes, short numyframes, short speed) :
    UI_Control(x, y)
{
    spr = nspr;
    isrcx = srcx;
    isrcy = srcy;
    iw = w;
    ih = h;

    iNumXFrames = numxframes;
    iNumYFrames = numyframes;
    iSpeed = speed;

    iTimer = 0;
    iXFrame = srcx;
    iYFrame = srcy;

    fPulse = false;
    iPulseValue = 0;
    fPulseOut = true;
    iPulseDelay = 0;

    fSwirl = false;
    dSwirlRadius = 0.0f;
    dSwirlAngle = 0.0f;
    dSwirlRadiusSpeed = 0.0f;
    dSwirlAngleSpeed = 0.0f;

    fBlink = false;
    iBlinkInterval = 0;
    iBlinkCounter = 0;
    fBlinkShow = true;
}

MI_Image::~MI_Image()
{}

void MI_Image::Update()
{
    if (!fShow)
        return;

    if (iSpeed > 0 && ++iTimer >= iSpeed) {
        iTimer = 0;
        iXFrame += iw;

        if (iXFrame >= iNumXFrames * iw + isrcx) {
            iXFrame = isrcx;
            iYFrame += ih;

            if (iYFrame >= iNumYFrames * ih + isrcy) {
                iYFrame = isrcy;
            }
        }
    }

    if (fPulse) {
        if (++iPulseDelay >= 3) {
            iPulseDelay = 0;

            if (fPulseOut) {
                if (++iPulseValue >= 10) {
                    fPulseOut = false;
                }
            } else {
                if (--iPulseValue <= 0) {
                    fPulseOut = true;
                }
            }
        }
    }

    if (fSwirl) {
        dSwirlRadius -= dSwirlRadiusSpeed;

        if (dSwirlRadius <= 0.0f) {
            fSwirl = false;
        } else {
            dSwirlAngle += dSwirlAngleSpeed;
        }
    }

    if (fBlink) {
        if (++iBlinkCounter > iBlinkInterval) {
            fBlinkShow = !fBlinkShow;
            iBlinkCounter = 0;
        }
    }
}

void MI_Image::Draw()
{
    if (!fShow || (fBlink && !fBlinkShow))
        return;

    short iXOffset = 0;
    short iYOffset = 0;

    if (fSwirl) {
        iXOffset = (short)(dSwirlRadius * cos(dSwirlAngle));
        iYOffset = (short)(dSwirlRadius * sin(dSwirlAngle));
    }

    if (fPulse)
        spr->drawStretch(ix - iPulseValue + iXOffset, iy - iPulseValue + iYOffset, iw + (iPulseValue << 1), ih + (iPulseValue << 1), iXFrame, iYFrame, iw, ih);
    else
        spr->draw(ix + iXOffset, iy + iYOffset, iXFrame, iYFrame, iw, ih);
}


/**************************************
 * MI_Text Class
 **************************************/

MI_Text::MI_Text(const char * text, short x, short y, short w, short size, short justified) :
    UI_Control(x, y)
{
    szText = new char[strlen(text) + 1];
    strcpy(szText, text);

    iw = w;
    iJustified = justified;

    if (size == 0)
        font = &rm->menu_font_small;
    else
        font = &rm->menu_font_large;
}

MI_Text::~MI_Text()
{
    delete [] szText;
}

void MI_Text::SetText(const char * text)
{
    delete [] szText;
    szText = new char[strlen(text) + 1];
    strcpy(szText, text);
}

void MI_Text::Draw()
{
    if (!fShow)
        return;

    if (iJustified == 0 && iw == 0)
        font->draw(ix, iy, szText);
    else if (iJustified == 0)
        font->drawChopRight(ix, iy, iw, szText);
    else if (iJustified == 1)
        font->drawCentered(ix, iy, szText);
    else if (iJustified == 2)
        font->drawRightJustified(ix, iy, szText);
}

/**************************************
 * MI_ScoreText Class
 **************************************/

MI_ScoreText::MI_ScoreText(short x, short y) :
    UI_Control(x, y)
{
    iScore = 0;

    iDigitLeftSrcX = 0;
    iDigitMiddleSrcX = 0;
    iDigitRightSrcX = 0;

    iDigitLeftDstX = 0;
    iDigitMiddleDstX = 0;
    iDigitRightDstX = 0;
}

void MI_ScoreText::Draw()
{
    if (!fShow)
        return;

    rm->spr_scoretext.draw(iDigitRightDstX, iy, iDigitRightSrcX, 0, 16, 16);

    if (iDigitLeftSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, iy, iDigitMiddleSrcX, 0, 16, 16);
        rm->spr_scoretext.draw(iDigitLeftDstX, iy, iDigitLeftSrcX, 0, 16, 16);
    } else if (iDigitMiddleSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, iy, iDigitMiddleSrcX, 0, 16, 16);
    }
}

void MI_ScoreText::SetScore(short sScore)
{
    short iDigits = sScore;
    while (iDigits > 999)
        iDigits -= 1000;

    iDigitLeftSrcX = iDigits / 100 * 16;
    iDigitMiddleSrcX = iDigits % 100 / 10 * 16;
    iDigitRightSrcX = iDigits % 10 * 16;

    if (iDigitLeftSrcX == 0) {
        if (iDigitMiddleSrcX == 0) {
            iDigitRightDstX = ix - 8;
        } else {
            iDigitMiddleDstX = ix - 16;
            iDigitRightDstX = ix;
        }
    } else {
        iDigitLeftDstX = ix - 24;
        iDigitMiddleDstX = ix - 8;
        iDigitRightDstX = ix + 8;
    }
}


/**************************************
 * MI_TextField Class
 **************************************/

MI_TextField::MI_TextField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent) :
    UI_Control(x, y)
{
    spr = nspr;

    szName = new char[strlen(name) + 1];
    strcpy(szName, name);

    iWidth = width;
    iIndent = indent;

    mcItemChangedCode = MENU_CODE_NONE;
    mcControlSelectedCode = MENU_CODE_NONE;

    szValue = NULL;

    miModifyCursor = new MI_Image(nspr, ix + indent, iy + 4, 136, 64, 15, 24, 4, 1, 8);
    miModifyCursor->SetBlink(true, 20);
    miModifyCursor->Show(false);

    iAdjustmentY = width > 256 ? 0 : 128;

    iMaxChars = 0;
    iNumChars = 0;
    iCursorIndex = 0;

    szTempValue = NULL;

    iStringWidth = 0;
    iAllowedWidth = iWidth - iIndent - 24;
}

MI_TextField::~MI_TextField()
{
    delete miModifyCursor;
    delete [] szName;
    delete [] szTempValue;
}

void MI_TextField::SetTitle(char * name)
{
    delete [] szName;
    szName = new char[strlen(name) + 1];
    strcpy(szName, name);
}

MenuCodeEnum MI_TextField::Modify(bool modify)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    if (MENU_CODE_NONE != mcControlSelectedCode)
        return mcControlSelectedCode;

    miModifyCursor->Show(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

short number_key_map[10] = {41, 33, 64, 35, 36, 37, 94, 38, 42, 40};

MenuCodeEnum MI_TextField::SendInput(CPlayerInput * playerInput)
{
#ifdef USE_SDL2
    const Uint8 * keystate = SDL_GetKeyboardState(NULL);
#else
    Uint8 * keystate = SDL_GetKeyState(NULL);
#endif

    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        // NOTE: copied from UI_Menu::SendInput
        // Only let player 1 on the keyboard control the menu
        if (iPlayer != 0
            && game_values.playerInput.inputControls[iPlayer]
            && game_values.playerInput.inputControls[iPlayer]->iDevice == DEVICE_KEYBOARD) {
            continue;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyCursor->Show(false);

            fModifying = false;

            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    if (!szValue || iNumChars >= iMaxChars)
        return MENU_CODE_NONE;

    // TODO: check string conversion
    //Watch for characters typed in including delete and backspace
    SDL_KEYTYPE key = playerInput->iPressedKey;
    if ((key >= SDLK_a && key <= SDLK_z) || key == SDLK_SPACE || (key >= SDLK_0 && key <= SDLK_9) || key == SDLK_EQUALS ||
            key == SDLK_MINUS || key == SDLK_BACKQUOTE || (key >= SDLK_LEFTBRACKET && key <= SDLK_RIGHTBRACKET) ||
            key == SDLK_SEMICOLON || key == SDLK_QUOTE || key == SDLK_COMMA || key == SDLK_PERIOD || key == SDLK_SLASH) {
        if (iNumChars < iMaxChars - 1) {
            //Take care of holding shift to shift the pressed key to another character
#ifdef USE_SDL2
            if (keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]) {
#else
            if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) {
#endif
                if (key >= SDLK_a && key <= SDLK_z) {
                    key -= 32;
                } else if (key >= SDLK_0 && key <= SDLK_9) {
                    key = number_key_map[key - 48];
                } else if (key == SDLK_MINUS) {
                    key = SDLK_UNDERSCORE;
                } else if (key == SDLK_EQUALS) {
                    key = SDLK_PLUS;
                } else if (key == SDLK_BACKQUOTE) {
                    key = 126;
                } else if (key >= SDLK_LEFTBRACKET && key <= SDLK_RIGHTBRACKET) {
                    key += 32;
                } else if (key == SDLK_SEMICOLON) {
                    key = SDLK_COLON;
                } else if (key == SDLK_QUOTE) {
                    key = SDLK_QUOTEDBL;
                } else if (key == SDLK_COMMA) {
                    key = SDLK_LESS;
                } else if (key == SDLK_PERIOD) {
                    key = SDLK_GREATER;
                } else if (key == SDLK_SLASH) {
                    key = SDLK_QUESTION;
                }
            }

            //Check to see if this is an allowed character for this field
            bool fAllowed = true;
            for (short iIndex = 0; iIndex < 32 && szDisallowedChars[iIndex] != 0; iIndex++) {
                if (szDisallowedChars[iIndex] == key) {
                    fAllowed = false;
                    break;
                }
            }

            //If it is an allowed character, then add it to the field
            if (fAllowed) {
                iNumChars++;

                for (short iCopy = iNumChars - 1; iCopy >= iCursorIndex; iCopy--) {
                    szValue[iCopy + 1] = szValue[iCopy];
                }

                szValue[iCursorIndex++] = (char)key;

                UpdateCursor();
                return mcItemChangedCode;
            }
        }
    } else if (key == SDLK_BACKSPACE) {
        if (iCursorIndex > 0) {
            iCursorIndex--;
            iNumChars--;
            for (short iCopy = iCursorIndex; iCopy < iNumChars; iCopy++) {
                szValue[iCopy] = szValue[iCopy + 1];
            }
            szValue[iNumChars] = 0;

            UpdateCursor();
            return mcItemChangedCode;
        }
    } else if (key == SDLK_DELETE) {
        if (iCursorIndex < iNumChars) {
            for (short iCopy = iCursorIndex; iCopy < iNumChars; iCopy++) {
                szValue[iCopy] = szValue[iCopy + 1];
            }

            szValue[--iNumChars] = 0;

            UpdateCursor();
            return mcItemChangedCode;
        }
    } else if (key == SDLK_LEFT) {
        if (iCursorIndex > 0) {
            iCursorIndex--;
            UpdateCursor();
        }
    } else if (key == SDLK_RIGHT) {
        if (iCursorIndex < iNumChars) {
            iCursorIndex++;
            UpdateCursor();
        }
    }

    return MENU_CODE_NONE;
}


void MI_TextField::Update()
{
    miModifyCursor->Update();
}

void MI_TextField::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

    if (szValue) {
        if (iStringWidth <= iAllowedWidth || !fModifying) {
            rm->menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iAllowedWidth, szValue);
        } else {
            rm->menu_font_large.drawChopLeft(ix + iWidth - 16, iy + 5, iAllowedWidth, szTempValue);
        }
    }

    miModifyCursor->Draw();
}

void MI_TextField::SetData(char * data, short maxchars)
{
    iMaxChars = maxchars;
    szValue = data;
    iCursorIndex = strlen(szValue);
    iNumChars = iCursorIndex;

    if (szTempValue)
        delete [] szTempValue;

    szTempValue = new char[iMaxChars];

    UpdateCursor();
}

MenuCodeEnum MI_TextField::MouseClick(short iMouseX, short iMouseY)
{
    if (!szValue || fDisable)
        return MENU_CODE_NONE;

    //If we are modifying this control, see if we clicked on a next/prev button
    if (fModifying) {
        //Move cursor to index in string where clicked
        short iPixelCount = 0;
        char szChar[2];
        szChar[1] = 0;
        for (short iChar = 0; iChar < iNumChars; iChar++) {
            szChar[0] = szValue[iChar];
            iPixelCount += rm->menu_font_large.getWidth(szChar);

            if (iPixelCount >= iMouseX - (ix + iIndent + 8)) {
                iCursorIndex = iChar;
                UpdateCursor();
                return MENU_CODE_NONE;
            }
        }
    }

    //Otherwise just check to see if we clicked on the whole control
    if (iMouseX >= ix && iMouseX < ix + iWidth && iMouseY >= iy && iMouseY < iy + 32) {
        iCursorIndex = strlen(szValue);
        UpdateCursor();
        return MENU_CODE_CLICKED;
    }

    //Otherwise this control wasn't clicked at all
    return MENU_CODE_NONE;
}

void MI_TextField::Refresh()
{
    //Look at destination string and update control based on that value
    if (!szValue)
        return;

    SetData(szValue, iMaxChars);
}

void MI_TextField::UpdateCursor()
{
    if (!szValue)
        return;

    strncpy(szTempValue, szValue, iCursorIndex);
    szTempValue[iCursorIndex] = 0;

    iStringWidth = rm->menu_font_large.getWidth(szTempValue);
    if (iStringWidth <= iAllowedWidth) {
        miModifyCursor->SetPosition(ix + iIndent + 10 + iStringWidth, iy + 4);
    } else {
        miModifyCursor->SetPosition(ix + iIndent + 10 + iAllowedWidth, iy + 4);
    }
}

void MI_TextField::SetDisallowedChars(const char * chars)
{
    strncpy(szDisallowedChars, chars, 31);
    szDisallowedChars[31] = 0;
}
