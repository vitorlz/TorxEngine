#pragma once
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>
#include "../Util/Shader.h"

struct Character
{
	unsigned int textureID;  // ID handle of the glyph texture
	glm::ivec2   size;       // Size of glyph
	glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
	unsigned int advance;    // Offset to advance to next glyph
};

class TextRendering
{
public:
	static int Init();
	static unsigned int m_VAO;
	static unsigned int m_VBO;
	int LoadFont(const std::string& path, int fontSize);
	void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);

	std::map<char, Character> Characters;
private:
	void CreateBuffers();
	static FT_Library ft;
	FT_Face m_Face;

};