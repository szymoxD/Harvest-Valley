#include "Pole.h"
Pole::Pole(Teren* teren_v, float max_lifespan_v) :max_lifespan(max_lifespan_v) {
	teren = teren_v;
}
float Pole::scale = 75;
Obiekt Pole::grass_s = Obiekt("../obj/plant_l.obj", scale);
Obiekt Pole::grass_m = Obiekt("../obj/plant_m.obj", scale);
Obiekt Pole::grass_h = Obiekt("../obj/plant_h.obj", scale);


bool Pole::plow(int const* wierzcholki_v) {
	if (current < 1000) {
		for (int i = 0; i < current; i++) {
			if (wierzcholki[i][0] == wierzcholki_v[0] && wierzcholki[i][1] == wierzcholki_v[1] && wierzcholki[i][2] == wierzcholki_v[2] && wierzcholki[i][3] == wierzcholki_v[3])
				return true;
			float mini = (*teren)[wierzcholki_v[0]].y, maks = (*teren)[wierzcholki_v[0]].y;
			for (int i = 0; i < 4; i++) {
				if (mini > (*teren)[wierzcholki_v[i]].y)
					mini = (*teren)[wierzcholki_v[i]].y;
				if (maks < (*teren)[wierzcholki_v[i]].y)
					maks = (*teren)[wierzcholki_v[i]].y;
			}
			if (maks - mini > 10)
				return false;
		}
		wierzcholki[current][0] = wierzcholki_v[0];
		wierzcholki[current][1] = wierzcholki_v[1];
		wierzcholki[current][2] = wierzcholki_v[2];
		wierzcholki[current][3] = wierzcholki_v[3];
		lifespan[current] = max_lifespan;
		plown[current] = false;
		current++;
		delete[] wierzcholki_v;
		return true;
	}
	return false;
}
bool Pole::mow(int const* wierzcholki_v) {
	int pole = -1;
	if (current < 1000) {
		for (int i = 0; i < current; i++) {
			if (wierzcholki[i][0] == wierzcholki_v[0] && wierzcholki[i][1] == wierzcholki_v[1] && wierzcholki[i][2] == wierzcholki_v[2] && wierzcholki[i][3] == wierzcholki_v[3]) {
				pole = i;
			}
		}
		if (pole >= 0) {
			float age = 1 - (lifespan[pole] / max_lifespan);
			if (age >= 0.9) {
				lifespan[pole] = 0;
				delete[] wierzcholki_v;
				plown[pole] = true;
				return true;
			}
		}
	}
	return false;
}

void Pole::update(float elapsed) {
	for (int i = 0; i < current; i++) {
		if (lifespan[i] > 0) {
			lifespan[i] -= elapsed;
		}
		if (plown[i]) {
			for (int j = i; j < current - 1; j++) {
				wierzcholki[j][0] = wierzcholki[j + 1][0];
				wierzcholki[j][1] = wierzcholki[j + 1][1];
				wierzcholki[j][2] = wierzcholki[j + 1][2];
				wierzcholki[j][3] = wierzcholki[j + 1][3];
				lifespan[j] = lifespan[j + 1];
				plown[j] = plown[j + 1];
			}
			current--;
		}
	}
}
void Pole::draw() {
	for (int i = 0; i < current; i++) {
		float age = 1 - (lifespan[i] / max_lifespan);  //0 - new,  1- old 

		glColor3f(0.28f - 0.20 * age, 0.13f + 0.8 * age, 0.02f - 0.01 * age);
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
		glColor3f(0.1f + 0.7f * age, 0.71f, 0.04f);
		Obiekt* trawa;
		if (age < 0.5)
			trawa = &grass_s;
		else if (age < 0.9)
			trawa = &grass_m;
		else
			trawa = &grass_h;
		trawa->set_position(((*teren)[wierzcholki[i][1]].x + (*teren)[wierzcholki[i][2]].x) / 2, ((*teren)[wierzcholki[i][0]].y+ (*teren)[wierzcholki[i][2]].y)/2,((*teren)[wierzcholki[i][0]].z+ (*teren)[wierzcholki[i][2]].z)/2);
		trawa->draw();
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

	// Poprawa warto�ci niekt�rych parametr�w, je�li s� nieprawid�owe
	if (imageSize == 0) {
		imageSize = width * height * 3;
	}
	if (dataPos == 0) {
		dataPos = 54;
	}

	// Alokacja pami�ci dla danych obrazu
	data = new unsigned char[imageSize];

	// Wczytanie danych obrazu
	fread(data, 1, imageSize, file);

	// Zamkni�cie pliku
	fclose(file);

	// Generowanie ID tekstury
	glGenTextures(1, &textureID);

	// Powi�zanie tekstury z ID
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Ustawienie parametr�w tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


	// Przypisanie obrazu tekstury do OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	// Zwolnienie pami�ci zajmowanej przez dane obrazu
	delete[] data;

	// Zwr�cenie ID tekstury
	return textureID;
}