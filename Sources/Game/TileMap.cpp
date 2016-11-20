#ifndef _MSC_VER
#ifdef __cplusplus
extern "C" {
#endif
#endif
#include "Common.h"
#include "GameVariables.h"
#include "FastMath.h"
#include "Vector.h"
#include "Files.h"
#include "Sprites.h"
#include "Sound.h"
#ifndef _MSC_VER
#ifdef __cplusplus
}
#endif
#endif
#include "TileMap.h"


TileMap::TileMap(void)
{
	draw_bound_left = -16;
	draw_bound_right = 16;
	draw_bound_top = 8;
	draw_bound_bottom = -8;

	data = NULL;
	width = 32;
	height = 32;
	interval.x = 16.0f;
	interval.y = 16.0f;
	tile_size.x = 16.0f;
	tile_size.y = 16.0f;
	
	sprites_indices = NULL;
	sprites_count = 0;

	process_special_intro = FALSE;
	special_intro_x = 0;
	special_intro_y = 0;
	strcpy(special_intro_line, "");

	special_intro_step = 0;
	special_intro_timer = 0;

	special_intro_speed_base = 0.2f;
	special_intro_speed_random = 0.0f;
	special_intro_pos = NULL;

	sound_source = Sound_AddSource();

	sound_key = Sound_AddSound("key.wav", SOUND_FLAG_DEFAULT, 256.0f);
	sound_char = Sound_AddSound("char.wav", SOUND_FLAG_DEFAULT, 256.0f);
	sound_hit = Sound_AddSound("hit.wav", SOUND_FLAG_DEFAULT, 256.0f);
	sound_boost = Sound_AddSound("boost.wav", SOUND_FLAG_DEFAULT, 256.0f);

	save_pos.x = 0.0f;
	save_pos.y = 0.0f;
}

void TileMap::Load(const char* filename)
{
	// TODO: safe release map and sprites
	FileHandler map_file;
	char texture_name[256];
	int atlas_width, atlas_height;
	
	if (!Files_OpenFile(&map_file, filename))
	{
		LogPrint("Error: tile map '%s' not found!\n", filename);
		return;
	}
	
	char *content = NULL;
	I32 content_size;

	Files_GetData(&map_file, (void **)&content, &content_size);

	if (content == NULL)
	{
		LogPrint("Error: tile map '%s' not found!\n", filename);
		return;
	}
	
	content[content_size - 1] = '\0';
	
	char *cur_exp = NULL;
	cur_exp = strtok(content, CONTENT_PARSE_SEPARATORS);
	
	while (cur_exp != NULL)
	{
		if (cur_exp == NULL)
			break;
		
		if (strcmp(cur_exp, "width") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			width = atoi(cur_exp);
			continue;
		}
		if (strcmp(cur_exp, "height") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			height = atoi(cur_exp);
			continue;
		}
		if (strcmp(cur_exp, "interval_x") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			interval.x = atof(cur_exp);
			continue;
		}
		if (strcmp(cur_exp, "interval_y") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			interval.y = atof(cur_exp);
			continue;
		}
		if (strcmp(cur_exp, "atlas_width") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			atlas_width = atoi(cur_exp);
			continue;
		}
		if (strcmp(cur_exp, "atlas_height") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			atlas_height = atoi(cur_exp);
			continue;
		}
		if (strcmp(cur_exp, "tiles_count") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			sprites_count = atoi(cur_exp);
			continue;
		}
		if (strcmp(cur_exp, "tile_width") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			tile_size.x = atof(cur_exp);
			continue;
		}
		if (strcmp(cur_exp, "tile_height") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			tile_size.y = atof(cur_exp);
			continue;
		}
		else if (strcmp(cur_exp, "texture") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			strcpy(texture_name, cur_exp);
			continue;
		}

		cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
	}

	Files_CloseFile(&map_file);

	data = new U8[width * height];

	for (int i = 0; i < width * height; i ++)
	{
		data[i] = 0x00;
	}

	sprites_indices = new int[sprites_count];

	for (int i = 0; i < sprites_count; i ++)
	{
		float x = (float)(i % atlas_width) * tile_size.x;
		float y = (float)(i / atlas_width) * tile_size.y;
		Sprites_AddSprite(&sprites_indices[i], texture_name, x, y, tile_size.x, tile_size.y, 32);
	}
}

void TileMap::Unload()
{
	SAFE_DELETE(data);
	SAFE_DELETE(sprites_indices);
}

void TileMap::CheckColision(Vector2D *object_pos, Vector2D *object_speed, const Vector2D *object_size)
{
	if (data == NULL)
		return;

	if (sprites_indices == NULL)
		return;

	Vector2D local_object_pos;
	local_object_pos.x = (object_pos->x - pos.x) / scale;
	local_object_pos.y = -(object_pos->y - pos.y) / scale;

	int left_bound = (int)(((local_object_pos.x - object_size->x * 0.5f / scale) / interval.x) + 0.5f);
	if (left_bound < 0)
		left_bound = 0;
	if (left_bound >= width)
		left_bound = width;

	int right_bound = (int)(((local_object_pos.x + object_size->x * 0.5f / scale) / interval.x) + 0.5f);
	if (right_bound < 0)
		right_bound = 0;
	if (right_bound >= width)
		right_bound = width;

	int horisontal_center = (int)((local_object_pos.x / interval.x) + 0.5f);
	if (horisontal_center < 0)
		horisontal_center = 0;
	if (horisontal_center >= width)
		horisontal_center = height;

	int upper_bound = (int)(((local_object_pos.y - object_size->y * 0.5f / scale) / interval.y) + 0.5f);
	if (upper_bound < 0)
		upper_bound = 0;
	if (upper_bound >= height)
		upper_bound = height;

	int lower_bound = (int)(((local_object_pos.y + object_size->y * 0.5f / scale) / interval.y) + 0.5f);
	if (lower_bound < 0)
		lower_bound = 0;
	if (lower_bound >= height)
		lower_bound = height;

	int vertical_center = (int)((local_object_pos.y / interval.y) + 0.5f);
	if (vertical_center < 0)
		vertical_center = 0;
	if (vertical_center >= height)
		vertical_center = height;

	if (data[vertical_center * width + left_bound] > 33)
	{
		local_object_pos.x = (((float)(left_bound + 1) - 0.5f) * interval.x) + object_size->x * 0.5f / scale;
		object_speed->x = 0.0f;
	}
	else if (data[vertical_center * width + right_bound] > 33)
	{
		local_object_pos.x = (((float)(right_bound) - 0.5f) * interval.x) - object_size->x * 0.5f / scale;
		object_speed->x = 0.0f;
	}

	if (data[upper_bound * width + horisontal_center] > 33)
	{
		local_object_pos.y = (((float)(upper_bound + 1) - 0.5f) * interval.y) + object_size->y * 0.5f / scale;
		object_speed->y = -32.0f;
	}
	else if (data[lower_bound * width + horisontal_center] > 33)
	{
		local_object_pos.y = (((float)(lower_bound) - 0.5f) * interval.y) - object_size->y * 0.5f / scale;
		if (object_speed->y < -20.0f)
			Sound_Play(sound_hit, sound_source, NULL, NULL, TRUE, 0.3f);
		object_speed->y = 0.0f;
	}

	if (data[vertical_center * width + horisontal_center] == 12)
	{
		if (object_speed->y < 220.0f)
			Sound_Play(sound_boost, sound_source, NULL, NULL, TRUE, 0.5f);

		object_speed->y = 300.0f;
	}

	if (data[vertical_center * width + horisontal_center] == 28)
	{
		process_outro = TRUE;
	}

	object_pos->x = local_object_pos.x * scale + pos.x;
	object_pos->y = -local_object_pos.y * scale + pos.y;

	if (data[vertical_center * width + horisontal_center] == 03)
	{
		save_pos.x = object_pos->x;
		save_pos.y = object_pos->y;
	}
}

void TileMap::StartSpecialIntro(char *line, int x, int y, float delay, float speed_base, float speed_random, Vector3D *intro_pos)
{
	process_special_intro = TRUE;
	special_intro_base_x = x;
	special_intro_x = x;
	special_intro_y = y;
	strcpy(special_intro_line, line);

	special_intro_step = 0;
	special_intro_timer = delay;

	special_intro_speed_base = speed_base;
	special_intro_speed_random = speed_random;

	special_intro_pos = intro_pos;

	data[special_intro_y * width + special_intro_x] = '_' + 1;

	paused = TRUE;
}

void TileMap::SpecialIntroProcess(BOOL sound)
{
	if (!process_special_intro)
		return;

	if (special_intro_step >= strlen(special_intro_line))
	{
		process_special_intro = FALSE;
		paused = FALSE;
		return;
	}

	special_intro_timer -= delta_t;
	if (special_intro_timer <= 0.0f)
	{
		special_intro_timer = special_intro_speed_base + RandFloat() * special_intro_speed_random;

		if (special_intro_line[special_intro_step] == '\a')
		{
			special_intro_step ++;
		}
		else
		{
			if (special_intro_line[special_intro_step] == '\f')
			{
				data[special_intro_y * width + special_intro_x] = 0x00;

				if (special_intro_pos != NULL)
				{
					special_intro_pos->x = (float)special_intro_x * interval.x * scale + pos.x;
					special_intro_pos->y = -(float)special_intro_y * interval.y * scale + pos.y;

					save_pos.x = special_intro_pos->x;
					save_pos.y = special_intro_pos->y;

					if (sound)
						Sound_Play(sound_char, sound_source, NULL, NULL, TRUE, 0.5f);
				}
				special_intro_x ++;
			}
			else if (special_intro_line[special_intro_step] == '\n')
			{
				data[special_intro_y * width + special_intro_x] = 0x00;
				special_intro_x = special_intro_base_x;
				special_intro_y ++;

				if (sound)
					Sound_Play(sound_key, sound_source, NULL, NULL, TRUE, 0.2f);
			}
			else if (special_intro_line[special_intro_step] == '\b')
			{
				data[special_intro_y * width + special_intro_x] = 0x00;
				special_intro_x --;

				if (sound)
					Sound_Play(sound_key, sound_source, NULL, NULL, TRUE, 0.2f);
			}
			else
			{
				data[special_intro_y * width + special_intro_x] = special_intro_line[special_intro_step] + 1;
				special_intro_x ++;

				if (sound)
					Sound_Play(sound_key, sound_source, NULL, NULL, TRUE, 0.2f);
			}

			data[special_intro_y * width + special_intro_x] = '_' + 1;

			special_intro_step ++;
		}
	}
}

void TileMap::Process()
{
	if (data == NULL)
		return;

	if (sprites_indices == NULL)
		return;

	if (special_intro_timer <= 0.0f)
	{
		do
		{
			SpecialIntroProcess(FALSE);
		}
		while (special_intro_line[special_intro_step] != '\n' && special_intro_line[special_intro_step] != '\0');

		if (!debug_mode)
		{
			special_intro_timer = 0.05f;
			Sound_Play(sound_key, sound_source, NULL, NULL, TRUE, 0.2f);
		}
	}
	else
	{
		SpecialIntroProcess(TRUE);
	}
}

void TileMap::Render()
{
	if (data == NULL)
		return;

	if (sprites_indices == NULL)
		return;

	if (draw_bound_left < 0)
		draw_bound_left = 0;
	if (draw_bound_right > width)
		draw_bound_right = width;
	if (draw_bound_top > height)
		draw_bound_top = height;
	if (draw_bound_bottom < 0)
		draw_bound_bottom = 0;

	for (int y = draw_bound_bottom; y < draw_bound_top; y ++)
	{
		for (int x = draw_bound_left; x < draw_bound_right; x ++)
		{
			U8 index = data[y * width + x];
			if (index != 0x00 && index <= sprites_count)
				if (!process_special_intro || index > 33 || process_outro)
					Sprites_DrawSprite(sprites_indices[index - 1], (x * interval.x) * scale + pos.x, (-y * interval.y) * scale + pos.y, scale, 0.0f, 0xffffffff, SPRITE_CENTERED);
		}
	}
}

TileMap::~TileMap(void)
{
	SAFE_DELETE(data);
	SAFE_DELETE(sprites_indices);
}
