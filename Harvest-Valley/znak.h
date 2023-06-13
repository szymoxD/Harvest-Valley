#pragma once
#include "glm\glm.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H 
class znak
{
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    FT_Pos advance;

public:
    znak(unsigned int textureID_v, glm::ivec2 size_v, glm::ivec2 bearing_v, FT_Pos advance_v);
    unsigned int getTextureId();
    glm::ivec2 getSize();
};

