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

// �������ݰ�����λ�����ꡢ���ߡ��������ꡢ���ߡ�
struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    // ���ڷ�����ͼ
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // ���ڹ�������
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;    // ��������
    string path;
};

class Mesh {
public:
    // mesh Data�����㡢����������
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
        //    // �����������ͼ�����ݸ���ɫ���Ķ�Ӧ��������sampler2D��
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

        //    // ������Ԫ�������ݸ���ɫ����uniform������
        //    // ��ɫ���е�texture_diffuse1���GL_TEXTUREi��Ԫ����
        //    // now set the sampler to the correct texture unit
        //    glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        //    // ��������󵽵�ǰ���������Ԫ
        //    // and finally bind the texture
        //    glBindTexture(GL_TEXTURE_2D, textures[i].id);
        //}
        if (textures.empty())
        {
            shader.setBool("u_hasDiffuseTexture", false);
            // �������Ҫ���� shader_m.h ��Ϊ Shader ����� setVec3(const std::string&, float, float, float) ����
            shader.setVec3("u_defaultColor", 0.5f, 0.5f, 0.5f); // Ĭ��Ϊ��ɫ
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
                shader.setInt((name + number).c_str(), i); // ʹ�� shader.setInt ���� uniform
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
        }
        
        // �������񼸺��壬��VAO��ʹ��������������������
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // �ָ�״̬
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
        // create buffers/arrays������VAO��VBO��EBO��ID
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO); // ��VAO
        // load data into vertex buffers����VBO�������Դ棬���ض�������
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Change to GL_DYNAMIC_DRAW for frequent updates
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);  

        // ��EBO�������Դ棬��������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // �����Կ���ν������ص��Դ���������ݣ���VBO�е����ݽ���Ϊ�����������ԣ�λ�á����ߡ��������꣩
        // ��һ�������Ͷ�����ɫ�������layout (location = x) ������һһ��Ӧ
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
        // ��CPU�˰��ˣ�����GPU�����ȡ����Щ���ݣ��˷��˰󶨿ռ䣬������Ч
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids���������õ��������������͵ģ�����float
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        // ���VAO
        glBindVertexArray(0);
    }
};
#endif
