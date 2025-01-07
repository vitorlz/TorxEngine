#include "TextRendering.h"
#include <glad/glad.h>

FT_Library TextRendering::m_ft;
unsigned int TextRendering::m_vao;
unsigned int TextRendering::m_vbo;

const unsigned int ARRAY_LIMIT = 200;
const unsigned int QUAD_SIZE = 128;

float vertex_data[] =
{
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
};

TextRendering::TextRendering() {};

int TextRendering::Init()
{
    if (FT_Init_FreeType(&m_ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


TextRendering::TextRendering(const std::string& path)
{
    if (FT_New_Face(m_ft, path.c_str(), 0, &m_face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(m_face, QUAD_SIZE, QUAD_SIZE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    glGenTextures(1, &m_textureArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, QUAD_SIZE, QUAD_SIZE, 128, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    FT_GlyphSlot slot = m_face->glyph;

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0, 0, 0, int(c),
            m_face->glyph->bitmap.width,
            m_face->glyph->bitmap.rows, 1,
            GL_RED,
            GL_UNSIGNED_BYTE,
            m_face->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            int(c),
            glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
            glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
            m_face->glyph->advance.x
        };
        m_characters.insert(std::pair<char, Character>(c, character));
    }

    CreateBuffers();

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    FT_Done_Face(m_face);
}

void TextRendering::CreateBuffers()
{
    for (int i = 0; i < ARRAY_LIMIT; i++)
    {
        m_letterMap.push_back(0);
        m_transforms.push_back(glm::mat4(1.0f));
    }
}

void TextRendering::RenderText(Shader& s, std::string text, float x, float y, float scale, float lineSpace, glm::vec3 color)
{
    // activate corresponding render state	
    scale = scale / float(QUAD_SIZE);
    float copyX = x;
    s.use();
    s.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    int workingIndex = 0;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = m_characters[*c];

        if (*c == '\n')
        {
            y -= QUAD_SIZE * lineSpace * scale;
            x = copyX;
        }
        else if (*c == ' ')
        {
            x += (ch.advance >> 6) * scale;
        }
        else
        {
            float xpos = x + ch.bearing.x * scale;
            float ypos = y - (QUAD_SIZE - ch.bearing.y) * scale;

            m_transforms[workingIndex] = glm::translate(glm::mat4(1.0f), glm::vec3(xpos, ypos, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(QUAD_SIZE * scale, QUAD_SIZE * scale, 0));
            m_letterMap[workingIndex] = ch.textureID;
    
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            workingIndex++;

            if (workingIndex == ARRAY_LIMIT - 1)
            {
                TextRenderCall(workingIndex, s.ID);
                workingIndex = 0;
            }
        }
    }

    TextRenderCall(workingIndex, s.ID);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRendering::TextRenderCall(int length, unsigned int shaderID)
{
    if (length != 0)
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), length, GL_FALSE, &m_transforms[0][0][0]);
        glUniform1iv(glGetUniformLocation(shaderID, "letterMap"), length, &m_letterMap[0]);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, length);
    }
}




