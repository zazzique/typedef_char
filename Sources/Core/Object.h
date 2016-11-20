#pragma once

class Object
{
	friend class Scene;

protected:
	float render_bound_radius;
	virtual void ProcessMovement();
public:
	BOOL active;
	BOOL visible;

	Vector3D pos;
	float alpha;
	float beta;
	float gamma;
	Vector3D speed;
	float speed_alpha;
	float speed_beta;
	float speed_gamma;
	float scale;

	Object(void);
	virtual void Process() = 0;
	virtual void Render() = 0;
	~Object(void);
};

