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
#include "Camera.h"
#include "Scene.h"


Scene::Scene(void)
{
	objects_count = 0;
	camera = NULL;
}

BOOL Scene::AddChild(Object *object)
{
	if (objects_count >= MAX_OBJECTS_COUNT)
		return FALSE;

	if (object == NULL)
		return FALSE;

	for (int i = 0; i < objects_count; i ++)
	{
		if (objects[i] == object)
		{
			return TRUE;
		}
	}

	objects[objects_count] = object;

	objects_count ++;

	return TRUE;
}

BOOL Scene::RemoveChild(Object *object)
{
	for (int i = 0; i < objects_count; i ++)
	{
		if (objects[i] == object)
		{
			for (int j = i; j < objects_count - 1; j ++)
				objects[j] = objects[j + 1];

			objects_count --;

			return TRUE;
		}
	}

	return FALSE;
}

void Scene::SetCamera(Camera *camera)
{
	this->camera = camera;
}

void Scene::Process()
{
	for (int i = 0; i < objects_count; i ++)
	{
		if (objects[i] != NULL)
			if (objects[i]->active)
				objects[i]->Process();
	}

	if (camera != NULL)
		camera->Process();
}

void Scene::Render()
{
	if (camera == NULL)
		return;

	camera->Setup();

	//Frustum f;
	//camera->CalculateFrustum(&f);

	for (int i = 0; i < objects_count; i ++)
	{
		if (objects[i] != NULL)
		{
			if (objects[i]->visible)
			{
				//if (Frustum_IsSphereInside(&f, &objects[i]->pos, objects[i]->render_bound_radius))
					objects[i]->Render();
			}
		}
	}
}

Scene::~Scene(void)
{
}
