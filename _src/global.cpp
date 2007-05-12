#include "global.h"

short g_iVersion[] = {1, 7, 1, 0};

//We're using these strings intead of the ugly ones returned by SDL_GetKeyName()
char * Keynames[340] = {"Unknown", "", "", "", "", "", "", "", "Backspace", "Tab", 
						"",	"", "Clear", "Return", "", "", "", "", "", "Pause",
						"", "", "", "", "", "", "", "Escape", "", "",
						"", "", "Space Bar", "!", "\"", "#", "$", "", "&", "'",
						"(", ")", "*", "+", ",", "-", ".", "/", "0", "1",
						"2", "3", "4", "5", "6", "7", "8", "9", ":", ";",
						"<", "=", ">", "?", "@", "", "", "", "", "", 
						"", "", "", "", "", "", "", "", "", "", 
						"", "", "", "", "", "", "", "", "", "", 
						"", "[", "\\", "]", "^", "_", "`", "A", "B", "C", 
				/*100*/	"D", "E", "F", "G", "H", "I", "J", "K", "L", "M", 
						"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", 
						"X", "Y", "Z", "", "", "", "", "Delete", "", "", 
						"", "", "", "", "", "", "", "", "", "", 
						"", "", "", "", "", "", "", "", "", "", 
						"", "", "", "", "", "", "", "", "", "", 
						"World 0", "World 1", "World 2", "World 3", "World 4", "World 5", "World 6", "World 7", "World 8", "World 9", 
						"World 10", "World 11", "World 12", "World 13", "World 14", "World 15", "World 16", "World 17", "World 18", "World 19", 
						"World 20", "World 21", "World 22", "World 23", "World 24", "World 25", "World 26", "World 27", "World 28", "World 29", 
						"World 30", "World 31", "World 32", "World 33", "World 34", "World 35", "World 36", "World 37", "World 38", "World 39", 
				/*200*/	"World 40", "World 41", "World 42", "World 43", "World 44", "World 45", "World 46", "World 47", "World 48", "World 49", 
						"World 50", "World 51", "World 52", "World 53", "World 54", "World 55", "World 56", "World 57", "World 58", "World 59", 
						"World 60", "World 61", "World 62", "World 63", "World 64", "World 65", "World 66", "World 67", "World 68", "World 69", 
						"World 70", "World 71", "World 72", "World 73", "World 74", "World 75", "World 76", "World 67", "World 78", "World 79", 
						"World 80", "World 81", "World 82", "World 83", "World 84", "World 85", "World 86", "World 77", "World 88", "World 89", 
						"World 90", "World 91", "World 92", "World 93", "World 94", "World 95", "Keypad 0", "Keypad 1", "Keypad 2", "Keypad 3", 
						"Keypad 4", "Keypad 5", "Keypad 6", "Keypad 7", "Keypad 8", "Keypad 9", "Keypad .", "Keypad /", "Keypad *", "Keypad -", 
						"Keypad +", "Keypad Enter", "Keypad =", "Up", "Down", "Right", "Left", "Insert", "Home", "End", 
						"Page Up", "Page Down", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", 
						"F9", "F10", "F11", "F12", "F13", "F14", "F15", "", "", "", 
				/*300*/	"Num Lock", "Caps Lock", "Scroll Lock", "Right Shift", "Left Shift", "Right Control", "Left Control", "Right Alt", "Left Alt", "Right Meta", 
						"Left Meta", "Left Super", "Right Super", "Mode", "Compose", "Help", "Print", "Sys Req", "Break", "Menu", 
						"Power", "Euro", "Undo", "Mouse Up", "Mouse Down", "Mouse Left", "Mouse Right", "Mouse Button 1", "Left Mouse Button", "Center Mouse Button",
						"Right Mouse Button", "Mouse Scroll Up", "Mouse Scroll Down", "Mouse Button 4", "Mouse Button 5", "Mouse Button 6", "Mouse Button 7", "Mouse Button 8", "Mouse Button 9", "Mouse Button 10"};

#ifdef _XBOX
char * Joynames[30] = {"Left Stick Up", "Left Stick Down", "Left Stick Left", "Left Stick Right", "Right Stick Up", "Right Stick Down", "Right Stick Left", "Right Stick Right", "Pad Up", "Pad Down", 
					   "Pad Left", "Pad Right", "A Button", "B Button", "X Button", "Y Button", "Black Button", "White Button", "Left Trigger", "Right Trigger", 
					   "Start Button", "Back Button", "Left Stick Click", "Right Stick Click", "Button 1", "Button 2", "Button 3", "Button 4", "Button 5", "Button 6"};
#else
char * Joynames[30] = {"Joystick Up", "Joystick Down", "Joystick Left", "Joystick Right", "Stick 2 Up", "Stick 2 Down", "Stick 2 Left", "Stick 2 Right", "Pad Up", "Pad Down", 
                       "Pad Left", "Pad Right", "Button 1", "Button 2", "Button 3", "Button 4", "Button 5", "Button 6", "Button 7", "Button 8", 
					   "Button 9", "Button 10", "Button 11", "Button 12", "Button 13", "Button 14", "Button 15", "Button 16", "Button 17", "Button 18"};
#endif



//[Keyboard/Joystick][Game/Menu][NumPlayers][NumKeys]  left, right, jump, down, turbo, powerup, start, cancel
short controlkeys[2][2][4][NUM_KEYS] = { { { {SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_RCTRL, SDLK_RSHIFT, SDLK_RETURN, SDLK_ESCAPE},
												  {SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_e, SDLK_q, SDLK_UNKNOWN, SDLK_UNKNOWN},
												  {SDLK_g, SDLK_j, SDLK_y, SDLK_h, SDLK_u, SDLK_t, SDLK_UNKNOWN, SDLK_UNKNOWN},
												  {SDLK_l, SDLK_QUOTE, SDLK_p, SDLK_SEMICOLON, SDLK_LEFTBRACKET, SDLK_o, SDLK_UNKNOWN, SDLK_UNKNOWN} },
											
												  //up, down, left, right, select, cancel, random, fast scroll
												{ {SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN, SDLK_ESCAPE, SDLK_SPACE,  SDLK_LSHIFT},
												  {SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_e, SDLK_q, SDLK_UNKNOWN, SDLK_UNKNOWN},
												  {SDLK_y, SDLK_h, SDLK_g, SDLK_j, SDLK_u, SDLK_t, SDLK_UNKNOWN, SDLK_UNKNOWN},
												  {SDLK_p, SDLK_SEMICOLON, SDLK_l, SDLK_QUOTE, SDLK_LEFTBRACKET, SDLK_o, SDLK_UNKNOWN, SDLK_UNKNOWN} } },
#ifdef _XBOX
												  //left, right, jump, down, turbo, powerup, start, cancel
											  { { {JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, JOY_HAT_DOWN, 14, 15, 20, 21},
												  {JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, JOY_HAT_DOWN, 14, 15, 20, 21},
												  {JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, JOY_HAT_DOWN, 14, 15, 20, 21},
												  {JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, JOY_HAT_DOWN, 14, 15, 20, 21} },
											
												  //up, down, left, right, select, cancel, random, fast scroll
												{ {JOY_HAT_UP, JOY_HAT_DOWN, JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, 21, 14, 15},
												  {JOY_HAT_UP, JOY_HAT_DOWN, JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, 21, 14, 15},
												  {JOY_HAT_UP, JOY_HAT_DOWN, JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, 21, 14, 15},
												  {JOY_HAT_UP, JOY_HAT_DOWN, JOY_HAT_LEFT, JOY_HAT_RIGHT, 12, 21, 14, 15} } } };
#else
												  //left, right, jump, down, turbo, powerup, start, cancel;
											  { { {JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_STICK_1_DOWN, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3, JOY_BUTTON_START + 4},
												  {JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_STICK_1_DOWN, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3, JOY_BUTTON_START + 4},
												  {JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_STICK_1_DOWN, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3, JOY_BUTTON_START + 4},
												  {JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_STICK_1_DOWN, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3, JOY_BUTTON_START + 4} },
											
												  //up, down, left, right, select, cancel, random, fast scroll
												{ {JOY_STICK_1_UP, JOY_STICK_1_DOWN, JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3},
												  {JOY_STICK_1_UP, JOY_STICK_1_DOWN, JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3},
												  {JOY_STICK_1_UP, JOY_STICK_1_DOWN, JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3},
												  {JOY_STICK_1_UP, JOY_STICK_1_DOWN, JOY_STICK_1_LEFT, JOY_STICK_1_RIGHT, JOY_BUTTON_START, JOY_BUTTON_START + 1, JOY_BUTTON_START + 2, JOY_BUTTON_START + 3} } } };
#endif

char * GameInputNames[NUM_KEYS] = {"Left", "Right", "Jump", "Down", "Turbo", "Use Item", "Pause", "Exit"};
char * MenuInputNames[NUM_KEYS] = {"Up", "Down", "Left", "Right", "Select", "Cancel", "Random", "Fast Map"};

//Poison, 1up, 2up, 3up, 5up, firepower, star, clock, bobomb, POW, Bullet Bill, hammer, Green Shell, Red Shell, Spiny Shell, Buzzy Shell, MOd, Feather
short g_iDefaultPowerupWeights[] = {5, 10, 4, 2, 1, 10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6};

//Conversion to switch 1.6 tiles into 1.7 tiles
short g_iTileConversion[] =    {0,1,2,3,4,5,6,7,8,9,
								10,11,12,13,14,15,575,670,702,703,
								32,33,34,35,36,37,38,39,40,41,
								42,43,44,45,46,47,332,331,330,637,
								64,65,66,67,68,69,70,71,72,73,
								74,75,76,77,78,79,365,299,366,853,
								537,595,505,658,659,656,657,774,775,776,
								540,96,97,98,643,644,645,26,27,506,
								122,123,124,690,691,688,689,745,746,747,
								569,128,129,130,704,677,907,90,91,572,
								931,602,539,885,728,729,730,731,186,187,
								188,160,161,162,736,192,194,30,31,898,
								737,738,739,800,760,761,762,763,218,219,
								220,864,865,509,768,224,226,62,63,930,
								769,770,771,508,598,599,600,507,601,510,
								603,896,897,192,193,260,259,193,194,541,
								627,699,697,940,941,942,860,861,862,250,
								252,543,158,498,499,500,922,924,854,886,
								605,125,126,127,720,721,752,754,753,722,
								723,928,929,563,531,532,923,571,882,851,
								309,310,311,343,278,341,99,100,101,489,
								490,491,384,385,386,147,113,148,914,664,
								373,374,375,376,310,377,131,132,133,553,
								554,555,416,417,418,179,145,180,946,570,
								867,868,869,213,214,215,163,164,165,566,
								567,568,448,449,450,863,530,504,892,883,
								899,900,901,245,246,247,777,778,779,250,
								251,252,856,857,858,859,562,712,893,905,
								908,909,910,624,625,626,592,593,594,282,
								283,284,888,889,890,891,710,711,894,937};

/*
short g_iNewTileConversion[] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,
60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,
100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,
140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,
200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,
220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,
260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,
280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,
300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,
320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,
340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,
360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,
380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,
400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,
420,421,422,423,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,439,
440,441,442,443,444,445,446,447,448,449,450,451,452,453,454,455,456,457,458,459,
460,461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,
480,481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,
500,501,502,503,504,505,506,507,508,509,510,511,512,513,514,515,516,517,518,519,
520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,
540,541,542,543,544,545,546,547,548,549,550,551,552,553,554,555,556,557,558,559,
560,561,562,563,851,852,566,567,568,569,570,571,572,573,574,575,576,577,578,579,
580,581,582,583,584,585,586,587,588,589,590,591,592,593,594,595,564,565,598,599,
600,601,602,603,604,605,606,607,608,609,610,611,612,613,614,615,616,617,618,619,
620,621,622,623,624,625,626,627,596,597,630,631,632,633,634,635,636,637,638,639,
640,641,642,643,644,645,646,647,648,649,650,651,652,653,654,655,656,657,658,659,
660,661,662,663,664,665,666,667,668,669,670,671,672,673,674,675,676,677,678,679,
680,681,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,697,698,699,
700,701,702,703,704,705,706,707,708,709,710,711,712,713,714,715,716,717,718,719,
720,721,722,723,724,725,726,727,728,729,730,731,732,733,734,735,736,737,738,739,
740,741,742,743,744,745,746,747,749,750,751,751,752,753,754,755,756,757,758,759,
760,761,762,763,764,765,766,767,768,769,770,771,772,773,774,775,776,777,778,779,
781,782,783,783,784,785,786,787,788,789,790,791,792,793,794,795,918,919,920,921,
832,800,834,867,868,869,806,807,808,809,810,811,813,814,815,815,816,817,818,819,
820,821,822,823,824,825,826,827,950,951,952,953,864,865,866,899,900,901,838,839,
840,841,842,843,853,854,855,879,848,849,850,851,852,853,854,855,856,857,858,859,
860,861,862,863,896,897,898,908,909,910,870,871,872,873,874,875,885,886,887,911,
880,881,882,883,884,885,886,887,888,889,890,891,892,893,894,895,928,929,930,931,
932,901,902,903,904,905,906,907,940,941,942,943,912,913,914,915,916,917,918,919,
922,923,924,925,926,927,863,927,844,846,876,878,845,877,934,935,936,937,938,939,
940,941,942,943,944,945,946,947,948,949,950,951,954,955,956,957,958,959,958,959};
*/


void GetNameFromFileName(char * szName, const char * szFileName)
{
#ifdef _XBOX
	const char * p = strrchr(szFileName,'\\');
#else
	const char * p = strrchr(szFileName,'/');
#endif

	if (!p) 
		p = szFileName;
	else 
		p++;

	strcpy(szName, p);
	
	char * pLastPeriod = strrchr(szName, '.');

	if(pLastPeriod)
		*pLastPeriod = 0;
}

std::string stripCreatorAndDotMap(const std::string &filename)
{
	unsigned int firstUnderscore = filename.find("_");	//find first _
	if(firstUnderscore == std::string::npos)	//if not found start with first character
		firstUnderscore = 0;
	else
		firstUnderscore++;						//we don't want the _

	std::string withoutPrefix = filename.substr(firstUnderscore);	//substring without bla_ and .map (length-4)
	withoutPrefix = withoutPrefix.substr(0, withoutPrefix.length()-4);		//i have no idea why this doesn't work if i do it like this: (leaves .map if the map starts with an underscore)
	//																return filename.substr(firstUnderscore, filename.length()-4);

	//Capitalize the first letter so the hash table sorting works correctly
	if(withoutPrefix[0] >= 97 && withoutPrefix[0] <= 122)
		withoutPrefix[0] -= 32;

	return withoutPrefix;
}

short iScoreboardPlayerOffsetsX[3][3] = {{40, 0, 0}, {19, 59, 0}, {6, 40, 74}};
short iKingOfTheHillZoneLimits[4][4] = {{0, 0, 1, 2}, {0, 1, 2, 4}, {0, 2, 4, 7}, {0, 2, 5, 12}};

Uint8 GetScreenBackgroundFade()
{
	#ifdef _XBOX
		return 96; //TV's seem to need extra shade
	#else
		return 72;
	#endif
}

char * g_szBackgroundConversion[26] = {"Land_Classic.png",
									   "Castle_Dungeon.png",
									   "Desert_Pyramids.png",
									   "Ghost_GhostHouse.png",
									   "Underground_Cave.png",
									   "Clouds_AboveTheClouds.png",
									   "Castle_GoombaHall.png",
									   "Platforms_GreenSpottedHills.png",
									   "Snow_SnowTrees.png",
									   "Desert_Desert.png",
									   "Underground_BrownRockWall.png",
									   "Land_CastleWall.png",
									   "Clouds_Clouds.png",
									   "Land_GreenMountains.png",
									   "Land_InTheTrees.png",
									   "Battle_Manor.png",
									   "Platforms_JaggedGreenStones.png",
									   "Underground_RockWallAndPlants.png",
									   "Underground_DarkPipes.png",
									   "Bonus_StarryNight.png",
									   "Platforms_CloudsAndWaterfalls.png",
									   "Battle_GoombaPillars.png",
									   "Bonus_HillsAtNight.png",
									   "Castle_CastlePillars.png",
									   "Land_GreenHillsAndClouds.png",
									   "Platforms_BlueSpottedHills.png"};

//Conversion from backgrounds to music categories created by NMcCoy
short g_iMusicCategoryConversion[26] = {0, 3, 8, 5, 1, 9, 3, 4, 10, 8, 1, 0, 9, 0, 0, 7, 4, 1, 1, 6, 4, 7, 6, 3, 0, 4};
char * g_szMusicCategoryNames[MAXMUSICCATEGORY] = {"Land", "Underground", "Underwater", "Castle", "Platforms", "Ghost", "Bonus", "Battle", "Desert", "Clouds", "Snow"};
short g_iDefaultMusicCategory[MAXMUSICCATEGORY] = {0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0};

char * g_szAutoFilterNames[NUM_AUTO_FILTERS] = {"Hazards", "Warps", "Ice", "Item Boxes", "Breakable Blocks", "Throwable Blocks", "On/Off Blocks", "Platforms"};
short g_iAutoFilterIcons[NUM_AUTO_FILTERS] = {37, 29, 33, 1, 0, 6, 40, 73};

short g_iPowerupToIcon[6] = {80, 176, 272, 304, 336, 368};

