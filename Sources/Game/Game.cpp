#ifndef _MSC_VER
#ifdef __cplusplus
extern "C" {
#endif
#endif
#include "Common.h"
#include "GameDefines.h"
#include "GameVariables.h"
#include "FastMath.h"
#include "Vector.h"
#include "Timer.h"
#include "GameConfig.h"
#include "Files.h"
#include "Sound.h"
#include "Render.h"
#include "TextureManager.h"
#include "Font.h"
#include "Sprites.h"
#include "GUIControls.h"
#include "ModelManager.h"
#include "Menu.h"
#ifndef _MSC_VER
#ifdef __cplusplus
}
#endif
#endif
#include "Hero.h"
#include "TileMap.h"
#include "Camera.h"
#include "Scene.h"
#include "Game.h"

int game_stage;
int next_game_stage;

Scene *scene;
Camera *camera;
TileMap *tile_map;
Hero *hero;

int sprite_background;

int sprite_crt_l, sprite_crt_r, sprite_crt_u, sprite_crt_d, sprite_crt_lu, sprite_crt_ld, sprite_crt_ru, sprite_crt_rd;
int sprite_crt_center;
int sprite_crt_stand;

int sprite_hen_1;
int sprite_hen_2;

float crt_bound_left, crt_bound_right, crt_bound_up, crt_bound_down;
U32 screen_color;

BOOL run_second_intro;
float intro_timer;

int sound_beep;
int sound_error;
float beep_delay;

int speech_bubble_sprite;
float speech_bubble_time;
char speech_bubble_text[1024];
Vector2D speech_bubble_pos;

float outro_timer;

Vector2D hen_pos;
int sound_hen;
int sound_hen_eat;

void Set_SpeechBubble(float x, float y, float time, char *text)
{
	speech_bubble_pos.x = x;
	speech_bubble_pos.y = y;
	strcpy(speech_bubble_text, text);

	speech_bubble_time = time;
}

void Draw_SpeechBubble()
{
	if (speech_bubble_time > 0)
	{
		speech_bubble_time -= delta_t;

		U32 flags = SPRITE_CENTERED;
		if (speech_bubble_pos.x + camera->pos.x < 0.0f)
			flags |= SPRITE_FLIP_X;
		Sprites_DrawSprite(speech_bubble_sprite, speech_bubble_pos.x + camera->pos.x, speech_bubble_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, flags);
		Font_PrintText(speech_bubble_pos.x + camera->pos.x, speech_bubble_pos.y + camera->pos.y + 8.0f, 1.0f, 0xffffffff, speech_bubble_text, TEXT_ALIGN_CENTER | TEXT_VERTICAL_CENTERED, "tronique");
	}
}

void Draw_CRT()
{

	Sprites_DrawSpriteEx(sprite_crt_l, crt_bound_left - 24.0f + camera->pos.x, (crt_bound_up + crt_bound_down) * 0.5f + camera->pos.y, 1.0f, (crt_bound_up - crt_bound_down) / 48.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);
	Sprites_DrawSpriteEx(sprite_crt_r, crt_bound_right + 24.0f, (crt_bound_up + crt_bound_down) * 0.5f + camera->pos.y, 1.0f, (crt_bound_up - crt_bound_down) / 48.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);
	Sprites_DrawSpriteEx(sprite_crt_u, (crt_bound_right + crt_bound_left) * 0.5f, crt_bound_up + 24.0f + camera->pos.y, (crt_bound_right - crt_bound_left) / 48.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);
	Sprites_DrawSpriteEx(sprite_crt_d, (crt_bound_right + crt_bound_left) * 0.5f, crt_bound_down - 24.0f + camera->pos.y, (crt_bound_right - crt_bound_left) / 48.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);

	Sprites_DrawSprite(sprite_crt_lu, crt_bound_left - 24.0f + camera->pos.x, crt_bound_up + 24.0f + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);
	Sprites_DrawSprite(sprite_crt_ld, crt_bound_left - 24.0f + camera->pos.x, crt_bound_down - 24.0f + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);
	Sprites_DrawSprite(sprite_crt_ru, crt_bound_right + 24.0f + camera->pos.x, crt_bound_up + 24.0f + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);
	Sprites_DrawSprite(sprite_crt_rd, crt_bound_right + 24.0f + camera->pos.x, crt_bound_down - 24.0f + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);

	Sprites_DrawSpriteEx(sprite_crt_center, (crt_bound_right + crt_bound_left) * 0.5f + camera->pos.x, (crt_bound_up + crt_bound_down) * 0.5f + camera->pos.y, (crt_bound_right - crt_bound_left) / 48.0f, (crt_bound_up - crt_bound_down) / 48.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, screen_color, SPRITE_CENTERED);
	Sprites_DrawSprite(sprite_crt_stand, 0.0f + camera->pos.x, crt_bound_down - 71.0f + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);
}

void Game_NewGame()
{
	paused = FALSE;
	level_time = 0;
	next_game_stage = GAME_STAGE_GAME;
	
	scene->SetCamera(camera);

	// TODO: add some lines
	crt_bound_left = -160.0f;
	crt_bound_right = 160.0f;
	crt_bound_up = 120.0f;
	crt_bound_down = -120.0f;

	screen_color = 0xff000000;

	tile_map->Load("map.cfg");
	tile_map->scale = 0.5f;

	hero->pos.x = -1000000.0f;
	hero->pos.y = -1000000.0f;

	tile_map->pos.x = crt_bound_left;
	tile_map->pos.y = crt_bound_up;

	FileHandler map_file;

	if (!Files_OpenFile(&map_file, "map.txt"))
		return;

	char *content = NULL;
	I32 content_size;

	Files_GetData(&map_file, (void **)&content, &content_size);

	tile_map->StartSpecialIntro(content, 2, 2, debug_mode ? 0.0f : 4.0f, 0.0f, 0.0f, &hero->pos);
	run_second_intro = TRUE;

	Files_CloseFile(&map_file);

	intro_timer = 2.0f;
	beep_delay = 2.0f;

	process_outro = FALSE;
	outro_timer = 0.0f;

	float speech_bubble_time = 0.0f;

	hen_pos.x = 45.0f;
	hen_pos.y = 285.0f + 256.0f;
	//Sound_Play(sound_beep, hero->sound_source, NULL, NULL, TRUE, 0.5f);

	//tile_map->StartSpecialIntro("#\a\a\ainclude\a\a <STD\a\a\a\a\b\b\b\a\a\a\a\a\a\amagic.h>\a\a\a\a\n\a\n\a\aint main\a()\n\a{\n   static char \a\a\a\a\a\a\a\a\a\f\a\a\a\a\a\a\a;\n\a   return 0;\n\a}\n\a\a\a\a\a\a\a\a\a", 2, 2, 0.08f, 0.2f, &hero->pos);
	//tile_map->StartSpecialIntro("#incl\f", 2, 2, 0.08f, 0.2f, &hero->pos);
}

void Game_Exit()
{
	paused = FALSE;

	next_game_stage = GAME_STAGE_MENU_MAIN;
}

void Game_Init()
{
	sound_volume = 1.0f;
	controls_arrow_left = FALSE;
	controls_arrow_right = FALSE;
	controls_arrow_up = FALSE;
	controls_arrow_down = FALSE;
	controls_spacebar = FALSE;

	//GameConfig_Load();

	Font_Add("tronique");
	
	global_fade_k = 0.0f;
	
	paused = FALSE;

    game_stage = GAME_STAGE_MENU_MAIN;
    next_game_stage = game_stage;

	scene = new Scene;
	camera = new Camera;
	tile_map = new TileMap;
	hero = new Hero;

	scene->AddChild(hero);
	scene->AddChild(tile_map);
	
	// TODO: init stuff here
	Vector2D size;
	size.x = (float)v_sx;
	size.y = (float)v_sy;
	Sprites_AddSprite(&sprite_background, "background.tga", 0.0f, 0.0f, 0.0f, 0.0f, -512);
	Sprites_SetSpriteSize(sprite_background, &size);

	Sprites_AddSprite(&sprite_crt_l, "screen_tileset.tga", 0.0f, 48.0f, 48.0f, 48.0f, 256);
	Sprites_AddSprite(&sprite_crt_r, "screen_tileset.tga", 96.0f, 48.0f, 48.0f, 48.0f, 256);
	Sprites_AddSprite(&sprite_crt_u, "screen_tileset.tga", 48.0f, 0.0f, 48.0f, 48.0f, 256);
	Sprites_AddSprite(&sprite_crt_d, "screen_tileset.tga", 48.0f, 96.0f, 48.0f, 48.0f, 256);
	Sprites_AddSprite(&sprite_crt_lu, "screen_tileset.tga", 0.0f, 0.0f, 48.0f, 48.0f, 256);
	Sprites_AddSprite(&sprite_crt_ld, "screen_tileset.tga", 0.0f, 96.0f, 48.0f, 48.0f, 256);
	Sprites_AddSprite(&sprite_crt_ru, "screen_tileset.tga", 96.0f, 0.0f, 48.0f, 48.0f, 256);
	Sprites_AddSprite(&sprite_crt_rd, "screen_tileset.tga", 96.0f, 96.0f, 48.0f, 48.0f, 256);

	Sprites_AddSprite(&sprite_crt_center, "screen_tileset.tga", 48.0f, 48.0f, 48.0f, 48.0f, -256);

	Sprites_AddSprite(&sprite_crt_stand, "screen_tileset.tga", 0.0f, 192.0f, 256.0f, 64.0f, 128);

	Sprites_AddSprite(&speech_bubble_sprite, "speech_bubble.tga", 0.0f, 0.0f, 0.0f, 0.0f, -256);

	Sprites_AddSprite(&sprite_hen_1, "hen.tga", 0.0f, 0.0f, 512.0f, 256.0f, 0);
	Sprites_AddSprite(&sprite_hen_2, "hen.tga", 0.0f, 256.0f, 512.0f, 256.0f, 0);

	sound_beep = Sound_AddSound("beep.wav", SOUND_FLAG_DEFAULT, 256.0f);
	sound_error = Sound_AddSound("error.wav", SOUND_FLAG_DEFAULT, 256.0f);

	sound_hen = Sound_AddSound("hen.wav", SOUND_FLAG_DEFAULT, 256.0f);
	sound_hen_eat = Sound_AddSound("hen_eat.wav", SOUND_FLAG_DEFAULT, 256.0f);

	Menu_Init();

	Game_NewGame();
}

void Game_ProcessGameTime()
{
	level_time += game_delta_t;
}

void Game_ProcessOutro()
{
	static BOOL play_hen_sound = TRUE;
	outro_timer += delta_t;

	if (outro_timer < 8.0f)
	{
		DeltaFunc(&hero->pos.y, crt_bound_up + camera->pos.y + 46.0f, delta_t * 36.0f);
	}
	else if (outro_timer < 12.0f)
	{
		play_hen_sound = TRUE;
		hero->direction = TRUE;
		DeltaFunc(&hero->pos.x, crt_bound_right + 12.0f, delta_t * 48.0f);
	}
	else if (outro_timer < 12.3f)
	{
		if (play_hen_sound)
		{
			Sound_Play(tile_map->sound_char, hero->sound_source, NULL, NULL, TRUE, 0.5f);
			play_hen_sound = FALSE;
		}
		Set_SpeechBubble(275.0f, 275.0f, 2.5f, "Finally i'm\nFREE!");
	}
	else if (outro_timer < 15.0f)
	{
		//
	}
	else if (outro_timer < 15.5f)
	{
		play_hen_sound = TRUE;
		DeltaFunc(&hen_pos.y, 285.0f, 512.0f * delta_t);
		Sprites_DrawSprite(sprite_hen_1, hen_pos.x + camera->pos.x, hen_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	}
	else if (outro_timer < 16.0f)
	{
		if (play_hen_sound)
		{
			Sound_Play(sound_hen_eat, hero->sound_source, NULL, NULL, TRUE, 0.5f);
			play_hen_sound = FALSE;
		}

		DeltaFunc(&hen_pos.y, 285.0f + 32.0f, 256.0f * delta_t);
		DeltaFunc(&hero->pos.y, crt_bound_up + camera->pos.y + 46.0f + 32.0f, delta_t * 256.0f);
		Sprites_DrawSprite(sprite_hen_1, hen_pos.x + camera->pos.x, hen_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	}
	else if (outro_timer < 16.4f)
	{
		Sprites_DrawSprite(sprite_hen_1, hen_pos.x + camera->pos.x, hen_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	}
	else if (outro_timer < 16.5f)
	{
		hero->pos.x = -1000000.0f;
		Sprites_DrawSprite(sprite_hen_2, hen_pos.x + camera->pos.x, hen_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	}
	else if (outro_timer < 16.6f)
	{
		Sprites_DrawSprite(sprite_hen_1, hen_pos.x + camera->pos.x, hen_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	}
	else if (outro_timer < 16.7f)
	{
		play_hen_sound = TRUE;
		Sprites_DrawSprite(sprite_hen_2, hen_pos.x + camera->pos.x, hen_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	}
	else if (outro_timer < 17.8f)
	{
		if (play_hen_sound)
		{
			Sound_Play(sound_hen, hero->sound_source, NULL, NULL, TRUE, 0.5f);
			play_hen_sound = FALSE;
		}

		Sprites_DrawSprite(sprite_hen_1, hen_pos.x + camera->pos.x, hen_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	}
	else if (outro_timer < 21.95f)
	{
		DeltaFunc(&hen_pos.y, 285.0f + 256.0f, 128.0f * delta_t);
		Sprites_DrawSprite(sprite_hen_1, hen_pos.x + camera->pos.x, hen_pos.y + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	}
	else if (outro_timer < 22.0f)
	{
		if (!tile_map->process_special_intro)
			tile_map->StartSpecialIntro("thanks for playing\a\a\a\n\n    made by zzq\a\a\n\n for ludum dare 31\a\a\a\a\n\n      the end", 5, 9, 0.1f, 0.15f, 0.3f, &hero->pos);
	}
	else
	{
		scene->Process();
	}
	//Sprites_DrawSprite(sprite_hen_2, 45.0f + camera->pos.x, 285.0f + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_ALIGN_LEFT);
	// TODO: finally i'm free
	// TODO: ending scene
	// TODO: 
}

void Game_Process()
{
	Game_ProcessGameTime();

	Sound_UpdateListener(&camera->pos, NULL, NULL, NULL, delta_t, 1.0f, sound_volume);

	Menu_Process();
	
	game_stage = next_game_stage;

	if (game_stage == GAME_STAGE_GAME)
	{

		if (process_outro)
		{
			Game_ProcessOutro();
			return;
		}

		if (beep_delay > 0)
		{
			beep_delay -= delta_t;
			if (beep_delay <= 0)
				Sound_Play(sound_beep, hero->sound_source, NULL, NULL, TRUE, 0.5f);
		}

		if (intro_timer <= 0.0f)
		{
			if (controls_arrow_left)
			{
				hero->speed.x -= game_delta_t * 128.0f;
				if (hero->speed.x < -128.0f)
					hero->speed.x = -128.0f;
				hero->direction = FALSE;
			}
			else if (controls_arrow_right)
			{
				hero->speed.x += game_delta_t * 128.0f;
				if (hero->speed.x > 128.0f)
					hero->speed.x = 128.0f;
				hero->direction = TRUE;
			}
			else
			{
				DeltaFunc(&hero->speed.x, 0.0f, game_delta_t * 400.0f);
			}

			if (controls_arrow_up || controls_spacebar)
			{
				controls_arrow_up = FALSE;

				if (hero->speed.y == 0.0f)
				{
					hero->speed.y = 154.0f;
					Sound_Play(hero->sound_jump, hero->sound_source, NULL, NULL, TRUE, 0.3f);
				}
			}

			if (hero->speed.y < -400.0f)
			{
				hero->speed.x = 0.0f;
				hero->speed.y = 0.0f;
				hero->pos.x = tile_map->save_pos.x;
				hero->pos.y = tile_map->save_pos.y;
				camera->pos.y = hero->pos.y + 72.0f;

				Sound_Play(sound_error, hero->sound_source, NULL, NULL, TRUE, 0.3f);
			}
		}

		if (intro_timer <= 0.0f)
		{
			if (-160.0f > hero->pos.x - 32.0f)
			{
				crt_bound_left = hero->pos.x - 32.0f;
				if (speech_bubble_time <= 0.0f)
					Set_SpeechBubble(-300.0f, 250.0f, 4.0f, "Ha-ha! You will\nnewer leave this screen");
			}
			else
			{
				crt_bound_left = -160.0f;
			}

			if (160.0f < hero->pos.x + 32.0f)
			{
				crt_bound_right = hero->pos.x + 32.0f;
				if (speech_bubble_time <= 0.0f)
					Set_SpeechBubble(300.0f, 250.0f, 4.0f, "All your attempts\nare hopeless!");
			}
			else
			{
				crt_bound_right = 160.0f;
			}

			
		}

		if (tile_map->process_special_intro)
			camera->pos.y = -tile_map->special_intro_y * tile_map->interval.y * tile_map->scale + 200.0f;
		else if (intro_timer <= 0.0f)
		{
			DeltaFunc(&camera->pos.y, hero->pos.y + 72.0f, game_delta_t * (hero->speed.y < 0.0f) ? 256.0f : 64.0f);
		}

		tile_map->draw_bound_left = (int)((camera->pos.x - tile_map->pos.x) / (tile_map->interval.x * tile_map->scale) - (crt_bound_left / -160.0f) * 15.0f); // Random numbers
		tile_map->draw_bound_right = (int)((camera->pos.x - tile_map->pos.x) / (tile_map->interval.x * tile_map->scale) + (crt_bound_right / 160.0f) * 14.5f);
		tile_map->draw_bound_top = (int)(-(camera->pos.y - tile_map->pos.y) / (tile_map->interval.y * tile_map->scale) + 9);
		tile_map->draw_bound_bottom = (int)(-(camera->pos.y - tile_map->pos.y) / (tile_map->interval.y * tile_map->scale) - 8);

		if (!tile_map->process_special_intro && run_second_intro)
		{
			tile_map->StartSpecialIntro("#\a\a\b\aprocedure\a\a <com\a\a\a\a\b\b\b\a\a\a\a\a\a\amagic>\a\a\a\a\n\a\a{\n  create char \a\a\a\a\a\a\a\a\a\f\a\a\a\a\a\a\a;\n\n  .res undefined;\a\n}\a\a\a\a\a\a\a\a\a\a\a\a", tile_map->special_intro_base_x, tile_map->special_intro_y + 2, debug_mode ? 0.0f : 3.0f, debug_mode ? 0.0f : 0.08f, debug_mode ? 0.0f : 0.2f, &hero->pos);
			run_second_intro = FALSE;
		}

		if (!tile_map->process_special_intro)
		{
			if (intro_timer > 0)
			{
				intro_timer -= game_delta_t;
				if (intro_timer <= 0)
					Sound_Play(sound_error, hero->sound_source, NULL, NULL, TRUE, 0.3f);
			}
		}

		scene->Process();

		if (!paused)
		{
			Vector2D pos, speed;
			pos.x = hero->pos.x;
			pos.y = hero->pos.y;
			speed.x = hero->speed.x;
			speed.y = hero->speed.y;
			tile_map->CheckColision(&pos, &speed, &hero->size);
			hero->pos.x = pos.x;
			hero->pos.y = pos.y;
			hero->speed.x = speed.x;
			hero->speed.y = speed.y;
			// TODO: process other things
		}
	}
}

void Game_Render()
{
	Render_DisableDepthMask();
	Render_DisableDepthTest();
	Render_DisableAlphaTest();
	Render_SetBlendFunc(TR_SRC_ALPHA, TR_ONE_MINUS_SRC_ALPHA);
	Render_EnableBlend();
	   
    Render_Clear(0.0f, 0.2f, 0.4f, 0.0f);

	if (game_stage == GAME_STAGE_GAME)
	{
		scene->Render();
		//Sprites_DrawSprite(sprite_background, 0.0f + camera->pos.x, 0.0f + camera->pos.y, 1.0f, 0.0f, 0xffffffff, SPRITE_CENTERED);
		Draw_CRT();
		Draw_SpeechBubble();
		//Font_PrintText(-140, 80, 1.0f, 0xff808080, "int main()\n{\n    char  ;\n\n    return 0;\n}", TEXT_ALIGN_LEFT, "tronique");
		// TODO: render stuff if required
		Sprites_Render();
		Font_Render();
	}
	
    Render_SetProjectionOrtho(-2048.0f, 2048.0f, 1.0f);
	Render_MatrixTranslate(-screen_center_x, -screen_center_y, 0.0f);
    
    Render_SetMatrixMode(TR_MODEL);
    Render_ResetMatrix();
    
    Render_SetMatrixMode(TR_TEXTURE);
    Render_ResetMatrix();
    
	Menu_Render();
}

void Game_Pause() // TODO: make your possibility to resume
{
    paused = TRUE;
}

void Game_Release()
{
	// TODO: clean it

	SAFE_DELETE(hero);
	SAFE_DELETE(tile_map);
	SAFE_DELETE(camera);
	SAFE_DELETE(scene);

	Menu_Release();
}
