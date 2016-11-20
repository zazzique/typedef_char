#ifndef _MSC_VER
#ifdef __cplusplus
extern "C" {
#endif
#endif
#include "Common.h"
#include "FastMath.h"
#include "Vector.h"
#include "Render.h"
#ifndef _MSC_VER
#ifdef __cplusplus
}
#endif
#endif
#include "Camera.h"


Camera::Camera(void)
{
	view_range = VIEW_RANGE_DEFAULT;

	fov_x = FOV_DEFAULT;
	float aspect_ratio = (float)v_sx / (float)v_sy;
	fov_y = fov_x / aspect_ratio;

	isometric = TRUE;

	scale = 1.0f;
}

void Camera::CalculateFrustum(Frustum *f) // TODO: different for isometry
{
	Vector3D v[8];
	Vector3D a;

	int points_count;
	
	if (isometric)
	{
		points_count = 8;

		v[0].x = -(float)v_sx * 0.5f * scale;
		v[0].y = -(float)v_sy * 0.5f * scale;
		v[0].z = view_range;

		v[1].x = v[0].x;
		v[1].y = -v[0].y;
		v[1].z = v[0].z;

		v[2].x = -v[0].x;
		v[2].y = -v[0].y;
		v[2].z = v[0].z;

		v[3].x = -v[0].x;
		v[3].y = v[0].y;
		v[3].z = v[0].z;

		v[4].x = v[0].x;
		v[4].y = v[0].y;
		v[4].z = -v[0].z;

		v[5].x = v[1].x;
		v[5].y = v[1].y;
		v[5].z = -v[1].z;

		v[6].x = v[2].x;
		v[6].y = v[2].y;
		v[6].z = -v[2].z;

		v[7].x = v[3].x;
		v[7].y = v[3].y;
		v[7].z = -v[3].z;
	}
	else
	{
		points_count = 5;

		v[0].x = 0.0f;
		v[0].y = 0.0f;
		v[0].z = 0.0f;
	
		v[1].x = - tanf(DEG2RAD(fov_x) * 0.5f) * view_range;
		v[1].y = v[1].x * (fov_y / fov_x);
		v[1].z = - view_range;
	
		v[2].x = v[1].x;
		v[2].y = - v[1].y;
		v[2].z = v[1].z;
	
		v[3].x = - v[1].x;
		v[3].y = v[2].y;
		v[3].z = v[1].z;
	
		v[4].x = v[3].x;
		v[4].y = v[1].y;
		v[4].z = v[1].z;
	}
	
	float ca, sa, cb, sb, cg, sg;
	
    sa = sinf(DEG2RAD(-alpha));
    ca = cosf(DEG2RAD(-alpha));
    sb = sinf(DEG2RAD(-beta));
    cb = cosf(DEG2RAD(-beta));
    sg = sinf(DEG2RAD(-gamma));
    cg = cosf(DEG2RAD(-gamma));
    

	for (int i = 0; i < points_count; i ++)
	{
		a.x = v[i].x * cg + v[i].y * sg;
		a.y = v[i].y * cg - v[i].x * sg;
		v[i].x = a.x;
		v[i].y = a.y;
		
		a.y = v[i].y * cb + v[i].z * sb;
		a.z = v[i].z * cb - v[i].y * sb;
		v[i].y = a.y;
		v[i].z = a.z;
		
		a.x = v[i].x * ca + v[i].z * sa;
		a.z = v[i].z * ca - v[i].x * sa;
		v[i].x = a.x;
		v[i].z = a.z;
        
		v[i].x += pos.x;
		v[i].y += pos.y;
		v[i].z += pos.z;
	}
	
	if (isometric)
		Frustum_CalculateIsometric(f, v);
	else
		Frustum_Calculate(f, v);
}

void Camera::Setup()
{
	if (isometric)
	{
		Render_SetMatrixMode(TR_PROJECTION);
		Render_SetProjectionOrtho(-view_range, view_range, scale);

		Render_MatrixRotate(-gamma, 0.0f, 0.0f, 1.0f);
		Render_MatrixRotate(-beta, 1.0f, 0.0f, 0.0f);
		Render_MatrixRotate(alpha, 0.0f, 1.0f, 0.0f);
		Render_MatrixTranslate(-pos.x, -pos.y, -pos.z);
	}
	else
	{
		Render_SetMatrixMode(TR_PROJECTION);
		Render_SetProjectionFrustum(8.0f, view_range, fov_x, fov_y);
        
		Render_MatrixRotate(-gamma, 0.0f, 0.0f, 1.0f);
		Render_MatrixRotate(-beta, 1.0f, 0.0f, 0.0f);
		Render_MatrixRotate(alpha, 0.0f, 1.0f, 0.0f);
		Render_MatrixTranslate(-pos.x, -pos.y, -pos.z);
	}
}

void Camera::Process()
{
	ProcessMovement();
}

void Camera::Render()
{
	//
}


Camera::~Camera(void)
{
}
