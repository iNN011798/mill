#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "learnopengl/shader_m.h"

// ������FreeType���ص����ε�����״̬��Ϣ
struct Character {
    unsigned int TextureID; // ���������ID
    glm::ivec2   Size;      // ���δ�С
    glm::ivec2   Bearing;   // �ӻ��ߵ��������/������ƫ����
    unsigned int Advance;   // ����һ�����ε�ˮƽƫ����
};

// TextRenderer�࣬���ڷ�װ������Ⱦ����
class TextRenderer
{
public:
    TextRenderer(unsigned int screenWidth, unsigned int screenHeight);
    ~TextRenderer();

    // ���ļ���������
    bool Load(std::string fontPath, unsigned int fontSize);

    // ��Ⱦ�ı��ַ���
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
    // ��Ⱦ״̬
    Shader shader;
    unsigned int VAO, VBO;
    std::map<GLchar, Character> Characters;
};

#endif 