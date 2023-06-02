#pragma once
#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <vector>
#include "glm\glm.hpp"
#include <gl\glu.h>
#include "Teren.h"

class Pole
{
	int wierzcholki[1000][4] = {};
	float lifespan[1000] = {};
	const float max_lifespan;
	int current=0;
	Teren* teren;
	GLuint textureID;
public:
	Pole(Teren* teren_v, float max_lifespan=20);
	bool plow(int const* wierzcholki);
	void update(float elapsed);
	void draw();
	GLuint loadBMP_custom(const char* imagepath);
};

