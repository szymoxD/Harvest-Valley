#pragma once
#include <windows.h>
#include <vector>
#include <array>
#include "glm\glm.hpp"
#include <gl\glu.h>
#include<iostream>
class Collision
{
	static const int size = 100;
	std::vector<std::array<float, 12>> squaresleft;
	std::vector<std::array<float, 12>> squaresright;
	std::vector<std::array<float, 4>> circlesleft;
	std::vector<std::array<float, 4>> circlesright;
public:
	Collision() {};
	bool collide(std::array<float, 12> square, float angle);
	void addsquare(std::array<float, 12>);
	void addcircle(std::array<float, 4>);
	void popcircle(std::array<float, 4>);
};

