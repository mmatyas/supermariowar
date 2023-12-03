#ifndef MENU_SOUNDOPTIONS_H
#define MENU_SOUNDOPTIONS_H

#include "uimenu.h"

class MI_AnnouncerField;
class MI_Button;
class MI_Image;
class MI_SelectField;
class MI_SliderField;
class MI_PacksField;
class MI_PlaylistField;
class MI_Text;

/*
    Description.
*/

class UI_SoundOptionsMenu : public UI_Menu
{
public:
    UI_SoundOptionsMenu();
    ~UI_SoundOptionsMenu();

    short GetCurrentWorldMusicID();

private:
	MI_SliderField * miSoundVolumeField;
	MI_SliderField * miMusicVolumeField;
	MI_SelectField * miPlayNextMusicField;
	MI_AnnouncerField * miAnnouncerField;
	MI_PlaylistField * miPlaylistField;
	MI_SelectField * miWorldMusicField;
	MI_PacksField * miSoundPackField;
	MI_Button * miSoundOptionsMenuBackButton;

	MI_Image * miSoundOptionsMenuLeftHeaderBar;
	MI_Image * miSoundOptionsMenuRightHeaderBar;
	MI_Text * miSoundOptionsMenuHeaderText;
};

#endif // MENU_SOUNDOPTIONS_H
