#include "SoundOptionsMenu.h"

#include "FileList.h"
#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_AnnouncerField.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_PacksField.h"
#include "ui/MI_PlaylistField.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_SliderField.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;
extern CGameValues game_values;

extern AnnouncerList* announcerlist;
extern MusicList* musiclist;
extern WorldMusicList* worldmusiclist;
extern SoundsList* soundpacklist;

UI_SoundOptionsMenu::UI_SoundOptionsMenu()
    : UI_Menu()
{
    miSoundVolumeField = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 70, 100, "Sound Volume", 500, 220, 484);
    miSoundVolumeField->add("Off", 0);
    miSoundVolumeField->add("1", 16);
    miSoundVolumeField->add("2", 32);
    miSoundVolumeField->add("3", 48);
    miSoundVolumeField->add("4", 64);
    miSoundVolumeField->add("5", 80);
    miSoundVolumeField->add("6", 96);
    miSoundVolumeField->add("7", 112);
    miSoundVolumeField->add("Max", 128);
    miSoundVolumeField->setOutputPtr(&game_values.soundvolume);
    miSoundVolumeField->setCurrentValue(game_values.soundvolume);
    miSoundVolumeField->allowWrap(false);
    miSoundVolumeField->setItemChangedCode(MENU_CODE_SOUND_VOLUME_CHANGED);

    miMusicVolumeField = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 70, 140, "Music Volume", 500, 220, 484);
    miMusicVolumeField->add("Off", 0);
    miMusicVolumeField->add("1", 16);
    miMusicVolumeField->add("2", 32);
    miMusicVolumeField->add("3", 48);
    miMusicVolumeField->add("4", 64);
    miMusicVolumeField->add("5", 80);
    miMusicVolumeField->add("6", 96);
    miMusicVolumeField->add("7", 112);
    miMusicVolumeField->add("Max", 128);
    miMusicVolumeField->setOutputPtr(&game_values.musicvolume);
    miMusicVolumeField->setCurrentValue(game_values.musicvolume);
    miMusicVolumeField->allowWrap(false);
    miMusicVolumeField->setItemChangedCode(MENU_CODE_MUSIC_VOLUME_CHANGED);

    miPlayNextMusicField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 180, "Next Music", 500, 220);
    miPlayNextMusicField->add("Off", false);
    miPlayNextMusicField->add("On", true);
    miPlayNextMusicField->setOutputPtr(&game_values.playnextmusic);
    miPlayNextMusicField->setCurrentValue(game_values.playnextmusic ? 1 : 0);
    miPlayNextMusicField->setAutoAdvance(true);

    miAnnouncerField = new MI_AnnouncerField(&rm->spr_selectfield, 70, 220, "Announcer", 500, 220, announcerlist);
    miSoundPackField = new MI_PacksField(&rm->spr_selectfield, 70, 260, "Sound Pack", 500, 220, soundpacklist, MENU_CODE_SOUND_PACK_CHANGED);

    miPlaylistField = new MI_PlaylistField(&rm->spr_selectfield, 70, 300, "Game Music Pack", 500, 220);
    miWorldMusicField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 340, "World Music Pack", 500, 220);

    int iCurrentMusic = worldmusiclist->currentIndex();
    worldmusiclist->setCurrent(0);
    for (short iMusic = 0; iMusic < worldmusiclist->count(); iMusic++) {
        miWorldMusicField->add(worldmusiclist->currentName(), iMusic);
        worldmusiclist->next();
    }
    miWorldMusicField->setCurrentValue(iCurrentMusic);
    worldmusiclist->setCurrent(iCurrentMusic);
    miWorldMusicField->setItemChangedCode(MENU_CODE_WORLD_MUSIC_CHANGED);

    miSoundOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miSoundOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miSoundOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miSoundOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miSoundOptionsMenuHeaderText = new MI_HeaderText("Sound Options Menu", 320, 5);

    AddControl(miSoundVolumeField, miSoundOptionsMenuBackButton, miMusicVolumeField, NULL, miSoundOptionsMenuBackButton);
    AddControl(miMusicVolumeField, miSoundVolumeField, miPlayNextMusicField, NULL, miSoundOptionsMenuBackButton);
    AddControl(miPlayNextMusicField, miMusicVolumeField, miAnnouncerField, NULL, miSoundOptionsMenuBackButton);
    AddControl(miAnnouncerField, miPlayNextMusicField, miSoundPackField, NULL, miSoundOptionsMenuBackButton);
    AddControl(miSoundPackField, miAnnouncerField, miPlaylistField, NULL, miSoundOptionsMenuBackButton);
    AddControl(miPlaylistField, miSoundPackField, miWorldMusicField, NULL, miSoundOptionsMenuBackButton);
    AddControl(miWorldMusicField, miPlaylistField, miSoundOptionsMenuBackButton, NULL, miSoundOptionsMenuBackButton);
    AddControl(miSoundOptionsMenuBackButton, miWorldMusicField, miSoundVolumeField, miSoundPackField, NULL);

    AddNonControl(miSoundOptionsMenuLeftHeaderBar);
    AddNonControl(miSoundOptionsMenuRightHeaderBar);
    AddNonControl(miSoundOptionsMenuHeaderText);

    setInitialFocus(miSoundVolumeField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};


short UI_SoundOptionsMenu::GetCurrentWorldMusicID() const
{
    return miWorldMusicField->currentValue();
}
