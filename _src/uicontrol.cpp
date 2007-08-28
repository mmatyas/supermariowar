#include "global.h"
#include <math.h>

extern char * Keynames[340];
extern char * Joynames[30];

extern char * GameInputNames[NUM_KEYS];
extern char * MenuInputNames[NUM_KEYS];

extern bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections);

extern bool __load_gfx(gfxSprite &g, const std::string& f);

extern short iScoreboardPlayerOffsetsX[3][3];
extern WorldMap g_worldmap;
extern short LookupTeamID(short id);

UI_Control::UI_Control(short x, short y)
{
	fSelected = false;
	fModifying = false;
	fAutoModify = false;
	ix = x;
	iy = y;

	fShow = true;

	for(int iNeighbor = 0; iNeighbor < 4; iNeighbor++)
		neighborControls[iNeighbor] = NULL;

	uiMenu = NULL;
}

/**************************************
 * MI_IPField Class
 **************************************/
MI_IPField::MI_IPField(gfxSprite * nspr, short x, short y) :
	UI_Control(x, y)
{
	spr = nspr;
	
	iSelectedDigit = 0;

	for(short k = 0; k < 12; k++)
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
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_right.fPressed)
		{
			if(++iSelectedDigit > 11)
				iSelectedDigit = 0;

			MoveImage();
		}
		
		if(playerInput->outputControls[iPlayer].menu_left.fPressed)
		{
			if(--iSelectedDigit < 0)
				iSelectedDigit = 11;

			MoveImage();
		}

		if(playerInput->outputControls[iPlayer].menu_up.fPressed)
		{
			values[iSelectedDigit]++;
			AssignHostAddress();
		}

		if(playerInput->outputControls[iPlayer].menu_down.fPressed)
		{
			values[iSelectedDigit]--;
			AssignHostAddress();
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			miModifyImage->Show(false);
			fModifying = false;
			return MENU_CODE_UNSELECT_ITEM;
		}
	}

	return MENU_CODE_NONE;
}

void MI_IPField::AssignHostAddress()
{
	if(iSelectedDigit == 0 || iSelectedDigit == 3 || iSelectedDigit == 6 || iSelectedDigit == 9)
	{
		if(values[iSelectedDigit] > 2)
			values[iSelectedDigit] = 0;
		else if(values[iSelectedDigit] < 0)
			values[iSelectedDigit] = 2;
	}
	else
	{
		if(values[iSelectedDigit] > 9)
			values[iSelectedDigit] = 0;
		else if(values[iSelectedDigit] < 0)
			values[iSelectedDigit] = 9;
	}

	game_values.hostaddress = GetValue();
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
	if(!fShow)
		return;

	if(fSelected)
		spr->draw(ix, iy, 0, 51, 278, 51);
	else
		spr->draw(ix, iy, 0, 0, 278, 51);

	miModifyImage->Draw();

	for(int iSection = 0; iSection < 12; iSection += 3)
	{
		if(values[iSection] != 0)
			menu_font_large.drawf(ix + iDigitPosition[iSection], iy + 12, "%i", values[iSection]);

		if(values[iSection] != 0 || values[iSection + 1] != 0)
			menu_font_large.drawf(ix + iDigitPosition[iSection + 1], iy + 12, "%i", values[iSection + 1]);
		
		menu_font_large.drawf(ix + iDigitPosition[iSection + 2], iy + 12, "%i", values[iSection + 2]);
	}
}


/**************************************
 * MI_SelectField Class
 **************************************/

MI_SelectField::MI_SelectField(gfxSprite * nspr, short x, short y, char * name, short width, short indent) :
	UI_Control(x, y)
{
	spr = nspr;
	strcpy(szName, name);
	iWidth = width;
	iIndent = indent;

	mcItemChangedCode = MENU_CODE_NONE;
	mcControlSelectedCode = MENU_CODE_NONE;

	fAutoAdvance = false;
	fNoWrap = false;
	fDisable = false;

	iValue = NULL;
	sValue = NULL;
	fValue = NULL;

	iIndex = 0;

	miModifyImageLeft = new MI_Image(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
	miModifyImageLeft->Show(false);

	miModifyImageRight = new MI_Image(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
	miModifyImageRight->Show(false);

	iAdjustmentY = width > 256 ? 0 : 128;
}

MI_SelectField::~MI_SelectField()
{
	delete miModifyImageLeft;
	delete miModifyImageRight;
}

void MI_SelectField::SetTitle(char * name)
{
	strcpy(szName, name);
}

//Sets current selected item based on the items int value
bool MI_SelectField::SetKey(short iID)
{
	if(items.empty())
		return false;

	std::vector<SF_ListItem*>::iterator search = items.begin();

	iIndex  = 0;
	while(search != items.end())
	{
		if((*search)->iValue == iID)
		{
			current = search;
			return true;
		}

		++search;
		++iIndex;
	}

	return false;
}

bool MI_SelectField::SetIndex(unsigned short index)
{
	if(index >= items.size())
		return false;

	current = items.begin();

	for(unsigned int iSearch = 0; iSearch < index; iSearch++)
	{
		++current;
	}

	iIndex = index;

	return true;
}

void MI_SelectField::Add(std::string name, short ivalue, std::string svalue, bool fvalue, bool fhidden)
{
	SF_ListItem * item = new SF_ListItem(name, ivalue, svalue, fvalue, fhidden);
	items.push_back(item);

	if(items.size() >= 1)
	{
		current = items.begin();
		iIndex = 0;
	}
}

bool MI_SelectField::HideItem(short iID, bool fhide)
{
	if(items.empty())
		return false;

	std::vector<SF_ListItem*>::iterator search = items.begin();

	while(search != items.end())
	{
		if((*search)->iValue == iID)
		{
			if(current == search)
			{
				if(!MoveNext())
					MovePrev();
			}

			(*search)->fHidden = fhide;
			return true;
		}

		++search;
	}

	return false;
}

void MI_SelectField::HideAllItems(bool fHide)
{
	if(items.empty())
		return;

	std::vector<SF_ListItem*>::iterator itr = items.begin();

	while(itr != items.end())
	{
		(*itr)->fHidden = fHide;
		++itr;
	}
}

MenuCodeEnum MI_SelectField::Modify(bool modify)
{
	if(fDisable)
		return MENU_CODE_UNSELECT_ITEM;

	if(fAutoAdvance && modify)
	{
		if(!items.empty())
		{
			if(current == --items.end())
			{
				current = items.begin();
				iIndex = 0;
			}
			else
			{
				++current;
				++iIndex;
			}

			//Saves the values to the pointed to values
			SetValues();
		}

		return mcItemChangedCode;
	}

	if(MENU_CODE_NONE != mcControlSelectedCode)
		return mcControlSelectedCode;

	miModifyImageLeft->Show(modify);
	miModifyImageRight->Show(modify);
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_SelectField::SendInput(CPlayerInput * playerInput)
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed)
		{
			if(MoveNext())
				return mcItemChangedCode;
		}
		
		if(playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed)
		{
			if(MovePrev())
				return mcItemChangedCode;
		}

		if(playerInput->outputControls[iPlayer].menu_random.fPressed)
		{
			if(MoveRandom())
				return mcItemChangedCode;
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			miModifyImageLeft->Show(false);
			miModifyImageRight->Show(false);

			fModifying = false;

			SetValues();

			return MENU_CODE_UNSELECT_ITEM;
		}
	}

	return MENU_CODE_NONE;
}


void MI_SelectField::Update()
{
	miModifyImageRight->Update();
	miModifyImageLeft->Update();
}

void MI_SelectField::Draw()
{
	if(!fShow)
		return;

	spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iIndent - 16, 32);
	spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
	spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iIndent - 16, 32);

	menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

	if(!items.empty())
	{
		menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 24, (*current)->sName.c_str());
	}

	if(current != items.begin() || !fNoWrap)
		miModifyImageLeft->Draw();

	if(current != --items.end() || !fNoWrap)
		miModifyImageRight->Draw();
}

void MI_SelectField::SetValues()
{
	if(iValue)
		*iValue = (*current)->iValue;

	if(sValue)
		*sValue = (*current)->sValue;

	if(fValue)
		*fValue = (*current)->fValue;
}

bool MI_SelectField::MoveNext()
{
	if(items.empty())
		return false;

	std::vector<SF_ListItem*>::iterator findNext = current;
	short iFindIndex = iIndex;

	while(true)
	{
		if(findNext == --items.end())
		{
			if(fNoWrap)
			{
				return false;
			}
			else
			{
				findNext = items.begin();
				iFindIndex = 0;
			}
		}
		else
		{
			findNext++;
			iFindIndex++;
		}

		if(findNext == current)
			return false;

		if(!(*findNext)->fHidden)
		{
			current = findNext;
			iIndex = iFindIndex;
			SetValues();
			return true;
		}
	}
	
	return false;
}

bool MI_SelectField::MovePrev()
{
	if(items.empty())
		return false;

	std::vector<SF_ListItem*>::iterator findPrev = current;
	short iFindIndex = iIndex;

	while(true)
	{
		if(findPrev == items.begin())
		{
			if(fNoWrap)
			{
				return false;
			}
			else
			{
				findPrev = --items.end();
				iFindIndex = (signed short)items.size() - 1;
			}
		}
		else
		{
			findPrev--;
			iFindIndex--;
		}

		if(findPrev == current)
			return false;

		if(!(*findPrev)->fHidden)
		{
			current = findPrev;
			iIndex = iFindIndex;
			SetValues();
			return true;
		}
	}

	return false;
}

bool MI_SelectField::MoveRandom()
{
	//Can't pick a random item from a list of 1
	if(items.size() <= 1)
		return false;

	std::vector<SF_ListItem*>::iterator searchHidden = items.begin();

	short iHiddenCount = 0;
	while(searchHidden != items.end())
	{
		if((*searchHidden)->fHidden)
		{
			iHiddenCount++;
		}

		++searchHidden;
	}

	//Can't pick a random item from a list containing only 1 non-hidden item
	if(items.size() - iHiddenCount <= 1)
		return false;

	std::vector<SF_ListItem*>::iterator findRandom = current;
	short iFindIndex = iIndex;

	while(true)
	{
		int index = rand() % items.size();

		for(int k = 0; k < index; k++)
		{
			if(findRandom == --items.end())
			{
				findRandom = items.begin();
				iFindIndex = 0;
			}
			else
			{
				++findRandom;
				++iFindIndex;
			}
		}

		if(findRandom != current && !(*findRandom)->fHidden)
		{
			current = findRandom;
			iIndex = iFindIndex;
			SetValues();
			return true;
		}
	}
	
	SetValues();
	return true;
}
/**************************************
 * MI_ImageSelectField Class
 **************************************/

MI_ImageSelectField::MI_ImageSelectField(gfxSprite * nspr, gfxSprite * nspr_image, short x, short y, char * name, short width, short indent, short imageHeight, short imageWidth) :
	MI_SelectField(nspr, x, y, name, width, indent)
{
	spr_image = nspr_image;

	iImageWidth = imageWidth;
	iImageHeight = imageHeight;
}

MI_ImageSelectField::~MI_ImageSelectField()
{}

void MI_ImageSelectField::Draw()
{
	if(!fShow)
		return;

	spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
	spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
	spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

	menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

	if(!items.empty())
	{
		menu_font_large.drawChopRight(ix + iIndent + iImageWidth + 10, iy + 5, iWidth - iIndent - 24, (*current)->sName.c_str());
	}

	spr_image->draw(ix + iIndent + 8, iy + 16 - (iImageHeight >> 1), (*current)->iValue * iImageWidth, 0, iImageWidth, iImageHeight);

	miModifyImageRight->Draw();
	miModifyImageLeft->Draw();
}

/**************************************
 * MI_InputControlField Class
 **************************************/

MI_InputControlField::MI_InputControlField(gfxSprite * nspr, short x, short y, char * name, short width, short indent) :
	UI_Control(x, y)
{
	spr = nspr;
	szName = name;
	iWidth = width;
	iIndent = indent;
	fSelected = false;

	iDevice = DEVICE_KEYBOARD;
	iKey = NULL;
	iType = 0;
	iKeyIndex = 0;
	iPlayerIndex = 0;
}

MenuCodeEnum MI_InputControlField::Modify(bool modify)
{
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_InputControlField::SendInput(CPlayerInput *)
{
	SDL_Event event;
	bool done = false;

	while (!done)
	{
		SDL_WaitEvent(&event);

		/*
		if(event.type == SDL_KEYDOWN)
		{
			printf("*********** Key Down ***********\n");
			printf("Keysym: %d\n", event.key.keysym.sym);
			printf("State: %d\n", event.key.state);
			printf("Type: %d\n", event.key.type);
			printf("Which: %d\n\n", event.key.which);
		}
		else if(event.type == SDL_MOUSEMOTION)
		{
			printf("*********** Mouse Motion ***********\n");
			printf("X: %d\n", event.motion.x);
			printf("Y: %d\n", event.motion.y);
			printf("State: %d\n", event.motion.state);
			printf("Type: %d\n", event.motion.type);
			printf("Which: %d\n\n", event.motion.which);
		}
		else if(event.type == SDL_MOUSEBUTTONDOWN)
		{
			printf("*********** Mouse Button ***********\n");
			printf("Button: %d\n", event.button.button);
			printf("State: %d\n", event.button.state);
			printf("Type: %d\n", event.button.type);
			printf("Which: %d\n\n", event.button.which);
		}
		else if(event.type == SDL_JOYHATMOTION)
		{
			printf("*********** Joystick Hat ***********\n");
			printf("Value: %d\n", event.jhat.value);
			printf("Type: %d\n", event.jhat.type);
			printf("Which: %d\n\n", event.jhat.which);
		}
		else if(event.type == SDL_JOYAXISMOTION)
		{
			printf("*********** Joystick Motion ***********\n");
			printf("Value: %d\n", event.jaxis.value);
			printf("Axis: %d\n", event.jaxis.axis);
			printf("Type: %d\n", event.jaxis.type);
			printf("Which: %d\n\n", event.jaxis.which);
		}
		else if(event.type == SDL_JOYBUTTONDOWN)
		{
			printf("*********** Joystick Button ***********\n");
			printf("Button: %d\n", event.jbutton.button);
			printf("State: %d\n", event.jbutton.state);
			printf("Type: %d\n", event.jbutton.type);
			printf("Which: %d\n\n", event.jbutton.which);
		}
		*/

		game_values.playerInput.Update(event, 1);

		//Check to see if user cancelled
		//This was causing trouble with the joystick.  If a user had the menu cancel button
		//configured to something that he wanted to use in game, it would cancel that action.
		//This happened a lot when configuring the in game exit button.
		/*
		for(int iPlayer = 0; iPlayer < 4; iPlayer++)
		{
			if (playerInput->outputControls[iPlayer].menu_cancel.fPressed)
			{
				playerInput->ResetKeys();

				fModifying = false;
				return MENU_CODE_UNSELECT_ITEM;
			}
		}
		*/

		if(iDevice == DEVICE_KEYBOARD)
		{
			if (event.type == SDL_KEYDOWN)
			{
				short key = (short)event.key.keysym.sym;

				SetKey(iKey, key, iDevice);
				done = true;
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				short xmag = (short)abs(event.motion.xrel);
				short ymag = (short)abs(event.motion.yrel);

				if(xmag < MOUSE_X_DEAD_ZONE && ymag < MOUSE_Y_DEAD_ZONE)
					continue;

				short key = KEY_NONE;
				if(xmag > ymag)
				{
					if(event.motion.xrel < 0)
						key = MOUSE_LEFT;
					else
						key = MOUSE_RIGHT;
				}
				else
				{
					if(event.motion.yrel < 0)
						key = MOUSE_UP;
					else
						key = MOUSE_DOWN;
				}

				if(key != KEY_NONE)
				{
					SetKey(iKey, key, iDevice);
					done = true;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				short key = event.button.button + MOUSE_BUTTON_START;
				SetKey(iKey, key, iDevice);
				done = true;
			}
		}
		else
		{
			if (event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_ESCAPE)
				{
					done = true;
				}
			}
			else if(event.type == SDL_JOYHATMOTION)
			{
				short key = KEY_NONE;

				if (event.jhat.value & SDL_HAT_UP)
				{
					key = JOY_HAT_UP;
				}
				else if (event.jhat.value & SDL_HAT_DOWN)
				{
					key = JOY_HAT_DOWN;
				}
				else if (event.jhat.value & SDL_HAT_LEFT)
				{
					key = JOY_HAT_LEFT;
				}
				else if (event.jhat.value & SDL_HAT_RIGHT)
				{
					key = JOY_HAT_RIGHT;
				}

				if(key != KEY_NONE)
				{
					SetKey(iKey, key, iDevice);
					done = true;
				}
			}
			else if (event.type == SDL_JOYAXISMOTION)
			{
				short key = KEY_NONE;

				if(event.jaxis.axis == 0)
				{
					if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						key = JOY_STICK_1_LEFT;
					}
					else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						key = JOY_STICK_1_RIGHT;
					}
				}
				else if(event.jaxis.axis == 1)
				{
					if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						key = JOY_STICK_1_UP;
					}
					else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						key = JOY_STICK_1_DOWN;
					}
				}
				else if(event.jaxis.axis == 2)
				{
					if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						key = JOY_STICK_2_LEFT;
					}
					else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						key = JOY_STICK_2_RIGHT;
					}
				}
				else if(event.jaxis.axis == 3)
				{
					if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						key = JOY_STICK_2_UP;
					}
					else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						key = JOY_STICK_2_DOWN;
					}
				}

				if(key != KEY_NONE)
				{
					SetKey(iKey, key, iDevice);
					done = true;
				}
			}
			else if (event.type == SDL_JOYBUTTONDOWN)
			{
				if(event.jbutton.state == SDL_PRESSED)
				{
					short key = event.jbutton.button + JOY_BUTTON_START;

					SetKey(iKey, key, iDevice);
					done = true;
				}
			}
		}
	}
	
	game_values.playerInput.CheckIfMouseUsed();

	//Need to clear down keys when coming into this (we'll ignore key up events in here)
	game_values.playerInput.ResetKeys();

	fModifying = false;
	return MENU_CODE_UNSELECT_ITEM;
}

void MI_InputControlField::SetKey(short * iSetKey, short key, short device)
{
	bool fNeedSwap = false;
	short iSwapPlayer, iSwapKey;

	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(game_values.playerInput.inputControls[iPlayer]->iDevice != device)
			continue;

		for(int iKey = 0; iKey < NUM_KEYS; iKey++)
		{
			if(iKey == iKeyIndex && iPlayer == iPlayerIndex)
				continue;
		
			if(game_values.playerInput.inputControls[iPlayer]->inputGameControls[iType].keys[iKey] == key)
			{
				fNeedSwap = true;
				iSwapPlayer = iPlayer;
				iSwapKey = iKey;
				break;
			}
		}

		if(fNeedSwap)
			break;
	}

	if(fNeedSwap)
	{
		game_values.playerInput.inputControls[iSwapPlayer]->inputGameControls[iType].keys[iSwapKey] = *iSetKey;
	}

	*iSetKey = key;
}

void MI_InputControlField::Draw()
{
	if(!fShow)
		return;

	spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
	spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
	spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

	menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

	if(iKey == NULL)
		menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 16, "Unassigned");
	else if(fModifying)
		menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 16, "(Press Button)");
	else if(iDevice == DEVICE_KEYBOARD)
		menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 16, Keynames[*iKey]);
	else
		menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 16, Joynames[*iKey]);
}

/**************************************
 * MI_InputControlContainer Class
 **************************************/

//call with x = 94, y = 19
MI_InputControlContainer::MI_InputControlContainer(gfxSprite * spr_button, short x, short y, short playerID) :
	UI_Control(x, y)
{
	iPlayerID = playerID;
	iDevice = game_values.playerInput.inputControls[iPlayerID]->iDevice;
	iSelectedInputType = 0;

	char szTitle[128];
	sprintf(szTitle, "Player %d Controls", iPlayerID + 1);
	miText = new MI_Text(szTitle, 320, 5, 0, 2, 1);

	miImage[0] = new MI_Image(spr_button, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miImage[1] = new MI_Image(spr_button, 320, 0, 192, 0, 320, 32, 1, 1, 0);

	miDeviceSelectField = new MI_SelectField(spr_button, x + 16, y + 38, "Device", 420, 150);
	miDeviceSelectField->SetItemChangedCode(MENU_CODE_INPUT_DEVICE_CHANGED);
	miDeviceSelectField->Add("Keyboard", -1, "", false, false);
#ifdef _XBOX	
	miDeviceSelectField->Disable(true);

	for(int iJoystick = 0; iJoystick < 4; iJoystick++)
	{
		char szJoystickNumber[2];
		sprintf(szJoystickNumber, "%d", iJoystick + 1);
		miDeviceSelectField->Add(std::string("Gamepad ") + std::string(szJoystickNumber), iJoystick, "", false, false);
	}

#else
	for(short iJoystick = 0; iJoystick < joystickcount; iJoystick++)
	{
		miDeviceSelectField->Add(SDL_JoystickName(iJoystick), iJoystick, "", false, false);
	}
#endif

	//If the device is not found, default to the keyboard
	if(!miDeviceSelectField->SetKey(iDevice))
	{
		iDevice = DEVICE_KEYBOARD;
		miDeviceSelectField->SetKey(iDevice);
	}

	miInputTypeButton = new MI_Button(spr_button, x + 336, y + 84, "Game", 100, 1);
	miInputTypeButton->SetCode(MENU_CODE_INPUT_TYPE_CHANGED);


	for(short iKey = 0; iKey < NUM_KEYS; iKey++)
	{
		miGameInputControlFields[iKey] = new MI_InputControlField(spr_button, x + 16, y + 118 + iKey * 34, GameInputNames[iKey], 420, 150);
		miGameInputControlFields[iKey]->SetDevice(iDevice);
		miGameInputControlFields[iKey]->SetType(0);
		miGameInputControlFields[iKey]->SetKeyIndex(iKey);
		miGameInputControlFields[iKey]->SetPlayerIndex(iPlayerID);
		miGameInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[0].keys[iKey]);
	}

	for(short iKey = 0; iKey < NUM_KEYS; iKey++)
	{
		miMenuInputControlFields[iKey] = new MI_InputControlField(spr_button, x + 16, y + 118 + iKey * 34, MenuInputNames[iKey], 420, 150);
		miMenuInputControlFields[iKey]->SetDevice(iDevice);
		miMenuInputControlFields[iKey]->SetType(1);
		miMenuInputControlFields[iKey]->SetKeyIndex(iKey);
		miMenuInputControlFields[iKey]->SetPlayerIndex(iPlayerID);
		miMenuInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[1].keys[iKey]);
	}

	miBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miBackButton->SetCode(MENU_CODE_BACK_TO_CONTROLS_MENU);

	mInputMenu = new UI_Menu();
	mInputMenu->SetCancelCode(MENU_CODE_CANCEL_INPUT);
	mInputMenu->AddNonControl(miImage[0]);
	mInputMenu->AddNonControl(miImage[1]);
	mInputMenu->AddNonControl(miText);

	mInputMenu->AddControl(miDeviceSelectField, miBackButton, miInputTypeButton, NULL, miBackButton);
	mInputMenu->AddControl(miInputTypeButton, miDeviceSelectField, miGameInputControlFields[0], NULL, miBackButton);
	mInputMenu->AddControl(miGameInputControlFields[0], miInputTypeButton, miGameInputControlFields[1], NULL, miBackButton);
	mInputMenu->AddControl(miGameInputControlFields[1], miGameInputControlFields[0], miGameInputControlFields[2], NULL, miBackButton);
	mInputMenu->AddControl(miGameInputControlFields[2], miGameInputControlFields[1], miGameInputControlFields[3], NULL, miBackButton);
	mInputMenu->AddControl(miGameInputControlFields[3], miGameInputControlFields[2], miGameInputControlFields[4], NULL, miBackButton);
	mInputMenu->AddControl(miGameInputControlFields[4], miGameInputControlFields[3], miGameInputControlFields[5], NULL, miBackButton);
	mInputMenu->AddControl(miGameInputControlFields[5], miGameInputControlFields[4], miGameInputControlFields[6], NULL, miBackButton);
	mInputMenu->AddControl(miGameInputControlFields[6], miGameInputControlFields[5], miGameInputControlFields[7], NULL, miBackButton);
	mInputMenu->AddControl(miGameInputControlFields[7], miGameInputControlFields[6], miMenuInputControlFields[0], NULL, miBackButton);
	mInputMenu->AddControl(miMenuInputControlFields[0], miGameInputControlFields[7], miMenuInputControlFields[1], NULL, miBackButton);
	mInputMenu->AddControl(miMenuInputControlFields[1], miMenuInputControlFields[0], miMenuInputControlFields[2], NULL, miBackButton);
	mInputMenu->AddControl(miMenuInputControlFields[2], miMenuInputControlFields[1], miMenuInputControlFields[3], NULL, miBackButton);
	mInputMenu->AddControl(miMenuInputControlFields[3], miMenuInputControlFields[2], miMenuInputControlFields[4], NULL, miBackButton);
	mInputMenu->AddControl(miMenuInputControlFields[4], miMenuInputControlFields[3], miMenuInputControlFields[5], NULL, miBackButton);
	mInputMenu->AddControl(miMenuInputControlFields[5], miMenuInputControlFields[4], miMenuInputControlFields[6], NULL, miBackButton);
	mInputMenu->AddControl(miMenuInputControlFields[6], miMenuInputControlFields[5], miMenuInputControlFields[7], NULL, miBackButton);
	mInputMenu->AddControl(miMenuInputControlFields[7], miMenuInputControlFields[6], miBackButton, NULL, miBackButton);

	mInputMenu->AddControl(miBackButton, miMenuInputControlFields[7], miDeviceSelectField, miDeviceSelectField, NULL);
		
	mInputMenu->SetHeadControl(miDeviceSelectField);

	SetVisibleInputFields();
}

MI_InputControlContainer::~MI_InputControlContainer()
{
	//takes care of deleting all added controls
	delete mInputMenu;
}


void MI_InputControlContainer::Update()
{
	mInputMenu->Update();
}
		
void MI_InputControlContainer::Draw()
{
	if(!fShow)
		return;

	mInputMenu->Draw();
}

MenuCodeEnum MI_InputControlContainer::SendInput(CPlayerInput * playerInput)
{
	MenuCodeEnum ret = mInputMenu->SendInput(playerInput);

	if(MENU_CODE_CANCEL_INPUT == ret)
	{
		fModifying = false;
		return MENU_CODE_UNSELECT_ITEM;
	}
	else if(MENU_CODE_INPUT_TYPE_CHANGED == ret)
	{
		if(0 == iSelectedInputType)
		{
			iSelectedInputType = 1;
			miInputTypeButton->SetName("Menu");
		}
		else
		{
			iSelectedInputType = 0;
			miInputTypeButton->SetName("Game");
		}

		SetVisibleInputFields();

		return MENU_CODE_NONE;
	}
	else if(MENU_CODE_INPUT_DEVICE_CHANGED == ret)
	{
		//TODO: Need to handle writing out input configurations, modifying input configs here
		//Need to handle case where reading in and using a device that is no longer
		//an option will crash the system (joystick unplugged or something)

		UpdateDeviceKeys(miDeviceSelectField->GetShortValue());
		playerInput->ResetKeys();

		return MENU_CODE_NONE;
	}
	
	return ret;
}

MenuCodeEnum MI_InputControlContainer::Modify(bool modify)
{
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

void MI_InputControlContainer::SetVisibleInputFields()
{
	int iDevice = miDeviceSelectField->GetShortValue();

	for(int iKey = 0; iKey < NUM_KEYS; iKey++)
	{
		miGameInputControlFields[iKey]->Show(0 == iSelectedInputType && (iKey < 6 || DEVICE_KEYBOARD != iDevice || iPlayerID == 0));
	}

	for(int iKey = 0; iKey < NUM_KEYS; iKey++)
	{
		miMenuInputControlFields[iKey]->Show(1 == iSelectedInputType && (iKey < 6 || DEVICE_KEYBOARD != iDevice || iPlayerID == 0));
	}
}

void MI_InputControlContainer::SetPlayer(short playerID)
{
	//Hide input options that other players are using
	miDeviceSelectField->HideAllItems(false);

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(iPlayer == playerID)
			continue;

		if(game_values.playerInput.inputControls[iPlayer]->iDevice > -1)
			miDeviceSelectField->HideItem(game_values.playerInput.inputControls[iPlayer]->iDevice, true);
	}

	iPlayerID = playerID;
	iDevice = game_values.playerInput.inputControls[iPlayerID]->iDevice;
	miDeviceSelectField->SetKey(iDevice);

	for(int iKey = 0; iKey < NUM_KEYS; iKey++)
	{
		miGameInputControlFields[iKey]->SetDevice(iDevice);
		miGameInputControlFields[iKey]->SetType(0);
		miGameInputControlFields[iKey]->SetKeyIndex(iKey);
		miGameInputControlFields[iKey]->SetPlayerIndex(iPlayerID);
		miGameInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[0].keys[iKey]);
	}

	for(int iKey = 0; iKey < NUM_KEYS; iKey++)
	{
		miMenuInputControlFields[iKey]->SetDevice(iDevice);
		miMenuInputControlFields[iKey]->SetType(1);
		miMenuInputControlFields[iKey]->SetKeyIndex(iKey);
		miMenuInputControlFields[iKey]->SetPlayerIndex(iPlayerID);
		miMenuInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[1].keys[iKey]);
	}

	SetVisibleInputFields();

	char szNewTitle[128];
	sprintf(szNewTitle, "Player %d Controls", iPlayerID + 1);
	miText->SetText(szNewTitle);
	mInputMenu->ResetMenu();
}

void MI_InputControlContainer::UpdateDeviceKeys(short iDevice)
{
	game_values.playerInput.inputControls[iPlayerID] = &game_values.inputConfiguration[iPlayerID][iDevice == DEVICE_KEYBOARD ? 0 : 1];
	game_values.playerInput.inputControls[iPlayerID]->iDevice = iDevice;

	for(int iKey = 0; iKey < NUM_KEYS; iKey++)
	{
		miGameInputControlFields[iKey]->SetDevice(iDevice);
		miGameInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[0].keys[iKey]);
	}

	for(int iKey = 0; iKey < NUM_KEYS; iKey++)
	{
		miMenuInputControlFields[iKey]->SetDevice(iDevice);
		miMenuInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[1].keys[iKey]);
	}
	
	SetVisibleInputFields();
}

/**************************************
 * MI_TeamSelect Class
 **************************************/

MI_TeamSelect::MI_TeamSelect(gfxSprite * spr_background, short x, short y) :
	UI_Control(x, y)
{
	spr = spr_background;
	miImage = new MI_Image(spr_background, ix, iy, 0, 0, 416, 256, 1, 1, 0);
	
	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		iTeamCounts[iTeam] = game_values.teamcounts[iTeam];

		for(short iSlot = 0; iSlot < 3; iSlot++)
			iTeamIDs[iTeam][iSlot] = game_values.teamids[iTeam][iSlot];

		fReady[iTeam] = false;
	}

	iAnimationTimer = 0;
	iAnimationFrame = 0;
	iRandomAnimationFrame = 0;
}

MI_TeamSelect::~MI_TeamSelect()
{
	delete miImage;
}

void MI_TeamSelect::Update()
{
	if(++iAnimationTimer > 7)
	{
		iAnimationTimer = 0;

		iAnimationFrame += 2;
		if(iAnimationFrame > 2)
			iAnimationFrame = 0;

		iRandomAnimationFrame += 32;
		if(iRandomAnimationFrame >= 128)
			iRandomAnimationFrame = 0;
	}
}

void MI_TeamSelect::Draw()
{
	if(!fShow)
		return;

	miImage->Draw();

	short iPlayerCount = 0;
	
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(game_values.playercontrol[iPlayer] > 0)
			iPlayerCount++;
	}

	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		for(short iTeamItem = 0; iTeamItem < iTeamCounts[iTeam]; iTeamItem++)
		{
			short iPlayerID = iTeamIDs[iTeam][iTeamItem];

			if(game_values.randomskin[iPlayerID])
				spr->draw(iTeam * 96 + 43 + ix, iTeamItem * 36 + 52 + iy, 416, fReady[iPlayerID] ? 0 : iRandomAnimationFrame, 42, 32);
			else
				spr_player[iPlayerID][fReady[iPlayerID] ? 0 : iAnimationFrame]->draw(iTeam * 96 + 48 + ix, iTeamItem * 36 + 52 + iy, 0, 0, 32, 32);

			spr_menu_boxed_numbers.draw(iTeam * 96 + 44 + ix, iTeamItem * 36 + 72 + iy, iPlayerID * 16, game_values.colorids[iPlayerID] * 16, 16, 16);
		}

		if(game_values.playercontrol[iTeam] > 0)
		{
			spr_player_select_ready.draw(iTeam * 160 + 16, 368, 0, 0, 128, 96);
			
			spr_menu_boxed_numbers.draw(iTeam * 160 + 32, 388, iTeam * 16, game_values.colorids[iTeam] * 16, 16, 16);
			menu_font_small.drawChopRight(iTeam * 160 + 52, 404 - menu_font_small.getHeight(), 80, game_values.randomskin[iTeam] ? "Random" : skinlist.GetSkinName(game_values.skinids[iTeam]));

			spr_player_select_ready.draw(iTeam * 160 + 64, 408, 128, (!fReady[iTeam] ? 0 : (game_values.playercontrol[iTeam] == 1 ? 32 : 64)), 34, 32);
		}
	}

	if(fAllReady)
	{
		menu_plain_field.draw(ix + 108, iy + 224, 0, 160, 100, 32);
		menu_plain_field.draw(ix + 208, iy + 224, 412, 160, 100, 32);
		menu_font_large.drawCentered(320, iy + 229, "Continue");
	}
}

MenuCodeEnum MI_TeamSelect::SendInput(CPlayerInput * playerInput)
{
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

		if(game_values.playercontrol[iPlayer] > 0 && !fReady[iPlayer]) //if this player is player or cpu
		{
			if(playerKeys->menu_left.fPressed)
			{
				if(playerKeys->menu_right.fDown)
					game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
				else
					FindNewTeam(iPlayer, -1);
			}
			
			if(playerKeys->menu_right.fPressed)
			{
				if(playerKeys->menu_left.fDown)
					game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
				else
					FindNewTeam(iPlayer, 1);
			}

			if(playerKeys->menu_up.fPressed && !game_values.randomskin[iPlayer])
			{
				do
				{
					if(playerKeys->menu_down.fDown)
					{
						game_values.skinids[iPlayer] = rand() % skinlist.GetCount();
					}
					else
					{
						if(--game_values.skinids[iPlayer] < 0)
							game_values.skinids[iPlayer] = (short)skinlist.GetCount() - 1;
					}
				}
				while(!LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
			}

			if(playerKeys->menu_down.fPressed && !game_values.randomskin[iPlayer])
			{
				do
				{
					if(playerKeys->menu_up.fDown)
					{
						game_values.skinids[iPlayer] = rand() % skinlist.GetCount();
					}
					else
					{
						if(++game_values.skinids[iPlayer] >= skinlist.GetCount())
							game_values.skinids[iPlayer] = 0;
					}
				}								
				while(!LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
			}

			if(playerKeys->menu_random.fPressed)
			{
				if(playerKeys->menu_scrollfast.fDown)
				{
					game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
				}
				else if(!game_values.randomskin[iPlayer])
				{
					do
					{
						game_values.skinids[iPlayer] = rand() % skinlist.GetCount();
					}								
					while(!LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
				}
			}
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed)
		{
			fReady[iPlayer] = true;

			if(fAllReady && (DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0))
			{
				fModifying = false;
				return MENU_CODE_TO_GAME_SETUP_MENU;
			}

			fAllReady = true;
			for(short i = 0; i < 4; i++)
			{
				if(!fReady[i])
				{
					fAllReady = false;
					break;
				}
			}
		}

		if(playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			if(game_values.playercontrol[iPlayer] > 0 && fReady[iPlayer])
			{
				fReady[iPlayer] = false;
				fAllReady = false;
			}
			else
			{
				if(DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0)
				{
					fModifying = false;
					return MENU_CODE_UNSELECT_ITEM;
				}
			}
		}
	}

	return MENU_CODE_NONE;
}

MenuCodeEnum MI_TeamSelect::Modify(bool modify)
{
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

void MI_TeamSelect::FindNewTeam(short iPlayerID, short iDirection)
{
	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		for(short iTeamItem = 0; iTeamItem < iTeamCounts[iTeam]; iTeamItem++)
		{
			if(iTeamIDs[iTeam][iTeamItem] == iPlayerID)
			{
				iTeamCounts[iTeam]--;

				for(int iMovePlayer = iTeamItem; iMovePlayer < iTeamCounts[iTeam]; iMovePlayer++)
                    iTeamIDs[iTeam][iMovePlayer] = iTeamIDs[iTeam][iMovePlayer + 1];
				
				short iNewTeam = iTeam;
				bool fOnlyTeam = true;
				
				do
				{
					iNewTeam += iDirection;

					if(iNewTeam < 0)
						iNewTeam = 3;
					else if(iNewTeam > 3)
						iNewTeam = 0;

					fOnlyTeam = true;
					for(int iMovePlayer = 0; iMovePlayer < 4; iMovePlayer++)
					{
						if(iMovePlayer == iNewTeam)
							continue;

						if(iTeamCounts[iMovePlayer] > 0)
						{
							fOnlyTeam = false;
							break;
						}
					}
				}
				while(fOnlyTeam);

				iTeamIDs[iNewTeam][iTeamCounts[iNewTeam]] = iPlayerID;
				iTeamCounts[iNewTeam]++;

				if(game_values.teamcolors)
				{
					game_values.colorids[iPlayerID] = iNewTeam;
					
					//Skip skins that are invalid
					while(!LoadMenuSkin(iPlayerID, game_values.skinids[iPlayerID], iNewTeam, false))
					{
						if(++game_values.skinids[iPlayerID] >= skinlist.GetCount())
							game_values.skinids[iPlayerID] = 0;
					}
				}

				return;
			}
		}
	}
}

void MI_TeamSelect::Reset()
{
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		short iTeamID;
		short iSlotID;
		bool fFound = false;
		for(iTeamID = 0; iTeamID < 4; iTeamID++)
		{
			for(iSlotID = 0; iSlotID < iTeamCounts[iTeamID]; iSlotID++)
			{
				if(iTeamIDs[iTeamID][iSlotID] == iPlayer)
				{
					fFound = true;
					break;
				}
			}

			if(fFound)
				break;
		}

		if(fFound)
		{
			//Need to remove the player
			if(game_values.playercontrol[iPlayer] == 0)
			{
				iTeamCounts[iTeamID]--;

				if(iTeamCounts[iTeamID] > iSlotID)
				{
					for(short iSlot = iSlotID; iSlot < iTeamCounts[iTeamID]; iSlot++)
					{
						iTeamIDs[iTeamID][iSlot] = iTeamIDs[iTeamID][iSlot + 1];
					}
				}
			}
		}
		else
		{
			//A new player was added so find a spot for him
			if(game_values.playercontrol[iPlayer] > 0)
			{
				short iLookForNewTeam = iPlayer;
				
				while(iTeamCounts[iLookForNewTeam] >= 3)
				{
					if(++iLookForNewTeam >= 4)
						iLookForNewTeam = 0;
				}

				iTeamIDs[iLookForNewTeam][iTeamCounts[iLookForNewTeam]] = iPlayer;
				iTeamCounts[iLookForNewTeam]++;
				
				if(game_values.teamcolors)
					game_values.colorids[iPlayer] = iLookForNewTeam;
			}
		}
	}

	//Check to see if there is only one team and if so, split them up
	
	short iCountTeams = 0;
	short iLastTeam = 0;
	for(short iTeamID = 0; iTeamID < 4; iTeamID++)
	{
		if(iTeamCounts[iTeamID] > 0)
		{
			iCountTeams++;
			iLastTeam = iTeamID;
		}
	}

	if(iCountTeams == 1)
	{
		short iLookForNewTeam = iLastTeam;
		if(++iLookForNewTeam >= 4)
			iLookForNewTeam = 0;
		
		iTeamCounts[iLastTeam]--;
		short iPlayer = iTeamIDs[iLastTeam][iTeamCounts[iLastTeam]];
		iTeamIDs[iLookForNewTeam][iTeamCounts[iLookForNewTeam]] = iPlayer;
		iTeamCounts[iLookForNewTeam]++;

		if(game_values.teamcolors)
			game_values.colorids[iPlayer] = iLookForNewTeam;
	}

	iAnimationTimer = 0;
	iAnimationFrame = 0;

	fAllReady = true;

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(game_values.playercontrol[iPlayer] == 1)
		{
			fReady[iPlayer] = false;
			fAllReady = false;
		}
		else
		{
			fReady[iPlayer] = true;
		}

		//If debug build, then ignore the player ready stuff
#ifdef _DEBUG
		fReady[iPlayer] = true;
		fAllReady = true;
#endif

		if(game_values.playercontrol[iPlayer] == 0)
			continue;
		
		if(game_values.teamcolors)
			game_values.colorids[iPlayer] = GetTeam(iPlayer);
		else
			game_values.colorids[iPlayer] = iPlayer;

		//Skip skins that are invalid
		while(!LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false))
		{
			if(++game_values.skinids[iPlayer] >= skinlist.GetCount())
				game_values.skinids[iPlayer] = 0;
		}
	}	
}

short MI_TeamSelect::OrganizeTeams()
{
	iNumTeams = 0;
	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		if(iTeamCounts[iTeam] > 0)
		{
			for(short iTeamSpot = 0; iTeamSpot < 3; iTeamSpot++)
				game_values.teamids[iNumTeams][iTeamSpot] = iTeamIDs[iTeam][iTeamSpot];

			game_values.teamcounts[iNumTeams] = iTeamCounts[iTeam];
			iNumTeams++;
		}
	}

	return iNumTeams;
}

short MI_TeamSelect::GetTeam(short iPlayerID)
{
	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		for(short iSlot = 0; iSlot < iTeamCounts[iTeam]; iSlot++)
		{
			if(iTeamIDs[iTeam][iSlot] == iPlayerID)
			{
				return iTeam;
			}
		}
	}

	return -1;
}

/**************************************
 * MI_PlayerSelect Class
 **************************************/
MI_PlayerSelect::MI_PlayerSelect(gfxSprite * nspr, short x, short y, char * name, short width, short indent) :
	UI_Control(x, y)
{
	spr = nspr;
	iSelectedPlayer = 0;

	szName = name;
	iWidth = width;
	iIndent = indent;

	miModifyImage = new MI_Image(nspr, ix, iy - 6, 32, 128, 78, 78, 4, 1, 8);
	miModifyImage->Show(false);

	short iSpacing = (width - indent - 136) / 5;
	iPlayerPosition[0] = iSpacing + indent;
	iPlayerPosition[1] = iPlayerPosition[0] + iSpacing + 34;
	iPlayerPosition[2] = iPlayerPosition[1] + iSpacing + 34;
	iPlayerPosition[3] = iPlayerPosition[2] + iSpacing + 34;

	SetImagePosition();
}

MI_PlayerSelect::~MI_PlayerSelect()
{
	delete miModifyImage;
}

MenuCodeEnum MI_PlayerSelect::Modify(bool modify)
{
	miModifyImage->Show(modify);
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_PlayerSelect::SendInput(CPlayerInput * playerInput)
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_right.fPressed)
		{
			if(++iSelectedPlayer > 3)
				iSelectedPlayer = 0;

			SetImagePosition();
		}
		
		if(playerInput->outputControls[iPlayer].menu_left.fPressed)
		{
			if(--iSelectedPlayer < 0)
				iSelectedPlayer = 3;

			SetImagePosition();
		}

		if(playerInput->outputControls[iPlayer].menu_up.fPressed)
		{
			if(--game_values.playercontrol[iSelectedPlayer] < 0)
				game_values.playercontrol[iSelectedPlayer] = 2;

			if(game_values.playercontrol[iSelectedPlayer] == 0)
			{
				int iCountPlayers = 0;
				for(int iPlayer = 0; iPlayer < 4; iPlayer++)
				{
					if(game_values.playercontrol[iPlayer] > 0)
						iCountPlayers++;
				}

				if(iCountPlayers < 2)
					game_values.playercontrol[iSelectedPlayer] = 2;
			}
		}

		if(playerInput->outputControls[iPlayer].menu_down.fPressed)
		{
			if(++game_values.playercontrol[iSelectedPlayer] > 2)
				game_values.playercontrol[iSelectedPlayer] = 0;

			if(game_values.playercontrol[iSelectedPlayer] == 0)
			{
				int iCountPlayers = 0;
				for(int iPlayer = 0; iPlayer < 4; iPlayer++)
				{
					if(game_values.playercontrol[iPlayer] > 0)
						iCountPlayers++;
				}

				if(iCountPlayers < 2)
					game_values.playercontrol[iSelectedPlayer] = 1;
			}
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			miModifyImage->Show(false);
			fModifying = false;
			return MENU_CODE_UNSELECT_ITEM;
		}
	}

	return MENU_CODE_NONE;
}

void MI_PlayerSelect::SetImagePosition()
{
	miModifyImage->SetPosition(ix + iPlayerPosition[iSelectedPlayer] - 22, iy - 7);
}

void MI_PlayerSelect::Update()
{
	miModifyImage->Update();
}

void MI_PlayerSelect::Draw()
{
	if(!fShow)
		return;

	spr->draw(ix, iy, 0, (fSelected ? 64 : 0), iIndent - 16, 64);
	spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 192 : 128), 32, 64);
	spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 64 : 0), iWidth - iIndent - 16, 64);

	menu_font_large.drawChopRight(ix + 16, iy + 20, iIndent - 8, szName);

	miModifyImage->Draw();
	
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		spr->draw(ix + iPlayerPosition[iPlayer], iy + 16, game_values.playercontrol[iPlayer] * 34 + 32, 206, 34, 32);
	}
}


/**************************************
 * MI_SliderField Class
 **************************************/

MI_SliderField::MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, char * name, short width, short indent1, short indent2) :
	MI_SelectField(nspr, x, y, name, width, indent1)
{
	iIndent2 = indent2;
	sprSlider = nsprSlider;

	miModifyImageLeft->SetPosition(ix + indent1 - 26, iy + 4);
	miModifyImageRight->SetPosition(ix + iWidth - 16, iy + 4);
}

MI_SliderField::~MI_SliderField()
{}

void MI_SliderField::Draw()
{
	if(!fShow)
		return;

	spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iIndent - 16, 32);
	spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
	spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iIndent - 16, 32);

	menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

	if(!items.empty())
	{
		menu_font_large.drawChopRight(ix + iIndent2 + 16, iy + 5, iWidth - iIndent2 - 24, (*current)->sName.c_str());
	}

	short iSpacing = (iIndent2 - iIndent - 20) / ((short)items.size() - 1);
	short iSpot = 0;

	for(unsigned int index = 0; index < items.size(); index++)
	{
		if(index < items.size() - 1)
            sprSlider->draw(ix + iIndent + iSpot + 16, iy + 10, 0, 0, iSpacing, 13);
		else
			sprSlider->draw(ix + iIndent + iSpot + 16, iy + 10, 164, 0, 4, 13);

		iSpot += iSpacing;
	}

	sprSlider->draw(ix + iIndent + (iIndex * iSpacing) + 14, iy + 8, 168, 0, 8, 16);

	if(current != items.begin() || !fNoWrap)
		miModifyImageLeft->Draw();

	if(current != --items.end() || !fNoWrap)
		miModifyImageRight->Draw();
}

MenuCodeEnum MI_SliderField::SendInput(CPlayerInput * playerInput)
{

	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_scrollfast.fPressed)
		{
			if(iIndex == 0)
				while(MoveNext());
			else
				while(MovePrev());

			return mcItemChangedCode;
		}
	}

	return MI_SelectField::SendInput(playerInput);
}


/**************************************
 * MI_PowerupSlider Class
 **************************************/

MI_PowerupSlider::MI_PowerupSlider(gfxSprite * nspr, gfxSprite * nsprSlider, gfxSprite * nsprPowerup, short x, short y, short width, short powerupIndex) :
	MI_SliderField(nspr, nsprSlider, x, y, "", width, 0, 0)
{
	sprPowerup = nsprPowerup;
	iPowerupIndex = powerupIndex;

	miModifyImageLeft->SetPosition(ix + 25, iy + 4);
	miModifyImageRight->SetPosition(ix + iWidth - 12, iy + 4);

	iHalfWidth = (width - 38) >> 1;
}

MI_PowerupSlider::~MI_PowerupSlider()
{}

void MI_PowerupSlider::Draw()
{
	if(!fShow)
		return;

	spr->draw(ix + 38, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iHalfWidth, 32);
	spr->draw(ix + 38 + iHalfWidth, iy, 550 - iWidth + iHalfWidth, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iHalfWidth - 38, 32);

	short iSpacing = (iWidth - 100) / ((short)items.size() - 1);
	short iSpot = 0;

	for(unsigned int index = 0; index < items.size(); index++)
	{
		if(index < items.size() - 1)
            sprSlider->draw(ix + iSpot + 56, iy + 10, 0, 0, iSpacing, 13);
		else
			sprSlider->draw(ix + iSpot + 56, iy + 10, 164, 0, 4, 13);

		iSpot += iSpacing;
	}

	sprSlider->draw(ix + (iIndex * iSpacing) + 54, iy + 8, 168, 0, 8, 16);

	sprSlider->draw(ix + iWidth - 34, iy + 8, iIndex * 16, 16, 16, 16);

	sprPowerup->draw(ix, iy, iPowerupIndex * 32, 0, 32, 32);

	if(current != items.begin() || !fNoWrap)
		miModifyImageLeft->Draw();

	if(current != --items.end() || !fNoWrap)
		miModifyImageRight->Draw();
}


/**************************************
 * MI_MapField Class
 **************************************/

MI_MapField::MI_MapField(gfxSprite * nspr, short x, short y, char * name, short width, short indent, bool showtags) :
	UI_Control(x, y)
{
	fDisable = false;

	spr = nspr;
	szName = name;
	iWidth = width;
	iIndent = indent;

	miModifyImageLeft = new MI_Image(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
	miModifyImageLeft->Show(false);

	miModifyImageRight = new MI_Image(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
	miModifyImageRight->Show(false);

	surfaceMapBackground = SDL_CreateRGBSurface(0, 320, 240, 16, 0, 0, 0, 0);
	surfaceMapForeground = SDL_CreateRGBSurface(0, 320, 240, 16, 0, 0, 0, 0);
	LoadCurrentMap();
	
	rectDst.x = x + 16;
	rectDst.y = y + 44;
	rectDst.w = 320;
	rectDst.h = 240;

	if(showtags)
	{
		iSlideListOut = (iWidth - 352) >> 1;
		iSlideListOutGoal = iSlideListOut;
	}
	else
	{
		iSlideListOut = 0;
		iSlideListOutGoal = iSlideListOut;
	}
}

MI_MapField::~MI_MapField()
{
	delete miModifyImageLeft;
	delete miModifyImageRight;
}

MenuCodeEnum MI_MapField::Modify(bool modify)
{
	if(fDisable)
		return MENU_CODE_UNSELECT_ITEM;

	miModifyImageLeft->Show(modify);
	miModifyImageRight->Show(modify);

	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_MapField::SendInput(CPlayerInput * playerInput)
{
	if(playerInput->iPressedKey > 0)
	{
		if((playerInput->iPressedKey >= SDLK_a && playerInput->iPressedKey <= SDLK_z) ||
			(playerInput->iPressedKey >= SDLK_0 && playerInput->iPressedKey <= SDLK_9))
		{
			short iOldIndex = maplist.GetCurrent()->second->iIndex;
			maplist.startswith((char)playerInput->iPressedKey);
			
			if(iOldIndex != maplist.GetCurrent()->second->iIndex)
			{
				LoadCurrentMap();
				return MENU_CODE_MAP_CHANGED;
			}
			
			return MENU_CODE_NONE;
		}
		/*
		else if(playerInput->iPressedKey == SDLK_PAGEUP)
		{
			short iOldIndex = maplist.GetCurrent()->second->iIndex;
			maplist.prev(true);
			
			if(iOldIndex != maplist.GetCurrent()->second->iIndex)
			{
				LoadCurrentMap();
				return MENU_CODE_MAP_CHANGED;
			}

			return MENU_CODE_NONE;
		}
		else if(playerInput->iPressedKey == SDLK_PAGEDOWN)
		{
			short iOldIndex = maplist.GetCurrent()->second->iIndex;
			maplist.next(true);

			if(iOldIndex != maplist.GetCurrent()->second->iIndex)
			{
				LoadCurrentMap();
				return MENU_CODE_MAP_CHANGED;
			}

			return MENU_CODE_NONE;
		}
		*/
	}

	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed)
		{
			int numadvance = 1;
			if(playerInput->outputControls[iPlayer].menu_scrollfast.fDown)
				numadvance = 10;

			short iOldIndex = maplist.GetCurrent()->second->iIndex;
			for(int k = 0; k < numadvance; k++)
				maplist.next(true);

			if(iOldIndex != maplist.GetCurrent()->second->iIndex)
			{
				LoadCurrentMap();
				return MENU_CODE_MAP_CHANGED;
			}

			return MENU_CODE_NONE;
		}
		
		if(playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed)
		{
			int numadvance = 1;
			if(playerInput->outputControls[iPlayer].menu_scrollfast.fDown)
				numadvance = 10;

			short iOldIndex = maplist.GetCurrent()->second->iIndex;
			for(int k = 0; k < numadvance; k++)
				maplist.prev(true);

			if(iOldIndex != maplist.GetCurrent()->second->iIndex)
			{
				LoadCurrentMap();
				return MENU_CODE_MAP_CHANGED;
			}

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_random.fPressed)
		{
			short iOldIndex = maplist.GetCurrent()->second->iIndex;
			maplist.random(true);
			
			if(iOldIndex != maplist.GetCurrent()->second->iIndex)
			{
				LoadCurrentMap();
				return MENU_CODE_MAP_CHANGED;
			}

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed)
		{
			miModifyImageLeft->Show(false);
			miModifyImageRight->Show(false);
			
			fModifying = false;
			return MENU_CODE_UNSELECT_ITEM;
		}

		if(playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			miModifyImageLeft->Show(false);
			miModifyImageRight->Show(false);
			
			fModifying = false;

			return MENU_CODE_UNSELECT_ITEM;
		}
	}

	return MENU_CODE_NONE;
}

void MI_MapField::Update()
{
	if(iSlideListOut != iSlideListOutGoal)
	{
		if(iSlideListOutGoal > iSlideListOut)
		{
			iSlideListOut += 4;

			if(iSlideListOut > iSlideListOutGoal)
				iSlideListOut = iSlideListOutGoal;
		}
		else if(iSlideListOutGoal < iSlideListOut)
		{
			iSlideListOut -= 4;

			if(iSlideListOut < iSlideListOutGoal)
				iSlideListOut = iSlideListOutGoal;
		}
	}

	miModifyImageRight->Update();
	miModifyImageLeft->Update();

	g_map.updatePlatforms();
}

void MI_MapField::Draw()
{
	if(!fShow)
		return;

	//Draw the select field background
	spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
	spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
	spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

	short iMapBoxX = ix + (iWidth >> 1) - 176 - iSlideListOut;

	//Draw the background for the map preview
	menu_dialog.draw(iMapBoxX, iy + 30, 0, 0, 336, 254);
	menu_dialog.draw(iMapBoxX + 336, iy + 30, 496, 0, 16, 254);
	menu_dialog.draw(iMapBoxX, iy + 284, 0, 464, 336, 16);
	menu_dialog.draw(iMapBoxX + 336, iy + 284, 496, 464, 16, 16);

	rectDst.x = iMapBoxX + 16;

	menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);
	menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 24, maplist.currentShortmapname());

	SDL_BlitSurface(surfaceMapBackground, NULL, blitdest, &rectDst);
	g_map.drawPlatforms(rectDst.x, rectDst.y);
	
	if(game_values.toplayer)
		SDL_BlitSurface(surfaceMapForeground, NULL, blitdest, &rectDst);
	
	miModifyImageLeft->Draw();
	miModifyImageRight->Draw();
}

void MI_MapField::LoadCurrentMap()
{
	szMapName = maplist.currentShortmapname();

	g_map.loadMap(maplist.currentFilename(), read_type_preview);
	SDL_Delay(10);  //Sleeps to help the music from skipping
	
	char filename[128];
	sprintf(filename, "gfx/packs/backgrounds/%s", g_map.szBackgroundFile);
	std::string path = convertPath(filename, gamegraphicspacklist.current_name());

	//if the background file doesn't exist, use the classic background
	if(!File_Exists(path))
		path = convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist.current_name());

	__load_gfx(spr_background, path);

	SDL_Delay(10);  //Sleeps to help the music from skipping

	g_map.preDrawPreviewBackground(&spr_background, surfaceMapBackground, false);
	g_map.preDrawPreviewMapItems(surfaceMapBackground, false);
	SDL_Delay(10);  //Sleeps to help the music from skipping
	g_map.preDrawPreviewForeground(surfaceMapForeground, false);
	g_map.preDrawPreviewWarps(surfaceMapForeground, false);
}

void MI_MapField::SetMap(const char * szMapName)
{
	maplist.findexact(szMapName);
	LoadCurrentMap();
}

/**************************************
 * MI_AnnouncerField Class
 **************************************/

MI_AnnouncerField::MI_AnnouncerField(gfxSprite * nspr, short x, short y, char * name, short width, short indent, SimpleFileList * pList) :
	UI_Control(x, y)
{
	spr = nspr;
	szName = name;
	iWidth = width;
	iIndent = indent;

	list = pList;
	UpdateName();

	miModifyImageLeft = new MI_Image(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
	miModifyImageLeft->Show(false);

	miModifyImageRight = new MI_Image(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
	miModifyImageRight->Show(false);
}

MI_AnnouncerField::~MI_AnnouncerField()
{
	delete miModifyImageLeft;
	delete miModifyImageRight;
}

MenuCodeEnum MI_AnnouncerField::Modify(bool modify)
{
	miModifyImageLeft->Show(modify);
	miModifyImageRight->Show(modify);

	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_AnnouncerField::SendInput(CPlayerInput * playerInput)
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed)
		{
			list->next();
			UpdateName();
			return MENU_CODE_NONE;
		}
		
		if(playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed)
		{
			list->prev();
			UpdateName();
			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_random.fPressed)
		{
            list->random();
			UpdateName();
            return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			miModifyImageLeft->Show(false);
			miModifyImageRight->Show(false);

			fModifying = false;

			return MENU_CODE_UNSELECT_ITEM;
		}
	}

	return MENU_CODE_NONE;
}

void MI_AnnouncerField::UpdateName()
{
	GetNameFromFileName(szFieldName, list->current_name());
}

void MI_AnnouncerField::Update()
{
	miModifyImageRight->Update();
	miModifyImageLeft->Update();
}

void MI_AnnouncerField::Draw()
{
	if(!fShow)
		return;

	//Draw the select field background
	spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
	spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
	spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

	menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);
	menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 24, szFieldName);

	miModifyImageLeft->Draw();
	miModifyImageRight->Draw();
}


/**************************************
 * MI_PacksField Class
 **************************************/

MI_PacksField::MI_PacksField(gfxSprite * nspr, short x, short y, char * name, short width, short indent, SimpleFileList * pList, MenuCodeEnum code) :
	MI_AnnouncerField(nspr, x, y, name, width, indent, pList)
{
	itemChangedCode = code;
}

MI_PacksField::~MI_PacksField()
{}

MenuCodeEnum MI_PacksField::SendInput(CPlayerInput * playerInput)
{
	int iLastIndex = list->GetCurrentIndex();
	MenuCodeEnum code = MI_AnnouncerField::SendInput(playerInput);

	if(MENU_CODE_UNSELECT_ITEM == code)
		return code;

	MenuCodeEnum returnCode = MENU_CODE_NONE;
	if(iLastIndex != list->GetCurrentIndex())
		returnCode = itemChangedCode;

	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed)
			return returnCode;
		
		if(playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed)
			return returnCode;

		if(playerInput->outputControls[iPlayer].menu_random.fPressed)
            return returnCode;
	}

	return MENU_CODE_NONE;
}

/**************************************
 * MI_PlayListField Class
 **************************************/

MI_PlaylistField::MI_PlaylistField(gfxSprite * nspr, short x, short y, char * name, short width, short indent) :
	UI_Control(x, y)
{
	spr = nspr;
	szName = name;
	iWidth = width;
	iIndent = indent;

	miModifyImageLeft = new MI_Image(nspr, ix + indent - 26, iy + 4, 32, 64, 26, 24, 4, 1, 8);
	miModifyImageLeft->Show(false);

	miModifyImageRight = new MI_Image(nspr, ix + iWidth - 16, iy + 4, 32, 88, 26, 24, 4, 1, 8);
	miModifyImageRight->Show(false);
}

MI_PlaylistField::~MI_PlaylistField()
{
	delete miModifyImageLeft;
	delete miModifyImageRight;
}

MenuCodeEnum MI_PlaylistField::Modify(bool modify)
{
	miModifyImageLeft->Show(modify);
	miModifyImageRight->Show(modify);

	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_PlaylistField::SendInput(CPlayerInput * playerInput)
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed)
		{
			musiclist.next();
			backgroundmusic[2].stop();
			backgroundmusic[2].load(musiclist.GetMusic(1));
			
			if(game_values.music)
				backgroundmusic[2].play(false, false);

			return MENU_CODE_NONE;
		}
		
		if(playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed)
		{
			musiclist.prev();
			backgroundmusic[2].stop();
			backgroundmusic[2].load(musiclist.GetMusic(1));
			
			if(game_values.music)
				backgroundmusic[2].play(false, false);

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_random.fPressed)
		{
            musiclist.random();
			backgroundmusic[2].stop();
			backgroundmusic[2].load(musiclist.GetMusic(1));
			
			if(game_values.music)
				backgroundmusic[2].play(false, false);

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			miModifyImageLeft->Show(false);
			miModifyImageRight->Show(false);

			fModifying = false;

			return MENU_CODE_UNSELECT_ITEM;
		}
	}

	return MENU_CODE_NONE;
}

void MI_PlaylistField::Update()
{
	miModifyImageRight->Update();
	miModifyImageLeft->Update();
}

void MI_PlaylistField::Draw()
{
	if(!fShow)
		return;

	//Draw the select field background
	spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
	spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
	spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

	menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);
	menu_font_large.drawChopRight(ix + iIndent + 8, iy + 5, iWidth - iIndent - 24, musiclist.current_name());

	miModifyImageLeft->Draw();
	miModifyImageRight->Draw();
}

/**************************************
 * MI_Button Class
 **************************************/

MI_Button::MI_Button(gfxSprite * nspr, short x, short y, char * name, short width, short justified) :
	UI_Control(x, y)
{
	spr = nspr;
	szName = name;
	iWidth = width;
	iTextJustified = justified;
	fSelected = false;
	menuCode = MENU_CODE_NONE;

	sprImage = NULL;
	iImageSrcX = 0;
	iImageSrcY = 0;
	iImageW = 0;
	iImageH = 0;

	iTextW = (short)menu_font_large.getWidth(name);

	iAdjustmentY = width > 256 ? 0 : 128;
	iHalfWidth = width >> 1;
}

MenuCodeEnum MI_Button::Modify(bool)
{
	return menuCode;
}

MenuCodeEnum MI_Button::SendInput(CPlayerInput *)
{
	//If input is being sent, that means the button is selected i.e. clicked
	return menuCode;
}

void MI_Button::Draw()
{
	if(!fShow)
		return;

	spr->draw(ix, iy, 0, (fSelected ? 32 : 0) + iAdjustmentY, iHalfWidth, 32);
	spr->draw(ix + iHalfWidth, iy, 512 - iWidth + iHalfWidth, (fSelected ? 32 : 0) + iAdjustmentY, iWidth - iHalfWidth, 32);

	if(0 == iTextJustified)
	{
		menu_font_large.drawChopRight(ix + 16 + (iImageW > 0 ? iImageW + 2 : 0), iy + 5, iWidth - 32, szName);
		
		if(sprImage)
			sprImage->draw(ix + 16, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
	}
	else if(1 == iTextJustified)
	{
		menu_font_large.drawCentered(ix + ((iWidth + (iImageW > 0 ? iImageW + 2 : 0)) >> 1), iy + 5, szName);

		if(sprImage)
			sprImage->draw(ix + (iWidth >> 1) - ((iTextW + iImageW) >> 1) - 1, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
	}
	else
	{
		menu_font_large.drawRightJustified(ix + iWidth - 16, iy + 5, szName);

		if(sprImage)
			sprImage->draw(ix + iWidth - 18 - iTextW - iImageW, iy + 16 - (iImageH >> 1), iImageSrcX, iImageSrcY, iImageW, iImageH);
	}
}

void MI_Button::SetName(char * name)
{
	szName = name; 
	iTextW = (short)menu_font_large.getWidth(name);
}

void MI_Button::SetImage(gfxSprite * nsprImage, short x, short y, short w, short h)
{
	sprImage = nsprImage;
	iImageSrcX = x;
	iImageSrcY = y;
	iImageW = w;
	iImageH = h;
}

/**************************************
 * MI_StoredPowerupResetButton Class
 **************************************/

MI_StoredPowerupResetButton::MI_StoredPowerupResetButton(gfxSprite * nspr, short x, short y, char * name, short width, short justified) :
	MI_Button(nspr, x, y, name, width, justified)
{
	spr = nspr;
	szName = name;
	iWidth = width;
	iTextJustified = justified;
	fSelected = false;
	menuCode = MENU_CODE_NONE;

	sprImage = NULL;
	iImageSrcX = 0;
	iImageSrcY = 0;
	iImageW = 0;
	iImageH = 0;

	iTextW = (short)menu_font_large.getWidth(name);
}

void MI_StoredPowerupResetButton::Draw()
{
	if(!fShow)
		return;

	MI_Button::Draw();

	for(short iPowerup = 0; iPowerup < 4; iPowerup++)
	{
		spr_selectfield.draw(ix + iWidth - 142 + iPowerup * 30, iy + 4, 188, 88, 24, 24);
		spr_storedpowerupsmall.draw(ix + iWidth - 138 + iPowerup * 30, iy + 8, game_values.storedpowerups[iPowerup] * 16, 0, 16, 16);
	}
}

/**************************************
 * MI_TourStop Class
 **************************************/

char * g_szWorldBonusNames[NUM_POWERUPS + 2] = {"None", "Random", "Poison Mushroom", "1up Mushroom", "2up Mushroom", "3up Mushroom", "5up Mushroom",
												"Fire Flower", "Star", "Clock", "Bob-omb", "POW", "Bullet Bill", "Hammer", "Green Shell",
												"Red Shell", "Spiked Shell", "Buzzy Shell", "MOd", "Feather", "Mystery Mushroom", "Boomerang"};
//Call with x = 70 and y == 80
MI_TourStop::MI_TourStop(short x, short y, bool fWorld) :
	UI_Control(x, y)
{
	fIsWorld = fWorld;

	if(fIsWorld)
	{
		miModeField = new MI_ImageSelectField(&spr_selectfielddisabled, &menu_mode_small, 70, 85, "Mode", 305, 90, 16, 16);
		miGoalField = new MI_SelectField(&spr_selectfielddisabled, 370, 85, "Goal", 200, 90);
		miPointsField = new MI_SelectField(&spr_selectfielddisabled, 370, 125, "Points", 200, 90);

		miBonusField = new MI_SelectField(&spr_selectfielddisabled, 70, 125, "Bonus", 305, 90);
		miBonusField->Disable(true);

		miEndStageImage = new MI_Image(&spr_worldbackground, 200, 300, 0, 0, 100, 40, 1, 1, 32000);
		miEndStageImage->Show(false);
	}
	else
	{
		miModeField = new MI_ImageSelectField(&spr_selectfielddisabled, &menu_mode_small, 70, 85, "Mode", 500, 120, 16, 16);
		miGoalField = new MI_SelectField(&spr_selectfielddisabled, 70, 125, "Goal", 246, 120);
		miPointsField = new MI_SelectField(&spr_selectfielddisabled, 70 + 254, 125, "Points", 246, 120);
	
		miBonusField = 0;
	}

	miStartButton = new MI_Button(&spr_selectfield, 70, 45, "Start", 500, 0);	
	miStartButton->SetCode(MENU_CODE_TOUR_STOP_CONTINUE);
	miStartButton->Select(true);

	miMapField = new MI_MapField(&spr_selectfielddisabled, 70, 165, "Map", 500, 120, false);
	miMapField->Disable(true);

	miModeField->Disable(true);
	miGoalField->Disable(true);
	miPointsField->Disable(true);
	

	miTourStopLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miTourStopMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miTourStopMenuHeaderText = new MI_Text("Tour Stop", 320, 5, 0, 2, 1);

	
}

MI_TourStop::~MI_TourStop()
{
	delete miModeField;
	delete miGoalField;
	delete miPointsField;
	delete miMapField;
	delete miStartButton;

	if(miBonusField)
	{
		delete miBonusField;
		delete miEndStageImage;
	}
}

MenuCodeEnum MI_TourStop::Modify(bool fModify)
{
	return miStartButton->Modify(fModify);
}

void MI_TourStop::Update()
{
	miStartButton->Update();

	miModeField->Update();
	miGoalField->Update();
	miPointsField->Update();
	miMapField->Update();

	if(fIsWorld)
	{
		miBonusField->Update();
	}

	miTourStopLeftHeaderBar->Update();
	miTourStopMenuRightHeaderBar->Update();
	miTourStopMenuHeaderText->Update();
}
		
void MI_TourStop::Draw()
{
	if(!fShow)
		return;

	miStartButton->Draw();

	miModeField->Draw();
	miGoalField->Draw();
	miPointsField->Draw();
	miMapField->Draw();

	if(fIsWorld)
	{
		miBonusField->Draw();
		miEndStageImage->Draw();
	}

	miTourStopLeftHeaderBar->Draw();
	miTourStopMenuRightHeaderBar->Draw();
	miTourStopMenuHeaderText->Draw();
}

void MI_TourStop::Refresh(short iTourStop)
{
	TourStop * tourstop = game_values.tourstops[iTourStop];

	miModeField->Clear();
	miModeField->Add(gamemodes[tourstop->iMode]->GetModeName(), tourstop->iMode, "", false, false);

	miGoalField->Clear();
	char szTemp[16];
	sprintf(szTemp, "%d", tourstop->iGoal);
	miGoalField->Add(szTemp, 0, "", false, false);
	miGoalField->SetTitle(gamemodes[tourstop->iMode]->GetGoalName());

	miPointsField->Clear();
	sprintf(szTemp, "%d", tourstop->iPoints);
	miPointsField->Add(szTemp, 0, "", false, false);

	miMapField->SetMap(tourstop->pszMapFile);
	miTourStopMenuHeaderText->SetText(tourstop->szName);

	if(fIsWorld)
	{
		miBonusField->Clear();
		miBonusField->Add(g_szWorldBonusNames[tourstop->iBonusType], tourstop->iBonusType, "", false, false);

		if(tourstop->fEndStage)
			miEndStageImage->Show(true);
		else
			miEndStageImage->Show(false);
	}
}

/**************************************
 * MI_TournamentScoreboard Class
 **************************************/

//Call with x = 70 and y == 80
MI_TournamentScoreboard::MI_TournamentScoreboard(gfxSprite * spr_background, short x, short y) :
	UI_Control(x, y)
{
	sprBackground = spr_background;
	fCreated = false;

	for(int iTeam = 0; iTeam < 4; iTeam++)
		tourScores[iTeam] = NULL;

	for(int iGame = 0; iGame < 10; iGame++)
	{
		tourPoints[iGame] = NULL;
		tourBonus[iGame] = NULL;
	}

	miTourPointBar = NULL;
}

MI_TournamentScoreboard::~MI_TournamentScoreboard()
{
	FreeScoreboard();
}

void MI_TournamentScoreboard::FreeScoreboard()
{
	if(!fCreated)
		return;

	for(int iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		for(int iGame = 0; iGame < iNumGames; iGame++)
		{
			delete miIconImages[iTeam][iGame];
		}

		for(int iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++)
		{
			delete miPlayerImages[iTeam][iPlayer];
		}

		delete [] miPlayerImages[iTeam];
		delete [] miIconImages[iTeam];
		delete miTeamImages[iTeam];

		if(tourScores[iTeam])
		{
			delete tourScores[iTeam];
			tourScores[iTeam] = NULL;
		}
	}

	for(int iGame = 0; iGame < iNumGames; iGame++)
	{
		if(tourPoints[iGame])
		{
			delete tourPoints[iGame];
			tourPoints[iGame] = NULL;
		}

		if(tourBonus[iGame])
		{
			delete tourBonus[iGame];
			tourBonus[iGame] = NULL;
		}
	}

	if(miTourPointBar)
	{
		delete miTourPointBar;
		miTourPointBar = NULL;
	}
	

	delete [] miPlayerImages;
	delete [] miIconImages;
}

void MI_TournamentScoreboard::Update()
{
	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		miTeamImages[iTeam]->Update();
	
		for(short iGame = 0; iGame < iNumGames; iGame++)
		{
			miIconImages[iTeam][iGame]->Update();
		}

		for(short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++)
		{
			miPlayerImages[iTeam][iPlayer]->Update();
		}
	}

	if(fTour)
	{
		for(short iGame = 0; iGame < iNumGames; iGame++)
		{
			if(tourBonus[iGame])
				tourBonus[iGame]->Update();
		}
	}

	if(uiMenu)
	{
		if(iTournamentWinner != -1)  //Single tournament winning team
		{
			if(--iFireworksCounter < 0 && iTournamentWinner >= 0)
			{
				iFireworksCounter = (short)(rand() % 100 + 25);

				ifsoundonplay(sfx_cannon);

				float dAngle = 0.0f; 
				short iRandX = (short)(rand() % 440 + 100);
				short iRandY = (short)(rand() % 280 + 100);

				for(short iBlock = 0; iBlock < 28; iBlock++)
				{
					float dVel = 7.0f + ((iBlock % 2) * 5.0f);
					float dVelX = dVel * cos(dAngle);
					float dVelY = dVel * sin(dAngle);
					
					short iRandomColor = (short)(rand() % iTeamCounts[iTournamentWinner]);
					uiMenu->AddEyeCandy(new EC_FallingObject(&spr_bonus, iRandX, iRandY, dVelX, dVelY, 4, 2, 0, game_values.colorids[iTeamIDs[iTournamentWinner][iRandomColor]] * 16, 16, 16));
					dAngle -= (float)PI / 14;
				}
			}

			if(--iWinnerTextCounter < 0)
			{
				iWinnerTextCounter = (short)(rand() % 35 + 15);

				char szWinnerText[64];
				if(iTournamentWinner == -2)
					sprintf(szWinnerText, "Tied Game!");
				else if(iTeamCounts[iTournamentWinner] == 1)
					sprintf(szWinnerText, "Player %d Wins!", iTeamIDs[iTournamentWinner][0] + 1);
				else if(iTeamCounts[iTournamentWinner] > 1)
					sprintf(szWinnerText, "Team %d Wins!", iTournamentWinner + 1);

				short iStringWidth = (short)menu_font_large.getWidth(szWinnerText);
				short iRandX = (short)(rand() % (640 - iStringWidth) + (iStringWidth >> 1));
				short iRandY = (short)(rand() % 380 + 100);

				uiMenu->AddEyeCandy(new EC_GravText(&menu_font_large, iRandX, iRandY, szWinnerText, -VELJUMP));
			}
		}
	}
}
		
void MI_TournamentScoreboard::Draw()
{
	if(!fShow)
		return;

	for(int iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		miTeamImages[iTeam]->Draw();

		for(int iGame = 0; iGame < iNumGames; iGame++)
		{
			if(iSwirlIconTeam != iTeam || iSwirlIconGame != iGame)
				miIconImages[iTeam][iGame]->Draw();
		}

		for(int iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++)
		{
			miPlayerImages[iTeam][iPlayer]->Draw();
		}
	}

	if(iSwirlIconTeam > -1)
	{
		miIconImages[iSwirlIconTeam][iSwirlIconGame]->Draw();
	}

	//Draw tour totals
	if(fTour)
	{
		for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
			tourScores[iTeam]->Draw();

		miTourPointBar->Draw();

		for(short iGame = 0; iGame < iNumGames; iGame++)
		{
			if(tourBonus[iGame])
				tourBonus[iGame]->Draw();

			tourPoints[iGame]->Draw();
		}
	}
}

void MI_TournamentScoreboard::CreateScoreboard(short numTeams, short numGames, gfxSprite * spr_icons, bool tour)
{
	FreeScoreboard();

	sprIcons = spr_icons;
	fTour = tour;

	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		iTeamCounts[iTeam] = game_values.teamcounts[iTeam];

		for(short iTeamSpot = 0; iTeamSpot < 3; iTeamSpot++)
		{
			iTeamIDs[iTeam][iTeamSpot] = game_values.teamids[iTeam][iTeamSpot];
		}
	}
	
	iTournamentWinner = -1;
	iGameWinner = -1;

	iSwirlIconTeam = -1;
	iSwirlIconGame = -1;

	iFireworksCounter = 0;
	iWinnerTextCounter = 0;

	iNumTeams = numTeams;
	iNumGames = numGames;
	
	miTeamImages = new MI_Image * [iNumTeams];
	miIconImages = new MI_Image ** [iNumTeams];
	miPlayerImages = new MI_Image ** [iNumTeams];
	
	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		short iTeamY = GetYFromPlace(iTeam);

		if(fTour)
			iTeamY += 28; //shift down 18 pxls for extra tour points bar

		miTeamImages[iTeam] = new MI_Image(sprBackground, ix - (fTour ? 40 : 0), iTeamY, 0, 0, fTour ? 580 : 500, 64, 1, 2, 0);
		miIconImages[iTeam] = new MI_Image * [iNumGames];
		miPlayerImages[iTeam] = new MI_Image * [iTeamCounts[iTeam]];

		for(short iGame = 0; iGame < iNumGames; iGame++)
		{
			float dSpacing = GetIconSpacing();

			miIconImages[iTeam][iGame] = new MI_Image(sprIcons, ix + 128 + (short)dSpacing + (short)((float)iGame * (32.0f + dSpacing)) - (fTour ? 40 : 0), iTeamY + 16, 0, 0, 32, 32, fTour ? 4 : 1, 1, fTour ? 8 : 0);
			miIconImages[iTeam][iGame]->Show(false);
		}

		for(short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++)
		{
			miPlayerImages[iTeam][iPlayer] = new MI_Image(spr_player[iTeamIDs[iTeam][iPlayer]][PGFX_STANDING_R], ix + iScoreboardPlayerOffsetsX[iTeamCounts[iTeam] - 1][iPlayer] - (fTour ? 40 : 0), iTeamY + 16, 0, 0, 32, 32, 2, 1, 0);
		}

		if(fTour)
			tourScores[iTeam] = new MI_ScoreText(0, 0);
	}

	if(fTour)
	{
		for(short iGame = 0; iGame < iNumGames; iGame++)
		{
			float dSpacing = GetIconSpacing();
			short iTourPointX = ix + 105 + (short)dSpacing + (short)((float)iGame * (32.0f + dSpacing));
			tourPoints[iGame] = new MI_ScoreText(iTourPointX, GetYFromPlace(0));
			tourPoints[iGame]->SetScore(game_values.tourstops[iGame]->iPoints);

			if(game_values.tourstops[iGame]->iBonusType)
				tourBonus[iGame] = new MI_Image(sprBackground, iTourPointX - 19, GetYFromPlace(0) - 12, 372, 128, 38, 31, 2, 1, 8);
			//else
			//	tourBonus[iGame] = new MI_Image(sprBackground, iTourPointX - 11, GetYFromPlace(0) - 3, 448, 128, 22, 22, 1, 1, 0);
		}

		miTourPointBar = new MI_Image(sprBackground, ix + 88, GetYFromPlace(0) - 8, 0, 128, 372, 32, 1, 1, 0);
	}

	fCreated = true;
}

//Called by Tour -- Arranges players in terms of standings
void MI_TournamentScoreboard::RefreshScores()
{
	//Detect a Tie
	short iNumWinningTeams = 0;
	short iWinningTeams[4] = {-1, -1, -1, -1};

	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		if(game_values.tournament_scores[iTeam].wins == 0)
		{
			iWinningTeams[iNumWinningTeams++] = iTeam;
		}
	}

	//Adjust tied scores so display is right
	for(short iMyTeam = 0; iMyTeam < iNumTeams; iMyTeam++)
	{
		for(short iTheirTeam = 0; iTheirTeam < iNumTeams; iTheirTeam++)
		{
			if(iMyTeam == iTheirTeam)
				continue;

			if(game_values.tournament_scores[iMyTeam].wins == game_values.tournament_scores[iTheirTeam].wins)
			{
				game_values.tournament_scores[iTheirTeam].wins++;
			}
		}
	}

	//There was a single team winner
	if(game_values.tournamentwinner != -1)
	{
		if(iNumWinningTeams == 1)
			iTournamentWinner = iWinningTeams[0];
		else
			iTournamentWinner = -2;

		//Flash the background of the winning teams
		for(short iTeam = 0; iTeam < iNumWinningTeams; iTeam++)
			miTeamImages[iWinningTeams[iTeam]]->SetAnimationSpeed(20);
	}
	

	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		short iTeamY = GetYFromPlace(game_values.tournament_scores[iTeam].wins);
		
		if(fTour)
			iTeamY += 28; //shift down 18 pxls for extra tour points bar

		float dSpacing = GetIconSpacing();

		miTeamImages[iTeam]->SetPosition(ix - 40, iTeamY);

		for(short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++)
		{
			miPlayerImages[iTeam][iPlayer]->SetPosition(ix + iScoreboardPlayerOffsetsX[iTeamCounts[iTeam] - 1][iPlayer] - 40, iTeamY + 16);
		}

		if(game_values.matchtype != MATCH_TYPE_WORLD)
		{
			for(short iGame = 0; iGame < game_values.tourstopcurrent; iGame++)
			{
				miIconImages[iTeam][iGame]->SetImage(fTour ? 0 : game_values.tournament_scores[iTeam].type[iGame] * 32, fTour ? game_values.tournament_scores[iTeam].type[iGame] * 32 : 0, 32, 32);

				miIconImages[iTeam][iGame]->SetPosition(ix + 128 + (short)dSpacing + (short)((float)iGame * (32.0f + dSpacing)) - 40, iTeamY + 16);

				miIconImages[iTeam][iGame]->SetSwirl(false, 0.0f, 0.0f, 0.0f, 0.0f);
				miIconImages[iTeam][iGame]->SetPulse(false);
				miIconImages[iTeam][iGame]->Show(true);
			}

			for(short iGame = game_values.tourstopcurrent; iGame < game_values.tourstoptotal; iGame++)
			{
				miIconImages[iTeam][iGame]->Show(false);
			}
		}

		tourScores[iTeam]->SetPosition(ix + 508, iTeamY + 24);
		tourScores[iTeam]->SetScore(game_values.tournament_scores[iTeam].total);
	}
}

//Called by Tournament -- Keeps players where they are and displays number of wins and mode type of win
void MI_TournamentScoreboard::RefreshScores(short gameWinner)
{
	iGameWinner = gameWinner;
	iSwirlIconTeam = -1;
	iSwirlIconGame = -1;

	if(game_values.tournament_scores[iGameWinner].wins == iNumGames)
	{
		iTournamentWinner = iGameWinner;
		miTeamImages[iTournamentWinner]->SetAnimationSpeed(20);
	}

	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		for(short iGame = 0; iGame < game_values.tournament_scores[iTeam].wins; iGame++)
		{
			miIconImages[iTeam][iGame]->SetImage(game_values.tournament_scores[iTeam].type[iGame] * 32, 0, 32, 32);

			if(iTournamentWinner < 0 && iGameWinner == iTeam && iGame == game_values.tournament_scores[iTeam].wins - 1)
			{
				iSwirlIconTeam = iTeam;
				iSwirlIconGame = iGame;
				
				miIconImages[iTeam][iGame]->SetSwirl(true, 250.0f, (float)(rand() % 1000) * TWO_PI / 1000.0f, 3.0f, 0.1f);
			}
				
			miIconImages[iTeam][iGame]->SetPulse(false);
			miIconImages[iTeam][iGame]->Show(true);
		}

		for(short iGame = game_values.tournament_scores[iTeam].wins; iGame < iNumGames; iGame++)
		{
			miIconImages[iTeam][iGame]->Show(false);
		}
	}
}

void MI_TournamentScoreboard::StopSwirl()
{
	if(iSwirlIconTeam >= 0)
	{
		miIconImages[iSwirlIconTeam][iSwirlIconGame]->StopSwirl();
		iSwirlIconTeam = -1;
		iSwirlIconGame = -1;
	}
}

/**************************************
 * MI_BonusWheel Class
 **************************************/

//Call with x = 144 and y == 64
//The spr_player full set of sprites needs to be created via loadfullskin() to use this class
MI_BonusWheel::MI_BonusWheel(gfxSprite * spr_background, gfxSprite * spr_icons, short x, short y) :
	UI_Control(x, y)
{
	spr = spr_background;
	miPlayerImages = NULL;

	miBonusImages = new MI_Image * [NUM_POWERUPS - 1];

	for(short iImage = 0; iImage < NUM_POWERUPS - 1; iImage++)
		miBonusImages[iImage] = new MI_Image(spr_icons, 0, 0, (iImage + 1) * 32, 0, 32, 32, 1, 1, 0);

	miContinueButton = new MI_Button(&menu_plain_field, ix + 76, iy + 390, "Continue", 200, 1);
	miContinueButton->Show(false);
	miContinueButton->SetCode(MENU_CODE_BONUS_DONE);

	//Reset(true);
}

MI_BonusWheel::~MI_BonusWheel()
{
	if(miBonusImages)
	{
		for(int iImage = 0; iImage < NUM_POWERUPS - 1; iImage++)
			delete miBonusImages[iImage];
		
		delete [] miBonusImages;
	}

	if(miPlayerImages)
	{
		for(int iPlayer = 0; iPlayer < iNumPlayers; iPlayer++)
			delete miPlayerImages[iPlayer];
		
		delete [] miPlayerImages;
	}
}

MenuCodeEnum MI_BonusWheel::Modify(bool fModify)
{
	if(fPowerupSelectionDone)
		return miContinueButton->Modify(fModify);

	return MENU_CODE_NONE;
}

void MI_BonusWheel::Update()
{
	for(int iImage = 0; iImage < NUM_POWERUPS - 1; iImage++)
	{
		miBonusImages[iImage]->Update();
	}

	for(int iPlayer = 0; iPlayer < iNumPlayers; iPlayer++)
	{
		miPlayerImages[iPlayer]->Update();
	}

	miContinueButton->Update();

	if(++iSelectorAnimationCounter > 8)
	{
		iSelectorAnimationCounter = 0;
		
		if(++iSelectorAnimation > 1)
			iSelectorAnimation = 0;
	}

	dSelectionSpeed -= 0.0005f;

	if(dSelectionSpeed <= 0.0f)
	{
		dSelectionSpeed = 0.0f;
		
		if(!fPowerupSelectionDone)
		{
			fPowerupSelectionDone = true;
			miContinueButton->Show(true);
			miContinueButton->Select(true);

			//Reset all player's stored item
			if(!game_values.keeppowerup)
			{
				for(short iPlayer = 0; iPlayer < 4; iPlayer++)
					game_values.storedpowerups[iPlayer] = -1;
			}

			//Give the newly won stored item to the winning players
			for(short iPlayer = 0; iPlayer < game_values.teamcounts[iWinningTeam]; iPlayer++)
				game_values.storedpowerups[game_values.teamids[iWinningTeam][iPlayer]] = iSelectedPowerup;

			ifsoundonplay(sfx_collectpowerup);
		}
	}

	dSelectionAngle += dSelectionSpeed;
}
		
void MI_BonusWheel::Draw()
{
	if(!fShow)
		return;

	spr->draw(ix, iy);

	short iSelectorX = ix + 144 + (short)(130.0f * cos(dSelectionAngle));
	short iSelectorY = iy + 192 + (short)(120.0f * sin(dSelectionAngle));

	spr_powerupselector.draw(iSelectorX, iSelectorY, iSelectorAnimation * 64, 0, 64, 64);

	for(int iImage = 0; iImage < NUM_POWERUPS - 1; iImage++)
	{
		miBonusImages[iImage]->Draw();
	}

	for(int iPlayer = 0; iPlayer < iNumPlayers; iPlayer++)
	{
		miPlayerImages[iPlayer]->Draw();
	}

	miContinueButton->Draw();
}

void MI_BonusWheel::Reset(bool fTournament)
{
	if(fTournament)
		iWinningTeam = game_values.tournamentwinner;
	else
		iWinningTeam = game_values.gamemode->winningteam;

	//Randomly display the powerups around the ring
	bool fPowerupUsed[NUM_POWERUPS - 1];
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS - 1; iPowerup++)
		fPowerupUsed[iPowerup] = false;

	for(short iPowerup = 0; iPowerup < NUM_POWERUPS - 1; iPowerup++)
	{
		short iChoosePowerup = (short)(rand() % (NUM_POWERUPS - 1));

		while(fPowerupUsed[iChoosePowerup])
		{
			if(++iChoosePowerup >= NUM_POWERUPS - 1)
				iChoosePowerup = 0;
		}

		fPowerupUsed[iChoosePowerup] = true;

		iPowerupOrder[iChoosePowerup] = iPowerup;

		float dAngle = (float)iPowerup * TWO_PI / (float)(NUM_POWERUPS - 1);
		short iPowerupX = ix + 160 + (short)(130.0f * cos(dAngle));
		short iPowerupY = iy + 210 + (short)(120.0f * sin(dAngle));

		miBonusImages[iChoosePowerup]->SetPosition(iPowerupX, iPowerupY);
	}
	
	//Delete the old winner images
	if(miPlayerImages)
	{
		for(short iPlayer = 0; iPlayer < iNumPlayers; iPlayer++)
			delete miPlayerImages[iPlayer];

		delete [] miPlayerImages;
	}

	iNumPlayers = game_values.teamcounts[iWinningTeam];

	miPlayerImages = new MI_Image * [iNumPlayers];

	short iPlayerX = ix + 160 - ((iNumPlayers - 1) * 17);
	for(short iPlayer = 0; iPlayer < iNumPlayers; iPlayer++)
	{
		miPlayerImages[iPlayer] = new MI_Image(spr_player[game_values.teamids[iWinningTeam][iPlayer]][PGFX_JUMPING_R], iPlayerX, iy + 210, 0, 0, 32, 32, 1, 1, 0);
		iPlayerX += 34;
	}

	short iCountWeight = 0;
	for(short iPowerup = 1; iPowerup < NUM_POWERUPS; iPowerup++)
		iCountWeight += game_values.powerupweights[iPowerup];

	if(iCountWeight == 0)
		iCountWeight = 1;

	int iRandPowerup = rand() % iCountWeight + 1;
	iSelectedPowerup = 1;
	int iPowerupWeightCount = game_values.powerupweights[iSelectedPowerup];

	while(iPowerupWeightCount < iRandPowerup)
		iPowerupWeightCount += game_values.powerupweights[++iSelectedPowerup];

	fPowerupSelectionDone = false;

	iSelectorAnimation = 0;
	iSelectorAnimationCounter = 0;
	dSelectionSpeed = 0.0f;
	dSelectionAngle = (float)(iPowerupOrder[iSelectedPowerup - 1]) * TWO_PI / (float)(NUM_POWERUPS - 1);
	
	//wind back the selection angle
	int iRandomWindBack = rand() % 50 + 250;
	for(int iWindback = 0; iWindback < iRandomWindBack; iWindback++)
	{
		dSelectionAngle -= dSelectionSpeed;
		dSelectionSpeed += 0.0005f;
	}

	miContinueButton->Show(false);
}

/**************************************
 * MI_ScreenResize Class
 **************************************/

#ifdef _XBOX
MI_ScreenResize::MI_ScreenResize() :
	UI_Control(0, 0)
{
	miText[0] = new MI_Text("Use left joystick to set", 320, 110, 0, 2, 1);
	miText[1] = new MI_Text("upper left corner.", 320, 135, 0, 2, 1);
	miText[2] = new MI_Text("Use right joystick to set", 320, 190, 0, 2, 1);
	miText[3] = new MI_Text("lower right corner.", 320, 215, 0, 2, 1);
	miText[4] = new MI_Text("Press random to snap", 320, 270, 0, 2, 1);
	miText[5] = new MI_Text("to preset screen size.", 320, 295, 0, 2, 1);
	miText[6] = new MI_Text("Press key to exit.", 320, 350, 0, 2, 1);

	fAutoModify = true;
	dPreset = 0.0f;
}

MI_ScreenResize::~MI_ScreenResize()
{
	for(int iText = 0; iText < 7; iText++)
		delete miText[iText];
}

MenuCodeEnum MI_ScreenResize::Modify(bool modify)
{
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_ScreenResize::SendInput(CPlayerInput * playerInput)
{
	SDL_Event event;
	bool done = false;

	short iLastJoy1X = 0, iLastJoy1Y = 0, iLastJoy2X = 0, iLastJoy2Y = 0;

	bool resized = false;
	while (!done)
	{
		game_values.playerInput.ClearPressedKeys(1);

		while(SDL_PollEvent(&event))
		{
			game_values.playerInput.Update(event, 1);

			for(int iPlayer = 0; iPlayer < 4; iPlayer++)
			{
				if (playerInput->outputControls[iPlayer].menu_cancel.fPressed ||
					playerInput->outputControls[iPlayer].menu_select.fPressed)
				{
					done = true;
					break;
				}
				else if (playerInput->outputControls[iPlayer].menu_random.fPressed)
				{
					if(dPreset * 4.0f == game_values.screenResizeX && dPreset * 3.0f == game_values.screenResizeY && 
						dPreset * -8.0f == game_values.screenResizeW && dPreset * -6.0f == game_values.screenResizeH)
					{
						dPreset += 1.0f;
						if(dPreset > 21.0f)
							dPreset = 0.0f;
					}

					game_values.screenResizeX = dPreset * 4.0f;
					game_values.screenResizeY = dPreset * 3.0f;
					game_values.screenResizeW = dPreset * -8.0f;
					game_values.screenResizeH = dPreset * -6.0f;
					resized = true;

					break;
				}
			}

			if (event.type == SDL_JOYAXISMOTION)
			{
				if(event.jaxis.axis == 0)
					iLastJoy1X = event.jaxis.value;
				else if(event.jaxis.axis == 1)
					iLastJoy1Y = event.jaxis.value;
				else if(event.jaxis.axis == 2)
					iLastJoy2X = event.jaxis.value;
				else if(event.jaxis.axis == 3)
					iLastJoy2Y = event.jaxis.value;
			}
		}
		
		if(iLastJoy1X > JOYSTICK_DEAD_ZONE || iLastJoy1X < -JOYSTICK_DEAD_ZONE)
		{
			game_values.screenResizeX += (float)iLastJoy1X / 50000.0f;
			resized = true;
		}

		if(iLastJoy1Y > JOYSTICK_DEAD_ZONE || iLastJoy1Y < -JOYSTICK_DEAD_ZONE)
		{
			game_values.screenResizeY += (float)iLastJoy1Y / 50000.0f;
			resized = true;
		}

		if(iLastJoy2X > JOYSTICK_DEAD_ZONE || iLastJoy2X < -JOYSTICK_DEAD_ZONE)
		{
			game_values.screenResizeW += (float)iLastJoy2X / 50000.0f;
			resized = true;
		}

		if(iLastJoy2Y > JOYSTICK_DEAD_ZONE || iLastJoy2Y < -JOYSTICK_DEAD_ZONE)
		{
			game_values.screenResizeH += (float)iLastJoy2Y / 50000.0f;
			resized = true;
		}
		
		if(resized)
		{
			resized = false;
			SDL_XBOX_SetScreenStretch(game_values.screenResizeW, game_values.screenResizeH);
			SDL_XBOX_SetScreenPosition(game_values.screenResizeX, game_values.screenResizeY);
		}
	}

	game_values.playerInput.ResetKeys();

	fModifying = false;
	return MENU_CODE_BACK_TO_SCREEN_SETTINGS_MENU;
}

void MI_ScreenResize::Draw()
{
	if(!fShow)
		return;

	for(int iText = 0; iText < 7; iText++)
		miText[iText]->Draw();
}
#endif //_XBOX

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
}

MI_Image::~MI_Image()
{}

void MI_Image::Update()
{
	if(!fShow)
		return;

	if(iSpeed > 0 && ++iTimer >= iSpeed)
	{
		iTimer = 0;
		iXFrame += iw;

		if(iXFrame >= iNumXFrames * iw + isrcx)
		{
			iXFrame = isrcx;
			iYFrame += ih;
		
			if(iYFrame >= iNumYFrames * ih + isrcy)
			{
				iYFrame = isrcy;
			}
		}
	}

	if(fPulse)
	{
		if(++iPulseDelay >= 3)
		{
			iPulseDelay = 0;

			if(fPulseOut)
			{
				if(++iPulseValue >= 10)
				{
					fPulseOut = false;
				}
			}
			else
			{
				if(--iPulseValue <= 0)
				{
					fPulseOut = true;
				}
			}
		}
	}

	if(fSwirl)
	{
		dSwirlRadius -= dSwirlRadiusSpeed;

		if(dSwirlRadius <= 0.0f)
		{
			fSwirl = false;
		}
		else
		{
			dSwirlAngle += dSwirlAngleSpeed;
		}
	}
}

void MI_Image::Draw()
{
	if(!fShow)
		return;

	short iXOffset = 0;
	short iYOffset = 0;

	if(fSwirl)
	{
		iXOffset = (short)(dSwirlRadius * cos(dSwirlAngle));
		iYOffset = (short)(dSwirlRadius * sin(dSwirlAngle));
	}

	if(fPulse)
		spr->drawStretch(ix - iPulseValue + iXOffset, iy - iPulseValue + iYOffset, iw + (iPulseValue << 1), ih + (iPulseValue << 1), iXFrame, iYFrame, iw, ih);
	else
		spr->draw(ix + iXOffset, iy + iYOffset, iXFrame, iYFrame, iw, ih);
}


/**************************************
 * MI_Text Class
 **************************************/

MI_Text::MI_Text(char * text, short x, short y, short w, short size, short justified) :
	UI_Control(x, y)
{
	szText = new char[strlen(text) + 1];
	strcpy(szText, text);

	iw = w;
	iJustified = justified;
	
	if(size == 0)
		font = &menu_font_small;
	else
		font = &menu_font_large;
}

MI_Text::~MI_Text()
{}

void MI_Text::SetText(char * text)
{
	delete [] szText;
	szText = new char[strlen(text) + 1];
	strcpy(szText, text);	
}

void MI_Text::Draw()
{
	if(!fShow)
		return;

	if(iJustified == 0 && iw == 0)
		font->draw(ix, iy, szText);
	else if(iJustified == 0)
		font->drawChopRight(ix, iy, iw, szText);
	else if(iJustified == 1)
		font->drawCentered(ix, iy, szText);
	else if(iJustified == 2)
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
	spr_scoretext.draw(iDigitRightDstX, iy, iDigitRightSrcX, 0, 16, 16);
	
	if(iDigitLeftSrcX > 0)
	{
		spr_scoretext.draw(iDigitMiddleDstX, iy, iDigitMiddleSrcX, 0, 16, 16);
		spr_scoretext.draw(iDigitLeftDstX, iy, iDigitLeftSrcX, 0, 16, 16);
	}
	else if(iDigitMiddleSrcX > 0)
	{
		spr_scoretext.draw(iDigitMiddleDstX, iy, iDigitMiddleSrcX, 0, 16, 16);
	}
}

void MI_ScoreText::SetScore(short iScore)
{
	short iDigits = iScore;
	while(iDigits > 999)
		iDigits -= 1000;
		
	iDigitLeftSrcX = iDigits / 100 * 16;
	iDigitMiddleSrcX = iDigits % 100 / 10 * 16;
	iDigitRightSrcX = iDigits % 10 * 16;

	if(iDigitLeftSrcX == 0)
	{
		if(iDigitMiddleSrcX == 0)
		{
			iDigitRightDstX = ix - 8;
		}
		else
		{
			iDigitMiddleDstX = ix - 16;
			iDigitRightDstX = ix;
		}
	}
	else
	{
		iDigitLeftDstX = ix - 24;
		iDigitMiddleDstX = ix - 8;
		iDigitRightDstX = ix + 8;
	}
}

/**************************************
 * MI_MapFilterScroll Class
 **************************************/

MI_MapFilterScroll::MI_MapFilterScroll(gfxSprite * nspr, short x, short y, short width, short numlines) :
	UI_Control(x, y)
{
	spr = nspr;
	iWidth = width;
	iNumLines = numlines;

	iSelectedColumn = 0;
	iSelectedLine = 0;
	iIndex = 0;
	iOffset = 0;

	iTopStop = (iNumLines - 1) >> 1;
	iBottomStop = 0;
}

MI_MapFilterScroll::~MI_MapFilterScroll()
{}

void MI_MapFilterScroll::Add(std::string name, short icon)
{
	MFS_ListItem * item = new MFS_ListItem(name, icon, false);
	items.push_back(item);

	if(items.size() >= 1)
	{
		current = items.begin();
		iIndex = 0;
	}

	iBottomStop = items.size() - iNumLines + iTopStop;
}

MenuCodeEnum MI_MapFilterScroll::Modify(bool modify)
{
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_MapFilterScroll::SendInput(CPlayerInput * playerInput)
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_down.fPressed)
		{
			MoveNext();
            return MENU_CODE_NONE;
		}
		
		if(playerInput->outputControls[iPlayer].menu_up.fPressed)
		{
			MovePrev();
			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_left.fPressed)
		{
			iSelectedColumn = 0;
			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_right.fPressed)
		{
			if(iIndex >= NUM_AUTO_FILTERS)
				iSelectedColumn = 1;

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed)
		{
			//If the left column is selected, then turn that filter on/off
			if(iSelectedColumn == 0)
			{
				items[iIndex]->fSelected = !items[iIndex]->fSelected;
				game_values.pfFilters[iIndex] = !game_values.pfFilters[iIndex];
			}
			else //otherwise if the right is selected, go into the details of that filter
			{
				game_values.selectedmapfilter = iIndex;
				return MENU_CODE_TO_MAP_FILTER_EDIT;
			}

			return MENU_CODE_NONE;
		}


		if(playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			return MENU_CODE_MAP_FILTER_EXIT;
		}
	}

	return MENU_CODE_NONE;
}


void MI_MapFilterScroll::Update()
{}

void MI_MapFilterScroll::Draw()
{
	if(!fShow)
		return;

	//Draw the background for the map preview
	menu_dialog.draw(ix, iy, 0, 0, iWidth - 16, iNumLines * 32 + 32);
	menu_dialog.draw(ix + iWidth - 16, iy, 496, 0, 16, iNumLines * 32 + 32);
	menu_dialog.draw(ix, iy + iNumLines * 32 + 32, 0, 464, iWidth - 16, 16);
	menu_dialog.draw(ix + iWidth - 16, iy + iNumLines * 32 + 32, 496, 464, 16, 16);

	menu_font_large.drawCentered(ix + (iWidth >> 1), iy + 5, "Map Filters");

	//Draw each filter field
	for(short iLine = 0; iLine < iNumLines && (unsigned short)iLine < items.size(); iLine++)
	{
		if(iOffset + iLine >= NUM_AUTO_FILTERS)
		{
			short iHalfLineWidth = (iWidth - 64) >> 1;
			short iLineWidth = iWidth - 64;

			spr->draw(ix + 16, iy + 32 + iLine * 32, 0, (iSelectedLine == iLine  && iSelectedColumn == 0 ? 32 : 0), iHalfLineWidth, 32);
			spr->draw(ix + 16 + iHalfLineWidth, iy + 32 + iLine * 32, 512 - iLineWidth + iHalfLineWidth, (iSelectedLine == iLine && iSelectedColumn == 0 ? 32 : 0), iLineWidth - iHalfLineWidth, 32);

			menu_map_filter.draw(ix + iWidth - 48, iy + 32 + iLine * 32, 48, (iSelectedLine == iLine && iSelectedColumn == 1 ? 32 : 0), 32, 32);
		}
		else
		{
			short iHalfLineWidth = (iWidth - 32) >> 1;
			short iLineWidth = iWidth - 32;
			spr->draw(ix + 16, iy + 32 + iLine * 32, 0, (iSelectedLine == iLine && iSelectedColumn == 0 ? 32 : 0), iHalfLineWidth, 32);
			spr->draw(ix + 16 + iHalfLineWidth, iy + 32 + iLine * 32, 512 - iLineWidth + iHalfLineWidth, (iSelectedLine == iLine && iSelectedColumn == 0 ? 32 : 0), iLineWidth - iHalfLineWidth, 32);
		}

		if(items[iOffset + iLine]->fSelected)
			menu_map_filter.draw(ix + 24, iy + 32 + iLine * 32 + 4, 24, 0, 24, 24);

		menu_font_large.drawChopRight(ix + 52, iy + 5 + iLine * 32 + 32, iWidth - 104, items[iOffset + iLine]->sName.c_str());
		spr_map_filter_icons.draw(ix + 28, iy + 32 + iLine * 32 + 8, items[iOffset + iLine]->iIcon % 10 * 16, items[iOffset + iLine]->iIcon / 10 * 16, 16, 16);
	}
}

bool MI_MapFilterScroll::MoveNext()
{
	if(items.empty())
		return false;

	if(iIndex == items.size() - 1)
		return false;

	iIndex++;
	current++;

	if(iIndex > iTopStop && iIndex <= iBottomStop)
		iOffset++;
	else
		iSelectedLine++;

	return true;
}

bool MI_MapFilterScroll::MovePrev()
{
	if(items.empty())
		return false;

	if(iIndex == 0)
		return false;

	iIndex--;
	current--;

	if(iIndex >= iTopStop && iIndex < iBottomStop)
		iOffset--;
	else
		iSelectedLine--;

	if(iIndex < NUM_AUTO_FILTERS)
		iSelectedColumn = 0;

	return true;
}


/**************************************
 * MI_MapBrowser Class
 **************************************/

MI_MapBrowser::MI_MapBrowser() : 
	UI_Control(0, 0)
{
	for(short iSurface = 0; iSurface < 9; iSurface++)
		mapSurfaces[iSurface] = NULL;

	srcRectBackground.x = 0;
	srcRectBackground.y = 0;
	srcRectBackground.w = 640;
	srcRectBackground.h = 480;

	dstRectBackground.x = 0;
	dstRectBackground.y = 0;
	dstRectBackground.w = 160;
	dstRectBackground.h = 120;

	iFilterTagAnimationFrame = 0;
}

MI_MapBrowser::~MI_MapBrowser()
{
	for(short iSurface = 0; iSurface < 9; iSurface++)
	{
		if(mapSurfaces[iSurface])
		{
			SDL_FreeSurface(mapSurfaces[iSurface]);
			mapSurfaces[iSurface] = NULL;
		}
	}
}

void MI_MapBrowser::Update()
{
	if(++iFilterTagAnimationTimer > 8)
	{
		iFilterTagAnimationTimer = 0;
		
		iFilterTagAnimationFrame += 24;
		if(iFilterTagAnimationFrame > 24)
			iFilterTagAnimationFrame = 0;
	}
}

void MI_MapBrowser::Draw()
{
	if(!fShow)
		return;

	SDL_Rect rSrc = {0, 0, 160, 120};
	SDL_Rect rDst = {0, 0, 160, 120};

	for(short iRow = 0; iRow < 3; iRow++)
	{
		rDst.y = iRow * 150 + 30;

		for(short iCol = 0; iCol < 3; iCol++)
		{
			if(iSelectedCol != iCol || iSelectedRow != iRow)
			{
				if(iRow * 3 + iCol + iPage * 9 >= iMapCount)
					break;

				rDst.x = iCol * 200 + 40;

				SDL_BlitSurface(mapSurfaces[iRow * 3 + iCol], &rSrc, blitdest, &rDst);

				if(iType == 0)
				{
					if(mapListNodes[iRow * 3 + iCol]->pfFilters[game_values.selectedmapfilter])
						menu_map_filter.draw(rDst.x, rDst.y, iFilterTagAnimationFrame, 24, 24, 24);
				}

				menu_font_large.drawChopRight(rDst.x, rDst.y + 120, 165, mapNames[iRow * 3 + iCol]);
			}
		}
	}

	//Draw the selected map
	rDst.y = iSelectedRow * 150 + 30;
	rDst.x = iSelectedCol * 200 + 40;

	menu_dialog.draw(rDst.x - 16, rDst.y - 16, 0, 0, 176, 148);
	menu_dialog.draw(rDst.x + 160, rDst.y - 16, 496, 0, 16, 148);
	menu_dialog.draw(rDst.x - 16, rDst.y + 132, 0, 464, 176, 16);
	menu_dialog.draw(rDst.x + 160, rDst.y + 132, 496, 464, 16, 16);

	SDL_BlitSurface(mapSurfaces[iSelectedRow * 3 + iSelectedCol], &rSrc, blitdest, &rDst);

	if(iType == 0)
	{
		if(mapListNodes[iSelectedRow * 3 + iSelectedCol]->pfFilters[game_values.selectedmapfilter])
			menu_map_filter.draw(rDst.x, rDst.y, iFilterTagAnimationFrame, 24, 24, 24);
	}

	menu_font_large.drawChopRight(rDst.x, rDst.y + 120, 165, mapNames[iSelectedRow * 3 + iSelectedCol]);
}

MenuCodeEnum MI_MapBrowser::Modify(bool modify)
{
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_MapBrowser::SendInput(CPlayerInput * playerInput)
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		if(playerInput->outputControls[iPlayer].menu_down.fPressed)
		{
			short iSkipRows = 1;
			if(playerInput->outputControls[iPlayer].menu_scrollfast.fDown)
				iSkipRows = 3;

			iSelectedIndex = (iSelectedRow + iSkipRows) * 3 + iSelectedCol + iPage * 9;
			
			if(iSelectedIndex >= iMapCount)
				iSelectedIndex = iMapCount - 1;
			
			iSelectedRow = (iSelectedIndex / 3) % 3;
			iSelectedCol = iSelectedIndex % 3;

			short iOldPage = iPage;
			iPage = iSelectedIndex / 9;

			if(iOldPage != iPage)
				LoadPage(iPage, iType == 1);

            return MENU_CODE_NONE;
		}
		
		if(playerInput->outputControls[iPlayer].menu_up.fPressed)
		{
			short iSkipRows = 1;
			if(playerInput->outputControls[iPlayer].menu_scrollfast.fDown)
				iSkipRows = 3;

			iSelectedIndex = (iSelectedRow - iSkipRows) * 3 + iSelectedCol + iPage * 9;
			
			if(iSelectedIndex < 0)
				iSelectedIndex = 0;

			iSelectedRow = (iSelectedIndex / 3) % 3;
			iSelectedCol = iSelectedIndex % 3;

			short iOldPage = iPage;
			iPage = iSelectedIndex / 9;

			if(iOldPage != iPage)
				LoadPage(iPage, iType == 1);

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_left.fPressed)
		{
			short iNextIndex = iSelectedRow * 3 + iSelectedCol - 1 + iPage * 9;
			
			if(iNextIndex < 0)
				return MENU_CODE_NONE;

			iSelectedIndex = iNextIndex;
			
			if(--iSelectedCol < 0)
			{
				iSelectedCol = 2;
				
				if(--iSelectedRow < 0)
				{
					iSelectedRow = 2;
					LoadPage(--iPage, iType == 1);
				}
			}

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_right.fPressed)
		{
			short iNextIndex = iSelectedRow * 3 + iSelectedCol + 1 + iPage * 9;
			
			if(iNextIndex >= iMapCount)
				return MENU_CODE_NONE;

			iSelectedIndex = iNextIndex;
			
			if(++iSelectedCol > 2)
			{
				iSelectedCol = 0;

				if(++iSelectedRow > 2)
				{
					iSelectedRow = 0;
					LoadPage(++iPage, iType == 1);
				}
			}

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_select.fPressed)
		{
			if(iType == 0)
			{
				mapListNodes[iSelectedRow * 3 + iSelectedCol]->pfFilters[game_values.selectedmapfilter] = !mapListNodes[iSelectedRow * 3 + iSelectedCol]->pfFilters[game_values.selectedmapfilter];
				game_values.fNeedWriteFilters = true;
			}
			else
			{
				maplist.SetCurrent(mapListItr[iSelectedRow * 3 + iSelectedCol]);
				return MENU_CODE_MAP_BROWSER_EXIT;
			}

			return MENU_CODE_NONE;
		}

		if(playerInput->outputControls[iPlayer].menu_cancel.fPressed)
		{
			return MENU_CODE_MAP_BROWSER_EXIT;
		}
	}

	return MENU_CODE_NONE;
}

void MI_MapBrowser::Reset(short type)
{
	iType = type;

	if(iType == 0)
	{
		iSelectedIndex = maplist.GetCurrent()->second->iIndex;

		iFilterTagAnimationTimer = 0;
		iFilterTagAnimationFrame = 72;

		iMapCount = maplist.GetCount();
	}
	else
	{
		iSelectedIndex = maplist.GetCurrent()->second->iFilteredIndex;
		iMapCount = maplist.GetFilteredCount();
	}

	iSelectedRow = (iSelectedIndex / 3) % 3;
	iSelectedCol = iSelectedIndex % 3;
	iPage = iSelectedIndex / 9;

	LoadPage(iPage, iType == 1);
}

void MI_MapBrowser::LoadPage(short page, bool fUseFilters)
{
	for(short iMap = 0; iMap < 9; iMap++)
	{
		short iIndex = iMap + page * 9;

		if(iIndex >= iMapCount)
			return;

		std::map<std::string, MapListNode*>::iterator itr = maplist.GetIteratorAt(iIndex, fUseFilters);
		
		//See if we already have a thumbnail saved for this map

		char szThumbnail[256];
		strcpy(szThumbnail, "maps/cache/");
		char * pszThumbnail = szThumbnail + strlen(szThumbnail);
		GetNameFromFileName(pszThumbnail, (*itr).second->filename.c_str());

#ifdef PNG_SAVE_FORMAT
		strcat(szThumbnail, ".png");
#else
		strcat(szThumbnail, ".bmp");
#endif

		std::string sConvertedPath = convertPath(szThumbnail);

		if(!File_Exists(sConvertedPath))
		{
			g_map.loadMap((*itr).second->filename, read_type_preview);
			SDL_Delay(10);  //Sleeps to help the music from skipping
			g_map.saveThumbnail(sConvertedPath, false);
			SDL_Delay(10);
		}
		
		mapSurfaces[iMap] = IMG_Load(sConvertedPath.c_str());

		mapListNodes[iMap] = (*itr).second;
		mapNames[iMap] = (*itr).first.c_str();
		mapListItr[iMap] = itr;
	}
}


/**************************************
 * MI_World Class
 **************************************/

MI_World::MI_World() :
	UI_Control(0, 0)
{
	iControllingTeam = 0;
	iReturnDirection = 0;

	sMapSurface = SDL_CreateRGBSurface(screen->flags, 768, 608, screen->format->BitsPerPixel, 0, 0, 0, 0);
	
	rectSrcSurface = new SDL_Rect();
	rectSrcSurface->x = 0;
	rectSrcSurface->y = 0;
	rectSrcSurface->w = 768;
	rectSrcSurface->h = 608;

	rectDstSurface = new SDL_Rect();
	rectDstSurface->x = 0;
	rectDstSurface->y = 0;
	rectDstSurface->w = 640;
	rectDstSurface->h = 480;
}

MI_World::~MI_World()
{
	SDL_FreeSurface(sMapSurface);
	delete rectSrcSurface;
	delete rectDstSurface;
}

void MI_World::Init()
{
	iAnimationTimer = 0;
	iAnimationFrame = 0;
	
	g_worldmap.InitPlayer();

	iMapDrawOffsetCol = 0;
	iMapDrawOffsetRow = 0;

	iMessageTimer = 0;

	iState = -2;
	iTeam = -1;
	iStateTransition = 0;
	iItemPopupDrawY = 0;

	iVehicleId = -1;

	SetMapOffset();
	RepositionMapImage();

	dTeleportStarRadius = 300.0f;
	dTeleportStarAngle = 0.0f;
	iTeleportStarAnimationFrame = 0;
	iTeleportStarAnimationTimer = 0;

	fForceStageStart = false;
	fNoInterestingMoves = false;
	iSleepTurns = 0;
}

void MI_World::SetControllingTeam(short iTeamID)
{
	iControllingTeam = iTeamID;
	g_worldmap.SetPlayerSprite(game_values.teamids[iControllingTeam][rand() % game_values.teamcounts[iControllingTeam]]);

	iMessageTimer = 120;

	if(game_values.teamcounts[iTeamID] <= 1)
		sprintf(szMessage, "Player %d Is In Control", game_values.teamids[iTeamID][0] + 1);
	else
		sprintf(szMessage, "Team %d Is In Control", iTeamID + 1);

	fNoInterestingMoves = false;
}

void MI_World::SetCurrentStageToCompleted(short iWinningTeam)
{
	if(iVehicleId >= 0)
	{
		game_values.tournament_scores[iWinningTeam].total += g_worldmap.GetVehicleStageScore(iVehicleId);
		g_worldmap.RemoveVehicle(iVehicleId);
	}
	else
	{
		short iPlayerCurrentTileX, iPlayerCurrentTileY;
		g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

		WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];
		tile->iForegroundSprite = iWinningTeam + WORLD_WINNING_TEAM_SPRITE_OFFSET; //Update with team completed sprite
		tile->fAnimated = false; //Update with team completed sprite
		tile->fCompleted = true;

		game_values.tournament_scores[iWinningTeam].total += game_values.tourstops[tile->iType - 6]->iPoints;
	}

	AdvanceTurn();
}

void MI_World::AdvanceTurn()
{
	if(iSleepTurns > 0)
		iSleepTurns--;
	else
		g_worldmap.MoveVehicles();

	g_worldmap.MoveBridges();

	//Update the completed stage with team colored tile on the map
	UpdateMapSurface();

	fNoInterestingMoves = false;
}

void MI_World::Update()
{
	bool fPlayerVehicleCollision = false;
	bool fPlayerMoveDone = g_worldmap.Update(&fPlayerVehicleCollision);

	short iPlayerX, iPlayerY;
	g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);

	short iPlayerState = g_worldmap.GetPlayerState();

	if(++iAnimationTimer > 15)
	{
		iAnimationTimer = 0;
		iAnimationFrame += TILESIZE;

		if(iAnimationFrame >= 128)
			iAnimationFrame = 0;

		//update background map surface
		UpdateMapSurface();
	}

	if(fPlayerMoveDone)
		RepositionMapImage();

	if(iSleepTurns <= 0 && !fUsingCloud && (fPlayerVehicleCollision || fPlayerMoveDone))
	{
		short iStage = g_worldmap.GetVehicleInPlayerTile(&iVehicleId);
		if(iStage >= 0)
		{
			game_values.tourstopcurrent = iStage;
			fForceStageStart = true;
		}
	}

	//Player is moving from one tile to the next (up)
	if(iPlayerState == 1)
	{
		if(g_worldmap.iHeight > 15 && iMapOffsetY < 0 && iPlayerY < g_worldmap.iHeight * TILESIZE - 256)
			iMapOffsetY += 2;
	}
	else if(iPlayerState == 2) //down
	{
		if(g_worldmap.iHeight > 15 && iMapOffsetY > 480 - g_worldmap.iHeight * TILESIZE && iPlayerY > 224)
			iMapOffsetY -= 2;
	}
	else if(iPlayerState == 3) //left
	{
		if(g_worldmap.iWidth > 20 && iMapOffsetX < 0 && iPlayerX < g_worldmap.iWidth * TILESIZE - 336)
			iMapOffsetX += 2;
	}
	else if(iPlayerState == 4) //right
	{
		if(g_worldmap.iWidth > 20 && iMapOffsetX > 640 - g_worldmap.iWidth * TILESIZE && iPlayerX > 304)
			iMapOffsetX -= 2;
	}

	if(iState == -2 || iState >= 4)
	{
		dTeleportStarRadius += (iState == 4 || iState == 6 ? 5.0f : -5.0f);
		dTeleportStarAngle -= 0.15f;

		if(++iTeleportStarAnimationTimer >= 3)
		{
			iTeleportStarAnimationTimer = 0;
			iTeleportStarAnimationFrame += 32;
			
			if(iTeleportStarAnimationFrame > 32)
				iTeleportStarAnimationFrame = 0;
		}

		if(dTeleportStarRadius <= 0.0f)
		{
			if(iState == -2 || iState == 7)
				iState = -1;

			dTeleportStarRadius = 0.0f;
		}
	}
	
	if(iState >= 0 && iState <= 3)
	{
		if(iStateTransition == 1)
		{
			iItemPopupDrawY += 4;

			if(iItemPopupDrawY >= 32)
			{
				iItemPopupDrawY = 32;
				iStateTransition = 0;
			}
		}
		else if(iStateTransition == 2)
		{
			iItemPopupDrawY -= 4;

			if(iItemPopupDrawY <= 0)
			{
				iState = -1;

				iItemPopupDrawY = 0;
				iStateTransition = 0;
			}
		}
	}
	else if(iState == 4 || iState == 5)
	{
		iScreenfade += iScreenfadeRate;

		if(iState == 4 && iScreenfade > 255)
		{
			g_worldmap.SetPlayerPosition(iWarpCol, iWarpRow);
			SetMapOffset();
			RepositionMapImage();

			iState = 5;
			iScreenfade = 255;
			iScreenfadeRate = -8;
		}
		else if(iState == 5 && iScreenfade < 0)
		{
			iState = -1;
			iScreenfade = 0;
		}
	}
	else if(iState == 6)
	{
		if(dTeleportStarRadius > 150.0f)
			iState = 7;
	}
}

void MI_World::UpdateMapSurface()
{
	g_worldmap.DrawMapToSurface(true, sMapSurface, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
}

void MI_World::SetMapOffset()
{
	short iPlayerX, iPlayerY;
	g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);

	if(g_worldmap.iWidth > 20)
	{
		if(iPlayerX < g_worldmap.iWidth * TILESIZE - 336 && iPlayerX > 304)
			iMapOffsetX = 304 - iPlayerX;
		else if(iPlayerX <= 304)
			iMapOffsetX = 0;
		else
			iMapOffsetX = 640 - g_worldmap.iWidth * TILESIZE;
	}
	else
	{
		iMapOffsetX = (640 - g_worldmap.iWidth * TILESIZE) >> 1;
	}

	if(g_worldmap.iHeight > 15)
	{
		if(iPlayerY < g_worldmap.iHeight * TILESIZE - 256 && iPlayerY > 224)
			iMapOffsetY = 224 - iPlayerY;
		else if(iPlayerY <= 224)
			iMapOffsetY = 0;
		else
			iMapOffsetY = 480 - g_worldmap.iHeight * TILESIZE;
	}
	else
	{
		iMapOffsetY = (480 - g_worldmap.iHeight * TILESIZE) >> 1;
	}
}

void MI_World::RepositionMapImage()
{
	short iPlayerCurrentTileX, iPlayerCurrentTileY;
	g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

	if(g_worldmap.iWidth > 24)
	{
		iMapDrawOffsetCol = iPlayerCurrentTileX - 11;
		if(iMapDrawOffsetCol < 0)
			iMapDrawOffsetCol = 0;
		else if(iMapDrawOffsetCol > g_worldmap.iWidth - 24)
			iMapDrawOffsetCol = g_worldmap.iWidth - 24;
	}

	if(g_worldmap.iHeight > 19)
	{
		iMapDrawOffsetRow = iPlayerCurrentTileY - 9;
		if(iMapDrawOffsetRow < 0)
			iMapDrawOffsetRow = 0;
		else if(iMapDrawOffsetRow > g_worldmap.iHeight - 19)
			iMapDrawOffsetRow = g_worldmap.iHeight - 19;
	}

	UpdateMapSurface();
}


void MI_World::Draw()
{
	if(!fShow)
		return;

	short iPlayerX, iPlayerY;
	g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);

	rectSrcSurface->x = 0;
	rectSrcSurface->y = 0;
	
	if(g_worldmap.iWidth > 24)
		rectSrcSurface->w = 768;
	else
		rectSrcSurface->w = g_worldmap.iWidth * TILESIZE;

	if(g_worldmap.iHeight > 19)
		rectSrcSurface->h = 608;
	else
		rectSrcSurface->h = g_worldmap.iHeight * TILESIZE;

	rectDstSurface->x = iMapOffsetX + iMapDrawOffsetCol * TILESIZE;
	rectDstSurface->y = iMapOffsetY + iMapDrawOffsetRow * TILESIZE;
	
	SDL_BlitSurface(sMapSurface, rectSrcSurface, blitdest, rectDstSurface);

	g_worldmap.Draw(iMapOffsetX, iMapOffsetY, iState != -2 && iState < 4 && !fUsingCloud, iSleepTurns > 0);

	if(fUsingCloud && iState != -2 && iState < 4)
		spr_worlditems.draw(iPlayerX + iMapOffsetX, iPlayerY + iMapOffsetY, 32, 0, 32, 32);

	if(iMessageTimer > 0)
	{
		iMessageTimer--;
		menu_font_large.drawCentered(320, 64, szMessage);
	}

	//If the item selector for a player is displayed
	if(iState == -2 || iState >= 4)
	{
		for(short iStar = 0; iStar < 10; iStar++)
		{
			float dAngle = dTeleportStarAngle + (TWO_PI / 10.0f) * (float)iStar;
			short iStarX = (short)(dTeleportStarRadius * cos(dAngle));
			short iStarY = (short)(dTeleportStarRadius * sin(dAngle));

			spr_teleportstar.draw(iStarX + iPlayerX + iMapOffsetX, iStarY + iPlayerY + iMapOffsetY, iTeleportStarAnimationFrame, 0, 32, 32);
		}
	}
	
	if(iState >= 0 && iState <= 3)
	{
		spr_worlditempopup.draw(0, 448 - iItemPopupDrawY, 0, iState * 64 + 32 - iItemPopupDrawY, 320, iItemPopupDrawY << 1);
		spr_worlditempopup.draw(320, 448 - iItemPopupDrawY, 192, iState * 64 + 32 - iItemPopupDrawY, 320, iItemPopupDrawY << 1);

		if(iStateTransition == 0)
		{
			short iNumPowerups = game_values.worldpowerupcount[iTeam];
			
			if(iNumPowerups > 0)
				spr_worlditempopup.draw(iItemCol * 52 + 114, 424, iState * 48, 256, 48, 48);

			short iStartItem = iItemPage << 3;
			for(short iItem = iStartItem; iItem < iStartItem + 8 && iItem < iNumPowerups; iItem++)
			{
				short iPowerup = game_values.worldpowerups[iTeam][iItem];
				if(iPowerup >= NUM_POWERUPS)
					spr_worlditems.draw((iItem - iItemPage * 8) * 52 + 122, 432, (iPowerup - NUM_POWERUPS) << 5, 0, 32, 32);
				else
					spr_storedpoweruplarge.draw((iItem - iItemPage * 8) * 52 + 122, 432, iPowerup << 5, 0, 32, 32);
			}
		}
	}
	else if(iState == 4 || iState == 5)
	{
		menu_shade.setalpha((Uint8)iScreenfade);
		menu_shade.draw(0, 0);
	}
}

//TODO:: need a way for AI to use world items like keys
//Also, there is a problem if you put a key into a stage but someone exits before it is collected
MenuCodeEnum MI_World::SendInput(CPlayerInput * playerInput)
{
	//Don't allow any input while vehicles are moving
	if(g_worldmap.IsVehicleMoving())
		return MENU_CODE_NONE;

	//If the player and a vehicle collided, force the start of that level
	if(fForceStageStart)
	{
		iState = -1;
		iItemPopupDrawY = 0;
		iStateTransition = 0;

		fForceStageStart = false;
		return MENU_CODE_TOUR_STOP_CONTINUE_FORCED;
	}

	short iPlayerState = g_worldmap.GetPlayerState();

	//Handle AI movement
	bool fNeedAiControl = false;
	if(iState == -1 && iPlayerState == 0)
	{
		if(!fNoInterestingMoves)
		{
			fNeedAiControl = true;
			for(short iTeamMember = 0; iTeamMember < game_values.teamcounts[iControllingTeam]; iTeamMember++)
			{
				if(game_values.playercontrol[game_values.teamids[iControllingTeam][iTeamMember]] == 1)
				{
					fNeedAiControl = false;
					break;
				}
			}

			if(fNeedAiControl)
			{
				short iPlayerCurrentTileX, iPlayerCurrentTileY;
				g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);
				short iNextMove = g_worldmap.GetNextInterestingMove(iPlayerCurrentTileX, iPlayerCurrentTileY);

				//Clear out all input from cpu controlled team
				COutputControl * playerKeys = NULL;
				for(short iTeamMember = 0; iTeamMember < game_values.teamcounts[iControllingTeam]; iTeamMember++)
				{
					playerKeys = &game_values.playerInput.outputControls[game_values.teamids[iControllingTeam][iTeamMember]];

					playerKeys->menu_up.fPressed = false;
					playerKeys->menu_down.fPressed = false;
					playerKeys->menu_left.fPressed = false;
					playerKeys->menu_right.fPressed = false;
					playerKeys->menu_select.fPressed = false;
					playerKeys->menu_random.fPressed = false;

					if(iControllingTeam != 0)
						playerKeys->menu_cancel.fPressed = false;

				}
				
				playerKeys = &game_values.playerInput.outputControls[game_values.teamids[iControllingTeam][0]];

				if(iNextMove == -1)
					fNoInterestingMoves = true;
				if(iNextMove == 0)
					playerKeys->menu_up.fPressed = true;
				else if(iNextMove == 1)
					playerKeys->menu_down.fPressed = true;
				else if(iNextMove == 2)
					playerKeys->menu_left.fPressed = true;
				else if(iNextMove == 3)
					playerKeys->menu_right.fPressed = true;
				else if(iNextMove == 4)
					playerKeys->menu_select.fPressed = true;
			}
		}
	}

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];
		
		short iPlayerCurrentTileX, iPlayerCurrentTileY;
		g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

		if(iState == -1)
		{

#ifdef _DEBUG
			if(playerKeys->menu_scrollfast.fPressed)
				AdvanceTurn();
#endif

			if(iControllingTeam == LookupTeamID(iPlayer) && iPlayerState == 0 && game_values.playercontrol[iPlayer] > 0) //if this player is player or cpu
			{
				WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];

				short iTemp; //Just a temp value so we can call the GetVehicleInPlayerTile method
				bool fVehicleInTile = g_worldmap.GetVehicleInPlayerTile(&iTemp) >= 0;

				if(playerKeys->menu_up.fPressed)
				{
					//Make sure there is a path connection and that there is no stage or vehicle blocking the way
					if(tile->fConnection[0] && !g_worldmap.IsDoor(iPlayerCurrentTileX, iPlayerCurrentTileY - 1) &&
						((tile->fCompleted && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 0 || fUsingCloud))
					{
						if(fUsingCloud && (!tile->fCompleted || fVehicleInTile) && iReturnDirection != 0)
							UseCloud(false);

						g_worldmap.MovePlayer(0);
						iReturnDirection = 1;
					}
				}
				else if(playerKeys->menu_down.fPressed)
				{
					if(tile->fConnection[1] && !g_worldmap.IsDoor(iPlayerCurrentTileX, iPlayerCurrentTileY + 1) &&
						((tile->fCompleted && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 1 || fUsingCloud))
					{
						if(fUsingCloud && (!tile->fCompleted || fVehicleInTile) && iReturnDirection != 1)
							UseCloud(false);

						g_worldmap.MovePlayer(1);
						iReturnDirection = 0;
					}
				}
				else if(playerKeys->menu_left.fPressed)
				{
					if(tile->fConnection[2] && !g_worldmap.IsDoor(iPlayerCurrentTileX - 1, iPlayerCurrentTileY) &&
						((tile->fCompleted && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 2 || fUsingCloud))
					{
						if(fUsingCloud && (!tile->fCompleted || fVehicleInTile) && iReturnDirection != 2)
							UseCloud(false);

						g_worldmap.MovePlayer(2);
						iReturnDirection = 3;
					}
					else
					{
						g_worldmap.FacePlayer(1);
					}
				}
				else if(playerKeys->menu_right.fPressed)
				{
					if(tile->fConnection[3] && !g_worldmap.IsDoor(iPlayerCurrentTileX + 1, iPlayerCurrentTileY) &&
						((tile->fCompleted && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 3 || fUsingCloud))
					{
						if(fUsingCloud && (!tile->fCompleted || fVehicleInTile) && iReturnDirection != 3)
							UseCloud(false);

						g_worldmap.MovePlayer(3);
						iReturnDirection = 2;
					}
					else
					{
						g_worldmap.FacePlayer(0);
					}
				}
				else if(playerInput->outputControls[iPlayer].menu_select.fPressed)
				{
					//Lookup current tile and see if it is a type of tile you can interact with
					//If there is a vehicle on this tile, then load it's stage
					short iStage = g_worldmap.GetVehicleInPlayerTile(&iVehicleId);
					if(iStage >= 0)
					{
						game_values.tourstopcurrent = iStage;
						
						if(fNeedAiControl)
							return MENU_CODE_TOUR_STOP_CONTINUE_FORCED;
						else
							return MENU_CODE_WORLD_STAGE_START;
					}

					if(g_worldmap.GetWarpInPlayerTile(&iWarpCol, &iWarpRow))
					{
						iState = 4;
						iScreenfade = 0;
						iScreenfadeRate = 8;

						ifsoundonplay(sfx_pipe);
					}

					//if it is a stage, then load the stage
					WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];

					short iType = tile->iType - 6;
					if(iType >= 0 && !tile->fCompleted)
					{
						game_values.tourstopcurrent = iType;

						if(fNeedAiControl)
							return MENU_CODE_TOUR_STOP_CONTINUE_FORCED;
						else
							return MENU_CODE_WORLD_STAGE_START;
					}
				}
			}
		}
		else if (iState == game_values.colorids[iPlayer] && iStateTransition == 0) //not transitioning to or from the item popup menu
		{
			if(playerKeys->menu_up.fPressed)
			{
				if(iItemPage > 0)
				{
					iItemPage--;
					iItemCol = 0;
				}
			}
			else if(playerKeys->menu_down.fPressed)
			{
				if(iItemPage < 3 && (iItemPage + 1) * 8 < game_values.worldpowerupcount[iTeam])
				{
					iItemPage++;
					iItemCol = 0;
				}
			}
			else if(playerKeys->menu_left.fPressed)
			{
				if(iItemCol > 0)
				{
					iItemCol--;
				}
				else if(iItemCol == 0 && iItemPage > 0)
				{
					iItemCol = 7;
					iItemPage--;
				}
			}
			else if(playerKeys->menu_right.fPressed)
			{
				if(iItemPage * 8 + iItemCol + 1 < game_values.worldpowerupcount[iTeam])
				{
					if(iItemCol < 7)
					{
						iItemCol++;
					}
					else if(iItemCol == 7 && iItemPage < 3)
					{
						iItemCol = 0;
						iItemPage++;
					}
				}
			}
			else if(playerKeys->menu_select.fPressed)
			{
				if(game_values.worldpowerupcount[iTeam] > 0)
				{
					short iIndex = iItemPage * 8 + iItemCol;
					short iPowerup = game_values.worldpowerups[iTeam][iIndex];

					bool fUsedItem = false;

					if(iPowerup < NUM_POWERUPS)
					{
						for(short iPlayer = 0; iPlayer < game_values.teamcounts[iTeam]; iPlayer++)
						{
							game_values.storedpowerups[game_values.teamids[iTeam][iPlayer]] = iPowerup;
						}

						ifsoundonplay(sfx_collectpowerup);
						fUsedItem = true;
					}
					else if(iPowerup == NUM_POWERUPS) //Music Box (put vehicles to sleep)
					{
						iSleepTurns = rand() % 4 + 2;
						fUsedItem = true;
						ifsoundonplay(sfx_collectpowerup);
					}
					else if(iPowerup == NUM_POWERUPS + 1) //Cloud (allows player to skip stages)
					{
						UseCloud(true);
						fUsedItem = true;
					}
					else if(iPowerup == NUM_POWERUPS + 2) //Player Switch
					{
						short iPlayerTeam = LookupTeamID(iPlayer);
						if(iControllingTeam != iPlayerTeam)
						{
							SetControllingTeam(iPlayerTeam);
							fUsedItem = true;
							ifsoundonplay(sfx_switchpress);
							iState = 6;
							fNoInterestingMoves = false;
						}
					}
					else if(iPowerup == NUM_POWERUPS + 3) //Advance Turn
					{
						AdvanceTurn();
						fUsedItem = true;
						ifsoundonplay(sfx_switchpress);
					}
					else if(iPowerup >= NUM_POWERUPS + 5 && iPowerup <= NUM_POWERUPS + 8) //Door Keys
					{
						short iDoorsOpened = g_worldmap.UseKey(iPowerup - NUM_POWERUPS - 5, iPlayerCurrentTileX, iPlayerCurrentTileY);

						if(iDoorsOpened > 0)
						{
							UpdateMapSurface();
							ifsoundonplay(sfx_transform);

							short iPlayerX = (iPlayerCurrentTileX << 5) + iMapOffsetX;
							short iPlayerY = (iPlayerCurrentTileY << 5) + iMapOffsetY;

							if(iDoorsOpened & 0x1)
								uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX - TILESIZE, iPlayerY, 3, 8));

							if(iDoorsOpened & 0x2)
								uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX + TILESIZE, iPlayerY, 3, 8));
				
							if(iDoorsOpened & 0x4)
								uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX, iPlayerY - TILESIZE, 3, 8));

							if(iDoorsOpened & 0x8)
								uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX, iPlayerY + TILESIZE, 3, 8));

							fUsedItem = true;
							fNoInterestingMoves = false;
						}
					}

					if(fUsedItem)
					{
						short iNumItems = --game_values.worldpowerupcount[iTeam];

						for(short iItem = iIndex; iItem < iNumItems; iItem++)
							game_values.worldpowerups[iTeam][iItem] = game_values.worldpowerups[iTeam][iItem + 1];

						iStateTransition = 2;
					}
					else
					{
						ifsoundonplay(sfx_stun);
					}
				}
			}
		}

		if(iState > -2 && iState < 4)
		{
			if(playerKeys->menu_cancel.fPressed)
			{
				if(DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0)
				{
					fModifying = false;
					return MENU_CODE_UNSELECT_ITEM;
				}
			}
			
			if ((game_values.playercontrol[iPlayer] == 1 || fNoInterestingMoves) && (playerKeys->menu_random.fPressed ||
				(iPlayer != 0 && playerInput->inputControls[iPlayer]->iDevice == DEVICE_KEYBOARD && playerKeys->menu_cancel.fPressed)))
			{
				if(iState == -1)
				{
					iState = game_values.colorids[iPlayer];
					iTeam = LookupTeamID(iPlayer);
					iStateTransition = 1;

					iItemPage = 0;
					iItemCol = 0;
				}
				else if (iState == game_values.colorids[iPlayer])
				{
					iStateTransition = 2;
				}
			}
		}
	}

	return MENU_CODE_NONE;
}

MenuCodeEnum MI_World::Modify(bool modify)
{
	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

void MI_World::UseCloud(bool fUseCloud)
{
	fUsingCloud = fUseCloud;
	ifsoundonplay(sfx_transform);

	short iPlayerX, iPlayerY;
	g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);
	uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX + iMapOffsetX, iPlayerY + iMapOffsetY, 3, 8));
}
