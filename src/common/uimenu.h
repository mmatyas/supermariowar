#ifndef UIMENU_H
#define UIMENU_H

#include "eyecandy.h"
#include "ui/MenuCode.h"

#include <list>

class UI_Control;
class CPlayerInput;


class UI_Menu
{
	public:

		UI_Menu();
		virtual ~UI_Menu();

		void AddControl(UI_Control * control, UI_Control * up, UI_Control * down, UI_Control * left, UI_Control * right);
		void AddNonControl(UI_Control * control);
	    UI_Control * GetHeadControl() {
	        return headControl;
	    }
			void SetHeadControl(UI_Control * control);
			void ResetMenu();

	    void SetCancelCode(MenuCodeEnum code) {
	        cancelCode = code;
	    }

		void Update();
		void Draw();

		void AddEyeCandy(CEyecandy * ec) {
	        eyeCandy.add(ec);
	    }
	    void ClearEyeCandy() {
	        eyeCandy.clean();
	    }

		void ResetCurrentControl();
		MenuCodeEnum SendInput(CPlayerInput * playerInput);


		void RememberCurrent();
		void RestoreCurrent();

	    UI_Control * GetCurrentControl() {
	        return current;
	    }

	    void SetControllingTeam(short teamid) {
	        iControllingTeam = teamid;
	    }
	    void SetAllowExit(bool allowExit) {
	        fAllowExitButton = allowExit;
	    }

			MenuCodeEnum MouseClick(short iMouseX, short iMouseY);
	    bool IsModifying() {
	        return fModifyingItem;
	    }

		void Refresh();

	protected:

		MenuCodeEnum MoveNextControl(MenuCodeEnum iDirection);

		std::list<UI_Control*> controls;

		UI_Control * current;
		UI_Control * savedCurrent;

		MenuCodeEnum cancelCode;
		bool fModifyingItem;

		UI_Control * headControl;

		CEyecandyContainer eyeCandy;

		short iControllingTeam;
		bool fAllowExitButton;
};

#endif // UIMENU_H
