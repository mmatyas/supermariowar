#ifndef MI_STRING_SCROLL_H
#define MI_STRING_SCROLL_H

#include "uicontrol.h"

#include <string>

class StringScrollElement {
public:
	StringScrollElement(const std::string&);

	void draw(int x, int y, unsigned max_w);

private:
	std::string str;
};

class MI_StringScroll : public UI_Control
{
	public:
		MI_StringScroll(gfxSprite * nspr, short x, short y, short width, short numlines);
		virtual ~MI_StringScroll();

		void Update();
		void Draw();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);

		void Add(const std::string&);

		bool MoveNext();
		bool MovePrev();

		void ClearItems();

		void SetAcceptCode(const MenuCodeEnum);
		void SetCancelCode(const MenuCodeEnum);

		void Activate();
		void Deactivate();

		unsigned short CurrentIndex() const;

	private:
		std::vector<StringScrollElement> items;
		std::vector<StringScrollElement>::iterator current;

		gfxSprite * spr;

		const short iNumLines;
		short iSelectedLine;
		unsigned short iIndex;
		const short iWidth;
		short iOffset;

		const short iTopStop;
		short iBottomStop;

        std::string sTitle;

        MenuCodeEnum iAcceptCode;
        MenuCodeEnum iCancelCode;
};

#endif // MI_STRING_SCROLL_H
