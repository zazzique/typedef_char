
#ifndef _GAME_H_
#define _GAME_H_

#ifndef _MSC_VER
#ifdef __cplusplus
extern "C" {
#endif
#endif

enum
{
	GAME_STAGE_MENU_MAIN,
	GAME_STAGE_GAME
};

enum
{
	GUI_GROUP_MENU_MAIN,
	GUI_GROUP_GAME,
	GUI_GROUP_GAME_MENU
};

extern int game_stage;
extern int next_game_stage;

void Game_Init();
void Game_Release();

void Game_NewGame();
void Game_Exit();

void Game_Process();
void Game_Render();

void Game_Pause();

#ifndef _MSC_VER
#ifdef __cplusplus
}
#endif
#endif

#endif /* _GAME_H_ */
