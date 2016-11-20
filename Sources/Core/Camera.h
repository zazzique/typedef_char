#pragma once

#include "Object.h"

class Camera : public Object
{
protected:
	//
public:
	float view_range;
	float fov_x, fov_y;
	BOOL isometric;

	Camera(void);
	void Setup();
	void CalculateFrustum(Frustum *f);
	virtual void Process();
	virtual void Render();
	~Camera(void);
};

