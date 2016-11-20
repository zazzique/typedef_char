#pragma once

#define MAX_OBJECTS_COUNT 2048

class Scene
{
protected:
	Camera *camera;
	Object *objects[MAX_OBJECTS_COUNT];
	int objects_count;
public:
	Scene(void);
	BOOL AddChild(Object *object);
	BOOL RemoveChild(Object *object);
	void SetCamera(Camera *camera);
	void Process();
	void Render();
	~Scene(void);
};

