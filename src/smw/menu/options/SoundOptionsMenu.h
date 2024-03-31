#pragma once

#include "uimenu.h"

class MI_AnnouncerField;
class MI_Button;
class MI_Image;
class MI_SliderField;
class MI_PacksField;
class MI_PlaylistField;
class MI_Text;
template<typename T> class MI_SelectField;


class UI_SoundOptionsMenu : public UI_Menu {
public:
    UI_SoundOptionsMenu();

    short GetCurrentWorldMusicID() const;

private:
    MI_SliderField* miSoundVolumeField;
    MI_SliderField* miMusicVolumeField;
    MI_SelectField<bool>* miPlayNextMusicField;
    MI_AnnouncerField* miAnnouncerField;
    MI_PlaylistField* miPlaylistField;
    MI_SelectField<short>* miWorldMusicField;
    MI_PacksField* miSoundPackField;
    MI_Button* miSoundOptionsMenuBackButton;

    MI_Image* miSoundOptionsMenuLeftHeaderBar;
    MI_Image* miSoundOptionsMenuRightHeaderBar;
    MI_Text* miSoundOptionsMenuHeaderText;
};
