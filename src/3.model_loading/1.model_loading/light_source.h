#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

class LightSource
{
public:
    glm::vec3 Position;
    glm::vec3 Color;

    // ��������
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;

    LightSource(glm::vec3 position, glm::vec3 color = glm::vec3(1.0f));

    // ���ƹ�����Ӧ�õ���������ɫ��
    void SetLightUniforms(Shader& shader, const std::string& uniformName = "light");

    // ��Ⱦ��Դ�Ŀ��ӻ���ʾ��һ�������壩
    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int VBO, VAO;
    // ��ʼ�����������������
    void setupMesh();
};

#endif 