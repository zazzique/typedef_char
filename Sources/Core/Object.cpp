#ifndef _MSC_VER
#ifdef __cplusplus
extern "C" {
#endif
#endif
#include "Common.h"
#include "FastMath.h"
#include "Vector.h"
#ifndef _MSC_VER
#ifdef __cplusplus
}
#endif
#endif
#include "Object.h"


Object::Object(void)
{
	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = 0.0f;
	alpha = 0.0f;
	beta = 0.0f;
	gamma = 0.0f;
	speed.x = 0.0f;
	speed.y = 0.0f;
	speed.z = 0.0f;
	speed_alpha = 0.0f;
	speed_beta = 0.0f;
	speed_gamma = 0.0f;
	scale = 1.0f;

	render_bound_radius = 16.0f;

	active = TRUE;
	visible = TRUE;
}

void Object::ProcessMovement()
{
	pos.x += speed.x * game_delta_t;
	pos.y += speed.y * game_delta_t;
	pos.z += speed.z * game_delta_t;

	if (speed_alpha != 0.0f)
	{
		alpha += speed_alpha * game_delta_t;
		BoundAngle_0_360_Deg(&alpha);
	}
	if (speed_beta != 0.0f)
	{
		beta += speed_beta * game_delta_t;
		BoundAngle_0_360_Deg(&beta);
	}
	if (speed_gamma != 0.0f)
	{
		gamma += speed_gamma * game_delta_t;
		BoundAngle_0_360_Deg(&gamma);
	}
}

Object::~Object(void)
{
}
