#include "Teren.h"
#include <iostream>
#include <cmath>
bool Teren::loadTerrain(const char* path, float scale) {
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;

	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}
	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.x =vertex.x * scale;
			vertex.z =vertex.z * scale;
			vertex.y = vertex.y * scale;
			terrain_map.push_back(vertex);
		}
	}
	fclose(file);
	int min = 0;
	glm::vec3 temp;
	for (int i = 0; i < terrain_map.size(); i++) {
		min = i;
		if (i == 1508)
			int test = 2;
		for (int j = i + 1; j < terrain_map.size(); j++) {
			if (abs(terrain_map[j].x - terrain_map[min].x) < 1) {
				if(terrain_map[j].z < terrain_map[min].z)
					min = j;
			}
			else if (terrain_map[j].x < terrain_map[min].x)
				min = j;
		}
		temp = terrain_map[i];
		terrain_map[i] = terrain_map[min];
		terrain_map[min] = temp;
	}
	z_size = 0;
	while (abs(terrain_map[z_size].x - terrain_map[z_size++].x) < 1);
	x_size = terrain_map.size() / z_size;
	return true;
}
float Teren::get_height(float x, float z)const {
	if (x < terrain_map[0].x)
		x = terrain_map[0].x;
	else if (x > terrain_map[terrain_map.size() - 1].x)
		x = terrain_map[terrain_map.size() - 1].x;
	if (z < terrain_map[0].z)
		z = terrain_map[0].z;
	else if (z > terrain_map[terrain_map.size() - 1].z)
		z = terrain_map[terrain_map.size() - 1].z;
	x += (abs(terrain_map[terrain_map.size() - 1].x) + abs(terrain_map[0].x)) / 2;
	z += (abs(terrain_map[terrain_map.size() - 1].z) + abs(terrain_map[0].z)) / 2;

	double xRatio = x / (abs(terrain_map[terrain_map.size() - 1].x) + abs(terrain_map[0].x));
	int xind = int(xRatio * (x_size-1));
	double xoff = (xRatio * (x_size-1)) - xind;

	double zRatio = z / (abs(terrain_map[terrain_map.size() - 1].z) + abs(terrain_map[0].z));
	int zind = int(zRatio * (z_size-1));
	double zoff = (zRatio * (z_size-1)) - zind;

	int xpom = xind + 1 >= x_size ? x_size-1 : xind + 1;
	int zpom = zind + 1 >= x_size ? x_size-1 : zind + 1;

	int index1 = xind * z_size + zind;
	int index2 = xpom * z_size + zind;
	int index3 = xind * z_size + zpom;
	int index4 = xpom * z_size + zpom;

	// Oblicz wysokoœæ interpolowan¹ na podstawie s¹siednich punktów terenu
	float height1 = terrain_map[index1].y;
	float height2 = terrain_map[index2].y;
	float height3 = terrain_map[index3].y;
	float height4 = terrain_map[index4].y;

	float interpolated_height1 = height1 + xoff * (height2 - height1);
	float interpolated_height2 = height3 + xoff * (height4 - height3);

	// Interpoluj wysokoœæ na osi z
	float interpolated_height = interpolated_height1 + zoff * (interpolated_height2 - interpolated_height1);

	return interpolated_height;

	int index = xind * z_size + zind;//   (xRatio * (x_size - 1)) + zRatio * (z_size)-1);
	float z1 = terrain_map[index].y+(terrain_map[index].y - terrain_map[index + z_size].y) * zoff;
	float height = terrain_map[index].y;
	return height;
}
