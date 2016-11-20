#pragma once

#include "Object.h"

class TileMap : public Object
{
protected:

	int *sprites_indices;
	int sprites_count;

	char special_intro_line[512 * 1024];
	int special_intro_step;
	float special_intro_timer;
	float special_intro_speed_base;
	float special_intro_speed_random;
	Vector3D *special_intro_pos;

public:
	U8 *data;
	int width;
	int height;
	Vector2D tile_size;
	Vector2D interval;
	BOOL process_special_intro;
	int special_intro_base_x;
	int special_intro_x, special_intro_y;

	Vector2D save_pos;

	int draw_bound_left;
	int draw_bound_right;
	int draw_bound_top;
	int draw_bound_bottom;

	int sound_source;
	int sound_key;
	int sound_char;
	int sound_hit;
	int sound_boost;

	TileMap(void);
	void Load(const char* filename);
	void Unload();
	void StartSpecialIntro(char *line, int x, int y, float delay, float speed_base, float speed_random, Vector3D *intro_pos);
	void SpecialIntroProcess(BOOL nosound);
	void CheckColision(Vector2D *object_pos, Vector2D *object_speed, const Vector2D *object_size);
	void Process();
	void Render();
	~TileMap(void);
};

