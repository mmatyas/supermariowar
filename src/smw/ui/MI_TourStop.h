#pragma once

#include "uicontrol.h"

#include <memory>

class MI_Button;
class MI_Image;
class MI_ImageSelectField;
class MI_MapField;
class MI_Text;
template<typename T> class MI_SelectFieldDyn;


class MI_TourStop : public UI_Control {
public:
    MI_TourStop(short x, short y, bool fWorld);

    MenuCodeEnum Modify(bool fModify) override;
    void Update() override;
    void Draw() override;

    void Refresh(short iTourStop);

private:
    std::unique_ptr<MI_ImageSelectField> miModeField;
    std::unique_ptr<MI_SelectFieldDyn<short>> miGoalField;
    std::unique_ptr<MI_SelectFieldDyn<short>> miPointsField;
    std::unique_ptr<MI_MapField> miMapField;
    std::unique_ptr<MI_Button> miStartButton;

    std::unique_ptr<MI_SelectFieldDyn<short>> miBonusField;
    std::array<std::unique_ptr<MI_Image>, 2> miEndStageImage;

    std::array<std::unique_ptr<MI_Image>, 10> miBonusIcon;
    std::array<std::unique_ptr<MI_Image>, 10> miBonusBackground;

    std::unique_ptr<MI_Image> miTourStopLeftHeaderBar;
    std::unique_ptr<MI_Image> miTourStopMenuRightHeaderBar;
    std::unique_ptr<MI_Text> miTourStopMenuHeaderText;

    bool fIsWorld = false;
};
