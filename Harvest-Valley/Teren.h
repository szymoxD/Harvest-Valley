#pragma once
#include "Obiekt.h"
class Teren : public Obiekt
{
	std::vector< glm::vec3 > terrain_map;
	int x_size;
	int z_size;
	float scale_v;
	
public:
	Teren(const char* path, float scale, const Obiekt* parent = nullptr, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f, float px = 0.0f, float py = 0.0f, float pz = 0.0f, float cx = 0.0f, float cy = 0.0f, float cz = 0.0f)
		: Obiekt(path, scale, parent, rx, ry, rz, px, py, pz, cx, cy, cz) {
		loadTerrain(path,scale);
		scale_v = scale;
	}
	bool loadTerrain(const char* path, float scale);
	float get_height(float x, float z)const;
};
