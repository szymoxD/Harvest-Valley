#include "Obiekt.h"
#include <iostream>

Obiekt::Obiekt(const char* path, float scale, const Obiekt* parent, float rx, float ry, float rz, float px, float py, float pz, float cx, float cy, float cz) {

	if_loaded = loadOBJ(path, scale);
	pos = glm::vec3(px, py, pz);
	rot = glm::vec3(rx, ry, rz);
	this->scale = scale;
	mounted = parent;
	center.x = cx * scale;
	center.y = cy * scale;
	center.z = cz * scale;
	textureID = 0;
	has_texture = false;
}
bool Obiekt::loadOBJ(const char* path, float scale) {

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;


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
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	fclose(file);
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		vertex *= scale;
		vertices.push_back(vertex);

		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		uvs.push_back(uv);

		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex - 1];
		normals.push_back(normal);
	}
	vertexArray = new GLfloat[vertices.size() * 3];
	// Copy the vertex data into the vertex array
	for (int i = 0; i < vertices.size(); i++) {
		vertexArray[i * 3] = vertices[i].x;
		vertexArray[i * 3 + 1] = vertices[i].y;
		vertexArray[i * 3 + 2] = vertices[i].z;
	}
	return true;
}

Obiekt::~Obiekt() {
	delete[] vertexArray;
}

void Obiekt::draw() {

	glVertexPointer(3, GL_FLOAT, 0, vertexArray);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPushMatrix();
	if (mounted == nullptr) {
		glTranslatef(pos.x, pos.y, pos.z);			//wokol punktu polozenia
		glRotatef(rot.y, 0.0f, 1.0f, 0.0f); // obrót wokó³ osi y
		glRotatef(rot.x, 1.0f, 0.0f, 0.0f); // obrót wokó³ osi x
		glRotatef(rot.z, 0.0f, 0.0f, 1.0f); // obrót wokó³ osi z
	}
	else {
		glTranslatef(mounted->get_position_x(), mounted->get_position_y(), mounted->get_position_z());			//wokol mounted
		glRotatef(mounted->get_rotation_y(), 0.0f, 1.0f, 0.0f); // obrót wokó³ osi y
		glRotatef(mounted->get_rotation_x(), 1.0f, 0.0f, 0.0f); // obrót wokó³ osi x
		glRotatef(mounted->get_rotation_z(), 0.0f, 0.0f, 1.0f); // obrót wokó³ osi z

		glTranslatef(center.x, center.y, center.z);		//wokol wlasnej osi
		glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
		glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
		glRotatef(rot.z, 0.0f, 0.0f, 1.0f);
		glTranslatef(-center.x, -center.y, -center.z);
	}
	if (has_texture) {
		glColor3f(1.0, 1.0, 1.0);
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		// Powi¹zanie tekstury z ID
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Wskazanie atrybutów tekstury
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs[0]);

		
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		// Wy³¹czenie tekstur
		glDisable(GL_TEXTURE_2D);
	}
	else {

		//glColor3f(1.0, 1.0, 1.0);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}	

	glPopMatrix();
}

void Obiekt::mount(const Obiekt& ob) {
	if (abs(pos.x - ob.get_position_x()) < 10 && abs(pos.y - ob.get_position_y()) < 10 && abs(pos.z - ob.get_position_z()) < 10) {
		mounted = &ob;
		set_position(mounted->get_position_x(), mounted->get_position_y(), mounted->get_position_z());
		set_rotate(0, 0, 0);
	}
}
void Obiekt::unmount() {
	set_position(mounted->get_position_x(), mounted->get_position_y(), mounted->get_position_z());
	set_rotate(mounted->get_rotation_x(), mounted->get_rotation_y(), mounted->get_rotation_z());
	mounted = nullptr;
}

void Obiekt::set_rotate(float x, float y, float z) {
	rot.x = x;
	rot.y = y;
	rot.z = z;
	normalize_rotation();
}
void Obiekt::rotate(float x, float y, float z) {
	rot.x += x;
	rot.y += y;
	rot.z += z;
	normalize_rotation();
}
void Obiekt::set_position(float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}
void Obiekt::move(float x, float y, float z) {
	pos.x += x;
	pos.y += y;
	pos.z += z;
}
void Obiekt::normalize_rotation() {
	rot.x = rot.x >= 360.0f ? (rot.x - 360) : (rot.x < 0.0f ? (rot.x + 360) : (rot.x));
	rot.y = rot.y >= 360.0f ? (rot.y - 360) : (rot.y < 0.0f ? (rot.y + 360) : (rot.y));
	rot.z = rot.z >= 360.0f ? (rot.z - 360) : (rot.z < 0.0f ? (rot.z + 360) : (rot.z));
}

GLuint Obiekt::loadBMP_custom(const char* imagepath) {
	has_texture = true;
	// Wczytanie pliku BMP i utworzenie tekstury

	unsigned char header[54];
	unsigned int dataPos, width, height, imageSize;
	unsigned char* data;

	// Otwarcie pliku BMP
	FILE* file = fopen(imagepath, "rb");
	if (!file) {
		printf("Image could not be opened\n");
		return 0;
	}

	// Weryfikacja formatu pliku BMP
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Pobranie informacji o danych
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Poprawa wartoœci niektórych parametrów, jeœli s¹ nieprawid³owe
	if (imageSize == 0) {
		imageSize = width * height * 3;
	}
	if (dataPos == 0) {
		dataPos = 54;
	}

	// Alokacja pamiêci dla danych obrazu
	data = new unsigned char[imageSize];

	// Wczytanie danych obrazu
	fread(data, 1, imageSize, file);

	// Zamkniêcie pliku
	fclose(file);

	// Generowanie ID tekstury
	glGenTextures(1, &textureID);

	// Powi¹zanie tekstury z ID
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Ustawienie parametrów tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


	// Przypisanie obrazu tekstury do OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	// Zwolnienie pamiêci zajmowanej przez dane obrazu
	delete[] data;

	// Zwrócenie ID tekstury
	return textureID;
}
Obiekt::Obiekt(const Obiekt& ob) {
	vertices = ob.vertices;
	uvs = ob.uvs;
	normals = ob.normals;
	textureID = ob.textureID;
	pos = ob.pos;
	rot = ob.rot;
	center = ob.center;
	delete [] vertexArray;
	vertexArray= new GLfloat[ob.vertices.size() * 3];
	for (int i = 0; i < ob.vertices.size() * 3; i++)
		vertexArray[i] = ob.vertexArray[i];
	delete mounted;
	mounted = ob.mounted;
	if_loaded = ob.if_loaded;
	has_texture = ob.has_texture;
	scale = ob.scale;
}