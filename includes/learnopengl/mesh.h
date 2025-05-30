#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

// 顶点数据包含：位置坐标、法线、纹理坐标、切线、
struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    // 用于法线贴图
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // 用于骨骼动画
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;    // 纹理类型
    string path;
};

class Mesh {
public:
    // mesh Data：顶点、索引、纹理
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader &shader) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        //for(unsigned int i = 0; i < textures.size(); i++)
        //{
        //    // 激活并绑定所有贴图，传递给着色器的对应采样器（sampler2D）
        //    glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        //    // retrieve texture number (the N in diffuse_textureN)
        //    string number;
        //    string name = textures[i].type;
        //    if(name == "texture_diffuse")
        //        number = std::to_string(diffuseNr++);
        //    else if(name == "texture_specular")
        //        number = std::to_string(specularNr++); // transfer unsigned int to string
        //    else if(name == "texture_normal")
        //        number = std::to_string(normalNr++); // transfer unsigned int to string
        //     else if(name == "texture_height")
        //        number = std::to_string(heightNr++); // transfer unsigned int to string

        //    // 将纹理单元索引传递给着色器的uniform采样器
        //    // 着色器中的texture_diffuse1会从GL_TEXTUREi单元采样
        //    // now set the sampler to the correct texture unit
        //    glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        //    // 绑定纹理对象到当前激活的纹理单元
        //    // and finally bind the texture
        //    glBindTexture(GL_TEXTURE_2D, textures[i].id);
        //}
        if (textures.empty())
        {
            shader.setBool("u_hasDiffuseTexture", false);
            // 你可能需要先在 shader_m.h 中为 Shader 类添加 setVec3(const std::string&, float, float, float) 方法
            shader.setVec3("u_defaultColor", 0.5f, 0.5f, 0.5f); // 默认为灰色
        }
        else
        {
            shader.setBool("u_hasDiffuseTexture", true);
            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                // retrieve texture number (the N in diffuse_textureN)
                string number;
                string name = textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++); // transfer unsigned int to string
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++); // transfer unsigned int to string
                else if (name == "texture_height")
                    number = std::to_string(heightNr++); // transfer unsigned int to string

                // now set the sampler to the correct texture unit
                shader.setInt((name + number).c_str(), i); // 使用 shader.setInt 设置 uniform
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
        }
        
        // 绘制网格几何体，绑定VAO，使用索引绘制三角形网格
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 恢复状态
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    // New method to update the VBO with modified vertex data
    void updateVertexBuffer() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays：创建VAO、VBO、EBO的ID
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO); // 绑定VAO
        // load data into vertex buffers：绑定VBO，开辟显存，加载顶点数据
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Change to GL_DYNAMIC_DRAW for frequent updates
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);  

        // 绑定EBO，开辟显存，加载索引
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // 告诉显卡如何解析加载到显存的连续数据，将VBO中的数据解释为各个顶点属性（位置、法线、纹理坐标）
        // 第一个参数和顶点着色器程序的layout (location = x) 的属性一一对应
        // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // 在CPU端绑定了，但是GPU不会读取到这些数据，浪费了绑定空间，设置无效
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids：这里设置的属性是整数类型的，不是float
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        // 解绑定VAO
        glBindVertexArray(0);
    }
};
#endif
