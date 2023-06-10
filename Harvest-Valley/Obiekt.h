#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <vector>
#include "glm\glm.hpp"
#include <gl\glu.h>
#define GL_PI 3.14159
#pragma once
class Obiekt
{
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	GLfloat* vertexArray;
	GLuint textureID;
	glm::vec3 pos;
	glm::vec3 center;
	glm::vec3 rot;

	const Obiekt* mounted;
	bool loadOBJ(const char* path, float scale);
	void normalize_rotation();
	bool if_loaded;
	bool has_texture;
	float scale;
public:
	Obiekt(const char* path, float scale, const Obiekt* parent = nullptr, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f, float px = 0.0f, float py = 0.0f, float pz = 0.0f, float cx = 0.0f, float cy = 0.0f, float cz = 0.0f);
	//konstr kop
	//oper przyp
	Obiekt(const Obiekt& ob);
	~Obiekt();
	bool loaded() const { return if_loaded; }
	void mount(const Obiekt&);
	void unmount();
	bool if_unmounted() { return mounted == nullptr; }
	bool to_mount(const Obiekt& ob) { return abs(pos.x - ob.get_position_x()) < 10 && abs(pos.y - ob.get_position_y()) < 10 && abs(pos.z - ob.get_position_z()) < 10; }
	void set_rotate(float x, float y = 0.0f, float z = 0.0f);
	void rotate(float x, float y = 0.0f, float z = 0.0f);
	void set_rotate_x(float x) { rot.x = x; normalize_rotation(); }
	void set_rotate_y(float y) { rot.y = y; normalize_rotation(); }
	void set_rotate_z(float z) { rot.z = z; normalize_rotation(); }
	void rotate_x(float x) { rot.x += x; normalize_rotation(); }
	void rotate_y(float y) { rot.y += y; normalize_rotation(); }
	void rotate_z(float z) { rot.z += z; normalize_rotation(); }
	void set_position(float x, float y, float z);
	void move(float x, float y, float z);
	void draw();
	float get_rotation_x()const { return rot.x; }
	float get_rotation_y()const { return rot.y; }
	float get_rotation_z()const { return rot.z; }
	float get_center_x()const { return center.x; }
	float get_center_y()const { return center.y; }
	float get_center_z()const { return center.z; }
	float get_position_x()const { return pos.x; }
	float get_position_y()const { return pos.y; }
	float get_position_z()const { return pos.z; }
	float get_absolute_position_x(float offx = 0, float offz = 0)const { if (mounted != nullptr)return pos.x + mounted->get_position_x() + (center.z + offz) * sin(mounted->get_rotation_y() * 3.14159 / 180) + (center.x + offx) * sin((mounted->get_rotation_y() + 90) * 3.14159 / 180); return pos.x + (center.x + offx) * sin((get_rotation_y() + 90) * 3.14159 / 180) + (center.z + offz) * sin(get_rotation_y() * 3.14159 / 180); }
	float get_absolute_position_y()const { if (mounted != nullptr)return pos.y + mounted->get_position_y() + center.y; return pos.y + center.y; }
	float get_absolute_position_z(float offx = 0, float offz = 0)const { if (mounted != nullptr)return pos.z + mounted->get_position_z() + (center.z + offz) * sin((mounted->get_rotation_y() + 90) * 3.14159 / 180) - (center.x + offx) * sin(mounted->get_rotation_y() * 3.14159 / 180); return pos.z + (center.z + offz) * sin((get_rotation_y() + 90) * 3.14159 / 180) - (center.x + offx) * sin(get_rotation_y() * 3.14159 / 180); }
	GLfloat* va() { return vertexArray; }
	std::vector< glm::vec3 > ver() { return vertices; }
	GLuint loadBMP_custom(const char* imagepath);
	void get_relative_position(float& x, float& y, float& z, float radius, float xrot, float zrot);
};

