#include "MI_TextField.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"

#include <array>

extern CGameValues game_values;
extern CResourceManager* rm;


namespace {
constexpr std::array<short, 10> NUMBER_KEY_MAP {41, 33, 64, 35, 36, 37, 94, 38, 42, 40};
} // namespace


MI_TextField::MI_TextField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent)
    : UI_Control(x, y)
    , spr(nspr)
    , szName(name)
    , iWidth(width)
    , iIndent(indent)
    , iAdjustmentY(width > 256 ? 0 : 128)
    , iAllowedWidth(iWidth - iIndent - 24)
{
    miModifyCursor = std::make_unique<MI_Image>(nspr, m_pos.x + indent, m_pos.y + 4, 136, 64, 15, 24, 4, 1, 8);
    miModifyCursor->SetBlink(true, 20);
    miModifyCursor->setVisible(false);
}

void MI_TextField::SetTitle(std::string name)
{
    szName = std::move(name);
}

MenuCodeEnum MI_TextField::Modify(bool modify)
{
    if (fDisable)
        return MENU_CODE_UNSELECT_ITEM;

    if (MENU_CODE_NONE != mcControlSelectedCode)
        return mcControlSelectedCode;

    miModifyCursor->setVisible(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

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
            miModifyCursor->setVisible(false);

            fModifying = false;

            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    if (!szOutValue || iNumChars >= iMaxChars)
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
                    key = NUMBER_KEY_MAP[key - 48];
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
                    szOutValue[iCopy + 1] = szOutValue[iCopy];
                }

                szOutValue[iCursorIndex++] = (char)key;

                UpdateCursor();
                return mcItemChangedCode;
            }
        }
    } else if (key == SDLK_BACKSPACE) {
        if (iCursorIndex > 0) {
            iCursorIndex--;
            iNumChars--;
            for (short iCopy = iCursorIndex; iCopy < iNumChars; iCopy++) {
                szOutValue[iCopy] = szOutValue[iCopy + 1];
            }
            szOutValue[iNumChars] = 0;

            UpdateCursor();
            return mcItemChangedCode;
        }
    } else if (key == SDLK_DELETE) {
        if (iCursorIndex < iNumChars) {
            for (short iCopy = iCursorIndex; iCopy < iNumChars; iCopy++) {
                szOutValue[iCopy] = szOutValue[iCopy + 1];
            }

            szOutValue[--iNumChars] = 0;

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
    if (!m_visible)
        return;

    spr->draw(m_pos.x, m_pos.y, 0, (fSelected ? 32 : 0) + iAdjustmentY, iIndent - 16, 32);
    spr->draw(m_pos.x + iIndent - 16, m_pos.y, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(m_pos.x + iIndent + 16, m_pos.y, 528 - iWidth + iIndent, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(m_pos.x + 16, m_pos.y + 5, iIndent - 8, szName.c_str());

    if (szOutValue) {
        if (iStringWidth <= iAllowedWidth || !fModifying) {
            rm->menu_font_large.drawChopRight(m_pos.x + iIndent + 8, m_pos.y + 5, iAllowedWidth, szOutValue);
        } else {
            rm->menu_font_large.drawChopLeft(m_pos.x + iWidth - 16, m_pos.y + 5, iAllowedWidth, szTempValue.c_str());
        }
    }

    miModifyCursor->Draw();
}

void MI_TextField::SetData(char* data, short maxchars)
{
    iMaxChars = maxchars;
    szOutValue = data;
    iCursorIndex = strlen(szOutValue);
    iNumChars = iCursorIndex;

    szTempValue.clear();
    szTempValue.reserve(iMaxChars);

    UpdateCursor();
}

MenuCodeEnum MI_TextField::MouseClick(short iMouseX, short iMouseY)
{
    if (!szOutValue || fDisable)
        return MENU_CODE_NONE;

    //If we are modifying this control, see if we clicked on a next/prev button
    if (fModifying) {
        //Move cursor to index in string where clicked
        short iPixelCount = 0;
        char szChar[2];
        szChar[1] = 0;
        for (short iChar = 0; iChar < iNumChars; iChar++) {
            szChar[0] = szOutValue[iChar];
            iPixelCount += rm->menu_font_large.getWidth(szChar);

            if (iPixelCount >= iMouseX - (m_pos.x + iIndent + 8)) {
                iCursorIndex = iChar;
                UpdateCursor();
                return MENU_CODE_NONE;
            }
        }
    }

    //Otherwise just check to see if we clicked on the whole control
    if (iMouseX >= m_pos.x && iMouseX < m_pos.x + iWidth && iMouseY >= m_pos.y && iMouseY < m_pos.y + 32) {
        iCursorIndex = strlen(szOutValue);
        UpdateCursor();
        return MENU_CODE_CLICKED;
    }

    //Otherwise this control wasn't clicked at all
    return MENU_CODE_NONE;
}

void MI_TextField::Refresh()
{
    //Look at destination string and update control based on that value
    if (!szOutValue)
        return;

    SetData(szOutValue, iMaxChars);
}

void MI_TextField::UpdateCursor()
{
    if (!szOutValue)
        return;

    szTempValue = szOutValue;
    szTempValue.resize(iCursorIndex);

    iStringWidth = rm->menu_font_large.getWidth(szTempValue.c_str());
    if (iStringWidth <= iAllowedWidth) {
        miModifyCursor->SetPosition(m_pos.x + iIndent + 10 + iStringWidth, m_pos.y + 4);
    } else {
        miModifyCursor->SetPosition(m_pos.x + iIndent + 10 + iAllowedWidth, m_pos.y + 4);
    }
}

void MI_TextField::SetDisallowedChars(const char * chars)
{
    strncpy(szDisallowedChars, chars, 31);
    szDisallowedChars[31] = 0;
}
