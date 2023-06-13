#include "znak.h"

znak::znak(unsigned int textureID_v, glm::ivec2 size_v, glm::ivec2 bearing_v, FT_Pos advance_v) {
	textureID = textureID_v;
	size = size_v;
	bearing = bearing_v;
	advance = advance_v;
}

unsigned int znak::getTextureId() {
	return textureID;
}

glm::ivec2 znak::getSize() {
	return size;
}