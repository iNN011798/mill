#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform bool u_hasDiffuseTexture;
uniform vec3 u_defaultColor;
void main()
{    
    if (u_hasDiffuseTexture)
    {
        FragColor = texture(texture_diffuse1, TexCoords);
    }
    else
    {
        FragColor = vec4(u_defaultColor, 1.0);
    }
}