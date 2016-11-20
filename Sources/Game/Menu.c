
#include "Common.h"
#include "GameDefines.h"
#include "GameVariables.h"
#include "FastMath.h"
#include "Vector.h"
#include "GameConfig.h"
#include "Timer.h"
#include "Sound.h"
#include "TextureManager.h"
#include "Font.h"
#include "Sprites.h"
#include "GUIControls.h"
#include "ModelManager.h"
#include "Game.h"
#include "Menu.h"

#define MENU_TIME_FADE_IN 0.12f
#define MENU_TIME_FADE_OUT 0.25f


void Menu_Init()
{
	//
}

void Menu_Process()
{
	//int event_id;
	//int new_event = GUI_ProcessEvents(&event_id, delta_t);
	
	if (game_stage == GAME_STAGE_GAME)
	{
		GUI_SetActiveGroup(GUI_GROUP_GAME, GUI_SUBGROUP_DEFAULT);
				
		/*if (GUI_ControlIsPressed(game_exit))
		{
			GUI_MakeFadeWithEvent(MENU_EVENT_GAME_TO_MAIN, MENU_TIME_FADE_IN, MENU_TIME_FADE_OUT);
		}

		if (new_event)
		{
			if (event_id == MENU_EVENT_GAME_TO_MAIN)
			{
				Game_Exit();
			}
		}*/
	}
	else if (game_stage == GAME_STAGE_MENU_MAIN)
	{
		GUI_SetActiveGroup(GUI_GROUP_MENU_MAIN, GUI_SUBGROUP_DEFAULT);
		
		/*if (GUI_ControlIsPressed(menu_play))
		{
			GUI_MakeFadeWithEvent(MENU_EVENT_MAIN_TO_GAME, MENU_TIME_FADE_IN, MENU_TIME_FADE_OUT);
		}

		if (new_event)
		{
			if (event_id == MENU_EVENT_MAIN_TO_GAME)
			{
				Game_NewGame();
			}
		}*/
	}
}


void Menu_Render()
{
	if (game_stage == GAME_STAGE_GAME)
	{
		GUI_SetActiveGroup(GUI_GROUP_GAME, GUI_SUBGROUP_DEFAULT);

		//
	}
	else if (game_stage == GAME_STAGE_MENU_MAIN)
	{
		GUI_SetActiveGroup(GUI_GROUP_MENU_MAIN, GUI_SUBGROUP_DEFAULT);

		//
	}

	// Debug info
	/*char screen_size_str[256];
	switch (screen_size)
	{
		case SCREEN_SIZE_SMALL:
			strcpy(screen_size_str, "small");
			break;
		case SCREEN_SIZE_NORMAL:
			strcpy(screen_size_str, "normal");
			break;
		case SCREEN_SIZE_LARGE:
			strcpy(screen_size_str, "large");
			break;
		case SCREEN_SIZE_XLARGE:
			strcpy(screen_size_str, "xlarge");
			break;
	}
		
	sprintf(str, "%d x %d, scale: %.1f, size: %s", v_sx, v_sy, pixel_scale, screen_size_str);
	Font_PrintText(UNISCALE(6.0f), UNISCALE(6.0f), 0.5f, 0xffffffff, str, TEXT_ALIGN_LEFT, "tronique");*/
}


void Menu_Release()
{
	//
}

