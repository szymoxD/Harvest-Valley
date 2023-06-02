#pragma once
#include <windows.h>
#include <vector>
#include "glm\glm.hpp"
#include <gl\glu.h>
#include <cstdlib>

class particle
{
	float lifespan;
	float max_lifetime;
	float size;
	glm::vec3 pos;
	float angle;
	float v_angle;
	float v_acc;
	float red;
	float green;
	float blue;
	float speed;
	float speedx;
	float speedz;
	float acceleration;
	bool type;
	bool dead;
public:

	particle(float x = 0, float y = 40, float z = 0, bool type_v = true, bool dead_v=true, float speed_v = 90, float acceleration_v = -55, float lifetime = 8, float size_v = 5, float red = 0.5, float green = 0.5, float blue = 0.5);
	void update(float elapsed);
	bool if_dead() { return dead; }
	void draw();
};

