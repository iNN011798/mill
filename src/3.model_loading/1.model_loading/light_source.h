#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

class LightSource
{
public:
    glm::vec3 Position;
    glm::vec3 Color;

    // 光照属性
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;

    LightSource(glm::vec3 position, glm::vec3 color = glm::vec3(1.0f));

    // 将灯光属性应用到给定的着色器
    void SetLightUniforms(Shader& shader, const std::string& uniformName = "light");

    // 渲染光源的可视化表示（一个立方体）
    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int VBO, VAO;
    // 初始化立方体的网格数据
    void setupMesh();
};

#endif 