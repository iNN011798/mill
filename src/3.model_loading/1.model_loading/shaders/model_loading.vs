#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 7) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // 使用法线矩阵来变换法线，避免模型发生非统一缩放时法线方向错误
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    TexCoords = aTexCoords;
    VertexColor = aColor;
    gl_Position = projection * view * vec4(FragPos, 1.0);
} 