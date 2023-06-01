#include "Pole.h"
Pole::Pole(Teren* teren_v, float max_lifespan_v) :max_lifespan(max_lifespan_v) {
	teren = teren_v;
}
void Pole::plow(int const* wierzcholki_v) {
	if (current < 1000) {
		for (int i = 0; i < current; i++) {
			if (wierzcholki[i][0] == wierzcholki_v[0] && wierzcholki[i][1] == wierzcholki_v[1] && wierzcholki[i][2] == wierzcholki_v[2] && wierzcholki[i][3] == wierzcholki_v[3])
				return;
			float mini= (*teren)[wierzcholki_v[0]].y, maks= (*teren)[wierzcholki_v[0]].y;
			for (int i = 0; i < 4; i++) {
				if (mini > (*teren)[wierzcholki_v[i]].y)
					mini = (*teren)[wierzcholki_v[i]].y;
				if (maks < (*teren)[wierzcholki_v[i]].y)
					maks = (*teren)[wierzcholki_v[i]].y;
			}
			if (maks - mini > 10)
				return;
		}
		wierzcholki[current][0] = wierzcholki_v[0];
		wierzcholki[current][1] = wierzcholki_v[1];
		wierzcholki[current][2] = wierzcholki_v[2];
		wierzcholki[current][3] = wierzcholki_v[3];
		lifespan[current] = max_lifespan;
		current++;
		delete[] wierzcholki_v;
	}
}
void Pole::update(float elapsed) {
	for (int i = 0; i < current; i++) {
		if (lifespan[i] > 0) {
			lifespan[i] -= elapsed;
		}
		if (lifespan[i] <= 0) {
			for (int j = i; j < current - 1; j++) {
				wierzcholki[j][0] = wierzcholki[j + 1][0];
				wierzcholki[j][1] = wierzcholki[j + 1][1];
				wierzcholki[j][2] = wierzcholki[j + 1][2];
				wierzcholki[j][3] = wierzcholki[j + 1][3];
				lifespan[j] = lifespan[j + 1];
			}
			current--;
		}
	}
}
void Pole::draw() {
	for (int i = 0; i < current; i++) {
		glColor3f(0.28f, 0.13f, 0.02f);
		//glColor3f(1.0f,1.0f,1.0f);

		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f((*teren)[wierzcholki[i][0]].x, (*teren)[wierzcholki[i][0]].y, (*teren)[wierzcholki[i][0]].z);
		glTexCoord2f(0, 1);
		glVertex3f((*teren)[wierzcholki[i][1]].x, (*teren)[wierzcholki[i][1]].y, (*teren)[wierzcholki[i][1]].z);
		glTexCoord2f(1, 0);
		glVertex3f((*teren)[wierzcholki[i][3]].x, (*teren)[wierzcholki[i][3]].y, (*teren)[wierzcholki[i][3]].z);
		glTexCoord2f(1, 1);
		glVertex3f((*teren)[wierzcholki[i][2]].x, (*teren)[wierzcholki[i][2]].y, (*teren)[wierzcholki[i][2]].z);
		glEnd();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);

		
	}

}
GLuint Pole::loadBMP_custom(const char* imagepath) {
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