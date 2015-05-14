#include "SoundOptionsMenu.h"

#include "FileList.h"
#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

extern AnnouncerList *announcerlist;
extern MusicList *musiclist;
extern WorldMusicList *worldmusiclist;
extern SoundsList *soundpacklist;

UI_SoundOptionsMenu::UI_SoundOptionsMenu() : UI_Menu()
{
    miSoundVolumeField = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 70, 100, "Sound Volume", 500, 220, 484);
    miSoundVolumeField->Add("Off", 0, "", false, false);
    miSoundVolumeField->Add("1", 16, "", false, false);
    miSoundVolumeField->Add("2", 32, "", false, false);
    miSoundVolumeField->Add("3", 48, "", false, false);
    miSoundVolumeField->Add("4", 64, "", false, false);
    miSoundVolumeField->Add("5", 80, "", false, false);
    miSoundVolumeField->Add("6", 96, "", false, false);
    miSoundVolumeField->Add("7", 112, "", false, false);
    miSoundVolumeField->Add("Max", 128, "", false, false);
    miSoundVolumeField->SetData(&game_values.soundvolume, NULL, NULL);
    miSoundVolumeField->SetKey(game_values.soundvolume);
    miSoundVolumeField->SetNoWrap(true);
    miSoundVolumeField->SetItemChangedCode(MENU_CODE_SOUND_VOLUME_CHANGED);

    miMusicVolumeField = new MI_SliderField(&rm->spr_selectfield, &rm->menu_slider_bar, 70, 140, "Music Volume", 500, 220, 484);
    miMusicVolumeField->Add("Off", 0, "", false, false);
    miMusicVolumeField->Add("1", 16, "", false, false);
    miMusicVolumeField->Add("2", 32, "", false, false);
    miMusicVolumeField->Add("3", 48, "", false, false);
    miMusicVolumeField->Add("4", 64, "", false, false);
    miMusicVolumeField->Add("5", 80, "", false, false);
    miMusicVolumeField->Add("6", 96, "", false, false);
    miMusicVolumeField->Add("7", 112, "", false, false);
    miMusicVolumeField->Add("Max", 128, "", false, false);
    miMusicVolumeField->SetData(&game_values.musicvolume, NULL, NULL);
    miMusicVolumeField->SetKey(game_values.musicvolume);
    miMusicVolumeField->SetNoWrap(true);
    miMusicVolumeField->SetItemChangedCode(MENU_CODE_MUSIC_VOLUME_CHANGED);

    miPlayNextMusicField = new MI_SelectField(&rm->spr_selectfield, 70, 180, "Next Music", 500, 220);
    miPlayNextMusicField->Add("Off", 0, "", false, false);
    miPlayNextMusicField->Add("On", 1, "", true, false);
    miPlayNextMusicField->SetData(NULL, NULL, &game_values.playnextmusic);
    miPlayNextMusicField->SetKey(game_values.playnextmusic ? 1 : 0);
    miPlayNextMusicField->SetAutoAdvance(true);

    miAnnouncerField = new MI_AnnouncerField(&rm->spr_selectfield, 70, 220, "Announcer", 500, 220, announcerlist);
    miSoundPackField = new MI_PacksField(&rm->spr_selectfield, 70, 260, "Sound Pack", 500, 220, soundpacklist, MENU_CODE_SOUND_PACK_CHANGED);

    miPlaylistField = new MI_PlaylistField(&rm->spr_selectfield, 70, 300, "Game Music Pack", 500, 220);
    miWorldMusicField = new MI_SelectField(&rm->spr_selectfield, 70, 340, "World Music Pack", 500, 220);

    int iCurrentMusic = worldmusiclist->GetCurrentIndex();
    worldmusiclist->SetCurrent(0);
    for (short iMusic = 0; iMusic < worldmusiclist->GetCount(); iMusic++) {
        miWorldMusicField->Add(worldmusiclist->current_name(), iMusic, "", false, false);
        worldmusiclist->next();
    }
    miWorldMusicField->SetKey(iCurrentMusic);
    worldmusiclist->SetCurrent(iCurrentMusic);
    miWorldMusicField->SetItemChangedCode(MENU_CODE_WORLD_MUSIC_CHANGED);

    miSoundOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miSoundOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miSoundOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miSoundOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miSoundOptionsMenuHeaderText = new MI_Text("Sound Options Menu", 320, 5, 0, 2, 1);

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

    SetHeadControl(miSoundVolumeField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};

UI_SoundOptionsMenu::~UI_SoundOptionsMenu() {
}


short UI_SoundOptionsMenu::GetCurrentWorldMusicID()
{
    return miWorldMusicField->GetShortValue();
}
