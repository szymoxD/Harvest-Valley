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

	particle(float x = 0, float y = 40, float z = 0,float lifetime=8, float size_v=5, float speed_v=90, float red=0.5, float green=0.5, float blue=0.5, float acceleration_v=-55, bool type_v=true);
	void update(float elapsed);
	bool if_dead() { return dead; }
	void draw();
};

