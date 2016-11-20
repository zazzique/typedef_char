#ifndef _MSC_VER
#ifdef __cplusplus
extern "C" {
#endif
#endif
#include "Common.h"
#include "FastMath.h"
#include "Vector.h"
#include "Sprites.h"
#include "Sound.h"
#ifndef _MSC_VER
#ifdef __cplusplus
}
#endif
#endif
#include "Object.h"
#include "Hero.h"


Hero::Hero(void)
{
	size.x = 6.0f;
	size.y = 8.0f;

	direction = TRUE;
	Sprites_AddSprite(&sprite_index, "char.tga", 0, 0, 0, 0, 128);

	sound_source = Sound_AddSource();

	sound_jump = Sound_AddSound("jump.wav", SOUND_FLAG_DEFAULT, 256.0f);
}

void Hero::Process()
{
	ProcessMovement();

	speed.y -= 256.0f * game_delta_t;
}

void Hero::Render()
{
	U32 sprite_flags = SPRITE_CENTERED;
	if (!direction)
		sprite_flags |= SPRITE_FLIP_X;

	Sprites_DrawSprite(sprite_index, pos.x, pos.y, 0.5f, 0.0f, 0xff00ff00, sprite_flags);
}

Hero::~Hero(void)
{
}
