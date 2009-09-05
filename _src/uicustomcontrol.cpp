#include "global.h"
#include <math.h>

extern const char * Keynames[340];
extern const char * Joynames[30];

extern const char * GameInputNames[NUM_KEYS];
extern const char * MenuInputNames[NUM_KEYS];

extern bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections);

extern short iScoreboardPlayerOffsetsX[3][3];
extern WorldMap g_worldmap;
extern short LookupTeamID(short id);


/**************************************
 * MI_InputControlField Class
 **************************************/

MI_InputControlField::MI_InputControlField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent) :
	UI_Control(x, y)
{
	spr = nspr;

	szName = new char[strlen(name) + 1];
	strcpy(szName, name);

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
		game_values.teamcounts[iTeam] = 0;

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
MI_PlayerSelect::MI_PlayerSelect(gfxSprite * nspr, short x, short y, const char * name, short width, short indent) :
	UI_Control(x, y)
{
	spr = nspr;
	iSelectedPlayer = 0;

	szName = new char[strlen(name) + 1];
	strcpy(szName, name);

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
 * MI_PowerupSelection Class
 **************************************/

//Rearrange display of powerups
//short iPowerupDisplayMap[NUM_POWERUPS] = { 4, 0, 1, 2, 3, 6, 10, 12, 11, 14, 13, 7, 16, 17, 18, 19, 15, 9, 5, 8, 20, 21, 22, 23, 24, 25};
//                                          0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 
short iPowerupPositionMap[NUM_POWERUPS] = { 1, 0, 2, 6, 3, 8, 4,20,18, 7, 5,10,11,22,19, 9,23,16,21,12,17,13,24,15,25,14};

/*
0 == poison mushroom
1 == 1up
2 == 2up
3 == 3up
4 == 5up
5 == flower
6 == star
7 == clock
8 == bobomb
9 == pow
10 == bulletbill
11 == hammer
12 == green shell
13 == red shell
14 == spike shell
15 == buzzy shell
16 == mod
17 == feather
18 == mystery mushroom
19 == boomerang
20 == tanooki
21 == ice wand
22 == podoboo
23 == bombs
24 == leaf
25 == pwings
*/
				
/*
0 1UP     Poison
2 2UP     Star
4 3UP     Bob-Omb
6 5UP     Tanooki
8 Mystery Clock
10 Flower  B.Bill
12 Hammer  P'Boo
14 B'Rang  POW
16 Bomb    MOd
18 Wand    G.Shell
20 Feather R.Shell
22 Leaf    B.Shell
24 P-Wing  S.Shell
*/

MI_PowerupSelection::MI_PowerupSelection(short x, short y, short width, short numlines) :
	UI_Control(x, y)
{
	iWidth = width;
	iNumLines = numlines;

	iIndex = 0;
	iOffset = 0;

	iTopStop = ((iNumLines - 1) >> 1) + 2;
	iBottomStop = (NUM_POWERUPS >> 1) - iNumLines + iTopStop;

	mMenu = new UI_Menu();

	miOverride = new MI_SelectField(&spr_selectfield, 70, iy, "Use Settings From", 500, 250);
	miOverride->Add("Map Only", 0, "", false, false);
	miOverride->Add("Game Only", 1, "", false, false);
	miOverride->Add("Basic Average", 2, "", false, false);
	miOverride->Add("Weighted Average", 3, "", false, false);
	miOverride->SetData(&game_values.overridepowerupsettings, NULL, NULL);
	miOverride->SetKey(game_values.overridepowerupsettings);
	//miOverride->SetItemChangedCode(MENU_CODE_POWERUP_OVERRIDE_CHANGED);

	miPreset = new MI_SelectField(&spr_selectfield, 70, iy + 40, "Item Set", 500, 250);
	miPreset->Add("Custom Set 1", 0, "", false, false);
	miPreset->Add("Custom Set 2", 1, "", false, false);
	miPreset->Add("Custom Set 3", 2, "", false, false);
	miPreset->Add("Custom Set 4", 3, "", false, false);
	miPreset->Add("Custom Set 5", 4, "", false, false);
	miPreset->Add("Balanced Set", 5, "", false, false);
	miPreset->Add("Weapons Only", 6, "", false, false);
	miPreset->Add("Koopa Bros Weapons", 7, "", false, false);
	miPreset->Add("Support Items", 8, "", false, false);
	miPreset->Add("Booms and Shakes", 9, "", false, false);
	miPreset->Add("Fly and Glide", 10, "", false, false);
	miPreset->Add("Shells Only", 11, "", false, false);
	miPreset->Add("Mushrooms Only", 12, "", false, false);
	miPreset->Add("Super Mario Bros 1", 13, "", false, false);
	miPreset->Add("Super Mario Bros 2", 14, "", false, false);
	miPreset->Add("Super Mario Bros 3", 15, "", false, false);
	miPreset->Add("Super Mario World", 16, "", false, false);
	miPreset->SetData(&game_values.poweruppreset, NULL, NULL);
	miPreset->SetKey(game_values.poweruppreset);
	miPreset->SetItemChangedCode(MENU_CODE_POWERUP_PRESET_CHANGED);

	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
	{
		miPowerupSlider[iPowerup] = new MI_PowerupSlider(&spr_selectfield, &menu_slider_bar, &spr_storedpoweruplarge, 0, 0, 245, iPowerupPositionMap[iPowerup]);
		miPowerupSlider[iPowerup]->Add("", 0, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 1, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 2, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 3, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 4, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 5, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 6, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 7, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 8, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 9, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 10, "", false, false);
		miPowerupSlider[iPowerup]->SetNoWrap(true);
		miPowerupSlider[iPowerup]->SetData(&game_values.powerupweights[iPowerupPositionMap[iPowerup]], NULL, NULL);
		miPowerupSlider[iPowerup]->SetKey(game_values.powerupweights[iPowerupPositionMap[iPowerup]]);
		miPowerupSlider[iPowerup]->SetItemChangedCode(MENU_CODE_POWERUP_SETTING_CHANGED);
	}

	miRestoreDefaultsButton = new MI_Button(&spr_selectfield, 160, 432, "Defaults", 150, 1);
	miRestoreDefaultsButton->SetCode(MENU_CODE_RESTORE_DEFAULT_POWERUP_WEIGHTS);

	miClearButton = new MI_Button(&spr_selectfield, 330, 432, "Clear", 150, 1);
	miClearButton->SetCode(MENU_CODE_CLEAR_POWERUP_WEIGHTS);

	//Are You Sure dialog box
	miDialogImage = new MI_Image(&spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
	miDialogAreYouText = new MI_Text("Are You", 320, 195, 0, 2, 1);
	miDialogSureText = new MI_Text("Sure?", 320, 220, 0, 2, 1);
	miDialogYesButton = new MI_Button(&spr_selectfield, 235, 250, "Yes", 80, 1);
	miDialogNoButton = new MI_Button(&spr_selectfield, 325, 250, "No", 80, 1);
	
	miDialogYesButton->SetCode(MENU_CODE_POWERUP_RESET_YES);
	miDialogNoButton->SetCode(MENU_CODE_POWERUP_RESET_NO);

	miDialogImage->Show(false);
	miDialogAreYouText->Show(false);
	miDialogSureText->Show(false);
	miDialogYesButton->Show(false);
	miDialogNoButton->Show(false);

	mMenu->AddControl(miOverride, NULL, miPreset, NULL, NULL);
	mMenu->AddControl(miPreset, miOverride, miPowerupSlider[0], NULL, NULL);

	miUpArrow = new MI_Image(&menu_verticalarrows, 310, 128, 20, 0, 20, 20, 1, 4, 8);
	miDownArrow = new MI_Image(&menu_verticalarrows, 310, 406, 0, 0, 20, 20, 1, 4, 8);
	miUpArrow->Show(false);

	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
	{
		UI_Control * upcontrol = NULL;
		if(iPowerup == 0)
			upcontrol = miPreset;
		else
			upcontrol = miPowerupSlider[iPowerup - 2];

		UI_Control * downcontrol = NULL;
		if(iPowerup >= NUM_POWERUPS - 2)
			downcontrol = miRestoreDefaultsButton;
		else
			downcontrol = miPowerupSlider[iPowerup + 2];

		mMenu->AddControl(miPowerupSlider[iPowerup], upcontrol, downcontrol, NULL, miPowerupSlider[iPowerup + 1]);

		if(++iPowerup < NUM_POWERUPS)
		{
			upcontrol = NULL;
			if(iPowerup == 1)
				upcontrol = miPreset;
			else
				upcontrol = miPowerupSlider[iPowerup - 2];

			UI_Control * downcontrol = NULL;
			if(iPowerup >= NUM_POWERUPS - 2)
				downcontrol = miClearButton;
			else
				downcontrol = miPowerupSlider[iPowerup + 2];

			mMenu->AddControl(miPowerupSlider[iPowerup], upcontrol, downcontrol, miPowerupSlider[iPowerup - 1], NULL);
		}
	}

	mMenu->AddControl(miRestoreDefaultsButton, miPowerupSlider[NUM_POWERUPS - 2], NULL, NULL, miClearButton);
	mMenu->AddControl(miClearButton, miPowerupSlider[NUM_POWERUPS - 1], NULL, miRestoreDefaultsButton, NULL);

	//Setup positions and visible powerups
	SetupPowerupFields();

	//Set enabled based on if we are overriding or not
	//EnablePowerupFields();

	mMenu->AddNonControl(miDialogImage);
	mMenu->AddNonControl(miDialogAreYouText);
	mMenu->AddNonControl(miDialogSureText);

	mMenu->AddControl(miDialogYesButton, NULL, NULL, NULL, miDialogNoButton);
	mMenu->AddControl(miDialogNoButton, NULL, NULL, miDialogYesButton, NULL);

	mMenu->AddNonControl(miUpArrow);
	mMenu->AddNonControl(miDownArrow);

	mMenu->SetHeadControl(miOverride);
	mMenu->SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
}

MI_PowerupSelection::~MI_PowerupSelection()
{
	delete mMenu;
}

void MI_PowerupSelection::SetupPowerupFields()
{
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
	{
		short iPosition = iPowerupPositionMap[iPowerup];
		MI_PowerupSlider * slider = miPowerupSlider[iPosition];

		if((iPosition >> 1) < iOffset || (iPosition >> 1) >= iOffset + iNumLines)
			slider->Show(false);
		else
		{
			slider->Show(true);
			slider->SetPosition(ix + (iPosition % 2) * 295, iy + 84 + 38 * (iPosition / 2 - iOffset));
		}
	}
}

void MI_PowerupSelection::EnablePowerupFields(bool fEnable)
{
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
	{
		miPowerupSlider[iPowerup]->Disable(!fEnable);
	}

	miPreset->SetNeighbor(1, fEnable ? miPowerupSlider[0] : NULL);
}

MenuCodeEnum MI_PowerupSelection::Modify(bool modify)
{
	mMenu->ResetMenu();
	iOffset = 0;
	iIndex = 0;
	SetupPowerupFields();

	//EnablePowerupFields(game_values.poweruppreset >= 1 && game_values.poweruppreset <= 3);

	fModifying = modify;
	return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_PowerupSelection::SendInput(CPlayerInput * playerInput)
{
	MenuCodeEnum ret = mMenu->SendInput(playerInput);

	if(MENU_CODE_CANCEL_INPUT == ret)
	{
		fModifying = false;
		return MENU_CODE_UNSELECT_ITEM;
	}
	else if (MENU_CODE_POWERUP_PRESET_CHANGED == ret)
	{
		for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		{
			short iCurrentValue = g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]];
			miPowerupSlider[iPowerup]->SetKey(iCurrentValue);
			game_values.powerupweights[iPowerupPositionMap[iPowerup]] = iCurrentValue;
		}

		//If it is a custom preset, then allow modification
		//EnablePowerupFields(game_values.poweruppreset >= 1 && game_values.poweruppreset <= 3);
	}
	else if (MENU_CODE_POWERUP_SETTING_CHANGED == ret)
	{
		//Update the custom presets
		for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
			g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]] = game_values.powerupweights[iPowerupPositionMap[iPowerup]];
	}
	else if (MENU_CODE_RESTORE_DEFAULT_POWERUP_WEIGHTS == ret || MENU_CODE_CLEAR_POWERUP_WEIGHTS == ret)
	{
		miDialogImage->Show(true);
		miDialogAreYouText->Show(true);
		miDialogSureText->Show(true);
		miDialogYesButton->Show(true);
		miDialogNoButton->Show(true);

		mMenu->RememberCurrent();

		mMenu->SetHeadControl(miDialogNoButton);
		mMenu->SetCancelCode(MENU_CODE_POWERUP_RESET_NO);
		mMenu->ResetMenu();

		if(MENU_CODE_CLEAR_POWERUP_WEIGHTS == ret)
			miDialogYesButton->SetCode(MENU_CODE_POWERUP_CLEAR_YES);
		else
			miDialogYesButton->SetCode(MENU_CODE_POWERUP_RESET_YES);

	}
	else if(MENU_CODE_POWERUP_RESET_YES == ret || MENU_CODE_POWERUP_RESET_NO == ret || MENU_CODE_POWERUP_CLEAR_YES == ret)
	{
		miDialogImage->Show(false);
		miDialogAreYouText->Show(false);
		miDialogSureText->Show(false);
		miDialogYesButton->Show(false);
		miDialogNoButton->Show(false);

		mMenu->SetHeadControl(miOverride);
		mMenu->SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

		mMenu->RestoreCurrent();

		if(MENU_CODE_POWERUP_RESET_YES == ret)
		{
			//restore default powerup weights for powerup selection menu
			for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
			{
				short iDefaultValue = g_iDefaultPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]];
				miPowerupSlider[iPowerup]->SetKey(iDefaultValue);
				game_values.powerupweights[iPowerupPositionMap[iPowerup]] = iDefaultValue;

				g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerupPositionMap[iPowerup]] = iDefaultValue;
			}
		}
		else if(MENU_CODE_POWERUP_CLEAR_YES == ret)
		{
			//restore default powerup weights for powerup selection menu
			for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
			{
				miPowerupSlider[iPowerup]->SetKey(0);
				game_values.powerupweights[iPowerup] = 0;

				g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerup] = 0;
			}
		}
	}
	else if(MENU_CODE_NEIGHBOR_UP == ret)
	{
		MovePrev();
	}
	else if(MENU_CODE_NEIGHBOR_DOWN == ret)
	{
		MoveNext();
	}

	return ret;
}

void MI_PowerupSelection::Update()
{
	mMenu->Update();
}

void MI_PowerupSelection::Draw()
{
	if(!fShow)
		return;

	mMenu->Draw();
}

void MI_PowerupSelection::MoveNext()
{
	iIndex++;

	if(iIndex > iTopStop && iIndex <= iBottomStop)
	{
		iOffset++;
		SetupPowerupFields();
	}

	AdjustDisplayArrows();
}

void MI_PowerupSelection::MovePrev()
{
	iIndex--;

	if(iIndex >= iTopStop && iIndex < iBottomStop)
	{
		iOffset--;
		SetupPowerupFields();
	}

	AdjustDisplayArrows();
}

void MI_PowerupSelection::AdjustDisplayArrows()
{
	if(iIndex > iTopStop)
		miUpArrow->Show(true);
	else
		miUpArrow->Show(false);

	if(iIndex < iBottomStop)
		miDownArrow->Show(true);
	else
		miDownArrow->Show(false);
}




/**************************************
 * MI_WorldPreviewDisplay Class
 **************************************/

MI_WorldPreviewDisplay::MI_WorldPreviewDisplay(short x, short y, short cols, short rows) :
	UI_Control(x, y)
{
	sMapSurface = SDL_CreateRGBSurface(screen->flags, 384, 304, screen->format->BitsPerPixel, 0, 0, 0, 0);

	iCols = cols;
	iRows = rows;

	iScrollCols = 0;
	iScrollRows = 0;

	Init();
}

MI_WorldPreviewDisplay::~MI_WorldPreviewDisplay()
{
	if(sMapSurface)
	{
		SDL_FreeSurface(sMapSurface);
		sMapSurface = NULL;
	}
}

void MI_WorldPreviewDisplay::Init()
{
	iMapOffsetX = 0;
	iMapOffsetY = 0;

	iMapGlobalOffsetX = 0;
	iMapGlobalOffsetY = 0;
	
	iMapDrawOffsetCol = 0;
	iMapDrawOffsetRow = 0;

	iMoveDirection = iScrollCols > 0 ? 0 : (iScrollRows > 0 ? 1 : -1);

	iAnimationTimer = 0;
	iAnimationFrame = 0;

	if(g_worldmap.iWidth > 20)
		rectSrcSurface.w = 320;
	else
		rectSrcSurface.w = g_worldmap.iWidth * PREVIEWTILESIZE;

	if(g_worldmap.iHeight > 15)
		rectSrcSurface.h = 240;
	else
		rectSrcSurface.h = g_worldmap.iHeight * PREVIEWTILESIZE;

	rectDstSurface.w = 320;
	rectDstSurface.h = 240;
	rectDstSurface.x = ix;
	rectDstSurface.y = iy;

	if(g_worldmap.iWidth < 20)
		rectDstSurface.x += (20 - g_worldmap.iWidth) * 8;

	if(g_worldmap.iHeight < 15)
		rectDstSurface.y += (15 - g_worldmap.iHeight) * 8;
	
	short iNumScrollTiles = (g_worldmap.iWidth > 20 ? g_worldmap.iWidth - 20 : 0) + (g_worldmap.iHeight > 15 ? g_worldmap.iHeight - 15 : 0);

	iScrollSpeed = 0;
	iScrollSpeedTimer = 0;
	if(iNumScrollTiles < 9)
		iScrollSpeed = (12 - iNumScrollTiles) >> 2;
}

void MI_WorldPreviewDisplay::Update()
{
	if(!fShow)
		return;

	bool fNeedMapSurfaceUpdate = false;
	bool fNeedFullRefresh = false;

	if(++iAnimationTimer > 15)
	{
		iAnimationTimer = 0;
		iAnimationFrame += PREVIEWTILESIZE;

		if(iAnimationFrame >= 64)
			iAnimationFrame = 0;

		fNeedMapSurfaceUpdate = true;
	}

	if(++iScrollSpeedTimer > iScrollSpeed)
	{
		iScrollSpeedTimer = 0;

		if(iMoveDirection == 0)
		{
			--iMapGlobalOffsetX;
			if(++iMapOffsetX >= PREVIEWTILESIZE)
			{
				iMapOffsetX = 0;

				fNeedMapSurfaceUpdate = true;
				fNeedFullRefresh = true;

				if(++iMapDrawOffsetCol >= iScrollCols)
				{
					if(iScrollRows > 0)
					{
						iMoveDirection = 1;
						iMapOffsetY = 0;
					}
					else
					{
						iMoveDirection = 2;
						iMapOffsetX = PREVIEWTILESIZE;
						--iMapDrawOffsetCol;
						iMapGlobalOffsetX += PREVIEWTILESIZE;
					}
				}
			}
		}
		else if(iMoveDirection == 1)
		{
			--iMapGlobalOffsetY;
			if(++iMapOffsetY >= PREVIEWTILESIZE)
			{
				iMapOffsetY = 0;

				fNeedMapSurfaceUpdate = true;
				fNeedFullRefresh = true;

				if(++iMapDrawOffsetRow >= iScrollRows)
				{
					if(iScrollCols > 0)
					{
						iMoveDirection = 2;
						iMapOffsetX = PREVIEWTILESIZE;
						--iMapDrawOffsetCol;
						iMapGlobalOffsetX += PREVIEWTILESIZE;
					}
					else
					{
						iMoveDirection = 3;
						iMapOffsetY = PREVIEWTILESIZE;
						--iMapDrawOffsetRow;
						iMapGlobalOffsetY += PREVIEWTILESIZE;
					}
				}
			}
		}
		else if(iMoveDirection == 2)  //scroll left
		{
			++iMapGlobalOffsetX;
			if(--iMapOffsetX <= 0)
			{
				fNeedMapSurfaceUpdate = true;
				fNeedFullRefresh = true;

				if(--iMapDrawOffsetCol < 0)
				{
					iMapDrawOffsetCol = 0;

					if(iScrollRows > 0)
					{
						iMoveDirection = 3;
						iMapOffsetY = PREVIEWTILESIZE;
						--iMapDrawOffsetRow;
						iMapGlobalOffsetY += PREVIEWTILESIZE;
						iMapGlobalOffsetX -= PREVIEWTILESIZE;
					}
					else
					{
						iMoveDirection = 0;
						iMapOffsetX = 0;
						iMapGlobalOffsetX -= PREVIEWTILESIZE;
					}
				}
				else
				{
					iMapOffsetX = PREVIEWTILESIZE;
				}
			}
		}
		else if(iMoveDirection == 3) //scroll up
		{
			++iMapGlobalOffsetY;
			if(--iMapOffsetY <= 0)
			{
				fNeedMapSurfaceUpdate = true;
				fNeedFullRefresh = true;

				if(--iMapDrawOffsetRow < 0)
				{
					iMapDrawOffsetRow = 0;
					iMoveDirection = iScrollCols > 0 ? 0 : 1;

					if(iScrollCols > 0)
					{
						iMoveDirection = 0;
						iMapOffsetX = 0;
						iMapGlobalOffsetY -= PREVIEWTILESIZE;
					}
					else
					{
						iMoveDirection = 1;
						iMapOffsetY = 0;
						iMapGlobalOffsetY -= PREVIEWTILESIZE;
					}
				}
				else
				{
					iMapOffsetY = PREVIEWTILESIZE;
				}
			}
		}
	}

	if(fNeedMapSurfaceUpdate)
		UpdateMapSurface(fNeedFullRefresh);
}

void MI_WorldPreviewDisplay::Draw()
{
	if(!fShow)
		return;

	rectSrcSurface.x = iMapOffsetX;
	rectSrcSurface.y = iMapOffsetY;
	
	SDL_BlitSurface(sMapSurface, &rectSrcSurface, blitdest, &rectDstSurface);
}

void MI_WorldPreviewDisplay::SetWorld()
{
	g_worldmap.Load(PREVIEWTILESIZE);

	short w, h;
	g_worldmap.GetWorldSize(&w, &h);

	iScrollCols = w > 20 ? w - 20 : 0;
	iScrollRows = h > 15 ? h - 15 : 0;

	Init();

	UpdateMapSurface(true);
}

void MI_WorldPreviewDisplay::UpdateMapSurface(bool fFullRefresh)
{
	g_worldmap.DrawMapToSurface(-1, fFullRefresh, sMapSurface, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

	SDL_Surface * olddest = blitdest;
	blitdest = sMapSurface;
	g_worldmap.Draw(iMapGlobalOffsetX, iMapGlobalOffsetY, false, false);
	blitdest = olddest;
}


/**************************************
 * MI_AnnouncerField Class
 **************************************/

MI_AnnouncerField::MI_AnnouncerField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent, SimpleFileList * pList) :
	UI_Control(x, y)
{
	spr = nspr;

	szName = new char[strlen(name) + 1];
	strcpy(szName, name);

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

MI_PacksField::MI_PacksField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent, SimpleFileList * pList, MenuCodeEnum code) :
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

MI_PlaylistField::MI_PlaylistField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent) :
	UI_Control(x, y)
{
	spr = nspr;

	szName = new char[strlen(name) + 1];
	strcpy(szName, name);

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
 * MI_StoredPowerupResetButton Class
 **************************************/

MI_StoredPowerupResetButton::MI_StoredPowerupResetButton(gfxSprite * nspr, short x, short y, const char * name, short width, short justified) :
	MI_Button(nspr, x, y, name, width, justified)
{
	spr = nspr;

	szName = new char[strlen(name) + 1];
	strcpy(szName, name);

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

//Call with x = 70 and y == 80
MI_TourStop::MI_TourStop(short x, short y, bool fWorld) :
	UI_Control(x, y)
{
	fIsWorld = fWorld;

	if(fIsWorld)
	{
		miModeField = new MI_ImageSelectField(&spr_selectfielddisabled, &menu_mode_small, 70, 85, "Mode", 305, 90, 16, 16);
		miGoalField = new MI_SelectField(&spr_selectfielddisabled, 380, 85, "Goal", 190, 90);
		miPointsField = new MI_SelectField(&spr_selectfielddisabled, 380, 125, "Score", 190, 90);

		miBonusField = new MI_SelectField(&spr_selectfielddisabled, 70, 125, "Bonus", 305, 90);
		miBonusField->Disable(true);

		miEndStageImage[0] = new MI_Image(&spr_worlditemsplace, 54, 201, 0, 20, 80, 248, 1, 1, 0);
		miEndStageImage[0]->Show(false);

		miEndStageImage[1] = new MI_Image(&spr_worlditemsplace, 506, 201, 0, 20, 80, 248, 1, 1, 0);
		miEndStageImage[1]->Show(false);

		for(short iBonus = 0; iBonus < 10; iBonus++)
		{
			miBonusIcon[iBonus] = new MI_Image(&spr_worlditemssmall, 170 + iBonus * 20, 133, 0, 0, 16, 16, 1, 1, 0); 
			miBonusBackground[iBonus] = new MI_Image(&spr_worlditemsplace, 168 + iBonus * 20, 131, 0, 0, 20, 20, 1, 1, 0); 

			miBonusIcon[iBonus]->Show(false);
			miBonusBackground[iBonus]->Show(false);
		}
	}
	else
	{
		miModeField = new MI_ImageSelectField(&spr_selectfielddisabled, &menu_mode_small, 70, 85, "Mode", 500, 120, 16, 16);
		miGoalField = new MI_SelectField(&spr_selectfielddisabled, 70, 125, "Goal", 246, 120);
		miPointsField = new MI_SelectField(&spr_selectfielddisabled, 70 + 254, 125, "Score", 246, 120);
	
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

	if(fIsWorld)
	{
		delete miBonusField;
		delete miEndStageImage[0];
		delete miEndStageImage[1];

		for(short iBonus = 0; iBonus < 10; iBonus++)
		{
			delete miBonusIcon[iBonus];
			delete miBonusBackground[iBonus];
		}
	}
}

MenuCodeEnum MI_TourStop::Modify(bool fModify)
{
	return miStartButton->Modify(fModify);
}

void MI_TourStop::Update()
{
	if(!fShow)
		return;

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
		miEndStageImage[0]->Draw();
		miEndStageImage[1]->Draw();

		for(short iBonus = 0; iBonus < 10; iBonus++)
		{
			miBonusBackground[iBonus]->Draw();
			miBonusIcon[iBonus]->Draw();
		}
	}

	miTourStopLeftHeaderBar->Draw();
	miTourStopMenuRightHeaderBar->Draw();
	miTourStopMenuHeaderText->Draw();
}

void MI_TourStop::Refresh(short iTourStop)
{
	TourStop * tourstop = game_values.tourstops[iTourStop];

	if(tourstop->iStageType == 0)
	{
		miModeField->Clear();

		CGameMode * gamemode = NULL;
		short tourstopicon = 0;
		if(tourstop->iMode == game_mode_pipe_minigame)
		{
			gamemode = pipegamemode;
			tourstopicon = 25;
		}
		else if(tourstop->iMode == game_mode_boss_minigame)
		{
			gamemode = bossgamemode;
			tourstopicon = 26;
		}
		else if(tourstop->iMode == game_mode_boxes_minigame)
		{
			gamemode = boxesgamemode;
			tourstopicon = 27;
		}
		else
		{
			gamemode = gamemodes[tourstop->iMode];
			tourstopicon = tourstop->iMode;
		}
			
		miModeField->Add(gamemode->GetModeName(), tourstopicon, "", false, false);

		miGoalField->Clear();
		char szTemp[16];
		sprintf(szTemp, "%d", tourstop->iGoal);
		miGoalField->Add(szTemp, 0, "", false, false);
		miGoalField->SetTitle(gamemode->GetGoalName());

		miPointsField->Clear();
		sprintf(szTemp, "%d", tourstop->iPoints);
		miPointsField->Add(szTemp, 0, "", false, false);

		if(tourstop->iMode == game_mode_pipe_minigame)
		{
			bool fFound = miMapField->SetMap(tourstop->pszMapFile, true);
			
			if(!fFound)
				miMapField->SetSpecialMap("Pipe Minigame", "maps/special/two52_special_pipe_minigame.map");
		}
		else if(tourstop->iMode == game_mode_boss_minigame)
		{
			bool fFound = miMapField->SetMap(tourstop->pszMapFile, true);
			
			if(!fFound)
			{
				short iBossType = tourstop->gmsSettings.boss.bosstype;
				if(iBossType == 0)
					miMapField->SetSpecialMap("Hammer Boss Minigame", "maps/special/two52_special_hammerboss_minigame.map");
				else if(iBossType == 1)
					miMapField->SetSpecialMap("Bomb Boss Minigame", "maps/special/two52_special_bombboss_minigame.map");
				else if(iBossType == 2)
					miMapField->SetSpecialMap("Fire Boss Minigame", "maps/special/two52_special_fireboss_minigame.map");
			}
		}
		else if(tourstop->iMode == game_mode_boxes_minigame)
		{
			bool fFound = miMapField->SetMap(tourstop->pszMapFile, true);
			
			if(!fFound)
				miMapField->SetSpecialMap("Boxes Minigame", "maps/special/two52_special_boxes_minigame.map");
		}
		else
		{
			miMapField->SetMap(tourstop->pszMapFile, true);
		}

		miTourStopMenuHeaderText->SetText(tourstop->szName);

		if(fIsWorld)
		{
			miBonusField->Clear();
			miEndStageImage[0]->Show(tourstop->fEndStage);
			miEndStageImage[1]->Show(tourstop->fEndStage);

			for(short iBonus = 0; iBonus < 10; iBonus++)
			{
				bool fShowBonus = iBonus < tourstop->iNumBonuses;
				if(fShowBonus)
				{
					short iBonusIcon = tourstop->wsbBonuses[iBonus].iBonus;
					miBonusIcon[iBonus]->SetImageSource(iBonusIcon < NUM_POWERUPS ? &spr_storedpowerupsmall : &spr_worlditemssmall);
					miBonusIcon[iBonus]->SetImage((iBonusIcon < NUM_POWERUPS ? iBonusIcon : iBonusIcon - NUM_POWERUPS) << 4, 0, 16, 16);
					miBonusBackground[iBonus]->SetImage(tourstop->wsbBonuses[iBonus].iWinnerPlace * 20, 0, 20, 20);
				}
				
				miBonusIcon[iBonus]->Show(fShowBonus);
				miBonusBackground[iBonus]->Show(fShowBonus);
			}
		}
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

	worldScore = NULL;
	worldScoreModifier = NULL;

	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		tourScores[iTeam] = NULL;
		
		for(short iBonus = 0; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++)
			worldBonus[iTeam][iBonus] = NULL;

		worldPlace[iTeam] = NULL;
		worldPointsBackground[iTeam] = NULL;
	}

	for(short iGame = 0; iGame < 10; iGame++)
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

	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
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

		if(worldPlace[iTeam])
		{
			delete worldPlace[iTeam];
			worldPlace[iTeam] = NULL;
		}

		for(short iBonus = 0; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++)
		{
			if(worldBonus[iTeam][iBonus])
			{
				delete worldBonus[iTeam][iBonus];
				worldBonus[iTeam][iBonus] = NULL;
			}
		}

		if(worldPointsBackground[iTeam])
		{
			delete worldPointsBackground[iTeam];
			worldPointsBackground[iTeam] = NULL;
		}
	}

	for(short iGame = 0; iGame < iNumGames; iGame++)
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

	if(worldScore)
	{
		delete worldScore;
		worldScore = NULL;
	}

	if(worldScoreModifier)
	{
		delete worldScoreModifier;
		worldScoreModifier = NULL;
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

		if(worldPointsBackground[iTeam])
			worldPointsBackground[iTeam]->Update();
	}

	if(game_values.matchtype == MATCH_TYPE_TOUR)
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
				iFireworksCounter = (short)(rand() % 30 + 10);
				
				if(--iExplosionCounter < 0)
				{
					iExplosionCounter = (short)(rand() % 6 + 5);

					ifsoundonplay(sfx_bobombsound);

					float dAngle = 0.0f; 
					short iRandX = (short)(rand() % 440 + 100);
					short iRandY = (short)(rand() % 280 + 100);

					for(short iBlock = 0; iBlock < 28; iBlock++)
					{
						float dVel = 7.0f + ((iBlock % 2) * 5.0f);
						float dVelX = dVel * cos(dAngle);
						float dVelY = dVel * sin(dAngle);
						
						short iRandomColor = (short)(rand() % iTeamCounts[iTournamentWinner]);
						uiMenu->AddEyeCandy(new EC_FallingObject(&spr_bonus, iRandX, iRandY, dVelX, dVelY, 4, 2, 0, game_values.colorids[iTeamIDs[iTournamentWinner][iRandomColor]] << 4, 16, 16));
						dAngle -= (float)PI / 14;
					}
				}
				else
				{
					ifsoundonplay(sfx_cannon);

					short iRandX = (short)(rand() % 576);
					short iRandY = (short)(rand() % 416);
					short iRandomColor = (short)(rand() % iTeamCounts[iTournamentWinner]);

					uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireworks, iRandX, iRandY, 8, 4, 0, game_values.colorids[iTeamIDs[iTournamentWinner][iRandomColor]] << 6, 64, 64));
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
	if(game_values.matchtype == MATCH_TYPE_TOUR)
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

	if(game_values.matchtype == MATCH_TYPE_WORLD)
	{
		for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
		{
			worldPointsBackground[iTeam]->Draw();
		}

		for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
		{
			tourScores[iTeam]->Draw();
			worldPlace[iTeam]->Draw();

			for(short iBonus = 0; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++)
			{
				worldBonus[iTeam][iBonus]->Draw();
			}
		}

		worldScoreModifier->Draw();
		worldScore->Draw();
	}
}

void MI_TournamentScoreboard::CreateScoreboard(short numTeams, short numGames, gfxSprite * spr_icons)
{
	FreeScoreboard();

	sprIcons = spr_icons;

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

	iExplosionCounter = 0;
	iFireworksCounter = 0;
	iWinnerTextCounter = 0;

	iNumTeams = numTeams;
	iNumGames = numGames;
	
	miTeamImages = new MI_Image * [iNumTeams];
	miIconImages = new MI_Image ** [iNumTeams];
	miPlayerImages = new MI_Image ** [iNumTeams];
	
	bool fTour = game_values.matchtype == MATCH_TYPE_TOUR;
	bool fNotTournament = game_values.matchtype != MATCH_TYPE_TOURNAMENT;

	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		short iTeamY = GetYFromPlace(iTeam);

		if(fTour)
			iTeamY += 28; //shift down 28 pxls for extra tour points bar

		miTeamImages[iTeam] = new MI_Image(sprBackground, ix - (fNotTournament ? 40 : 0), iTeamY, 0, game_values.matchtype == MATCH_TYPE_WORLD ? 160 : 0, fNotTournament ? 580 : 500, 64, 1, 2, 0);
		miIconImages[iTeam] = new MI_Image * [iNumGames];
		miPlayerImages[iTeam] = new MI_Image * [iTeamCounts[iTeam]];

		if(game_values.matchtype != MATCH_TYPE_WORLD)
		{
			for(short iGame = 0; iGame < iNumGames; iGame++)
			{
				float dSpacing = GetIconSpacing();

				miIconImages[iTeam][iGame] = new MI_Image(sprIcons, ix + 128 + (short)dSpacing + (short)((float)iGame * (32.0f + dSpacing)) - (fTour ? 40 : 0), iTeamY + 16, 0, 0, 32, 32, fTour ? 4 : 1, 1, fTour ? 8 : 0);
				miIconImages[iTeam][iGame]->Show(false);
			}
		}

		for(short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++)
		{
			miPlayerImages[iTeam][iPlayer] = new MI_Image(spr_player[iTeamIDs[iTeam][iPlayer]][PGFX_STANDING_R], ix + iScoreboardPlayerOffsetsX[iTeamCounts[iTeam] - 1][iPlayer] - (fTour ? 40 : 0), iTeamY + 16, 0, 0, 32, 32, 2, 1, 0);
		}

		if(fNotTournament)
			tourScores[iTeam] = new MI_ScoreText(0, 0);

		if(game_values.matchtype == MATCH_TYPE_WORLD)
		{
			worldPointsBackground[iTeam] = new MI_Image(sprBackground, ix + 476, iTeamY, 516, 160, 64, 64, 1, 2, 0);
			worldPlace[iTeam] = new MI_Image(sprIcons, ix + 102, iTeamY + 14, 0, 0, 32, 32, 4, 1, 8);

			for(short iBonus = 0; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++)
			{
				worldBonus[iTeam][iBonus] = new MI_Image(&spr_worlditems, ix + 180 + 38 * iBonus, iTeamY + 14, 0, 0, 32, 32, 1, 1, 0);
				worldBonus[iTeam][iBonus]->Show(false);
			}
		}
	}

	if(game_values.matchtype == MATCH_TYPE_WORLD)
	{
		worldScoreModifier = new MI_Image(&spr_worlditems, 0, 0, 0, 0, 32, 32, 1, 1, 0);
		worldScore = new MI_ScoreText(0, 0);
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

void MI_TournamentScoreboard::RefreshWorldScores(short gameWinner)
{
	iGameWinner = gameWinner;
	DetermineScoreboardWinners();

	TourStop * tourStop = game_values.tourstops[game_values.tourstopcurrent];

	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		short iTeamY = GetYFromPlace(game_values.tournament_scores[iTeam].wins);

		miTeamImages[iTeam]->SetPosition(ix - 40, iTeamY);
		worldPointsBackground[iTeam]->SetPosition(ix + 476, iTeamY);

		for(short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++)
		{
			static short iPlaceSprite[4] = {4, 0, 8, 9};
			miPlayerImages[iTeam][iPlayer]->SetPosition(ix + iScoreboardPlayerOffsetsX[iTeamCounts[iTeam] - 1][iPlayer] - 40, iTeamY + 16);
			miPlayerImages[iTeam][iPlayer]->SetImageSource(spr_player[iTeamIDs[iTeam][iPlayer]][iPlaceSprite[game_values.tournament_scores[iTeam].wins]]);
		}

		tourScores[iTeam]->SetPosition(ix + 508, iTeamY + 24);
		tourScores[iTeam]->SetScore(game_values.tournament_scores[iTeam].total);

		if(iGameWinner == -1)
		{
			worldScore->Show(false);
			worldScoreModifier->Show(false);
		}

		if(iGameWinner == iTeam)
		{
			worldScore->SetPosition(ix + 350, iTeamY + 24);
			worldScore->SetScore(tourStop->iPoints);
			worldScore->Show(true);

			miTeamImages[iTeam]->SetImage(0, 160, 496, 64);

			if(game_values.worldpointsbonus >= 0)
			{
				worldScoreModifier->SetImage((game_values.worldpointsbonus + 9) << 5, 0, 32, 32);
				worldScoreModifier->SetPosition(ix + 410, iTeamY + 14);
				worldScoreModifier->Show(true);
			}
			else
			{
				worldScoreModifier->Show(false);
			}
		}
		else
		{
			miTeamImages[iTeam]->SetImage(0, 160, 344, 64);
		}

		worldPlace[iTeam]->SetPosition(ix + 102, iTeamY + 14);
		worldPlace[iTeam]->SetImage(0, score[iTeam]->place << 5, 32, 32);
		worldPlace[iTeam]->Show(gameWinner >= 0);
	}

	short iBonusCounts[4] = {0, 0, 0, 0};
	
	if(gameWinner >= 0)
	{
		for(short iBonus = 0; iBonus < tourStop->iNumBonuses; iBonus++)
		{
			WorldStageBonus * bonus = &tourStop->wsbBonuses[iBonus];

			if(bonus->iWinnerPlace < iNumTeams)
			{
				for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
				{
					if(score[iTeam]->place == bonus->iWinnerPlace)
					{
						short iDisplayPosition = game_values.tournament_scores[iTeam].wins;

						if(iBonusCounts[iDisplayPosition] < MAX_WORLD_BONUSES_AWARDED)
						{
							worldBonus[iDisplayPosition][iBonusCounts[iDisplayPosition]]->Show(true);
							worldBonus[iDisplayPosition][iBonusCounts[iDisplayPosition]]->SetImageSource(bonus->iBonus < NUM_POWERUPS ? &spr_storedpoweruplarge : &spr_worlditems);
							worldBonus[iDisplayPosition][iBonusCounts[iDisplayPosition]]->SetImage((bonus->iBonus < NUM_POWERUPS ? bonus->iBonus : bonus->iBonus - NUM_POWERUPS) << 5, 0, 32, 32);

							iBonusCounts[iDisplayPosition]++;
						}
					}
				}
			}
		}
	}

	for(short iTeam = 0; iTeam < iNumTeams;  iTeam++)
	{
		for(short iBonus = iBonusCounts[iTeam]; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++)
		{
			worldBonus[iTeam][iBonus]->Show(false);
		}
	}

	if(gameWinner >= 0)
		game_values.worldpointsbonus = -1;
}

//Called by Tour -- Arranges players in terms of standings
void MI_TournamentScoreboard::RefreshTourScores()
{
	DetermineScoreboardWinners();

	for(short iTeam = 0; iTeam < iNumTeams; iTeam++)
	{
		short iTeamY = GetYFromPlace(game_values.tournament_scores[iTeam].wins);
		iTeamY += 28; //shift down 28 pxls for extra tour points bar

		float dSpacing = GetIconSpacing();

		miTeamImages[iTeam]->SetPosition(ix - 40, iTeamY);

		for(short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++)
		{
			static short iPlaceSprite[4] = {4, 0, 8, 9};
			miPlayerImages[iTeam][iPlayer]->SetPosition(ix + iScoreboardPlayerOffsetsX[iTeamCounts[iTeam] - 1][iPlayer] - 40, iTeamY + 16);
			miPlayerImages[iTeam][iPlayer]->SetImageSource(spr_player[iTeamIDs[iTeam][iPlayer]][iPlaceSprite[game_values.tournament_scores[iTeam].wins]]);
		}

		for(short iGame = 0; iGame < game_values.tourstopcurrent; iGame++)
		{
			miIconImages[iTeam][iGame]->SetImage(0, game_values.tournament_scores[iTeam].type[iGame] * 32, 32, 32);

			miIconImages[iTeam][iGame]->SetPosition(ix + 128 + (short)dSpacing + (short)((float)iGame * (32.0f + dSpacing)) - 40, iTeamY + 16);

			miIconImages[iTeam][iGame]->SetSwirl(false, 0.0f, 0.0f, 0.0f, 0.0f);
			miIconImages[iTeam][iGame]->SetPulse(false);
			miIconImages[iTeam][iGame]->Show(true);
		}

		for(short iGame = game_values.tourstopcurrent; iGame < game_values.tourstoptotal; iGame++)
		{
			miIconImages[iTeam][iGame]->Show(false);
		}

		tourScores[iTeam]->SetPosition(ix + 508, iTeamY + 24);
		tourScores[iTeam]->SetScore(game_values.tournament_scores[iTeam].total);
	}
}

void MI_TournamentScoreboard::DetermineScoreboardWinners()
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

		game_values.tournamentwinner = iTournamentWinner;

		//Flash the background of the winning teams
		for(short iTeam = 0; iTeam < iNumWinningTeams; iTeam++)
		{
			miTeamImages[iWinningTeams[iTeam]]->SetAnimationSpeed(20);
			
			if(worldPointsBackground[iWinningTeams[iTeam]])
				worldPointsBackground[iWinningTeams[iTeam]]->SetAnimationSpeed(20);
		}
	}
}

//Called by Tournament -- Keeps players where they are and displays number of wins and mode type of win
void MI_TournamentScoreboard::RefreshTournamentScores(short gameWinner)
{
	iGameWinner = gameWinner;
	iSwirlIconTeam = -1;
	iSwirlIconGame = -1;

	if(game_values.tournament_scores[iGameWinner].wins == iNumGames)
	{
		iTournamentWinner = iGameWinner;
		miTeamImages[iTournamentWinner]->SetAnimationSpeed(20);

		if(worldPointsBackground[iTournamentWinner])
			worldPointsBackground[iTournamentWinner]->SetAnimationSpeed(20);
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

MI_BonusWheel::MI_BonusWheel(short x, short y) :
	UI_Control(x, y)
{
	miPlayerImages = NULL;
	fCpuControlled = false;

	for(short iImage = 0; iImage < NUMBONUSITEMSONWHEEL; iImage++)
	{
		dSelectionSector[iImage] = (float)iImage * TWO_PI / (float)(NUMBONUSITEMSONWHEEL);
		
		short iPowerupX = x + 160 + (short)(110.0f * cos(dSelectionSector[iImage]));
		short iPowerupY = y + 208 + (short)(110.0f * sin(dSelectionSector[iImage]));

		miBonusImages[iImage] = new MI_Image(&spr_storedpoweruplarge, iPowerupX, iPowerupY, 0, 0, 32, 32, 1, 1, 0);
	}

	//Fix the last sector to allow correct detection of sector for tick sound
	dSelectionSector[NUMBONUSITEMSONWHEEL] = TWO_PI;

	miContinueButton = new MI_Button(&menu_plain_field, ix + 76, iy + 390, "Continue", 200, 1);
	miContinueButton->Show(false);
	miContinueButton->SetCode(MENU_CODE_BONUS_DONE);
}

MI_BonusWheel::~MI_BonusWheel()
{
	if(miBonusImages)
	{
		for(int iImage = 0; iImage < NUMBONUSITEMSONWHEEL; iImage++)
			delete miBonusImages[iImage];
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
	if(fModify && !fPressedSelect)
	{
		fPressedSelect = true;

		float dNumWinddownSteps = dSelectionSpeed / 0.0005f - 1;
		float dWinddownAngle = dSelectionSpeed / 2.0f * dNumWinddownSteps;
		float dFinalAngle = dSelectionAngle + dWinddownAngle;
		
		//Bring the radians back down to between 0 and TWO_PI to do comparisons to the powerups on the wheel
		while(dFinalAngle > TWO_PI)
			dFinalAngle -= TWO_PI;

		float dSectorSize = TWO_PI / NUMBONUSITEMSONWHEEL;
		for(short iSector = 0; iSector < NUMBONUSITEMSONWHEEL; iSector++)
		{
			if(dFinalAngle >= iSector * dSectorSize && dFinalAngle < (iSector + 1) * dSectorSize)
			{
				iSelectedPowerup = iChosenPowerups[iSector + 1 >= NUMBONUSITEMSONWHEEL ? 0 : iSector + 1];
				float dNewWinddownAngle = dWinddownAngle + (iSector + 1) * dSectorSize - dFinalAngle;

				//Determine the speed we need to exactly hit the selected powerup when the selector winds down
				dSelectionWinddownSpeed = dSelectionSpeed / (dNewWinddownAngle * 2.0f / dSelectionSpeed + 1.0f);
				break;
			}
		}
	}

	if(fPowerupSelectionDone)
		return miContinueButton->Modify(fModify);

	return MENU_CODE_NONE;
}

void MI_BonusWheel::Update()
{
	if(iState == 0)
	{
		if(--iDisplayPowerupTimer <= 0)
		{
			iDisplayPowerupTimer = 20;

			short iPoofX = ix + 152 + (short)(110.0f * cos(dSelectionSector[iDisplayPowerupIndex]));
			short iPoofY = iy + 200 + (short)(110.0f * sin(dSelectionSector[iDisplayPowerupIndex]));

			uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_poof, iPoofX, iPoofY, 4, 5));

			ifsoundonplay(sfx_cannon);

			if(++iDisplayPowerupIndex >= NUMBONUSITEMSONWHEEL)
			{
				iState = 1;
			}
		}
	}
	else
	{
		if(!fPressedSelect && (fCpuControlled || ++iPressSelectTimer > 620))
			Modify(true);

		for(int iImage = 0; iImage < NUMBONUSITEMSONWHEEL; iImage++)
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

		if(iSelectionSpeedTimer > 0)
		{
			if(--iSelectionSpeedTimer <= 0)
			{
				dSelectionSpeedGoal = (float)(rand() % 100 + 200) * 0.0005f;
				iSelectionSpeedTimer = 0;
			}
		}

		if(fPressedSelect)
		{
			dSelectionSpeed -= dSelectionWinddownSpeed;
		}
		else
		{
			if(dSelectionSpeed < dSelectionSpeedGoal)
			{
				dSelectionSpeed += 0.0005f;

				if(dSelectionSpeed >= dSelectionSpeedGoal)
				{
					dSelectionSpeed = dSelectionSpeedGoal;
					iSelectionSpeedTimer = rand() % 60 + 30;
				}
			}
			else if(dSelectionSpeed > dSelectionSpeedGoal)
			{
				dSelectionSpeed -= 0.0005f;

				if(dSelectionSpeed <= dSelectionSpeedGoal)
				{
					dSelectionSpeed = dSelectionSpeedGoal;
					iSelectionSpeedTimer = rand() % 60 + 30;
				}
			}
		}

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

		//If we hit the next powerup, play a tick sound
		if(dSelectionAngle >= dSelectionSector[iNextSelectionSoundIndex])
		{
			ifsoundonplay(sfx_worldmove);

			if(++iNextSelectionSoundIndex > NUMBONUSITEMSONWHEEL)
				iNextSelectionSoundIndex = 1;
		}

		while(dSelectionAngle > TWO_PI)
			dSelectionAngle -= TWO_PI;
	}
}
		
void MI_BonusWheel::Draw()
{
	if(!fShow)
		return;

	spr_tournament_powerup_splash.draw(ix, iy);

	short iSelectorX = ix + 144 + (short)(110.0f * cos(dSelectionAngle));
	short iSelectorY = iy + 190 + (short)(110.0f * sin(dSelectionAngle));

	if(iState > 0)
		spr_powerupselector.draw(iSelectorX, iSelectorY, iSelectorAnimation * 64, 0, 64, 64);

	for(int iImage = 0; iImage < NUMBONUSITEMSONWHEEL; iImage++)
	{
		if(iImage >= iDisplayPowerupIndex)
			break;

		miBonusImages[iImage]->Draw();
	}

	for(int iPlayer = 0; iPlayer < iNumPlayers; iPlayer++)
	{
		miPlayerImages[iPlayer]->Draw();
	}

	miContinueButton->Draw();

	if(iState == 1 && !fPressedSelect)
		menu_font_large.drawCentered(320, iy + 390, "Press a Button To Stop The Wheel");
}

void MI_BonusWheel::Reset(bool fTournament)
{
	//Setup the state so that we make powerups appear one by one before the wheel starts spinning
	iState = 0;
	iDisplayPowerupIndex = 0;
	iDisplayPowerupTimer = 0;

	if(fTournament)
		iWinningTeam = game_values.tournamentwinner;
	else
		iWinningTeam = game_values.gamemode->winningteam;

	//Randomly display the powerups around the ring
	short iCountWeight = 0;
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		iCountWeight += game_values.powerupweights[iPowerup];

	//Always have at least 1 poison mushroom to try to avoid
	short iPoisonMushroom = rand() % NUMBONUSITEMSONWHEEL;

	for(short iPowerup = 0; iPowerup < NUMBONUSITEMSONWHEEL; iPowerup++)
	{
		int iChoosePowerup = 0;

		if(iCountWeight > 0 && iPoisonMushroom != iPowerup)
		{
			int iRandPowerup = rand() % iCountWeight + 1;
			int iPowerupWeightCount = game_values.powerupweights[iChoosePowerup];

			while(iPowerupWeightCount < iRandPowerup)
				iPowerupWeightCount += game_values.powerupweights[++iChoosePowerup];
		}

		miBonusImages[iPowerup]->SetImage(iChoosePowerup << 5, 0, 32, 32);
		iChosenPowerups[iPowerup] = iChoosePowerup;
	}
	
	//Setup player images on wheel
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

	//Indicate that the player hasn't choosen a powerup yet
	iPressSelectTimer = 0;
	fPressedSelect = false;
	fPowerupSelectionDone = false;
	miContinueButton->Show(false);

	//Counters to animate the selector's wings
	iSelectorAnimation = 0;
	iSelectorAnimationCounter = 0;

	//Figure out the initial position and speed of the selector
	dSelectionSpeed = (float)(rand() % 100 + 200) * 0.0005f;
	dSelectionAngle = (float)(rand() % NUMBONUSITEMSONWHEEL) * TWO_PI / (float)(NUMBONUSITEMSONWHEEL);
	dSelectionSpeedGoal = (float)(rand() % 100 + 200) * 0.0005f;
	iSelectionSpeedTimer = 0;

	for(short iSector = 0; iSector < NUMBONUSITEMSONWHEEL; iSector++)
	{
		if(dSelectionAngle > dSelectionSector[iSector])
			iNextSelectionSoundIndex = iSector + 1;
		else
			break;
	}

	//Figure out if only cpus are on the winning team, if so, the wheel will be stopped early
	fCpuControlled = true;
	for(short iPlayer = 0; iPlayer < game_values.teamcounts[iWinningTeam]; iPlayer++)
	{
		if(game_values.playercontrol[game_values.teamids[iWinningTeam][iPlayer]] == 1)
			fCpuControlled = false;
	}
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
		//Only allow the controlling team to control the menu (if there is one)
		if(iControllingTeam != -1)
		{
			if(iControllingTeam != LookupTeamID(iPlayer) || game_values.playercontrol[iPlayer] != 1)
				continue;
		}

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
		//Only allow the controlling team to control the menu (if there is one)
		if(iControllingTeam != -1)
		{
			if(iControllingTeam != LookupTeamID(iPlayer) || game_values.playercontrol[iPlayer] != 1)
				continue;
		}

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
		
		if(mapSurfaces[iMap])
			SDL_FreeSurface(mapSurfaces[iMap]);

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

	sMapSurface[0] = SDL_CreateRGBSurface(screen->flags, 768, 608, screen->format->BitsPerPixel, 0, 0, 0, 0);
	sMapSurface[1] = SDL_CreateRGBSurface(screen->flags, 768, 608, screen->format->BitsPerPixel, 0, 0, 0, 0);

	iCurrentSurfaceIndex = 0;
	iCycleIndex = 0;

	rectSrcSurface.x = 0;
	rectSrcSurface.y = 0;
	rectSrcSurface.w = 768;
	rectSrcSurface.h = 608;

	rectDstSurface.x = 0;
	rectDstSurface.y = 0;
	rectDstSurface.w = 640;
	rectDstSurface.h = 480;
}

MI_World::~MI_World()
{
	if(sMapSurface[0])
	{
		SDL_FreeSurface(sMapSurface[0]);
		sMapSurface[0] = NULL;
	}
	
	if(sMapSurface[1])
	{
		SDL_FreeSurface(sMapSurface[1]);
		sMapSurface[1] = NULL;
	}
}

void MI_World::Init()
{
	iCycleIndex = 0;
	g_worldmap.ResetDrawCycle();

	iAnimationFrame = 0;
	iDrawFullRefresh = 0;
	
	g_worldmap.InitPlayer();

	iMapDrawOffsetCol = 0;
	iMapDrawOffsetRow = 0;

	iNextMapDrawOffsetCol = 0;
	iNextMapDrawOffsetRow = 0;

	iState = -2;
	
	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		iStateTransition[iTeam] = 0;
		iItemPopupDrawY[iTeam] = 0;
		iPopupFlag[iTeam] = false;
	}
	
	iNumPopups = 0;
	
	iStoredItemPopupDrawY = -48;

	iVehicleId = -1;

	SetMapOffset();
	RepositionMapImage();

	g_worldmap.DrawMapToSurface(-1, true, sMapSurface[0], iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
	g_worldmap.DrawMapToSurface(-1, true, sMapSurface[1], iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

	dTeleportStarRadius = 300.0f;
	dTeleportStarAngle = 0.0f;
	iTeleportStarAnimationFrame = 0;
	iTeleportStarAnimationTimer = 0;

	fForceStageStart = false;
	fNoInterestingMoves = false;
	iSleepTurns = 0;
	fUsingCloud = false;
	game_values.worldpointsbonus = -1;

	iPressSelectTimer = 0;
	pressSelectKeys = NULL;

	iDrawWidth = g_worldmap.iWidth < 20 ? g_worldmap.iWidth << 5 : 640;
	iDrawHeight = g_worldmap.iHeight < 15 ? g_worldmap.iHeight << 5 : 480;

	iSrcOffsetX = 0;
	iSrcOffsetY = 0;
	iDstOffsetX = 0;
	iDstOffsetY = 0;

	if(g_worldmap.iWidth < 20)
		iDstOffsetX = (20 - g_worldmap.iWidth) << 4;

	if(g_worldmap.iHeight < 15)
		iDstOffsetY = (15 - g_worldmap.iHeight) << 4;
}

void MI_World::SetControllingTeam(short iWinningTeam)
{
	iControllingTeam = iWinningTeam;
	iControllingPlayerId = game_values.teamids[iControllingTeam][rand() % game_values.teamcounts[iControllingTeam]];
	g_worldmap.SetPlayerSprite(iControllingPlayerId);

	fNoInterestingMoves = false;
}

void MI_World::DisplayTeamControlAnnouncement()
{
	char szMessage[128];
	if(game_values.teamcounts[iControllingTeam] <= 1)
		sprintf(szMessage, "Player %d Is In Control", game_values.teamids[iControllingTeam][0] + 1);
	else
		sprintf(szMessage, "Team %d Is In Control", iControllingTeam + 1);

	uiMenu->AddEyeCandy(new EC_Announcement(&menu_font_large, &spr_announcementicons, szMessage, game_values.colorids[iControllingPlayerId], 120, 100));
}

void MI_World::SetCurrentStageToCompleted(short iWinningTeam)
{
	short iBonusAdd = 0;
	short iBonusMult = 1;

	if(game_values.worldpointsbonus == 0)
		iBonusMult = 0;
	else if(game_values.worldpointsbonus >= 1 && game_values.worldpointsbonus <= 3)
		iBonusAdd = game_values.worldpointsbonus;
	else if(game_values.worldpointsbonus >= 4 && game_values.worldpointsbonus <= 5)
		iBonusMult = game_values.worldpointsbonus - 2;

	if(iVehicleId >= 0)
	{
		game_values.tournament_scores[iWinningTeam].total += g_worldmap.GetVehicleStageScore(iVehicleId) * iBonusMult + iBonusAdd;
		g_worldmap.RemoveVehicle(iVehicleId);
	}
	else
	{
		short iPlayerCurrentTileX, iPlayerCurrentTileY;
		g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

		WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];
		//tile->iForegroundSprite = game_values.colorids[game_values.teamids[iWinningTeam][0]] + WORLD_WINNING_TEAM_SPRITE_OFFSET; //Update with team completed sprite
		//tile->fAnimated = false; //Update with team completed sprite
		tile->iCompleted = game_values.colorids[game_values.teamids[iWinningTeam][0]];

		g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
		g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
		
		game_values.tournament_scores[iWinningTeam].total += game_values.tourstops[tile->iType - 6]->iPoints * iBonusMult + iBonusAdd;
	}

	//Only advance the turn if the players played a real game, no bonus houses
	if(game_values.singleplayermode == -1)
		AdvanceTurn();
}

void MI_World::AdvanceTurn()
{
	if(iSleepTurns > 0)
	{
		if(--iSleepTurns <= 0)
		{
			backgroundmusic[5].load(worldmusiclist.GetMusic(g_worldmap.GetMusicCategory(), g_worldmap.GetWorldName()));
			backgroundmusic[5].play(false, false);
		}
	}
	else
	{
		g_worldmap.MoveVehicles();
	}

	g_worldmap.MoveBridges();

	//Update the map with the flipped bridges
	//TODO:: Need to update just bridge tiles across both surfaces

	fNoInterestingMoves = false;
}

void MI_World::Update()
{
	bool fPlayerVehicleCollision = false;
	bool fPlayerMoveDone = g_worldmap.Update(&fPlayerVehicleCollision);

	short iPlayerX, iPlayerY;
	g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);

	short iPlayerState = g_worldmap.GetPlayerState();

	UpdateMapSurface(iCycleIndex);

	if(++iCycleIndex > 16)
	{
		iCurrentSurfaceIndex = 1 - iCurrentSurfaceIndex;

		if(iDrawFullRefresh == 1)
			iDrawFullRefresh = 2;
		else
			iDrawFullRefresh = 0;

		iCycleIndex = 0;
		iAnimationFrame += TILESIZE;

		if(iAnimationFrame >= 128)
			iAnimationFrame = 0;
	}

	if(fPlayerMoveDone)
		RepositionMapImage();

	if(iSleepTurns <= 0 && !fUsingCloud && (fPlayerVehicleCollision || fPlayerMoveDone))
	{
		short iStage = g_worldmap.GetVehicleInPlayerTile(&iVehicleId);
		if(iStage >= 0 && iStage < g_worldmap.iNumStages)
		{
			game_values.tourstopcurrent = iStage;
			fForceStageStart = true;
		}
	}

	//Player is moving from one tile to the next (up)
	if(iPlayerState == 1)
	{
		if(g_worldmap.iHeight > 15 && iMapOffsetY < 0 && iPlayerY < (g_worldmap.iHeight << 5) - 256)
			iMapOffsetY += 2;
	}
	else if(iPlayerState == 2) //down
	{
		if(g_worldmap.iHeight > 15 && iMapOffsetY > 480 - (g_worldmap.iHeight << 5) && iPlayerY > 224)
			iMapOffsetY -= 2;
	}
	else if(iPlayerState == 3) //left
	{
		if(g_worldmap.iWidth > 20 && iMapOffsetX < 0 && iPlayerX < (g_worldmap.iWidth << 5) - 336)
			iMapOffsetX += 2;
	}
	else if(iPlayerState == 4) //right
	{
		if(g_worldmap.iWidth > 20 && iMapOffsetX > 640 - (g_worldmap.iWidth << 5) && iPlayerX > 304)
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
	
	if(iNumPopups > 0)
	{
		for(short iTeam = 0; iTeam < 4; iTeam++)
		{
			//Do inventory popup open effect
			if(iStateTransition[iTeam] == 1)
			{
				iItemPopupDrawY[iTeam] += 4;

				if(iItemPopupDrawY[iTeam] >= 32)
				{
					iItemPopupDrawY[iTeam] = 32;
					iStateTransition[iTeam] = 3;
				}
			}
			else if(iStateTransition[iTeam] == 2) // Do close effect
			{
				iItemPopupDrawY[iTeam] -= 4;

				if(iItemPopupDrawY[iTeam] <= 0)
				{
					iItemPopupDrawY[iTeam] = 0;
					iStateTransition[iTeam] = 0;

					//Shift down popup menus if one was removed that was below
					bool fStartShift = false;
					for(short iMoveTeam = 0; iMoveTeam < iNumPopups - 1; iMoveTeam++)
					{
						if(iPopupOrder[iMoveTeam] == iTeam)
						{
							fStartShift = true;
						}

						if(fStartShift)
						{
							iPopupOrder[iMoveTeam] = iPopupOrder[iMoveTeam + 1];
							iPopupOffsets[iPopupOrder[iMoveTeam]] = iMoveTeam << 6;
						}
					}

					iNumPopups--;
				}
			}
		}

		for(short iTeam = 0; iTeam < 4; iTeam++)
		{
			//Transition the inventory popup to it's new location if it needs to move
			if(iPopupOffsetsCurrent[iTeam] > iPopupOffsets[iTeam])
			{
				iPopupOffsetsCurrent[iTeam] -= 4;
				if(iPopupOffsetsCurrent[iTeam] < iPopupOffsets[iTeam])
					iPopupOffsetsCurrent[iTeam] = iPopupOffsets[iTeam];
			}
		}
	}

	bool iShowStoredItems = false;
	for(short iTeam = 0; iTeam < 4; iTeam++)
	{
		if(iStateTransition[iTeam] == 1 || iStateTransition[iTeam] == 3)
		{
			iShowStoredItems = true;
			break;
		}
	}

	if(iShowStoredItems && iStoredItemPopupDrawY < 16)
		iStoredItemPopupDrawY += 8;
	else if(!iShowStoredItems && iStoredItemPopupDrawY > -48)
		iStoredItemPopupDrawY -= 8;

	
	if(iState == 4 || iState == 5)
	{
		iScreenfade += iScreenfadeRate;

		if(iState == 4 && iScreenfade > 255)
		{
			g_worldmap.SetPlayerPosition(iWarpCol, iWarpRow);
			SetMapOffset();
			RepositionMapImage();

			//These 3 lines allow us to only refresh the entire map to one surface and sets it up so that the 
			//other surface will update with the normal flow
			iCurrentSurfaceIndex = 0;
			iCycleIndex = 0;
			g_worldmap.ResetDrawCycle();

			iDrawFullRefresh = 2; //Draw one full refresh to next surface

			g_worldmap.DrawMapToSurface(-1, true, sMapSurface[0], iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
			//g_worldmap.DrawMapToSurface(-1, true, sMapSurface[1], iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

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

void MI_World::UpdateMapSurface(short iCycleIndex)
{
	if(iCycleIndex >= 0 && iCycleIndex <= 15)
		g_worldmap.DrawMapToSurface(iCycleIndex, iDrawFullRefresh > 0, sMapSurface[1 - iCurrentSurfaceIndex], iNextMapDrawOffsetCol, iNextMapDrawOffsetRow, iAnimationFrame);
}

void MI_World::SetMapOffset()
{
	short iPlayerX, iPlayerY;
	g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);

	if(g_worldmap.iWidth > 20)
	{
		if(iPlayerX < (g_worldmap.iWidth << 5) - 336 && iPlayerX > 304)
			iMapOffsetX = 304 - iPlayerX;
		else if(iPlayerX <= 304)
			iMapOffsetX = 0;
		else
			iMapOffsetX = 640 - (g_worldmap.iWidth << 5);
	}
	else
	{
		iMapOffsetX = (640 - (g_worldmap.iWidth << 5)) >> 1;
	}

	if(g_worldmap.iHeight > 15)
	{
		if(iPlayerY < (g_worldmap.iHeight << 5) - 256 && iPlayerY > 224)
			iMapOffsetY = 224 - iPlayerY;
		else if(iPlayerY <= 224)
			iMapOffsetY = 0;
		else
			iMapOffsetY = 480 - (g_worldmap.iHeight << 5);
	}
	else
	{
		iMapOffsetY = (480 - (g_worldmap.iHeight << 5)) >> 1;
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

	iNextMapDrawOffsetCol = iMapDrawOffsetCol;
	iNextMapDrawOffsetRow = iMapDrawOffsetRow;
}


void MI_World::Draw()
{
	if(!fShow)
		return;

	short iPlayerX, iPlayerY;
	g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);
	
	if(g_worldmap.iWidth > 20)
		iSrcOffsetX = -iMapOffsetX - (iMapDrawOffsetCol << 5);

	if(g_worldmap.iHeight > 15)
		iSrcOffsetY = -iMapOffsetY - (iMapDrawOffsetRow << 5);

	gfx_setrect(&rectSrcSurface, iSrcOffsetX, iSrcOffsetY, iDrawWidth, iDrawHeight);
	gfx_setrect(&rectDstSurface, iDstOffsetX, iDstOffsetY, iDrawWidth, iDrawHeight);

	SDL_BlitSurface(sMapSurface[iCurrentSurfaceIndex], &rectSrcSurface, blitdest, &rectDstSurface);

	//Draw the world, vehicles and player
	g_worldmap.Draw(iMapOffsetX, iMapOffsetY, iState == -1 && !fUsingCloud, iSleepTurns > 0);

	//Draw the cloud if the player is one
	if(fUsingCloud && iState == -1)
		spr_worlditems.draw(iPlayerX + iMapOffsetX, iPlayerY + iMapOffsetY, 32, 0, 32, 32);

	//If a points modifier is in place, display it
	if(game_values.worldpointsbonus >= 0)
	{
		spr_worlditems.draw(603, 5, (game_values.worldpointsbonus + 9) << 5, 0, 32, 32);
	}
	
	//Draw the teleport/warp stars effect
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
	
	//If the item selector for a player is displayed
	if(iNumPopups > 0)
	{
		//Draw Spots and Stored Powerups
		short iPlayerCount = 0;
		for(short iCountPlayers = 0; iCountPlayers < 4; iCountPlayers++)
		{
			iPlayerCount += game_values.teamcounts[iCountPlayers];
		}

		short iStoredPowerupBoxX = 296 - 48 * (iPlayerCount - 1);
		for(short iTeamStore = 0; iTeamStore < 4; iTeamStore++)
		{
			for(short iMemberStore = 0; iMemberStore < game_values.teamcounts[iTeamStore]; iMemberStore++)
			{
				short iPlayerId = game_values.teamids[iTeamStore][iMemberStore];
				
				spr_worlditempopup.draw(iStoredPowerupBoxX, iStoredItemPopupDrawY, game_values.colorids[iPlayerId] * 48, 256, 48, 48);

				spr_storedpoweruplarge.draw(iStoredPowerupBoxX + 8, iStoredItemPopupDrawY + 8, game_values.storedpowerups[iPlayerId] << 5, 0, 32, 32);

				iStoredPowerupBoxX += 96;
			}
		}

		for(short iTeam = 0; iTeam < 4; iTeam++)
		{
			if(iStateTransition[iTeam] != 0)
			{
				short iColorId = game_values.colorids[game_values.teamids[iTeam][0]];
				spr_worlditempopup.draw(0, 448 - iItemPopupDrawY[iTeam] - iPopupOffsetsCurrent[iTeam], 0, iColorId * 64 + 32 - iItemPopupDrawY[iTeam], 320, iItemPopupDrawY[iTeam] << 1);
				spr_worlditempopup.draw(320, 448 - iItemPopupDrawY[iTeam] - iPopupOffsetsCurrent[iTeam], 192, iColorId * 64 + 32 - iItemPopupDrawY[iTeam], 320, iItemPopupDrawY[iTeam] << 1);

				if(iStateTransition[iTeam] == 3)
				{
					short iNumPowerups = game_values.worldpowerupcount[iTeam];
					
					if(iNumPowerups > 0)
						spr_worlditempopup.draw(iItemCol[iTeam] * 52 + 114, 424 - iPopupOffsetsCurrent[iTeam], iColorId * 48, 256, 48, 48);

					short iStartItem = iItemPage[iTeam] << 3;
					for(short iItem = iStartItem; iItem < iStartItem + 8 && iItem < iNumPowerups; iItem++)
					{
						short iPowerup = game_values.worldpowerups[iTeam][iItem];
						if(iPowerup >= NUM_POWERUPS)
							spr_worlditems.draw((iItem - iItemPage[iTeam] * 8) * 52 + 122, 432 - iPopupOffsetsCurrent[iTeam], (iPowerup - NUM_POWERUPS) << 5, 0, 32, 32);
						else
							spr_storedpoweruplarge.draw((iItem - iItemPage[iTeam] * 8) * 52 + 122, 432 - iPopupOffsetsCurrent[iTeam], iPowerup << 5, 0, 32, 32);
					}
				}
			}
		}
	}
	
	if(iState == 4 || iState == 5)
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
		
		for(short iTeam = 0; iTeam < 4; iTeam++)
		{
			iNumPopups = 0;
			iStateTransition[iTeam] = 0;
			iItemPopupDrawY[iTeam] = 0;
			iPopupFlag[iTeam] = false;
		}

		fForceStageStart = false;
		return MENU_CODE_TOUR_STOP_CONTINUE_FORCED;
	}

	short iPlayerState = g_worldmap.GetPlayerState();

	//Handle AI movement
	bool fNeedAiControl = false;
	if(iState == -1 && iPlayerState == 0 && iNumPopups == 0)
	{
		if(!fNoInterestingMoves)
		{
			fNeedAiControl = true;

			short iTeamMember;
			for(iTeamMember = 0; iTeamMember < game_values.teamcounts[iControllingTeam]; iTeamMember++)
			{
				if(game_values.playercontrol[game_values.teamids[iControllingTeam][iTeamMember]] == 1)
				{
					fNeedAiControl = false;
					break;
				}
			}

			if(fNeedAiControl)
			{
				if(iPressSelectTimer == 0)
				{
					short iPlayerCurrentTileX, iPlayerCurrentTileY;
					g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);
					short iNextMove = g_worldmap.GetNextInterestingMove(iPlayerCurrentTileX, iPlayerCurrentTileY);

					//Clear out all input from cpu controlled team
					COutputControl * playerKeys = NULL;
					for(iTeamMember = 0; iTeamMember < game_values.teamcounts[iControllingTeam]; iTeamMember++)
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

					//If there are no AI moves to make
					if(iNextMove == -1)
					{
						bool fUsedItem = false;

						//See if we are touching a door
						bool fDoor[4] = {false, false, false, false};
						g_worldmap.IsTouchingDoor(iPlayerCurrentTileX, iPlayerCurrentTileY, fDoor);

						//See if we can use a key to keep going
						for(short iPowerup = 0; iPowerup < game_values.worldpowerupcount[iControllingTeam]; iPowerup++)
						{
							short iType = game_values.worldpowerups[iControllingTeam][iPowerup];

							if(iType >= NUM_POWERUPS + 5 && iType <= NUM_POWERUPS + 8) //Door Keys
							{
								short iKeyType = iType - NUM_POWERUPS - 5;
								if(fDoor[iKeyType])
								{
									fUsedItem = UsePowerup(game_values.teamids[iControllingTeam][iTeamMember], iControllingTeam, iPowerup, false);
								}
							}
						}

						//if not, flag that there are no moves to make
						fNoInterestingMoves = !fUsedItem;
					}
					if(iNextMove == 0)
						playerKeys->menu_up.fPressed = true;
					else if(iNextMove == 1)
						playerKeys->menu_down.fPressed = true;
					else if(iNextMove == 2)
						playerKeys->menu_left.fPressed = true;
					else if(iNextMove == 3)
						playerKeys->menu_right.fPressed = true;
					else if(iNextMove == 4)
					{
						pressSelectKeys = playerKeys;
						iPressSelectTimer = 60;
					}
				}
				else
				{
					if(--iPressSelectTimer <= 0)
					{
						iPressSelectTimer = 0;
						pressSelectKeys->menu_select.fPressed = true;
					}
				}
			}
			else
			{
				iPressSelectTimer = 0;
			}
		}
	}

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];
		
		short iPlayerCurrentTileX, iPlayerCurrentTileY;
		g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

		short iTeamId = LookupTeamID(iPlayer);

		if(iState == -1 && iNumPopups == 0 && iPopupFlag[0] == false && iPopupFlag[1] == false && iPopupFlag[2] == false && iPopupFlag[3] == false)
		{
			if(iControllingTeam == LookupTeamID(iPlayer) && iPlayerState == 0 && game_values.playercontrol[iPlayer] > 0) //if this player is player or cpu
			{
				WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];

				short iTemp; //Just a temp value so we can call the GetVehicleInPlayerTile method
				bool fVehicleInTile = g_worldmap.GetVehicleInPlayerTile(&iTemp) >= 0;

				if(playerKeys->menu_up.fPressed || playerKeys->menu_up.fDown)
				{
					//Make sure there is a path connection and that there is no stage or vehicle blocking the way
					if((tile->fConnection[0] || tile->iConnectionType == 14) && !g_worldmap.IsDoor(iPlayerCurrentTileX, iPlayerCurrentTileY - 1) &&
						(((tile->iCompleted >= -1 || (tile->iType >= 6 && game_values.tourstops[tile->iType - 6]->iStageType == 1)) && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 0 || fUsingCloud))
					{
						if(fUsingCloud && (tile->iCompleted == -2 || fVehicleInTile) && iReturnDirection != 0)
							UseCloud(false);

						g_worldmap.MovePlayer(0);
						iPlayerState = g_worldmap.GetPlayerState();

						iReturnDirection = 1;

						ifsoundonplay(sfx_worldmove);

						//Start draw cycle over
						RestartDrawCycleIfNeeded();
					}
					else
					{
						if(playerKeys->menu_up.fPressed)
							ifsoundonplay(sfx_hit);
					}
				}
				else if(playerKeys->menu_down.fPressed || playerKeys->menu_down.fDown)
				{
					if((tile->fConnection[1] || tile->iConnectionType == 14) && !g_worldmap.IsDoor(iPlayerCurrentTileX, iPlayerCurrentTileY + 1) &&
						(((tile->iCompleted >= -1 || (tile->iType >= 6 && game_values.tourstops[tile->iType - 6]->iStageType == 1)) && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 1 || fUsingCloud))
					{
						if(fUsingCloud && (tile->iCompleted == -2 || fVehicleInTile) && iReturnDirection != 1)
							UseCloud(false);

						g_worldmap.MovePlayer(1);
						iPlayerState = g_worldmap.GetPlayerState();

						iReturnDirection = 0;

						//Start draw cycle over
						RestartDrawCycleIfNeeded();

						ifsoundonplay(sfx_worldmove);
					}
					else
					{
						if(playerKeys->menu_down.fPressed)
							ifsoundonplay(sfx_hit);
					}
				}
				else if(playerKeys->menu_left.fPressed || playerKeys->menu_left.fDown)
				{
					if((tile->fConnection[2] || tile->iConnectionType == 12) && !g_worldmap.IsDoor(iPlayerCurrentTileX - 1, iPlayerCurrentTileY) &&
						(((tile->iCompleted >= -1 || (tile->iType >= 6 && game_values.tourstops[tile->iType - 6]->iStageType == 1)) && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 2 || fUsingCloud))
					{
						if(fUsingCloud && (tile->iCompleted == -2 || fVehicleInTile) && iReturnDirection != 2)
							UseCloud(false);

						g_worldmap.MovePlayer(2);
						iPlayerState = g_worldmap.GetPlayerState();

						iReturnDirection = 3;

						//Start draw cycle over
						RestartDrawCycleIfNeeded();

						ifsoundonplay(sfx_worldmove);
					}
					else
					{
						g_worldmap.FacePlayer(1);

						if(playerKeys->menu_left.fPressed)
							ifsoundonplay(sfx_hit);
					}
				}
				else if(playerKeys->menu_right.fPressed || playerKeys->menu_right.fDown)
				{
					if((tile->fConnection[3] || tile->iConnectionType == 12) && !g_worldmap.IsDoor(iPlayerCurrentTileX + 1, iPlayerCurrentTileY) &&
						(((tile->iCompleted >= -1 || (tile->iType >= 6 && game_values.tourstops[tile->iType - 6]->iStageType == 1)) && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 3 || fUsingCloud))
					{
						if(fUsingCloud && (tile->iCompleted == -2 || fVehicleInTile) && iReturnDirection != 3)
							UseCloud(false);

						g_worldmap.MovePlayer(3);
						iPlayerState = g_worldmap.GetPlayerState();

						iReturnDirection = 2;

						//Start draw cycle over
						RestartDrawCycleIfNeeded();

						ifsoundonplay(sfx_worldmove);
					}
					else
					{
						g_worldmap.FacePlayer(0);
						
						if(playerKeys->menu_right.fPressed)
							ifsoundonplay(sfx_hit);
					}
				}
				else if(playerInput->outputControls[iPlayer].menu_select.fPressed)
				{
					//Lookup current tile and see if it is a type of tile you can interact with
					//If there is a vehicle on this tile, then load it's stage
					short iStage = g_worldmap.GetVehicleInPlayerTile(&iVehicleId);
					if(iStage >= 0 && iStage < g_worldmap.iNumStages)
					{
						return InitGame(iStage, iPlayer, fNeedAiControl);
					}

					//if it is a stage, then load the stage
					WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];

					short iType = tile->iType - 6;
					if(iType >= 0 && iType < g_worldmap.iNumStages && tile->iCompleted == -2)
					{
						return InitGame(iType, iPlayer, fNeedAiControl);
					}

					if(g_worldmap.GetWarpInPlayerTile(&iWarpCol, &iWarpRow))
					{
						iState = 4;
						iScreenfade = 0;
						iScreenfadeRate = 8;

						ifsoundonplay(sfx_pipe);
					}
				}
			}
		}
		else if (iStateTransition[iTeamId] == 3) //not transitioning to or from the item popup menu
		{
			if(playerKeys->menu_up.fPressed)
			{
				if(iItemPage[iTeamId] > 0)
				{
					iItemPage[iTeamId]--;
					iItemCol[iTeamId] = 0;
					ifsoundonplay(sfx_worldmove);
				}
				else
				{
					ifsoundonplay(sfx_hit);
				}
			}
			else if(playerKeys->menu_down.fPressed)
			{
				if(iItemPage[iTeamId] < 3 && (iItemPage[iTeamId] + 1) * 8 < game_values.worldpowerupcount[iTeamId])
				{
					iItemPage[iTeamId]++;
					iItemCol[iTeamId] = 0;
					ifsoundonplay(sfx_worldmove);
				}
				else
				{
					ifsoundonplay(sfx_hit);
				}
			}
			else if(playerKeys->menu_left.fPressed)
			{
				if(iItemCol[iTeamId] > 0)
				{
					iItemCol[iTeamId]--;
					ifsoundonplay(sfx_worldmove);
				}
				else if(iItemCol[iTeamId] == 0 && iItemPage[iTeamId] > 0)
				{
					iItemCol[iTeamId] = 7;
					iItemPage[iTeamId]--;
					ifsoundonplay(sfx_worldmove);
				}
				else
				{
					ifsoundonplay(sfx_hit);
				}
			}
			else if(playerKeys->menu_right.fPressed)
			{
				if(iItemPage[iTeamId] * 8 + iItemCol[iTeamId] + 1 < game_values.worldpowerupcount[iTeamId])
				{
					if(iItemCol[iTeamId] < 7)
					{
						iItemCol[iTeamId]++;
						ifsoundonplay(sfx_worldmove);
					}
					else if(iItemCol[iTeamId] == 7 && iItemPage[iTeamId] < 3)
					{
						iItemCol[iTeamId] = 0;
						iItemPage[iTeamId]++;
						ifsoundonplay(sfx_worldmove);
					}
					else
					{
						ifsoundonplay(sfx_hit);
					}
				}
			}
			else if(playerKeys->menu_select.fPressed)
			{
				if(game_values.worldpowerupcount[iTeamId] > 0)
				{
					short iIndex = iItemPage[iTeamId] * 8 + iItemCol[iTeamId];
					UsePowerup(iPlayer, iTeamId, iIndex, true);
				}
			}
		}

		if(iState == -1 && (iStateTransition[iTeamId] == 0 || iStateTransition[iTeamId] == 3))
		{
			if(playerKeys->menu_cancel.fPressed)
			{
				if(DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0)
				{
					fModifying = false;
					return MENU_CODE_UNSELECT_ITEM;
				}
			}
			
			if ((game_values.playercontrol[iPlayer] == 1 || fNoInterestingMoves) && (iPopupFlag[iTeamId] || playerKeys->menu_random.fPressed ||
				(iPlayer != 0 && playerInput->inputControls[iPlayer]->iDevice == DEVICE_KEYBOARD && playerKeys->menu_cancel.fPressed)))
			{
				iPopupFlag[iTeamId] = true;

				if(iPlayerState == 0 && iStateTransition[iTeamId] == 0)
				{
					iPopupFlag[iTeamId] = false;

					iPopupOrder[iNumPopups] = iTeamId;
					iPopupOffsets[iTeamId] = iNumPopups++ << 6;
					iPopupOffsetsCurrent[iTeamId] = iPopupOffsets[iTeamId];
					
					iStateTransition[iTeamId] = 1;

					iItemPage[iTeamId] = 0;
					iItemCol[iTeamId] = 0;

					ifsoundonplay(sfx_inventory);
				}
				else if (iStateTransition[iTeamId] == 3)
				{
					iPopupFlag[iTeamId] = false;

					iStateTransition[iTeamId] = 2;
					ifsoundonplay(sfx_inventory);
				}
			}
		}
	}

	return MENU_CODE_NONE;
}

void MI_World::RestartDrawCycleIfNeeded()
{
	iNextMapDrawOffsetCol = iMapDrawOffsetCol;
	iNextMapDrawOffsetRow = iMapDrawOffsetRow;

	short iPlayerDestTileX, iPlayerDestTileY;
	g_worldmap.GetPlayerDestTile(&iPlayerDestTileX, &iPlayerDestTileY);
	
	if(g_worldmap.iWidth > 24)
	{
		iNextMapDrawOffsetCol = iPlayerDestTileX - 11;
		if(iNextMapDrawOffsetCol < 0)
			iNextMapDrawOffsetCol = 0;
		else if(iNextMapDrawOffsetCol > g_worldmap.iWidth - 24)
			iNextMapDrawOffsetCol = g_worldmap.iWidth - 24;
	}

	if(g_worldmap.iHeight > 19)
	{
		iNextMapDrawOffsetRow = iPlayerDestTileY - 9;
		if(iNextMapDrawOffsetRow < 0)
			iNextMapDrawOffsetRow = 0;
		else if(iNextMapDrawOffsetRow > g_worldmap.iHeight - 19)
			iNextMapDrawOffsetRow = g_worldmap.iHeight - 19;
	}

	//Reset the draw cycle if the map offset col/row is going to move
	if(iMapDrawOffsetCol != iNextMapDrawOffsetCol || iMapDrawOffsetRow != iNextMapDrawOffsetRow)
	{
		iCycleIndex = 0;
		g_worldmap.ResetDrawCycle();
		iDrawFullRefresh = 1;
	}
}

bool MI_World::UsePowerup(short iPlayer, short iTeam, short iIndex, bool fPopupIsUp)
{
	short iPlayerCurrentTileX, iPlayerCurrentTileY;
	g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);
					
	short iPowerup = game_values.worldpowerups[iTeam][iIndex];
	bool fUsedItem = false;

	if(iPowerup < NUM_POWERUPS)
	{
		/*
		//Comment this in to give the powerup to all members of the team
		for(short iPlayer = 0; iPlayer < game_values.teamcounts[iTeam]; iPlayer++)
		{
			game_values.storedpowerups[game_values.teamids[iTeam][iPlayer]] = iPowerup;
		}*/

		//Give the powerup only to the player that selected it
		game_values.storedpowerups[iPlayer] = iPowerup;

		ifsoundonplay(sfx_collectpowerup);
		fUsedItem = true;
	}
	else if(iPowerup == NUM_POWERUPS) //Music Box (put vehicles to sleep)
	{
		iSleepTurns = rand() % 4 + 2;
		fUsedItem = true;
		ifsoundonplay(sfx_collectpowerup);

		backgroundmusic[5].load(worldmusiclist.GetMusic(WORLDMUSICSLEEP, ""));
		backgroundmusic[5].play(false, false);
	}
	else if(iPowerup == NUM_POWERUPS + 1) //Cloud (allows player to skip stages)
	{
		if(!fUsingCloud && iState == -1)
		{
			UseCloud(true);
			fUsedItem = true;
		}
	}
	else if(iPowerup == NUM_POWERUPS + 2) //Player Switch
	{
		if(iControllingTeam != iTeam && iState == -1)
		{
			SetControllingTeam(iTeam);
			DisplayTeamControlAnnouncement();

			fUsedItem = true;
			ifsoundonplay(sfx_switchpress);
			iState = 6;
			fNoInterestingMoves = false;
		}
	}
	else if(iPowerup == NUM_POWERUPS + 3 && iState == -1) //Advance Turn
	{
		short iDestX, iDestY;
		g_worldmap.GetPlayerDestTile(&iDestX, &iDestY);
		short iSprite = g_worldmap.tiles[iDestX][iDestY].iForegroundSprite;

		if(iSprite < WORLD_BRIDGE_SPRITE_OFFSET || iSprite > WORLD_BRIDGE_SPRITE_OFFSET + 3)
		{
			AdvanceTurn();
			fUsedItem = true;
			ifsoundonplay(sfx_switchpress);
		}
	}
	else if(iPowerup == NUM_POWERUPS + 4 && iState == -1) //Revive stage
	{
		short iDestX, iDestY;
		g_worldmap.GetPlayerDestTile(&iDestX, &iDestY);
		WorldMapTile * tile = &g_worldmap.tiles[iDestX][iDestY];

		if(tile->iType >= 6 && tile->iCompleted >= 0)
		{
			tile->iCompleted = -2;

			g_worldmap.UpdateTile(sMapSurface[0], iDestX - iMapDrawOffsetCol, iDestY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
			g_worldmap.UpdateTile(sMapSurface[1], iDestX - iMapDrawOffsetCol, iDestY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

			uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_poof, (iDestX << 5) + iMapOffsetX - 8, (iDestY << 5) + iMapOffsetY - 8, 4, 5));

			fUsedItem = true;
			ifsoundonplay(sfx_transform);
		}
	}
	else if(iPowerup >= NUM_POWERUPS + 5 && iPowerup <= NUM_POWERUPS + 8 && iState == -1) //Door Keys
	{
		short iDoorsOpened = g_worldmap.UseKey(iPowerup - NUM_POWERUPS - 5, iPlayerCurrentTileX, iPlayerCurrentTileY, fUsingCloud);

		if(iDoorsOpened > 0)
		{
			ifsoundonplay(sfx_transform);

			short iPlayerX = (iPlayerCurrentTileX << 5) + iMapOffsetX;
			short iPlayerY = (iPlayerCurrentTileY << 5) + iMapOffsetY;

			if(iDoorsOpened & 0x1)
			{
				uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX - TILESIZE, iPlayerY, 3, 8));

				g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol - 1, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
				g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol - 1, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
			}

			if(iDoorsOpened & 0x2)
			{
				uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX + TILESIZE, iPlayerY, 3, 8));

				g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol + 1, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
				g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol + 1, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
			}

			if(iDoorsOpened & 0x4)
			{
				uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX, iPlayerY - TILESIZE, 3, 8));

				g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow - 1, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
				g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow - 1, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
			}

			if(iDoorsOpened & 0x8)
			{
				uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_fireballexplosion, iPlayerX, iPlayerY + TILESIZE, 3, 8));

				g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow + 1, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
				g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow + 1, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
			}

			fUsedItem = true;
			fNoInterestingMoves = false;
		}
	}
	else if(iPowerup >= NUM_POWERUPS + 9 && iPowerup <= NUM_POWERUPS + 14) //Stage Points Modifiers
	{
		game_values.worldpointsbonus = iPowerup - NUM_POWERUPS - 9;
		fUsedItem = true;
		ifsoundonplay(sfx_switchpress);
	}

	if(fUsedItem)
	{
		short iNumItems = --game_values.worldpowerupcount[iTeam];

		for(short iItem = iIndex; iItem < iNumItems; iItem++)
			game_values.worldpowerups[iTeam][iItem] = game_values.worldpowerups[iTeam][iItem + 1];

		if(fPopupIsUp)
			iStateTransition[iTeam] = 2;
	}
	else
	{
		ifsoundonplay(sfx_stun);
	}

	return fUsedItem;
}

MenuCodeEnum MI_World::InitGame(short iStage, short iPlayer, bool fNeedAiControl)
{
	game_values.tourstopcurrent = iStage;
	game_values.worldskipscoreboard = false;

	bool fBonusHouse = game_values.tourstops[game_values.tourstopcurrent]->iStageType == 1;

	if(fBonusHouse)
		game_values.singleplayermode = iPlayer;

	//If player was trying to get to his inventory menu, cancel that because a game is about to start
	for(short iTeam = 0; iTeam < 4; iTeam++)
		iPopupFlag[iTeam] = false;

	if(fNeedAiControl || fBonusHouse)
		return MENU_CODE_TOUR_STOP_CONTINUE_FORCED;
	else
		return MENU_CODE_WORLD_STAGE_START;
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
	uiMenu->AddEyeCandy(new EC_SingleAnimation(&spr_poof, iPlayerX + iMapOffsetX - 8, iPlayerY + iMapOffsetY - 8, 4, 5));
}
