#include "uicontrol.h"

class NLS_ListItem {
public:
    NLS_ListItem();
    NLS_ListItem(const std::string& left, const std::string& right);
    ~NLS_ListItem();

    std::string sLeft;  // Left-aligned text (eg. name)
    std::string sRight; // Right-aligned text (eg. player count)
};

class MI_NetworkListScroll : public UI_Control
{
    public:

        MI_NetworkListScroll(gfxSprite * nspr, short x, short y, short width, short numlines, const std::string& title, MenuCodeEnum acceptCode, MenuCodeEnum cancelCode);
        virtual ~MI_NetworkListScroll();

        void Update();
        void Draw();

        MenuCodeEnum SendInput(CPlayerInput * playerInput);
        MenuCodeEnum Modify(bool modify);

        void Add(const std::string& left, const std::string& right);
        void Clear();

        bool MoveNext();
        bool MovePrev();
        void RemoteIndex(unsigned short * index);

    protected:

        std::vector<NLS_ListItem> items;

        gfxSprite * spr;
        MenuCodeEnum iAcceptCode;
        MenuCodeEnum iCancelCode;

        std::string sTitle;
        short iNumLines;
        short iSelectedLine;
        short iSelectedLineBackup;
        unsigned short iIndex;
        unsigned short * iRemoteIndex;
        short iWidth;
        short iOffset;

        short iTopStop;
        short iBottomStop;
};
