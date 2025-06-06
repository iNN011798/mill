#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "learnopengl/shader_m.h"

// 保存了FreeType加载的字形的所有状态信息
struct Character {
    unsigned int TextureID; // 字形纹理的ID
    glm::ivec2   Size;      // 字形大小
    glm::ivec2   Bearing;   // 从基线到字形左侧/顶部的偏移量
    unsigned int Advance;   // 到下一个字形的水平偏移量
};

// TextRenderer类，用于封装所有渲染操作
class TextRenderer
{
public:
    TextRenderer(unsigned int screenWidth, unsigned int screenHeight);
    ~TextRenderer();

    // 从文件加载字体
    bool Load(std::string fontPath, unsigned int fontSize);

    // 渲染文本字符串
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
    // 渲染状态
    Shader shader;
    unsigned int VAO, VBO;
    std::map<GLchar, Character> Characters;
};

#endif 