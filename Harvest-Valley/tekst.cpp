#include "tekst.h"
#include "glm\glm.hpp"
#include <gl\glu.h>
tekst::tekst(const char* value_v, float posx_v, float posy_v) {
	value = value_v;
	posx = posx_v;
	posy = posy_v;
}

void tekst::draw(std::map<char, znak> znaki) {
	glColor3f(1,1,1);
	float offx = 0;
	float sizx = 0, sizy = 0;
	for (int i = 0; i < value.length(); i++) {
		unsigned int val=0;
		glm::ivec2 size;
		if (value[i] == ' ') {
			offx += sizx + 0.002;
			continue;
		}
		for (auto keyValue : znaki) {
			char key = keyValue.first;
			if (key == value[i]) {
				val = keyValue.second.getTextureId();
				size = keyValue.second.getSize();
			}
		}
		sizx = (float)size.x / 500.f;
		sizy = (float)size.y / 300.f;
		GLuint textureID = val;
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex2f(posx+offx,posy-sizy);
		glTexCoord2f(1, 0);
		glVertex2f(posx+offx+sizx, posy-sizy);
		glTexCoord2f(1, 1);
		glVertex2f(posx+offx+sizx, posy);
		glTexCoord2f(0, 1);
		glVertex2f(posx+offx,posy);
		glEnd();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
		offx += sizx+0.002;
	}
}
void tekst::drawRect(float posx_v, float posy_v, float posx_v2, float posy_v2) {
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(posx_v, posy_v);
	glVertex2f(posx_v2, posy_v);
	glVertex2f(posx_v2, posy_v2);
	glVertex2f(posx_v, posy_v2);
	glEnd();
}


void tekst::move(float posx_v, float posy_v) {
	posx = posx_v;
	posy = posy_v;
}
void tekst::setValue(std::string value_v) {
	value = value_v;
}