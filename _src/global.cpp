#include "global.h"
#include <math.h>

#include "gfx.h"
extern bool g_fLoadMessages;

short g_iVersion[] = {1, 8, 0, 0};

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

//Poison == 0
//1up == 1
//2up == 2
//3up == 3
//5up == 4
//firepower == 5
//star == 6
//clock == 7
//bobomb == 8
//POW == 9
//Bullet Bill == 10
//hammer == 11
//Green Shell == 12
//Red Shell == 13
//Spiny Shell == 14
//Buzzy Shell == 15
//MOd == 16
//Feather == 17
//Mystery == 18
//Boomerang == 19
//Tanooki == 20
//Ice Wand == 21
//Podobo == 22
//Bombs == 23
//Leaf == 24
//Pwings == 25

short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS] = {  
//   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
	{5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 1
	{5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 2
	{5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 3
	{5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 4
	{5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 5
	{5,10, 5, 3, 1,10, 2, 3, 4, 3, 3, 4, 9, 6, 2, 4, 4, 7, 5, 6, 6, 3, 2, 2, 5, 5}, //Balanced
	{5, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 7, 9, 6, 3, 4, 0, 0, 0, 4, 0, 2, 0, 2, 0, 0}, //Weapons Only
	{0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0}, //Koopa Bros Weapons
	{5,10, 7, 5, 2, 0, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 5, 0, 3, 0, 0, 0, 8, 6}, //Support Items
	{3, 3, 1, 0, 0, 0, 0, 0, 4, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 3, 3, 0, 0}, //Booms and Shakes
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 2, 0, 0, 0, 8, 3}, //Fly and Glide
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //Shells
	{5, 8, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0}, //Mushrooms Only
	{3, 5, 0, 0, 0, 5, 2, 0, 0, 0, 3, 0, 6, 4, 1, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, //Super Mario Bros 1
	{0, 5, 0, 0, 0, 0, 2, 4, 3, 2, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, //Super Mario Bros 2
	{0, 3, 0, 0, 0, 8, 4, 0, 0, 0, 5, 2,10, 9, 4, 5, 0, 0, 0, 4, 3, 0, 4, 0, 8, 5}, //Super Mario Bros 3
	{0,10, 0, 0, 0,10, 6, 0, 0, 0, 2, 0, 8, 4, 2, 4, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0}, //Super Mario World
};

//Koopa Bros Weapons
//Mushrooms Only
//Super Mario Bros (All mushrooms ((poison was in lost levels, that counts as SMB1)), fireflower, star, and maybe the shells)
//Super Mario Bros 2 (All mushrooms but poison, star, clock, pow, green/red shell, bombs)
//One I'd like to see is SMWorld (esc{sp}) only stuff...
//0,10,0,1,0,10,6,0,0,0,2,0,8,4,2,4,0,10,0,5,2,0,0

short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];


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

void _load_drawmsg(const std::string& f)
{
	if(g_fLoadMessages)
	{
		/*
		static SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = 500;
		r.h = (Uint16)menu_font_small.getHeight();
		Uint32 col = SDL_MapRGB(screen->format, 189, 251, 255);
		SDL_FillRect(screen, &r, col);		//fill empty area
		*/

		menu_font_small.draw(0, 0, f.c_str());
	}
}

void _load_waitforkey()
{
	SDL_Event event;
	while (true)
	{
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_KEYDOWN)
				return;
			if(event.type == SDL_JOYBUTTONDOWN)
				return;
		}

		SDL_Delay(10);
	}
}

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

char * g_szWorldMusicCategoryNames[MAXWORLDMUSICCATEGORY] = {"Grass", "Desert", "Water", "Giant", "Sky", "Ice", "Pipe", "Dark", "Space"};

char * g_szAutoFilterNames[NUM_AUTO_FILTERS] = {"Death Tiles", "Warps", "Ice", "Item Boxes", "Breakable Blocks", "Throwable Blocks", "On/Off Blocks", "Platforms", "Hazards", "Item Destroyable Blocks", "Hidden Blocks"};
short g_iAutoFilterIcons[NUM_AUTO_FILTERS] = {37, 29, 33, 1, 0, 6, 40, 73, 19, 87, 17};

short g_iPowerupToIcon[8] = {80, 176, 272, 304, 336, 368, 384, 400};

short ReadTourStopSetting(short * iSetting, bool * fSetting, short iDefault, bool fDefault)
{
	char * pszTemp = strtok(NULL, ",\n");
	if(pszTemp)
	{
		if(iSetting)
			*iSetting = atoi(pszTemp);

		if(fSetting)
			*fSetting = atoi(pszTemp) == 1;

		return 1;
	}
	else
	{
		if(iSetting)
			*iSetting = iDefault;

		if(fSetting)
			*fSetting = fDefault;
	}

	return 0;
}

TourStop * ParseTourStopLine(char * buffer, short iVersion[4], bool fIsWorld)
{
	TourStop * ts = new TourStop();
	ts->fUseSettings = false;
	ts->iNumUsedSettings = 0;

	char * pszTemp = strtok(buffer, ",\n");

	short iStageType = 0;
	if(fIsWorld)
	{
		iStageType = atoi(pszTemp);
		if(iStageType < 0 || iStageType > 1)
			iStageType = 0;

		pszTemp = strtok(NULL, ",\n");
	}

	ts->iStageType = iStageType;
	
	if(iStageType == 0)
	{
		//Using the maplist to cheat and find a map for us
		maplist.SaveCurrent();

		//If that map is not found
		if(!maplist.findexact(pszTemp))
			maplist.random(false);
		
		ts->pszMapFile = maplist.currentShortmapname();
		maplist.ResumeCurrent();
		
		pszTemp = strtok(NULL, ",\n");

		if(pszTemp)
			ts->iMode = atoi(pszTemp);
		else
			ts->iMode = -1;

		if(ts->iMode < 0 || (ts->iMode >= GAMEMODE_LAST && ts->iMode != game_mode_pipe_minigame))
			ts->iMode = rand() % GAMEMODE_LAST;

		pszTemp = strtok(NULL, ",\n");
		
		//This gets the closest game mode to what the tour has
		ts->iGoal = -1;
		if(pszTemp)
		{
			//If it is commented out, this will allow things like 33 coins, 17 kill goals, etc.
			//ts->iGoal = gamemodes[ts->iMode]->GetClosestGoal(atoi(pszTemp));
			ts->iGoal = atoi(pszTemp);
		}
		
		//Default to a random goal if an invalid goal was used
		if(ts->iGoal <= 0)
		{
			if(ts->iMode < GAMEMODE_LAST)
				ts->iGoal = gamemodes[ts->iMode]->GetOptions()[rand() % (GAMEMODE_NUM_OPTIONS - 1)].iValue;
			else
				ts->iGoal = 50;
		}

		if(iVersion[0] == 1 && ((iVersion[1] == 7 && iVersion[2] == 0 && iVersion[3] > 1) || iVersion[1] > 7))
		{
			pszTemp = strtok(NULL, ",\n");

			//Read in point value for tour stop
			if(pszTemp)
				ts->iPoints = atoi(pszTemp);
			else
				ts->iPoints = 1;

			pszTemp = strtok(NULL, ",\n");

			if(fIsWorld)
			{
				ts->iBonusType = 0;
				ts->iNumBonuses = 0;

				char * pszStart = pszTemp;

				while(pszStart != NULL)
				{
					char * pszEnd = strstr(pszStart, "|");
					if(pszEnd)
						*pszEnd = 0;

					//if it is "0", then no bonuses
					short iWinnerPlace = pszStart[0] - 48;
					if(iWinnerPlace == 0)
						break;
					else if(iWinnerPlace < 1 || iWinnerPlace > 4)
						iWinnerPlace = 1;

					strcpy(ts->wsbBonuses[ts->iNumBonuses].szBonusString, pszStart);
					
					ts->wsbBonuses[ts->iNumBonuses].iWinnerPlace = iWinnerPlace - 1;
					
					short iPowerupOffset = 0;
					if(pszStart[1] == 'w' || pszStart[1] == 'W')
						iPowerupOffset += NUM_POWERUPS;

					pszStart += 2;

					short iBonus = atoi(pszStart) + iPowerupOffset;
					if(iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS)
						iBonus = 0;

					ts->wsbBonuses[ts->iNumBonuses].iBonus = iBonus;

					if(++ts->iNumBonuses >= 10)
						break;

					if(pszEnd)
						pszStart = pszEnd + 1;
					else
						pszStart = NULL;
				}
			}
			else
			{
				if(pszTemp)
					ts->iBonusType = atoi(pszTemp);
				else
					ts->iBonusType = 0;
			}

			pszTemp = strtok(NULL, ",\n");

			if(pszTemp)
			{
				strncpy(ts->szName, pszTemp, 127);
				ts->szName[127] = 0;
			}
			else
			{
				sprintf(ts->szName, "Tour Stop %d", game_values.tourstoptotal + 1);
			}
		}
		else
		{
			ts->iPoints = 1;
			ts->iBonusType = 0;
			sprintf(ts->szName, "Tour Stop %d", game_values.tourstoptotal + 1);
		}

		if(iVersion[0] == 1 && iVersion[1] >= 8)
		{
			if(fIsWorld)
			{
				//is this a world ending stage?
				pszTemp = strtok(NULL, ",\n");

				if(pszTemp)
					ts->fEndStage = pszTemp[0] == '1';
				else
					ts->fEndStage = false;
			}

			if(ts->iMode == 0) //classic
			{
				ts->fUseSettings = true;
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.classic.style, NULL, game_values.gamemodemenusettings.classic.style, false);
			}
			else if(ts->iMode == 1) //frag
			{
				ts->fUseSettings = true;
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.frag.style, NULL, game_values.gamemodemenusettings.frag.style, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.frag.scoring, NULL, game_values.gamemodemenusettings.frag.scoring, false);
			}
			else if(ts->iMode == 2) //time
			{
				ts->fUseSettings = true;
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.time.style, NULL, game_values.gamemodemenusettings.time.style, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.time.scoring, NULL, game_values.gamemodemenusettings.time.scoring, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.time.percentextratime, NULL, game_values.gamemodemenusettings.time.percentextratime, false);
			}
			else if(ts->iMode == 3) //jail
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.jail.style, NULL, game_values.gamemodemenusettings.jail.style, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.jail.timetofree, NULL, game_values.gamemodemenusettings.jail.timetofree, false);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.jail.tagfree, 0, game_values.gamemodemenusettings.jail.tagfree);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.jail.percentkey, NULL, game_values.gamemodemenusettings.jail.percentkey, false);
			}
			else if(ts->iMode == 4) //coins
			{
				ts->fUseSettings = true;

				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.coins.penalty, 0, game_values.gamemodemenusettings.coins.penalty);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.coins.quantity, NULL, game_values.gamemodemenusettings.coins.quantity, false);
			}
			else if(ts->iMode == 5) //stomp
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.stomp.rate, NULL, game_values.gamemodemenusettings.stomp.rate, false);

				for(int iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++)
					ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.stomp.enemyweight[iEnemy]), NULL, game_values.gamemodemenusettings.stomp.enemyweight[iEnemy], false);
			}
			else if(ts->iMode == 6) //egg
			{
				ts->fUseSettings = true;
				
				for(int iEgg = 0; iEgg < 4; iEgg++)
					ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.egg.eggs[iEgg]), NULL, game_values.gamemodemenusettings.egg.eggs[iEgg], false);

				for(int iYoshi = 0; iYoshi < 4; iYoshi++)
					ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.egg.yoshis[iYoshi]), NULL, game_values.gamemodemenusettings.egg.yoshis[iYoshi], false);

				ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.egg.explode), NULL, game_values.gamemodemenusettings.egg.explode, false);
			}
			else if(ts->iMode == 7) //capture the flag
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.flag.speed, NULL, game_values.gamemodemenusettings.flag.speed, false);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.flag.touchreturn, 0, game_values.gamemodemenusettings.flag.touchreturn);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.flag.pointmove, 0, game_values.gamemodemenusettings.flag.pointmove);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.flag.autoreturn, NULL, game_values.gamemodemenusettings.flag.autoreturn, false);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.flag.homescore, 0, game_values.gamemodemenusettings.flag.homescore);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.flag.centerflag, 0, game_values.gamemodemenusettings.flag.centerflag);
			}
			else if(ts->iMode == 8) //chicken
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.chicken.usetarget, 0, game_values.gamemodemenusettings.chicken.usetarget);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.chicken.glide, 0, game_values.gamemodemenusettings.chicken.glide);
			}
			else if(ts->iMode == 9) //tag
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.tag.tagontouch, 0, game_values.gamemodemenusettings.tag.tagontouch);
			}
			else if(ts->iMode == 10) //star
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.star.time, NULL, game_values.gamemodemenusettings.star.time, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.star.shine, NULL, game_values.gamemodemenusettings.star.shine, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.star.percentextratime, NULL, game_values.gamemodemenusettings.star.percentextratime, false);
			}
			else if(ts->iMode == 11) //domination
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.domination.quantity, NULL, game_values.gamemodemenusettings.domination.quantity, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.domination.relocationfrequency, NULL, game_values.gamemodemenusettings.domination.relocationfrequency, false);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.domination.loseondeath, 0, game_values.gamemodemenusettings.domination.loseondeath);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.domination.relocateondeath, 0, game_values.gamemodemenusettings.domination.relocateondeath);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.domination.stealondeath, 0, game_values.gamemodemenusettings.domination.stealondeath);
			}
			else if(ts->iMode == 12) //king of the hill
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.kingofthehill.areasize, NULL, game_values.gamemodemenusettings.kingofthehill.areasize, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.kingofthehill.relocationfrequency, NULL, game_values.gamemodemenusettings.kingofthehill.relocationfrequency, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.kingofthehill.maxmultiplier, NULL, game_values.gamemodemenusettings.kingofthehill.maxmultiplier, false);
			}
			else if(ts->iMode == 13) //race
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.race.quantity, NULL, game_values.gamemodemenusettings.race.quantity, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.race.speed, NULL, game_values.gamemodemenusettings.race.speed, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.race.penalty, NULL, game_values.gamemodemenusettings.race.penalty, false);
			}
			else if(ts->iMode == 15) //frenzy
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.frenzy.quantity, NULL, game_values.gamemodemenusettings.frenzy.quantity, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.frenzy.rate, NULL, game_values.gamemodemenusettings.frenzy.rate, false);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.frenzy.storedshells, 0, game_values.gamemodemenusettings.frenzy.storedshells);

				for(short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
					ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.frenzy.powerupweight[iPowerup]), NULL, game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup], false);
			}
			else if(ts->iMode == 16) //survival
			{
				ts->fUseSettings = true;
				
				for(short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
					ts->iNumUsedSettings += ReadTourStopSetting(&(ts->gmsSettings.survival.enemyweight[iEnemy]), NULL, game_values.gamemodemenusettings.survival.enemyweight[iEnemy], false);

				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.survival.density, NULL, game_values.gamemodemenusettings.survival.density, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.survival.speed, NULL, game_values.gamemodemenusettings.survival.speed, false);
				ts->iNumUsedSettings += ReadTourStopSetting(NULL, &ts->gmsSettings.survival.shield, 0, game_values.gamemodemenusettings.survival.shield);
			}
			else if(ts->iMode == 17) //greed
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.greed.coinlife, NULL, game_values.gamemodemenusettings.greed.coinlife, false);
			}
			else if(ts->iMode == 18) //health
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.health.startlife, NULL, game_values.gamemodemenusettings.health.startlife, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.health.maxlife, NULL, game_values.gamemodemenusettings.health.maxlife, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.health.percentextralife, NULL, game_values.gamemodemenusettings.health.percentextralife, false);
			}
			else if(ts->iMode == 19) //card collection
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.collection.quantity, NULL, game_values.gamemodemenusettings.collection.quantity, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.collection.rate, NULL, game_values.gamemodemenusettings.collection.rate, false);
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.collection.banktime, NULL, game_values.gamemodemenusettings.collection.banktime, false);
			}
			else if(ts->iMode == 20) //chase (phanto)
			{
				ts->fUseSettings = true;
				
				ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.chase.phantospeed, NULL, game_values.gamemodemenusettings.chase.phantospeed, false);

				for(short iPhanto = 0; iPhanto < 3; iPhanto++)
					ts->iNumUsedSettings += ReadTourStopSetting(&ts->gmsSettings.chase.phantoquantity[iPhanto], NULL, game_values.gamemodemenusettings.chase.phantoquantity[iPhanto], false);
			}
		}
	}
	else if(iStageType == 1) //Bonus House
	{
		if(pszTemp)
		{
			strncpy(ts->szName, pszTemp, 127);
			ts->szName[127] = 0;
		}
		else
		{
			sprintf(ts->szName, "Bonus House %d", game_values.tourstoptotal + 1);
		}

		pszTemp = strtok(NULL, ",\n");

		short iBonusOrdering = atoi(pszTemp);
		if(iBonusOrdering < 0 || iBonusOrdering > 1)
			iBonusOrdering = 0;

		ts->iBonusType = iBonusOrdering;

		pszTemp = strtok(NULL, ",\n");

		char * pszStart = pszTemp;

		ts->iBonusTextLines = 0;
		while(pszStart != NULL && pszStart[0] != '-')
		{
			char * pszEnd = strstr(pszStart, "|");
			
			if(pszEnd)
				*pszEnd = 0;

			strcpy(ts->szBonusText[ts->iBonusTextLines], pszStart);

			if(++ts->iBonusTextLines >= 5 || !pszEnd)
				break;

			pszStart = pszEnd + 1;
		}

		ts->iNumBonuses = 0;
		pszTemp = strtok(NULL, ",\n");
		while(pszTemp)
		{
			strcpy(ts->wsbBonuses[ts->iNumBonuses].szBonusString, pszTemp);

			short iPowerupOffset = 0;
			if(pszTemp[0] == 'w' || pszTemp[0] == 'W')
				iPowerupOffset += NUM_POWERUPS;
			else if(pszTemp[0] == 's' || pszTemp[0] == 'S')
				iPowerupOffset += NUM_POWERUPS + NUM_WORLD_POWERUPS - 1;

			pszTemp++;
	
			short iBonus = atoi(pszTemp) + iPowerupOffset;
			if(iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS + NUM_WORLD_SCORE_BONUSES)
				iBonus = 0;

			ts->wsbBonuses[ts->iNumBonuses].iBonus = iBonus;
			ts->wsbBonuses[ts->iNumBonuses].iWinnerPlace = -1;

			if(++ts->iNumBonuses >= MAX_BONUS_CHESTS)
				break;

			pszTemp = strtok(NULL, ",\n");
		}
	}

	return ts;
}

void WriteTourStopLine(TourStop * ts, char * buffer, bool fIsWorld)
{
	buffer[0] = 0;
	char szTemp[32];

	if(fIsWorld)
	{
		//Write stage type (battle stage vs. bonus house, etc.)
		sprintf(szTemp, "%d,", ts->iStageType);
		strcat(buffer, szTemp);
	}

	//Battle stage
	if(ts->iStageType == 0)
	{
		strcat(buffer, ts->pszMapFile);
		strcat(buffer, ",");

		sprintf(szTemp, "%d,", ts->iMode);
		strcat(buffer, szTemp);
		
		sprintf(szTemp, "%d,", ts->iGoal);
		strcat(buffer, szTemp);

		sprintf(szTemp, "%d,", ts->iPoints);
		strcat(buffer, szTemp);

		if(fIsWorld)
		{
			if(ts->iNumBonuses == 0)
			{
				strcat(buffer, "0");
			}
			else
			{
				for(short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++)
				{
					if(iBonus > 0)
						strcat(buffer, "|");

					strcat(buffer, ts->wsbBonuses[iBonus].szBonusString);
				}
			}
			
			strcat(buffer, ",");
		}
		else
		{
			sprintf(szTemp, "%d,", ts->iBonusType);
			strcat(buffer, szTemp);
		}

		strcat(buffer, ts->szName);
		strcat(buffer, ",");

		if(fIsWorld)
		{
			sprintf(szTemp, "%d", ts->fEndStage);
			strcat(buffer, szTemp);
		}

		if(ts->fUseSettings)
		{
			if(ts->iMode == 0) //classic
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.classic.style);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 1) //frag
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.frag.style);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.frag.scoring);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 2) //time
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.time.style);
					strcat(buffer, szTemp);
				}
				
				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.time.scoring);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.time.percentextratime);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 3) //jail
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.jail.style);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.jail.timetofree);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.jail.tagfree);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 3)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.jail.percentkey);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 4) //coins
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.coins.penalty);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.coins.quantity);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 5) //stomp
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.stomp.rate);
					strcat(buffer, szTemp);
				}

				for(int iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++)
				{
					if(ts->iNumUsedSettings > iEnemy + 1)
					{
						sprintf(szTemp, ",%d", ts->gmsSettings.stomp.enemyweight[iEnemy]);
						strcat(buffer, szTemp);
					}
				}
			}
			else if(ts->iMode == 6) //egg
			{
				for(int iEgg = 0; iEgg < 4; iEgg++)
				{
					if(ts->iNumUsedSettings > iEgg)
					{
						sprintf(szTemp, ",%d", ts->gmsSettings.egg.eggs[iEgg]);
						strcat(buffer, szTemp);
					}
				}

				for(int iYoshi = 0; iYoshi < 4; iYoshi++)
				{
					if(ts->iNumUsedSettings > iYoshi + 4)
					{
						sprintf(szTemp, ",%d", ts->gmsSettings.egg.yoshis[iYoshi]);
						strcat(buffer, szTemp);
					}
				}

				if(ts->iNumUsedSettings > 8)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.egg.explode);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 7) //capture the flag
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.flag.speed);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.flag.touchreturn);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.flag.pointmove);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 3)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.flag.autoreturn);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 4)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.flag.homescore);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 5)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.flag.centerflag);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 8) //chicken
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.chicken.usetarget);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.chicken.glide);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 9) //tag
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.tag.tagontouch);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 10) //star
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.star.time);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.star.shine);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 11) //domination
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.domination.quantity);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.domination.relocationfrequency);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.domination.loseondeath);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 3)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.domination.relocateondeath);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 4)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.domination.stealondeath);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 12) //king of the hill
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.areasize);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.relocationfrequency);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.maxmultiplier);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 13) //race
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.race.quantity);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.race.speed);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.race.penalty);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 15) //frenzy
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.quantity);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.rate);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.storedshells);
					strcat(buffer, szTemp);
				}

				for(short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
				{
					if(ts->iNumUsedSettings > iPowerup + 3)
					{
						sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.powerupweight[iPowerup]);
						strcat(buffer, szTemp);
					}
				}
			}
			else if(ts->iMode == 16) //survival
			{
				for(short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
				{
					if(ts->iNumUsedSettings > iEnemy)
					{
						sprintf(szTemp, ",%d", ts->gmsSettings.survival.enemyweight[iEnemy]);
						strcat(buffer, szTemp);
					}
				}

				if(ts->iNumUsedSettings > 3)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.survival.density);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 4)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.survival.speed);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 5)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.survival.shield);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 17) //greed
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.greed.coinlife);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 18) //health
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.health.startlife);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.health.maxlife);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.health.percentextralife);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 19) //card collection
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.collection.quantity);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 1)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.collection.rate);
					strcat(buffer, szTemp);
				}

				if(ts->iNumUsedSettings > 2)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.collection.banktime);
					strcat(buffer, szTemp);
				}
			}
			else if(ts->iMode == 20) //phanto chase mode
			{
				if(ts->iNumUsedSettings > 0)
				{
					sprintf(szTemp, ",%d", ts->gmsSettings.chase.phantospeed);
					strcat(buffer, szTemp);
				}

				for(short iPhanto = 0; iPhanto < 3; iPhanto++)
				{
					if(ts->iNumUsedSettings > iPhanto + 1)
					{
						sprintf(szTemp, ",%d", ts->gmsSettings.chase.phantoquantity[iPhanto]);
						strcat(buffer, szTemp);
					}
				}
			}
		}
	}
	else if(ts->iStageType == 1) //Bonus House
	{
		strcat(buffer, ts->szName);
		strcat(buffer, ",");

		sprintf(szTemp, "%d,", ts->iBonusType);
		strcat(buffer, szTemp);

		for(short iText = 0; iText < ts->iBonusTextLines; iText++)
		{
			if(iText != 0)
				strcat(buffer, "|");

			strcat(buffer, ts->szBonusText[iText]);
		}

		for(short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++)
		{
			strcat(buffer, ",");
			strcat(buffer, ts->wsbBonuses[iBonus].szBonusString);
		}
	}

	strcat(buffer, "\n");
}

void ResetTourStops()
{
	game_values.tourstopcurrent = 0;
	game_values.tourstoptotal = 0;
	
	for(short iTourStop = 0; iTourStop < (short)game_values.tourstops.size(); iTourStop++)
	{
		delete game_values.tourstops[iTourStop];
	}

	game_values.tourstops.clear();
}

void LoadCurrentMapBackground()
{
	char filename[128];
	sprintf(filename, "gfx/packs/backgrounds/%s", g_map.szBackgroundFile);
	std::string path = convertPath(filename, gamegraphicspacklist.current_name());

	//if the background file doesn't exist, use the classic background
	if(!File_Exists(path))
		path = convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist.current_name());

	gfx_loadimagenocolorkey(&spr_background, path);
}

//TODO  - Review what is colliding with what and remove duplicates (i.e. shell vs. throwblock and throwblock vs. shell should only detect one way)
short g_iCollisionMap[MOVINGOBJECT_LAST][MOVINGOBJECT_LAST] = 
{
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_none = 0
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_powerup = 1
	{0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0}, //movingobject_fireball = 2
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_goomba = 3
	{0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,1,0}, //movingobject_bulletbill = 4
	{0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0}, //movingobject_hammer = 5 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_poisonpowerup = 6
	{0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,1,1,0,1,1,0}, //movingobject_shell = 7
	{0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,1,0,1,1,0,1,1,0}, //movingobject_throwblock = 8
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_egg = 9
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_star = 10
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_flag = 11
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_cheepcheep = 12
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_koopa = 13
	{0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0}, //movingobject_boomerang = 14
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_carried = 15
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_iceblast = 16
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_bomb = 17
	{0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_podobo = 18
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_treasurechest = 19
	{0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0}, //movingobject_attackzone = 20
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_pirhanaplant = 21
	{0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0}, //movingobject_explosion = 22
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_buzzybeetle = 23
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_spiny = 24
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_phantokey = 25
};

short iPlatformPathDotSize[3] = {12, 6, 4};
short iPlatformPathDotOffset[3] = {0, 12, 18};
short iScreenshotSize[3][2] = {{640, 480}, {320, 240}, {160, 120}};
void DrawPlatform(short pathtype, TilesetTile ** tiles, short startX, short startY, short endX, short endY, float angle, float radiusX, float radiusY, short iSize, short iPlatformWidth, short iPlatformHeight, bool fDrawPlatform, bool fDrawShadow)
{
	short iStartX = startX >> iSize;
	short iStartY = startY >> iSize;
	short iEndX = endX >> iSize;
	short iEndY = endY >> iSize;

	float fRadiusX = radiusX / (float)(1 << iSize);
	float fRadiusY = radiusY / (float)(1 << iSize);

	short iSizeShift = 5 - iSize;
	int iTileSize = 1 << iSizeShift;

	if(fDrawPlatform)
	{
		for(short iPlatformX = 0; iPlatformX < iPlatformWidth; iPlatformX++)
		{
			for(short iPlatformY = 0; iPlatformY < iPlatformHeight; iPlatformY++)
			{
				TilesetTile * tile = &tiles[iPlatformX][iPlatformY];

				int iDstX = 0;
				int iDstY = 0;

				if(pathtype == 2)
				{
					iDstX = iStartX + (iPlatformX << iSizeShift) + (short)(fRadiusX * cos(angle)) - (iPlatformWidth << (iSizeShift - 1));
					iDstY = iStartY + (iPlatformY << iSizeShift) + (short)(fRadiusY * sin(angle)) - (iPlatformHeight << (iSizeShift - 1));
				}
				else
				{
					iDstX = iStartX + (iPlatformX << iSizeShift) - (iPlatformWidth << (iSizeShift - 1));
					iDstY = iStartY + (iPlatformY << iSizeShift) - (iPlatformHeight << (iSizeShift - 1));
				}

				SDL_Rect bltrect = {iDstX, iDstY, iTileSize, iTileSize};
				if(tile->iID >= 0)
				{
					SDL_BlitSurface(g_tilesetmanager.GetTileset(tile->iID)->GetSurface(iSize), &g_tilesetmanager.rRects[iSize][tile->iCol][tile->iRow], blitdest, &bltrect);
				}
				else if(tile->iID == TILESETUNKNOWN)
				{
					//Draw unknown tile
					SDL_BlitSurface(spr_unknowntile[iSize].getSurface(), &g_tilesetmanager.rRects[iSize][0][0], blitdest, &bltrect);
				}

				bool fNeedWrap = false;
				if(iDstX + iTileSize >= iScreenshotSize[iSize][0])
				{
					iDstX -= iScreenshotSize[iSize][0];
					fNeedWrap = true;
				}
				else if(iDstX < 0)
				{
					iDstX += iScreenshotSize[iSize][0];
					fNeedWrap = true;
				}

				if(fNeedWrap)
				{
					bltrect.x = iDstX;
					bltrect.y = iDstY;
					bltrect.w = iTileSize;
					bltrect.h = iTileSize;

					if(tile->iID >= 0)
						SDL_BlitSurface(g_tilesetmanager.GetTileset(tile->iID)->GetSurface(iSize), &g_tilesetmanager.rRects[iSize][tile->iCol][tile->iRow], blitdest, &bltrect);
					else if(tile->iID == TILESETUNKNOWN)
						SDL_BlitSurface(spr_unknowntile[iSize].getSurface(), &g_tilesetmanager.rRects[iSize][0][0], blitdest, &bltrect);
				}
			}
		}
	}

	SDL_Rect rPathSrc = {iPlatformPathDotOffset[iSize], 0, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]}, rPathDst;

	if(pathtype == 0) //line segment
	{
		if(fDrawShadow)
		{
			for(short iCol = 0; iCol < iPlatformWidth; iCol++)
			{
				for(short iRow = 0; iRow < iPlatformHeight; iRow++)
				{
					if(tiles[iCol][iRow].iID != -2)
						spr_platformstarttile.draw(iStartX - (iPlatformWidth << (iSizeShift - 1)) + (iCol << iSizeShift), iStartY - (iPlatformHeight << (iSizeShift - 1)) + (iRow << iSizeShift), 0, 0, iTileSize, iTileSize);
				}
			}

			for(short iCol = 0; iCol < iPlatformWidth; iCol++)
			{
				for(short iRow = 0; iRow < iPlatformHeight; iRow++)
				{
					if(tiles[iCol][iRow].iID != -2)
						spr_platformendtile.draw(iEndX - (iPlatformWidth << (iSizeShift - 1)) + (iCol << iSizeShift), iEndY - (iPlatformHeight << (iSizeShift - 1)) + (iRow << iSizeShift), 0, 0, iTileSize, iTileSize);
				}
			}
		}

		//Draw connecting dots
		float dDiffX = (float)(iEndX - iStartX);
		float dDiffY = (float)(iEndY - iStartY);

		short iDistance = (short)sqrt(dDiffX * dDiffX + dDiffY * dDiffY);

		short iNumSpots = (iDistance >> iSizeShift);
		float dIncrementX = dDiffX / (float)iNumSpots;
		float dIncrementY = dDiffY / (float)iNumSpots;

		float dX = (float)(iStartX) - (float)(iPlatformPathDotSize[iSize] >> 1);
		float dY = (float)(iStartY) - (float)(iPlatformPathDotSize[iSize] >> 1);

		for(short iSpot = 0; iSpot < iNumSpots + 1; iSpot++)
		{
			gfx_setrect(&rPathDst, (short)dX, (short)dY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
			SDL_BlitSurface(spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);

			dX += dIncrementX;
			dY += dIncrementY;
		}
	}
	else if(pathtype == 1) //continuous straight path
	{
		if(fDrawShadow)
		{
			for(short iCol = 0; iCol < iPlatformWidth; iCol++)
			{
				for(short iRow = 0; iRow < iPlatformHeight; iRow++)
				{
					if(tiles[iCol][iRow].iID != -2)
						spr_platformstarttile.draw(iStartX - (iPlatformWidth << (iSizeShift - 1)) + (iCol << iSizeShift), iStartY - (iPlatformHeight << (iSizeShift - 1)) + (iRow << iSizeShift), 0, 0, iTileSize, iTileSize);
				}
			}
		}

		float dIncrementX = (float)iTileSize * cos(angle);
		float dIncrementY = (float)iTileSize * sin(angle);

		float dX = (float)(iStartX) - (float)(iPlatformPathDotSize[iSize] >> 1);
		float dY = (float)(iStartY) - (float)(iPlatformPathDotSize[iSize] >> 1);

		for(short iSpot = 0; iSpot < 50; iSpot++)
		{
			gfx_setrect(&rPathDst, (short)dX, (short)dY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
			SDL_BlitSurface(spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);

			short iWrapX = (short)dX;
			short iWrapY = (short)dY;
			bool fNeedWrap = false;
			if(dX + iPlatformPathDotSize[iSize] >= iScreenshotSize[iSize][0])
			{
				iWrapX = (short)(dX - iScreenshotSize[iSize][0]);
				fNeedWrap = true;
			}
			else if(dX < 0.0f)
			{
				iWrapX = (short)(dX + iScreenshotSize[iSize][0]);
				fNeedWrap = true;
			}

			if(dY + iPlatformPathDotSize[iSize] >= iScreenshotSize[iSize][1])
			{
				iWrapY = (short)(dY - iScreenshotSize[iSize][1]);
				fNeedWrap = true;
			}
			else if(dY < 0.0f)
			{
				iWrapY = (short)(dY + iScreenshotSize[iSize][1]);
				fNeedWrap = true;
			}

			if(fNeedWrap)
			{
				gfx_setrect(&rPathDst, iWrapX, iWrapY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
				SDL_BlitSurface(spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);
			}

			dX += dIncrementX;
			dY += dIncrementY;
		}
	}
	else if(pathtype == 2) //ellipse
	{
		//Calculate the starting position
		if(fDrawShadow)
		{
			short iEllipseStartX = (short)(fRadiusX * cos(angle)) - (iPlatformWidth << (iSizeShift - 1)) + iStartX;
			short iEllipseStartY = (short)(fRadiusY * sin(angle)) - (iPlatformHeight << (iSizeShift - 1)) + iStartY;

			for(short iCol = 0; iCol < iPlatformWidth; iCol++)
			{
				for(short iRow = 0; iRow < iPlatformHeight; iRow++)
				{
					if(tiles[iCol][iRow].iID != -2)
						spr_platformstarttile.draw(iEllipseStartX + (iCol << iSizeShift), iEllipseStartY + (iRow << iSizeShift), 0, 0, iTileSize, iTileSize);
				}
			}
		}

		float fAngle = angle;
		for(short iSpot = 0; iSpot < 32; iSpot++)
		{
			short iX = (short)(fRadiusX * cos(fAngle)) - (iPlatformPathDotSize[iSize] >> 1) + iStartX;
			short iY = (short)(fRadiusY * sin(fAngle)) - (iPlatformPathDotSize[iSize] >> 1) + iStartY;

			gfx_setrect(&rPathDst, iX, iY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
			SDL_BlitSurface(spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);

			if(iX + iPlatformPathDotSize[iSize] >= iScreenshotSize[iSize][0])
			{
				gfx_setrect(&rPathDst, iX - iScreenshotSize[iSize][0], iY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
				SDL_BlitSurface(spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);
			}
			else if(iX < 0)
			{
				gfx_setrect(&rPathDst, iX + iScreenshotSize[iSize][0], iY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
				SDL_BlitSurface(spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);
			}

			fAngle += TWO_PI / 32.0f;
		}
	}
}


short iFireballHazardSize[3] = {18, 9, 5};

short iStandardOffset[3] = {0, 32, 48};
float dBulletBillFrequency[3] = {10.0f, 5.0f, 2.5f};

short iPirhanaPlantOffsetY[4][3] = {{0, 0, 0}, {48, 24, 12}, {96, 48, 24}, {160, 80, 40}};
void DrawMapHazard(MapHazard * hazard, short iSize, bool fDrawCenter)
{
	short iSizeShift = 5 - iSize;
	short iTileSize = 1 << iSizeShift;

	SDL_Rect rDotSrc = {iPlatformPathDotOffset[iSize] + 22, 0, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]}, rDotDst;
	SDL_Rect rPathSrc = {iStandardOffset[iSize], 12, iTileSize, iTileSize}, rPathDst;

	gfx_setrect(&rPathDst, hazard->ix << (iSizeShift - 1), hazard->iy << (iSizeShift - 1), iTileSize, iTileSize);

	if(fDrawCenter)
	{
		if(hazard->itype <= 1)
		{
			SDL_BlitSurface(spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);
		}
	}

	if(hazard->itype == 0) //fireball string
	{
		short iNumDots = 16;
		float dRadius = (float)((hazard->iparam[0] - 1) * 24) / (float)(1 << iSize) + (iPlatformPathDotSize[iSize] >> 1);
		float dAngle = hazard->dparam[1];
		for(short iDot = 0; iDot < iNumDots; iDot++)
		{
			rDotDst.x = (short)(dRadius * cos(dAngle)) + rPathDst.x + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1);
			rDotDst.y = (short)(dRadius * sin(dAngle)) + rPathDst.y + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1);
			rDotDst.h = rDotDst.w = iPlatformPathDotSize[iSize];
			
			spr_platformpath.draw(rDotDst.x, rDotDst.y, rDotSrc.x, rDotSrc.y, rDotDst.w, rDotDst.h);
			//SDL_BlitSurface(spr_platformpath.getSurface(), &rDotSrc, blitdest, &rDotDst);

			dAngle += TWO_PI / iNumDots;
		}

		//Draw the fireball string
		for(short iFireball = 0; iFireball < hazard->iparam[0]; iFireball++)
		{
			short x = (hazard->ix << (iSizeShift - 1)) + (short)((float)(iFireball * (24 >> iSize)) * cos(hazard->dparam[1])) + (iTileSize >> 1) - (iFireballHazardSize[iSize] >> 1);
			short y = (hazard->iy << (iSizeShift - 1)) + (short)((float)(iFireball * (24 >> iSize)) * sin(hazard->dparam[1])) + (iTileSize >> 1) - (iFireballHazardSize[iSize] >> 1);

			spr_hazard_fireball[iSize].draw(x, y, 0, 0, iFireballHazardSize[iSize], iFireballHazardSize[iSize]);
		}
	}
	else if(hazard->itype == 1) //rotodisc
	{
		short iNumDots = 16;
		float dRadius = (hazard->dparam[2] + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1)) / (float)(1 << iSize);
		float dAngle = hazard->dparam[1];
		for(short iDot = 0; iDot < iNumDots; iDot++)
		{
			rDotDst.x = (short)(dRadius * cos(dAngle)) + rPathDst.x + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1);
			rDotDst.y = (short)(dRadius * sin(dAngle)) + rPathDst.y + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1);
			rDotDst.h = rDotDst.w = iPlatformPathDotSize[iSize];
			
			spr_platformpath.draw(rDotDst.x, rDotDst.y, rDotSrc.x, rDotSrc.y, rDotDst.w, rDotDst.h);
			//SDL_BlitSurface(spr_platformpath.getSurface(), &rDotSrc, blitdest, &rDotDst);

			dAngle += TWO_PI / iNumDots;
		}

		//Draw the rotodiscs
		float dSector = TWO_PI / hazard->iparam[0];
		dAngle = hazard->dparam[1];
		dRadius = hazard->dparam[2] / (float)(1 << iSize);
		for(short iRotodisc = 0; iRotodisc < hazard->iparam[0]; iRotodisc++)
		{
			short x = rPathDst.x + (short)(dRadius * cos(dAngle));
			short y = rPathDst.y + (short)(dRadius * sin(dAngle));

			spr_hazard_rotodisc[iSize].draw(x, y, 0, 0, iTileSize, iTileSize);

			dAngle += dSector;
		}
	}
	else if(hazard->itype == 2) //bullet bill
	{
		spr_hazard_bulletbill[iSize].draw(rPathDst.x, rPathDst.y, 0, hazard->dparam[0] < 0.0f ? 0 : iTileSize, iTileSize, iTileSize);

		short iBulletPathX = rPathDst.x - iPlatformPathDotSize[iSize];
		if(hazard->dparam[0] > 0.0f)
			iBulletPathX = rPathDst.x + iTileSize;

		short iBulletPathSpacing = (short)(hazard->dparam[0] * dBulletBillFrequency[iSize]);
		while(iBulletPathX >= 0 && iBulletPathX < iScreenshotSize[iSize][0])
		{
			gfx_setrect(&rDotDst, iBulletPathX, rPathDst.y + ((iTileSize - iPlatformPathDotSize[iSize]) >> 1), iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
			SDL_BlitSurface(spr_platformpath.getSurface(), &rDotSrc, blitdest, &rDotDst);

			iBulletPathX += hazard->iparam[0] < 0.0f ? -iBulletPathSpacing : iBulletPathSpacing;
		}
	}
	else if(hazard->itype == 3) //flame cannon
	{
		spr_hazard_flame[iSize].draw(rPathDst.x + (hazard->iparam[1] == 0 ? -(iTileSize << 1) : 0), rPathDst.y, hazard->iparam[1] == 0 ? iTileSize * 3 : 0, iTileSize << 1, iTileSize * 3, iTileSize);
	}
	else if(hazard->itype >= 4 && hazard->itype <= 7) //pirhana plants
	{
		spr_hazard_pirhanaplant[iSize].draw(rPathDst.x, rPathDst.y + (hazard->iparam[1] == 0 ? (hazard->itype == 6 ? -iTileSize : -(iTileSize >> 1)) : 0), hazard->iparam[1] == 0 ? 0 : (hazard->itype >= 6 ? iTileSize << 1: iTileSize << 2), iPirhanaPlantOffsetY[hazard->itype - 4][iSize], iTileSize, hazard->itype == 6 ? iTileSize << 1 : iPirhanaPlantOffsetY[1][iSize]);
	}
}

SDL_Rect iCountDownNumbers[4][4][2] = { {{{0, 0, 64, 64},{288, 208, 64, 64}},
									     {{64, 0, 48, 48},{296, 216, 48, 48}},
									     {{112, 0, 32, 32},{304, 224, 32, 32}},
									     {{144, 0, 16, 16},{312, 232, 16, 16}}},

										{{{0, 0, 64, 64},{288, 208, 64, 64}},
									     {{64, 0, 48, 48},{296, 216, 48, 48}},
									     {{112, 0, 32, 32},{304, 224, 32, 32}},
									     {{144, 0, 16, 16},{312, 232, 16, 16}}},

										{{{0, 0, 64, 64},{288, 208, 64, 64}},
									     {{64, 0, 48, 48},{296, 216, 48, 48}},
									     {{112, 0, 32, 32},{304, 224, 32, 32}},
									     {{144, 0, 16, 16},{312, 232, 16, 16}}},

										{{{0, 0, 64, 64},{288, 208, 64, 64}},
									     {{64, 0, 48, 48},{296, 216, 48, 48}},
									     {{112, 0, 32, 32},{304, 224, 32, 32}},
										 {{144, 0, 16, 16},{312, 232, 16, 16}}}};

short iCountDownTimes[28] = {3, 3, 3, 15, 3, 3, 3, 3, 3, 3, 15, 3, 3, 3, 3, 3, 3, 15, 3, 3, 3, 3, 3, 3, 45, 3, 3, 3};
short iCountDownRectSize[28] = {3, 2, 1, 0, 1, 2, 3, 3, 2, 1, 0, 1, 2, 3, 3, 2, 1, 0, 1, 2, 3, 3, 2, 1, 0, 1, 2, 3};
short iCountDownRectGroup[28] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3};
short iCountDownAnnounce[28] = {-1, -1, -1, 12, -1, -1, -1, -1, -1, -1, 13, -1, -1, -1, -1, -1, -1, 14, -1, -1, -1, -1, -1, -1, 15, -1, -1, -1};
