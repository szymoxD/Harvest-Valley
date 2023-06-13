#pragma once
#include "znak.h"
#include <string>
#include <Windows.h>
#include <map>
class tekst
{
	std::string value;
	float posx;
	float posy;

public:
	tekst(const char* value_v = "", float posx_v = 0, float posy_v = 0);
	void draw(std::map<char, znak> znaki_v = {});
	void move(float posx_v, float posy_v);
	void setValue(std::string value_v);
	void drawRect(float posx_v, float posy_v, float posx_v2, float posy_v2);
};

