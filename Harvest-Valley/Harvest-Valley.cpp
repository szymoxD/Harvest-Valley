﻿// Gl_template.c
//Wyłšczanie błędów przed "fopen"
#define  _CRT_SECURE_NO_WARNINGS

// Ładowanie bibliotek:
#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif

// Ustalanie trybu tekstowego:
#ifdef _MSC_VER        // Check if MS Visual C compiler
#   ifndef _MBCS
#      define _MBCS    // Uses Multi-byte character set
#   endif
#   ifdef _UNICODE     // Not using Unicode character set
#      undef _UNICODE 
#   endif
#   ifdef UNICODE
#      undef UNICODE 
#   endif
#endif

#include <windows.h>            // Window defines
#include <gl\gl.h>              // OpenGL
#include <gl\glu.h>             // GLU library
#include "glm\glm.hpp"			// GLM LIB
#include <math.h>				// Define for sqrt
#include <stdio.h>
#include "resource.h"           // About box resource identifiers.
#include<vector>				// vertices storage
#include <iostream>				//
#include "Obiekt.h"
#include "Teren.h"
#include "Keyboard.h"
#include "pole.h"
#include "particle.h"
#include <ctime>
#include "Collision.h"
#include <map>
#include "tekst.h"
#include FT_COLOR_H

#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)
#define BITMAP_ID 0x4D42		// identyfikator formatu BMP
#define GL_PI 3.14159

// Color Palette handle
HPALETTE hPalette = NULL;

// Application name and instance storeage
static LPCTSTR lpszAppName = "GL Template";
static HINSTANCE hInstance;

// Rotation amounts
static GLfloat xRot = -30.0f;
static GLfloat yRot = 0.0f;

static GLsizei lastHeight;
static GLsizei lastWidth;

// Opis tekstury
BITMAPINFOHEADER	bitmapInfoHeader;	// nagłówek obrazu
unsigned char* bitmapData;			// dane tekstury
unsigned int		texture[2];			// obiekt tekstury


// Declaration for Window procedure
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);

//deklaracje
float scale = 20.0f;

Collision collision;
Obiekt szopa("../obj/szopa.obj", scale, nullptr, 0, 0, 0, 0, 0, 0);
Obiekt ursus("../obj/tractor.obj", scale, nullptr, 0, 0, 0, 0, 0, 0);
Obiekt plug("../obj/plug.obj", scale, &ursus, 0, 0, 0, 0, 0, 0, 0, 1.2988, -0.772357);
Obiekt kosiarka("../obj/mower_body.obj", scale, nullptr, 0, 0, 0, -100, 0, 0, 0, 1.2988, -0.772357);
Obiekt ostrze("../obj/mower_blade.obj", scale, nullptr, 0, 0, 0, -100, 0, 0, 0, 1.2988, -0.772357);
Obiekt kolotl("../obj/kolo_duze_l.obj", scale, &ursus, 0, 0, 0, 0, 0, 0, 1.02619, 0.916822, 0);
Obiekt kolotp("../obj/kolo_duze_p.obj", scale, &ursus, 0, 0, 0, 0, 0, 0, -1.02619, 0.916822, 0);
Obiekt kolopl("../obj/kolo_male_l.obj", scale, &ursus, 0, 0, 0, 0, 0, 0, 0.914969, 0.42519, 2.69381); //origin z blendera dopisany
Obiekt kolopp("../obj/kolo_male_p.obj", scale, &ursus, 0, 0, 0, 0, 0, 0, -0.914969, 0.42519, 2.69381);   //x=x,  z=-y 
Teren terrain("../obj/terrain.obj", scale, nullptr, 0, 0, 0, 0, -5, 0);
Obiekt ursus_pokaz("../obj/full_tractor.obj", scale, nullptr, 0, 0, 0, -80, 0, 0);

Obiekt tree("../obj/tree.obj", scale, nullptr, 0, 0, 0, 2000, 0, 2000);
Obiekt tree2("../obj/tree.obj", scale, nullptr, 0, 0, 0, 500, 0, -1000);
Obiekt tree3("../obj/tree.obj", scale, nullptr, 0, 0, 0, -1500, 0, 0);
Pole pole(&terrain);
const int gestosc = 15000;
const int gestosc_ziemi = 100;
particle dym[gestosc];
particle ziemia[gestosc_ziemi];
int fps = 60;

bool quit = false;
bool plowing = false;
bool firstcam = true;
float wheel_angle = 20;
float speed = 0;
float money = 0;
float moneymultiplier = 1;
float maxspeed = 250;
float powerfactor = 1;  //zwieksza acc, plow acc, maxplow speed, spowolnienie pod górkę
float maxplowspeedfactor = 0.4f + 0.6f * (1.0f - 1.0f / powerfactor);  //od 0,4 do 1 dla powerfactor od 1 do niesk
float minanglespeed = 1.0f - 1.0f / powerfactor; //pow 1 - 45 deg=0 ruchu      0 min  1 max
float acceleration = 50 + 15 * (powerfactor - 1); //od 50 do niesk
float plowfactor = 0.5 + 0.5f * (1.0f - 1.0f / powerfactor);   //od 0,5 do 1 dla powerfactor od 1 do niesk

float wheelspan = 0.0;  //rozstaw kół wzdłóż
float pipe_distance;  //odl wydechu
float test = 0;
float axelspan = 0.0;	//rozstaw kół w szerz
float angle;
Keyboard keyboard;

clock_t delay = 1.0f / (float)fps * CLOCKS_PER_SEC;
clock_t last_refresh;
float elapsed;


std::map<char, znak> znaki;
FT_Library ft;
FT_Face face;

tekst speedtext("",0.02,0.07);
tekst moneytext("",0.02, 0.97);
tekst powertext("[1] UPG PWR",0.75, 0.07);
tekst poweruptext("", 0.75, 0.13);
tekst maxspeedtext("[2] UPG SPD", 0.75, 0.19);
tekst maxspeeduptext("",0.75,0.25);
tekst moneymulttext("[3] UPG GAIN",0.75,0.31);
tekst moneymultuptext("",0.75,0.37);

int powercost = 50;
int maxspeedcost = 50;
int moneymultcost = 50;


void acceleratevehicle(float& currentSpeed, float accelerationRate, float maxSpeed, float deltaTime, float vehicleAngle, bool plow, int power) {
	float plowMultiplier = (plow) ? plowfactor : 1.0f;
	float anglefactor = sin(vehicleAngle * GL_PI / 180);   //1 - w dół, -1 - pod górę

	if (currentSpeed >= 0) {
		if (anglefactor < 0) {
			anglefactor += -anglefactor * minanglespeed;
		}
		anglefactor += 1;  //0 do góry, 2x w dół
		maxSpeed *= anglefactor;
		if (plow && currentSpeed >= maxplowspeedfactor * maxSpeed) {
			currentSpeed -= accelerationRate * deltaTime * 1 / plowMultiplier * 2.0f * (2 - anglefactor);
			if (power == -1)
				currentSpeed -= accelerationRate * deltaTime * 1 / plowMultiplier * 2.0f * (2 - anglefactor);
			else if (power == 0)
				currentSpeed -= accelerationRate * deltaTime * 1 / plowMultiplier * (2 - anglefactor);
		}
		else if (power == 1) {
			currentSpeed += accelerationRate * deltaTime * plowMultiplier * anglefactor;
			if (currentSpeed >= maxSpeed)
			{
				currentSpeed = maxSpeed;
			}
		}
		else if (power == -1)
			currentSpeed -= accelerationRate * deltaTime * 1 / plowMultiplier * 2.0f * (2 - anglefactor);
		else if (speed > 0) {
			currentSpeed -= accelerationRate * deltaTime * 1 / plowMultiplier * (2 - anglefactor);
			if (currentSpeed < 0)
				currentSpeed = 0;
		}
	}
	else {
		if (anglefactor > 0) {
			anglefactor += -anglefactor * minanglespeed;
		}
		anglefactor += 1;  //0 do góry, 2x w dół
		maxSpeed *= (2 - anglefactor);
		if (plow && currentSpeed <= -maxplowspeedfactor * maxSpeed / 2.0f) {
			currentSpeed += accelerationRate * deltaTime * 1 / plowMultiplier * 2.0f * anglefactor;
			if (power == 1)
				currentSpeed += accelerationRate * deltaTime * 1 / plowMultiplier * 2.0f * anglefactor;
			else if (power == 0)
				currentSpeed += accelerationRate * deltaTime * 1 / plowMultiplier * anglefactor;
		}
		else if (power == -1) {
			currentSpeed -= accelerationRate * deltaTime * plowMultiplier * (2 - anglefactor);
			if (currentSpeed <= -maxSpeed / 2.0f)
			{
				currentSpeed = -maxSpeed / 2.0f;
			}
		}
		else if (power == 1)
			currentSpeed += accelerationRate * deltaTime * 1 / plowMultiplier * 2.0f * anglefactor;
		else if (speed < 0) {
			currentSpeed += accelerationRate * deltaTime * 1 / plowMultiplier * anglefactor;
			if (currentSpeed > 0)
				currentSpeed = 0;
		}
	}
}

// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
{
	float length;

	// Calculate the length of the vector		
	length = (float)sqrt((vector[0] * vector[0]) +
		(vector[1] * vector[1]) +
		(vector[2] * vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if (length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
{
	float v1[3], v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
}



// Change viewing volume and viewport.  Called when window is resized
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat nRange = 100.0f;
	GLfloat fAspect;
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	lastWidth = w;
	lastHeight = h;

	fAspect = (GLfloat)w / (GLfloat)h;
	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	/*if (w <= h)
		glOrtho(-nRange, nRange, -nRange * h / w, nRange * h / w, -nRange, nRange);
	else
		glOrtho(-nRange * w / h, nRange * w / h, -nRange, nRange, -nRange, nRange);*/

		// Establish perspective: 

	gluPerspective(60.0f, fAspect, 1.0, 5000);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC()
{
	// Light values and coordinates
	//GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	//GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	//GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	//GLfloat	 lightPos[] = { 0.0f, 150.0f, 150.0f, 1.0f };
	//GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	//glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Enable lighting
	//glEnable(GL_LIGHTING);

	// Setup and enable light 0
	//glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	//glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	//glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	//glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	//glEnable(GL_LIGHT0);

	// Enable color tracking
	//glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	//glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
	//glMateriali(GL_FRONT,GL_SHININESS,128);


	// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Black brush
	glColor3f(0.0, 0.0, 0.0);
}



// LoadBitmapFile
// opis: ładuje mapę bitową z pliku i zwraca jej adres.
//       Wypełnia strukturę nagłówka.
//	 Nie obsługuje map 8-bitowych.
unsigned char* LoadBitmapFile(char* filename, BITMAPINFOHEADER* bitmapInfoHeader)
{
	FILE* filePtr;							// wskaźnik pozycji pliku
	BITMAPFILEHEADER	bitmapFileHeader;		// nagłówek pliku
	unsigned char* bitmapImage;			// dane obrazu
	int					imageIdx = 0;		// licznik pikseli
	unsigned char		tempRGB;				// zmienna zamiany składowych

	// otwiera plik w trybie "read binary"
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// wczytuje nagłówek pliku
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// sprawdza, czy jest to plik formatu BMP
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// wczytuje nagłówek obrazu
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// ustawia wskaźnik pozycji pliku na początku danych obrazu
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// przydziela pamięć buforowi obrazu
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// sprawdza, czy udało się przydzielić pamięć
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// wczytuje dane obrazu
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// sprawdza, czy dane zostały wczytane
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// zamienia miejscami składowe R i B 
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// zamyka plik i zwraca wskaźnik bufora zawierającego wczytany obraz
	fclose(filePtr);
	return bitmapImage;
}
void cross(float x, float z, float size)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow

		GLfloat sa[3] = { x - size,0.0f,z };
		GLfloat sb[3] = { x + size,0.0f,z };
		GLfloat sc[3] = { x,0.0f,z + size };
		GLfloat sd[3] = { x,0.0f,z - size };
		GLfloat se[3] = { x - size,size,z };
		GLfloat sf[3] = { x + size,size,z };
		GLfloat sg[3] = { x,size,z + size };
		GLfloat sh[3] = { x,size,z - size };

		// Sciany skladowe
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(se);
		glEnd();

		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sc);
		glVertex3fv(sd);
		glVertex3fv(sh);
		glVertex3fv(sg);
		glEnd();
	}
}
void Update() {
	if (keyboard.Pressed('W') != -1) {
		ursus.move(speed * elapsed * sin(ursus.get_rotation_y() * GL_PI / 180), 0, speed * elapsed * sin((ursus.get_rotation_y() + 90) * GL_PI / 180));
		if (collision.collide({ kolotp.get_absolute_position_x(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotp.get_absolute_position_y(),kolotp.get_absolute_position_z(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),kolotl.get_absolute_position_x(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotl.get_absolute_position_y(),kolotl.get_absolute_position_z(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),
								kolopp.get_absolute_position_x(-2,8), kolopp.get_absolute_position_y(),kolopp.get_absolute_position_z(-2,8), kolopl.get_absolute_position_x(2,8), kolopl.get_absolute_position_y(),kolopl.get_absolute_position_z(2,8) }, ursus.get_rotation_y())) {
			ursus.move(-speed * elapsed * sin(ursus.get_rotation_y() * GL_PI / 180), 0, -speed * elapsed * sin((ursus.get_rotation_y() + 90) * GL_PI / 180));
			speed = 0;
		}
		else {
			acceleratevehicle(speed, acceleration, maxspeed, elapsed, ursus.get_rotation_x(), plowing, 1);
			//plug.move(50.0f * elapsed * sin(ursus.get_rotation_y() * GL_PI / 180), 0, 50.0f * elapsed * sin((ursus.get_rotation_y() + 90) * GL_PI / 180));
			kolopp.rotate_x(speed * elapsed / (kolopp.get_center_y() * scale) * 360);
			kolopl.rotate_x(speed * elapsed / (kolopl.get_center_y() * scale) * 360);
			kolotp.rotate_x(speed * elapsed / (kolotp.get_center_y() * scale) * 360);
			kolotl.rotate_x(speed * elapsed / (kolotl.get_center_y() * scale) * 360);

			float xrota = atan(25.0f / 60.0f) * 180 / GL_PI;
			float zrota = atan(-14.0f / 60.0f) * 180 / GL_PI;
			float px, py, pz;
			ursus.get_relative_position(px, py, pz, pipe_distance, xrota, zrota);
			for (int i = 0; i < gestosc; i++)
				if (dym[i].if_dead())
					dym[i] = particle(px, py, pz, true, false);
		}

	}
	else if (keyboard.Pressed('S') != -1) {
		ursus.move(speed * elapsed * sin(ursus.get_rotation_y() * GL_PI / 180), 0, speed * elapsed * sin((ursus.get_rotation_y() + 90) * GL_PI / 180));
		if (collision.collide({ kolotp.get_absolute_position_x(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotp.get_absolute_position_y(),kolotp.get_absolute_position_z(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),kolotl.get_absolute_position_x(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotl.get_absolute_position_y(),kolotl.get_absolute_position_z(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),
								kolopp.get_absolute_position_x(-2,8), kolopp.get_absolute_position_y(),kolopp.get_absolute_position_z(-2,8), kolopl.get_absolute_position_x(2,8), kolopl.get_absolute_position_y(),kolopl.get_absolute_position_z(2,8) }, ursus.get_rotation_y())) {
			ursus.move(-speed * elapsed * sin(ursus.get_rotation_y() * GL_PI / 180), 0, -speed * elapsed * sin((ursus.get_rotation_y() + 90) * GL_PI / 180));
			speed = 0;
		}
		else {
			acceleratevehicle(speed, acceleration, maxspeed, elapsed, ursus.get_rotation_x(), plowing, -1);
			//plug.move(-50.0f * elapsed * sin(ursus.get_rotation_y() * GL_PI / 180), 0, -50.0f * elapsed * sin((ursus.get_rotation_y() + 90) * GL_PI / 180));
			kolopp.rotate_x(speed * elapsed / (kolopp.get_center_y() * scale) * 360);
			kolopl.rotate_x(speed * elapsed / (kolopl.get_center_y() * scale) * 360);
			kolotp.rotate_x(speed * elapsed / (kolotp.get_center_y() * scale) * 360);
			kolotl.rotate_x(speed * elapsed / (kolotl.get_center_y() * scale) * 360);
			float xrota = atan(25.0f / 60.0f) * 180 / GL_PI;
			float zrota = atan(-14.0f / 60.0f) * 180 / GL_PI;
			float px, py, pz;
			ursus.get_relative_position(px, py, pz, pipe_distance, xrota, zrota);
			for (int i = 0; i < gestosc; i++)
				if (dym[i].if_dead())
					dym[i] = particle(px, py, pz, true, false);
		}
	}
	else {
		ursus.move(speed * elapsed * sin(ursus.get_rotation_y() * GL_PI / 180), 0, speed * elapsed * sin((ursus.get_rotation_y() + 90) * GL_PI / 180));
		if (collision.collide({ kolotp.get_absolute_position_x(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotp.get_absolute_position_y(),kolotp.get_absolute_position_z(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),kolotl.get_absolute_position_x(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotl.get_absolute_position_y(),kolotl.get_absolute_position_z(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),
								kolopp.get_absolute_position_x(-2,8), kolopp.get_absolute_position_y(),kolopp.get_absolute_position_z(-2,8), kolopl.get_absolute_position_x(2,8), kolopl.get_absolute_position_y(),kolopl.get_absolute_position_z(2,8) }, ursus.get_rotation_y())) {
			ursus.move(-speed * elapsed * sin(ursus.get_rotation_y() * GL_PI / 180), 0, -speed * elapsed * sin((ursus.get_rotation_y() + 90) * GL_PI / 180));
			speed = 0;
		}
		else {
			acceleratevehicle(speed, acceleration, maxspeed, elapsed, ursus.get_rotation_x(), plowing, 0);
			kolopp.rotate_x(speed * elapsed / (kolopp.get_center_y() * scale) * 360);
			kolopl.rotate_x(speed * elapsed / (kolopl.get_center_y() * scale) * 360);
			kolotp.rotate_x(speed * elapsed / (kolotp.get_center_y() * scale) * 360);
			kolotl.rotate_x(speed * elapsed / (kolotl.get_center_y() * scale) * 360);
		}
	}
	if (!plug.if_unmounted() && plug.get_rotation_x() < 5 && speed > 5) {
		float xrot = -80;// pole
		float zrot = 0;
		float dist = 2.2 * scale;
		float px, py, pz;
		ursus.get_relative_position(px, py, pz, dist, xrot, zrot);
		if (pole.plow(terrain.get_indexes(px, pz))) {
			for (int i = 0; i < gestosc_ziemi; i++)
				if (ziemia[i].if_dead())
					ziemia[i] = particle(px, py, pz, false, false, 150, -750, 0.3, 5, 0.28f, 0.13f, 0.02f);
			plowing = true;
		}
		else
			plowing = false;
	}
	else if (!kosiarka.if_unmounted() && kosiarka.get_rotation_x() < 5 && speed > 5) {
		float xrot = -80;// pole
		float zrot = 0;
		float dist = 2.2 * scale;
		float px, py, pz;
		ursus.get_relative_position(px, py, pz, dist, xrot, zrot);
		if (pole.mow(terrain.get_indexes(px, pz))) {
			for (int i = 0; i < gestosc_ziemi; i++)
				if (ziemia[i].if_dead())
					ziemia[i] = particle(px, py, pz, false, false, 150, -750, 0.3, 5, 0.01f, 0.33f, 0.02f);
			plowing = true;
			money += 10*moneymultiplier;
		}
		else
			plowing = false;
	}
	else
		plowing = false;
	if (keyboard.Pressed('A') != -1) {
		kolopp.set_rotate_y(wheel_angle);
		kolopl.set_rotate_y(wheel_angle);
		ursus.rotate(0, speed * elapsed / (2 * GL_PI * wheelspan / sin(wheel_angle * GL_PI / 180)) * 360, 0); //obrót po okręgu skrętu
		if (collision.collide({ kolotp.get_absolute_position_x(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotp.get_absolute_position_y(),kolotp.get_absolute_position_z(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),kolotl.get_absolute_position_x(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotl.get_absolute_position_y(),kolotl.get_absolute_position_z(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),
								kolopp.get_absolute_position_x(-2,8), kolopp.get_absolute_position_y(),kolopp.get_absolute_position_z(-2,8), kolopl.get_absolute_position_x(2,8), kolopl.get_absolute_position_y(),kolopl.get_absolute_position_z(2,8) }, ursus.get_rotation_y()))
			ursus.rotate(0, -speed * elapsed / (2 * GL_PI * wheelspan / sin(wheel_angle * GL_PI / 180)) * 360, 0); //obrót po okręgu skrętu

	}
	else if (keyboard.Pressed('D') != -1) {
		kolopp.set_rotate_y(-wheel_angle);
		kolopl.set_rotate_y(-wheel_angle);
		ursus.rotate(0, -speed * elapsed / (2 * GL_PI * wheelspan / sin(wheel_angle * GL_PI / 180)) * 360, 0);
		if (collision.collide({ kolotp.get_absolute_position_x(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotp.get_absolute_position_y(),kolotp.get_absolute_position_z(-7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),kolotl.get_absolute_position_x(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())), kolotl.get_absolute_position_y(),kolotl.get_absolute_position_z(7,-20 - 50 * (!plug.if_unmounted() || !kosiarka.if_unmounted())),
								kolopp.get_absolute_position_x(-2,8), kolopp.get_absolute_position_y(),kolopp.get_absolute_position_z(-2,8), kolopl.get_absolute_position_x(2,8), kolopl.get_absolute_position_y(),kolopl.get_absolute_position_z(2,8) }, ursus.get_rotation_y()))
			ursus.rotate(0, speed * elapsed / (2 * GL_PI * wheelspan / sin(wheel_angle * GL_PI / 180)) * 360, 0); //obrót po okręgu skrętu
	}
	else if (keyboard.Pressed('Q') != -1 && !keyboard.switched('Q')) {
		ursus.rotate(0, 90, 0);
		keyboard.block('Q');
	}
	else {
		kolopp.set_rotate_y(0);
		kolopl.set_rotate_y(0);
	}
	if (keyboard.Pressed('I') != -1) {
		if (!plug.if_unmounted()) {
			if (plug.get_rotation_x() < 30)
				plug.rotate_x(5);
		}
		else if (!kosiarka.if_unmounted()) {
			if (kosiarka.get_rotation_x() < 30) {
				kosiarka.rotate_x(5);
				ostrze.rotate_x(5);
			}
		}
	}
	else if (keyboard.Pressed('J') != -1) {
		if (!plug.if_unmounted()) {
			if (plug.get_rotation_x() > 0)
				plug.rotate_x(-5);
		}
		else if (!kosiarka.if_unmounted()) {
			if (kosiarka.get_rotation_x() > 0) {
				kosiarka.rotate_x(-5);
				ostrze.rotate_x(-5);
			}
		}
	}
	if (keyboard.Pressed('K') != -1) {
		if (!plug.if_unmounted()) {
			plug.unmount();
			collision.addcircle({ plug.get_absolute_position_x(0,-30), plug.get_absolute_position_y(), plug.get_absolute_position_z(0,-30), 30 });
		}
		else if (!kosiarka.if_unmounted()) {
			kosiarka.unmount();
			ostrze.unmount();
			collision.addcircle({ kosiarka.get_absolute_position_x(0,-30), kosiarka.get_absolute_position_y(), kosiarka.get_absolute_position_z(0,-30), 30 });
		}
	}
	else if (keyboard.Pressed('L') != -1) {
		if (plug.if_unmounted() && kosiarka.if_unmounted()) {
			if (plug.to_mount(ursus) && kosiarka.if_unmounted()) {
				collision.popcircle({ plug.get_absolute_position_x(0,-30), plug.get_absolute_position_y(), plug.get_absolute_position_z(0,-30), 30 });
				plug.mount(ursus);
			}
			if (kosiarka.to_mount(ursus) && plug.if_unmounted()) {
				collision.popcircle({ kosiarka.get_absolute_position_x(0,-30), kosiarka.get_absolute_position_y(), kosiarka.get_absolute_position_z(0,-30), 30 });
				kosiarka.mount(ursus);
				ostrze.mount(ursus);
			}
		}
	}
	else if (keyboard.Pressed('F') != -1 && !keyboard.switched('F')) {
		firstcam = !firstcam;
		xRot = -30;
		keyboard.block('F');
	}
	else if (keyboard.Pressed('1') != -1 && !keyboard.switched('1')) {
		if (money >= powercost) {
			money -= powercost;
			powerfactor += 0.5;
			maxplowspeedfactor = 0.4f + 0.6f * (1.0f - 1.0f / powerfactor);  //od 0,4 do 1 dla powerfactor od 1 do niesk
			minanglespeed = 1.0f - 1.0f / powerfactor; //pow 1 - 45 deg=0 ruchu      0 min  1 max
			acceleration = 50 + 15 * (powerfactor - 1); //od 50 do niesk
			plowfactor = 0.5 + 0.5f * (1.0f - 1.0f / powerfactor);   //od 0,5 do 1 dla powerfactor od 1 do niesk
			powercost *= 1.2;
		}
		keyboard.block('1');
	}
	else if (keyboard.Pressed('2') != -1 && !keyboard.switched('2')) {
		if (money >= maxspeedcost) {
			money -= maxspeedcost;
			maxspeed += 5;
			maxspeedcost *= 1.2;
		}
		keyboard.block('2');
	}
	else if (keyboard.Pressed('3') != -1 && !keyboard.switched('3')) {
		if (money >= moneymultcost) {
			money -= moneymultcost;
			moneymultiplier += 0.1;
			moneymultcost *= 1.2;
		}
		keyboard.block('3');
	}
	else if (keyboard.Pressed('P') != -1 && !keyboard.switched('P')) {
		money += 500;
		keyboard.block('P');
	}
	/*else if (keyboard.Pressed('T') != -1 && !keyboard.switched('T')) {  //teleport x +1000
		ursus.set_position(ursus.get_position_x() + 1000, ursus.get_position_y(), ursus.get_position_z());
		keyboard.block('T');
	}*/
	if (keyboard.Pressed(VK_UP) != -1) {
		if (xRot > -90)
			xRot -= 100.0f * elapsed;
	}
	else if (keyboard.Pressed(VK_DOWN) != -1) {
		if (xRot < 0)
			xRot += 100.0f * elapsed;
	}
	if (keyboard.Pressed(VK_LEFT) != -1)
		yRot += 100.0f * elapsed;
	else if (keyboard.Pressed(VK_RIGHT) != -1)
		yRot -= 100.0f * elapsed;
	xRot = (const int)xRot % 360;
	yRot = (const int)yRot % 360;

	if (keyboard.Pressed(VK_ESCAPE) != -1)
		quit = true;

	float ppheight = terrain.get_height(kolopp.get_absolute_position_x(), kolopp.get_absolute_position_z()) - kolopp.get_position_y();
	float plheight = terrain.get_height(kolopl.get_absolute_position_x(), kolopl.get_absolute_position_z()) - kolopl.get_position_y();
	float tpheight = terrain.get_height(kolotp.get_absolute_position_x(), kolotp.get_absolute_position_z()) - kolotp.get_position_y();
	float tlheight = terrain.get_height(kolotl.get_absolute_position_x(), kolotl.get_absolute_position_z()) - kolotl.get_position_y();
	float ursus_pos = terrain.get_height(ursus.get_absolute_position_x(), ursus.get_absolute_position_z());
	float ursus_rot_x = -(plheight + ppheight - tlheight - tpheight) / 2;
	float ursus_rot_z = -(tpheight + ppheight - tlheight - plheight) / 2;
	float ursus_rot_x_tan = atan(ursus_rot_x / wheelspan);
	float ursus_rot_z_tan = atan(ursus_rot_z / axelspan);//    //ursus_rot_x_tan * 180 / GL_PI   //-ursus_rot_z_tan * 180 / GL_PI
	ursus.set_rotate(ursus_rot_x_tan * 180 / GL_PI, ursus.get_rotation_y(), -ursus_rot_z_tan * 180 / GL_PI);  //przod tyl, 0, lewo prawo
	ursus.set_position(ursus.get_position_x(), ursus_pos, ursus.get_position_z());
	for (int i = 0; i < gestosc; i++) {
		dym[i].update(elapsed);
	}
	for (int i = 0; i < gestosc_ziemi; i++) {
		ziemia[i].update(elapsed);
	}
	pole.update(elapsed);
}
void RenderScene(void)
{
	//float normal[3];	// Storeage for calculated surface normal

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glPushMatrix();

	float camX;
	float camY;
	float camZ;
	// pozycja kamery
	if (firstcam) {
		ursus.get_relative_position(camX, camY, camZ, 55, 0, 0);
	}
	else {
		camX = ursus.get_position_x() - (125 + 60 * xRot / -90) * sin((ursus.get_rotation_y() + yRot) * GL_PI / 180);
		camY = ursus.get_position_y() + 50 - 125 * sin((ursus.get_rotation_x() + xRot) * GL_PI / 180);
		camZ = ursus.get_position_z() - (125 + 60 * xRot / -90) * cos((ursus.get_rotation_y() + yRot) * GL_PI / 180);
	}
	if (firstcam) {
		glRotatef(xRot + 40, 1.0f, 0.0f, 0.0f); // Obrót kamery względem osi X
		glRotatef(-yRot + 180, 0.0f, 1.0f, 0.0f); // Obrót kamery względem osi Y

		glRotatef(-ursus.get_rotation_z(), 0.0f, 0.0f, 1.0f); // Obrót kamery względem osi Z
		glRotatef(-ursus.get_rotation_x(), 1.0f, 0.0f, 0.0f); // Obrót kamery względem osi X
		glRotatef(-ursus.get_rotation_y(), 0.0f, 1.0f, 0.0f); // Obrót kamery względem osi Y		
	}
	else {
		glRotatef(-xRot / 4 + 20 + 5 * xRot / 90, 1.0f, 0.0f, 0.0f); // Obrót kamery względem osi X
		glRotatef(180 - yRot - ursus.get_rotation_y(), 0.0f, 1.0f, 0.0f); // Obrót kamery względem osi Y
	}
	// wzgledem traktora:
	glTranslatef(-camX, -camY, -camZ);

	/////////////////////////////////////////////////////////////////
	// MIEJSCE NA KOD OPENGL DO TWORZENIA WLASNYCH SCEN:		   //
	/////////////////////////////////////////////////////////////////
	//szescian();

	//Sposób na odróźnienie "przedniej" i "tylniej" ściany wielokąta:
	glPolygonMode(GL_BACK, GL_FILL);
	// Enable vertex attributes
	//ursus.set_position(ursus.get_position_x(), 0, ursus.get_position_z());
	glColor3f(0.5f, 0.5f, 0.5f);
	tree.draw();
	tree2.draw();
	tree3.draw();
	plug.draw();
	kosiarka.draw();
	ostrze.draw();
	//cross(plug.get_absolute_position_x(0, -30), plug.get_absolute_position_z(0, -30), 30);
	szopa.draw();
	glColor3f(0.4, 0.4, 0.4);
	//ursus_pokaz.draw();
	ursus.draw();
	glColor3f(0.3, 0.3, 0.3);
	kolotl.draw();
	kolotp.draw();
	kolopl.draw();
	//glTranslatef(20 * 0.914969, 0, 20 * 2.69381);
	//glRotatef(45, 0.0f, 1.0f, 0.0f);
	kolopp.draw();
	for (int i = 0; i < gestosc; i++)
		dym[i].draw();
	for (int i = 0; i < gestosc_ziemi; i++)
		ziemia[i].draw();
	pole.draw();
	//glRotatef(-45, 0.0f, 1.0f, 0.0f);
	//glTranslatef(-20 * 0.914969, 0,- 20 * 2.69381);

	glColor3f(0.2, 0.7, 0.2);
	terrain.draw();

	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	//Uzyskanie siatki:
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//Wyrysowanie prostokata:
	//glRectd(-10.0,-10.0,20.0,20.0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 1, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//add gui here
	char b[8] = {}, b2[8] = {};
	_itoa((int)abs(speed/5), b,10);
	std::string temp = b;
	speedtext.setValue("SPD: "+temp);
	speedtext.draw(znaki);
	speedtext.drawRect(0.015, 0.015, 0.21, 0.077);

	_itoa((int)money, b, 10);
	temp = b;
	moneytext.setValue("$ "+temp);
	moneytext.draw(znaki);
	speedtext.drawRect(0.015, 0.91, 0.3, 0.975);

	powertext.draw(znaki);
	_itoa(powercost, b, 10);
	temp = b;
	poweruptext.setValue("$ " + temp);
	poweruptext.draw(znaki);
	maxspeedtext.draw(znaki);
	_itoa(maxspeedcost, b, 10);
	temp = b;
	maxspeeduptext.setValue("$ " + temp);
	maxspeeduptext.draw(znaki);
	moneymulttext.draw(znaki);
	_itoa(moneymultcost, b, 10);
	temp = b;
	moneymultuptext.setValue("$ " + temp);
	moneymultuptext.draw(znaki);
	powertext.drawRect(0.74,0,0.997,0.39);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();




	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Flush drawing commands
	glFlush();
}


// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // Size of this structure
		1,                                                              // Version of this structure    
		PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,                       // Double buffered
		PFD_TYPE_RGBA,                          // RGBA Color mode
		24,                                     // Want 24bit color 
		0,0,0,0,0,0,                            // Not used to select mode
		0,0,                                    // Not used to select mode
		0,0,0,0,0,                              // Not used to select mode
		32,                                     // Size of depth buffer
		0,                                      // Not used to select mode
		0,                                      // Not used to select mode
		PFD_MAIN_PLANE,                         // Draw in main plane
		0,                                      // Not used to select mode
		0,0,0 };                                // Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE* pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange, GreenRange, BlueRange;
	// Range for each color entry (7,7,and 3)


// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if (!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) - 1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) - 1;

	// Loop through all the palette entries
	for (i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double)pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char)NULL;
	}


	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC, hRetPal, FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
}


// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE       hInst,
	HINSTANCE       hPrevInstance,
	LPSTR           lpCmdLine,
	int                     nCmdShow)
{
	MSG                     msg;            // Windows message structure
	WNDCLASS        wc;                     // Windows class structure
	HWND            hWnd;           // Storeage for window handle

	hInstance = hInst;

	// Register Window style
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// No need for background brush for OpenGL window
	wc.hbrBackground = NULL;

	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wc.lpszClassName = lpszAppName;

	// Register the window class
	if (RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,

		// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,

		// Window position and size
		50, 50,
		800, 600,
		NULL,
		NULL,
		hInstance,
		NULL);

	// If window was not created, quit
	if (hWnd == NULL)
		return FALSE;

	//obiekty


	// Display the window
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while (!quit) {
		elapsed = clock() - last_refresh;
		if (elapsed >= delay) {
			last_refresh = clock();
			elapsed = elapsed / 1000; //w sekundach
			Update();
			InvalidateRect(hWnd, NULL, FALSE);
			elapsed = 0;
		}
		while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}




// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam)
{
	static HGLRC hRC;               // Permenant Rendering context
	static HDC hDC;                 // Private GDI Device context
	char file[] = "Bitmapy\\checker.bmp";
	bool res;
	switch (message)
	{
		// Window creation, setup for OpenGL
	case WM_CREATE:
		// Store the device context
		hDC = GetDC(hWnd);

		// Select the pixel format
		SetDCPixelFormat(hDC);

		// Create palette if needed
		hPalette = GetOpenGLPalette(hDC);

		// Create the rendering context and make it current
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		SetupRC();
		glGenTextures(2, &texture[0]);                  // tworzy obiekt tekstury			

		// ładuje pierwszy obraz tekstury:

		bitmapData = LoadBitmapFile(file, &bitmapInfoHeader);

		glBindTexture(GL_TEXTURE_2D, texture[0]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ładuje drugi obraz tekstury:
		bitmapData = LoadBitmapFile(file, &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[1]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ustalenie sposobu mieszania tekstury z tłem
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		//tworzenie obj
		// Read our .obj file
		glEnableClientState(GL_VERTEX_ARRAY);
		szopa.loadBMP_custom("../obj/szopa_texture.bmp");
		plug.loadBMP_custom("../obj/plug_texture.bmp");
		terrain.loadBMP_custom("../obj/terrain_material.bmp");
		ursus.loadBMP_custom("../obj/tractor_texture.bmp");
		tree.loadBMP_custom("../obj/tree_texture.bmp");
		tree2.loadBMP_custom("../obj/tree_texture.bmp");
		tree3.loadBMP_custom("../obj/tree_texture.bmp");
		kolopp.loadBMP_custom("../obj/kola_texture.bmp");
		kolopl.loadBMP_custom("../obj/kola_texture.bmp");
		kolotp.loadBMP_custom("../obj/kola_texture.bmp");
		kolotl.loadBMP_custom("../obj/kola_texture.bmp");
		pole.loadBMP_custom("../obj/szopa_texture.bmp");
		kosiarka.loadBMP_custom("../obj/mower_texture.bmp");
		ostrze.loadBMP_custom("../obj/mower_texture.bmp");
		collision.addsquare({ szopa.get_position_x() - 7.14f * scale,0,szopa.get_position_z() + 4.23f * scale,szopa.get_position_x() - 6.94f * scale,0,szopa.get_position_z() + 4.23f * scale,
							  szopa.get_position_x() - 7.14f * scale,0,szopa.get_position_z() - 5.76f * scale,szopa.get_position_x() - 6.94f * scale,0,szopa.get_position_z() - 5.76f * scale });
		collision.addsquare({ szopa.get_position_x() + 3.05f * scale,0,szopa.get_position_z() + 4.23f * scale,szopa.get_position_x() + 2.85f * scale,0,szopa.get_position_z() + 4.23f * scale,
							  szopa.get_position_x() + 3.05f * scale,0,szopa.get_position_z() - 5.76f * scale,szopa.get_position_x() + 2.85f * scale,0,szopa.get_position_z() - 5.76f * scale });
		collision.addsquare({ szopa.get_position_x() - 7.04f * scale,0,szopa.get_position_z() - 5.66f * scale,szopa.get_position_x() - 7.04f * scale,0,szopa.get_position_z() - 5.86f * scale,
							  szopa.get_position_x() + 2.95f * scale,0,szopa.get_position_z() - 5.66f * scale,szopa.get_position_x() + 2.95f * scale,0,szopa.get_position_z() - 5.86f * scale });
		collision.addcircle({ tree.get_position_x(),0, tree.get_position_z(),9 * scale });
		collision.addcircle({ tree2.get_position_x(),0, tree2.get_position_z(),9 * scale });
		collision.addcircle({ tree3.get_position_x(),0, tree3.get_position_z(),9 * scale });

		wheelspan = kolopp.get_center_z() * sin((ursus.get_rotation_y() + 90) * 3.14159 / 180)
			+ kolotp.get_center_z() * sin((ursus.get_rotation_y() + 90) * 3.14159 / 180);//rozstaw w długości
		axelspan = -kolotl.get_center_x() * sin((ursus.get_rotation_y() + 90) * 3.14159 / 180);//rozstaw w szerokości kól
		pipe_distance = sqrt(14 * 14 + 60 * 60 + 25 * 25); //odl wydechu
		if (plug.if_unmounted()) {
			collision.addcircle({ plug.get_absolute_position_x(0,-30), plug.get_absolute_position_y(), plug.get_absolute_position_z(0,-30), 30 });
		}
		if (kosiarka.if_unmounted()) {
			collision.addcircle({ kosiarka.get_absolute_position_x(0,-30), kosiarka.get_absolute_position_y(), kosiarka.get_absolute_position_z(0,-30), 30 });
		}

		//font init
		if (FT_Init_FreeType(&ft))
			break;
		if (FT_New_Face(ft, "../fonts/cour.ttf", 0, &face))
			break;
		FT_Set_Pixel_Sizes(face, 0, 20);
		/*FT_Color color;
		color.alpha = 0;
		color.blue = 0;
		color.green = 0;
		color.red = 0;
		FT_Palette_Set_Foreground_Color(face, color);*/
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

		for (unsigned char c = 0; c < 128; c++)
		{
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			znak character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			znaki.insert(std::pair<char, znak>(c, character));
		}


		FT_Done_Face(face);
		FT_Done_FreeType(ft);


		break;

		// Window is being destroyed, cleanup
	case WM_DESTROY:

		// Draw the vertices
		quit = true;

		// Disable vertex attributes
		glDisableClientState(GL_VERTEX_ARRAY);

		// Free the vertex array memory

		// Deselect the current rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);

		// Delete the palette if it was created
		if (hPalette != NULL)
			DeleteObject(hPalette);

		// Tell the application to terminate after the window
		// is gone.
		PostQuitMessage(0);
		break;

		// Window is resized.
	case WM_SIZE:
		// Call our function which modifies the clipping
		// volume and viewport
		ChangeSize(LOWORD(lParam), HIWORD(lParam));
		break;


		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
	case WM_PAINT:
	{
		// Call OpenGL drawing code
		RenderScene();

		SwapBuffers(hDC);

		// Validate the newly painted client area
		ValidateRect(hWnd, NULL);
	}
	break;

	// Windows is telling the application that it may modify
	// the system palette.  This message in essance asks the 
	// application for a new palette.
	case WM_QUERYNEWPALETTE:
		// If the palette was created.
		if (hPalette)
		{
			int nRet;

			// Selects the palette into the current device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries from the currently selected palette to
			// the system palette.  The return value is the number 
			// of palette entries modified.
			nRet = RealizePalette(hDC);

			// Repaint, forces remap of palette in current window
			InvalidateRect(hWnd, NULL, FALSE);

			return nRet;
		}
		break;


		// This window may set the palette, even though it is not the 
		// currently active window.
	case WM_PALETTECHANGED:
		// Don't do anything if the palette does not exist, or if
		// this is the window that changed the palette.
		if ((hPalette != NULL) && ((HWND)wParam != hWnd))
		{
			// Select the palette into the device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries to system palette
			RealizePalette(hDC);

			// Remap the current colors to the newly realized palette
			UpdateColors(hDC);
			return 0;
		}
		break;

		// Key press, check for arrow keys to do cube rotation.

	case WM_KEYDOWN:
	{
		keyboard.Add(wParam);
	}
	break;
	case WM_KEYUP:
	{
		keyboard.Remove(wParam);
	}
	break;

	// A menu command
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			// Exit the program
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;

			// Display the about box
		}
	}
	break;


	default:   // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));

	}

	return (0L);
}

