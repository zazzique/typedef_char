#pragma once

#include "Object.h"

class Hero : public Object
{
public:
	Vector2D size;
	BOOL direction;
	int sprite_index;
	int sound_source;
	int sound_jump;

	Hero(void);
	void Process();
	void Render();
	~Hero(void);
};

