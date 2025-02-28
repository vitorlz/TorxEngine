#pragma once
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>
#include "../Util/Shader.h"
#include <vector>

struct Character
{
	int textureID;  // index is ASCII character
	glm::ivec2   size;       // Size of glyph
	glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
	unsigned int advance;    // Offset to advance to next glyph
};

class TextRendering
{
public:
	TextRendering();

	TextRendering(const std::string& path);
	TextRendering(const std::string& path, bool isIcon);

	static int Init();
	
	void RenderText(Shader& s, std::string text, float x, float y, float scale, float lineSpace, glm::vec3 color);

	void RenderIcon(Shader& s, wchar_t unicode, float x, float y, float scale, glm::vec3 worldPos, glm::vec3 color);
	
	static FT_Library m_ft;

private:
	static unsigned int m_vao;
	static unsigned int m_vbo;

	void CreateBuffers();
	void TextRenderCall(int length, unsigned int shaderID);
	
	FT_Face m_face;
	std::vector<glm::mat4> m_transforms;
	std::vector<int> m_letterMap;
	std::map<char, Character> m_characters;
	std::map<wchar_t, Character> m_icons;
	unsigned int m_textureArray;
};